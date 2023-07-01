#include <libultraship/libultraship.h>

#include "chaos_commands.h"
#include "chaos_commands_macros.h"
#include "chaos_utils.h"

#include <z64.h>
#include <macros.h>
#include <variables.h>
#undef Polygon

#include "../OTRGlobals.h"
#include "debugconsole.h"
#include "soh/Enhancements/game-interactor/GameInteractor.h"
#include "soh/Enhancements/audio/AudioEditor.h"
#include "soh/Enhancements/cosmetics/CosmeticsEditor.h"

#include <algorithm>
#include <functional>
#include <map>
#include <vector>
#include <queue>
#include <ctime>
#include <string>

#if defined(__linux__)
#include "chaos_linux.h"
#elif !defined(__APPLE__) && !defined(__SWITCH__) && !defined(__WIIU__)
#include "chaos_win.h"
#else
#include "chaos_dummy.h"
#endif

extern "C" {
void Chaos_ResetAll();
}

bool g_link_is_ready_this_frame = true;
int32_t g_gravity_modifier = 0;
uint32_t g_climb_speed_modifier = 0;
uint32_t g_hookshot_length_modifier = 0;

template<typename T>
T Read(const std::vector<uint8_t>& bytes, size_t start_index) {
	return *((T*)(bytes.data() + start_index));
}

struct CommandCreator {
	std::function<std::vector<uint8_t>(const std::vector<uint8_t>&)> read_payload_;
	std::function<std::unique_ptr<ChaosCommand>(const std::vector<uint8_t>&)> create_;
};

uint8_t CMD_ID = 0x11;
static std::map<uint8_t, CommandCreator> kCommands {
	// CMD_ONE_SHOT(0x00, PL_NONE(), { push_pending_ice_trap(); }),
	CMD_ONE_SHOT_INTERACTOR(0x00, new GameInteractionEffect::FreezePlayer(), false, nullptr),

	CMD(0x01, PL_NONE(), 
		CR_PRED(
			[&]() { return g_link_is_ready_this_frame; },
			CR_ONE_SHOT({ void_out(); }))),

	// Don't change age until there are no force-equipped boots so that we don't got to adult,
	// immediately try to equip F boots, and crash
	CMD(0x02, PL_NONE(), 
		CR_PRED(
			[&]() { return g_link_is_ready_this_frame && !CVarGetInteger("gChaosForcedBoots", 0); },
			CR_ONE_SHOT({ toggle_age(); }))),

	CMD_ONE_SHOT(0x03, PL_NONE(), { gSaveContext.health = 0; }),
	// CMD_ONE_SHOT(0x04, PL_NONE(), { scale(&(GET_PLAYER(gPlayState)->actor), 1.5f, 1.5f, 1.5f); }),
	CMD_ONE_SHOT(0x04, PL_NONE(), { GameInteractor::ChaosState::CustomLinkScale *= 1.5f; }),
	// CMD_ONE_SHOT_INTERACTOR(0x04, new GameInteractionEffect::ModifyLinkScale(), true,
	// 	[=](GameInteractionEffectBase* effect) {
	// 		effect->parameters[0] = 1.5f;
	// 	}),
	// CMD_ONE_SHOT(0x05, PL_NONE(), { scale(&(GET_PLAYER(gPlayState)->actor), 0.66f, 0.66f, 0.66f); }),
	CMD_ONE_SHOT(0x05, PL_NONE(), { GameInteractor::ChaosState::CustomLinkScale *= 0.66f; }),
	// CMD_ONE_SHOT_INTERACTOR(0x05, new GameInteractionEffect::ModifyLinkScale(), true,
	// 	[=](GameInteractionEffectBase* effect) {
	// 		effect->parameters[0] = 0.66;
	// 	}),

	// CMD_TIMED_BOOL_CVAR(0x06, "gChaosOHKO"),
	CMD_TIMED_INTERACTOR(0x06, new GameInteractionEffect::OneHitKO(), false, nullptr),
	// CMD_TIMED_BOOL_CVAR(0x07, "gChaosNoHud"),
	CMD_TIMED_INTERACTOR(0x07, new GameInteractionEffect::NoUI(), false, nullptr),
	// CMD_TIMED_BOOL_CVAR(0x08, "gChaosNoZ"),
	CMD_TIMED_INTERACTOR(0x08, new GameInteractionEffect::DisableZTargeting(), false, nullptr),
	CMD_TIMED_BOOL_CVAR(0x09, "gChaosTurbo"),
	// CMD_TIMED_BOOL_CVAR(0x0A, "gChaosInvertControls"),
	CMD_TIMED_INTERACTOR(0x0A, new GameInteractionEffect::ReverseControls(), false, nullptr),

	CMD(0x0B, PL_NONE(), 
		CR_PRED(
			[&]() { return g_link_is_ready_this_frame; },
			CR_ONE_SHOT({ spawn_on_link(0x013B, 0x0001); }))),

	CMD(0x0C, PL_BYTES(2 * sizeof(int16_t) + sizeof(int32_t)), 
		CR_PRED(
			[&]() { return g_link_is_ready_this_frame; },
			CR_ONE_SHOT({
				int32_t count = Read<int32_t>(payload, 0);
				spawn_n(
					Read<int16_t>(payload, sizeof(int32_t)),
					Read<int16_t>(payload, sizeof(int16_t) + sizeof(int32_t)),
					count); 
	}))),

	// CMD_ONE_SHOT(0x0D, PL_BYTES(sizeof(uint32_t)), { 
	// 	gSaveContext.health = s_add(gSaveContext.health, Read<uint32_t>(payload, 0), gSaveContext.healthCapacity);
	// }),
	// CMD_ONE_SHOT(0x0E, PL_BYTES(sizeof(uint32_t)), { 
	// 	gSaveContext.health = s_sub(gSaveContext.health, Read<uint32_t>(payload, 0), 16);
	// }),
	CMD_ONE_SHOT_INTERACTOR(0x0D, new GameInteractionEffect::ModifyHealth(), true,
		[=](GameInteractionEffectBase* effect) {
			effect->parameters[0] = Read<uint32_t>(payload, 0);
		}),
	CMD_ONE_SHOT_INTERACTOR(0x0E, new GameInteractionEffect::ModifyHealth(), true,
		[=](GameInteractionEffectBase* effect) {
			int healthToRemove = Read<uint32_t>(payload, 0);
			if (gSaveContext.health - (healthToRemove * 16) <= 0) {
				healthToRemove = ceil((gSaveContext.health / 16) - 1);
			}
			effect->parameters[0] = -healthToRemove;
		}),

	// CMD_ONE_SHOT(0x0F, PL_BYTES(sizeof(uint32_t)), { 
	// 	gSaveContext.rupees = s_add(gSaveContext.rupees, Read<uint32_t>(payload, 0), CUR_CAPACITY(UPG_WALLET));
	// }),
	// CMD_ONE_SHOT(0x10, PL_BYTES(sizeof(uint32_t)), { 
	// 	gSaveContext.rupees = s_sub(gSaveContext.rupees, Read<uint32_t>(payload, 0), 16);
	// }),
	CMD_ONE_SHOT_INTERACTOR(0x0F, new GameInteractionEffect::ModifyRupees(), true,
		[=](GameInteractionEffectBase* effect) {
			effect->parameters[0] = Read<uint32_t>(payload, 0);
		}),
	CMD_ONE_SHOT_INTERACTOR(0x10, new GameInteractionEffect::ModifyRupees(), true,
		[=](GameInteractionEffectBase* effect) {
			effect->parameters[0] = -Read<uint32_t>(payload, 0);
		}),

	CMD_TIMED_BOOL_CVAR(CMD_ID++, "gDisableFPSView"),
	CMD_TIMED_BOOL_CVAR(CMD_ID++, "gForceNormalArrows"),
	// CMD_TIMED_BOOL_CVAR(CMD_ID++, "gDisableLedges"),
	CMD_TIMED_INTERACTOR(CMD_ID++, new GameInteractionEffect::DisableLedgeGrabs(), false, nullptr),
	CMD_TIMED_BOOL_CVAR(CMD_ID++, "gFloorIsLava"),
	CMD_TIMED_BOOL_CVAR(CMD_ID++, "gExplodingRolls"),
	CMD_TIMED_BOOL_CVAR(CMD_ID++, "gFreezingRolls"),
	CMD_TIMED_BOOL_CVAR(CMD_ID++, "gDisableTargeting"),
	CMD_TIMED_BOOL_CVAR(CMD_ID++, "gMegaLetterbox"),
	CMD_TIMED_BOOL_CVAR(CMD_ID++, "gDisableTurning"),
	CMD_TIMED_BOOL_CVAR(CMD_ID++, "gJailTime"),
	CMD_TIMED_BOOL_CVAR(CMD_ID++, "gOnHold"),
	CMD_TIMED_BOOL_CVAR(CMD_ID++, "gSonicRoll"),
	CMD_TIMED_BOOL_CVAR(CMD_ID++, "gNaviSpam"),
	CMD_TIMED_BOOL_CVAR(CMD_ID++, "gScuffedLink"),
	CMD_TIMED_BOOL_CVAR(CMD_ID++, "gRaveMode"),
	// CMD_TIMED_BOOL_CVAR(CMD_ID++, "gInvisPlayer"),
	CMD_TIMED_INTERACTOR(CMD_ID++, new GameInteractionEffect::InvisibleLink(), false, nullptr),
	CMD_TIMED_BOOL_CVAR(CMD_ID++, "gSlipperyFloor"),
	// CMD_TIMED_INTERACTOR(CMD_ID++, new GameInteractionEffect::SlipperyFloor(), false, nullptr),
	CMD_TIMED_BOOL_CVAR(CMD_ID++, "gIceDamage"),
	CMD_TIMED_BOOL_CVAR(CMD_ID++, "gElectricDamage"),
	CMD_TIMED_BOOL_CVAR(CMD_ID++, "gKnockbackDamage"),
	CMD_TIMED_BOOL_CVAR(CMD_ID++, "gFireDamage"),
	CMD_TIMED_BOOL_CVAR(CMD_ID++, "gForwardJump"),
	CMD_TIMED_BOOL_CVAR(CMD_ID++, "gBigHead"),
	CMD_TIMED_BOOL_CVAR(CMD_ID++, "gTinyHead"),
	CMD_TIMED_BOOL_CVAR(CMD_ID++, "gDarkenArea"),
	CMD_TIMED_BOOL_CVAR(CMD_ID++, "gChaosSpin"),
	CMD_TIMED_BOOL_CVAR(CMD_ID++, "gDisableMeleeAttacks"),
	CMD_TIMED_BOOL_CVAR(CMD_ID++, "gDisableEnemyDraw"),
	CMD_TIMED_BOOL_CVAR(CMD_ID++, "gSandstorm"),
	CMD_TIMED_BOOL_CVAR(CMD_ID++, "gSinkingFloor"),
	CMD_TIMED_BOOL_CVAR(CMD_ID++, "gCowRitual"),
	CMD_TIMED_BOOL_CVAR(CMD_ID++, "gFireRockRain"),
	CMD_TIMED_BOOL_CVAR(CMD_ID++, "gCuccoAttack"),
    CMD_ONE_SHOT_CVAR(CMD_ID++, "gExplodingRupeeChallenge"),
	CMD_TIMED_BOOL_CVAR(CMD_ID++, "gBanItemDropPickup"),
	CMD_TIMED_BOOL_CVAR(CMD_ID++, "gBrokenBombchus"),
	CMD_TIMED_BOOL_CVAR(CMD_ID++, "gAnnoyingGetItems"),

	// Gravity (- down, + up)
    CMD(CMD_ID++, PL_BYTES(sizeof(uint32_t)),
			CR_ONE_SHOT_TIMED(
                [&]() { g_gravity_modifier += 3; },
                [&]() { g_gravity_modifier -= 3; })),
    CMD(CMD_ID++, PL_BYTES(sizeof(uint32_t)),
			CR_ONE_SHOT_TIMED(
                [&]() { g_gravity_modifier -= 4; },
                [&]() { g_gravity_modifier += 4; })),

	// Climb speed
    CMD(CMD_ID++, PL_BYTES(sizeof(uint32_t)),
			CR_ONE_SHOT_TIMED(
                [&]() { g_climb_speed_modifier += 3; },
                [&]() { g_climb_speed_modifier -= 3; })),

	// HS Length 
    CMD(CMD_ID++, PL_BYTES(sizeof(uint32_t)),
			CR_ONE_SHOT_TIMED(
                [&]() { g_hookshot_length_modifier += 3; },
                [&]() { g_hookshot_length_modifier -= 3; })),

	CMD_ONE_SHOT_CVAR(CMD_ID++, "gSpawnExplosion"),
	CMD_ONE_SHOT_CVAR(CMD_ID++, "gRestrainLink"),
	CMD_ONE_SHOT_CVAR(CMD_ID++, "gTripToSpace"),
	CMD_ONE_SHOT_CVAR(CMD_ID++, "gRedoRando"),

	CMD_TIMED_BOOL_CVAR(CMD_ID++, "gAnnoyingText"),

	// Paper link
	CMD_TIMED_INTERACTOR(CMD_ID++, new GameInteractionEffect::ModifyLinkSize(), false,
		[&](GameInteractionEffectBase* effect) { effect->parameters[0] = GI_LINK_SIZE_PAPER; }),

	// CMD(CMD_ID++, PL_BYTES(sizeof(uint32_t)),
	// 	CR_ONE_SHOT_TIMED(
	// 		[&]() {
	// 			GameInteractionEffectBase* effect = new GameInteractionEffect::ModifyLinkSize();
	// 			effect->parameters[0] = GI_LINK_SIZE_PAPER;
	// 			GameInteractor::ApplyEffect(effect);
	// 		},
	// 		[&]() {
	// 			GameInteractionEffectBase* effect = new GameInteractionEffect::ModifyLinkSize();
	// 			effect->parameters[0] = GI_LINK_SIZE_PAPER;
	// 			GameInteractor::RemoveEffect(effect);
	// 		})),

	CMD_TIMED_BOOL_CVAR(CMD_ID++, "gThiccLink"),
	CMD_TIMED_BOOL_CVAR(CMD_ID++, "gFlipLink"),
	CMD_TIMED_BOOL_CVAR(CMD_ID++, "gNoStart"),
	CMD_ONE_SHOT_CVAR(CMD_ID++, "gBackToHome"),
	CMD_TIMED_BOOL_CVAR(CMD_ID++, "gFlashbang"),
	CMD_ONE_SHOT(CMD_ID++, PL_BYTES(sizeof(uint32_t)), { AudioEditor_RandomizeAll(); }),
	CMD_ONE_SHOT(CMD_ID++, PL_BYTES(sizeof(uint32_t)), { CosmeticsEditor_RandomizeAll(); }),
	CMD_TIMED_BOOL_CVAR(CMD_ID++, "gPogoStick"),
	CMD_ONE_SHOT_CVAR(CMD_ID++, "gSunsSong"),
	// CMD_ONE_SHOT_INTERACTOR(CMD_ID++, new GameInteractionEffect::SetTimeOfDay(), false,
	// 	[=](GameInteractionEffectBase* effect) {
	// 		effect->parameters[0] = IS_DAY ? 0 : 0x8000;
	// 	}),
	// CMD_ONE_SHOT_CVAR(CMD_ID++, "gPressA"),
	CMD_ONE_SHOT_INTERACTOR(CMD_ID++, new GameInteractionEffect::PressButton(), false,
		[=](GameInteractionEffectBase* effect) {
			effect->parameters[0] = BTN_A;
		}),
	CMD_TIMED_BOOL_CVAR(CMD_ID++, "gButtonSwap"),
	CMD_ONE_SHOT_CVAR(CMD_ID++, "gRandoMagic"),
	CMD_TIMED_BOOL_CVAR(CMD_ID++, "gNoStrength"),
	CMD_TIMED_BOOL_CVAR(CMD_ID++, "gNoWater"),
	CMD_ONE_SHOT_CVAR(CMD_ID++, "gForceUnequip"),
	CMD_ONE_SHOT_CVAR(CMD_ID++, "gShuffleItems"),
	CMD_TIMED_BOOL_CVAR(CMD_ID++, "gMoonwalk"),
	CMD_TIMED_BOOL_CVAR(CMD_ID++, "gDisableLedgeJump"),
	CMD_TIMED_BOOL_CVAR(CMD_ID++, "gChaosRedeem"),

	// CMD_TAKE_AMMO(0x80, ITEM_BOMBCHU),
	// CMD_TAKE_AMMO(0x81, ITEM_STICK),
	// CMD_TAKE_AMMO(0x82, ITEM_NUT),
	// CMD_TAKE_AMMO(0x83, ITEM_BOMB),
	// CMD_TAKE_AMMO(0x84, ITEM_BOW),
	// CMD_TAKE_AMMO(0x85, ITEM_SLINGSHOT),
	CMD_TAKE_AMMO_INTERACTOR(0x80, ITEM_BOMBCHU),
	CMD_TAKE_AMMO_INTERACTOR(0x81, ITEM_STICK),
	CMD_TAKE_AMMO_INTERACTOR(0x82, ITEM_NUT),
	CMD_TAKE_AMMO_INTERACTOR(0x83, ITEM_BOMB),
	CMD_TAKE_AMMO_INTERACTOR(0x84, ITEM_BOW),
	CMD_TAKE_AMMO_INTERACTOR(0x85, ITEM_SLINGSHOT),

	// You can carry 50 chus if you have bomb bag when chus are not in logic, or have found chus previously with chus in logic
	// CMD_ONE_SHOT(0xC0, PL_BYTES(sizeof(uint32_t)), {
	// 	uint32_t amt = Read<uint32_t>(payload, 0);
	// 	size_t cap = 50;
	// 	bool bombchusInLogic = OTRGlobals::Instance->gRandomizer->GetRandoSettingValue(RSK_BOMBCHUS_IN_LOGIC);
	// 	if ((bombchusInLogic && INV_CONTENT(ITEM_BOMBCHU) == ITEM_NONE) ||
	// 		(!bombchusInLogic && CUR_CAPACITY(UPG_BOMB_BAG) == 0)) {
	// 		cap = 0;
	// 	}
	// 	AMMO(ITEM_BOMBCHU) = s_add(AMMO(ITEM_BOMBCHU), amt, cap);
	// }),
	// CMD_GIVE_AMMO(0xC1, ITEM_STICK, UPG_STICKS),
	// CMD_GIVE_AMMO(0xC2, ITEM_NUT, UPG_NUTS),
	// CMD_GIVE_AMMO(0xC3, ITEM_BOMB, UPG_BOMB_BAG),
	// CMD_GIVE_AMMO(0xC4, ITEM_BOW, UPG_QUIVER),
	// CMD_GIVE_AMMO(0xC5, ITEM_SLINGSHOT, UPG_BULLET_BAG),
	CMD_GIVE_AMMO_INTERACTOR(0xC0, ITEM_BOMBCHU),
	CMD_GIVE_AMMO_INTERACTOR(0xC1, ITEM_STICK),
	CMD_GIVE_AMMO_INTERACTOR(0xC2, ITEM_NUT),
	CMD_GIVE_AMMO_INTERACTOR(0xC3, ITEM_BOMB),
	CMD_GIVE_AMMO_INTERACTOR(0xC4, ITEM_BOW),
	CMD_GIVE_AMMO_INTERACTOR(0xC5, ITEM_SLINGSHOT),

	CMD(0xE2, PL_BYTES(sizeof(uint32_t)),
		CR_PRED(
			[]() { return LINK_IS_ADULT && !CVarGetInteger("gChaosForcedBoots", 0); },
			CR_TIMED_CVAR("gChaosForcedBoots", 0, 2))),
	CMD(0xE3, PL_BYTES(sizeof(uint32_t)),
		CR_PRED(
			[]() { return LINK_IS_ADULT && !CVarGetInteger("gChaosForcedBoots", 0); },
			CR_TIMED_CVAR("gChaosForcedBoots", 0, 3))),
	CMD(0xEF, PL_BYTES(sizeof(uint32_t)),
		CR_PRED(
			[]() { return LINK_IS_CHILD && !CVarGetInteger("gChaosForcedBoots", 0); },
			CR_TIMED_CVAR("gChaosForcedBoots", 0, 0xF))),
	// CMD_TIMED_INTERACTOR(0xE2, new GameInteractionEffect::ForceEquipBoots(), false,
	// 	[=](GameInteractionEffectBase* effect) {
	// 		effect->parameters[0] = PLAYER_BOOTS_IRON;
	// 	}),
	// CMD_TIMED_INTERACTOR(0xE3, new GameInteractionEffect::ForceEquipBoots(), false,
	// 	[=](GameInteractionEffectBase* effect) {
	// 		effect->parameters[0] = PLAYER_BOOTS_HOVER;
	// 	}),
	// CMD_TIMED_INTERACTOR(0xEF, new GameInteractionEffect::ForceEquipBoots(), false,
	// 	[=](GameInteractionEffectBase* effect) {
	// 		effect->parameters[0] = 0xD; // F boots
	// 	}),
};

static bool g_is_enabled = false;
static CommandStorage g_command_storage;

void Start() {
    PlatformStart();
    LUS::Context::GetInstance()->GetWindow()->GetGui()->GetGameOverlay()->TextDrawNotification(10.0f, true, "Chaos Mode Enabled");
    CVarSetInteger("gEnemyHealthBar", 1);
}

void DisplayCommandMessage(const std::vector<uint8_t>& bytes, size_t start_index) {
	std::string msg(bytes.begin() + start_index, bytes.end());
    LUS::Context::GetInstance()->GetWindow()->GetGui()->GetGameOverlay()->TextDrawNotification(15.0f, true, msg.c_str());
}

void EnqueueCommand(const std::vector<uint8_t>& bytes) {
	size_t message_start_index = 1; // Account for the command idx itself

	auto it = kCommands.find(bytes[0]);
	if (it == kCommands.end()) {
		LUS::Context::GetInstance()->GetWindow()->GetGui()->GetGameOverlay()->TextDrawNotification(10.0f, true, "Unrecognized command");
		return;
	}

	auto payload = it->second.read_payload_(bytes);
	g_command_storage.AddCommand(it->second.create_(payload));

	DisplayCommandMessage(bytes, message_start_index + payload.size());
}

bool ReadBytes(size_t num, std::vector<uint8_t>* buf) {
    return PlatformReadBytes(num, buf);
}

void Stop() {
    PlatformStop();
    Chaos_ResetAll();
    LUS::Context::GetInstance()->GetWindow()->GetGui()->GetGameOverlay()->TextDrawNotification(10.0f, true, "Chaos Mode Disabled");
}

void EachFrameCallback() {
	bool new_val = CVarGetInteger("gChaosEnabled", 0);
	if (g_is_enabled && !new_val) {
		// TODO: disable? close pipe?
	}

	if (!g_is_enabled && new_val) {
		Start();
	}

	g_is_enabled = new_val;

	if (!g_is_enabled) return;

	// Try to read a command from the buffer. If there isn't one, try again next frame.
	// First byte is the size of the command, in bytes (excluding the size byte).
	std::vector<uint8_t> buf(1);
	if (ReadBytes(1, &buf)){
		std::vector<uint8_t> current_command_buffer;
		size_t bytes_to_read = buf[0];
		current_command_buffer.resize(bytes_to_read);

		if (!ReadBytes(bytes_to_read, &current_command_buffer)) {
			std::string msg = "Error reading command, turning off Chaos Mode";
			LUS::Context::GetInstance()->GetWindow()->GetGui()->GetGameOverlay()->TextDrawNotification(10.0f, true, msg.c_str());
            Stop();
			CVarSetInteger("gChaosEnabled", 0);
			g_is_enabled = false;
			return;
		}

		EnqueueCommand(current_command_buffer);
	}

	g_link_is_ready_this_frame = link_is_ready();
	if (g_link_is_ready_this_frame && ice_trap_is_pending()) {
		give_ice_trap();
		g_link_is_ready_this_frame = false;
	}

	g_command_storage.Tick();

	apply_ongoing_effects();
	uint32_t grav = std::clamp<int32_t>(g_gravity_modifier, -12, 9);
	CVarSetInteger("gPlayerGravity", grav);
	CVarSetInteger("gChaosClimbSpeed", std::clamp<uint32_t>(g_climb_speed_modifier, 0, 9));
	CVarSetInteger("gHookshotLengthRemove", std::clamp<uint32_t>(g_hookshot_length_modifier, 0, 9));
}

void RegisterChaosHooks() {
	GameInteractor::Instance->RegisterGameHook<GameInteractor::OnSceneInit>([](int16_t sceneNum) {
		// GameInteractor::RemoveEffect(new GameInteractionEffect::ModifyLinkScale());
		GameInteractor::ChaosState::CustomLinkScale = 1.0f;
    });
}

extern "C" {
	void Chaos_ResetAll() {
		CVarSetInteger("gChaosForcedBoots", 0);
		CVarSetInteger("gChaosOHKO", 0);
		CVarSetInteger("gChaosNoHud", 0);
		CVarSetInteger("gChaosNoZ", 0);
		CVarSetInteger("gChaosTurbo", 0);
		CVarSetInteger("gChaosInvertControls", 0);
		CVarSetInteger("gEnemyHealthBar", 0);
		CVarSetInteger("gDisableFPSView", 0);
		CVarSetInteger("gForceNormalArrows", 0);
		CVarSetInteger("gDisableLedges", 0);
		CVarSetInteger("gFloorIsLava", 0);
		CVarSetInteger("gExplodingRolls", 0);
		CVarSetInteger("gFreezingRolls", 0);
		CVarSetInteger("gDisableTargeting", 0);
		CVarSetInteger("gMegaLetterbox", 0);
		CVarSetInteger("gDisableTurning", 0);
		CVarSetInteger("gJailTime", 0);
		CVarSetInteger("gOnHold", 0);
		CVarSetInteger("gSonicRoll", 0);
		CVarSetInteger("gNaviSpam", 0);
		CVarSetInteger("gScuffedLink", 0);
		CVarSetInteger("gRaveMode", 0);
		CVarSetInteger("gInvisPlayer", 0);
		CVarSetInteger("gSlipperyFloor", 0);
		CVarSetInteger("gIceDamage", 0);
		CVarSetInteger("gElectricDamage", 0);
		CVarSetInteger("gKnockbackDamage", 0);
		CVarSetInteger("gFireDamage", 0);
		CVarSetInteger("gForwardJump", 0);
		CVarSetInteger("gBigHead", 0);
		CVarSetInteger("gTinyHead", 0);
		CVarSetInteger("gDarkenArea", 0);
		CVarSetInteger("gChaosSpin", 0);
		CVarSetInteger("gDisableMeleeAttacks", 0);
		CVarSetInteger("gDisableEnemyDraw", 0);
		CVarSetInteger("gSandstorm", 0);
		CVarSetInteger("gSinkingFloor", 0);
		CVarSetInteger("gCowRitual", 0);
		CVarSetInteger("gFireRockRain", 0);
		CVarSetInteger("gCuccoAttack", 0);
		CVarSetInteger("gExplodingRupeeChallenge", 0);
		CVarSetInteger("gBanItemDropPickup", 0);
		CVarSetInteger("gBrokenBombchus", 0);
		CVarSetInteger("gAnnoyingGetItems", 0);
		CVarSetInteger("gPlayerGravity", 0);
		CVarSetInteger("gChaosClimbSpeed", 0);
		CVarSetInteger("gHookshotLengthRemove", 0);
		CVarSetInteger("gSpawnExplosion", 0);
		CVarSetInteger("gRestrainLink", 0);
		CVarSetInteger("gTripToSpace", 0);
		CVarSetInteger("gThiccLink", 0);
		CVarSetInteger("gFlipLink", 0);
		CVarSetInteger("gNoStart", 0);
		CVarSetInteger("gFlashbang", 0);
		CVarSetInteger("gPogoStick", 0);
		CVarSetInteger("gNoStrength", 0);
		CVarSetInteger("gNoWater", 0);
		CVarSetInteger("gMoonwalk", 0);
		CVarSetInteger("gDisableLedgeJump", 0);
		CVarSetInteger("gChaosRedeem", 0);
	}

	void Chaos_Init() {
		CVarSetInteger("gMusicMuted", 0);
  		CVarRegisterInteger("gChaosEnabled", 0);

  		CVarRegisterInteger("gChaosForcedBoots", 0);
  		CVarRegisterInteger("gChaosOHKO", 0);
  		CVarRegisterInteger("gChaosNoHud", 0);
  		CVarRegisterInteger("gChaosNoZ", 0);
  		CVarRegisterInteger("gChaosTurbo", 0);
  		CVarRegisterInteger("gChaosInvertControls", 0);

		Chaos_ResetAll();

		RegisterChaosHooks();
	}

	void Chaos_EachFrame() {
		EachFrameCallback();
	}
}

