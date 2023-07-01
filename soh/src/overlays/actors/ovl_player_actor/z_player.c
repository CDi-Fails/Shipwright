/*
 * File: z_player.c
 * Overlay: ovl_player_actor
 * Description: Link
 */

#include <libultraship/libultra.h>
#include "global.h"

#include "overlays/actors/ovl_Bg_Heavy_Block/z_bg_heavy_block.h"
#include "overlays/actors/ovl_Door_Shutter/z_door_shutter.h"
#include "overlays/actors/ovl_En_Boom/z_en_boom.h"
#include "overlays/actors/ovl_En_Arrow/z_en_arrow.h"
#include "overlays/actors/ovl_En_Box/z_en_box.h"
#include "overlays/actors/ovl_En_Door/z_en_door.h"
#include "overlays/actors/ovl_En_Elf/z_en_elf.h"
#include "overlays/actors/ovl_En_Fish/z_en_fish.h"
#include "overlays/actors/ovl_En_Horse/z_en_horse.h"
#include "overlays/effects/ovl_Effect_Ss_Fhg_Flash/z_eff_ss_fhg_flash.h"
#include "objects/gameplay_keep/gameplay_keep.h"
#include "objects/object_link_child/object_link_child.h"
#include "textures/icon_item_24_static/icon_item_24_static.h"
#include <soh/Enhancements/custom-message/CustomMessageTypes.h>
#include "soh/Enhancements/item-tables/ItemTableTypes.h"
#include "soh/Enhancements/game-interactor/GameInteractor.h"
#include "soh/Enhancements/randomizer/randomizer_entrance.h"
#include <overlays/actors/ovl_En_Partner/z_en_partner.h>
#include "soh/Enhancements/enhancementTypes.h"

#include "overlays/actors/ovl_En_Bom/z_en_bom.h"
#include "overlays/actors/ovl_Bg_Spot15_Saku/z_bg_spot15_saku.h"
#include "overlays/actors/ovl_En_Cow/z_en_cow.h"
#include "overlays/actors/ovl_En_Light/z_en_light.h"
#include "overlays/actors/ovl_En_Encount2/z_en_encount2.h"
#include "overlays/actors/ovl_En_Attack_Niw/z_en_attack_niw.h"

typedef enum {
    /* 0x00 */ KNOB_ANIM_ADULT_L,
    /* 0x01 */ KNOB_ANIM_CHILD_L,
    /* 0x02 */ KNOB_ANIM_ADULT_R,
    /* 0x03 */ KNOB_ANIM_CHILD_R
} KnobDoorAnim;

typedef struct {
    /* 0x00 */ u8 itemId;
    /* 0x02 */ s16 actorId;
} ExplosiveInfo; // size = 0x04

typedef struct {
    /* 0x00 */ s16 actorId;
    /* 0x02 */ u8 itemId;
    /* 0x03 */ u8 actionParam;
    /* 0x04 */ u8 textId;
} BottleCatchInfo; // size = 0x06

typedef struct {
    /* 0x00 */ s16 actorId;
    /* 0x02 */ s16 actorParams;
} BottleDropInfo; // size = 0x04

typedef struct {
    /* 0x00 */ s8 damage;
    /* 0x01 */ u8 unk_01;
    /* 0x02 */ u8 unk_02;
    /* 0x03 */ u8 unk_03;
    /* 0x04 */ u16 sfxId;
} FallImpactInfo; // size = 0x06

typedef struct {
    /* 0x00 */ Vec3f pos;
    /* 0x0C */ s16 yaw;
} SpecialRespawnInfo; // size = 0x10

typedef struct {
    /* 0x00 */ u16 sfxId;
    /* 0x02 */ s16 field;
} PlayerAnimSfxEntry; // size = 0x04

typedef struct {
    /* 0x00 */ u16 unk_00;
    /* 0x02 */ s16 unk_02;
} struct_808551A4; // size = 0x04

typedef struct {
    /* 0x00 */ LinkAnimationHeader* anim;
    /* 0x04 */ u8 unk_04;
} ItemChangeAnimInfo; // size = 0x08

typedef struct {
    /* 0x00 */ LinkAnimationHeader* bottleSwingAnim;
    /* 0x04 */ LinkAnimationHeader* bottleCatchAnim;
    /* 0x08 */ u8 unk_08;
    /* 0x09 */ u8 unk_09;
} BottleSwingAnimInfo; // size = 0x0C

typedef struct {
    /* 0x00 */ LinkAnimationHeader* unk_00;
    /* 0x04 */ LinkAnimationHeader* unk_04;
    /* 0x08 */ LinkAnimationHeader* unk_08;
    /* 0x0C */ u8 unk_0C;
    /* 0x0D */ u8 unk_0D;
} MeleeAttackAnimInfo; // size = 0x10

typedef struct {
    /* 0x00 */ LinkAnimationHeader* anim;
    /* 0x04 */ f32 unk_04;
    /* 0x04 */ f32 unk_08;
} struct_80854578; // size = 0x0C

typedef struct {
    /* 0x00 */ s8 type;
    /* 0x04 */ union {
        void* ptr;
        void (*func)(PlayState*, Player*, CsCmdActorAction*);
    };
} struct_80854B18; // size = 0x08

typedef struct {
    /* 0x00 */ s16 unk_00;
    /* 0x02 */ s16 unk_02;
    /* 0x04 */ s16 unk_04;
    /* 0x06 */ s16 unk_06;
    /* 0x08 */ s16 unk_08;
} struct_80858AC8; // size = 0x0A

void Player_Draw(Actor* thisx, PlayState* play2);

void Player_SpawnExplosion(PlayState* play, Player* this);
void Player_SetupReturnToStandStill(Player* this, PlayState* play);

void Player_DoNothing(PlayState* play, Player* this);
void Player_DoNothing2(PlayState* play, Player* this);
void Player_SetupBowOrSlingshot(PlayState* play, Player* this);
void Player_SetupDekuStick(PlayState* play, Player* this);
void Player_SetupExplosive(PlayState* play, Player* this);
void Player_SetupHookshot(PlayState* play, Player* this);
void Player_SetupBoomerang(PlayState* play, Player* this);
void Player_ChangeItem(PlayState* play, Player* this, s8 actionParam);
s32 Player_SetupStartZTargetDefend(Player* this, PlayState* play);
s32 Player_SetupStartZTargetDefend2(Player* this, PlayState* play);
s32 Player_StartChangeItem(Player* this, PlayState* play);
s32 Player_StandingDefend(Player* this, PlayState* play);
s32 Player_EndDefend(Player* this, PlayState* play);
s32 Player_HoldFpsItem(Player* this, PlayState* play);
s32 Player_ReadyFpsItemToShoot(Player* this, PlayState* play); // Arrow nocked
s32 Player_AimFpsItem(Player* this, PlayState* play); // Aiming in first person
s32 Player_EndAimFpsItem(Player* this, PlayState* play);
s32 Player_HoldActor(Player* this, PlayState* play);
s32 Player_HoldBoomerang(Player* this, PlayState* play);
s32 Player_SetupAimBoomerang(Player* this, PlayState* play); // Start aiming boomerang
s32 Player_AimBoomerang(Player* this, PlayState* play); // Aim boomerang
s32 Player_ThrowBoomerang(Player* this, PlayState* play); // Throw boomerang
s32 spawn_boomerang_ivan(EnPartner* this, PlayState* play); // Throw boomerang Ivan
s32 Player_WaitForThrownBoomerang(Player* this, PlayState* play); // Boomerang active
s32 Player_CatchBoomerang(Player* this, PlayState* play);
void Player_UseItem(PlayState* play, Player* this, s32 item);
void Player_SetupStandingStillType(Player* this, PlayState* play);
s32 Player_SetupWallJumpBehavior(Player* this, PlayState* play);
s32 Player_SetupOpenDoor(Player* this, PlayState* play);
s32 Player_SetupItemCutsceneOrFirstPerson(Player* this, PlayState* play);
s32 Player_SetupCUpBehavior(Player* this, PlayState* play);
s32 Player_SetupSpeakOrCheck(Player* this, PlayState* play);
s32 Player_SetupJumpSlashOrRoll(Player* this, PlayState* play);
s32 Player_SetupRollOrPutAway(Player* this, PlayState* play);
s32 Player_SetupDefend(Player* this, PlayState* play);
s32 Player_SetupStartChargeSpinAttack(Player* this, PlayState* play);
s32 Player_SetupThrowDekuNut(PlayState* play, Player* this);
void Player_SpawnNoMomentum(PlayState* play, Player* this);
void Player_SpawnWalkingSlow(PlayState* play, Player* this);
void Player_SpawnWalkingPreserveMomentum(PlayState* play, Player* this);
s32 Player_SetupMountHorse(Player* this, PlayState* play);
void Player_SetPendingFlag(Player* this, PlayState* play);
s32 Player_SetupGetItemOrHoldBehavior(Player* this, PlayState* play);
s32 Player_SetupPutDownOrThrowActor(Player* this, PlayState* play);
s32 Player_SetupSpecialWallInteraction(Player* this, PlayState* play);
void Player_UnfriendlyZTargetStandingStill(Player* this, PlayState* play);
void Player_FriendlyZTargetStandingStill(Player* this, PlayState* play);
void Player_StandingStill(Player* this, PlayState* play);
void Player_EndSidewalk(Player* this, PlayState* play);
void Player_FriendlyBackwalk(Player* this, PlayState* play);
void Player_HaltFriendlyBackwalk(Player* this, PlayState* play);
void Player_EndHaltFriendlyBackwalk(Player* this, PlayState* play);
void Player_Sidewalk(Player* this, PlayState* play);
void Player_Turn(Player* this, PlayState* play);
void Player_Run(Player* this, PlayState* play);
void Player_ZTargetingRun(Player* this, PlayState* play);
void func_8084279C(Player* this, PlayState* play);
void Player_UnfriendlyBackwalk(Player* this, PlayState* play);
void Player_EndUnfriendlyBackwalk(Player* this, PlayState* play);
void Player_AimShieldCrouched(Player* this, PlayState* play);
void Player_DeflectAttackWithShield(Player* this, PlayState* play);
void func_8084370C(Player* this, PlayState* play);
void Player_StartKnockback(Player* this, PlayState* play);
void Player_DownFromKnockback(Player* this, PlayState* play);
void Player_GetUpFromKnockback(Player* this, PlayState* play);
void Player_Die(Player* this, PlayState* play);
void Player_UpdateMidair(Player* this, PlayState* play);
void Player_Rolling(Player* this, PlayState* play);
void Player_FallingDive(Player* this, PlayState* play);
void Player_JumpSlash(Player* this, PlayState* play);
void Player_ChargeSpinAttack(Player* this, PlayState* play);
void Player_WalkChargingSpinAttack(Player* this, PlayState* play);
void Player_SidewalkChargingSpinAttack(Player* this, PlayState* play);
void Player_JumpUpToLedge(Player* this, PlayState* play);
void Player_RunMiniCutsceneFunc(Player* this, PlayState* play);
void Player_MiniCsMovement(Player* this, PlayState* play);
void Player_OpenDoor(Player* this, PlayState* play);
void Player_LiftActor(Player* this, PlayState* play);
void Player_ThrowStonePillar(Player* this, PlayState* play);
void Player_LiftSilverBoulder(Player* this, PlayState* play);
void Player_ThrowSilverBoulder(Player* this, PlayState* play);
void Player_FailToLiftActor(Player* this, PlayState* play);
void Player_SetupPutDownActor(Player* this, PlayState* play);
void Player_StartThrowActor(Player* this, PlayState* play);
void Player_SpawnNoUpdateOrDraw(PlayState* play, Player* this);
void Player_SetupSpawnFromBlueWarp(PlayState* play, Player* this);
void Player_SpawnFromTimeTravel(PlayState* play, Player* this);
void Player_SpawnOpeningDoor(PlayState* play, Player* this);
void Player_SpawnExitingGrotto(PlayState* play, Player* this);
void Player_SpawnWithKnockback(PlayState* play, Player* this);
void Player_SetupSpawnFromWarpSong(PlayState* play, Player* this);
void Player_SetupSpawnFromFaroresWind(PlayState* play, Player* this);
void Player_FirstPersonAiming(Player* this, PlayState* play);
void Player_TalkWithActor(Player* this, PlayState* play);
void Player_GrabPushPullWall(Player* this, PlayState* play);
void Player_PushWall(Player* this, PlayState* play);
void Player_PullWall(Player* this, PlayState* play);
void Player_GrabLedge(Player* this, PlayState* play);
void Player_ClimbOntoLedge(Player* this, PlayState* play);
void Player_ClimbingWallOrDownLedge(Player* this, PlayState* play);
void Player_UpdateCommon(Player* this, PlayState* play, Input* input);
void Player_EndClimb(Player* this, PlayState* play);
void Player_InsideCrawlspace(Player* this, PlayState* play);
void Player_ExitCrawlspace(Player* this, PlayState* play);
void Player_RideHorse(Player* this, PlayState* play);
void Player_DismountHorse(Player* this, PlayState* play);
void Player_UpdateSwimIdle(Player* this, PlayState* play);
void Player_SpawnSwimming(Player* this, PlayState* play);
void Player_Swim(Player* this, PlayState* play);
void Player_ZTargetSwimming(Player* this, PlayState* play);
void Player_Dive(Player* this, PlayState* play);
void Player_GetItemInWater(Player* this, PlayState* play);
void Player_DamagedSwim(Player* this, PlayState* play);
void Player_Drown(Player* this, PlayState* play);
void Player_PlayOcarina(Player* this, PlayState* play);
void Player_ThrowDekuNut(Player* this, PlayState* play);
void Player_GetItem(Player* this, PlayState* play);
void Player_EndTimeTravel(Player* this, PlayState* play);
void Player_DrinkFromBottle(Player* this, PlayState* play);
void Player_SwingBottle(Player* this, PlayState* play);
void Player_HealWithFairy(Player* this, PlayState* play);
void Player_DropItemFromBottle(Player* this, PlayState* play);
void Player_PresentExchangeItem(Player* this, PlayState* play);
void Player_SlipOnSlope(Player* this, PlayState* play);
void Player_SetDrawAndStartCutsceneAfterTimer(Player* this, PlayState* play);
void Player_SpawnFromWarpSong(Player* this, PlayState* play);
void Player_SpawnFromBlueWarp(Player* this, PlayState* play);
void Player_EnterGrotto(Player* this, PlayState* play);
void Player_SetupOpenDoorFromSpawn(Player* this, PlayState* play);
void Player_JumpFromGrotto(Player* this, PlayState* play);
void Player_ShootingGalleryPlay(Player* this, PlayState* play);
void Player_FrozenInIce(Player* this, PlayState* play);
void Player_SetupElectricShock(Player* this, PlayState* play);
s32 Player_CheckNoDebugModeCombo(Player* this, PlayState* play);
void Player_BowStringMoveAfterShot(Player* this);
void Player_BunnyHoodPhysics(Player* this);
s32 Player_SetupStartMeleeWeaponAttack(Player* this, PlayState* play);
void Player_MeleeWeaponAttack(Player* this, PlayState* play);
void Player_MeleeWeaponRebound(Player* this, PlayState* play);
void Player_ChooseFaroresWindOption(Player* this, PlayState* play);
void Player_SpawnFromFaroresWind(Player* this, PlayState* play);
void Player_UpdateMagicSpell(Player* this, PlayState* play);
void Player_MoveAlongHookshotPath(Player* this, PlayState* play);
void Player_CastFishingRod(Player* this, PlayState* play);
void Player_ReleaseCaughtFish(Player* this, PlayState* play);
void Player_AnimPlaybackType0(PlayState* play, Player* this, void* anim);
void Player_AnimPlaybackType1(PlayState* play, Player* this, void* anim);
void Player_AnimPlaybackType13(PlayState* play, Player* this, void* anim);
void Player_AnimPlaybackType2(PlayState* play, Player* this, void* anim);
void Player_AnimPlaybackType3(PlayState* play, Player* this, void* anim);
void Player_AnimPlaybackType4(PlayState* play, Player* this, void* anim);
void Player_AnimPlaybackType5(PlayState* play, Player* this, void* anim);
void Player_AnimPlaybackType6(PlayState* play, Player* this, void* anim);
void Player_AnimPlaybackType7(PlayState* play, Player* this, void* anim);
void Player_AnimPlaybackType8(PlayState* play, Player* this, void* anim);
void Player_AnimPlaybackType9(PlayState* play, Player* this, void* anim);
void Player_AnimPlaybackType14(PlayState* play, Player* this, void* anim);
void Player_AnimPlaybackType15(PlayState* play, Player* this, void* anim);
void Player_AnimPlaybackType10(PlayState* play, Player* this, void* anim);
void Player_AnimPlaybackType11(PlayState* play, Player* this, void* anim);
void Player_AnimPlaybackType16(PlayState* play, Player* this, void* anim);
void Player_AnimPlaybackType12(PlayState* play, Player* this, void* anim);
void Player_AnimPlaybackType17(PlayState* play, Player* this, void* arg2);
void Player_CutsceneSetupSwimIdle(PlayState* play, Player* this, CsCmdActorAction* arg2);
void Player_CutsceneSurfaceFromDive(PlayState* play, Player* this, CsCmdActorAction* arg2);
void Player_CutsceneIdle(PlayState* play, Player* this, CsCmdActorAction* arg2);
void Player_CutsceneTurnAroundSurprisedShort(PlayState* play, Player* this, CsCmdActorAction* arg2);
void Player_CutsceneSetupIdle(PlayState* play, Player* this, CsCmdActorAction* arg2);
void Player_CutsceneWait(PlayState* play, Player* this, CsCmdActorAction* arg2);
void Player_CutsceneTurnAroundSurprisedLong(PlayState* play, Player* this, CsCmdActorAction* arg2);
void Player_CutsceneSetupEnterWarp(PlayState* play, Player* this, CsCmdActorAction* arg2);
void Player_CutsceneEnterWarp(PlayState* play, Player* this, CsCmdActorAction* arg2);
void Player_CutsceneSetupFightStance(PlayState* play, Player* this, CsCmdActorAction* arg2);
void Player_CutsceneFightStance(PlayState* play, Player* this, CsCmdActorAction* arg2);
void Player_CutsceneUnk3Update(PlayState* play, Player* this, CsCmdActorAction* arg2);
void Player_CutsceneUnk4Update(PlayState* play, Player* this, CsCmdActorAction* arg2);
void Player_CutsceneSetupSwordPedestal(PlayState* play, Player* this, CsCmdActorAction* arg2);
void Player_CutsceneSwordPedestal(PlayState* play, Player* this, CsCmdActorAction* arg2);
void Player_CutsceneSetupWarpToSages(PlayState* play, Player* this, CsCmdActorAction* arg2);
void Player_CutsceneWarpToSages(PlayState* play, Player* this, CsCmdActorAction* arg2);
void Player_CutsceneKnockedToGround(PlayState* play, Player* this, CsCmdActorAction* arg2);
void Player_CutsceneSetupStartPlayOcarina(PlayState* play, Player* this, CsCmdActorAction* arg2);
void Player_CutsceneDrawAndBrandishSword(PlayState* play, Player* this, CsCmdActorAction* arg2);
void Player_CutsceneCloseEyes(PlayState* play, Player* this, CsCmdActorAction* arg2);
void Player_CutsceneOpenEyes(PlayState* play, Player* this, CsCmdActorAction* arg2);
void Player_CutsceneSetupGetItemInWater(PlayState* play, Player* this, CsCmdActorAction* arg2);
void Player_CutsceneSetupSleeping(PlayState* play, Player* this, CsCmdActorAction* arg2);
void Player_CutsceneSleeping(PlayState* play, Player* this, CsCmdActorAction* arg2);
void Player_CutsceneSetupSleepingRestless(PlayState* play, Player* this, CsCmdActorAction* arg2);
void Player_CutsceneAwaken(PlayState* play, Player* this, CsCmdActorAction* arg2);
void Player_CutsceneGetOffBed(PlayState* play, Player* this, CsCmdActorAction* arg2);
void Player_CutsceneSetupBlownBackward(PlayState* play, Player* this, CsCmdActorAction* arg2);
void Player_CutsceneBlownBackward(PlayState* play, Player* this, CsCmdActorAction* arg2);
void Player_CutsceneRaisedByWarp(PlayState* play, Player* this, CsCmdActorAction* arg2);
void Player_CutsceneSetupIdle3(PlayState* play, Player* this, CsCmdActorAction* arg2);
void Player_CutsceneIdle3(PlayState* play, Player* this, CsCmdActorAction* arg2);
void Player_CutsceneSetupStop(PlayState* play, Player* this, CsCmdActorAction* arg2);
void Player_CutsceneSetDraw(PlayState* play, Player* this, CsCmdActorAction* arg2);
void Player_CutsceneInspectGroundCarefully(PlayState* play, Player* this, CsCmdActorAction* arg2);
void Player_CutsceneStartPassOcarina(PlayState* play, Player* this, CsCmdActorAction* arg2);
void Player_CutsceneDrawSwordChild(PlayState* play, Player* this, CsCmdActorAction* arg2);
void Player_CutsceneTurnAroundSlowly(PlayState* play, Player* this, CsCmdActorAction* arg2);
void Player_CutsceneDesperateLookAtZeldasCrystal(PlayState* play, Player* this, CsCmdActorAction* arg2);
void Player_CutsceneStepBackCautiously(PlayState* play, Player* this, CsCmdActorAction* arg2);
void Player_CutsceneSetupSpinAttackIdle(PlayState* play, Player* this, CsCmdActorAction* arg2);
void Player_CutsceneSpinAttackIdle(PlayState* play, Player* this, CsCmdActorAction* arg2);
void Player_CutsceneInspectWeapon(PlayState* play, Player* this, CsCmdActorAction* arg2);
void Player_SetupDoNothing4(PlayState* play, Player* this, CsCmdActorAction* arg2);
void Player_DoNothing5(PlayState* play, Player* this, CsCmdActorAction* arg2);
void Player_CutsceneSetupKnockedToGroundDamaged(PlayState* play, Player* this, CsCmdActorAction* arg2);
void Player_CutsceneKnockedToGroundDamaged(PlayState* play, Player* this, CsCmdActorAction* arg2);
void Player_CutsceneSetupGetSwordBack(PlayState* play, Player* this, CsCmdActorAction* arg2);
void Player_CutsceneSwordKnockedFromHand(PlayState* play, Player* this, CsCmdActorAction* arg2);
void Player_LearnOcarinaSong(PlayState* play, Player* this, CsCmdActorAction* arg2);
void Player_CutsceneGetSwordBack(PlayState* play, Player* this, CsCmdActorAction* arg2);
void Player_CutsceneGanonKillCombo(PlayState* play, Player* this, CsCmdActorAction* arg2);
void Player_CutsceneEnd(PlayState* play, Player* this, CsCmdActorAction* arg2);
void Player_CutsceneSetPosAndYaw(PlayState* play, Player* this, CsCmdActorAction* arg2);
void Player_CutsceneUnk6Update(PlayState* play, Player* this, CsCmdActorAction* arg2);
void Player_StartCutscene(Player* this, PlayState* play);
s32 Player_IsDroppingFish(PlayState* play);
s32 Player_StartFishing(PlayState* play);
s32 Player_SetupRestrainedByEnemy(PlayState* play, Player* this);
s32 Player_SetupPlayerCutscene(PlayState* play, Actor* actor, s32 csMode);
void Player_SetupStandingStillMorph(Player* this, PlayState* play);
s32 Player_InflictDamage(PlayState* play, s32 damage);
s32 Player_InflictDamageModified(PlayState* play, s32 damage, u8 modified);
void Player_StartTalkingWithActor(PlayState* play, Actor* actor);

// .bss part 1
static s32 sPrevSkelAnimeMoveFlags;
static s32 sCurrentMask;
static Vec3f sWallIntersectPos;
static Input* sControlInput;

// .data

static u8 D_80853410[] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 };

static PlayerAgeProperties sAgeProperties[] = {
    {
        56.0f,
        90.0f,
        1.0f,
        111.0f,
        70.0f,
        79.4f,
        59.0f,
        41.0f,
        19.0f,
        36.0f,
        44.8f,
        56.0f,
        68.0f,
        70.0f,
        18.0f,
        15.0f,
        70.0f,
        { 9, 4671, 359 },
        {
            { 8, 4694, 380 },
            { 9, 6122, 359 },
            { 8, 4694, 380 },
            { 9, 6122, 359 },
        },
        {
            { 9, 6122, 359 },
            { 9, 7693, 380 },
            { 9, 6122, 359 },
            { 9, 7693, 380 },
        },
        {
            { 8, 4694, 380 },
            { 9, 6122, 359 },
        },
        {
            { -1592, 4694, 380 },
            { -1591, 6122, 359 },
        },
        0,
        0x80,
        &gPlayerAnim_link_demo_Tbox_open,
        &gPlayerAnim_link_demo_back_to_past,
        &gPlayerAnim_link_demo_return_to_past,
        &gPlayerAnim_link_normal_climb_startA,
        &gPlayerAnim_link_normal_climb_startB,
        { &gPlayerAnim_link_normal_climb_upL, &gPlayerAnim_link_normal_climb_upR, &gPlayerAnim_link_normal_Fclimb_upL,
          &gPlayerAnim_link_normal_Fclimb_upR },
        { &gPlayerAnim_link_normal_Fclimb_sideL, &gPlayerAnim_link_normal_Fclimb_sideR },
        { &gPlayerAnim_link_normal_climb_endAL, &gPlayerAnim_link_normal_climb_endAR },
        { &gPlayerAnim_link_normal_climb_endBR, &gPlayerAnim_link_normal_climb_endBL },
    },
    {
        40.0f,
        60.0f,
        11.0f / 17.0f,
        71.0f,
        50.0f,
        47.0f,
        39.0f,
        27.0f,
        19.0f,
        22.0f,
        29.6f,
        32.0f,
        48.0f,
        70.0f * (11.0f / 17.0f),
        14.0f,
        12.0f,
        55.0f,
        { -24, 3565, 876 },
        {
            { -24, 3474, 862 },
            { -24, 4977, 937 },
            { 8, 4694, 380 },
            { 9, 6122, 359 },
        },
        {
            { -24, 4977, 937 },
            { -24, 6495, 937 },
            { 9, 6122, 359 },
            { 9, 7693, 380 },
        },
        {
            { 8, 4694, 380 },
            { 9, 6122, 359 },
        },
        {
            { -1592, 4694, 380 },
            { -1591, 6122, 359 },
        },
        0x20,
        0,
        &gPlayerAnim_clink_demo_Tbox_open,
        &gPlayerAnim_clink_demo_goto_future,
        &gPlayerAnim_clink_demo_return_to_future,
        &gPlayerAnim_clink_normal_climb_startA,
        &gPlayerAnim_clink_normal_climb_startB,
        { &gPlayerAnim_clink_normal_climb_upL, &gPlayerAnim_clink_normal_climb_upR, &gPlayerAnim_link_normal_Fclimb_upL,
          &gPlayerAnim_link_normal_Fclimb_upR },
        { &gPlayerAnim_link_normal_Fclimb_sideL, &gPlayerAnim_link_normal_Fclimb_sideR },
        { &gPlayerAnim_clink_normal_climb_endAL, &gPlayerAnim_clink_normal_climb_endAR },
        { &gPlayerAnim_clink_normal_climb_endBR, &gPlayerAnim_clink_normal_climb_endBL },
    },
};

static u32 sDebugModeFlag = false;
static f32 sAnalogStickDistance = 0.0f;
static s16 sAnalogStickAngle = 0;
static s16 sCameraOffsetAnalogStickAngle = 0;
static s32 D_808535E0 = 0;
static s32 sFloorSpecialProperty = 0;
static f32 sWaterSpeedScale = 1.0f;
static f32 sInvertedWaterSpeedScale = 1.0f;
static u32 sTouchedWallFlags = 0;
static u32 sConveyorSpeedIndex = 0;
static s16 sIsFloorConveyor = 0;
static s16 sConveyorYaw = 0;
static f32 sPlayerYDistToFloor = 0.0f;
static s32 sFloorProperty = 0;
static s32 sYawToTouchedWall = 0;
static s32 sYawToTouchedWall2 = 0;
static s16 sAngleToFloorX = 0;
static s32 sUsingItemAlreadyInHand = 0;
static s32 sUsingItemAlreadyInHand2 = 0;

static u16 sInterruptableSfx[] = {
    NA_SE_VO_LI_SWEAT,
    NA_SE_VO_LI_SNEEZE,
    NA_SE_VO_LI_RELAX,
    NA_SE_VO_LI_FALL_L,
};

#define GET_PLAYER_ANIM(group, type) sPlayerAnimations[group * PLAYER_ANIMTYPE_MAX + type]

static LinkAnimationHeader* sPlayerAnimations[PLAYER_ANIMGROUP_MAX * PLAYER_ANIMTYPE_MAX] = {
    /* PLAYER_ANIMGROUP_STANDING_STILL */
    &gPlayerAnim_link_normal_wait_free,  // PLAYER_ANIMTYPE_DEFAULT
    &gPlayerAnim_link_normal_wait,       // PLAYER_ANIMTYPE_HOLDING_ONE_HAND_WEAPON
    &gPlayerAnim_link_normal_wait,       // PLAYER_ANIMTYPE_HOLDING_SHIELD
    &gPlayerAnim_link_fighter_wait_long, // PLAYER_ANIMTYPE_HOLDING_TWO_HAND_WEAPON
    &gPlayerAnim_link_normal_wait_free,  // PLAYER_ANIMTYPE_HOLDING_ITEM_IN_LEFT_HAND
    &gPlayerAnim_link_normal_wait_free,  // PLAYER_ANIMTYPE_USED_EXPLOSIVE
    /* PLAYER_ANIMGROUP_WALKING */
    &gPlayerAnim_link_normal_walk_free,  // PLAYER_ANIMTYPE_DEFAULT
    &gPlayerAnim_link_normal_walk,       // PLAYER_ANIMTYPE_HOLDING_ONE_HAND_WEAPON
    &gPlayerAnim_link_normal_walk,       // PLAYER_ANIMTYPE_HOLDING_SHIELD
    &gPlayerAnim_link_fighter_walk_long, // PLAYER_ANIMTYPE_HOLDING_TWO_HAND_WEAPON
    &gPlayerAnim_link_normal_walk_free,  // PLAYER_ANIMTYPE_HOLDING_ITEM_IN_LEFT_HAND
    &gPlayerAnim_link_normal_walk_free,  // PLAYER_ANIMTYPE_USED_EXPLOSIVE
    /* PLAYER_ANIMGROUP_RUNNING */
    &gPlayerAnim_link_normal_run_free,  // PLAYER_ANIMTYPE_DEFAULT
    &gPlayerAnim_link_fighter_run,      // PLAYER_ANIMTYPE_HOLDING_ONE_HAND_WEAPON
    &gPlayerAnim_link_normal_run,       // PLAYER_ANIMTYPE_HOLDING_SHIELD
    &gPlayerAnim_link_fighter_run_long, // PLAYER_ANIMTYPE_HOLDING_TWO_HAND_WEAPON
    &gPlayerAnim_link_normal_run_free,  // PLAYER_ANIMTYPE_HOLDING_ITEM_IN_LEFT_HAND
    &gPlayerAnim_link_normal_run_free,  // PLAYER_ANIMTYPE_USED_EXPLOSIVE
    /* PLAYER_ANIMGROUP_RUNNING_DAMAGED */
    &gPlayerAnim_link_normal_damage_run_free,  // PLAYER_ANIMTYPE_DEFAULT
    &gPlayerAnim_link_fighter_damage_run,      // PLAYER_ANIMTYPE_HOLDING_ONE_HAND_WEAPON
    &gPlayerAnim_link_normal_damage_run_free,  // PLAYER_ANIMTYPE_HOLDING_SHIELD
    &gPlayerAnim_link_fighter_damage_run_long, // PLAYER_ANIMTYPE_HOLDING_TWO_HAND_WEAPON
    &gPlayerAnim_link_normal_damage_run_free,  // PLAYER_ANIMTYPE_HOLDING_ITEM_IN_LEFT_HAND
    &gPlayerAnim_link_normal_damage_run_free,  // PLAYER_ANIMTYPE_USED_EXPLOSIVE
    /* PLAYER_ANIMGROUP_IRON_BOOTS */
    &gPlayerAnim_link_normal_heavy_run_free,  // PLAYER_ANIMTYPE_DEFAULT
    &gPlayerAnim_link_normal_heavy_run,       // PLAYER_ANIMTYPE_HOLDING_ONE_HAND_WEAPON
    &gPlayerAnim_link_normal_heavy_run_free,  // PLAYER_ANIMTYPE_HOLDING_SHIELD
    &gPlayerAnim_link_fighter_heavy_run_long, // PLAYER_ANIMTYPE_HOLDING_TWO_HAND_WEAPON
    &gPlayerAnim_link_normal_heavy_run_free,  // PLAYER_ANIMTYPE_HOLDING_ITEM_IN_LEFT_HAND
    &gPlayerAnim_link_normal_heavy_run_free,  // PLAYER_ANIMTYPE_USED_EXPLOSIVE
    /* PLAYER_ANIMGROUP_FIGHTING_LEFT_OF_ENEMY */
    &gPlayerAnim_link_normal_waitL_free,  // PLAYER_ANIMTYPE_DEFAULT
    &gPlayerAnim_link_anchor_waitL,       // PLAYER_ANIMTYPE_HOLDING_ONE_HAND_WEAPON
    &gPlayerAnim_link_anchor_waitL,       // PLAYER_ANIMTYPE_HOLDING_SHIELD
    &gPlayerAnim_link_fighter_waitL_long, // PLAYER_ANIMTYPE_HOLDING_TWO_HAND_WEAPON
    &gPlayerAnim_link_normal_waitL_free,  // PLAYER_ANIMTYPE_HOLDING_ITEM_IN_LEFT_HAND
    &gPlayerAnim_link_normal_waitL_free,  // PLAYER_ANIMTYPE_USED_EXPLOSIVE
    /* PLAYER_ANIMGROUP_FIGHTING_RIGHT_OF_ENEMY */
    &gPlayerAnim_link_normal_waitR_free,  // PLAYER_ANIMTYPE_DEFAULT
    &gPlayerAnim_link_anchor_waitR,       // PLAYER_ANIMTYPE_HOLDING_ONE_HAND_WEAPON
    &gPlayerAnim_link_anchor_waitR,       // PLAYER_ANIMTYPE_HOLDING_SHIELD
    &gPlayerAnim_link_fighter_waitR_long, // PLAYER_ANIMTYPE_HOLDING_TWO_HAND_WEAPON
    &gPlayerAnim_link_normal_waitR_free,  // PLAYER_ANIMTYPE_HOLDING_ITEM_IN_LEFT_HAND
    &gPlayerAnim_link_normal_waitR_free,  // PLAYER_ANIMTYPE_USED_EXPLOSIVE
    /* PLAYER_ANIMGROUP_START_FIGHTING */
    &gPlayerAnim_link_fighter_wait2waitR_long, // PLAYER_ANIMTYPE_DEFAULT
    &gPlayerAnim_link_normal_wait2waitR,       // PLAYER_ANIMTYPE_HOLDING_ONE_HAND_WEAPON
    &gPlayerAnim_link_normal_wait2waitR,       // PLAYER_ANIMTYPE_HOLDING_SHIELD
    &gPlayerAnim_link_fighter_wait2waitR_long, // PLAYER_ANIMTYPE_HOLDING_TWO_HAND_WEAPON
    &gPlayerAnim_link_fighter_wait2waitR_long, // PLAYER_ANIMTYPE_HOLDING_ITEM_IN_LEFT_HAND
    &gPlayerAnim_link_fighter_wait2waitR_long, // PLAYER_ANIMTYPE_USED_EXPLOSIVE
    /* PLAYER_ANIMGROUP_8 */
    &gPlayerAnim_link_normal_normal2fighter_free, // PLAYER_ANIMTYPE_DEFAULT
    &gPlayerAnim_link_fighter_normal2fighter,     // PLAYER_ANIMTYPE_HOLDING_ONE_HAND_WEAPON
    &gPlayerAnim_link_fighter_normal2fighter,     // PLAYER_ANIMTYPE_HOLDING_SHIELD
    &gPlayerAnim_link_normal_normal2fighter_free, // PLAYER_ANIMTYPE_HOLDING_TWO_HAND_WEAPON
    &gPlayerAnim_link_normal_normal2fighter_free, // PLAYER_ANIMTYPE_HOLDING_ITEM_IN_LEFT_HAND
    &gPlayerAnim_link_normal_normal2fighter_free, // PLAYER_ANIMTYPE_USED_EXPLOSIVE
    /* PLAYER_ANIMGROUP_OPEN_DOOR_ADULT_LEFT */
    &gPlayerAnim_link_demo_doorA_link_free, // PLAYER_ANIMTYPE_DEFAULT
    &gPlayerAnim_link_demo_doorA_link,      // PLAYER_ANIMTYPE_HOLDING_ONE_HAND_WEAPON
    &gPlayerAnim_link_demo_doorA_link,      // PLAYER_ANIMTYPE_HOLDING_SHIELD
    &gPlayerAnim_link_demo_doorA_link_free, // PLAYER_ANIMTYPE_HOLDING_TWO_HAND_WEAPON
    &gPlayerAnim_link_demo_doorA_link_free, // PLAYER_ANIMTYPE_HOLDING_ITEM_IN_LEFT_HAND
    &gPlayerAnim_link_demo_doorA_link_free, // PLAYER_ANIMTYPE_USED_EXPLOSIVE
    /* PLAYER_ANIMGROUP_OPEN_DOOR_CHILD_LEFT */
    &gPlayerAnim_clink_demo_doorA_link, // PLAYER_ANIMTYPE_DEFAULT
    &gPlayerAnim_clink_demo_doorA_link, // PLAYER_ANIMTYPE_HOLDING_ONE_HAND_WEAPON
    &gPlayerAnim_clink_demo_doorA_link, // PLAYER_ANIMTYPE_HOLDING_SHIELD
    &gPlayerAnim_clink_demo_doorA_link, // PLAYER_ANIMTYPE_HOLDING_TWO_HAND_WEAPON
    &gPlayerAnim_clink_demo_doorA_link, // PLAYER_ANIMTYPE_HOLDING_ITEM_IN_LEFT_HAND
    &gPlayerAnim_clink_demo_doorA_link, // PLAYER_ANIMTYPE_USED_EXPLOSIVE
    /* PLAYER_ANIMGROUP_OPEN_DOOR_ADULT_RIGHT */
    &gPlayerAnim_link_demo_doorB_link_free, // PLAYER_ANIMTYPE_DEFAULT
    &gPlayerAnim_link_demo_doorB_link,      // PLAYER_ANIMTYPE_HOLDING_ONE_HAND_WEAPON
    &gPlayerAnim_link_demo_doorB_link,      // PLAYER_ANIMTYPE_HOLDING_SHIELD
    &gPlayerAnim_link_demo_doorB_link_free, // PLAYER_ANIMTYPE_HOLDING_TWO_HAND_WEAPON
    &gPlayerAnim_link_demo_doorB_link_free, // PLAYER_ANIMTYPE_HOLDING_ITEM_IN_LEFT_HAND
    &gPlayerAnim_link_demo_doorB_link_free, // PLAYER_ANIMTYPE_USED_EXPLOSIVE
    /* PLAYER_ANIMGROUP_OPEN_DOOR_CHILD_RIGHT */
    &gPlayerAnim_clink_demo_doorB_link, // PLAYER_ANIMTYPE_DEFAULT
    &gPlayerAnim_clink_demo_doorB_link, // PLAYER_ANIMTYPE_HOLDING_ONE_HAND_WEAPON
    &gPlayerAnim_clink_demo_doorB_link, // PLAYER_ANIMTYPE_HOLDING_SHIELD
    &gPlayerAnim_clink_demo_doorB_link, // PLAYER_ANIMTYPE_HOLDING_TWO_HAND_WEAPON
    &gPlayerAnim_clink_demo_doorB_link, // PLAYER_ANIMTYPE_HOLDING_ITEM_IN_LEFT_HAND
    &gPlayerAnim_clink_demo_doorB_link, // PLAYER_ANIMTYPE_USED_EXPLOSIVE
    /* PLAYER_ANIMGROUP_HOLDING_OBJECT */
    &gPlayerAnim_link_normal_carryB_free, // PLAYER_ANIMTYPE_DEFAULT
    &gPlayerAnim_link_normal_carryB,      // PLAYER_ANIMTYPE_HOLDING_ONE_HAND_WEAPON
    &gPlayerAnim_link_normal_carryB,      // PLAYER_ANIMTYPE_HOLDING_SHIELD
    &gPlayerAnim_link_normal_carryB_free, // PLAYER_ANIMTYPE_HOLDING_TWO_HAND_WEAPON
    &gPlayerAnim_link_normal_carryB_free, // PLAYER_ANIMTYPE_HOLDING_ITEM_IN_LEFT_HAND
    &gPlayerAnim_link_normal_carryB_free, // PLAYER_ANIMTYPE_USED_EXPLOSIVE
    /* PLAYER_ANIMGROUP_TALL_JUMP_LANDING */
    &gPlayerAnim_link_normal_landing_free, // PLAYER_ANIMTYPE_DEFAULT
    &gPlayerAnim_link_normal_landing,      // PLAYER_ANIMTYPE_HOLDING_ONE_HAND_WEAPON
    &gPlayerAnim_link_normal_landing,      // PLAYER_ANIMTYPE_HOLDING_SHIELD
    &gPlayerAnim_link_normal_landing_free, // PLAYER_ANIMTYPE_HOLDING_TWO_HAND_WEAPON
    &gPlayerAnim_link_normal_landing_free, // PLAYER_ANIMTYPE_HOLDING_ITEM_IN_LEFT_HAND
    &gPlayerAnim_link_normal_landing_free, // PLAYER_ANIMTYPE_USED_EXPLOSIVE
    /* PLAYER_ANIMGROUP_SHORT_JUMP_LANDING */
    &gPlayerAnim_link_normal_short_landing_free, // PLAYER_ANIMTYPE_DEFAULT
    &gPlayerAnim_link_normal_short_landing,      // PLAYER_ANIMTYPE_HOLDING_ONE_HAND_WEAPON
    &gPlayerAnim_link_normal_short_landing,      // PLAYER_ANIMTYPE_HOLDING_SHIELD
    &gPlayerAnim_link_normal_short_landing_free, // PLAYER_ANIMTYPE_HOLDING_TWO_HAND_WEAPON
    &gPlayerAnim_link_normal_short_landing_free, // PLAYER_ANIMTYPE_HOLDING_ITEM_IN_LEFT_HAND
    &gPlayerAnim_link_normal_short_landing_free, // PLAYER_ANIMTYPE_USED_EXPLOSIVE
    /* PLAYER_ANIMGROUP_ROLLING */
    &gPlayerAnim_link_normal_landing_roll_free,  // PLAYER_ANIMTYPE_DEFAULT
    &gPlayerAnim_link_normal_landing_roll,       // PLAYER_ANIMTYPE_HOLDING_ONE_HAND_WEAPON
    &gPlayerAnim_link_normal_landing_roll,       // PLAYER_ANIMTYPE_HOLDING_SHIELD
    &gPlayerAnim_link_fighter_landing_roll_long, // PLAYER_ANIMTYPE_HOLDING_TWO_HAND_WEAPON
    &gPlayerAnim_link_normal_landing_roll_free,  // PLAYER_ANIMTYPE_HOLDING_ITEM_IN_LEFT_HAND
    &gPlayerAnim_link_normal_landing_roll_free,  // PLAYER_ANIMTYPE_USED_EXPLOSIVE
    /* PLAYER_ANIMGROUP_ROLL_BONKING */
    &gPlayerAnim_link_normal_hip_down_free, // PLAYER_ANIMTYPE_DEFAULT
    &gPlayerAnim_link_normal_hip_down,      // PLAYER_ANIMTYPE_HOLDING_ONE_HAND_WEAPON
    &gPlayerAnim_link_normal_hip_down,      // PLAYER_ANIMTYPE_HOLDING_SHIELD
    &gPlayerAnim_link_normal_hip_down_long, // PLAYER_ANIMTYPE_HOLDING_TWO_HAND_WEAPON
    &gPlayerAnim_link_normal_hip_down_free, // PLAYER_ANIMTYPE_HOLDING_ITEM_IN_LEFT_HAND
    &gPlayerAnim_link_normal_hip_down_free, // PLAYER_ANIMTYPE_USED_EXPLOSIVE
    /* PLAYER_ANIMGROUP_END_WALK_ON_LEFT_FOOT */
    &gPlayerAnim_link_normal_walk_endL_free,  // PLAYER_ANIMTYPE_DEFAULT
    &gPlayerAnim_link_normal_walk_endL,       // PLAYER_ANIMTYPE_HOLDING_ONE_HAND_WEAPON
    &gPlayerAnim_link_normal_walk_endL,       // PLAYER_ANIMTYPE_HOLDING_SHIELD
    &gPlayerAnim_link_fighter_walk_endL_long, // PLAYER_ANIMTYPE_HOLDING_TWO_HAND_WEAPON
    &gPlayerAnim_link_normal_walk_endL_free,  // PLAYER_ANIMTYPE_HOLDING_ITEM_IN_LEFT_HAND
    &gPlayerAnim_link_normal_walk_endL_free,  // PLAYER_ANIMTYPE_USED_EXPLOSIVE
    /* PLAYER_ANIMGROUP_END_WALK_ON_RIGHT_FOOT */
    &gPlayerAnim_link_normal_walk_endR_free,  // PLAYER_ANIMTYPE_DEFAULT
    &gPlayerAnim_link_normal_walk_endR,       // PLAYER_ANIMTYPE_HOLDING_ONE_HAND_WEAPON
    &gPlayerAnim_link_normal_walk_endR,       // PLAYER_ANIMTYPE_HOLDING_SHIELD
    &gPlayerAnim_link_fighter_walk_endR_long, // PLAYER_ANIMTYPE_HOLDING_TWO_HAND_WEAPON
    &gPlayerAnim_link_normal_walk_endR_free,  // PLAYER_ANIMTYPE_HOLDING_ITEM_IN_LEFT_HAND
    &gPlayerAnim_link_normal_walk_endR_free,  // PLAYER_ANIMTYPE_USED_EXPLOSIVE
    /* PLAYER_ANIMGROUP_START_DEFENDING */
    &gPlayerAnim_link_normal_defense_free, // PLAYER_ANIMTYPE_DEFAULT
    &gPlayerAnim_link_normal_defense,      // PLAYER_ANIMTYPE_HOLDING_ONE_HAND_WEAPON
    &gPlayerAnim_link_normal_defense,      // PLAYER_ANIMTYPE_HOLDING_SHIELD
    &gPlayerAnim_link_normal_defense_free, // PLAYER_ANIMTYPE_HOLDING_TWO_HAND_WEAPON
    &gPlayerAnim_link_bow_defense,         // PLAYER_ANIMTYPE_HOLDING_ITEM_IN_LEFT_HAND
    &gPlayerAnim_link_normal_defense_free, // PLAYER_ANIMTYPE_USED_EXPLOSIVE
    /* PLAYER_ANIMGROUP_DEFENDING */
    &gPlayerAnim_link_normal_defense_wait_free, // PLAYER_ANIMTYPE_DEFAULT
    &gPlayerAnim_link_normal_defense_wait,      // PLAYER_ANIMTYPE_HOLDING_ONE_HAND_WEAPON
    &gPlayerAnim_link_normal_defense_wait,      // PLAYER_ANIMTYPE_HOLDING_SHIELD
    &gPlayerAnim_link_normal_defense_wait_free, // PLAYER_ANIMTYPE_HOLDING_TWO_HAND_WEAPON
    &gPlayerAnim_link_bow_defense_wait,         // PLAYER_ANIMTYPE_HOLDING_ITEM_IN_LEFT_HAND
    &gPlayerAnim_link_normal_defense_wait_free, // PLAYER_ANIMTYPE_USED_EXPLOSIVE
    /* PLAYER_ANIMGROUP_END_DEFENDING */
    &gPlayerAnim_link_normal_defense_end_free, // PLAYER_ANIMTYPE_DEFAULT
    &gPlayerAnim_link_normal_defense_end,      // PLAYER_ANIMTYPE_HOLDING_ONE_HAND_WEAPON
    &gPlayerAnim_link_normal_defense_end,      // PLAYER_ANIMTYPE_HOLDING_SHIELD
    &gPlayerAnim_link_normal_defense_end_free, // PLAYER_ANIMTYPE_HOLDING_TWO_HAND_WEAPON
    &gPlayerAnim_link_normal_defense_end_free, // PLAYER_ANIMTYPE_HOLDING_ITEM_IN_LEFT_HAND
    &gPlayerAnim_link_normal_defense_end_free, // PLAYER_ANIMTYPE_USED_EXPLOSIVE
    /* PLAYER_ANIMGROUP_END_SIDEWALKING */
    &gPlayerAnim_link_normal_side_walk_free,  // PLAYER_ANIMTYPE_DEFAULT
    &gPlayerAnim_link_normal_side_walk,       // PLAYER_ANIMTYPE_HOLDING_ONE_HAND_WEAPON
    &gPlayerAnim_link_normal_side_walk,       // PLAYER_ANIMTYPE_HOLDING_SHIELD
    &gPlayerAnim_link_fighter_side_walk_long, // PLAYER_ANIMTYPE_HOLDING_TWO_HAND_WEAPON
    &gPlayerAnim_link_normal_side_walk_free,  // PLAYER_ANIMTYPE_HOLDING_ITEM_IN_LEFT_HAND
    &gPlayerAnim_link_normal_side_walk_free,  // PLAYER_ANIMTYPE_USED_EXPLOSIVE
    /* PLAYER_ANIMGROUP_SIDEWALKING_LEFT */
    &gPlayerAnim_link_normal_side_walkL_free,  // PLAYER_ANIMTYPE_DEFAULT
    &gPlayerAnim_link_anchor_side_walkL,       // PLAYER_ANIMTYPE_HOLDING_ONE_HAND_WEAPON
    &gPlayerAnim_link_anchor_side_walkL,       // PLAYER_ANIMTYPE_HOLDING_SHIELD
    &gPlayerAnim_link_fighter_side_walkL_long, // PLAYER_ANIMTYPE_HOLDING_TWO_HAND_WEAPON
    &gPlayerAnim_link_normal_side_walkL_free,  // PLAYER_ANIMTYPE_HOLDING_ITEM_IN_LEFT_HAND
    &gPlayerAnim_link_normal_side_walkL_free,  // PLAYER_ANIMTYPE_USED_EXPLOSIVE
    /* PLAYER_ANIMGROUP_SIDEWALKING_RIGHT */
    &gPlayerAnim_link_normal_side_walkR_free,  // PLAYER_ANIMTYPE_DEFAULT
    &gPlayerAnim_link_anchor_side_walkR,       // PLAYER_ANIMTYPE_HOLDING_ONE_HAND_WEAPON
    &gPlayerAnim_link_anchor_side_walkR,       // PLAYER_ANIMTYPE_HOLDING_SHIELD
    &gPlayerAnim_link_fighter_side_walkR_long, // PLAYER_ANIMTYPE_HOLDING_TWO_HAND_WEAPON
    &gPlayerAnim_link_normal_side_walkR_free,  // PLAYER_ANIMTYPE_HOLDING_ITEM_IN_LEFT_HAND
    &gPlayerAnim_link_normal_side_walkR_free,  // PLAYER_ANIMTYPE_USED_EXPLOSIVE
    /* PLAYER_ANIMGROUP_SHUFFLE_TURN */
    &gPlayerAnim_link_normal_45_turn_free, // PLAYER_ANIMTYPE_DEFAULT
    &gPlayerAnim_link_normal_45_turn,      // PLAYER_ANIMTYPE_HOLDING_ONE_HAND_WEAPON
    &gPlayerAnim_link_normal_45_turn,      // PLAYER_ANIMTYPE_HOLDING_SHIELD
    &gPlayerAnim_link_normal_45_turn_free, // PLAYER_ANIMTYPE_HOLDING_TWO_HAND_WEAPON
    &gPlayerAnim_link_normal_45_turn_free, // PLAYER_ANIMTYPE_HOLDING_ITEM_IN_LEFT_HAND
    &gPlayerAnim_link_normal_45_turn_free, // PLAYER_ANIMTYPE_USED_EXPLOSIVE
    /* PLAYER_ANIMGROUP_END_FIGHTING_LEFT_OF_ENEMY */
    &gPlayerAnim_link_fighter_waitL2wait_long, // PLAYER_ANIMTYPE_DEFAULT
    &gPlayerAnim_link_normal_waitL2wait,       // PLAYER_ANIMTYPE_HOLDING_ONE_HAND_WEAPON
    &gPlayerAnim_link_normal_waitL2wait,       // PLAYER_ANIMTYPE_HOLDING_SHIELD
    &gPlayerAnim_link_fighter_waitL2wait_long, // PLAYER_ANIMTYPE_HOLDING_TWO_HAND_WEAPON
    &gPlayerAnim_link_fighter_waitL2wait_long, // PLAYER_ANIMTYPE_HOLDING_ITEM_IN_LEFT_HAND
    &gPlayerAnim_link_fighter_waitL2wait_long, // PLAYER_ANIMTYPE_USED_EXPLOSIVE
    /* PLAYER_ANIMGROUP_END_FIGHTING_RIGHT_OF_ENEMY */
    &gPlayerAnim_link_fighter_waitR2wait_long, // PLAYER_ANIMTYPE_DEFAULT
    &gPlayerAnim_link_normal_waitR2wait,       // PLAYER_ANIMTYPE_HOLDING_ONE_HAND_WEAPON
    &gPlayerAnim_link_normal_waitR2wait,       // PLAYER_ANIMTYPE_HOLDING_SHIELD
    &gPlayerAnim_link_fighter_waitR2wait_long, // PLAYER_ANIMTYPE_HOLDING_TWO_HAND_WEAPON
    &gPlayerAnim_link_fighter_waitR2wait_long, // PLAYER_ANIMTYPE_HOLDING_ITEM_IN_LEFT_HAND
    &gPlayerAnim_link_fighter_waitR2wait_long, // PLAYER_ANIMTYPE_USED_EXPLOSIVE
    /* PLAYER_ANIMGROUP_THROWING_OBJECT */
    &gPlayerAnim_link_normal_throw_free, // PLAYER_ANIMTYPE_DEFAULT
    &gPlayerAnim_link_normal_throw,      // PLAYER_ANIMTYPE_HOLDING_ONE_HAND_WEAPON
    &gPlayerAnim_link_normal_throw,      // PLAYER_ANIMTYPE_HOLDING_SHIELD
    &gPlayerAnim_link_normal_throw_free, // PLAYER_ANIMTYPE_HOLDING_TWO_HAND_WEAPON
    &gPlayerAnim_link_normal_throw_free, // PLAYER_ANIMTYPE_HOLDING_ITEM_IN_LEFT_HAND
    &gPlayerAnim_link_normal_throw_free, // PLAYER_ANIMTYPE_USED_EXPLOSIVE
    /* PLAYER_ANIMGROUP_PUTTING_DOWN_OBJECT */
    &gPlayerAnim_link_normal_put_free, // PLAYER_ANIMTYPE_DEFAULT
    &gPlayerAnim_link_normal_put,      // PLAYER_ANIMTYPE_HOLDING_ONE_HAND_WEAPON
    &gPlayerAnim_link_normal_put,      // PLAYER_ANIMTYPE_HOLDING_SHIELD
    &gPlayerAnim_link_normal_put_free, // PLAYER_ANIMTYPE_HOLDING_TWO_HAND_WEAPON
    &gPlayerAnim_link_normal_put_free, // PLAYER_ANIMTYPE_HOLDING_ITEM_IN_LEFT_HAND
    &gPlayerAnim_link_normal_put_free, // PLAYER_ANIMTYPE_USED_EXPLOSIVE
    /* PLAYER_ANIMGROUP_BACKWALKING */
    &gPlayerAnim_link_normal_back_walk, // PLAYER_ANIMTYPE_DEFAULT
    &gPlayerAnim_link_normal_back_walk, // PLAYER_ANIMTYPE_HOLDING_ONE_HAND_WEAPON
    &gPlayerAnim_link_normal_back_walk, // PLAYER_ANIMTYPE_HOLDING_SHIELD
    &gPlayerAnim_link_normal_back_walk, // PLAYER_ANIMTYPE_HOLDING_TWO_HAND_WEAPON
    &gPlayerAnim_link_normal_back_walk, // PLAYER_ANIMTYPE_HOLDING_ITEM_IN_LEFT_HAND
    &gPlayerAnim_link_normal_back_walk, // PLAYER_ANIMTYPE_USED_EXPLOSIVE
    /* PLAYER_ANIMGROUP_START_CHECKING_OR_SPEAKING */
    &gPlayerAnim_link_normal_check_free, // PLAYER_ANIMTYPE_DEFAULT
    &gPlayerAnim_link_normal_check,      // PLAYER_ANIMTYPE_HOLDING_ONE_HAND_WEAPON
    &gPlayerAnim_link_normal_check,      // PLAYER_ANIMTYPE_HOLDING_SHIELD
    &gPlayerAnim_link_normal_check_free, // PLAYER_ANIMTYPE_HOLDING_TWO_HAND_WEAPON
    &gPlayerAnim_link_normal_check_free, // PLAYER_ANIMTYPE_HOLDING_ITEM_IN_LEFT_HAND
    &gPlayerAnim_link_normal_check_free, // PLAYER_ANIMTYPE_USED_EXPLOSIVE
    /* PLAYER_ANIMGROUP_CHECKING_OR_SPEAKING */
    &gPlayerAnim_link_normal_check_wait_free, // PLAYER_ANIMTYPE_DEFAULT
    &gPlayerAnim_link_normal_check_wait,      // PLAYER_ANIMTYPE_HOLDING_ONE_HAND_WEAPON
    &gPlayerAnim_link_normal_check_wait,      // PLAYER_ANIMTYPE_HOLDING_SHIELD
    &gPlayerAnim_link_normal_check_wait_free, // PLAYER_ANIMTYPE_HOLDING_TWO_HAND_WEAPON
    &gPlayerAnim_link_normal_check_wait_free, // PLAYER_ANIMTYPE_HOLDING_ITEM_IN_LEFT_HAND
    &gPlayerAnim_link_normal_check_wait_free, // PLAYER_ANIMTYPE_USED_EXPLOSIVE
    /* PLAYER_ANIMGROUP_END_CHECKING_OR_SPEAKING */
    &gPlayerAnim_link_normal_check_end_free, // PLAYER_ANIMTYPE_DEFAULT
    &gPlayerAnim_link_normal_check_end,      // PLAYER_ANIMTYPE_HOLDING_ONE_HAND_WEAPON
    &gPlayerAnim_link_normal_check_end,      // PLAYER_ANIMTYPE_HOLDING_SHIELD
    &gPlayerAnim_link_normal_check_end_free, // PLAYER_ANIMTYPE_HOLDING_TWO_HAND_WEAPON
    &gPlayerAnim_link_normal_check_end_free, // PLAYER_ANIMTYPE_HOLDING_ITEM_IN_LEFT_HAND
    &gPlayerAnim_link_normal_check_end_free, // PLAYER_ANIMTYPE_USED_EXPLOSIVE
    /* PLAYER_ANIMGROUP_END_PULL_OBJECT */
    &gPlayerAnim_link_normal_pull_start_free, // PLAYER_ANIMTYPE_DEFAULT
    &gPlayerAnim_link_normal_pull_start,      // PLAYER_ANIMTYPE_HOLDING_ONE_HAND_WEAPON
    &gPlayerAnim_link_normal_pull_start,      // PLAYER_ANIMTYPE_HOLDING_SHIELD
    &gPlayerAnim_link_normal_pull_start_free, // PLAYER_ANIMTYPE_HOLDING_TWO_HAND_WEAPON
    &gPlayerAnim_link_normal_pull_start_free, // PLAYER_ANIMTYPE_HOLDING_ITEM_IN_LEFT_HAND
    &gPlayerAnim_link_normal_pull_start_free, // PLAYER_ANIMTYPE_USED_EXPLOSIVE
    /* PLAYER_ANIMGROUP_PULL_OBJECT */
    &gPlayerAnim_link_normal_pulling_free, // PLAYER_ANIMTYPE_DEFAULT
    &gPlayerAnim_link_normal_pulling,      // PLAYER_ANIMTYPE_HOLDING_ONE_HAND_WEAPON
    &gPlayerAnim_link_normal_pulling,      // PLAYER_ANIMTYPE_HOLDING_SHIELD
    &gPlayerAnim_link_normal_pulling_free, // PLAYER_ANIMTYPE_HOLDING_TWO_HAND_WEAPON
    &gPlayerAnim_link_normal_pulling_free, // PLAYER_ANIMTYPE_HOLDING_ITEM_IN_LEFT_HAND
    &gPlayerAnim_link_normal_pulling_free, // PLAYER_ANIMTYPE_USED_EXPLOSIVE
    /* PLAYER_ANIMGROUP_PUSH_OBJECT */
    &gPlayerAnim_link_normal_pull_end_free, // PLAYER_ANIMTYPE_DEFAULT
    &gPlayerAnim_link_normal_pull_end,      // PLAYER_ANIMTYPE_HOLDING_ONE_HAND_WEAPON
    &gPlayerAnim_link_normal_pull_end,      // PLAYER_ANIMTYPE_HOLDING_SHIELD
    &gPlayerAnim_link_normal_pull_end_free, // PLAYER_ANIMTYPE_HOLDING_TWO_HAND_WEAPON
    &gPlayerAnim_link_normal_pull_end_free, // PLAYER_ANIMTYPE_HOLDING_ITEM_IN_LEFT_HAND
    &gPlayerAnim_link_normal_pull_end_free, // PLAYER_ANIMTYPE_USED_EXPLOSIVE
    /* PLAYER_ANIMGROUP_KNOCKED_FROM_CLIMBING */
    &gPlayerAnim_link_normal_fall_up_free, // PLAYER_ANIMTYPE_DEFAULT
    &gPlayerAnim_link_normal_fall_up,      // PLAYER_ANIMTYPE_HOLDING_ONE_HAND_WEAPON
    &gPlayerAnim_link_normal_fall_up,      // PLAYER_ANIMTYPE_HOLDING_SHIELD
    &gPlayerAnim_link_normal_fall_up_free, // PLAYER_ANIMTYPE_HOLDING_TWO_HAND_WEAPON
    &gPlayerAnim_link_normal_fall_up_free, // PLAYER_ANIMTYPE_HOLDING_ITEM_IN_LEFT_HAND
    &gPlayerAnim_link_normal_fall_up_free, // PLAYER_ANIMTYPE_USED_EXPLOSIVE
    /* PLAYER_ANIMGROUP_HANGING_FROM_LEDGE */
    &gPlayerAnim_link_normal_jump_climb_hold_free, // PLAYER_ANIMTYPE_DEFAULT
    &gPlayerAnim_link_normal_jump_climb_hold,      // PLAYER_ANIMTYPE_HOLDING_ONE_HAND_WEAPON
    &gPlayerAnim_link_normal_jump_climb_hold,      // PLAYER_ANIMTYPE_HOLDING_SHIELD
    &gPlayerAnim_link_normal_jump_climb_hold_free, // PLAYER_ANIMTYPE_HOLDING_TWO_HAND_WEAPON
    &gPlayerAnim_link_normal_jump_climb_hold_free, // PLAYER_ANIMTYPE_HOLDING_ITEM_IN_LEFT_HAND
    &gPlayerAnim_link_normal_jump_climb_hold_free, // PLAYER_ANIMTYPE_USED_EXPLOSIVE
    /* PLAYER_ANIMGROUP_CLIMBING_IDLE */
    &gPlayerAnim_link_normal_jump_climb_wait_free, // PLAYER_ANIMTYPE_DEFAULT
    &gPlayerAnim_link_normal_jump_climb_wait,      // PLAYER_ANIMTYPE_HOLDING_ONE_HAND_WEAPON
    &gPlayerAnim_link_normal_jump_climb_wait,      // PLAYER_ANIMTYPE_HOLDING_SHIELD
    &gPlayerAnim_link_normal_jump_climb_wait_free, // PLAYER_ANIMTYPE_HOLDING_TWO_HAND_WEAPON
    &gPlayerAnim_link_normal_jump_climb_wait_free, // PLAYER_ANIMTYPE_HOLDING_ITEM_IN_LEFT_HAND
    &gPlayerAnim_link_normal_jump_climb_wait_free, // PLAYER_ANIMTYPE_USED_EXPLOSIVE
    /* PLAYER_ANIMGROUP_CLIMBING */
    &gPlayerAnim_link_normal_jump_climb_up_free, // PLAYER_ANIMTYPE_DEFAULT
    &gPlayerAnim_link_normal_jump_climb_up,      // PLAYER_ANIMTYPE_HOLDING_ONE_HAND_WEAPON
    &gPlayerAnim_link_normal_jump_climb_up,      // PLAYER_ANIMTYPE_HOLDING_SHIELD
    &gPlayerAnim_link_normal_jump_climb_up_free, // PLAYER_ANIMTYPE_HOLDING_TWO_HAND_WEAPON
    &gPlayerAnim_link_normal_jump_climb_up_free, // PLAYER_ANIMTYPE_HOLDING_ITEM_IN_LEFT_HAND
    &gPlayerAnim_link_normal_jump_climb_up_free, // PLAYER_ANIMTYPE_USED_EXPLOSIVE
    /* PLAYER_ANIMGROUP_SLIDING_DOWN_SLOPE */
    &gPlayerAnim_link_normal_down_slope_slip_end_free, // PLAYER_ANIMTYPE_DEFAULT
    &gPlayerAnim_link_normal_down_slope_slip_end,      // PLAYER_ANIMTYPE_HOLDING_ONE_HAND_WEAPON
    &gPlayerAnim_link_normal_down_slope_slip_end,      // PLAYER_ANIMTYPE_HOLDING_SHIELD
    &gPlayerAnim_link_normal_down_slope_slip_end_long, // PLAYER_ANIMTYPE_HOLDING_TWO_HAND_WEAPON
    &gPlayerAnim_link_normal_down_slope_slip_end_free, // PLAYER_ANIMTYPE_HOLDING_ITEM_IN_LEFT_HAND
    &gPlayerAnim_link_normal_down_slope_slip_end_free, // PLAYER_ANIMTYPE_USED_EXPLOSIVE
    /* PLAYER_ANIMGROUP_END_SLIDING_DOWN_SLOPE */
    &gPlayerAnim_link_normal_up_slope_slip_end_free, // PLAYER_ANIMTYPE_DEFAULT
    &gPlayerAnim_link_normal_up_slope_slip_end,      // PLAYER_ANIMTYPE_HOLDING_ONE_HAND_WEAPON
    &gPlayerAnim_link_normal_up_slope_slip_end,      // PLAYER_ANIMTYPE_HOLDING_SHIELD
    &gPlayerAnim_link_normal_up_slope_slip_end_long, // PLAYER_ANIMTYPE_HOLDING_TWO_HAND_WEAPON
    &gPlayerAnim_link_normal_up_slope_slip_end_free, // PLAYER_ANIMTYPE_HOLDING_ITEM_IN_LEFT_HAND
    &gPlayerAnim_link_normal_up_slope_slip_end_free, // PLAYER_ANIMTYPE_USED_EXPLOSIVE
    /* PLAYER_ANIMGROUP_RELAX */
    &gPlayerAnim_sude_nwait, // PLAYER_ANIMTYPE_DEFAULT
    &gPlayerAnim_lkt_nwait,  // PLAYER_ANIMTYPE_HOLDING_ONE_HAND_WEAPON
    &gPlayerAnim_lkt_nwait,  // PLAYER_ANIMTYPE_HOLDING_SHIELD
    &gPlayerAnim_sude_nwait, // PLAYER_ANIMTYPE_HOLDING_TWO_HAND_WEAPON
    &gPlayerAnim_sude_nwait, // PLAYER_ANIMTYPE_HOLDING_ITEM_IN_LEFT_HAND
    &gPlayerAnim_sude_nwait, // PLAYER_ANIMTYPE_USED_EXPLOSIVE
};

static LinkAnimationHeader* sManualJumpAnims[][3] = {
    { &gPlayerAnim_link_fighter_front_jump, &gPlayerAnim_link_fighter_front_jump_end,
      &gPlayerAnim_link_fighter_front_jump_endR },
    { &gPlayerAnim_link_fighter_Lside_jump, &gPlayerAnim_link_fighter_Lside_jump_end,
      &gPlayerAnim_link_fighter_Lside_jump_endL },
    { &gPlayerAnim_link_fighter_backturn_jump, &gPlayerAnim_link_fighter_backturn_jump_end,
      &gPlayerAnim_link_fighter_backturn_jump_endR },
    { &gPlayerAnim_link_fighter_Rside_jump, &gPlayerAnim_link_fighter_Rside_jump_end,
      &gPlayerAnim_link_fighter_Rside_jump_endR },
};

static LinkAnimationHeader* sIdleAnims[][2] = {
    { &gPlayerAnim_link_normal_wait_typeA_20f, &gPlayerAnim_link_normal_waitF_typeA_20f },
    { &gPlayerAnim_link_normal_wait_typeC_20f, &gPlayerAnim_link_normal_waitF_typeC_20f },
    { &gPlayerAnim_link_normal_wait_typeB_20f, &gPlayerAnim_link_normal_waitF_typeB_20f },
    { &gPlayerAnim_link_normal_wait_typeB_20f, &gPlayerAnim_link_normal_waitF_typeB_20f },
    { &gPlayerAnim_link_wait_typeD_20f, &gPlayerAnim_link_waitF_typeD_20f },
    { &gPlayerAnim_link_wait_typeD_20f, &gPlayerAnim_link_waitF_typeD_20f },
    { &gPlayerAnim_link_wait_typeD_20f, &gPlayerAnim_link_waitF_typeD_20f },
    { &gPlayerAnim_link_wait_heat1_20f, &gPlayerAnim_link_waitF_heat1_20f },
    { &gPlayerAnim_link_wait_heat2_20f, &gPlayerAnim_link_waitF_heat2_20f },
    { &gPlayerAnim_link_wait_itemD1_20f, &gPlayerAnim_link_wait_itemD1_20f },
    { &gPlayerAnim_link_wait_itemA_20f, &gPlayerAnim_link_waitF_itemA_20f },
    { &gPlayerAnim_link_wait_itemB_20f, &gPlayerAnim_link_waitF_itemB_20f },
    { &gPlayerAnim_link_wait_itemC_20f, &gPlayerAnim_link_wait_itemC_20f },
    { &gPlayerAnim_link_wait_itemD2_20f, &gPlayerAnim_link_wait_itemD2_20f }
};

static PlayerAnimSfxEntry D_80853DEC[] = {
    { NA_SE_VO_LI_SNEEZE, -0x2008 },
};

static PlayerAnimSfxEntry D_80853DF0[] = {
    { NA_SE_VO_LI_SWEAT, -0x2012 },
};

static PlayerAnimSfxEntry D_80853DF4[] = {
    { NA_SE_VO_LI_BREATH_REST, -0x200D },
};

static PlayerAnimSfxEntry D_80853DF8[] = {
    { NA_SE_VO_LI_BREATH_REST, -0x200A },
};

static PlayerAnimSfxEntry D_80853DFC[] = {
    { NA_SE_PL_CALM_HIT, 0x82C }, { NA_SE_PL_CALM_HIT, 0x830 },  { NA_SE_PL_CALM_HIT, 0x834 },
    { NA_SE_PL_CALM_HIT, 0x838 }, { NA_SE_PL_CALM_HIT, -0x83C },
};

static PlayerAnimSfxEntry D_80853E10[] = {
    { 0, 0x4019 }, { 0, 0x401E }, { 0, 0x402C }, { 0, 0x4030 }, { 0, 0x4034 }, { 0, -0x4038 },
};

static PlayerAnimSfxEntry D_80853E28[] = {
    { NA_SE_IT_SHIELD_POSTURE, 0x810 },
    { NA_SE_IT_SHIELD_POSTURE, 0x814 },
    { NA_SE_IT_SHIELD_POSTURE, -0x846 },
};

static PlayerAnimSfxEntry D_80853E34[] = {
    { NA_SE_IT_HAMMER_SWING, 0x80A },
    { NA_SE_VO_LI_AUTO_JUMP, 0x200A },
    { NA_SE_IT_SWORD_SWING, 0x816 },
    { NA_SE_VO_LI_SWORD_N, -0x2016 },
};

static PlayerAnimSfxEntry D_80853E44[] = {
    { NA_SE_IT_SWORD_SWING, 0x827 },
    { NA_SE_VO_LI_SWORD_N, -0x2027 },
};

static PlayerAnimSfxEntry D_80853E4C[] = {
    { NA_SE_VO_LI_RELAX, -0x2014 },
};

static PlayerAnimSfxEntry* D_80853E50[] = {
    D_80853DEC, D_80853DF0, D_80853DF4, D_80853DF8, D_80853DFC, D_80853E10,
    D_80853E28, D_80853E34, D_80853E44, D_80853E4C, NULL,
};

static u8 D_80853E7C[] = {
    0, 0, 1, 1, 2, 2, 2, 2, 10, 10, 10, 10, 10, 10, 3, 3, 4, 4, 8, 8, 5, 5, 6, 6, 7, 7, 9, 9, 0,
};

// Used to map item IDs to action params
static s8 sItemActionParams[] = {
    PLAYER_IA_STICK,
    PLAYER_IA_NUT,
    PLAYER_IA_BOMB,
    PLAYER_IA_BOW,
    PLAYER_IA_BOW_FIRE,
    PLAYER_IA_DINS_FIRE,
    PLAYER_IA_SLINGSHOT,
    PLAYER_IA_OCARINA_FAIRY,
    PLAYER_IA_OCARINA_TIME,
    PLAYER_IA_BOMBCHU,
    PLAYER_IA_HOOKSHOT,
    PLAYER_IA_LONGSHOT,
    PLAYER_IA_BOW_ICE,
    PLAYER_IA_FARORES_WIND,
    PLAYER_IA_BOOMERANG,
    PLAYER_IA_LENS,
    PLAYER_IA_BEAN,
    PLAYER_IA_HAMMER,
    PLAYER_IA_BOW_LIGHT,
    PLAYER_IA_NAYRUS_LOVE,
    PLAYER_IA_BOTTLE,
    PLAYER_IA_BOTTLE_POTION_RED,
    PLAYER_IA_BOTTLE_POTION_GREEN,
    PLAYER_IA_BOTTLE_POTION_BLUE,
    PLAYER_IA_BOTTLE_FAIRY,
    PLAYER_IA_BOTTLE_FISH,
    PLAYER_IA_BOTTLE_MILK,
    PLAYER_IA_BOTTLE_LETTER,
    PLAYER_IA_BOTTLE_FIRE,
    PLAYER_IA_BOTTLE_BUG,
    PLAYER_IA_BOTTLE_BIG_POE,
    PLAYER_IA_BOTTLE_MILK_HALF,
    PLAYER_IA_BOTTLE_POE,
    PLAYER_IA_WEIRD_EGG,
    PLAYER_IA_CHICKEN,
    PLAYER_IA_LETTER_ZELDA,
    PLAYER_IA_MASK_KEATON,
    PLAYER_IA_MASK_SKULL,
    PLAYER_IA_MASK_SPOOKY,
    PLAYER_IA_MASK_BUNNY,
    PLAYER_IA_MASK_GORON,
    PLAYER_IA_MASK_ZORA,
    PLAYER_IA_MASK_GERUDO,
    PLAYER_IA_MASK_TRUTH,
    PLAYER_IA_SWORD_MASTER,
    PLAYER_IA_POCKET_EGG,
    PLAYER_IA_POCKET_CUCCO,
    PLAYER_IA_COJIRO,
    PLAYER_IA_ODD_MUSHROOM,
    PLAYER_IA_ODD_POTION,
    PLAYER_IA_SAW,
    PLAYER_IA_SWORD_BROKEN,
    PLAYER_IA_PRESCRIPTION,
    PLAYER_IA_FROG,
    PLAYER_IA_EYEDROPS,
    PLAYER_IA_CLAIM_CHECK,
    PLAYER_IA_BOW_FIRE,
    PLAYER_IA_BOW_ICE,
    PLAYER_IA_BOW_LIGHT,
    PLAYER_IA_SWORD_KOKIRI,
    PLAYER_IA_SWORD_MASTER,
    PLAYER_IA_SWORD_BGS,
    PLAYER_IA_SHIELD_DEKU,
    PLAYER_IA_SHIELD_HYLIAN,
    PLAYER_IA_SHIELD_MIRROR,
    PLAYER_IA_TUNIC_KOKIRI,
    PLAYER_IA_TUNIC_GORON,
    PLAYER_IA_TUNIC_ZORA,
    PLAYER_IA_BOOTS_KOKIRI,
    PLAYER_IA_BOOTS_IRON,
    PLAYER_IA_BOOTS_HOVER,
};

static u8 sMaskMemory;
u8 gWalkSpeedToggle1;
u8 gWalkSpeedToggle2;

// Used to map action params to update functions
static s32 (*sUpperBodyItemFuncs[])(Player* this, PlayState* play) = {
    Player_SetupStartZTargetDefend,  // PLAYER_AP_NONE,
    Player_SetupStartZTargetDefend,  // PLAYER_AP_LAST_USED,
    Player_SetupStartZTargetDefend,  // PLAYER_AP_FISHING_POLE,
    Player_SetupStartZTargetDefend2, // PLAYER_AP_SWORD_MASTER,
    Player_SetupStartZTargetDefend2, // PLAYER_AP_SWORD_KOKIRI,
    Player_SetupStartZTargetDefend2, // PLAYER_AP_SWORD_BGS,
    Player_SetupStartZTargetDefend,  // PLAYER_AP_STICK,
    Player_SetupStartZTargetDefend,  // PLAYER_AP_HAMMER,
    Player_HoldFpsItem,              // PLAYER_AP_BOW,
    Player_HoldFpsItem,              // PLAYER_AP_BOW_FIRE,
    Player_HoldFpsItem,              // PLAYER_AP_BOW_ICE,
    Player_HoldFpsItem,              // PLAYER_AP_BOW_LIGHT,
    Player_HoldFpsItem,              // PLAYER_AP_BOW_0C,
    Player_HoldFpsItem,              // PLAYER_AP_BOW_0D,
    Player_HoldFpsItem,              // PLAYER_AP_BOW_0E,
    Player_HoldFpsItem,              // PLAYER_AP_SLINGSHOT,
    Player_HoldFpsItem,              // PLAYER_AP_HOOKSHOT,
    Player_HoldFpsItem,              // PLAYER_AP_LONGSHOT,
    Player_HoldActor,                // PLAYER_AP_BOMB,
    Player_HoldActor,                // PLAYER_AP_BOMBCHU,
    Player_HoldBoomerang,            // PLAYER_AP_BOOMERANG,
    Player_SetupStartZTargetDefend,  // PLAYER_AP_MAGIC_SPELL_15,
    Player_SetupStartZTargetDefend,  // PLAYER_AP_MAGIC_SPELL_16,
    Player_SetupStartZTargetDefend,  // PLAYER_AP_MAGIC_SPELL_17,
    Player_SetupStartZTargetDefend,  // PLAYER_AP_FARORES_WIND,
    Player_SetupStartZTargetDefend,  // PLAYER_AP_NAYRUS_LOVE,
    Player_SetupStartZTargetDefend,  // PLAYER_AP_DINS_FIRE,
    Player_SetupStartZTargetDefend,  // PLAYER_AP_NUT,
    Player_SetupStartZTargetDefend,  // PLAYER_AP_OCARINA_FAIRY,
    Player_SetupStartZTargetDefend,  // PLAYER_AP_OCARINA_TIME,
    Player_SetupStartZTargetDefend,  // PLAYER_AP_BOTTLE,
    Player_SetupStartZTargetDefend,  // PLAYER_AP_BOTTLE_FISH,
    Player_SetupStartZTargetDefend,  // PLAYER_AP_BOTTLE_FIRE,
    Player_SetupStartZTargetDefend,  // PLAYER_AP_BOTTLE_BUG,
    Player_SetupStartZTargetDefend,  // PLAYER_AP_BOTTLE_POE,
    Player_SetupStartZTargetDefend,  // PLAYER_AP_BOTTLE_BIG_POE,
    Player_SetupStartZTargetDefend,  // PLAYER_AP_BOTTLE_LETTER,
    Player_SetupStartZTargetDefend,  // PLAYER_AP_BOTTLE_POTION_RED,
    Player_SetupStartZTargetDefend,  // PLAYER_AP_BOTTLE_POTION_BLUE,
    Player_SetupStartZTargetDefend,  // PLAYER_AP_BOTTLE_POTION_GREEN,
    Player_SetupStartZTargetDefend,  // PLAYER_AP_BOTTLE_MILK,
    Player_SetupStartZTargetDefend,  // PLAYER_AP_BOTTLE_MILK_HALF,
    Player_SetupStartZTargetDefend,  // PLAYER_AP_BOTTLE_FAIRY,
    Player_SetupStartZTargetDefend,  // PLAYER_AP_LETTER_ZELDA,
    Player_SetupStartZTargetDefend,  // PLAYER_AP_WEIRD_EGG,
    Player_SetupStartZTargetDefend,  // PLAYER_AP_CHICKEN,
    Player_SetupStartZTargetDefend,  // PLAYER_AP_BEAN,
    Player_SetupStartZTargetDefend,  // PLAYER_AP_POCKET_EGG,
    Player_SetupStartZTargetDefend,  // PLAYER_AP_POCKET_CUCCO,
    Player_SetupStartZTargetDefend,  // PLAYER_AP_COJIRO,
    Player_SetupStartZTargetDefend,  // PLAYER_AP_ODD_MUSHROOM,
    Player_SetupStartZTargetDefend,  // PLAYER_AP_ODD_POTION,
    Player_SetupStartZTargetDefend,  // PLAYER_AP_SAW,
    Player_SetupStartZTargetDefend,  // PLAYER_AP_SWORD_BROKEN,
    Player_SetupStartZTargetDefend,  // PLAYER_AP_PRESCRIPTION,
    Player_SetupStartZTargetDefend,  // PLAYER_AP_FROG,
    Player_SetupStartZTargetDefend,  // PLAYER_AP_EYEDROPS,
    Player_SetupStartZTargetDefend,  // PLAYER_AP_CLAIM_CHECK,
    Player_SetupStartZTargetDefend,  // PLAYER_AP_MASK_KEATON,
    Player_SetupStartZTargetDefend,  // PLAYER_AP_MASK_SKULL,
    Player_SetupStartZTargetDefend,  // PLAYER_AP_MASK_SPOOKY,
    Player_SetupStartZTargetDefend,  // PLAYER_AP_MASK_BUNNY,
    Player_SetupStartZTargetDefend,  // PLAYER_AP_MASK_GORON,
    Player_SetupStartZTargetDefend,  // PLAYER_AP_MASK_ZORA,
    Player_SetupStartZTargetDefend,  // PLAYER_AP_MASK_GERUDO,
    Player_SetupStartZTargetDefend,  // PLAYER_AP_MASK_TRUTH,
    Player_SetupStartZTargetDefend,  // PLAYER_AP_LENS,
};

static void (*sItemChangeFuncs[])(PlayState* play, Player* this) = {
    Player_DoNothing,           // PLAYER_AP_NONE,
    Player_DoNothing,           // PLAYER_AP_LAST_USED,
    Player_DoNothing,           // PLAYER_AP_FISHING_POLE,
    Player_DoNothing,           // PLAYER_AP_SWORD_MASTER,
    Player_DoNothing,           // PLAYER_AP_SWORD_KOKIRI,
    Player_DoNothing,           // PLAYER_AP_SWORD_BGS,
    Player_SetupDekuStick,      // PLAYER_AP_STICK,
    Player_DoNothing2,          // PLAYER_AP_HAMMER,
    Player_SetupBowOrSlingshot, // PLAYER_AP_BOW,
    Player_SetupBowOrSlingshot, // PLAYER_AP_BOW_FIRE,
    Player_SetupBowOrSlingshot, // PLAYER_AP_BOW_ICE,
    Player_SetupBowOrSlingshot, // PLAYER_AP_BOW_LIGHT,
    Player_SetupBowOrSlingshot, // PLAYER_AP_BOW_0C,
    Player_SetupBowOrSlingshot, // PLAYER_AP_BOW_0D,
    Player_SetupBowOrSlingshot, // PLAYER_AP_BOW_0E,
    Player_SetupBowOrSlingshot, // PLAYER_AP_SLINGSHOT,
    Player_SetupHookshot,       // PLAYER_AP_HOOKSHOT,
    Player_SetupHookshot,       // PLAYER_AP_LONGSHOT,
    Player_SetupExplosive,      // PLAYER_AP_BOMB,
    Player_SetupExplosive,      // PLAYER_AP_BOMBCHU,
    Player_SetupBoomerang,      // PLAYER_AP_BOOMERANG,
    Player_DoNothing,           // PLAYER_AP_MAGIC_SPELL_15,
    Player_DoNothing,           // PLAYER_AP_MAGIC_SPELL_16,
    Player_DoNothing,           // PLAYER_AP_MAGIC_SPELL_17,
    Player_DoNothing,           // PLAYER_AP_FARORES_WIND,
    Player_DoNothing,           // PLAYER_AP_NAYRUS_LOVE,
    Player_DoNothing,           // PLAYER_AP_DINS_FIRE,
    Player_DoNothing,           // PLAYER_AP_NUT,
    Player_DoNothing,           // PLAYER_AP_OCARINA_FAIRY,
    Player_DoNothing,           // PLAYER_AP_OCARINA_TIME,
    Player_DoNothing,           // PLAYER_AP_BOTTLE,
    Player_DoNothing,           // PLAYER_AP_BOTTLE_FISH,
    Player_DoNothing,           // PLAYER_AP_BOTTLE_FIRE,
    Player_DoNothing,           // PLAYER_AP_BOTTLE_BUG,
    Player_DoNothing,           // PLAYER_AP_BOTTLE_POE,
    Player_DoNothing,           // PLAYER_AP_BOTTLE_BIG_POE,
    Player_DoNothing,           // PLAYER_AP_BOTTLE_LETTER,
    Player_DoNothing,           // PLAYER_AP_BOTTLE_POTION_RED,
    Player_DoNothing,           // PLAYER_AP_BOTTLE_POTION_BLUE,
    Player_DoNothing,           // PLAYER_AP_BOTTLE_POTION_GREEN,
    Player_DoNothing,           // PLAYER_AP_BOTTLE_MILK,
    Player_DoNothing,           // PLAYER_AP_BOTTLE_MILK_HALF,
    Player_DoNothing,           // PLAYER_AP_BOTTLE_FAIRY,
    Player_DoNothing,           // PLAYER_AP_LETTER_ZELDA,
    Player_DoNothing,           // PLAYER_AP_WEIRD_EGG,
    Player_DoNothing,           // PLAYER_AP_CHICKEN,
    Player_DoNothing,           // PLAYER_AP_BEAN,
    Player_DoNothing,           // PLAYER_AP_POCKET_EGG,
    Player_DoNothing,           // PLAYER_AP_POCKET_CUCCO,
    Player_DoNothing,           // PLAYER_AP_COJIRO,
    Player_DoNothing,           // PLAYER_AP_ODD_MUSHROOM,
    Player_DoNothing,           // PLAYER_AP_ODD_POTION,
    Player_DoNothing,           // PLAYER_AP_SAW,
    Player_DoNothing,           // PLAYER_AP_SWORD_BROKEN,
    Player_DoNothing,           // PLAYER_AP_PRESCRIPTION,
    Player_DoNothing,           // PLAYER_AP_FROG,
    Player_DoNothing,           // PLAYER_AP_EYEDROPS,
    Player_DoNothing,           // PLAYER_AP_CLAIM_CHECK,
    Player_DoNothing,           // PLAYER_AP_MASK_KEATON,
    Player_DoNothing,           // PLAYER_AP_MASK_SKULL,
    Player_DoNothing,           // PLAYER_AP_MASK_SPOOKY,
    Player_DoNothing,           // PLAYER_AP_MASK_BUNNY,
    Player_DoNothing,           // PLAYER_AP_MASK_GORON,
    Player_DoNothing,           // PLAYER_AP_MASK_ZORA,
    Player_DoNothing,           // PLAYER_AP_MASK_GERUDO,
    Player_DoNothing,           // PLAYER_AP_MASK_TRUTH,
    Player_DoNothing,           // PLAYER_AP_LENS,
};

typedef enum {
    /*  0 */ PLAYER_ITEM_CHANGE_DEFAULT,
    /*  1 */ PLAYER_ITEM_CHANGE_SHIELD_TO_1HAND,
    /*  2 */ PLAYER_ITEM_CHANGE_SHIELD_TO_2HAND,
    /*  3 */ PLAYER_ITEM_CHANGE_SHIELD,
    /*  4 */ PLAYER_ITEM_CHANGE_2HAND_TO_1HAND,
    /*  5 */ PLAYER_ITEM_CHANGE_1HAND,
    /*  6 */ PLAYER_ITEM_CHANGE_2HAND,
    /*  7 */ PLAYER_ITEM_CHANGE_2HAND_TO_2HAND,
    /*  8 */ PLAYER_ITEM_CHANGE_DEFAULT_2,
    /*  9 */ PLAYER_ITEM_CHANGE_1HAND_TO_BOMB,
    /* 10 */ PLAYER_ITEM_CHANGE_2HAND_TO_BOMB,
    /* 11 */ PLAYER_ITEM_CHANGE_BOMB,
    /* 12 */ PLAYER_ITEM_CHANGE_UNK_12,
    /* 13 */ PLAYER_ITEM_CHANGE_LEFT_HAND,
    /* 14 */ PLAYER_ITEM_CHANGE_MAX
} PlayersItemChangeAnimsIndex;

static ItemChangeAnimInfo sItemChangeAnimsInfo[PLAYER_ITEM_CHANGE_MAX] = {
    /* PLAYER_ITEM_CHANGE_DEFAULT */ { &gPlayerAnim_link_normal_free2free, 12 },
    /* PLAYER_ITEM_CHANGE_SHIELD_TO_1HAND */ { &gPlayerAnim_link_normal_normal2fighter, 6 },
    /* PLAYER_ITEM_CHANGE_SHIELD_TO_2HAND */ { &gPlayerAnim_link_hammer_normal2long, 8 },
    /* PLAYER_ITEM_CHANGE_SHIELD */ { &gPlayerAnim_link_normal_normal2free, 8 },
    /* PLAYER_ITEM_CHANGE_2HAND_TO_1HAND */ { &gPlayerAnim_link_fighter_fighter2long, 8 },
    /* PLAYER_ITEM_CHANGE_1HAND */ { &gPlayerAnim_link_normal_fighter2free, 10 },
    /* PLAYER_ITEM_CHANGE_2HAND */ { &gPlayerAnim_link_hammer_long2free, 7 },
    /* PLAYER_ITEM_CHANGE_2HAND_TO_2HAND */ { &gPlayerAnim_link_hammer_long2long, 11 },
    /* PLAYER_ITEM_CHANGE_DEFAULT_2 */ { &gPlayerAnim_link_normal_free2free, 12 },
    /* PLAYER_ITEM_CHANGE_1HAND_TO_BOMB */ { &gPlayerAnim_link_normal_normal2bom, 4 },
    /* PLAYER_ITEM_CHANGE_2HAND_TO_BOMB */ { &gPlayerAnim_link_normal_long2bom, 4 },
    /* PLAYER_ITEM_CHANGE_BOMB */ { &gPlayerAnim_link_normal_free2bom, 4 },
    /* PLAYER_ITEM_CHANGE_UNK_12 */ { &gPlayerAnim_link_anchor_anchor2fighter, 5 },
    /* PLAYER_ITEM_CHANGE_LEFT_HAND */ { &gPlayerAnim_link_normal_free2freeB, 13 },
};

static s8 sAnimtypeToItemChangeAnims[PLAYER_ANIMTYPE_MAX][PLAYER_ANIMTYPE_MAX] = {
    // From: PLAYER_ANIMTYPE_DEFAULT, to:
    {
        PLAYER_ITEM_CHANGE_DEFAULT_2, // PLAYER_ANIMTYPE_DEFAULT
        -PLAYER_ITEM_CHANGE_1HAND,    // PLAYER_ANIMTYPE_HOLDING_ONE_HAND_WEAPON
        -PLAYER_ITEM_CHANGE_SHIELD,   // PLAYER_ANIMTYPE_HOLDING_SHIELD
        -PLAYER_ITEM_CHANGE_2HAND,    // PLAYER_ANIMTYPE_HOLDING_TWO_HAND_WEAPON
        PLAYER_ITEM_CHANGE_DEFAULT_2, // PLAYER_ANIMTYPE_HOLDING_ITEM_IN_LEFT_HAND
        PLAYER_ITEM_CHANGE_BOMB       // PLAYER_ANIMTYPE_USED_EXPLOSIVE
    },
    // From: PLAYER_ANIMTYPE_HOLDING_ONE_HAND_WEAPON, to:
    {
        PLAYER_ITEM_CHANGE_1HAND,            // PLAYER_ANIMTYPE_DEFAULT
        PLAYER_ITEM_CHANGE_DEFAULT,          // PLAYER_ANIMTYPE_HOLDING_ONE_HAND_WEAPON
        -PLAYER_ITEM_CHANGE_SHIELD_TO_1HAND, // PLAYER_ANIMTYPE_HOLDING_SHIELD
        PLAYER_ITEM_CHANGE_2HAND_TO_1HAND,   // PLAYER_ANIMTYPE_HOLDING_TWO_HAND_WEAPON
        PLAYER_ITEM_CHANGE_1HAND,            // PLAYER_ANIMTYPE_HOLDING_ITEM_IN_LEFT_HAND
        PLAYER_ITEM_CHANGE_1HAND_TO_BOMB     // PLAYER_ANIMTYPE_USED_EXPLOSIVE
    },
    // From: PLAYER_ANIMTYPE_HOLDING_SHIELD, to:
    {
        PLAYER_ITEM_CHANGE_SHIELD,          // PLAYER_ANIMTYPE_DEFAULT
        PLAYER_ITEM_CHANGE_SHIELD_TO_1HAND, // PLAYER_ANIMTYPE_HOLDING_ONE_HAND_WEAPON
        PLAYER_ITEM_CHANGE_DEFAULT,         // PLAYER_ANIMTYPE_HOLDING_SHIELD
        PLAYER_ITEM_CHANGE_SHIELD_TO_2HAND, // PLAYER_ANIMTYPE_HOLDING_TWO_HAND_WEAPON
        PLAYER_ITEM_CHANGE_SHIELD,          // PLAYER_ANIMTYPE_HOLDING_ITEM_IN_LEFT_HAND
        PLAYER_ITEM_CHANGE_1HAND_TO_BOMB    // PLAYER_ANIMTYPE_USED_EXPLOSIVE
    },
    // From: PLAYER_ANIMTYPE_HOLDING_TWO_HAND_WEAPON, to:
    {
        PLAYER_ITEM_CHANGE_2HAND,            // PLAYER_ANIMTYPE_DEFAULT
        -PLAYER_ITEM_CHANGE_2HAND_TO_1HAND,  // PLAYER_ANIMTYPE_HOLDING_ONE_HAND_WEAPON
        -PLAYER_ITEM_CHANGE_SHIELD_TO_2HAND, // PLAYER_ANIMTYPE_HOLDING_SHIELD
        PLAYER_ITEM_CHANGE_2HAND_TO_2HAND,   // PLAYER_ANIMTYPE_HOLDING_TWO_HAND_WEAPON
        PLAYER_ITEM_CHANGE_2HAND,            // PLAYER_ANIMTYPE_HOLDING_ITEM_IN_LEFT_HAND
        PLAYER_ITEM_CHANGE_2HAND_TO_BOMB     // PLAYER_ANIMTYPE_USED_EXPLOSIVE
    },
    // From: PLAYER_ANIMTYPE_HOLDING_ITEM_IN_LEFT_HAND, to:
    {
        PLAYER_ITEM_CHANGE_DEFAULT_2, // PLAYER_ANIMTYPE_DEFAULT
        -PLAYER_ITEM_CHANGE_1HAND,    // PLAYER_ANIMTYPE_HOLDING_ONE_HAND_WEAPON
        -PLAYER_ITEM_CHANGE_SHIELD,   // PLAYER_ANIMTYPE_HOLDING_SHIELD
        -PLAYER_ITEM_CHANGE_2HAND,    // PLAYER_ANIMTYPE_HOLDING_TWO_HAND_WEAPON
        PLAYER_ITEM_CHANGE_DEFAULT_2, // PLAYER_ANIMTYPE_HOLDING_ITEM_IN_LEFT_HAND
        PLAYER_ITEM_CHANGE_BOMB       // PLAYER_ANIMTYPE_USED_EXPLOSIVE
    },
    // From: PLAYER_ANIMTYPE_USED_EXPLOSIVE, to:
    {
        PLAYER_ITEM_CHANGE_DEFAULT_2, // PLAYER_ANIMTYPE_DEFAULT
        -PLAYER_ITEM_CHANGE_1HAND,    // PLAYER_ANIMTYPE_HOLDING_ONE_HAND_WEAPON
        -PLAYER_ITEM_CHANGE_SHIELD,   // PLAYER_ANIMTYPE_HOLDING_SHIELD
        -PLAYER_ITEM_CHANGE_2HAND,    // PLAYER_ANIMTYPE_HOLDING_TWO_HAND_WEAPON
        PLAYER_ITEM_CHANGE_DEFAULT_2, // PLAYER_ANIMTYPE_HOLDING_ITEM_IN_LEFT_HAND
        PLAYER_ITEM_CHANGE_BOMB       // PLAYER_ANIMTYPE_USED_EXPLOSIVE
    },
};

static ExplosiveInfo sExplosiveInfos[] = {
    { ITEM_BOMB, ACTOR_EN_BOM },
    { ITEM_BOMBCHU, ACTOR_EN_BOM_CHU },
};

static MeleeAttackAnimInfo sMeleeAttackAnims[] = {
    /* PLAYER_MELEEATKTYPE_FORWARD_SLASH_1H */
    { &gPlayerAnim_link_fighter_normal_kiru, &gPlayerAnim_link_fighter_normal_kiru_end,
      &gPlayerAnim_link_fighter_normal_kiru_endR, 1, 4 },
    /* PLAYER_MELEEATKTYPE_FORWARD_SLASH_2H */
    { &gPlayerAnim_link_fighter_Lnormal_kiru, &gPlayerAnim_link_fighter_Lnormal_kiru_end,
      &gPlayerAnim_link_anchor_Lnormal_kiru_endR, 1, 4 },
    /* PLAYER_MELEEATKTYPE_FORWARD_COMBO_1H */
    { &gPlayerAnim_link_fighter_normal_kiru_finsh, &gPlayerAnim_link_fighter_normal_kiru_finsh_end,
      &gPlayerAnim_link_anchor_normal_kiru_finsh_endR, 0, 5 },
    /* PLAYER_MELEEATKTYPE_FORWARD_COMBO_2H */
    { &gPlayerAnim_link_fighter_Lnormal_kiru_finsh, &gPlayerAnim_link_fighter_Lnormal_kiru_finsh_end,
      &gPlayerAnim_link_anchor_Lnormal_kiru_finsh_endR, 1, 7 },
    /* PLAYER_MELEEATKTYPE_LEFT_SLASH_1H */
    { &gPlayerAnim_link_fighter_Lside_kiru, &gPlayerAnim_link_fighter_Lside_kiru_end,
      &gPlayerAnim_link_anchor_Lside_kiru_endR, 1, 4 },
    /* PLAYER_MELEEATKTYPE_LEFT_SLASH_2H */
    { &gPlayerAnim_link_fighter_LLside_kiru, &gPlayerAnim_link_fighter_LLside_kiru_end,
      &gPlayerAnim_link_anchor_LLside_kiru_endL, 0, 5 },
    /* PLAYER_MELEEATKTYPE_LEFT_COMBO_1H */
    { &gPlayerAnim_link_fighter_Lside_kiru_finsh, &gPlayerAnim_link_fighter_Lside_kiru_finsh_end,
      &gPlayerAnim_link_anchor_Lside_kiru_finsh_endR, 2, 8 },
    /* PLAYER_MELEEATKTYPE_LEFT_COMBO_2H */
    { &gPlayerAnim_link_fighter_LLside_kiru_finsh, &gPlayerAnim_link_fighter_LLside_kiru_finsh_end,
      &gPlayerAnim_link_anchor_LLside_kiru_finsh_endR, 3, 8 },
    /* PLAYER_MELEEATKTYPE_RIGHT_SLASH_1H */
    { &gPlayerAnim_link_fighter_Rside_kiru, &gPlayerAnim_link_fighter_Rside_kiru_end,
      &gPlayerAnim_link_anchor_Rside_kiru_endR, 0, 4 },
    /* PLAYER_MELEEATKTYPE_RIGHT_SLASH_2H */
    { &gPlayerAnim_link_fighter_LRside_kiru, &gPlayerAnim_link_fighter_LRside_kiru_end,
      &gPlayerAnim_link_anchor_LRside_kiru_endR, 0, 5 },
    /* PLAYER_MELEEATKTYPE_RIGHT_COMBO_1H */
    { &gPlayerAnim_link_fighter_Rside_kiru_finsh, &gPlayerAnim_link_fighter_Rside_kiru_finsh_end,
      &gPlayerAnim_link_anchor_Rside_kiru_finsh_endR, 0, 6 },
    /* PLAYER_MELEEATKTYPE_RIGHT_COMBO_2H */
    { &gPlayerAnim_link_fighter_LRside_kiru_finsh, &gPlayerAnim_link_fighter_LRside_kiru_finsh_end,
      &gPlayerAnim_link_anchor_LRside_kiru_finsh_endL, 1, 5 },
    /* PLAYER_MELEEATKTYPE_STAB_1H */
    { &gPlayerAnim_link_fighter_pierce_kiru, &gPlayerAnim_link_fighter_pierce_kiru_end,
      &gPlayerAnim_link_anchor_pierce_kiru_endR, 0, 3 },
    /* PLAYER_MELEEATKTYPE_STAB_2H */
    { &gPlayerAnim_link_fighter_Lpierce_kiru, &gPlayerAnim_link_fighter_Lpierce_kiru_end,
      &gPlayerAnim_link_anchor_Lpierce_kiru_endL, 0, 3 },
    /* PLAYER_MELEEATKTYPE_STAB_COMBO_1H */
    { &gPlayerAnim_link_fighter_pierce_kiru_finsh, &gPlayerAnim_link_fighter_pierce_kiru_finsh_end,
      &gPlayerAnim_link_anchor_pierce_kiru_finsh_endR, 1, 9 },
    /* PLAYER_MELEEATKTYPE_STAB_COMBO_2H */
    { &gPlayerAnim_link_fighter_Lpierce_kiru_finsh, &gPlayerAnim_link_fighter_Lpierce_kiru_finsh_end,
      &gPlayerAnim_link_anchor_Lpierce_kiru_finsh_endR, 1, 8 },
    /* PLAYER_MELEEATKTYPE_FLIPSLASH_START */
    { &gPlayerAnim_link_fighter_jump_rollkiru, &gPlayerAnim_link_fighter_jump_kiru_finsh,
      &gPlayerAnim_link_fighter_jump_kiru_finsh, 1, 10 },
    /* PLAYER_MELEEATKTYPE_JUMPSLASH_START */
    { &gPlayerAnim_link_fighter_Lpower_jump_kiru, &gPlayerAnim_link_fighter_Lpower_jump_kiru_hit,
      &gPlayerAnim_link_fighter_Lpower_jump_kiru_hit, 1, 11 },
    /* PLAYER_MELEEATKTYPE_FLIPSLASH_FINISH */
    { &gPlayerAnim_link_fighter_jump_kiru_finsh, &gPlayerAnim_link_fighter_jump_kiru_finsh_end,
      &gPlayerAnim_link_fighter_jump_kiru_finsh_end, 1, 2 },
    /* PLAYER_MELEEATKTYPE_JUMPSLASH_FINISH */
    { &gPlayerAnim_link_fighter_Lpower_jump_kiru_hit, &gPlayerAnim_link_fighter_Lpower_jump_kiru_end,
      &gPlayerAnim_link_fighter_Lpower_jump_kiru_end, 1, 2 },
    /* PLAYER_MELEEATKTYPE_BACKSLASH_RIGHT */
    { &gPlayerAnim_link_fighter_turn_kiruR, &gPlayerAnim_link_fighter_turn_kiruR_end,
      &gPlayerAnim_link_fighter_turn_kiruR_end, 1, 5 },
    /* PLAYER_MELEEATKTYPE_BACKSLASH_LEFT */
    { &gPlayerAnim_link_fighter_turn_kiruL, &gPlayerAnim_link_fighter_turn_kiruL_end,
      &gPlayerAnim_link_fighter_turn_kiruL_end, 1, 4 },
    /* PLAYER_MELEEATKTYPE_HAMMER_FORWARD */
    { &gPlayerAnim_link_hammer_hit, &gPlayerAnim_link_hammer_hit_end, &gPlayerAnim_link_hammer_hit_endR, 3, 10 },
    /* PLAYER_MELEEATKTYPE_HAMMER_SIDE */
    { &gPlayerAnim_link_hammer_side_hit, &gPlayerAnim_link_hammer_side_hit_end, &gPlayerAnim_link_hammer_side_hit_endR,
      2, 11 },
    /* PLAYER_MELEEATKTYPE_SPIN_ATTACK_1H */
    { &gPlayerAnim_link_fighter_rolling_kiru, &gPlayerAnim_link_fighter_rolling_kiru_end,
      &gPlayerAnim_link_anchor_rolling_kiru_endR, 0, 12 },
    /* PLAYER_MELEEATKTYPE_SPIN_ATTACK_2H */
    { &gPlayerAnim_link_fighter_Lrolling_kiru, &gPlayerAnim_link_fighter_Lrolling_kiru_end,
      &gPlayerAnim_link_anchor_Lrolling_kiru_endR, 0, 15 },
    /* PLAYER_MELEEATKTYPE_BIG_SPIN_1H */
    { &gPlayerAnim_link_fighter_Wrolling_kiru, &gPlayerAnim_link_fighter_Wrolling_kiru_end,
      &gPlayerAnim_link_anchor_rolling_kiru_endR, 0, 16 },
    /* PLAYER_MELEEATKTYPE_BIG_SPIN_2H */
    { &gPlayerAnim_link_fighter_Wrolling_kiru, &gPlayerAnim_link_fighter_Wrolling_kiru_end,
      &gPlayerAnim_link_anchor_Lrolling_kiru_endR, 0, 16 },
};

static LinkAnimationHeader* sSpinAttackAnims2[] = {
    &gPlayerAnim_link_fighter_power_kiru_start,
    &gPlayerAnim_link_fighter_Lpower_kiru_start,
};

static LinkAnimationHeader* sSpinAttackAnims1[] = {
    &gPlayerAnim_link_fighter_power_kiru_startL,
    &gPlayerAnim_link_fighter_Lpower_kiru_start,
};

static LinkAnimationHeader* sSpinAttackChargeAnims[] = {
    &gPlayerAnim_link_fighter_power_kiru_wait,
    &gPlayerAnim_link_fighter_Lpower_kiru_wait,
};

static LinkAnimationHeader* sCancelSpinAttackChargeAnims[] = {
    &gPlayerAnim_link_fighter_power_kiru_wait_end,
    &gPlayerAnim_link_fighter_Lpower_kiru_wait_end,
};

static LinkAnimationHeader* sSpinAttackChargeWalkAnims[] = {
    &gPlayerAnim_link_fighter_power_kiru_walk,
    &gPlayerAnim_link_fighter_Lpower_kiru_walk,
};

static LinkAnimationHeader* sSpinAttackChargeSidewalkAnims[] = {
    &gPlayerAnim_link_fighter_power_kiru_side_walk,
    &gPlayerAnim_link_fighter_Lpower_kiru_side_walk,
};

static u8 D_80854380[2] = { 0x18, 0x19 };
static u8 D_80854384[2] = { 0x1A, 0x1B };

static u16 sUseItemButtons[] = { BTN_B, BTN_CLEFT, BTN_CDOWN, BTN_CRIGHT, BTN_DUP, BTN_DDOWN, BTN_DLEFT, BTN_DRIGHT };

static u8 sMagicSpellCosts[] = { 12, 24, 24, 12, 24, 12 };

static u16 sFpsItemReadySfx[] = { NA_SE_IT_BOW_DRAW, NA_SE_IT_SLING_DRAW, NA_SE_IT_HOOKSHOT_READY };

static u8 sMagicArrowCosts[] = { 4, 4, 8 };

static LinkAnimationHeader* sRightDefendStandingAnims[] = {
    &gPlayerAnim_link_anchor_waitR2defense,
    &gPlayerAnim_link_anchor_waitR2defense_long,
};

static LinkAnimationHeader* sLeftDefendStandingAnims[] = {
    &gPlayerAnim_link_anchor_waitL2defense,
    &gPlayerAnim_link_anchor_waitL2defense_long,
};

static LinkAnimationHeader* sLeftStandingDeflectWithShieldAnims[] = {
    &gPlayerAnim_link_anchor_defense_hit,
    &gPlayerAnim_link_anchor_defense_long_hitL,
};

static LinkAnimationHeader* sRightStandingDeflectWithShieldAnims[] = {
    &gPlayerAnim_link_anchor_defense_hit,
    &gPlayerAnim_link_anchor_defense_long_hitR,
};

static LinkAnimationHeader* sDeflectWithShieldAnims[] = {
    &gPlayerAnim_link_normal_defense_hit,
    &gPlayerAnim_link_fighter_defense_long_hit,
};

static LinkAnimationHeader* sReadyFpsItemWhileWalkingAnims[] = {
    &gPlayerAnim_link_bow_walk2ready,
    &gPlayerAnim_link_hook_walk2ready,
};

static LinkAnimationHeader* sReadyFpsItemAnims[] = {
    &gPlayerAnim_link_bow_bow_wait,
    &gPlayerAnim_link_hook_wait,
};

// return type can't be void due to regalloc in Player_CheckNoDebugModeCombo
s32 Player_StopMovement(Player* this) {
    this->actor.speedXZ = 0.0f;
    this->linearVelocity = 0.0f;
}

// return type can't be void due to regalloc in func_8083F72C
s32 Player_ClearAttentionModeAndStopMoving(Player* this) {
    Player_StopMovement(this);
    this->attentionMode = 0;
}

s32 Player_CheckActorTalkRequested(PlayState* play) {
    Player* this = GET_PLAYER(play);

    return CHECK_FLAG_ALL(this->actor.flags, ACTOR_FLAG_PLAYER_TALKED_TO);
}

void Player_PlayAnimOnce(PlayState* play, Player* this, LinkAnimationHeader* anim) {
    LinkAnimation_PlayOnce(play, &this->skelAnime, anim);
}

void Player_PlayAnimLoop(PlayState* play, Player* this, LinkAnimationHeader* anim) {
    LinkAnimation_PlayLoop(play, &this->skelAnime, anim);
}

void Player_PlayAnimLoopSlowed(PlayState* play, Player* this, LinkAnimationHeader* anim) {
    LinkAnimation_PlayLoopSetSpeed(play, &this->skelAnime, anim, 2.0f / 3.0f);
}

void Player_PlayAnimOnceSlowed(PlayState* play, Player* this, LinkAnimationHeader* anim) {
    LinkAnimation_PlayOnceSetSpeed(play, &this->skelAnime, anim, 2.0f / 3.0f);
}

void Player_AddRootYawToShapeYaw(Player* this) {
    this->actor.shape.rot.y += this->skelAnime.jointTable[1].y;
    this->skelAnime.jointTable[1].y = 0;
}

void Player_InactivateMeleeWeapon(Player* this) {
    this->stateFlags2 &= ~PLAYER_STATE2_RELEASING_SPIN_ATTACK;
    this->swordState = 0;
    this->meleeWeaponInfo[0].active = this->meleeWeaponInfo[1].active = this->meleeWeaponInfo[2].active = 0;
}

void Player_ResetSubCam(PlayState* play, Player* this) {
    Camera* camera;

    if (this->subCamId != SUBCAM_NONE) {
        camera = play->cameraPtrs[this->subCamId];
        if ((camera != NULL) && (camera->csId == 1100)) {
            OnePointCutscene_EndCutscene(play, this->subCamId);
            this->subCamId = SUBCAM_NONE;
        }
    }

    this->stateFlags2 &= ~(PLAYER_STATE2_DIVING | PLAYER_STATE2_ENABLE_DIVE_CAMERA_AND_TIMER);
}

void Player_DetatchHeldActor(PlayState* play, Player* this) {
    Actor* heldActor = this->heldActor;

    if ((heldActor != NULL) && !Player_HoldsHookshot(this)) {
        this->actor.child = NULL;
        this->heldActor = NULL;
        this->interactRangeActor = NULL;
        heldActor->parent = NULL;
        this->stateFlags1 &= ~PLAYER_STATE1_HOLDING_ACTOR;
    }

    if (Player_GetExplosiveHeld(this) >= 0) {
        Player_ChangeItem(play, this, PLAYER_IA_NONE);
        this->heldItemId = ITEM_NONE_FE;
    }
}

void Player_ResetAttributes(PlayState* play, Player* this) {
    if ((this->stateFlags1 & PLAYER_STATE1_HOLDING_ACTOR) && (this->heldActor == NULL)) {
        if (this->interactRangeActor != NULL) {
            if (this->getItemId == GI_NONE) {
                this->stateFlags1 &= ~PLAYER_STATE1_HOLDING_ACTOR;
                this->interactRangeActor = NULL;
            }
        } else {
            this->stateFlags1 &= ~PLAYER_STATE1_HOLDING_ACTOR;
        }
    }

    Player_InactivateMeleeWeapon(this);
    this->attentionMode = 0;

    Player_ResetSubCam(play, this);
    func_8005B1A4(Play_GetCamera(play, 0));

    this->stateFlags1 &= ~(PLAYER_STATE1_HANGING_FROM_LEDGE_SLIP | PLAYER_STATE1_CLIMBING_ONTO_LEDGE |
                           PLAYER_STATE1_IN_FIRST_PERSON_MODE | PLAYER_STATE1_CLIMBING);
    this->stateFlags2 &=
        ~(PLAYER_STATE2_MOVING_PUSH_PULL_WALL | PLAYER_STATE2_RESTRAINED_BY_ENEMY | PLAYER_STATE2_INSIDE_CRAWLSPACE);

    this->actor.shape.rot.x = 0;
    this->actor.shape.yOffset = 0.0f;

    this->slashCounter = this->comboTimer = 0;
}

s32 Player_UnequipItem(PlayState* play, Player* this) {
    if (this->heldItemAction >= PLAYER_IA_FISHING_POLE) {
        Player_UseItem(play, this, ITEM_NONE);
        return 1;
    } else {
        return 0;
    }
}

void Player_ResetAttributesAndHeldActor(PlayState* play, Player* this) {
    Player_ResetAttributes(play, this);
    Player_DetatchHeldActor(play, this);
}

s32 Player_MashTimerThresholdExceeded(Player* this, s32 arg1, s32 arg2) {
    s16 temp = this->analogStickAngle - sAnalogStickAngle;

    this->genericTimer += arg1 + (s16)(ABS(temp) * fabsf(sAnalogStickDistance) * 2.5415802156203426e-06f);

    if (CHECK_BTN_ANY(sControlInput->press.button, BTN_A | BTN_B)) {
        this->genericTimer += 5;
    }

    return this->genericTimer > arg2;
}

void Player_SetFreezeFlashTimer(PlayState* play) {
    if (play->actorCtx.freezeFlashTimer == 0) {
        play->actorCtx.freezeFlashTimer = 1;
    }
}

void Player_RequestRumble(Player* this, s32 arg1, s32 arg2, s32 arg3, s32 arg4) {
    if (this->actor.category == ACTORCAT_PLAYER) {
        func_800AA000(arg4, arg1, arg2, arg3);
    }
}

void Player_PlayVoiceSfxForAge(Player* this, u16 sfxId) {
    if (this->actor.category == ACTORCAT_PLAYER) {
        func_8002F7DC(&this->actor, sfxId + this->ageProperties->unk_92);
    } else {
        func_800F4190(&this->actor.projectedPos, sfxId);
    }
}

void Player_StopInterruptableSfx(Player* this) {
    u16* entry = &sInterruptableSfx[0];
    s32 i;

    for (i = 0; i < 4; i++) {
        Audio_StopSfxById((u16)(*entry + this->ageProperties->unk_92));
        entry++;
    }
}

u16 Player_GetMoveSfx(Player* this, u16 sfxId) {
    return sfxId + this->surfaceMaterial;
}

void Player_PlayMoveSfx(Player* this, u16 sfxId) {
    func_8002F7DC(&this->actor, Player_GetMoveSfx(this, sfxId));
}

u16 Player_GetMoveSfxForAge(Player* this, u16 sfxId) {
    return sfxId + this->surfaceMaterial + this->ageProperties->unk_94;
}

void Player_PlayMoveSfxForAge(Player* this, u16 sfxId) {
    func_8002F7DC(&this->actor, Player_GetMoveSfxForAge(this, sfxId));
}

void Player_PlayWalkSfx(Player* this, f32 arg1) {
    s32 sfxId;

    if (this->currentBoots == PLAYER_BOOTS_IRON) {
        sfxId = NA_SE_PL_WALK_HEAVYBOOTS;
    } else {
        sfxId = Player_GetMoveSfxForAge(this, NA_SE_PL_WALK_GROUND);
    }

    func_800F4010(&this->actor.projectedPos, sfxId, arg1);
    // Gameplay stats: Count footsteps
    // Only count while game isn't complete and don't count Link's idle animations or crawling in crawlspaces
    if (!gSaveContext.sohStats.gameComplete && !(this->stateFlags2 & PLAYER_STATE2_IDLING) &&
        !(this->stateFlags2 & PLAYER_STATE2_INSIDE_CRAWLSPACE)) {
        gSaveContext.sohStats.count[COUNT_STEPS]++;
    }
}

void Player_PlayJumpSfx(Player* this) {
    s32 sfxId;

    if (this->currentBoots == PLAYER_BOOTS_IRON) {
        sfxId = NA_SE_PL_JUMP_HEAVYBOOTS;
    } else {
        sfxId = Player_GetMoveSfxForAge(this, NA_SE_PL_JUMP);
    }

    func_8002F7DC(&this->actor, sfxId);
}

void Player_PlayLandingSfx(Player* this) {
    s32 sfxId;

    if (this->currentBoots == PLAYER_BOOTS_IRON) {
        sfxId = NA_SE_PL_LAND_HEAVYBOOTS;
    } else {
        sfxId = Player_GetMoveSfxForAge(this, NA_SE_PL_LAND);
    }

    func_8002F7DC(&this->actor, sfxId);
}

void Player_PlayReactableSfx(Player* this, u16 sfxId) {
    func_8002F7DC(&this->actor, sfxId);
    this->stateFlags2 |= PLAYER_STATE2_MAKING_REACTABLE_NOISE;
}

void Player_PlayAnimSfx(Player* this, PlayerAnimSfxEntry* entry) {
    s32 data;
    s32 flags;
    u32 cont;
    s32 pad;

    do {
        data = ABS(entry->field);
        flags = data & 0x7800;
        if (LinkAnimation_OnFrame(&this->skelAnime, fabsf(data & 0x7FF))) {
            if (flags == 0x800) {
                func_8002F7DC(&this->actor, entry->sfxId);
            } else if (flags == 0x1000) {
                Player_PlayMoveSfx(this, entry->sfxId);
            } else if (flags == 0x1800) {
                Player_PlayMoveSfxForAge(this, entry->sfxId);
            } else if (flags == 0x2000) {
                Player_PlayVoiceSfxForAge(this, entry->sfxId);
            } else if (flags == 0x2800) {
                Player_PlayLandingSfx(this);
            } else if (flags == 0x3000) {
                Player_PlayWalkSfx(this, 6.0f);
            } else if (flags == 0x3800) {
                Player_PlayJumpSfx(this);
            } else if (flags == 0x4000) {
                Player_PlayWalkSfx(this, 0.0f);
            } else if (flags == 0x4800) {
                func_800F4010(&this->actor.projectedPos, this->ageProperties->unk_94 + NA_SE_PL_WALK_LADDER, 0.0f);
            }
        }
        cont = (entry->field >= 0);
        entry++;
    } while (cont);
}

void Player_ChangeAnimMorphToLastFrame(PlayState* play, Player* this, LinkAnimationHeader* anim) {
    LinkAnimation_Change(play, &this->skelAnime, anim, 1.0f, 0.0f, Animation_GetLastFrame(anim), ANIMMODE_ONCE,
                         -6.0f);
}

void Player_ChangeAnimSlowedMorphToLastFrame(PlayState* play, Player* this, LinkAnimationHeader* anim) {
    LinkAnimation_Change(play, &this->skelAnime, anim, 2.0f / 3.0f, 0.0f, Animation_GetLastFrame(anim),
                         ANIMMODE_ONCE, -6.0f);
}

void Player_ChangeAnimShortMorphLoop(PlayState* play, Player* this, LinkAnimationHeader* anim) {
    LinkAnimation_Change(play, &this->skelAnime, anim, 1.0f, 0.0f, 0.0f, ANIMMODE_LOOP, -6.0f);
}

void Player_ChangeAnimOnce(PlayState* play, Player* this, LinkAnimationHeader* anim) {
    LinkAnimation_Change(play, &this->skelAnime, anim, 1.0f, 0.0f, 0.0f, ANIMMODE_ONCE, 0.0f);
}

void Player_ChangeAnimLongMorphLoop(PlayState* play, Player* this, LinkAnimationHeader* anim) {
    LinkAnimation_Change(play, &this->skelAnime, anim, 1.0f, 0.0f, 0.0f, ANIMMODE_LOOP, -16.0f);
}

s32 Player_LoopAnimContinuously(PlayState* play, Player* this, LinkAnimationHeader* anim) {
    if (LinkAnimation_Update(play, &this->skelAnime)) {
        Player_PlayAnimLoop(play, this, anim);
        return 1;
    } else {
        return 0;
    }
}

void Player_AnimUpdatePrevTranslRot(Player* this) {
    this->skelAnime.prevTransl = this->skelAnime.baseTransl;
    this->skelAnime.prevRot = this->actor.shape.rot.y;
}

void Player_AnimUpdatePrevTranslRotApplyAgeScale(Player* this) {
    Player_AnimUpdatePrevTranslRot(this);
    this->skelAnime.prevTransl.x *= this->ageProperties->unk_08;
    this->skelAnime.prevTransl.y *= this->ageProperties->unk_08;
    this->skelAnime.prevTransl.z *= this->ageProperties->unk_08;
}

void Player_ClearRootLimbPosY(Player* this) {
    this->skelAnime.jointTable[1].y = 0;
}

void Player_EndAnimMovement(Player* this) {
    if (this->skelAnime.moveFlags != 0) {
        Player_AddRootYawToShapeYaw(this);
        this->skelAnime.jointTable[0].x = this->skelAnime.baseTransl.x;
        this->skelAnime.jointTable[0].z = this->skelAnime.baseTransl.z;
        if (this->skelAnime.moveFlags & 8) {
            if (this->skelAnime.moveFlags & 2) {
                this->skelAnime.jointTable[0].y = this->skelAnime.prevTransl.y;
            }
        } else {
            this->skelAnime.jointTable[0].y = this->skelAnime.baseTransl.y;
        }
        Player_AnimUpdatePrevTranslRot(this);
        this->skelAnime.moveFlags = 0;
    }
}

void Player_UpdateAnimMovement(Player* this, s32 flags) {
    Vec3f pos;

    this->skelAnime.moveFlags = flags;
    this->skelAnime.prevTransl = this->skelAnime.baseTransl;
    SkelAnime_UpdateTranslation(&this->skelAnime, &pos, this->actor.shape.rot.y);

    if (flags & 1) {
        if (!LINK_IS_ADULT) {
            pos.x *= 0.64f;
            pos.z *= 0.64f;
        }
        this->actor.world.pos.x += pos.x * this->actor.scale.x;
        this->actor.world.pos.z += pos.z * this->actor.scale.z;
    }

    if (flags & 2) {
        if (!(flags & 4)) {
            pos.y *= this->ageProperties->unk_08;
        }
        this->actor.world.pos.y += pos.y * this->actor.scale.y;
    }

    Player_AddRootYawToShapeYaw(this);
}

void Player_SetupAnimMovement(PlayState* play, Player* this, s32 flags) {
    if (flags & 0x200) {
        Player_AnimUpdatePrevTranslRotApplyAgeScale(this);
    } else if ((flags & 0x100) || (this->skelAnime.moveFlags != 0)) {
        Player_AnimUpdatePrevTranslRot(this);
    } else {
        this->skelAnime.prevTransl = this->skelAnime.jointTable[0];
        this->skelAnime.prevRot = this->actor.shape.rot.y;
    }

    this->skelAnime.moveFlags = flags;
    Player_StopMovement(this);
    AnimationContext_DisableQueue(play);
}

void Player_PlayAnimOnceWithMovementSetSpeed(PlayState* play, Player* this, LinkAnimationHeader* anim,
                                             s32 flags, f32 playbackSpeed) {
    LinkAnimation_PlayOnceSetSpeed(play, &this->skelAnime, anim, playbackSpeed);
    Player_SetupAnimMovement(play, this, flags);
}

void Player_PlayAnimOnceWithMovement(PlayState* play, Player* this, LinkAnimationHeader* anim, s32 flags) {
    Player_PlayAnimOnceWithMovementSetSpeed(play, this, anim, flags, 1.0f);
}

void Player_PlayAnimOnceWithMovementSlowed(PlayState* play, Player* this, LinkAnimationHeader* anim,
                                           s32 flags) {
    Player_PlayAnimOnceWithMovementSetSpeed(play, this, anim, flags, 2.0f / 3.0f);
}

void Player_PlayAnimOnceWithMovementPresetFlagsSlowed(PlayState* play, Player* this,
                                                      LinkAnimationHeader* anim) {
    Player_PlayAnimOnceWithMovementSlowed(play, this, anim, 0x1C);
}

void Player_PlayAnimLoopWithMovementSetSpeed(PlayState* play, Player* this, LinkAnimationHeader* anim,
                                             s32 flags, f32 playbackSpeed) {
    LinkAnimation_PlayLoopSetSpeed(play, &this->skelAnime, anim, playbackSpeed);
    Player_SetupAnimMovement(play, this, flags);
}

void Player_PlayAnimLoopWithMovement(PlayState* play, Player* this, LinkAnimationHeader* anim, s32 flags) {
    Player_PlayAnimLoopWithMovementSetSpeed(play, this, anim, flags, 1.0f);
}

void Player_PlayAnimLoopWithMovementSlowed(PlayState* play, Player* this, LinkAnimationHeader* anim,
                                           s32 flags) {
    Player_PlayAnimLoopWithMovementSetSpeed(play, this, anim, flags, 2.0f / 3.0f);
}

void Player_PlayAnimLoopWithMovementPresetFlagsSlowed(PlayState* play, Player* this,
                                                      LinkAnimationHeader* anim) {
    Player_PlayAnimLoopWithMovementSlowed(play, this, anim, 0x1C);
}

void Player_StoreAnalogStickInput(PlayState* play, Player* this) {
    s8 scaledStickAngle;
    s8 scaledCamOffsetStickAngle;

    this->analogStickDistance = sAnalogStickDistance;
    this->analogStickAngle = sAnalogStickAngle;

    func_80077D10(&sAnalogStickDistance, &sAnalogStickAngle, sControlInput);

    sCameraOffsetAnalogStickAngle = Camera_GetInputDirYaw(GET_ACTIVE_CAM(play)) + sAnalogStickAngle;

    this->inputFrameCounter = (this->inputFrameCounter + 1) % 4;

    if (sAnalogStickDistance < 55.0f) {
        scaledCamOffsetStickAngle = -1;
        scaledStickAngle = -1;
    } else {
        scaledStickAngle = (u16)(sAnalogStickAngle + 0x2000) >> 9;
        scaledCamOffsetStickAngle =
            (u16)((s16)(sCameraOffsetAnalogStickAngle - this->actor.shape.rot.y) + 0x2000) >> 14;
    }

    this->analogStickInputs[this->inputFrameCounter] = scaledStickAngle;
    this->relativeAnalogStickInputs[this->inputFrameCounter] = scaledCamOffsetStickAngle;
}

void Player_PlayAnimOnceWithWaterInfluence(PlayState* play, Player* this, LinkAnimationHeader* linkAnim) {
    LinkAnimation_PlayOnceSetSpeed(play, &this->skelAnime, linkAnim, sWaterSpeedScale);
}

s32 Player_IsSwimming(Player* this) {
    return (this->stateFlags1 & PLAYER_STATE1_SWIMMING) && (this->currentBoots != PLAYER_BOOTS_IRON);
}

s32 Player_IsAimingBoomerang(Player* this) {
    return (this->stateFlags1 & PLAYER_STATE1_AIMING_BOOMERANG);
}

void Player_SetGetItemDrawIdPlusOne(Player* this, PlayState* play) {
    GetItemEntry giEntry;
    if (this->getItemEntry.objectId == OBJECT_INVALID || (this->getItemId != this->getItemEntry.getItemId)) {
        giEntry = ItemTable_Retrieve(this->getItemId);
    }
    else {
        giEntry = this->getItemEntry;
    }

    this->giDrawIdPlusOne = ABS(giEntry.gi);
}

static LinkAnimationHeader* Player_GetStandingStillAnim(Player* this) {
    return GET_PLAYER_ANIM(PLAYER_ANIMGROUP_STANDING_STILL, this->modelAnimType);
}

s32 Player_IsPlayingIdleAnim(Player* this) {
    LinkAnimationHeader** entry;
    s32 i;

    if (Player_GetStandingStillAnim(this) != this->skelAnime.animation) {
        for (i = 0, entry = &sIdleAnims[0][0]; i < 28; i++, entry++) {
            if (this->skelAnime.animation == *entry) {
                return i + 1;
            }
        }
        return 0;
    }

    return -1;
}

void Player_PlayIdleAnimSfx(Player* this, s32 arg1) {
    if (D_80853E7C[arg1] != 0) {
        Player_PlayAnimSfx(this, D_80853E50[D_80853E7C[arg1] - 1]);
    }
}

LinkAnimationHeader* Player_GetRunningAnim(Player* this) {
    if (this->runDamageTimer != 0) {
        return GET_PLAYER_ANIM(PLAYER_ANIMGROUP_RUNNING_DAMAGED, this->modelAnimType);
    } else if (!(this->stateFlags1 & (PLAYER_STATE1_SWIMMING | PLAYER_STATE1_IN_CUTSCENE)) &&
               (this->currentBoots == PLAYER_BOOTS_IRON)) {
        return GET_PLAYER_ANIM(PLAYER_ANIMGROUP_IRON_BOOTS, this->modelAnimType);
    } else {
        return GET_PLAYER_ANIM(PLAYER_ANIMGROUP_RUNNING, this->modelAnimType);
    }
}

s32 Player_IsAimingReadyBoomerang(Player* this) {
    return Player_IsAimingBoomerang(this) && (this->fpsItemTimer != 0);
}

LinkAnimationHeader* Player_GetFightingRightAnim(Player* this) {
    if (Player_IsAimingReadyBoomerang(this)) {
        return &gPlayerAnim_link_boom_throw_waitR;
    } else {
        return GET_PLAYER_ANIM(PLAYER_ANIMGROUP_FIGHTING_RIGHT_OF_ENEMY, this->modelAnimType);
    }
}

LinkAnimationHeader* Player_GetFightingLeftAnim(Player* this) {
    if (Player_IsAimingReadyBoomerang(this)) {
        return &gPlayerAnim_link_boom_throw_waitL;
    } else {
        return GET_PLAYER_ANIM(PLAYER_ANIMGROUP_FIGHTING_LEFT_OF_ENEMY, this->modelAnimType);
    }
}

LinkAnimationHeader* Player_GetEndSidewalkAnim(Player* this) {
    if (Actor_PlayerIsAimingReadyFpsItem(this)) {
        return &gPlayerAnim_link_bow_side_walk;
    } else {
        return GET_PLAYER_ANIM(PLAYER_ANIMGROUP_END_SIDEWALKING, this->modelAnimType);
    }
}

LinkAnimationHeader* Player_GetSidewalkRightAnim(Player* this) {
    if (Player_IsAimingReadyBoomerang(this)) {
        return &gPlayerAnim_link_boom_throw_side_walkR;
    } else {
        return GET_PLAYER_ANIM(PLAYER_ANIMGROUP_SIDEWALKING_RIGHT, this->modelAnimType);
    }
}

LinkAnimationHeader* Player_GetSidewalkLeftAnim(Player* this) {
    if (Player_IsAimingReadyBoomerang(this)) {
        return &gPlayerAnim_link_boom_throw_side_walkL;
    } else {
        return GET_PLAYER_ANIM(PLAYER_ANIMGROUP_SIDEWALKING_LEFT, this->modelAnimType);
    }
}

void Player_SetUpperActionFunc(Player* this, PlayerUpperActionFunc arg1) {
    this->upperActionFunc = arg1;
    this->fpsItemShootState = 0;
    this->upperInterpWeight = 0.0f;
    Player_StopInterruptableSfx(this);
}

void Player_SetupChangeItemAnim(PlayState* play, Player* this, s8 actionParam) {
    LinkAnimationHeader* current = this->skelAnime.animation;
    LinkAnimationHeader** iter = sPlayerAnimations + this->modelAnimType;
    u32 i;

    this->stateFlags1 &= ~(PLAYER_STATE1_AIMING_FPS_ITEM | PLAYER_STATE1_AIMING_BOOMERANG);

    for (i = 0; i < PLAYER_ANIMGROUP_MAX; i++) {
        if (current == *iter) {
            break;
        }
        iter += PLAYER_ANIMTYPE_MAX;
    }

    Player_ChangeItem(play, this, actionParam);

    if (i < PLAYER_ANIMGROUP_MAX) {
        this->skelAnime.animation = GET_PLAYER_ANIM(i, this->modelAnimType);
    }
}

s8 Player_ItemToActionParam(s32 item) {
    if (item >= ITEM_NONE_FE) {
        return PLAYER_IA_NONE;
    } else if (item == ITEM_LAST_USED) {
        return PLAYER_IA_LAST_USED;
    } else if (item == ITEM_FISHING_POLE) {
        return PLAYER_IA_FISHING_POLE;
    } else {
        return sItemActionParams[item];
    }
}

void Player_DoNothing(PlayState* play, Player* this) {
}

void Player_SetupDekuStick(PlayState* play, Player* this) {
    this->unk_85C = 1.0f;
}

void Player_DoNothing2(PlayState* play, Player* this) {
}

void Player_SetupBowOrSlingshot(PlayState* play, Player* this) {
    this->stateFlags1 |= PLAYER_STATE1_AIMING_FPS_ITEM;

    if (this->heldItemAction != PLAYER_IA_SLINGSHOT) {
        this->fpsItemType = -1;
    } else {
        this->fpsItemType = -2;
    }
}

void Player_SetupExplosive(PlayState* play, Player* this) {
    s32 explosiveType;
    ExplosiveInfo* explosiveInfo;
    Actor* spawnedActor;

    if (this->stateFlags1 & PLAYER_STATE1_HOLDING_ACTOR) {
        Player_UnequipItem(play, this);
        return;
    }

    explosiveType = Player_GetExplosiveHeld(this);
    explosiveInfo = &sExplosiveInfos[explosiveType];

    spawnedActor = Actor_SpawnAsChild(&play->actorCtx, &this->actor, play, explosiveInfo->actorId,
                                      this->actor.world.pos.x, this->actor.world.pos.y, this->actor.world.pos.z, 0,
                                      this->actor.shape.rot.y, 0, 0);
    if (spawnedActor != NULL) {
        if ((explosiveType != 0) && (play->bombchuBowlingStatus != 0)) {
            if (!CVarGetInteger("gInfiniteAmmo", 0)) {
                play->bombchuBowlingStatus--;
            }
            if (play->bombchuBowlingStatus == 0) {
                play->bombchuBowlingStatus = -1;
            }
        } else {
            Inventory_ChangeAmmo(explosiveInfo->itemId, -1);
        }

        this->interactRangeActor = spawnedActor;
        this->heldActor = spawnedActor;
        this->getItemId = GI_NONE;
        this->getItemEntry = (GetItemEntry)GET_ITEM_NONE;
        this->leftHandRot.y = spawnedActor->shape.rot.y - this->actor.shape.rot.y;
        this->stateFlags1 |= PLAYER_STATE1_HOLDING_ACTOR;
    }
}

void Player_SetupHookshot(PlayState* play, Player* this) {
    this->stateFlags1 |= PLAYER_STATE1_AIMING_FPS_ITEM;
    this->fpsItemType = -3;

    this->heldActor =
        Actor_SpawnAsChild(&play->actorCtx, &this->actor, play, ACTOR_ARMS_HOOK, this->actor.world.pos.x,
                           this->actor.world.pos.y, this->actor.world.pos.z, 0, this->actor.shape.rot.y, 0, 0);
}

void Player_SetupBoomerang(PlayState* play, Player* this) {
    this->stateFlags1 |= PLAYER_STATE1_AIMING_BOOMERANG;
}

void Player_ChangeItem(PlayState* play, Player* this, s8 actionParam) {
    this->fpsItemType = 0;
    this->unk_85C = 0.0f;
    this->unk_858 = 0.0f;

    this->heldItemAction = this->itemAction = actionParam;
    this->modelGroup = this->nextModelGroup;

    this->stateFlags1 &= ~(PLAYER_STATE1_AIMING_FPS_ITEM | PLAYER_STATE1_AIMING_BOOMERANG);

    sItemChangeFuncs[actionParam](play, this);

    Player_SetModelGroup(this, this->modelGroup);
}

void Player_MeleeAttack(Player* this, s32 newSwordState) {
    u16 itemSfx;
    u16 voiceSfx;

    if (this->swordState == 0) {
        if ((this->heldItemAction == PLAYER_IA_SWORD_BGS) && (gSaveContext.swordHealth > 0.0f)) {
            itemSfx = NA_SE_IT_HAMMER_SWING;
        } else {
            itemSfx = NA_SE_IT_SWORD_SWING;
        }

        voiceSfx = NA_SE_VO_LI_SWORD_N;
        if (this->heldItemAction == PLAYER_IA_HAMMER) {
            itemSfx = NA_SE_IT_HAMMER_SWING;
        } else if (this->meleeWeaponAnimation >= 0x18) {
            itemSfx = 0;
            voiceSfx = NA_SE_VO_LI_SWORD_L;
        } else if (this->slashCounter >= 3) {
            itemSfx = NA_SE_IT_SWORD_SWING_HARD;
            voiceSfx = NA_SE_VO_LI_SWORD_L;
        }

        if (itemSfx != 0) {
            Player_PlayReactableSfx(this, itemSfx);
        }

        if ((this->meleeWeaponAnimation < 0x10) || (this->meleeWeaponAnimation >= 0x14)) {
            Player_PlayVoiceSfxForAge(this, voiceSfx);
        }

        if (this->heldItemAction >= PLAYER_IA_SWORD_MASTER && this->heldItemAction <= PLAYER_IA_SWORD_BGS) {
            gSaveContext.sohStats.count[COUNT_SWORD_SWINGS]++;
        }
    }

    this->swordState = newSwordState;
}

s32 Player_IsFriendlyZTargeting(Player* this) {
    if (this->stateFlags1 & (PLAYER_STATE1_FORCE_STRAFING | PLAYER_STATE1_Z_TARGETING_FRIENDLY | PLAYER_STATE1_30)) {
        return 1;
    } else {
        return 0;
    }
}

s32 Player_SetupStartUnfriendlyZTargeting(Player* this) {
    if ((this->targetActor != NULL) && CHECK_FLAG_ALL(this->targetActor->flags, ACTOR_FLAG_TARGETABLE | ACTOR_FLAG_HOSTILE)) {
        this->stateFlags1 |= PLAYER_STATE1_Z_TARGETING_UNFRIENDLY;
        return 1;
    }

    if (this->stateFlags1 & PLAYER_STATE1_Z_TARGETING_UNFRIENDLY) {
        this->stateFlags1 &= ~PLAYER_STATE1_Z_TARGETING_UNFRIENDLY;
        if (this->linearVelocity == 0.0f) {
            this->currentYaw = this->actor.shape.rot.y;
        }
    }

    return 0;
}

s32 Player_IsZTargeting(Player* this) {
    return Player_IsUnfriendlyZTargeting(this) || Player_IsFriendlyZTargeting(this);
}

s32 Player_IsZTargetingSetupStartUnfriendly(Player* this) {
    return Player_SetupStartUnfriendlyZTargeting(this) || Player_IsFriendlyZTargeting(this);
}

void Player_ResetLeftRightBlendWeight(Player* this) {
    this->leftRightBlendWeight = this->leftRightBlendWeightTarget = 0.0f;
}

s32 Player_IsItemValid(Player* this, s32 item) {
    if ((item < ITEM_NONE_FE) && (Player_ItemToActionParam(item) == this->itemAction)) {
        return 1;
    } else {
        return 0;
    }
}

s32 Player_IsWearableMaskValid(s32 item1, s32 actionParam) {
    if ((item1 < ITEM_NONE_FE) && (Player_ItemToActionParam(item1) == actionParam)) {
        return 1;
    } else {
        return 0;
    }
}

s32 Player_GetButtonItem(PlayState* play, s32 index) {
    if (index >= ((CVarGetInteger("gDpadEquips", 0) != 0) ? 8 : 4)) {
        return ITEM_NONE;
    } else if (play->bombchuBowlingStatus != 0) {
        return (play->bombchuBowlingStatus > 0) ? ITEM_BOMBCHU : ITEM_NONE;
    } else if (index == 0) {
        return B_BTN_ITEM;
    } else if (index == 1) {
        return C_BTN_ITEM(0);
    } else if (index == 2) {
        return C_BTN_ITEM(1);
    } else if (index == 3) {
        return C_BTN_ITEM(2);
    } else if (index == 4) {
        return DPAD_ITEM(0);
    } else if (index == 5) {
        return DPAD_ITEM(1);
    } else if (index == 6) {
        return DPAD_ITEM(2);
    } else if (index == 7) {
        return DPAD_ITEM(3);
    }
}

void Player_SetupUseItem(Player* this, PlayState* play) {
    s32 maskActionParam;
    s32 item;
    s32 i;

    if (this->currentMask != PLAYER_MASK_NONE) {
        if (CVarGetInteger("gMMBunnyHood", BUNNY_HOOD_VANILLA) != BUNNY_HOOD_VANILLA) {
            s32 maskItem = this->currentMask - PLAYER_MASK_KEATON + ITEM_MASK_KEATON;
            bool hasOnDpad = false;
            if (CVarGetInteger("gDpadEquips", 0) != 0) {
                for (int buttonIndex = 4; buttonIndex < 8; buttonIndex++) {
                    hasOnDpad |= gSaveContext.equips.buttonItems[buttonIndex] == maskItem;
                }
            }

            if (gSaveContext.equips.buttonItems[0] != maskItem && gSaveContext.equips.buttonItems[1] != maskItem &&
                gSaveContext.equips.buttonItems[2] != maskItem && gSaveContext.equips.buttonItems[3] != maskItem &&
                !hasOnDpad) {
                this->currentMask = sMaskMemory = PLAYER_MASK_NONE;
                Player_PlayReactableSfx(this, NA_SE_PL_CHANGE_ARMS);
            }
        } else {
            maskActionParam = this->currentMask - 1 + PLAYER_IA_MASK_KEATON;
            bool hasOnDpad = false;
            if (CVarGetInteger("gDpadEquips", 0) != 0) {
                for (int buttonIndex = 0; buttonIndex < 4; buttonIndex++) {
                    hasOnDpad |= Player_IsWearableMaskValid(DPAD_ITEM(buttonIndex), maskActionParam);
                }
            }
            if (!Player_IsWearableMaskValid(C_BTN_ITEM(0), maskActionParam) && !Player_IsWearableMaskValid(C_BTN_ITEM(1), maskActionParam) &&
                !Player_IsWearableMaskValid(C_BTN_ITEM(2), maskActionParam) && !hasOnDpad) {
                this->currentMask = PLAYER_MASK_NONE;
            }
        }
    }

    if (!(this->stateFlags1 & (PLAYER_STATE1_HOLDING_ACTOR | PLAYER_STATE1_IN_CUTSCENE)) &&
        !Player_IsShootingHookshot(this)) {
        if (this->itemAction >= PLAYER_IA_FISHING_POLE) {
            bool hasOnDpad = false;
            if (CVarGetInteger("gDpadEquips", 0) != 0) {
                for (int buttonIndex = 0; buttonIndex < 4; buttonIndex++) {
                    hasOnDpad |= Player_IsItemValid(this, DPAD_ITEM(buttonIndex));
                }
            }
            if (!Player_IsItemValid(this, B_BTN_ITEM) && !Player_IsItemValid(this, C_BTN_ITEM(0)) &&
                !Player_IsItemValid(this, C_BTN_ITEM(1)) && !Player_IsItemValid(this, C_BTN_ITEM(2)) && !hasOnDpad) {
                Player_UseItem(play, this, ITEM_NONE);
                return;
            }
        }

        for (i = 0; i < ARRAY_COUNT(sUseItemButtons); i++) {
            if (CHECK_BTN_ALL(sControlInput->press.button, sUseItemButtons[i])) {
                break;
            }
        }

        item = Player_GetButtonItem(play, i);
        if (item >= ITEM_NONE_FE) {
            for (i = 0; i < ARRAY_COUNT(sUseItemButtons); i++) {
                if (CHECK_BTN_ALL(sControlInput->cur.button, sUseItemButtons[i])) {
                    break;
                }
            }

            item = Player_GetButtonItem(play, i);
            if ((item < ITEM_NONE_FE) && (Player_ItemToActionParam(item) == this->heldItemAction)) {
                sUsingItemAlreadyInHand2 = true;
            }
        } else {
            this->heldItemButton = i;
            Player_UseItem(play, this, item);
        }
    }
}

void Player_SetupStartChangeItem(Player* this, PlayState* play) {
    LinkAnimationHeader* anim;
    f32 frameCount;
    f32 startFrame;
    f32 endFrame;
    f32 playSpeed;
    s32 itemChangeAnim;
    s8 actionParam;
    s32 nextAnimType;

    actionParam = Player_ItemToActionParam(this->heldItemId);
    Player_SetUpperActionFunc(this, Player_StartChangeItem);

    nextAnimType = gPlayerModelTypes[this->nextModelGroup][PLAYER_MODELGROUPENTRY_ANIM];
    itemChangeAnim = sAnimtypeToItemChangeAnims[gPlayerModelTypes[this->modelGroup][PLAYER_MODELGROUPENTRY_ANIM]][nextAnimType];
    if ((actionParam == PLAYER_IA_BOTTLE) || (actionParam == PLAYER_IA_BOOMERANG) ||
        ((actionParam == PLAYER_IA_NONE) &&
         ((this->heldItemAction == PLAYER_IA_BOTTLE) || (this->heldItemAction == PLAYER_IA_BOOMERANG)))) {
        itemChangeAnim = (actionParam == PLAYER_IA_NONE) ? -PLAYER_ITEM_CHANGE_LEFT_HAND : PLAYER_ITEM_CHANGE_LEFT_HAND;
    }

    this->itemChangeAnim = ABS(itemChangeAnim);

    anim = sItemChangeAnimsInfo[this->itemChangeAnim].anim;
    if ((anim == &gPlayerAnim_link_normal_fighter2free) && (this->currentShield == PLAYER_SHIELD_NONE)) {
        anim = &gPlayerAnim_link_normal_free2fighter_free;
    }

    frameCount = Animation_GetLastFrame(anim);
    endFrame = frameCount;

    if (itemChangeAnim >= 0) {
        playSpeed = 1.2f;
        startFrame = 0.0f;
    } else {
        endFrame = 0.0f;
        playSpeed = -1.2f;
        startFrame = frameCount;
    }

    if (actionParam != PLAYER_IA_NONE) {
        playSpeed *= 2.0f;
    }

    LinkAnimation_Change(play, &this->skelAnimeUpper, anim, playSpeed, startFrame, endFrame, ANIMMODE_ONCE, 0.0f);

    this->stateFlags1 &= ~PLAYER_STATE1_START_CHANGE_ITEM;
}

void Player_SetupItem(Player* this, PlayState* play) {
    if ((this->actor.category == ACTORCAT_PLAYER) && (CVarGetInteger("gQuickPutaway", 0) || !(this->stateFlags1 & PLAYER_STATE1_START_CHANGE_ITEM)) &&
        ((this->heldItemAction == this->itemAction) || (this->stateFlags1 & PLAYER_STATE1_SHIELDING)) &&
        (gSaveContext.health != 0) && (play->csCtx.state == CS_STATE_IDLE) && (this->csMode == 0) &&
        (play->shootingGalleryStatus == 0) && (play->activeCamera == MAIN_CAM) &&
        (play->sceneLoadFlag != 0x14) && (gSaveContext.timer1State != 10)) {
        Player_SetupUseItem(this, play);
    }

    if (this->stateFlags1 & PLAYER_STATE1_START_CHANGE_ITEM) {
        Player_SetupStartChangeItem(this, play);
    }
}

s32 Player_GetFpsItemAmmo(PlayState* play, Player* this, s32* itemPtr, s32* typePtr) {
    bool useBow = LINK_IS_ADULT;
    if(CVarGetInteger("gBowSlingShotAmmoFix", 0)){
        useBow = this->heldItemAction != PLAYER_IA_SLINGSHOT;
    }
    if (useBow) {
        *itemPtr = ITEM_BOW;
        if (this->stateFlags1 & PLAYER_STATE1_RIDING_HORSE) {
            *typePtr = ARROW_NORMAL_HORSE;
        } else {
            *typePtr = this->heldItemAction - 6;
        }
    } else {
        *itemPtr = ITEM_SLINGSHOT;
        *typePtr = ARROW_SEED;
    }

    if (gSaveContext.minigameState == 1) {
        return play->interfaceCtx.hbaAmmo;
    } else if (play->shootingGalleryStatus != 0) {
        return play->shootingGalleryStatus;
    } else {
        return AMMO(*itemPtr);
    }
}

s32 Player_SetupReadyFpsItemToShoot(Player* this, PlayState* play) {
    s32 item;
    s32 arrowType;
    s32 magicArrowType;

    if ((this->heldItemAction >= PLAYER_IA_BOW_FIRE) && (this->heldItemAction <= PLAYER_IA_BOW_0E) &&
        (gSaveContext.magicState != 0)) {
        func_80078884(NA_SE_SY_ERROR);
    } else {
        Player_SetUpperActionFunc(this, Player_ReadyFpsItemToShoot);

        this->stateFlags1 |= PLAYER_STATE1_READY_TO_SHOOT;
        this->fpsItemTimer = 14;

        if (this->fpsItemType >= 0) {
            func_8002F7DC(&this->actor, sFpsItemReadySfx[ABS(this->fpsItemType) - 1]);

            if (!Player_HoldsHookshot(this) && (Player_GetFpsItemAmmo(play, this, &item, &arrowType) > 0)) {
                // Chaos
                if (CVarGetInteger("gForceNormalArrows", 0)) {
                    arrowType = ARROW_NORMAL;
                }

                magicArrowType = arrowType - ARROW_FIRE;
                
                if (this->fpsItemType >= 0) {
                    if ((magicArrowType >= 0) && (magicArrowType <= 2) &&
                        !func_80087708(play, sMagicArrowCosts[magicArrowType], 0)) {
                        arrowType = ARROW_NORMAL;
                    }

                    this->heldActor = Actor_SpawnAsChild(
                        &play->actorCtx, &this->actor, play, ACTOR_EN_ARROW, this->actor.world.pos.x,
                        this->actor.world.pos.y, this->actor.world.pos.z, 0, this->actor.shape.rot.y, 0, arrowType);
                }
            }
        }

        return 1;
    }

    return 0;
}

void Player_ChangeItemWithSfx(PlayState* play, Player* this) {
    if (this->heldItemAction != PLAYER_IA_NONE) {
        if (Player_GetSwordItemAP(this, this->heldItemAction) >= 0) {
            Player_PlayReactableSfx(this, NA_SE_IT_SWORD_PUTAWAY);
        } else {
            Player_PlayReactableSfx(this, NA_SE_PL_CHANGE_ARMS);
        }
    }

    Player_UseItem(play, this, this->heldItemId);

    if (Player_GetSwordItemAP(this, this->heldItemAction) >= 0) {
        Player_PlayReactableSfx(this, NA_SE_IT_SWORD_PICKOUT);
    } else if (this->heldItemAction != PLAYER_IA_NONE) {
        Player_PlayReactableSfx(this, NA_SE_PL_CHANGE_ARMS);
    }
}

void Player_SetupHeldItemUpperActionFunc(PlayState* play, Player* this) {
    if (Player_StartChangeItem == this->upperActionFunc) {
        Player_ChangeItemWithSfx(play, this);
    }

    Player_SetUpperActionFunc(this, sUpperBodyItemFuncs[this->heldItemAction]);
    this->fpsItemTimer = 0;
    this->idleCounter = 0;
    Player_DetatchHeldActor(play, this);
    this->stateFlags1 &= ~PLAYER_STATE1_START_CHANGE_ITEM;
}

LinkAnimationHeader* Player_GetStandingDefendAnim(PlayState* play, Player* this) {
    Player_SetUpperActionFunc(this, Player_StandingDefend);
    Player_DetatchHeldActor(play, this);

    if (this->leftRightBlendWeight < 0.5f) {
        return sRightDefendStandingAnims[Player_HoldsTwoHandedWeapon(this) &&
                                         !(CVarGetInteger("gShieldTwoHanded", 0) &&
                                           (this->heldItemAction != PLAYER_IA_STICK))];
    } else {
        return sLeftDefendStandingAnims[Player_HoldsTwoHandedWeapon(this) &&
                                        !(CVarGetInteger("gShieldTwoHanded", 0) &&
                                          (this->heldItemAction != PLAYER_IA_STICK))];
    }
}

s32 Player_StartZTargetDefend(PlayState* play, Player* this) {
    LinkAnimationHeader* anim;
    f32 frame;

    if (!(this->stateFlags1 & (PLAYER_STATE1_SHIELDING | PLAYER_STATE1_RIDING_HORSE | PLAYER_STATE1_IN_CUTSCENE)) &&
        (play->shootingGalleryStatus == 0) && (this->heldItemAction == this->itemAction) &&
        (this->currentShield != PLAYER_SHIELD_NONE) && !Player_IsChildWithHylianShield(this) && Player_IsZTargeting(this) &&
        CHECK_BTN_ALL(sControlInput->cur.button, BTN_R)) {

        anim = Player_GetStandingDefendAnim(play, this);
        frame = Animation_GetLastFrame(anim);
        LinkAnimation_Change(play, &this->skelAnimeUpper, anim, 1.0f, frame, frame, ANIMMODE_ONCE, 0.0f);
        func_8002F7DC(&this->actor, NA_SE_IT_SHIELD_POSTURE);

        return 1;
    } else {
        return 0;
    }
}

s32 Player_SetupStartZTargetDefend(Player* this, PlayState* play) {
    if (Player_StartZTargetDefend(play, this)) {
        return 1;
    } else {
        return 0;
    }
}

void Player_SetupEndDefend(Player* this) {
    Player_SetUpperActionFunc(this, Player_EndDefend);

    if (this->itemAction < 0) {
        Player_SetHeldItem(this);
    }

    Animation_Reverse(&this->skelAnimeUpper);
    func_8002F7DC(&this->actor, NA_SE_IT_SHIELD_REMOVE);
}

void Player_SetupChangeItem(PlayState* play, Player* this) {
    ItemChangeAnimInfo* ptr = &sItemChangeAnimsInfo[this->itemChangeAnim];
    f32 itemChangeFrame;

    itemChangeFrame = ptr->unk_04;
    itemChangeFrame = (this->skelAnimeUpper.playSpeed < 0.0f) ? itemChangeFrame - 1.0f : itemChangeFrame;

    if (LinkAnimation_OnFrame(&this->skelAnimeUpper, itemChangeFrame)) {
        Player_ChangeItemWithSfx(play, this);
    }

    Player_SetupStartUnfriendlyZTargeting(this);
}

s32 func_8083499C(Player* this, PlayState* play) {
    if (this->stateFlags1 & PLAYER_STATE1_START_CHANGE_ITEM) {
        Player_SetupStartChangeItem(this, play);
    } else {
        return 0;
    }

    return 1;
}

s32 Player_SetupStartZTargetDefend2(Player* this, PlayState* play) {
    if (Player_StartZTargetDefend(play, this) || func_8083499C(this, play)) {
        return 1;
    } else {
        return 0;
    }
}

s32 Player_StartChangeItem(Player* this, PlayState* play) {
    if (LinkAnimation_Update(play, &this->skelAnimeUpper) ||
        ((Player_ItemToActionParam(this->heldItemId) == this->heldItemAction) &&
         (sUsingItemAlreadyInHand =
              (sUsingItemAlreadyInHand || ((this->modelAnimType != PLAYER_ANIMTYPE_HOLDING_TWO_HAND_WEAPON) &&
                                           (play->shootingGalleryStatus == 0)))))) {
        Player_SetUpperActionFunc(this, sUpperBodyItemFuncs[this->heldItemAction]);
        this->fpsItemTimer = 0;
        this->idleCounter = 0;
        sUsingItemAlreadyInHand2 = sUsingItemAlreadyInHand;
        return this->upperActionFunc(this, play);
    }

    if (Player_IsPlayingIdleAnim(this) != 0) {
        Player_SetupChangeItem(play, this);
        Player_PlayAnimOnce(play, this, Player_GetStandingStillAnim(this));
        this->idleCounter = 0;
    } else {
        Player_SetupChangeItem(play, this);
    }

    return 1;
}

s32 Player_StandingDefend(Player* this, PlayState* play) {
    LinkAnimation_Update(play, &this->skelAnimeUpper);

    if (!CHECK_BTN_ALL(sControlInput->cur.button, BTN_R)) {
        Player_SetupEndDefend(this);
        return 1;
    } else {
        this->stateFlags1 |= PLAYER_STATE1_SHIELDING;
        Player_SetModelsForHoldingShield(this);
        return 1;
    }
}

s32 Player_EndDeflectAttackStanding(Player* this, PlayState* play) {
    LinkAnimationHeader* anim;
    f32 frame;

    if (LinkAnimation_Update(play, &this->skelAnimeUpper)) {
        anim = Player_GetStandingDefendAnim(play, this);
        frame = Animation_GetLastFrame(anim);
        LinkAnimation_Change(play, &this->skelAnimeUpper, anim, 1.0f, frame, frame, ANIMMODE_ONCE, 0.0f);
    }

    this->stateFlags1 |= PLAYER_STATE1_SHIELDING;
    Player_SetModelsForHoldingShield(this);

    return 1;
}

s32 Player_EndDefend(Player* this, PlayState* play) {
    sUsingItemAlreadyInHand = sUsingItemAlreadyInHand2;

    if (sUsingItemAlreadyInHand || LinkAnimation_Update(play, &this->skelAnimeUpper)) {
        Player_SetUpperActionFunc(this, sUpperBodyItemFuncs[this->heldItemAction]);
        LinkAnimation_PlayLoop(play, &this->skelAnimeUpper, GET_PLAYER_ANIM(PLAYER_ANIMGROUP_STANDING_STILL, this->modelAnimType));
        this->idleCounter = 0;
        this->upperActionFunc(this, play);
        return 0;
    }

    return 1;
}

s32 Player_SetupUseFpsItem(Player* this, PlayState* play) {
    LinkAnimationHeader* anim;

    if (this->heldItemAction != PLAYER_IA_BOOMERANG) {
        if (!Player_SetupReadyFpsItemToShoot(this, play)) {
            return 0;
        }

        if (!Player_HoldsHookshot(this)) {
            anim = &gPlayerAnim_link_bow_bow_ready;
        } else {
            anim = &gPlayerAnim_link_hook_shot_ready;
        }
        LinkAnimation_PlayOnce(play, &this->skelAnimeUpper, anim);
    } else {
        Player_SetUpperActionFunc(this, Player_SetupAimBoomerang);
        this->fpsItemTimer = 10;
        LinkAnimation_PlayOnce(play, &this->skelAnimeUpper, &gPlayerAnim_link_boom_throw_wait2waitR);
    }

    if (this->stateFlags1 & PLAYER_STATE1_RIDING_HORSE) {
        Player_PlayAnimLoop(play, this, &gPlayerAnim_link_uma_anim_walk);
    } else if ((this->actor.bgCheckFlags & 1) && !Player_SetupStartUnfriendlyZTargeting(this)) {
        Player_PlayAnimLoop(play, this, GET_PLAYER_ANIM(PLAYER_ANIMGROUP_STANDING_STILL, this->modelAnimType));
    }

    return 1;
}

s32 Player_CheckShootingGalleryShootInput(PlayState* play) {
    return (play->shootingGalleryStatus > 0) && CHECK_BTN_ALL(sControlInput->press.button, BTN_B);
}

s32 func_80834E7C(PlayState* play) {
    u16 buttonsToCheck = BTN_A | BTN_B | BTN_CUP | BTN_CLEFT | BTN_CRIGHT | BTN_CDOWN;
    if (CVarGetInteger("gDpadEquips", 0) != 0) {
        buttonsToCheck |= BTN_DUP | BTN_DDOWN | BTN_DLEFT | BTN_DRIGHT;
    }
    return (play->shootingGalleryStatus != 0) &&
           ((play->shootingGalleryStatus < 0) || CHECK_BTN_ANY(sControlInput->cur.button, buttonsToCheck));
}

s32 Player_SetupAimAttention(Player* this, PlayState* play) {
    if ((this->attentionMode == PLAYER_ATTENTIONMODE_NONE) || (this->attentionMode == PLAYER_ATTENTIONMODE_AIMING)) {
        if (Player_IsZTargeting(this) || (Camera_CheckValidMode(Play_GetCamera(play, 0), 7) == 0)) {
            return 1;
        }
        this->attentionMode = PLAYER_ATTENTIONMODE_AIMING;
    }

    return 0;
}

s32 Player_CanUseFpsItem(Player* this, PlayState* play) {
    if ((this->doorType == PLAYER_DOORTYPE_NONE) && !(this->stateFlags1 & PLAYER_STATE1_AWAITING_THROWN_BOOMERANG)) {
        if (sUsingItemAlreadyInHand || Player_CheckShootingGalleryShootInput(play)) {
            if (Player_SetupUseFpsItem(this, play)) {
                return Player_SetupAimAttention(this, play);
            }
        }
    }

    return 0;
}

s32 Player_EndHookshotMove(Player* this) {
    if (this->actor.child != NULL) {
        if (this->heldActor == NULL) {
            this->heldActor = this->actor.child;
            Player_RequestRumble(this, 255, 10, 250, 0);
            func_8002F7DC(&this->actor, NA_SE_IT_HOOKSHOT_RECEIVE);
        }

        return 1;
    }

    return 0;
}

s32 Player_HoldFpsItem(Player* this, PlayState* play) {
    if (this->fpsItemType >= 0) {
        this->fpsItemType = -this->fpsItemType;
    }

    if ((!Player_HoldsHookshot(this) || Player_EndHookshotMove(this)) && !Player_StartZTargetDefend(play, this) &&
        !Player_CanUseFpsItem(this, play)) {
        return 0;
    } else if (this->rideActor != NULL) {
        this->attentionMode = 2; // OTRTODO: THIS IS A BAD IDEA BUT IT FIXES THE HORSE FIRST PERSON?
    }

    return 1;
}

// Fire the projectile
s32 Player_UpdateShotFpsItem(PlayState* play, Player* this) {
    s32 item;
    s32 arrowType;

    if (this->heldActor != NULL) {
        if (!Player_HoldsHookshot(this)) {
            Player_GetFpsItemAmmo(play, this, &item, &arrowType);

            if (gSaveContext.minigameState == 1) {
                if (!CVarGetInteger("gInfiniteAmmo", 0)) {
                    play->interfaceCtx.hbaAmmo--;
                }
            } else if (play->shootingGalleryStatus != 0) {
                if (!CVarGetInteger("gInfiniteAmmo", 0)) {
                    play->shootingGalleryStatus--;
                }
            } else {
                Inventory_ChangeAmmo(item, -1);
            }

            if (play->shootingGalleryStatus == 1) {
                play->shootingGalleryStatus = -10;
            }

            Player_RequestRumble(this, 150, 10, 150, 0);
        } else {
            Player_RequestRumble(this, 255, 20, 150, 0);
        }

        this->fpsItemShotTimer = 4;
        this->heldActor->parent = NULL;
        this->actor.child = NULL;
        this->heldActor = NULL;

        return 1;
    }

    return 0;
}

static u16 sFpsItemNoAmmoSfx[] = { NA_SE_IT_BOW_FLICK, NA_SE_IT_SLING_FLICK };

s32 Player_ReadyFpsItemToShoot(Player* this, PlayState* play) {
    s32 holdingHookshot;

    if (!Player_HoldsHookshot(this)) {
        holdingHookshot = 0;
    } else {
        holdingHookshot = 1;
    }

    Math_ScaledStepToS(&this->upperBodyRot.z, 1200, 400);
    this->lookFlags |= 0x100;

    if ((this->fpsItemShootState == 0) && (Player_IsPlayingIdleAnim(this) == 0) &&
        (this->skelAnime.animation == &gPlayerAnim_link_bow_side_walk)) {
        LinkAnimation_PlayOnce(play, &this->skelAnimeUpper, sReadyFpsItemWhileWalkingAnims[holdingHookshot]);
        this->fpsItemShootState = -1;
    } else if (LinkAnimation_Update(play, &this->skelAnimeUpper)) {
        LinkAnimation_PlayLoop(play, &this->skelAnimeUpper, sReadyFpsItemAnims[holdingHookshot]);
        this->fpsItemShootState = 1;
    } else if (this->fpsItemShootState == 1) {
        this->fpsItemShootState = 2;
    }

    if (this->fpsItemTimer > 10) {
        this->fpsItemTimer--;
    }

    Player_SetupAimAttention(this, play);

    if ((this->fpsItemShootState > 0) &&
        ((this->fpsItemType < 0) || (!sUsingItemAlreadyInHand2 && !func_80834E7C(play)))) {
        Player_SetUpperActionFunc(this, Player_AimFpsItem);
        if (this->fpsItemType >= 0) {
            if (holdingHookshot == 0) {
                if (!Player_UpdateShotFpsItem(play, this)) {
                    func_8002F7DC(&this->actor, sFpsItemNoAmmoSfx[ABS(this->fpsItemType) - 1]);
                }
            } else if (this->actor.bgCheckFlags & 1) {
                Player_UpdateShotFpsItem(play, this);
            }
        }
        this->fpsItemTimer = 10;
        Player_StopMovement(this);
    } else {
        this->stateFlags1 |= PLAYER_STATE1_READY_TO_SHOOT;
    }

    return 1;
}

s32 Player_AimFpsItem(Player* this, PlayState* play) {
    LinkAnimation_Update(play, &this->skelAnimeUpper);

    if (Player_HoldsHookshot(this) && !Player_EndHookshotMove(this)) {
        return 1;
    }

    if (!Player_StartZTargetDefend(play, this) &&
        (sUsingItemAlreadyInHand || ((this->fpsItemType < 0) && sUsingItemAlreadyInHand2) || Player_CheckShootingGalleryShootInput(play))) {
        this->fpsItemType = ABS(this->fpsItemType);

        if (Player_SetupReadyFpsItemToShoot(this, play)) {
            if (Player_HoldsHookshot(this)) {
                this->fpsItemShootState = 1;
            } else {
                LinkAnimation_PlayOnce(play, &this->skelAnimeUpper, &gPlayerAnim_link_bow_bow_shoot_next);
            }
        }
    } else {
        if (this->fpsItemTimer != 0) {
            this->fpsItemTimer--;
        }

        if (Player_IsZTargeting(this) || (this->attentionMode != 0) ||
            (this->stateFlags1 & PLAYER_STATE1_IN_FIRST_PERSON_MODE)) {
            if (this->fpsItemTimer == 0) {
                this->fpsItemTimer++;
            }
            return 1;
        }

        if (Player_HoldsHookshot(this)) {
            Player_SetUpperActionFunc(this, Player_HoldFpsItem);
        } else {
            Player_SetUpperActionFunc(this, Player_EndAimFpsItem);
            LinkAnimation_PlayOnce(play, &this->skelAnimeUpper, &gPlayerAnim_link_bow_bow_shoot_end);
        }

        this->fpsItemTimer = 0;
    }

    return 1;
}

s32 Player_EndAimFpsItem(Player* this, PlayState* play) {
    if (!(this->actor.bgCheckFlags & 1) || LinkAnimation_Update(play, &this->skelAnimeUpper)) {
        Player_SetUpperActionFunc(this, Player_HoldFpsItem);
    }

    return 1;
}

void Player_SetZTargetFriendlyYaw(Player* this) {
    this->stateFlags1 |= PLAYER_STATE1_Z_TARGETING_FRIENDLY;

    if (!(this->skelAnime.moveFlags & 0x80) && (this->actor.bgCheckFlags & 0x200) && (sYawToTouchedWall < 0x2000)) {
        this->currentYaw = this->actor.shape.rot.y = this->actor.wallYaw + 0x8000;
    }

    this->targetYaw = this->actor.shape.rot.y;
}

s32 Player_InterruptHoldingActor(PlayState* play, Player* this, Actor* heldActor) {
    if (heldActor == NULL) {
        Player_ResetAttributesAndHeldActor(play, this);
        Player_SetupStandingStillType(this, play);
        return 1;
    }

    return 0;
}

void Player_SetupHoldActorUpperAction(Player* this, PlayState* play) {
    if (!Player_InterruptHoldingActor(play, this, this->heldActor)) {
        Player_SetUpperActionFunc(this, Player_HoldActor);
        LinkAnimation_PlayLoop(play, &this->skelAnimeUpper, &gPlayerAnim_link_normal_carryB_wait);
    }
}

s32 Player_HoldActor(Player* this, PlayState* play) {
    Actor* heldActor = this->heldActor;

    if (heldActor == NULL) {
        Player_SetupHeldItemUpperActionFunc(play, this);
    }

    if (Player_StartZTargetDefend(play, this)) {
        return 1;
    }

    if (this->stateFlags1 & PLAYER_STATE1_HOLDING_ACTOR) {
        if (LinkAnimation_Update(play, &this->skelAnimeUpper)) {
            LinkAnimation_PlayLoop(play, &this->skelAnimeUpper, &gPlayerAnim_link_normal_carryB_wait);
        }

        if ((heldActor->id == ACTOR_EN_NIW) && (this->actor.velocity.y <= 0.0f)) {
            this->actor.minVelocityY = -2.0f;
            this->actor.gravity = -0.5f;
            this->fallStartHeight = this->actor.world.pos.y;
        }

        return 1;
    }

    return Player_SetupStartZTargetDefend(this, play);
}

void Player_SetLeftHandDlists(Player* this, Gfx** dLists) {
    this->leftHandDLists = &dLists[gSaveContext.linkAge];
}

s32 Player_HoldBoomerang(Player* this, PlayState* play) {
    if (Player_StartZTargetDefend(play, this)) {
        return 1;
    }

    if (this->stateFlags1 & PLAYER_STATE1_AWAITING_THROWN_BOOMERANG) {
        Player_SetUpperActionFunc(this, Player_WaitForThrownBoomerang);
    } else if (Player_CanUseFpsItem(this, play)) {
        return 1;
    }

    return 0;
}

s32 Player_SetupAimBoomerang(Player* this, PlayState* play) {
    if (LinkAnimation_Update(play, &this->skelAnimeUpper)) {
        Player_SetUpperActionFunc(this, Player_AimBoomerang);
        LinkAnimation_PlayLoop(play, &this->skelAnimeUpper, &gPlayerAnim_link_boom_throw_waitR);
    }

    Player_SetupAimAttention(this, play);

    return 1;
}

s32 Player_AimBoomerang(Player* this, PlayState* play) {
    LinkAnimationHeader* animSeg = this->skelAnime.animation;

    if ((Player_GetFightingRightAnim(this) == animSeg) || (Player_GetFightingLeftAnim(this) == animSeg) ||
        (Player_GetSidewalkRightAnim(this) == animSeg) || (Player_GetSidewalkLeftAnim(this) == animSeg)) {
        AnimationContext_SetCopyAll(play, this->skelAnime.limbCount, this->skelAnimeUpper.jointTable,
                                    this->skelAnime.jointTable);
    } else {
        LinkAnimation_Update(play, &this->skelAnimeUpper);
    }

    Player_SetupAimAttention(this, play);

    if (!sUsingItemAlreadyInHand2) {
        Player_SetUpperActionFunc(this, Player_ThrowBoomerang);
        LinkAnimation_PlayOnce(play, &this->skelAnimeUpper,
                               (this->leftRightBlendWeight < 0.5f) ? &gPlayerAnim_link_boom_throwR : &gPlayerAnim_link_boom_throwL);
    }

    return 1;
}

s32 Player_ThrowBoomerang(Player* this, PlayState* play) {
    if (LinkAnimation_Update(play, &this->skelAnimeUpper)) {
        Player_SetUpperActionFunc(this, Player_WaitForThrownBoomerang);
        this->fpsItemTimer = 0;
    } else if (LinkAnimation_OnFrame(&this->skelAnimeUpper, 6.0f)) {
        f32 posX = (Math_SinS(this->actor.shape.rot.y) * 10.0f) + this->actor.world.pos.x;
        f32 posZ = (Math_CosS(this->actor.shape.rot.y) * 10.0f) + this->actor.world.pos.z;
        s32 yaw = (this->targetActor != NULL) ? this->actor.shape.rot.y + 14000 : this->actor.shape.rot.y;
        EnBoom* boomerang =
            (EnBoom*)Actor_Spawn(&play->actorCtx, play, ACTOR_EN_BOOM, posX, this->actor.world.pos.y + 30.0f,
                                 posZ, this->actor.focus.rot.x, yaw, 0, 0, true);

        this->boomerangActor = &boomerang->actor;
        if (boomerang != NULL) {
            boomerang->moveTo = this->targetActor;
            boomerang->returnTimer = 20;
            this->stateFlags1 |= PLAYER_STATE1_AWAITING_THROWN_BOOMERANG;
            if (!Player_IsUnfriendlyZTargeting(this)) {
                Player_SetZTargetFriendlyYaw(this);
            }
            this->fpsItemShotTimer = 4;
            func_8002F7DC(&this->actor, NA_SE_IT_BOOMERANG_THROW);
            Player_PlayVoiceSfxForAge(this, NA_SE_VO_LI_SWORD_N);
        }
    }

    return 1;
}

s32 spawn_boomerang_ivan(EnPartner* this, PlayState* play) {
    if (!CVarGetInteger("gIvanCoopModeEnabled", 0)) {
        return 0;
    }

    f32 posX = (Math_SinS(this->actor.shape.rot.y) * 1.0f) + this->actor.world.pos.x;
    f32 posZ = (Math_CosS(this->actor.shape.rot.y) * 1.0f) + this->actor.world.pos.z;
    s32 yaw = this->actor.shape.rot.y;
    EnBoom* boomerang =
        (EnBoom*)Actor_Spawn(&play->actorCtx, play, ACTOR_EN_BOOM, posX, this->actor.world.pos.y + 7.0f, posZ,
                             this->actor.focus.rot.x, yaw, 0, 0, true);

    this->boomerangActor = &boomerang->actor;
    if (boomerang != NULL) {
        boomerang->returnTimer = 20;
        Audio_PlayActorSound2(&this->actor, NA_SE_IT_BOOMERANG_THROW);
    }

    return 1;
}

s32 Player_WaitForThrownBoomerang(Player* this, PlayState* play) {
    if (Player_StartZTargetDefend(play, this)) {
        return 1;
    }

    if (!(this->stateFlags1 & PLAYER_STATE1_AWAITING_THROWN_BOOMERANG)) {
        Player_SetUpperActionFunc(this, Player_CatchBoomerang);
        LinkAnimation_PlayOnce(play, &this->skelAnimeUpper, &gPlayerAnim_link_boom_catch);
        Player_SetLeftHandDlists(this, gPlayerLeftHandBoomerangDLs);
        func_8002F7DC(&this->actor, NA_SE_PL_CATCH_BOOMERANG);
        Player_PlayVoiceSfxForAge(this, NA_SE_VO_LI_SWORD_N);
        return 1;
    }

    if (sUsingItemAlreadyInHand && CVarGetInteger("gFastBoomerang", 0)) {
        this->boomerangQuickRecall = true;
    }

    return 0;
}

s32 Player_CatchBoomerang(Player* this, PlayState* play) {
    if (!Player_HoldBoomerang(this, play) && LinkAnimation_Update(play, &this->skelAnimeUpper)) {
        Player_SetUpperActionFunc(this, Player_HoldBoomerang);
    }

    return 1;
}

s32 Player_SetActionFunc(PlayState* play, Player* this, PlayerActionFunc func, s32 flags) {
    if (func == this->actionFunc) {
        return 0;
    }

    if (Player_PlayOcarina == this->actionFunc) {
        Audio_OcaSetInstrument(0);
        this->stateFlags2 &= ~(PLAYER_STATE2_ATTEMPT_PLAY_OCARINA_FOR_ACTOR | PLAYER_STATE2_PLAYING_OCARINA_FOR_ACTOR);
    } else if (Player_UpdateMagicSpell == this->actionFunc) {
        Player_ResetSubCam(play, this);
    }

    this->actionFunc = func;

    if ((this->itemAction != this->heldItemAction) &&
        (!(flags & 1) || !(this->stateFlags1 & PLAYER_STATE1_SHIELDING))) {
        Player_SetHeldItem(this);
    }

    if (!(flags & 1) && (!(this->stateFlags1 & PLAYER_STATE1_HOLDING_ACTOR))) {
        Player_SetupHeldItemUpperActionFunc(play, this);
        this->stateFlags1 &= ~PLAYER_STATE1_SHIELDING;
    }

    Player_EndAnimMovement(this);
    this->stateFlags1 &= ~(PLAYER_STATE1_END_HOOKSHOT_MOVE | PLAYER_STATE1_TALKING | PLAYER_STATE1_TAKING_DAMAGE |
                           PLAYER_STATE1_SKIP_OTHER_ACTORS_UPDATE | PLAYER_STATE1_IN_CUTSCENE |
                           PLAYER_STATE1_FALLING_INTO_GROTTO_OR_VOID);
    this->stateFlags2 &=
        ~(PLAYER_STATE2_BACKFLIPPING_OR_SIDEHOPPING | PLAYER_STATE2_PLAYING_OCARINA_GENERAL | PLAYER_STATE2_IDLING);
    this->stateFlags3 &=
        ~(PLAYER_STATE3_MIDAIR | PLAYER_STATE3_ENDING_MELEE_ATTACK | PLAYER_STATE3_MOVING_ALONG_HOOKSHOT_PATH);
    this->genericVar = 0;
    this->genericTimer = 0;
    this->idleCounter = 0;
    Player_StopInterruptableSfx(this);

    return 1;
}

void Player_SetActionFuncPreserveMoveFlags(PlayState* play, Player* this, PlayerActionFunc func, s32 flags) {
    s32 flagsToRestore;

    flagsToRestore = this->skelAnime.moveFlags;
    this->skelAnime.moveFlags = 0;
    Player_SetActionFunc(play, this, func, flags);
    this->skelAnime.moveFlags = flagsToRestore;
}

void Player_SetActionFuncPreserveItemAP(PlayState* play, Player* this, PlayerActionFunc func, s32 flags) {
    s32 temp;

    if (this->itemAction >= 0) {
        temp = this->itemAction;
        this->itemAction = this->heldItemAction;
        Player_SetActionFunc(play, this, func, flags);
        this->itemAction = temp;
        Player_SetModels(this, Player_ActionToModelGroup(this, this->itemAction));
    }
}

void Player_ChangeCameraSetting(PlayState* play, s16 camSetting) {
    if (!func_800C0CB8(play)) {
        if (camSetting == CAM_SET_SCENE_TRANSITION) {
            Interface_ChangeAlpha(2);
        }
    } else {
        Camera_ChangeSetting(Play_GetCamera(play, 0), camSetting);
    }
}

void Player_SetCameraTurnAround(PlayState* play, s32 arg1) {
    Player_ChangeCameraSetting(play, CAM_SET_TURN_AROUND);
    Camera_SetCameraData(Play_GetCamera(play, 0), 4, 0, 0, arg1, 0, 0);
}

void Player_PutAwayHookshot(Player* this) {
    if (Player_HoldsHookshot(this)) {
        Actor* heldActor = this->heldActor;

        if (heldActor != NULL) {
            Actor_Kill(heldActor);
            this->actor.child = NULL;
            this->heldActor = NULL;
        }
    }
}

void Player_UseItem(PlayState* play, Player* this, s32 item) {
    s8 actionParam;
    s32 temp;
    s32 nextAnimType;

    actionParam = Player_ItemToActionParam(item);

    if (((this->heldItemAction == this->itemAction) &&
         (!(this->stateFlags1 & PLAYER_STATE1_SHIELDING) || (Player_ActionToSword(actionParam) != 0) ||
          (actionParam == PLAYER_IA_NONE))) ||
        ((this->itemAction < 0) &&
         ((Player_ActionToSword(actionParam) != 0) || (actionParam == PLAYER_IA_NONE)))) {

        if ((actionParam == PLAYER_IA_NONE) || !(this->stateFlags1 & PLAYER_STATE1_SWIMMING) ||
            ((this->actor.bgCheckFlags & 1) &&
             ((actionParam == PLAYER_IA_HOOKSHOT) || (actionParam == PLAYER_IA_LONGSHOT))) ||
            ((actionParam >= PLAYER_IA_SHIELD_DEKU) && (actionParam <= PLAYER_IA_BOOTS_HOVER))) {

            if ((play->bombchuBowlingStatus == 0) &&
                (((actionParam == PLAYER_IA_STICK) && (AMMO(ITEM_STICK) == 0)) ||
                 ((actionParam == PLAYER_IA_BEAN) && (AMMO(ITEM_BEAN) == 0)) ||
                 (temp = Player_ActionToExplosive(this, actionParam),
                  ((temp >= 0) && ((AMMO(sExplosiveInfos[temp].itemId) == 0) ||
                                   (play->actorCtx.actorLists[ACTORCAT_EXPLOSIVE].length >= 3)))))) {
                func_80078884(NA_SE_SY_ERROR);
                return;
            }

            if (actionParam >= PLAYER_IA_BOOTS_KOKIRI) {
                u16 bootsValue = actionParam - PLAYER_IA_BOOTS_KOKIRI + 1;
                if (CUR_EQUIP_VALUE(EQUIP_BOOTS) == bootsValue) {
                    Inventory_ChangeEquipment(EQUIP_BOOTS, PLAYER_BOOTS_KOKIRI + 1);
                } else {
                    Inventory_ChangeEquipment(EQUIP_BOOTS, bootsValue);
                }
                Player_SetEquipmentData(play, this);
                Player_PlayReactableSfx(this, CUR_EQUIP_VALUE(EQUIP_BOOTS) == PLAYER_BOOTS_IRON + 1
                    ? NA_SE_PL_WALK_HEAVYBOOTS
                    : NA_SE_PL_CHANGE_ARMS);
                return;
            }

            if (actionParam >= PLAYER_IA_TUNIC_KOKIRI) {
                u16 tunicValue = actionParam - PLAYER_IA_TUNIC_KOKIRI + 1;
                if (CUR_EQUIP_VALUE(EQUIP_TUNIC) == tunicValue) {
                    Inventory_ChangeEquipment(EQUIP_TUNIC, PLAYER_TUNIC_KOKIRI + 1);
                } else {
                    Inventory_ChangeEquipment(EQUIP_TUNIC, tunicValue);
                }
                Player_SetEquipmentData(play, this);
                Player_PlayReactableSfx(this, NA_SE_PL_CHANGE_ARMS);
                return;
            }

            if (actionParam >= PLAYER_IA_SHIELD_DEKU) {
                // Changing shields through action commands is unimplemented
                return;
            }

            if (actionParam == PLAYER_IA_LENS) {
                if (func_80087708(play, 0, 3)) {
                    if (play->actorCtx.lensActive) {
                        Actor_DisableLens(play);
                    } else {
                        play->actorCtx.lensActive = true;
                    }
                    func_80078884((play->actorCtx.lensActive) ? NA_SE_SY_GLASSMODE_ON : NA_SE_SY_GLASSMODE_OFF);
                } else {
                    func_80078884(NA_SE_SY_ERROR);
                }
                return;
            }

            if (actionParam == PLAYER_IA_NUT) {
                if (AMMO(ITEM_NUT) != 0) {
                    Player_SetupThrowDekuNut(play, this);
                } else {
                    func_80078884(NA_SE_SY_ERROR);
                }
                return;
            }

            temp = Player_ActionToMagicSpell(this, actionParam);
            if (temp >= 0) {
                if (((actionParam == PLAYER_IA_FARORES_WIND) && (gSaveContext.respawn[RESPAWN_MODE_TOP].data > 0)) ||
                    ((gSaveContext.magicCapacity != 0) && (gSaveContext.magicState == 0) &&
                     (gSaveContext.magic >= sMagicSpellCosts[temp]))) {
                    this->itemAction = actionParam;
                    this->attentionMode = 4;
                } else {
                    func_80078884(NA_SE_SY_ERROR);
                }
                return;
            }

            if (actionParam >= PLAYER_IA_MASK_KEATON) {
                if (this->currentMask != PLAYER_MASK_NONE) {
                    this->currentMask = PLAYER_MASK_NONE;
                } else {
                    this->currentMask = actionParam - PLAYER_IA_MASK_KEATON + 1;
                }
                sMaskMemory = this->currentMask;
                Player_PlayReactableSfx(this, NA_SE_PL_CHANGE_ARMS);
                return;
            }

            if (((actionParam >= PLAYER_IA_OCARINA_FAIRY) && (actionParam <= PLAYER_IA_OCARINA_TIME)) ||
                (actionParam >= PLAYER_IA_BOTTLE_FISH)) {
                if (!Player_IsUnfriendlyZTargeting(this) ||
                    ((actionParam >= PLAYER_IA_BOTTLE_POTION_RED) && (actionParam <= PLAYER_IA_BOTTLE_FAIRY))) {
                    func_8002D53C(play, &play->actorCtx.titleCtx);
                    this->attentionMode = 4;
                    this->itemAction = actionParam;
                }
                return;
            }

            if ((actionParam != this->heldItemAction) ||
                ((this->heldActor == 0) && (Player_ActionToExplosive(this, actionParam) >= 0))) {
                this->nextModelGroup = Player_ActionToModelGroup(this, actionParam);
                nextAnimType = gPlayerModelTypes[this->nextModelGroup][PLAYER_MODELGROUPENTRY_ANIM];
                if ((this->heldItemAction >= 0) && (Player_ActionToMagicSpell(this, actionParam) < 0) &&
                    (item != this->heldItemId) &&
                    (sAnimtypeToItemChangeAnims[gPlayerModelTypes[this->modelGroup][PLAYER_MODELGROUPENTRY_ANIM]][nextAnimType] !=
                     PLAYER_ITEM_CHANGE_DEFAULT) &&
                    (!CVarGetInteger("gSeparateArrows", 0) ||
                      actionParam < PLAYER_IA_BOW || actionParam > PLAYER_IA_BOW_0E ||
                      this->heldItemAction < PLAYER_IA_BOW || this->heldItemAction > PLAYER_IA_BOW_0E)) {
                    this->heldItemId = item;
                    this->stateFlags1 |= PLAYER_STATE1_START_CHANGE_ITEM;
                } else {
                    Player_PutAwayHookshot(this);
                    Player_DetatchHeldActor(play, this);
                    Player_SetupChangeItemAnim(play, this, actionParam);
                }
                return;
            }

            sUsingItemAlreadyInHand = sUsingItemAlreadyInHand2 = true;
        }
    }
}

void Player_SetupDie(PlayState* play, Player* this, LinkAnimationHeader* anim) {
    s32 isSwimming = Player_IsSwimming(this);

    Player_ResetAttributesAndHeldActor(play, this);

    Player_SetActionFunc(play, this, isSwimming ? Player_Drown : Player_Die, 0);

    this->stateFlags1 |= PLAYER_STATE1_IN_DEATH_CUTSCENE;

    Player_PlayAnimOnce(play, this, anim);
    if (anim == &gPlayerAnim_link_derth_rebirth) {
        this->skelAnime.endFrame = 84.0f;
    }

    Player_ClearAttentionModeAndStopMoving(this);
    Player_PlayVoiceSfxForAge(this, NA_SE_VO_LI_DOWN);

    if (this->actor.category == ACTORCAT_PLAYER) {
        func_800F47BC();

        if (Inventory_ConsumeFairy(play)) {
            play->gameOverCtx.state = GAMEOVER_REVIVE_START;
            this->genericVar = 1;
        } else {
            play->gameOverCtx.state = GAMEOVER_DEATH_START;
            func_800F6AB0(0);
            Audio_PlayFanfare(NA_BGM_GAME_OVER);
            gSaveContext.seqId = (u8)NA_BGM_DISABLED;
            gSaveContext.natureAmbienceId = NATURE_ID_DISABLED;
        }

        OnePointCutscene_Init(play, 9806, isSwimming ? 120 : 60, &this->actor, MAIN_CAM);
        ShrinkWindow_SetVal(0x20);
    }
}

s32 Player_CanUseItem(Player* this) {
    return (!(Player_RunMiniCutsceneFunc == this->actionFunc) ||
            ((this->stateFlags1 & PLAYER_STATE1_START_CHANGE_ITEM) &&
             ((this->heldItemId == ITEM_LAST_USED) || (this->heldItemId == ITEM_NONE)))) &&
           (!(Player_StartChangeItem == this->upperActionFunc) ||
            (Player_ItemToActionParam(this->heldItemId) == this->heldItemAction));
}

s32 Player_SetupCurrentUpperAction(Player* this, PlayState* play) {
    if (!(this->stateFlags1 & PLAYER_STATE1_RIDING_HORSE) && (this->actor.parent != NULL) && Player_HoldsHookshot(this)) {
        Player_SetActionFunc(play, this, Player_MoveAlongHookshotPath, 1);
        this->stateFlags3 |= PLAYER_STATE3_MOVING_ALONG_HOOKSHOT_PATH;
        Player_PlayAnimOnce(play, this, &gPlayerAnim_link_hook_fly_start);
        Player_SetupAnimMovement(play, this, 0x9B);
        Player_ClearAttentionModeAndStopMoving(this);
        this->currentYaw = this->actor.shape.rot.y;
        this->actor.bgCheckFlags &= ~1;
        this->hoverBootsTimer = 0;
        this->lookFlags |= 0x43;
        Player_PlayVoiceSfxForAge(this, NA_SE_VO_LI_LASH);
        return 1;
    }

    if (Player_CanUseItem(this)) {
        Player_SetupItem(this, play);
        if (Player_ThrowDekuNut == this->actionFunc) {
            return 1;
        }
    }

    if (!this->upperActionFunc(this, play)) {
        return 0;
    }

    if (this->upperInterpWeight != 0.0f) {
        if ((Player_IsPlayingIdleAnim(this) == 0) || (this->linearVelocity != 0.0f)) {
            AnimationContext_SetCopyFalse(play, this->skelAnime.limbCount, this->skelAnimeUpper.jointTable,
                                          this->skelAnime.jointTable, D_80853410);
        }
        Math_StepToF(&this->upperInterpWeight, 0.0f, 0.25f);
        AnimationContext_SetInterp(play, this->skelAnime.limbCount, this->skelAnime.jointTable,
                                   this->skelAnimeUpper.jointTable, 1.0f - this->upperInterpWeight);
    } else if ((Player_IsPlayingIdleAnim(this) == 0) || (this->linearVelocity != 0.0f)) {
        AnimationContext_SetCopyTrue(play, this->skelAnime.limbCount, this->skelAnime.jointTable,
                                     this->skelAnimeUpper.jointTable, D_80853410);
    } else {
        AnimationContext_SetCopyAll(play, this->skelAnime.limbCount, this->skelAnime.jointTable,
                                    this->skelAnimeUpper.jointTable);
    }

    return 1;
}

s32 Player_SetupMiniCsFunc(PlayState* play, Player* this, PlayerMiniCsFunc func) {
    this->miniCsFunc = func;
    Player_SetActionFunc(play, this, Player_RunMiniCutsceneFunc, 0);
    this->stateFlags2 |= PLAYER_STATE2_ALWAYS_DISABLE_MOVE_ROTATION;
    return Player_UnequipItem(play, this);
}

void Player_UpdateYaw(Player* this, PlayState* play) {
    s16 previousYaw = this->actor.shape.rot.y;

    if (!(this->stateFlags2 &
          (PLAYER_STATE2_DISABLE_MOVE_ROTATION_WHILE_Z_TARGETING | PLAYER_STATE2_ALWAYS_DISABLE_MOVE_ROTATION))) {
        if ((this->targetActor != NULL) &&
            ((play->actorCtx.targetCtx.unk_4B != 0) || (this->actor.category != ACTORCAT_PLAYER))) {
            Math_ScaledStepToS(&this->actor.shape.rot.y,
                               Math_Vec3f_Yaw(&this->actor.world.pos, &this->targetActor->focus.pos), 4000);
        } else if ((this->stateFlags1 & PLAYER_STATE1_Z_TARGETING_FRIENDLY) &&
                   !(this->stateFlags2 & (PLAYER_STATE2_DISABLE_MOVE_ROTATION_WHILE_Z_TARGETING |
                                          PLAYER_STATE2_ALWAYS_DISABLE_MOVE_ROTATION))) {
            Math_ScaledStepToS(&this->actor.shape.rot.y, this->targetYaw, 4000);
        }
    } else if (!(this->stateFlags2 & PLAYER_STATE2_ALWAYS_DISABLE_MOVE_ROTATION)) {
        Math_ScaledStepToS(&this->actor.shape.rot.y, this->currentYaw, 2000);
    }

    this->unk_87C = this->actor.shape.rot.y - previousYaw;
}

s32 Player_StepAngleWithOffset(s16* pValue, s16 target, s16 step, s16 angleMinMax, s16 referenceAngle,
                               s16 angleDiffMinMax) {
    s16 angleDiff;
    s16 clampedAngleDiff;
    s16 originalAngle;

    angleDiff = clampedAngleDiff = referenceAngle - *pValue;
    clampedAngleDiff = CLAMP(clampedAngleDiff, -angleDiffMinMax, angleDiffMinMax);
    *pValue += (s16)(angleDiff - clampedAngleDiff);

    Math_ScaledStepToS(pValue, target, step);

    originalAngle = *pValue;
    if (*pValue < -angleMinMax) {
        *pValue = -angleMinMax;
    } else if (*pValue > angleMinMax) {
        *pValue = angleMinMax;
    }
    return originalAngle - *pValue;
}

s32 Player_UpdateLookAngles(Player* this, s32 syncUpperRotToFocusRot) {
    s16 yawDiff;
    s16 lookYaw;

    lookYaw = this->actor.shape.rot.y;
    if (syncUpperRotToFocusRot != 0) {
        lookYaw = this->actor.focus.rot.y;
        this->upperBodyRot.x = this->actor.focus.rot.x;
        this->lookFlags |= 0x41;
    } else {
        Player_StepAngleWithOffset(&this->upperBodyRot.x,
                                   Player_StepAngleWithOffset(&this->headRot.x, this->actor.focus.rot.x, 600, 10000,
                                                              this->actor.focus.rot.x, 0),
                                   200, 4000, this->headRot.x, 10000);
        yawDiff = this->actor.focus.rot.y - lookYaw;
        Player_StepAngleWithOffset(&yawDiff, 0, 200, 24000, this->upperBodyRot.y, 8000);
        lookYaw = this->actor.focus.rot.y - yawDiff;
        Player_StepAngleWithOffset(&this->headRot.y, yawDiff - this->upperBodyRot.y, 200, 8000, yawDiff, 8000);
        Player_StepAngleWithOffset(&this->upperBodyRot.y, yawDiff, 200, 8000, this->headRot.y, 8000);
        this->lookFlags |= PLAYER_LOOKFLAGS_OVERRIDE_UPPERBODY_ROT_Y | PLAYER_LOOKFLAGS_OVERRIDE_UPPERBODY_ROT_X |
                           PLAYER_LOOKFLAGS_OVERRIDE_HEAD_ROT_Y | PLAYER_LOOKFLAGS_OVERRIDE_HEAD_ROT_X |
                           PLAYER_LOOKFLAGS_OVERRIDE_FOCUS_ROT_X;
    }

    return lookYaw;
}

void Player_SetupZTargeting(Player* this, PlayState* play) {
    s32 isRangeCheckDisabled = 0;
    s32 zTrigPressed = CHECK_BTN_ALL(sControlInput->cur.button, BTN_Z);
    Actor* actorToTarget;
    s32 pad;
    s32 holdTarget;
    s32 actorRequestingTalk;

    if (!zTrigPressed) {
        this->stateFlags1 &= ~PLAYER_STATE1_30;
    }

    if ((play->csCtx.state != CS_STATE_IDLE) || (this->csMode != 0) ||
        (this->stateFlags1 & (PLAYER_STATE1_IN_DEATH_CUTSCENE | PLAYER_STATE1_IN_CUTSCENE)) ||
        (this->stateFlags3 & PLAYER_STATE3_MOVING_ALONG_HOOKSHOT_PATH)) {
        this->targetSwitchTimer = 0;
    } else if ((zTrigPressed && !CVarGetInteger("gDisableTargeting", 0)) ||
               (this->stateFlags2 & PLAYER_STATE2_USING_SWITCH_Z_TARGETING) || (this->forcedTargetActor != NULL)) {
        if (this->targetSwitchTimer <= 5) {
            this->targetSwitchTimer = 5;
        } else {
            this->targetSwitchTimer--;
        }
    } else if (this->stateFlags1 & PLAYER_STATE1_Z_TARGETING_FRIENDLY) {
        this->targetSwitchTimer = 0;
    } else if (this->targetSwitchTimer != 0) {
        this->targetSwitchTimer--;
    }

    if (this->targetSwitchTimer >= 6) {
        isRangeCheckDisabled = 1;
    }

    actorRequestingTalk = Player_CheckActorTalkRequested(play);
    if (actorRequestingTalk || (this->targetSwitchTimer != 0) ||
        (this->stateFlags1 & (PLAYER_STATE1_CHARGING_SPIN_ATTACK | PLAYER_STATE1_AWAITING_THROWN_BOOMERANG))) {
        if (!actorRequestingTalk) {
            if (!(this->stateFlags1 & PLAYER_STATE1_AWAITING_THROWN_BOOMERANG) &&
                ((this->heldItemAction != PLAYER_IA_FISHING_POLE) || (this->fpsItemType == 0)) &&
                CHECK_BTN_ALL(sControlInput->press.button, BTN_Z)) {

                if (this->actor.category == ACTORCAT_PLAYER) {
                    actorToTarget = play->actorCtx.targetCtx.arrowPointedActor;
                } else {
                    actorToTarget = &GET_PLAYER(play)->actor;
                }

                holdTarget = (gSaveContext.zTargetSetting != 0) || (this->actor.category != ACTORCAT_PLAYER);
                this->stateFlags1 |= PLAYER_STATE1_UNUSED_Z_TARGETING_FLAG;

                if ((actorToTarget != NULL) && !(actorToTarget->flags & ACTOR_FLAG_NO_LOCKON)) {
                    if ((actorToTarget == this->targetActor) && (this->actor.category == ACTORCAT_PLAYER)) {
                        actorToTarget = play->actorCtx.targetCtx.unk_94;
                    }

                    if (actorToTarget != this->targetActor) {
                        if (!holdTarget) {
                            this->stateFlags2 |= PLAYER_STATE2_USING_SWITCH_Z_TARGETING;
                        }
                        this->targetActor = actorToTarget;
                        this->targetSwitchTimer = 15;
                        this->stateFlags2 &= ~(PLAYER_STATE2_CAN_SPEAK_OR_CHECK | PLAYER_STATE2_NAVI_REQUESTING_TALK);
                    } else {
                        if (!holdTarget) {
                            func_8008EDF0(this);
                        }
                    }

                    this->stateFlags1 &= ~PLAYER_STATE1_30;
                } else {
                    if (!(this->stateFlags1 & (PLAYER_STATE1_Z_TARGETING_FRIENDLY | PLAYER_STATE1_30))) {
                        Player_SetZTargetFriendlyYaw(this);
                    }
                }
            }

            if (this->targetActor != NULL) {
                if ((this->actor.category == ACTORCAT_PLAYER) && (this->targetActor != this->forcedTargetActor) &&
                    func_8002F0C8(this->targetActor, this, isRangeCheckDisabled)) {
                    func_8008EDF0(this);
                    this->stateFlags1 |= PLAYER_STATE1_30;
                } else if (this->targetActor != NULL) {
                    this->targetActor->targetPriority = 40;
                }
            } else if (this->forcedTargetActor != NULL) {
                this->targetActor = this->forcedTargetActor;
            }
        }

        if (this->targetActor != NULL) {
            this->stateFlags1 &= ~(PLAYER_STATE1_FORCE_STRAFING | PLAYER_STATE1_Z_TARGETING_FRIENDLY);
            if ((this->stateFlags1 & PLAYER_STATE1_HOLDING_ACTOR) ||
                !CHECK_FLAG_ALL(this->targetActor->flags, ACTOR_FLAG_TARGETABLE | ACTOR_FLAG_HOSTILE)) {
                this->stateFlags1 |= PLAYER_STATE1_FORCE_STRAFING;
            }
        } else {
            if (this->stateFlags1 & PLAYER_STATE1_Z_TARGETING_FRIENDLY) {
                this->stateFlags2 &= ~PLAYER_STATE2_USING_SWITCH_Z_TARGETING;
            } else {
                func_8008EE08(this);
            }
        }
    } else {
        func_8008EE08(this);
    }
}

s32 Player_CalculateTargetVelocityAndYaw(PlayState* play, Player* this, f32* targetVelocity, s16* targetYaw,
                                         f32 arg4) {
    f32 baseSpeedScale;
    f32 slope;
    f32 slopeSpeedScale;
    f32 speedLimit;

    if ((this->attentionMode != 0) || (play->sceneLoadFlag == 0x14) ||
        (this->stateFlags1 & PLAYER_STATE1_EXITING_SCENE)) {
        *targetVelocity = 0.0f;
        *targetYaw = this->actor.shape.rot.y;
    } else {
        *targetVelocity = sAnalogStickDistance;
        *targetYaw = sAnalogStickAngle;

        if (arg4 != 0.0f) {
            *targetVelocity -= 20.0f;
            if (*targetVelocity < 0.0f) {
                *targetVelocity = 0.0f;
            } else {
                baseSpeedScale = 1.0f - Math_CosS(*targetVelocity * 450.0f);
                *targetVelocity = ((baseSpeedScale * baseSpeedScale) * 30.0f) + 7.0f;
            }
        } else {
            *targetVelocity *= 0.8f;
        }

        if (sAnalogStickDistance != 0.0f) {
            slope = Math_SinS(this->angleToFloorX);
            speedLimit = this->speedLimit;
            slopeSpeedScale = CLAMP(slope, 0.0f, 0.6f);

            if (this->shapeOffsetY != 0.0f) {
                speedLimit = speedLimit - (this->shapeOffsetY * 0.008f);
                if (speedLimit < 2.0f) {
                    speedLimit = 2.0f;
                }
            }

            *targetVelocity = (*targetVelocity * 0.14f) - (8.0f * slopeSpeedScale * slopeSpeedScale);
            *targetVelocity = CLAMP(*targetVelocity, 0.0f, speedLimit);

            if ((CVarGetInteger("gMoonwalk", 0) || (CVarGetInteger("gChaosRedeem", 0) && Rand_ZeroOne() < 0.02f)) && *targetVelocity > 0) {
                *targetVelocity = -*targetVelocity;
            }

            return 1;
        }
    }

    return 0;
}

s32 Player_StepLinearVelocityToZero(Player* this) {
    return Math_StepToF(&this->linearVelocity, 0.0f, REG(43) / 100.0f);
}

s32 Player_GetTargetVelocityAndYaw(Player* this, f32* arg1, s16* arg2, f32 arg3, PlayState* play) {
    if (!Player_CalculateTargetVelocityAndYaw(play, this, arg1, arg2, arg3)) {
        *arg2 = this->actor.shape.rot.y;

        if (this->targetActor != NULL) {
            if ((play->actorCtx.targetCtx.unk_4B != 0) &&
                !(this->stateFlags2 & PLAYER_STATE2_ALWAYS_DISABLE_MOVE_ROTATION)) {
                *arg2 = Math_Vec3f_Yaw(&this->actor.world.pos, &this->targetActor->focus.pos);
                return 0;
            }
        } else if (Player_IsFriendlyZTargeting(this)) {
            *arg2 = this->targetYaw;
        }

        return 0;
    } else {
        *arg2 += Camera_GetInputDirYaw(GET_ACTIVE_CAM(play));
        return 1;
    }
}

static s8 sTargetEnemyStandStillSubActions[] = { 13, 2, 4, 9, 10, 11, 8, -7 };
static s8 sFriendlyTargetingStandStillSubActions[] = { 13, 1, 2, 5, 3, 4, 9, 10, 11, 7, 8, -6 };
static s8 sEndSidewalkSubActions[] = { 13, 1, 2, 3, 4, 9, 10, 11, 8, 7, -6 };
static s8 sFriendlyBackwalkSubActions[] = { 13, 2, 4, 9, 10, 11, 8, -7 };
static s8 sSidewalkSubActions[] = { 13, 2, 4, 9, 10, 11, 12, 8, -7 };
static s8 sTurnSubActions[] = { -7 };
static s8 sStandStillSubActions[] = { 0, 11, 1, 2, 3, 5, 4, 9, 8, 7, -6 };
static s8 sRunSubActions[] = { 0, 11, 1, 2, 3, 12, 5, 4, 9, 8, 7, -6 };
static s8 sTargetRunSubActions[] = { 13, 1, 2, 3, 12, 5, 4, 9, 10, 11, 8, 7, -6 };
static s8 sEndBackwalkSubActions[] = { 10, 8, -7 };
static s8 sSwimSubActions[] = { 0, 12, 5, -4 };

static s32 (*sSubActions[])(Player* this, PlayState* play) = {
    Player_SetupCUpBehavior,               // 0
    Player_SetupOpenDoor,                  // 1
    Player_SetupGetItemOrHoldBehavior,     // 2
    Player_SetupMountHorse,                // 3
    Player_SetupSpeakOrCheck,              // 4
    Player_SetupSpecialWallInteraction,    // 5
    Player_SetupRollOrPutAway,             // 6
    Player_SetupStartMeleeWeaponAttack,    // 7
    Player_SetupStartChargeSpinAttack,     // 8
    Player_SetupPutDownOrThrowActor,       // 9
    Player_SetupJumpSlashOrRoll,           // 10
    Player_SetupDefend,                    // 11
    Player_SetupWallJumpBehavior,          // 12
    Player_SetupItemCutsceneOrFirstPerson, // 13
};

s32 Player_SetupSubAction(PlayState* play, Player* this, s8* subActionIndex, s32 arg3) {
    s32 i;

    if (!(this->stateFlags1 &
          (PLAYER_STATE1_EXITING_SCENE | PLAYER_STATE1_IN_DEATH_CUTSCENE | PLAYER_STATE1_IN_CUTSCENE))) {
        if (arg3 != 0) {
            D_808535E0 = Player_SetupCurrentUpperAction(this, play);
            if (Player_ThrowDekuNut == this->actionFunc) {
                return 1;
            }
        }

        if (Player_IsShootingHookshot(this)) {
            this->lookFlags |= PLAYER_LOOKFLAGS_OVERRIDE_UPPERBODY_ROT_X | PLAYER_LOOKFLAGS_OVERRIDE_FOCUS_ROT_X;
            return 1;
        }

        if (!(this->stateFlags1 & PLAYER_STATE1_START_CHANGE_ITEM) &&
            (Player_StartChangeItem != this->upperActionFunc)) {
            while (*subActionIndex >= 0) {
                if (sSubActions[*subActionIndex](this, play)) {
                    return 1;
                }
                subActionIndex++;
            }

            if (sSubActions[-(*subActionIndex)](this, play)) {
                return 1;
            }
        }
    }

    return 0;
}

// Checks if action is interrupted within a certain number of frames from the end of the current animation
// Returns -1 is action is not interrupted at all, 0 if interrupted by a sub-action, 1 if interrupted by the player
// moving
s32 Player_IsActionInterrupted(PlayState* play, Player* this, SkelAnime* skelAnime, f32 framesFromEnd) {
    f32 targetVelocity;
    s16 targetYaw;

    if ((skelAnime->endFrame - framesFromEnd) <= skelAnime->curFrame) {
        if (Player_SetupSubAction(play, this, sStandStillSubActions, 1)) {
            return 0;
        }

        if (Player_GetTargetVelocityAndYaw(this, &targetVelocity, &targetYaw, 0.018f, play)) {
            return 1;
        }
    }

    return -1;
}

void Player_SetupSpinAttackActor(PlayState* play, Player* this, s32 spinAttackParams) {
    if (spinAttackParams != 0) {
        this->unk_858 = 0.0f;
    } else {
        this->unk_858 = 0.5f;
    }

    this->stateFlags1 |= PLAYER_STATE1_CHARGING_SPIN_ATTACK;

    if (this->actor.category == ACTORCAT_PLAYER) {
        Actor_Spawn(&play->actorCtx, play, ACTOR_EN_M_THUNDER, this->bodyPartsPos[PLAYER_BODYPART_WAIST].x,
                    this->bodyPartsPos[PLAYER_BODYPART_WAIST].y, this->bodyPartsPos[PLAYER_BODYPART_WAIST].z, 0, 0, 0,
                    Player_GetSwordHeld(this) | spinAttackParams, true);
    }
}

s32 Player_CanQuickspin(Player* this) {
    s8 stickInputsArray[4];
    s8* analogStickInput;
    s8* stickInput;
    s8 inputDiff1;
    s8 inputDiff2;
    s32 i;

    if ((this->heldItemAction == PLAYER_IA_STICK) || Player_HoldsBrokenKnife(this)) {
        return 0;
    }

    analogStickInput = &this->analogStickInputs[0];
    stickInput = &stickInputsArray[0];
    // Check all four stored frames of input to see if stick distance traveled is ever less than 55.0f from the center
    for (i = 0; i < 4; i++, analogStickInput++, stickInput++) {
        if ((*stickInput = *analogStickInput) < 0) {
            return 0;
        }
        // Multiply each stored stickInput by 2
        *stickInput *= 2;
    }

    // Get diff between first two frames of stick input
    inputDiff1 = stickInputsArray[0] - stickInputsArray[1];
    // Return false if the difference is too small (< ~28 degrees) for the player to be spinning the analog stick
    if (ABS(inputDiff1) < 10) {
        return 0;
    }

    stickInput = &stickInputsArray[1];
    // *stickInput will be the second, then third frame of stick input in this loop
    for (i = 1; i < 3; i++, stickInput++) {
        // Get diff between current input frame and next input frame
        inputDiff2 = *stickInput - *(stickInput + 1);
        // If the difference too small, or stick has changed directions, return false
        if ((ABS(inputDiff2) < 10) || (inputDiff2 * inputDiff1 < 0)) {
            return 0;
        }
    }

    if (CVarGetInteger("gDisableMeleeAttacks", 0)) {
        return 0;
    }

    return 1;
}

void Player_SetupSpinAttackAnims(PlayState* play, Player* this) {
    LinkAnimationHeader* anim;

    if ((this->meleeWeaponAnimation >= 4) && (this->meleeWeaponAnimation < 8)) {
        anim = sSpinAttackAnims1[Player_HoldsTwoHandedWeapon(this)];
    } else {
        anim = sSpinAttackAnims2[Player_HoldsTwoHandedWeapon(this)];
    }

    Player_InactivateMeleeWeapon(this);
    LinkAnimation_Change(play, &this->skelAnime, anim, 1.0f, 8.0f, Animation_GetLastFrame(anim), ANIMMODE_ONCE,
                         -9.0f);
    Player_SetupSpinAttackActor(play, this, 0x200);
}

void Player_StartChargeSpinAttack(PlayState* play, Player* this) {
    Player_SetActionFunc(play, this, Player_ChargeSpinAttack, 1);
    Player_SetupSpinAttackAnims(play, this);
}

static s8 sMeleeWeaponAttackDirections[] = { 12, 4, 4, 8 };
static s8 sHammerAttackDirections[] = { 22, 23, 22, 23 };

s32 Player_GetMeleeAttackAnim(Player* this) {
    s32 relativeStickInput = this->relativeAnalogStickInputs[this->inputFrameCounter];
    s32 attackAnim;

    if (this->heldItemAction == PLAYER_IA_HAMMER) {
        if (relativeStickInput < 0) {
            relativeStickInput = 0;
        }
        attackAnim = sHammerAttackDirections[relativeStickInput];
        this->slashCounter = 0;
    } else {
        if (Player_CanQuickspin(this)) {
            attackAnim = 24;
        } else {
            if (relativeStickInput < 0) {
                if (Player_IsZTargeting(this)) {
                    attackAnim = 0;
                } else {
                    attackAnim = 4;
                }
            } else {
                attackAnim = sMeleeWeaponAttackDirections[relativeStickInput];
                if (attackAnim == 12) {
                    this->stateFlags2 |= PLAYER_STATE2_ENABLE_FORWARD_SLIDE_FROM_ATTACK;
                    if (!Player_IsZTargeting(this)) {
                        attackAnim = 0;
                    }
                }
            }
            if (this->heldItemAction == PLAYER_IA_STICK) {
                attackAnim = 0;
            }
        }
        if (Player_HoldsTwoHandedWeapon(this)) {
            attackAnim++;
        }
    }

    return attackAnim;
}

void Player_SetupMeleeWeaponToucherFlags(Player* this, s32 quadIndex, u32 flags) {
    this->meleeWeaponQuads[quadIndex].info.toucher.dmgFlags = flags;

    if (flags == 2) {
        this->meleeWeaponQuads[quadIndex].info.toucherFlags = TOUCH_ON | TOUCH_NEAREST | TOUCH_SFX_WOOD;
    } else {
        this->meleeWeaponQuads[quadIndex].info.toucherFlags = TOUCH_ON | TOUCH_NEAREST;
    }
}

static u32 sMeleeWeaponDmgFlags[][2] = {
    { 0x00000200, 0x08000000 }, { 0x00000100, 0x02000000 }, { 0x00000400, 0x04000000 },
    { 0x00000002, 0x08000000 }, { 0x00000040, 0x40000000 },
};

void Player_StartMeleeWeaponAttack(PlayState* play, Player* this, s32 arg2) {
    s32 pad;
    u32 flags;
    s32 temp;

    if (!CVarGetInteger("gDisableMeleeAttacks", 0)) {
        Player_SetActionFunc(play, this, Player_MeleeWeaponAttack, 0);
        this->comboTimer = 8;
        if ((arg2 < 18) || (arg2 >= 20)) {
            Player_InactivateMeleeWeapon(this);
        }

        if ((arg2 != this->meleeWeaponAnimation) || !(this->slashCounter < 3)) {
            this->slashCounter = 0;
        }

        this->slashCounter++;
        if (this->slashCounter >= 3) {
            arg2 += 2;
        }

        this->meleeWeaponAnimation = arg2;

        Player_PlayAnimOnceSlowed(play, this, sMeleeAttackAnims[arg2].unk_00);
        if ((arg2 != 16) && (arg2 != 17)) {
            Player_SetupAnimMovement(play, this, 0x209);
        }

        this->currentYaw = this->actor.shape.rot.y;

        if (Player_HoldsBrokenKnife(this)) {
            temp = 1;
        } else {
            temp = Player_GetSwordHeld(this) - 1;
        }

        if ((arg2 >= 16) && (arg2 < 20)) {
            if (CVarGetInteger("gRestoreQPA", 1) && temp == -1) {
                flags = 0x16171617;
            }
            else {
                flags = sMeleeWeaponDmgFlags[temp][1];
            }
        } else {
            flags = sMeleeWeaponDmgFlags[temp][0];
        }

        Player_SetupMeleeWeaponToucherFlags(this, 0, flags);
        Player_SetupMeleeWeaponToucherFlags(this, 1, flags);
    }
    else {
        Player_SetupReturnToStandStill(this, play);
    }
}

void Player_SetupInvincibility(Player* this, s32 timer) {
    if (this->invincibilityTimer >= 0) {
        this->invincibilityTimer = timer;
        this->damageFlashTimer = 0;
    }
}

void Player_SetupInvincibilityNoDamageFlash(Player* this, s32 timer) {
    if (this->invincibilityTimer > timer) {
        this->invincibilityTimer = timer;
    }
    this->damageFlashTimer = 0;
}

s32 Player_Damage_modified(PlayState* play, Player* this, s32 damage, u8 modified) {
    if ((this->invincibilityTimer != 0) || (this->actor.category != ACTORCAT_PLAYER)) {
        return 1;
    }

    s32 modifiedDamage = damage;
    if (modified) {
        modifiedDamage *= (1 << CVarGetInteger("gDamageMul", 0));
    }

    return Health_ChangeBy(play, modifiedDamage);
}

s32 Player_Damage(PlayState* play, Player* this, s32 damage) {
    return Player_Damage_modified(play, this, damage, true);
}

void Player_SetLedgeGrabPosition(Player* this) {
    this->skelAnime.prevTransl = this->skelAnime.jointTable[0];
    Player_UpdateAnimMovement(this, 3);
}

void Player_SetupFallFromLedge(Player* this, PlayState* play) {
    Player_SetActionFunc(play, this, Player_UpdateMidair, 0);
    Player_PlayAnimLoop(play, this, &gPlayerAnim_link_normal_landing_wait);
    this->genericTimer = 1;
    if (this->attentionMode != 3) {
        this->attentionMode = 0;
    }
}

static LinkAnimationHeader* sLinkDamageAnims[] = {
    &gPlayerAnim_link_normal_front_shit, &gPlayerAnim_link_normal_front_shitR, &gPlayerAnim_link_normal_back_shit,
    &gPlayerAnim_link_normal_back_shitR, &gPlayerAnim_link_normal_front_hit,   &gPlayerAnim_link_anchor_front_hitR,
    &gPlayerAnim_link_normal_back_hit,   &gPlayerAnim_link_anchor_back_hitR,
};

void Player_SetupDamage(PlayState* play, Player* this, s32 damageReaction, f32 knockbackVelXZ,
                        f32 knockbackVelY, s16 damageYaw, s32 invincibilityTimer) {
    LinkAnimationHeader* anim = NULL;
    LinkAnimationHeader** damageAnims;

    if (this->stateFlags1 & PLAYER_STATE1_HANGING_FROM_LEDGE_SLIP) {
        Player_SetLedgeGrabPosition(this);
    }

    this->runDamageTimer = 0;

    func_8002F7DC(&this->actor, NA_SE_PL_DAMAGE);

    if (!Player_Damage(play, this, 0 - this->actor.colChkInfo.damage)) {
        this->stateFlags2 &= ~PLAYER_STATE2_RESTRAINED_BY_ENEMY;
        if (!(this->actor.bgCheckFlags & 1) && !(this->stateFlags1 & PLAYER_STATE1_SWIMMING)) {
            Player_SetupFallFromLedge(this, play);
        }
        return;
    }

    Player_SetupInvincibility(this, invincibilityTimer);

    if (damageReaction == PLAYER_DMGREACTION_FROZEN) {
        Player_SetActionFunc(play, this, Player_FrozenInIce, 0);

        anim = &gPlayerAnim_link_normal_ice_down;

        Player_ClearAttentionModeAndStopMoving(this);
        Player_RequestRumble(this, 255, 10, 40, 0);

        func_8002F7DC(&this->actor, NA_SE_PL_FREEZE_S);
        Player_PlayVoiceSfxForAge(this, NA_SE_VO_LI_FREEZE);
    } else if (damageReaction == PLAYER_DMGREACTION_ELECTRIC_SHOCKED) {
        Player_SetActionFunc(play, this, Player_SetupElectricShock, 0);

        Player_RequestRumble(this, 255, 80, 150, 0);

        Player_PlayAnimLoopSlowed(play, this, &gPlayerAnim_link_normal_electric_shock);
        Player_ClearAttentionModeAndStopMoving(this);

        this->genericTimer = 20;
    } else {
        damageYaw -= this->actor.shape.rot.y;
        if (this->stateFlags1 & PLAYER_STATE1_SWIMMING) {
            Player_SetActionFunc(play, this, Player_DamagedSwim, 0);
            Player_RequestRumble(this, 180, 20, 50, 0);

            this->linearVelocity = 4.0f;
            this->actor.velocity.y = 0.0f;

            anim = &gPlayerAnim_link_swimer_swim_hit;

            Player_PlayVoiceSfxForAge(this, NA_SE_VO_LI_DAMAGE_S);
        } else if ((damageReaction == PLAYER_DMGREACTION_KNOCKBACK) || (damageReaction == PLAYER_DMGREACTION_HOP) ||
                   !(this->actor.bgCheckFlags & 1) ||
                   (this->stateFlags1 & (PLAYER_STATE1_HANGING_FROM_LEDGE_SLIP | PLAYER_STATE1_CLIMBING_ONTO_LEDGE |
                                         PLAYER_STATE1_CLIMBING))) {
            Player_SetActionFunc(play, this, Player_StartKnockback, 0);

            this->stateFlags3 |= PLAYER_STATE3_MIDAIR;
            this->stateFlags3 |= PLAYER_STATE3_MIDAIR;

            Player_RequestRumble(this, 255, 20, 150, 0);
            Player_ClearAttentionModeAndStopMoving(this);

            if (damageReaction == PLAYER_DMGREACTION_HOP) {
                this->genericTimer = 4;

                this->actor.speedXZ = 3.0f;
                this->linearVelocity = 3.0f;
                this->actor.velocity.y = 6.0f;

                Player_ChangeAnimOnce(play, this,
                                      GET_PLAYER_ANIM(PLAYER_ANIMGROUP_RUNNING_DAMAGED, this->modelAnimType));
                Player_PlayVoiceSfxForAge(this, NA_SE_VO_LI_DAMAGE_S);
            } else {
                this->actor.speedXZ = knockbackVelXZ;
                this->linearVelocity = knockbackVelXZ;
                this->actor.velocity.y = knockbackVelY;

                if (ABS(damageYaw) > 0x4000) {
                    anim = &gPlayerAnim_link_normal_front_downA;
                } else {
                    anim = &gPlayerAnim_link_normal_back_downA;
                }

                if ((this->actor.category != ACTORCAT_PLAYER) && (this->actor.colChkInfo.health == 0)) {
                    Player_PlayVoiceSfxForAge(this, NA_SE_VO_BL_DOWN);
                } else {
                    Player_PlayVoiceSfxForAge(this, NA_SE_VO_LI_FALL_L);
                }
            }

            this->hoverBootsTimer = 0;
            this->actor.bgCheckFlags &= ~1;
        } else {
            if ((this->linearVelocity > 4.0f) && !Player_IsUnfriendlyZTargeting(this)) {
                this->runDamageTimer = 20;
                Player_RequestRumble(this, 120, 20, 10, 0);
                Player_PlayVoiceSfxForAge(this, NA_SE_VO_LI_DAMAGE_S);
                return;
            }

            damageAnims = sLinkDamageAnims;

            Player_SetActionFunc(play, this, func_8084370C, 0);
            Player_ResetLeftRightBlendWeight(this);

            if (this->actor.colChkInfo.damage < 5) {
                Player_RequestRumble(this, 120, 20, 10, 0);
            } else {
                Player_RequestRumble(this, 180, 20, 100, 0);
                this->linearVelocity = 23.0f;
                damageAnims += 4;
            }

            if (ABS(damageYaw) <= 0x4000) {
                damageAnims += 2;
            }

            if (Player_IsUnfriendlyZTargeting(this)) {
                damageAnims += 1;
            }

            anim = *damageAnims;

            Player_PlayVoiceSfxForAge(this, NA_SE_VO_LI_DAMAGE_S);
        }

        this->actor.shape.rot.y += damageYaw;
        this->currentYaw = this->actor.shape.rot.y;
        this->actor.world.rot.y = this->actor.shape.rot.y;
        if (ABS(damageYaw) > 0x4000) {
            this->actor.shape.rot.y += 0x8000;
        }
    }

    Player_ResetAttributesAndHeldActor(play, this);

    this->stateFlags1 |= PLAYER_STATE1_TAKING_DAMAGE;

    if (anim != NULL) {
        Player_PlayAnimOnceSlowed(play, this, anim);
    }
}

s32 Player_GetHurtFloorType(s32 arg0) {
    s32 temp = arg0 - 2;

    if ((temp >= 0) && (temp < 2)) {
        return temp;
    } else {
        return -1;
    }
}

s32 Player_IsFloorSinkingSand(s32 arg0) {
    return (arg0 == 4) || (arg0 == 7) || (arg0 == 12);
}

void Player_BurnDekuShield(Player* this, PlayState* play) {
    if (this->currentShield == PLAYER_SHIELD_DEKU && (CVarGetInteger("gFireproofDekuShield", 0) == 0)) {
        Actor_Spawn(&play->actorCtx, play, ACTOR_ITEM_SHIELD, this->actor.world.pos.x,
                    this->actor.world.pos.y, this->actor.world.pos.z, 0, 0, 0, 1, true);
        Inventory_DeleteEquipment(play, EQUIP_SHIELD);
        Message_StartTextbox(play, 0x305F, NULL);
    }
}

void Player_StartBurning(Player* this) {
    s32 i;

    // clang-format off
    for (i = 0; i < 18; i++) { this->flameTimers[i] = Rand_S16Offset(0, 200); }
    // clang-format on

    this->isBurning = true;
}

void Player_PlayFallSfxAndCheckBurning(Player* this) {
    if (this->actor.colChkInfo.acHitEffect == 1 || CVarGetInteger("gFireDamage", 0)) {
        Player_StartBurning(this);
    }
    Player_PlayVoiceSfxForAge(this, NA_SE_VO_LI_FALL_L);
}

void Player_RoundUpInvincibilityTimer(Player* this) {
    if ((this->invincibilityTimer >= 0) && (this->invincibilityTimer < 20)) {
        this->invincibilityTimer = 20;
    }
}

s32 Player_UpdateDamage(Player* this, PlayState* play) {
    s32 pad;
    s32 sinkingGroundVoidOut = false;
    s32 attackHitShield;

    if (this->voidRespawnCounter != 0) {
        if (!Player_InBlockingCsMode(play, this)) {
            Player_InflictDamageModified(play, -16 * CVarGetInteger("gVoidDamageMul", 1), false);
            this->voidRespawnCounter = 0;
        }
    } else {
        sinkingGroundVoidOut = ((Player_GetHeight(this) - 8.0f) < (this->shapeOffsetY * this->actor.scale.y));

        if (sinkingGroundVoidOut || (this->actor.bgCheckFlags & 0x100) || (sFloorSpecialProperty == 9) ||
            (this->stateFlags2 & PLAYER_STATE2_FORCE_VOID_OUT)) {
            Player_PlayVoiceSfxForAge(this, NA_SE_VO_LI_DAMAGE_S);

            if (sinkingGroundVoidOut) {
                Play_TriggerRespawn(play);
                func_800994A0(play);
            } else {
                // Special case for getting crushed in Forest Temple's Checkboard Ceiling Hall or Shadow Temple's
                // Falling Spike Trap Room, to respawn the player in a specific place
                if (((play->sceneNum == SCENE_BMORI1) && (play->roomCtx.curRoom.num == 15)) ||
                    ((play->sceneNum == SCENE_HAKADAN) && (play->roomCtx.curRoom.num == 10))) {
                    static SpecialRespawnInfo checkboardCeilingRespawn = { { 1992.0f, 403.0f, -3432.0f }, 0 };
                    static SpecialRespawnInfo fallingSpikeTrapRespawn = { { 1200.0f, -1343.0f, 3850.0f }, 0 };
                    SpecialRespawnInfo* respawnInfo;

                    if (play->sceneNum == SCENE_BMORI1) {
                        respawnInfo = &checkboardCeilingRespawn;
                    } else {
                        respawnInfo = &fallingSpikeTrapRespawn;
                    }

                    Play_SetupRespawnPoint(play, RESPAWN_MODE_DOWN, 0xDFF);
                    gSaveContext.respawn[RESPAWN_MODE_DOWN].pos = respawnInfo->pos;
                    gSaveContext.respawn[RESPAWN_MODE_DOWN].yaw = respawnInfo->yaw;
                }

                Play_TriggerVoidOut(play);
            }

            Player_PlayVoiceSfxForAge(this, NA_SE_VO_LI_TAKEN_AWAY);
            play->unk_11DE9 = 1;
            func_80078884(NA_SE_OC_ABYSS);
        } else if ((this->damageEffect != 0) && ((this->damageEffect >= 2) || (this->invincibilityTimer == 0))) {
            u8 sp5C[] = { 2, 1, 1 };

            Player_PlayFallSfxAndCheckBurning(this);

            if (this->damageEffect == 3) {
                this->shockTimer = 40;
            }

            this->actor.colChkInfo.damage += this->damageAmount;
            Player_SetupDamage(play, this, sp5C[this->damageEffect - 1], this->knockbackVelXZ, this->knockbackVelY,
                               this->damageYaw, 20);
        } else {
            attackHitShield = (this->shieldQuad.base.acFlags & AC_BOUNCED) != 0;

            //! @bug The second set of conditions here seems intended as a way for Link to "block" hits by rolling.
            // However, `Collider.atFlags` is a byte so the flag check at the end is incorrect and cannot work.
            // Additionally, `Collider.atHit` can never be set while already colliding as AC, so it's also bugged.
            // This behavior was later fixed in MM, most likely by removing both the `atHit` and `atFlags` checks.
            if (attackHitShield ||
                ((this->invincibilityTimer < 0) && (this->cylinder.base.acFlags & AC_HIT) &&
                 (this->cylinder.info.atHit != NULL) && (this->cylinder.info.atHit->atFlags & 0x20000000))) {

                Player_RequestRumble(this, 180, 20, 100, 0);

                if (!Player_IsChildWithHylianShield(this)) {
                    if (this->invincibilityTimer >= 0) {
                        LinkAnimationHeader* anim;
                        s32 sp54 = Player_AimShieldCrouched == this->actionFunc;

                        if (!Player_IsSwimming(this)) {
                            Player_SetActionFunc(play, this, Player_DeflectAttackWithShield, 0);
                        }

                        if (!(this->genericVar = sp54)) {
                            Player_SetUpperActionFunc(this, Player_EndDeflectAttackStanding);

                            if (this->leftRightBlendWeight < 0.5f) {
                                anim = sRightStandingDeflectWithShieldAnims[Player_HoldsTwoHandedWeapon(this) &&
                                                                            !(CVarGetInteger("gShieldTwoHanded", 0) &&
                                                                              (this->heldItemAction !=
                                                                               PLAYER_IA_STICK))];
                            } else {
                                anim = sLeftStandingDeflectWithShieldAnims[Player_HoldsTwoHandedWeapon(this) &&
                                                                           !(CVarGetInteger("gShieldTwoHanded", 0) &&
                                                                             (this->heldItemAction !=
                                                                              PLAYER_IA_STICK))];
                            }
                            LinkAnimation_PlayOnce(play, &this->skelAnimeUpper, anim);
                        } else {
                            Player_PlayAnimOnce(
                                play, this,
                                sDeflectWithShieldAnims[Player_HoldsTwoHandedWeapon(this) &&
                                                        !(CVarGetInteger("gShieldTwoHanded", 0) &&
                                                          (this->heldItemAction != PLAYER_IA_STICK))]);
                        }
                    }

                    if (!(this->stateFlags1 & (PLAYER_STATE1_HANGING_FROM_LEDGE_SLIP |
                                               PLAYER_STATE1_CLIMBING_ONTO_LEDGE | PLAYER_STATE1_CLIMBING))) {
                        this->linearVelocity = -18.0f;
                        this->currentYaw = this->actor.shape.rot.y;
                    }
                }

                if (attackHitShield && (this->shieldQuad.info.acHitInfo->toucher.effect == PLAYER_HITEFFECTAC_FIRE)) {
                    Player_BurnDekuShield(this, play);
                }

                return 0;
            }

            if ((this->deathTimer != 0) || (this->invincibilityTimer > 0) ||
                (this->stateFlags1 & PLAYER_STATE1_TAKING_DAMAGE) || (this->csMode != 0) ||
                (this->meleeWeaponQuads[0].base.atFlags & AT_HIT) || (this->meleeWeaponQuads[1].base.atFlags & AT_HIT)) {
                return 0;
            }

            if (this->cylinder.base.acFlags & AC_HIT) {
                Actor* ac = this->cylinder.base.ac;
                s32 damageReaction;

                if (ac->flags & ACTOR_FLAG_PLAY_HIT_SFX) {
                    func_8002F7DC(&this->actor, NA_SE_PL_BODY_HIT);
                }

                u8 damageOverride = CVarGetInteger("gIceDamage", 0) || CVarGetInteger("gElectricDamage", 0) ||
                                    CVarGetInteger("gKnockbackDamage", 0);

                if (!damageOverride) {
                    if (this->stateFlags1 & PLAYER_STATE1_SWIMMING) {
                        damageReaction = PLAYER_DMGREACTION_DEFAULT;
                    } else if (this->actor.colChkInfo.acHitEffect == PLAYER_HITEFFECTAC_ICE) {
                        damageReaction = PLAYER_DMGREACTION_FROZEN;
                    } else if (this->actor.colChkInfo.acHitEffect == PLAYER_HITEFFECTAC_ELECTRIC) {
                        damageReaction = PLAYER_DMGREACTION_ELECTRIC_SHOCKED;
                    } else if (this->actor.colChkInfo.acHitEffect == PLAYER_HITEFFECTAC_POWERFUL_HIT) {
                        damageReaction = PLAYER_DMGREACTION_KNOCKBACK;
                    } else {
                        Player_PlayFallSfxAndCheckBurning(this);
                        damageReaction = PLAYER_DMGREACTION_DEFAULT;
                    }
                } else {
                    if (CVarGetInteger("gIceDamage", 0)) {
                        damageReaction = PLAYER_DMGREACTION_FROZEN;
                    }
                    if (CVarGetInteger("gElectricDamage", 0)) {
                        damageReaction = PLAYER_DMGREACTION_ELECTRIC_SHOCKED;
                    }
                    if (CVarGetInteger("gKnockbackDamage", 0)) {
                        damageReaction = PLAYER_DMGREACTION_KNOCKBACK;
                    }
                }

                Player_SetupDamage(play, this, damageReaction, 4.0f, 5.0f,
                                   Actor_WorldYawTowardActor(ac, &this->actor), 20);
            } else if (this->invincibilityTimer != 0) {
                return 0;
            } else {
                static u8 D_808544F4[] = { 120, 60 };
                s32 hurtFloorType = Player_GetHurtFloorType(sFloorSpecialProperty);

                if (((this->actor.wallPoly != NULL) &&
                     SurfaceType_IsWallDamage(&play->colCtx, this->actor.wallPoly, this->actor.wallBgId)) ||
                    ((hurtFloorType >= 0) &&
                     SurfaceType_IsWallDamage(&play->colCtx, this->actor.floorPoly, this->actor.floorBgId) &&
                     (this->hurtFloorTimer >= D_808544F4[hurtFloorType])) ||
                    ((hurtFloorType >= 0) && ((this->currentTunic != PLAYER_TUNIC_GORON && CVarGetInteger("gSuperTunic", 0) == 0) ||
                                     (this->hurtFloorTimer >= D_808544F4[hurtFloorType]))) ||
                    (CVarGetInteger("gFloorIsLava", 0) && this->actor.bgCheckFlags & BGCHECKFLAG_GROUND)) {
                    this->hurtFloorTimer = 0;
                    this->actor.colChkInfo.damage = 4;
                    Player_SetupDamage(play, this, 0, 4.0f, 5.0f, this->actor.shape.rot.y, 20);
                } else {
                    return 0;
                }
            }
        }
    }

    return 1;
}

void Player_SetupJumpWithSfx(Player* this, LinkAnimationHeader* anim, f32 arg2, PlayState* play, u16 sfxId) {
    Player_SetActionFunc(play, this, Player_UpdateMidair, 1);

    if (anim != NULL) {
        Player_PlayAnimOnceSlowed(play, this, anim);
    }

    this->actor.velocity.y = arg2 * sWaterSpeedScale;
    this->hoverBootsTimer = 0;
    this->actor.bgCheckFlags &= ~1;

    Player_PlayJumpSfx(this);
    Player_PlayVoiceSfxForAge(this, sfxId);

    this->stateFlags1 |= PLAYER_STATE1_JUMPING;
    this->stateFlags1 |= PLAYER_STATE1_JUMPING;
}

void Player_SetupJump(Player* this, LinkAnimationHeader* anim, f32 arg2, PlayState* play) {
    Player_SetupJumpWithSfx(this, anim, arg2, play, NA_SE_VO_LI_SWORD_N);
}

s32 Player_SetupWallJumpBehavior(Player* this, PlayState* play) {
    s32 canJumpToLedge;
    LinkAnimationHeader* anim;
    f32 wallHeight;
    f32 yVel;
    f32 wallPolyNormalX;
    f32 wallPolyNormalZ;
    f32 wallDist;

    if (!(this->stateFlags1 & PLAYER_STATE1_HOLDING_ACTOR) && (this->touchedWallJumpType >= 2) &&
        (!(this->stateFlags1 & PLAYER_STATE1_SWIMMING) || (this->ageProperties->unk_14 > this->wallHeight))) {
        canJumpToLedge = 0;

        if (Player_IsSwimming(this)) {
            if (this->actor.yDistToWater < 50.0f) {
                if ((this->touchedWallJumpType < 2) || (this->wallHeight > this->ageProperties->unk_10)) {
                    return 0;
                }
            } else if ((this->currentBoots != PLAYER_BOOTS_IRON) || (this->touchedWallJumpType > 2)) {
                return 0;
            }
        } else if (!(this->actor.bgCheckFlags & 1) ||
                   ((this->ageProperties->unk_14 <= this->wallHeight) && (this->stateFlags1 & PLAYER_STATE1_SWIMMING))) {
            return 0;
        }

        if ((this->actor.wallBgId != BGCHECK_SCENE) && (sTouchedWallFlags & 0x40)) {
            if (this->wallTouchTimer >= 6) {
                this->stateFlags2 |= PLAYER_STATE2_CAN_CLIMB_PUSH_PULL_WALL;
                if (CHECK_BTN_ALL(sControlInput->press.button, BTN_A)) {
                    canJumpToLedge = 1;
                }
            }
        } else if ((this->wallTouchTimer >= 6) || CHECK_BTN_ALL(sControlInput->press.button, BTN_A)) {
            canJumpToLedge = 1;
        }

        // Chaos
        if (CVarGetInteger("gDisableLedges", 0)) {
            canJumpToLedge = 0;
        }

        if (canJumpToLedge != 0) {
            Player_SetActionFunc(play, this, Player_JumpUpToLedge, 0);

            this->stateFlags1 |= PLAYER_STATE1_JUMPING;

            wallHeight = this->wallHeight;

            if (this->ageProperties->unk_14 <= wallHeight) {
                anim = &gPlayerAnim_link_normal_250jump_start;
                this->linearVelocity = 1.0f;
            } else {
                wallPolyNormalX = COLPOLY_GET_NORMAL(this->actor.wallPoly->normal.x);
                wallPolyNormalZ = COLPOLY_GET_NORMAL(this->actor.wallPoly->normal.z);
                wallDist = this->wallDistance + 0.5f;

                this->stateFlags1 |= PLAYER_STATE1_CLIMBING_ONTO_LEDGE;

                if (Player_IsSwimming(this)) {
                    anim = &gPlayerAnim_link_swimer_swim_15step_up;
                    wallHeight -= (60.0f * this->ageProperties->unk_08);
                    this->stateFlags1 &= ~PLAYER_STATE1_SWIMMING;
                } else if (this->ageProperties->unk_18 <= wallHeight) {
                    anim = &gPlayerAnim_link_normal_150step_up;
                    wallHeight -= (59.0f * this->ageProperties->unk_08);
                } else {
                    anim = &gPlayerAnim_link_normal_100step_up;
                    wallHeight -= (41.0f * this->ageProperties->unk_08);
                }

                this->actor.shape.yOffset -= wallHeight * 100.0f;

                this->actor.world.pos.x -= wallDist * wallPolyNormalX;
                this->actor.world.pos.y += this->wallHeight;
                this->actor.world.pos.z -= wallDist * wallPolyNormalZ;

                Player_ClearAttentionModeAndStopMoving(this);
            }

            this->actor.bgCheckFlags |= 1;

            LinkAnimation_PlayOnceSetSpeed(play, &this->skelAnime, anim, 1.3f);
            AnimationContext_DisableQueue(play);

            this->actor.shape.rot.y = this->currentYaw = this->actor.wallYaw + 0x8000;

            return 1;
        }
    } else if ((this->actor.bgCheckFlags & 1) && (this->touchedWallJumpType == 1) && (this->wallTouchTimer >= 3)) {
        yVel = (this->wallHeight * 0.08f) + 5.5f;
        Player_SetupJump(this, &gPlayerAnim_link_normal_jump, yVel, play);
        this->linearVelocity = 2.5f;

        return 1;
    }

    return 0;
}

void Player_SetupMiniCsMovement(PlayState* play, Player* this, f32 arg2, s16 arg3) {
    Player_SetActionFunc(play, this, Player_MiniCsMovement, 0);
    Player_ResetAttributes(play, this);

    this->genericVar = 1;
    this->genericTimer = 1;

    this->csStartPos.x = (Math_SinS(arg3) * arg2) + this->actor.world.pos.x;
    this->csStartPos.z = (Math_CosS(arg3) * arg2) + this->actor.world.pos.z;

    Player_PlayAnimOnce(play, this, Player_GetStandingStillAnim(this));
}

void Player_SetupSwimIdle(PlayState* play, Player* this) {
    Player_SetActionFunc(play, this, Player_UpdateSwimIdle, 0);
    Player_ChangeAnimLongMorphLoop(play, this, &gPlayerAnim_link_swimer_swim_wait);
}

void Player_SetupEnterGrotto(PlayState* play, Player* this) {
    Player_SetActionFunc(play, this, Player_EnterGrotto, 0);

    this->stateFlags1 |= PLAYER_STATE1_IN_CUTSCENE | PLAYER_STATE1_FALLING_INTO_GROTTO_OR_VOID;

    Camera_ChangeSetting(Play_GetCamera(play, 0), CAM_SET_FREE0);
}

s32 Player_ShouldEnterGrotto(PlayState* play, Player* this) {
    if ((play->sceneLoadFlag == 0) && (this->stateFlags1 & PLAYER_STATE1_FALLING_INTO_GROTTO_OR_VOID)) {
        Player_SetupEnterGrotto(play, this);
        Player_PlayAnimLoop(play, this, &gPlayerAnim_link_normal_landing_wait);
        Player_PlayVoiceSfxForAge(this, NA_SE_VO_LI_FALL_S);
        func_800788CC(NA_SE_OC_SECRET_WARP_IN);
        return 1;
    }

    return 0;
}

s16 sReturnEntranceGroupData[] = {
    0x045B, // DMT from Magic Fairy Fountain
    0x0482, // DMC from Double Defense Fairy Fountain
    0x0340, // Hyrule Castle from Dins Fire Fairy Fountain
    0x044B, // Kakariko from Potion Shop
    0x02A2, // Market (child day) from Potion Shop
    0x0201, // Kakariko from Bazaar
    0x03B8, // Market (child day) from Bazaar
    0x04EE, // Kakariko from House of Skulltulas
    0x03C0, // Back Alley (day) from Bombchu Shop
    0x0463, // Kakariko from Shooting Gallery
    0x01CD, // Market (child day) from Shooting Gallery
    0x0394, // Zoras Fountain from Farores Wind Fairy Fountain
    0x0340, // Hyrule Castle from Dins Fire Fairy Fountain
    0x057C, // Desert Colossus from Nayrus Love Fairy Fountain
};

/**
 * The values are indices into `sReturnEntranceGroupData` marking the start of each group
 */
u8 sReturnEntranceGroupIndices[] = {
    11, // ENTR_RETURN_YOUSEI_IZUMI_YOKO
    9,  // ENTR_RETURN_SYATEKIJYOU
    3,  // ENTR_RETURN_2
    5,  // ENTR_RETURN_SHOP1
    7,  // ENTR_RETURN_4
    0,  // ENTR_RETURN_DAIYOUSEI_IZUMI
};

s32 Player_SetupExit(PlayState* play, Player* this, CollisionPoly* poly, u32 bgId) {
    s32 exitIndex;
    s32 floorSpecialProperty;
    s32 yDistToExit;
    f32 linearVel;
    s32 yaw;

    if (this->actor.category == ACTORCAT_PLAYER) {
        exitIndex = 0;

        if (!(this->stateFlags1 & PLAYER_STATE1_IN_DEATH_CUTSCENE) && (play->sceneLoadFlag == 0) &&
            (this->csMode == 0) && !(this->stateFlags1 & PLAYER_STATE1_EXITING_SCENE) &&
            (((poly != NULL) && (exitIndex = SurfaceType_GetSceneExitIndex(&play->colCtx, poly, bgId), exitIndex != 0)) ||
             (Player_IsFloorSinkingSand(sFloorSpecialProperty) && (this->floorProperty == 12)))) {

            yDistToExit = this->sceneExitPosY - (s32)this->actor.world.pos.y;

            if (!(this->stateFlags1 &
                  (PLAYER_STATE1_RIDING_HORSE | PLAYER_STATE1_SWIMMING | PLAYER_STATE1_IN_CUTSCENE)) &&
                !(this->actor.bgCheckFlags & 1) && (yDistToExit < 100) && (sPlayerYDistToFloor > 100.0f)) {
                return 0;
            }

            if (exitIndex == 0) {
                Play_TriggerVoidOut(play);
                func_800994A0(play);
            } else {
                play->nextEntranceIndex = play->setupExitList[exitIndex - 1];

                // Main override for entrance rando and entrance skips
                if (gSaveContext.n64ddFlag) {
                    play->nextEntranceIndex = Entrance_OverrideNextIndex(play->nextEntranceIndex);
                }

                if (play->nextEntranceIndex == 0x7FFF) {
                    gSaveContext.respawnFlag = 2;
                    play->nextEntranceIndex = gSaveContext.respawn[RESPAWN_MODE_RETURN].entranceIndex;
                    play->fadeTransition = 3;
                    gSaveContext.nextTransitionType = 3;
                } else if (play->nextEntranceIndex >= 0x7FF9) {
                    // handle dynamic exits
                    if (gSaveContext.n64ddFlag) {
                        play->nextEntranceIndex =
                            Entrance_OverrideDynamicExit(sReturnEntranceGroupIndices[play->nextEntranceIndex - 0x7FF9] + play->curSpawn);
                    } else {
                        play->nextEntranceIndex =
                            sReturnEntranceGroupData[sReturnEntranceGroupIndices[play->nextEntranceIndex - 0x7FF9] + play->curSpawn];
                    }

                    func_800994A0(play);
                } else {
                    if (SurfaceType_GetSlope(&play->colCtx, poly, bgId) == 2) {
                        gSaveContext.respawn[RESPAWN_MODE_DOWN].entranceIndex = play->nextEntranceIndex;
                        Play_TriggerVoidOut(play);
                        gSaveContext.respawnFlag = -2;
                    }
                    gSaveContext.retainWeatherMode = 1;
                    func_800994A0(play);
                }
                play->sceneLoadFlag = 0x14;
            }

            if (!(this->stateFlags1 & (PLAYER_STATE1_RIDING_HORSE | PLAYER_STATE1_IN_CUTSCENE)) &&
                !(this->stateFlags2 & PLAYER_STATE2_INSIDE_CRAWLSPACE) && !Player_IsSwimming(this) &&
                (floorSpecialProperty = func_80041D4C(&play->colCtx, poly, bgId), (floorSpecialProperty != 10)) &&
                ((yDistToExit < 100) || (this->actor.bgCheckFlags & 1))) {

                if (floorSpecialProperty == 11) {
                    func_800788CC(NA_SE_OC_SECRET_HOLE_OUT);
                    func_800F6964(5);
                    gSaveContext.seqId = (u8)NA_BGM_DISABLED;
                    gSaveContext.natureAmbienceId = NATURE_ID_DISABLED;
                } else {
                    linearVel = this->linearVelocity;

                    if (linearVel < 0.0f) {
                        this->actor.world.rot.y += 0x8000;
                        linearVel = -linearVel;
                    }

                    if (linearVel > R_RUN_SPEED_LIMIT / 100.0f) {
                        gSaveContext.entranceSpeed = R_RUN_SPEED_LIMIT / 100.0f;
                    } else {
                        gSaveContext.entranceSpeed = linearVel;
                    }

                    if (sConveyorSpeedIndex != 0) {
                        yaw = sConveyorYaw;
                    } else {
                        yaw = this->actor.world.rot.y;
                    }
                    Player_SetupMiniCsMovement(play, this, 400.0f, yaw);
                }
            } else {
                if (!(this->actor.bgCheckFlags & 1)) {
                    Player_StopMovement(this);
                }
            }

            this->stateFlags1 |= PLAYER_STATE1_EXITING_SCENE | PLAYER_STATE1_IN_CUTSCENE;

            Player_ChangeCameraSetting(play, 0x2F);

            return 1;
        } else {
            if (play->sceneLoadFlag == 0) {

                if ((this->actor.world.pos.y < -4000.0f) ||
                    (((this->floorProperty == 5) || (this->floorProperty == 12)) &&
                     ((sPlayerYDistToFloor < 100.0f) || (this->fallDistance > 400.0f) ||
                      ((play->sceneNum != SCENE_HAKADAN) && (this->fallDistance > 200.0f)))) ||
                    ((play->sceneNum == SCENE_GANON_FINAL) && (this->fallDistance > 320.0f))) {

                    if (this->actor.bgCheckFlags & 1) {
                        if (this->floorProperty == 5) {
                            Play_TriggerRespawn(play);
                        } else {
                            Play_TriggerVoidOut(play);
                        }
                        play->fadeTransition = 4;
                        func_80078884(NA_SE_OC_ABYSS);
                    } else {
                        Player_SetupEnterGrotto(play, this);
                        this->genericTimer = 9999;
                        if (this->floorProperty == 5) {
                            this->genericVar = -1;
                        } else {
                            this->genericVar = 1;
                        }
                    }
                }

                this->sceneExitPosY = this->actor.world.pos.y;
            }
        }
    }

    return 0;
}

void Player_GetWorldPosRelativeToPlayer(Player* this, Vec3f* arg1, Vec3f* arg2, Vec3f* arg3) {
    f32 cos = Math_CosS(this->actor.shape.rot.y);
    f32 sin = Math_SinS(this->actor.shape.rot.y);

    arg3->x = arg1->x + ((arg2->x * cos) + (arg2->z * sin));
    arg3->y = arg1->y + arg2->y;
    arg3->z = arg1->z + ((arg2->z * cos) - (arg2->x * sin));
}

Actor* Player_SpawnFairy(PlayState* play, Player* this, Vec3f* arg2, Vec3f* arg3, s32 type) {
    Vec3f pos;

    Player_GetWorldPosRelativeToPlayer(this, arg2, arg3, &pos);

    return Actor_Spawn(&play->actorCtx, play, ACTOR_EN_ELF, pos.x, pos.y, pos.z, 0, 0, 0, type, true);
}

f32 Player_RaycastFloorWithOffset(PlayState* play, Player* this, Vec3f* raycastPosOffset, Vec3f* raycastPos, CollisionPoly** colPoly, s32* bgId) {
    Player_GetWorldPosRelativeToPlayer(this, &this->actor.world.pos, raycastPosOffset, raycastPos);

    return BgCheck_EntityRaycastFloor3(&play->colCtx, colPoly, bgId, raycastPos);
}

f32 Player_RaycastFloorWithOffset2(PlayState* play, Player* this, Vec3f* raycastPosOffset, Vec3f* raycastPos) {
    CollisionPoly* colPoly;
    s32 polyBgId;

    return Player_RaycastFloorWithOffset(play, this, raycastPosOffset, raycastPos, &colPoly, &polyBgId);
}

s32 Player_WallLineTestWithOffset(PlayState* play, Player* this, Vec3f* posOffset, CollisionPoly** wallPoly, s32* bgId, Vec3f* posResult) {
    Vec3f sp44;
    Vec3f sp38;

    sp44.x = this->actor.world.pos.x;
    sp44.y = this->actor.world.pos.y + posOffset->y;
    sp44.z = this->actor.world.pos.z;

    Player_GetWorldPosRelativeToPlayer(this, &this->actor.world.pos, posOffset, &sp38);

    return BgCheck_EntityLineTest1(&play->colCtx, &sp44, &sp38, posResult, wallPoly, true, false, false, true, bgId);
}

s32 Player_SetupOpenDoor(Player* this, PlayState* play) {
    DoorShutter* doorShutter;
    EnDoor* door; // Can also be DoorKiller*
    s32 doorDirection;
    f32 cos;
    f32 sin;
    Actor* doorActor;
    f32 doorOpeningPosOffset;
    s32 pad3;
    s32 frontRoom;
    Actor* attachedActor;
    LinkAnimationHeader* anim;
    CollisionPoly* floorPoly;
    Vec3f raycastPos;

    if ((this->doorType != PLAYER_DOORTYPE_NONE) &&
        (!(this->stateFlags1 & PLAYER_STATE1_HOLDING_ACTOR) ||
         ((this->heldActor != NULL) && (this->heldActor->id == ACTOR_EN_RU1)))) {
        // Disable doors in Boss Rush so the player can't leave the boss rooms backwards.
        if ((CHECK_BTN_ALL(sControlInput->press.button, BTN_A) || (Player_SetupOpenDoorFromSpawn == this->actionFunc)) && !gSaveContext.isBossRush) {
            doorActor = this->doorActor;

            if (this->doorType <= PLAYER_DOORTYPE_AJAR) {
                doorActor->textId = 0xD0;
                Player_StartTalkingWithActor(play, doorActor);
                return 0;
            }

            doorDirection = this->doorDirection;
            cos = Math_CosS(doorActor->shape.rot.y);
            sin = Math_SinS(doorActor->shape.rot.y);

            if (this->doorType == PLAYER_DOORTYPE_SLIDING) {
                doorShutter = (DoorShutter*)doorActor;

                this->currentYaw = doorShutter->dyna.actor.home.rot.y;
                if (doorDirection > 0) {
                    this->currentYaw -= 0x8000;
                }
                this->actor.shape.rot.y = this->currentYaw;

                if (this->linearVelocity <= 0.0f) {
                    this->linearVelocity = 0.1f;
                }

                Player_SetupMiniCsMovement(play, this, 50.0f, this->actor.shape.rot.y);

                this->genericVar = 0;
                this->csDoorType = this->doorType;
                this->stateFlags1 |= PLAYER_STATE1_IN_CUTSCENE;

                this->csStartPos.x = this->actor.world.pos.x + ((doorDirection * 20.0f) * sin);
                this->csStartPos.z = this->actor.world.pos.z + ((doorDirection * 20.0f) * cos);
                this->csEndPos.x = this->actor.world.pos.x + ((doorDirection * -120.0f) * sin);
                this->csEndPos.z = this->actor.world.pos.z + ((doorDirection * -120.0f) * cos);

                doorShutter->unk_164 = 1;
                Player_ClearAttentionModeAndStopMoving(this);

                if (this->doorTimer != 0) {
                    this->genericTimer = 0;
                    Player_ChangeAnimMorphToLastFrame(play, this, Player_GetStandingStillAnim(this));
                    this->skelAnime.endFrame = 0.0f;
                } else {
                    this->linearVelocity = 0.1f;
                }

                if (doorShutter->dyna.actor.category == ACTORCAT_DOOR) {
                    this->doorBgCamIndex = play->transiActorCtx.list[(u16)doorShutter->dyna.actor.params >> 10]
                                        .sides[(doorDirection > 0) ? 0 : 1]
                                        .effects;

                    Actor_DisableLens(play);
                }
            } else {
                // This actor can be either EnDoor or DoorKiller.
                // Don't try to access any struct vars other than `animStyle` and `playerIsOpening`! These two variables
                // are common across the two actors' structs however most other variables are not!
                door = (EnDoor*)doorActor;

                door->animStyle = (doorDirection < 0.0f) ? (LINK_IS_ADULT ? KNOB_ANIM_ADULT_L : KNOB_ANIM_CHILD_L)
                                                         : (LINK_IS_ADULT ? KNOB_ANIM_ADULT_R : KNOB_ANIM_CHILD_R);

                if (door->animStyle == KNOB_ANIM_ADULT_L) {
                    anim = GET_PLAYER_ANIM(PLAYER_ANIMGROUP_OPEN_DOOR_ADULT_LEFT, this->modelAnimType);
                } else if (door->animStyle == KNOB_ANIM_CHILD_L) {
                    anim = GET_PLAYER_ANIM(PLAYER_ANIMGROUP_OPEN_DOOR_CHILD_LEFT, this->modelAnimType);
                } else if (door->animStyle == KNOB_ANIM_ADULT_R) {
                    anim = GET_PLAYER_ANIM(PLAYER_ANIMGROUP_OPEN_DOOR_ADULT_RIGHT, this->modelAnimType);
                } else {
                    anim = GET_PLAYER_ANIM(PLAYER_ANIMGROUP_OPEN_DOOR_CHILD_RIGHT, this->modelAnimType);
                }

                Player_SetActionFunc(play, this, Player_OpenDoor, 0);
                Player_UnequipItem(play, this);

                if (doorDirection < 0) {
                    this->actor.shape.rot.y = doorActor->shape.rot.y;
                } else {
                    this->actor.shape.rot.y = doorActor->shape.rot.y - 0x8000;
                }

                this->currentYaw = this->actor.shape.rot.y;

                doorOpeningPosOffset = (doorDirection * 22.0f);
                this->actor.world.pos.x = doorActor->world.pos.x + doorOpeningPosOffset * sin;
                this->actor.world.pos.z = doorActor->world.pos.z + doorOpeningPosOffset * cos;

                Player_PlayAnimOnceWithWaterInfluence(play, this, anim);

                if (this->doorTimer != 0) {
                    this->skelAnime.endFrame = 0.0f;
                }

                Player_ClearAttentionModeAndStopMoving(this);
                Player_SetupAnimMovement(play, this, 0x28F);

                if (doorActor->parent != NULL) {
                    doorDirection = -doorDirection;
                }

                door->playerIsOpening = 1;

                if (this->doorType != PLAYER_DOORTYPE_FAKE) {
                    this->stateFlags1 |= PLAYER_STATE1_IN_CUTSCENE;
                    Actor_DisableLens(play);

                    if (((doorActor->params >> 7) & 7) == 3) {
                        raycastPos.x = doorActor->world.pos.x - (doorOpeningPosOffset * sin);
                        raycastPos.y = doorActor->world.pos.y + 10.0f;
                        raycastPos.z = doorActor->world.pos.z - (doorOpeningPosOffset * cos);

                        BgCheck_EntityRaycastFloor1(&play->colCtx, &floorPoly, &raycastPos);

                        if (Player_SetupExit(play, this, floorPoly, BGCHECK_SCENE)) {
                            gSaveContext.entranceSpeed = 2.0f;
                            gSaveContext.entranceSound = NA_SE_OC_DOOR_OPEN;
                        }
                    } else {
                        Camera_ChangeDoorCam(Play_GetCamera(play, 0), doorActor,
                                             play->transiActorCtx.list[(u16)doorActor->params >> 10]
                                                 .sides[(doorDirection > 0) ? 0 : 1]
                                                 .effects,
                                             0, 38.0f * sInvertedWaterSpeedScale, 26.0f * sInvertedWaterSpeedScale,
                                             10.0f * sInvertedWaterSpeedScale);
                    }
                }
            }

            if ((this->doorType != PLAYER_DOORTYPE_FAKE) && (doorActor->category == ACTORCAT_DOOR)) {
                frontRoom = play->transiActorCtx.list[(u16)doorActor->params >> 10]
                                .sides[(doorDirection > 0) ? 0 : 1]
                                .room;

                if ((frontRoom >= 0) && (frontRoom != play->roomCtx.curRoom.num)) {
                    func_8009728C(play, &play->roomCtx, frontRoom);
                }
            }

            doorActor->room = play->roomCtx.curRoom.num;

            if (((attachedActor = doorActor->child) != NULL) || ((attachedActor = doorActor->parent) != NULL)) {
                attachedActor->room = play->roomCtx.curRoom.num;
            }

            return 1;
        }
    }

    return 0;
}

void Player_SetupUnfriendlyZTargetStandStill(Player* this, PlayState* play) {
    LinkAnimationHeader* anim;

    Player_SetActionFunc(play, this, Player_UnfriendlyZTargetStandingStill, 1);

    if (this->leftRightBlendWeight < 0.5f) {
        anim = Player_GetFightingRightAnim(this);
        this->leftRightBlendWeight = 0.0f;
    } else {
        anim = Player_GetFightingLeftAnim(this);
        this->leftRightBlendWeight = 1.0f;
    }

    this->leftRightBlendWeightTarget = this->leftRightBlendWeight;
    Player_PlayAnimLoop(play, this, anim);
    this->currentYaw = this->actor.shape.rot.y;
}

void Player_SetupFriendlyZTargetingStandStill(Player* this, PlayState* play) {
    Player_SetActionFunc(play, this, Player_FriendlyZTargetStandingStill, 1);
    Player_ChangeAnimMorphToLastFrame(play, this, Player_GetStandingStillAnim(this));
    this->currentYaw = this->actor.shape.rot.y;
}

void Player_SetupStandingStillType(Player* this, PlayState* play) {
    if (Player_IsUnfriendlyZTargeting(this)) {
        Player_SetupUnfriendlyZTargetStandStill(this, play);
    } else if (Player_IsFriendlyZTargeting(this)) {
        Player_SetupFriendlyZTargetingStandStill(this, play);
    } else {
        Player_SetupStandingStillMorph(this, play);
    }
}

void Player_ReturnToStandStill(Player* this, PlayState* play) {
    PlayerActionFunc func;

    if (Player_IsUnfriendlyZTargeting(this)) {
        func = Player_UnfriendlyZTargetStandingStill;
    } else if (Player_IsFriendlyZTargeting(this)) {
        func = Player_FriendlyZTargetStandingStill;
    } else {
        func = Player_StandingStill;
    }

    Player_SetActionFunc(play, this, func, 1);
}

void Player_SetupReturnToStandStill(Player* this, PlayState* play) {
    Player_ReturnToStandStill(this, play);
    if (Player_IsUnfriendlyZTargeting(this)) {
        this->genericTimer = 1;
    }
}

void Player_SetupReturnToStandStillSetAnim(Player* this, LinkAnimationHeader* anim, PlayState* play) {
    Player_SetupReturnToStandStill(this, play);
    Player_PlayAnimOnceWithWaterInfluence(play, this, anim);
}

s32 Player_CanHoldActor(Player* this) {
    return (this->interactRangeActor != NULL) && (this->heldActor == NULL);
}

void Player_SetupHoldActor(PlayState* play, Player* this) {
    if (Player_CanHoldActor(this)) {
        Actor* interactRangeActor = this->interactRangeActor;
        s32 interactActorId = interactRangeActor->id;

        if (interactActorId == ACTOR_BG_TOKI_SWD) {
            this->interactRangeActor->parent = &this->actor;
            Player_SetActionFunc(play, this, Player_SetDrawAndStartCutsceneAfterTimer, 0);
            this->stateFlags1 |= PLAYER_STATE1_IN_CUTSCENE;
            sMaskMemory = PLAYER_MASK_NONE;
        } else {
            LinkAnimationHeader* anim;

            if (interactActorId == ACTOR_BG_HEAVY_BLOCK) {
                Player_SetActionFunc(play, this, Player_ThrowStonePillar, 0);
                this->stateFlags1 |= PLAYER_STATE1_IN_CUTSCENE;
                anim = &gPlayerAnim_link_normal_heavy_carry;
            } else if ((interactActorId == ACTOR_EN_ISHI) && ((interactRangeActor->params & 0xF) == 1)) {
                Player_SetActionFunc(play, this, Player_LiftSilverBoulder, 0);
                anim = &gPlayerAnim_link_silver_carry;
            } else if (((interactActorId == ACTOR_EN_BOMBF) || (interactActorId == ACTOR_EN_KUSA)) &&
                       (Player_GetStrength() <= PLAYER_STR_NONE)) {
                Player_SetActionFunc(play, this, Player_FailToLiftActor, 0);
                this->actor.world.pos.x =
                    (Math_SinS(interactRangeActor->yawTowardsPlayer) * 20.0f) + interactRangeActor->world.pos.x;
                this->actor.world.pos.z =
                    (Math_CosS(interactRangeActor->yawTowardsPlayer) * 20.0f) + interactRangeActor->world.pos.z;
                this->currentYaw = this->actor.shape.rot.y = interactRangeActor->yawTowardsPlayer + 0x8000;
                anim = &gPlayerAnim_link_normal_nocarry_free;
            } else {
                Player_SetActionFunc(play, this, Player_LiftActor, 0);
                anim = GET_PLAYER_ANIM(PLAYER_ANIMGROUP_HOLDING_OBJECT, this->modelAnimType);
            }

            // Same actor is used for small and large silver rocks, use actor params to identify large ones
            bool isLargeSilverRock = interactActorId == ACTOR_EN_ISHI && interactRangeActor->params & 1 == 1;
            if (CVarGetInteger("gFasterHeavyBlockLift", 0) && (isLargeSilverRock || interactActorId == ACTOR_BG_HEAVY_BLOCK)) {
                LinkAnimation_PlayOnceSetSpeed(play, &this->skelAnime, anim, 5.0f);
            } else {
                LinkAnimation_PlayOnce(play, &this->skelAnime, anim);
            }
        }
    } else {
        Player_SetupStandingStillType(this, play);
        this->stateFlags1 &= ~PLAYER_STATE1_HOLDING_ACTOR;
    }
}

void Player_SetupTalkWithActor(PlayState* play, Player* this) {
    Player_SetActionFuncPreserveMoveFlags(play, this, Player_TalkWithActor, 0);

    this->stateFlags1 |= PLAYER_STATE1_TALKING | PLAYER_STATE1_IN_CUTSCENE;

    if (this->actor.textId != 0) {
        Message_StartTextbox(play, this->actor.textId, this->talkActor);
        this->targetActor = this->talkActor;
    }
}

void Player_SetupRideHorse(PlayState* play, Player* this) {
    Player_SetActionFuncPreserveMoveFlags(play, this, Player_RideHorse, 0);
}

void Player_SetupGrabPushPullWall(PlayState* play, Player* this) {
    Player_SetActionFunc(play, this, Player_GrabPushPullWall, 0);
}

void Player_SetupClimbingWallOrDownLedge(PlayState* play, Player* this) {
    s32 preservedTimer = this->genericTimer;
    s32 preservedVar = this->genericVar;

    Player_SetActionFuncPreserveMoveFlags(play, this, Player_ClimbingWallOrDownLedge, 0);
    this->actor.velocity.y = 0.0f;

    this->genericTimer = preservedTimer;
    this->genericVar = preservedVar;
}

void Player_SetupInsideCrawlspace(PlayState* play, Player* this) {
    Player_SetActionFuncPreserveMoveFlags(play, this, Player_InsideCrawlspace, 0);
}

void Player_SetupGetItem(PlayState* play, Player* this) {
    Player_SetActionFuncPreserveMoveFlags(play, this, Player_GetItem, 0);

    this->stateFlags1 |= PLAYER_STATE1_GETTING_ITEM | PLAYER_STATE1_IN_CUTSCENE;

    if (this->getItemId == GI_HEART_CONTAINER_2) {
        this->genericTimer = 20;
    } else if (this->getItemId >= 0 || (this->getItemEntry.objectId != OBJECT_INVALID && this->getItemEntry.getItemId >= 0)) {
        this->genericTimer = 1;
    } else {
        this->getItemId = -this->getItemId;
        this->getItemEntry.getItemId = -this->getItemEntry.getItemId;
    }
}

s32 Player_StartJump(Player* this, PlayState* play) {
    s16 yawDiff;
    LinkAnimationHeader* anim;
    f32 yVel;

    yawDiff = this->currentYaw - this->actor.shape.rot.y;

    if ((ABS(yawDiff) < 0x1000) && (this->linearVelocity > 4.0f)) {
        anim = &gPlayerAnim_link_normal_run_jump;
    } else {
        anim = &gPlayerAnim_link_normal_jump;
    }

    if (this->linearVelocity > (IREG(66) / 100.0f)) {
        yVel = IREG(67) / 100.0f;
    } else {
        yVel = (IREG(68) / 100.0f) + ((IREG(69) * this->linearVelocity) / 1000.0f);
    }

    Player_SetupJumpWithSfx(this, anim, yVel, play, NA_SE_VO_LI_AUTO_JUMP);
    this->genericTimer = 1;

    return 1;
}

void Player_SetupGrabLedge(PlayState* play, Player* this, CollisionPoly* arg2, f32 arg3, LinkAnimationHeader* arg4) {
    f32 sp24 = COLPOLY_GET_NORMAL(arg2->normal.x);
    f32 sp20 = COLPOLY_GET_NORMAL(arg2->normal.z);

    Player_SetActionFunc(play, this, Player_GrabLedge, 0);
    Player_ResetAttributesAndHeldActor(play, this);
    Player_PlayAnimOnce(play, this, arg4);

    this->actor.world.pos.x -= (arg3 + 1.0f) * sp24;
    this->actor.world.pos.z -= (arg3 + 1.0f) * sp20;
    this->actor.shape.rot.y = this->currentYaw = Math_Atan2S(sp20, sp24);

    Player_ClearAttentionModeAndStopMoving(this);
    Player_AnimUpdatePrevTranslRot(this);
}

s32 Player_SetupGrabLedgeInsteadOfFalling(Player* this, PlayState* play) {
    CollisionPoly* colPoly;
    s32 polyBgId;
    Vec3f pos;
    Vec3f colPolyPos;
    f32 dist;

    if ((this->actor.yDistToWater < -80.0f) && (ABS(this->angleToFloorX) < 2730) && (ABS(this->angleToFloorY) < 2730)) {
        pos.x = this->actor.prevPos.x - this->actor.world.pos.x;
        pos.z = this->actor.prevPos.z - this->actor.world.pos.z;

        dist = sqrtf(SQ(pos.x) + SQ(pos.z));
        if (dist != 0.0f) {
            dist = 5.0f / dist;
        } else {
            dist = 0.0f;
        }

        pos.x = this->actor.prevPos.x + (pos.x * dist);
        pos.y = this->actor.world.pos.y;
        pos.z = this->actor.prevPos.z + (pos.z * dist);

        if (BgCheck_EntityLineTest1(&play->colCtx, &this->actor.world.pos, &pos, &colPolyPos, &colPoly, true, false, false,
                                    true, &polyBgId) &&
            ((ABS(colPoly->normal.y) < 600) || (CVarGetInteger("gClimbEverything", 0) != 0))) {
            f32 nx = COLPOLY_GET_NORMAL(colPoly->normal.x);
            f32 ny = COLPOLY_GET_NORMAL(colPoly->normal.y);
            f32 nz = COLPOLY_GET_NORMAL(colPoly->normal.z);
            f32 distToPoly;
            s32 shouldClimbDownAdjacentWall;

            distToPoly = Math3D_UDistPlaneToPos(nx, ny, nz, colPoly->dist, &this->actor.world.pos);

            shouldClimbDownAdjacentWall = sFloorProperty == 6;
            if (!shouldClimbDownAdjacentWall && (func_80041DB8(&play->colCtx, colPoly, polyBgId) & 8)) {
                shouldClimbDownAdjacentWall = 1;
            }

            Player_SetupGrabLedge(play, this, colPoly, distToPoly,
                                  shouldClimbDownAdjacentWall ? &gPlayerAnim_link_normal_Fclimb_startB : &gPlayerAnim_link_normal_fall);

            if (shouldClimbDownAdjacentWall) {
                Player_SetupMiniCsFunc(play, this, Player_SetupClimbingWallOrDownLedge);

                this->currentYaw += 0x8000;
                this->actor.shape.rot.y = this->currentYaw;

                this->stateFlags1 |= PLAYER_STATE1_CLIMBING;
                Player_SetupAnimMovement(play, this, 0x9F);

                this->genericTimer = -1;
                this->genericVar = shouldClimbDownAdjacentWall;
            } else {
                this->stateFlags1 |= PLAYER_STATE1_HANGING_FROM_LEDGE_SLIP;
                this->stateFlags1 &= ~PLAYER_STATE1_Z_TARGETING_FRIENDLY;
            }

            func_8002F7DC(&this->actor, NA_SE_PL_SLIPDOWN);
            Player_PlayVoiceSfxForAge(this, NA_SE_VO_LI_HANG);
            return 1;
        }
    }

    return 0;
}

void Player_SetupClimbOntoLedge(Player* this, LinkAnimationHeader* anim, PlayState* play) {
    Player_SetActionFunc(play, this, Player_ClimbOntoLedge, 0);
    LinkAnimation_PlayOnceSetSpeed(play, &this->skelAnime, anim, 1.3f);
}

static Vec3f sWaterRaycastOffset = { 0.0f, 0.0f, 100.0f };

void Player_SetupMidairBehavior(Player* this, PlayState* play) {
    s32 yawDiff;
    CollisionPoly* floorPoly;
    s32 floorBgId;
    WaterBox* waterbox;
    Vec3f raycastPos;
    f32 floorPosY;
    f32 waterPosY;

    this->fallDistance = this->fallStartHeight - (s32)this->actor.world.pos.y;

    if (!(this->stateFlags1 & (PLAYER_STATE1_SWIMMING | PLAYER_STATE1_IN_CUTSCENE)) &&
        !(this->actor.bgCheckFlags & 1)) {
        if (!Player_ShouldEnterGrotto(play, this)) {
            if (sFloorProperty == 8) {
                this->actor.world.pos.x = this->actor.prevPos.x;
                this->actor.world.pos.z = this->actor.prevPos.z;
                return;
            }

            if (!(this->stateFlags3 & PLAYER_STATE3_MIDAIR) && !(this->skelAnime.moveFlags & 0x80) &&
                (Player_UpdateMidair != this->actionFunc) && (Player_FallingDive != this->actionFunc)) {

                if ((sFloorProperty == 7) || (this->swordState != 0)) {
                    Math_Vec3f_Copy(&this->actor.world.pos, &this->actor.prevPos);
                    Player_StopMovement(this);
                    return;
                }

                if (this->hoverBootsTimer != 0) {
                    this->actor.velocity.y = 1.0f;
                    sFloorProperty = 9;
                    return;
                }

                yawDiff = (s16)(this->currentYaw - this->actor.shape.rot.y);

                Player_SetActionFunc(play, this, Player_UpdateMidair, 1);
                Player_ResetAttributes(play, this);

                this->surfaceMaterial = this->prevSurfaceMaterial;

                if ((this->actor.bgCheckFlags & 4) && !(this->stateFlags1 & PLAYER_STATE1_SWIMMING) &&
                    (sFloorProperty != 6) && (sFloorProperty != 9) && (sPlayerYDistToFloor > 20.0f) &&
                    (this->swordState == 0) && (ABS(yawDiff) < 0x2000) && (this->linearVelocity > 3.0f)) {

                    if ((sFloorProperty == 11) && !(this->stateFlags1 & PLAYER_STATE1_HOLDING_ACTOR)) {

                        floorPosY = Player_RaycastFloorWithOffset(play, this, &sWaterRaycastOffset, &raycastPos, &floorPoly, &floorBgId);
                        waterPosY = this->actor.world.pos.y;

                        if (WaterBox_GetSurface1(play, &play->colCtx, raycastPos.x, raycastPos.z, &waterPosY, &waterbox) &&
                            ((waterPosY - floorPosY) > 50.0f)) {
                            Player_SetupJump(this, &gPlayerAnim_link_normal_run_jump_water_fall, 6.0f, play);
                            Player_SetActionFunc(play, this, Player_FallingDive, 0);
                            return;
                        }
                    }

                    Player_StartJump(this, play);
                    return;
                }

                if (CVarGetInteger("gDisableLedges", 0)) {
                    Player_PlayAnimLoop(play, this, &gPlayerAnim_link_normal_landing_wait);
                    return;
                }
                
                if ((sFloorProperty == 9) || (sPlayerYDistToFloor <= this->ageProperties->unk_34) ||
                    !Player_SetupGrabLedgeInsteadOfFalling(this, play)) {
                    Player_PlayAnimLoop(play, this, &gPlayerAnim_link_normal_landing_wait);
                    return;
                }
            }
        }
    } else {
        this->fallStartHeight = this->actor.world.pos.y;
    }
}

s32 Player_SetupCameraMode(PlayState* play, Player* this) {
    s32 cameraMode;

    if (!(CVarGetInteger("gDisableFPSView", 0))) {
        if (this->attentionMode == PLAYER_ATTENTIONMODE_AIMING) {
            if (Actor_PlayerIsAimingFpsItem(this)) {
                bool shouldUseBowCamera = LINK_IS_ADULT;

                if (CVarGetInteger("gBowSlingShotAmmoFix", 0)) {
                    shouldUseBowCamera = this->heldItemAction != PLAYER_IA_SLINGSHOT;
                }

                cameraMode = shouldUseBowCamera ? CAM_MODE_BOWARROW : CAM_MODE_SLINGSHOT;
            } else {
                cameraMode = CAM_MODE_BOOMERANG;
            }
        } else {
            cameraMode = CAM_MODE_FIRSTPERSON;
        }

        return Camera_ChangeMode(Play_GetCamera(play, 0), cameraMode);
    }
}

s32 Player_SetupCutscene(PlayState* play, Player* this) {
    if (this->attentionMode == PLAYER_ATTENTIONMODE_CUTSCENE) {
        Player_SetActionFunc(play, this, Player_StartCutscene, 0);
        if (this->doorBgCamIndex != 0) {
            this->stateFlags1 |= PLAYER_STATE1_IN_CUTSCENE;
        }
        Player_InactivateMeleeWeapon(this);
        return 1;
    } else {
        return 0;
    }
}

void Player_LoadGetItemObject(Player* this, s16 objectId) {
    s32 pad;
    size_t size;

    if (objectId != OBJECT_INVALID) {
        this->giObjectLoading = true;
        osCreateMesgQueue(&this->giObjectLoadQueue, &this->giObjectLoadMsg, 1);

        size = gObjectTable[objectId].vromEnd - gObjectTable[objectId].vromStart;

        LOG_HEX("size", size);
        ASSERT(size <= 1024 * 8);

        DmaMgr_SendRequest2(&this->giObjectDmaRequest, (uintptr_t)this->giObjectSegment,
                            gObjectTable[objectId].vromStart, size, 0, &this->giObjectLoadQueue, OS_MESG_PTR(NULL),
                            __FILE__, __LINE__);
    }
}

void Player_SetupMagicSpell(PlayState* play, Player* this, s32 magicSpell) {
    Player_SetActionFuncPreserveItemAP(play, this, Player_UpdateMagicSpell, 0);

    this->genericVar = magicSpell - 3;
    func_80087708(play, sMagicSpellCosts[magicSpell], 4);

    u8 isFastFarores = CVarGetInteger("gFastFarores", 0) && this->itemAction == PLAYER_IA_FARORES_WIND;

    if (isFastFarores) {
        LinkAnimation_PlayOnceSetSpeed(play, &this->skelAnime, &gPlayerAnim_link_magic_tame, 0.83f * 2);
        return;
    } else {
        LinkAnimation_PlayOnceSetSpeed(play, &this->skelAnime, &gPlayerAnim_link_magic_tame, 0.83f);
    }

    if (magicSpell == 5) {
        this->subCamId = OnePointCutscene_Init(play, 1100, -101, NULL, MAIN_CAM);
    } else {
        Player_SetCameraTurnAround(play, 10);
    }
}

void Player_ResetLookAngles(Player* this) {
    this->actor.focus.rot.x = this->actor.focus.rot.z = this->headRot.x = this->headRot.y = this->headRot.z =
        this->upperBodyRot.x = this->upperBodyRot.y = this->upperBodyRot.z = 0;

    this->actor.focus.rot.y = this->actor.shape.rot.y;
}

static u8 sExchangeGetItemIDs[] = {
    GI_LETTER_ZELDA, GI_WEIRD_EGG,    GI_CHICKEN,     GI_BEAN,        GI_POCKET_EGG,   GI_POCKET_CUCCO,
    GI_COJIRO,       GI_ODD_MUSHROOM, GI_ODD_POTION,  GI_SAW,         GI_SWORD_BROKEN, GI_PRESCRIPTION,
    GI_FROG,         GI_EYEDROPS,     GI_CLAIM_CHECK, GI_MASK_SKULL,  GI_MASK_SPOOKY,  GI_MASK_KEATON,
    GI_MASK_BUNNY,   GI_MASK_TRUTH,   GI_MASK_GORON,  GI_MASK_ZORA,   GI_MASK_GERUDO,  GI_LETTER_RUTO,
    GI_LETTER_RUTO,  GI_LETTER_RUTO,  GI_LETTER_RUTO, GI_LETTER_RUTO, GI_LETTER_RUTO,
};

static LinkAnimationHeader* sExchangeItemAnims[] = {
    &gPlayerAnim_link_normal_give_other,
    &gPlayerAnim_link_bottle_read,
    &gPlayerAnim_link_normal_take_out,
};

s32 Player_SetupItemCutsceneOrFirstPerson(Player* this, PlayState* play) {
    s32 item;
    s32 sp28;
    GetItemEntry giEntry;
    Actor* targetActor;

    if ((this->attentionMode != 0) && (Player_IsSwimming(this) || (this->actor.bgCheckFlags & 1) ||
                                       (this->stateFlags1 & PLAYER_STATE1_RIDING_HORSE))) {

        if (!Player_SetupCutscene(play, this)) {
            if (this->attentionMode == 4) {
                item = Player_ActionToMagicSpell(this, this->itemAction);
                if (item >= 0) {
                    if ((item != 3) || (gSaveContext.respawn[RESPAWN_MODE_TOP].data <= 0)) {
                        Player_SetupMagicSpell(play, this, item);
                    } else {
                        Player_SetActionFunc(play, this, Player_ChooseFaroresWindOption, 1);
                        this->stateFlags1 |= PLAYER_STATE1_SKIP_OTHER_ACTORS_UPDATE | PLAYER_STATE1_IN_CUTSCENE;
                        Player_PlayAnimOnce(play, this, Player_GetStandingStillAnim(this));
                        Player_SetCameraTurnAround(play, 4);
                    }

                    Player_ClearAttentionModeAndStopMoving(this);
                    return 1;
                }

                item = this->itemAction - PLAYER_IA_LETTER_ZELDA;
                if ((item >= 0) ||
                    (sp28 = Player_ActionToBottle(this, this->itemAction) - 1,
                     ((sp28 >= 0) && (sp28 < 6) &&
                      ((this->itemAction > PLAYER_IA_BOTTLE_POE) ||
                       ((this->talkActor != NULL) &&
                        (((this->itemAction == PLAYER_IA_BOTTLE_POE) && (this->exchangeItemId == EXCH_ITEM_POE)) ||
                         (this->exchangeItemId == EXCH_ITEM_BLUE_FIRE))))))) {

                    if ((play->actorCtx.titleCtx.delayTimer == 0) && (play->actorCtx.titleCtx.alpha == 0)) {
                        Player_SetActionFuncPreserveItemAP(play, this, Player_PresentExchangeItem, 0);

                        if (item >= 0) {
                            if (this->getItemEntry.objectId == OBJECT_INVALID) {
                                giEntry = ItemTable_Retrieve(sExchangeGetItemIDs[item]);
                            } else {
                                giEntry = this->getItemEntry;
                            }
                            Player_LoadGetItemObject(this, giEntry.objectId);
                        }

                        this->stateFlags1 |=
                            PLAYER_STATE1_TALKING | PLAYER_STATE1_SKIP_OTHER_ACTORS_UPDATE | PLAYER_STATE1_IN_CUTSCENE;

                        if (item >= 0) {
                            item = item + 1;
                        } else {
                            item = sp28 + 0x18;
                        }

                        targetActor = this->talkActor;

                        if ((targetActor != NULL) &&
                            ((this->exchangeItemId == item) || (this->exchangeItemId == EXCH_ITEM_BLUE_FIRE) ||
                             ((this->exchangeItemId == EXCH_ITEM_POE) &&
                              (this->itemAction == PLAYER_IA_BOTTLE_BIG_POE)) ||
                             ((this->exchangeItemId == EXCH_ITEM_BEAN) &&
                              (this->itemAction == PLAYER_IA_BOTTLE_BUG))) &&
                            ((this->exchangeItemId != EXCH_ITEM_BEAN) || (this->itemAction == PLAYER_IA_BEAN))) {
                            if (this->exchangeItemId == EXCH_ITEM_BEAN) {
                                Inventory_ChangeAmmo(ITEM_BEAN, -1);
                                Player_SetActionFuncPreserveItemAP(play, this, func_8084279C, 0);
                                this->stateFlags1 |= PLAYER_STATE1_IN_CUTSCENE;
                                this->genericTimer = 0x50;
                                this->genericVar = -1;
                            }
                            targetActor->flags |= ACTOR_FLAG_PLAYER_TALKED_TO;
                            this->targetActor = this->talkActor;
                        } else if (item == EXCH_ITEM_LETTER_RUTO) {
                            this->genericVar = 1;
                            this->actor.textId = 0x4005;
                            Player_SetCameraTurnAround(play, 1);
                        } else {
                            this->genericVar = 2;
                            this->actor.textId = 0xCF;
                            Player_SetCameraTurnAround(play, 4);
                        }

                        this->actor.flags |= ACTOR_FLAG_PLAYER_TALKED_TO;
                        this->exchangeItemId = item;

                        if (this->genericVar < 0) {
                            Player_ChangeAnimMorphToLastFrame(
                                play, this,
                                GET_PLAYER_ANIM(PLAYER_ANIMGROUP_START_CHECKING_OR_SPEAKING, this->modelAnimType));
                        } else {
                            Player_PlayAnimOnce(play, this, sExchangeItemAnims[this->genericVar]);
                        }

                        Player_ClearAttentionModeAndStopMoving(this);
                    }
                    return 1;
                }

                item = Player_ActionToBottle(this, this->itemAction);
                if (item >= 0) {
                    if (item == 0xC) {
                        Player_SetActionFuncPreserveItemAP(play, this, Player_HealWithFairy, 0);
                        Player_PlayAnimOnceSlowed(play, this, &gPlayerAnim_link_bottle_bug_out);
                        Player_SetCameraTurnAround(play, 3);
                    } else if ((item > 0) && (item < 4)) {
                        Player_SetActionFuncPreserveItemAP(play, this, Player_DropItemFromBottle, 0);
                        Player_PlayAnimOnceSlowed(play, this, &gPlayerAnim_link_bottle_fish_out);
                        Player_SetCameraTurnAround(play, (item == 1) ? 1 : 5);
                    } else {
                        Player_SetActionFuncPreserveItemAP(play, this, Player_DrinkFromBottle, 0);
                        Player_ChangeAnimSlowedMorphToLastFrame(play, this, &gPlayerAnim_link_bottle_drink_demo_start);
                        Player_SetCameraTurnAround(play, 2);
                    }
                } else {
                    Player_SetActionFuncPreserveItemAP(play, this, Player_PlayOcarina, 0);
                    Player_PlayAnimOnceSlowed(play, this, &gPlayerAnim_link_normal_okarina_start);
                    this->stateFlags2 |= PLAYER_STATE2_PLAYING_OCARINA_GENERAL;
                    Player_SetCameraTurnAround(play, (this->ocarinaActor != NULL) ? 0x5B : 0x5A);
                    if (this->ocarinaActor != NULL) {
                        this->stateFlags2 |= PLAYER_STATE2_PLAYING_OCARINA_FOR_ACTOR;
                        Camera_SetParam(Play_GetCamera(play, 0), 8, this->ocarinaActor);
                    }
                }
            } else if (Player_SetupCameraMode(play, this)) {
                if (!(this->stateFlags1 & PLAYER_STATE1_RIDING_HORSE)) {
                    Player_SetActionFunc(play, this, Player_FirstPersonAiming, 1);
                    this->genericTimer = 13;
                    Player_ResetLookAngles(this);
                }
                this->stateFlags1 |= PLAYER_STATE1_IN_FIRST_PERSON_MODE;
                func_80078884(NA_SE_SY_CAMERA_ZOOM_UP);
                Player_StopMovement(this);
                return 1;
            } else {
                this->attentionMode = 0;
                func_80078884(NA_SE_SY_ERROR);
                return 0;
            }

            this->stateFlags1 |= PLAYER_STATE1_SKIP_OTHER_ACTORS_UPDATE | PLAYER_STATE1_IN_CUTSCENE;
        }

        Player_ClearAttentionModeAndStopMoving(this);
        return 1;
    }

    return 0;
}

s32 Player_SetupSpeakOrCheck(Player* this, PlayState* play) {
    Actor* talkActor = this->talkActor;
    Actor* targetActor = this->targetActor;
    Actor* naviActor = NULL;
    s32 naviHasText = 0;
    s32 targetActorHasText;

    targetActorHasText = (targetActor != NULL) && (CHECK_FLAG_ALL(targetActor->flags, ACTOR_FLAG_TARGETABLE | ACTOR_FLAG_NAVI_HAS_INFO) || (targetActor->naviEnemyId != 0xFF));

    if (targetActorHasText || (this->naviTextId != 0)) {
        naviHasText = (this->naviTextId < 0) && ((ABS(this->naviTextId) & 0xFF00) != 0x200);
        if (naviHasText || !targetActorHasText) {
            naviActor = this->naviActor;
            if (naviHasText) {
                targetActor = NULL;
                talkActor = NULL;
            }
        } else {
            naviActor = targetActor;
        }
    }

    if ((talkActor != NULL) || (naviActor != NULL)) {
        if ((targetActor == NULL) || (targetActor == talkActor) || (targetActor == naviActor)) {
            if (!(this->stateFlags1 & PLAYER_STATE1_HOLDING_ACTOR) ||
                ((this->heldActor != NULL) && (naviHasText || (talkActor == this->heldActor) || (naviActor == this->heldActor) ||
                                               ((talkActor != NULL) && (talkActor->flags & ACTOR_FLAG_WILL_TALK))))) {
                if ((this->actor.bgCheckFlags & 1) || (this->stateFlags1 & PLAYER_STATE1_RIDING_HORSE) ||
                    (Player_IsSwimming(this) && !(this->stateFlags2 & PLAYER_STATE2_DIVING))) {

                    if (talkActor != NULL) {
                        this->stateFlags2 |= PLAYER_STATE2_CAN_SPEAK_OR_CHECK;
                        if (CHECK_BTN_ALL(sControlInput->press.button, BTN_A) || (talkActor->flags & ACTOR_FLAG_WILL_TALK)) {
                            naviActor = NULL;
                        } else if (naviActor == NULL) {
                            return 0;
                        }
                    }

                    if (naviActor != NULL) {
                        if (!naviHasText) {
                            this->stateFlags2 |= PLAYER_STATE2_NAVI_REQUESTING_TALK;
                        }

                        if (!CHECK_BTN_ALL(sControlInput->press.button, CVarGetInteger("gNaviOnL", 0) ? BTN_L : BTN_CUP) &&
                            !naviHasText) {
                            return 0;
                        }

                        talkActor = naviActor;
                        this->talkActor = NULL;

                        if (naviHasText || !targetActorHasText) {
                            if (this->naviTextId >= 0) {
                                naviActor->textId = this->naviTextId;
                            } else {
                                naviActor->textId = -this->naviTextId;
                            }
                        } else {
                            if (naviActor->naviEnemyId != 0xFF) {
                                naviActor->textId = naviActor->naviEnemyId + 0x600;
                            }
                        }
                    }

                    this->currentMask = sCurrentMask;
                    Player_StartTalkingWithActor(play, talkActor);
                    return 1;
                }
            }
        }
    }

    return 0;
}

s32 Player_ForceFirstPerson(Player* this, PlayState* play) {
    if (!(this->stateFlags1 & (PLAYER_STATE1_HOLDING_ACTOR | PLAYER_STATE1_RIDING_HORSE)) &&
        Camera_CheckValidMode(Play_GetCamera(play, 0), 6)) {
        if ((this->actor.bgCheckFlags & 1) ||
            (Player_IsSwimming(this) && (this->actor.yDistToWater < this->ageProperties->unk_2C))) {
            this->attentionMode = 1;
            return 1;
        }
    }

    return 0;
}

s32 Player_SetupCUpBehavior(Player* this, PlayState* play) {
    if (this->attentionMode != 0) {
        Player_SetupItemCutsceneOrFirstPerson(this, play);
        return 1;
    }

    if ((this->targetActor != NULL) &&
        (CHECK_FLAG_ALL(this->targetActor->flags, ACTOR_FLAG_TARGETABLE | ACTOR_FLAG_NAVI_HAS_INFO) || (this->targetActor->naviEnemyId != 0xFF))) {
        this->stateFlags2 |= PLAYER_STATE2_NAVI_REQUESTING_TALK;
    } else if ((this->naviTextId == 0 || CVarGetInteger("gNaviOnL", 0)) && !Player_IsUnfriendlyZTargeting(this) &&
               CHECK_BTN_ALL(sControlInput->press.button, BTN_CUP) && (YREG(15) != 0x10) && (YREG(15) != 0x20) &&
               !Player_ForceFirstPerson(this, play)) {
        func_80078884(NA_SE_SY_ERROR);
    }

    return 0;
}

void Player_SetupJumpSlash(PlayState* play, Player* this, s32 arg2, f32 xzVelocity, f32 yVelocity) {
    if (!CVarGetInteger("gDisableMeleeAttacks", 0)) {
        Player_StartMeleeWeaponAttack(play, this, arg2);
        Player_SetActionFunc(play, this, Player_JumpSlash, 0);

        this->stateFlags3 |= PLAYER_STATE3_MIDAIR;

        this->currentYaw = this->actor.shape.rot.y;
        this->linearVelocity = xzVelocity;
        this->actor.velocity.y = yVelocity;

        this->actor.bgCheckFlags &= ~1;
        this->hoverBootsTimer = 0;

        Player_PlayJumpSfx(this);
        Player_PlayVoiceSfxForAge(this, NA_SE_VO_LI_SWORD_L);
    }
    else {
        Player_SetupReturnToStandStill(this, play);
    }
}

s32 Player_CanJumpSlash(Player* this) {
    if (!(this->stateFlags1 & PLAYER_STATE1_SHIELDING) && (Player_GetSwordHeld(this) != 0)) {
        if (sUsingItemAlreadyInHand ||
            ((this->actor.category != ACTORCAT_PLAYER) && CHECK_BTN_ALL(sControlInput->press.button, BTN_B))) {
            return 1;
        }
    }

    return 0;
}

s32 Player_SetupMidairJumpSlash(Player* this, PlayState* play) {
    if (Player_CanJumpSlash(this) && (sFloorSpecialProperty != 7)) {
        Player_SetupJumpSlash(play, this, 17, 3.0f, 4.5f);
        return 1;
    }

    return 0;
}

void Player_SetupRolling(Player* this, PlayState* play) {
    // Chaos
    if (CVarGetInteger("gExplodingRolls", 0) || (CVarGetInteger("gChaosRedeem", 0) && Rand_ZeroOne() < 0.05f)) {
        Player_SpawnExplosion(play, this);
    }
    if (CVarGetInteger("gFreezingRolls", 0) || (CVarGetInteger("gChaosRedeem", 0) && Rand_ZeroOne() < 0.05f)) {
        this->actor.colChkInfo.damage = 0;
        Player_SetupDamage(play, this, PLAYER_DMGREACTION_FROZEN, 0.0f, 0.0f, 0, 20);
    }
    else {
        Player_SetActionFunc(play, this, Player_Rolling, 0);

        LinkAnimation_PlayOnceSetSpeed(play, &this->skelAnime,
                                       GET_PLAYER_ANIM(PLAYER_ANIMGROUP_ROLLING, this->modelAnimType),
                                       1.25f * sWaterSpeedScale);
        gSaveContext.sohStats.count[COUNT_ROLLS]++;
    }
}

void Player_SetupBackflipOrSidehop(Player* this, PlayState* play, s32 relativeStickInput) {
    Player_SetupJumpWithSfx(this, sManualJumpAnims[relativeStickInput][0], !(relativeStickInput & 1) ? 5.8f : 3.5f, play, NA_SE_VO_LI_SWORD_N);

    if (relativeStickInput) {}

    this->genericTimer = 1;
    this->genericVar = relativeStickInput;

    this->currentYaw = this->actor.shape.rot.y + (relativeStickInput << 0xE);
    this->linearVelocity = !(relativeStickInput & 1) ? 6.0f : 8.5f;

    this->stateFlags2 |= PLAYER_STATE2_BACKFLIPPING_OR_SIDEHOPPING;

    func_8002F7DC(&this->actor, ((relativeStickInput << 0xE) == 0x8000) ? NA_SE_PL_ROLL : NA_SE_PL_SKIP);
}

s32 Player_CanRoll(Player* this, PlayState* play) {
    if ((this->relativeAnalogStickInputs[this->inputFrameCounter] == 0) && (sFloorSpecialProperty != 7)) {
        if (CVarGetInteger("gForwardJump", 0)) {
            Player_SetupBackflipOrSidehop(this, play, this->relativeAnalogStickInputs[this->inputFrameCounter]);
        }
        else {
            Player_SetupRolling(this, play);
        }
        return 1;
    }

    return 0;
}

s32 Player_SetupJumpSlashOrRoll(Player* this, PlayState* play) {
    s32 relativeStickInput;

    if (CHECK_BTN_ALL(sControlInput->press.button, BTN_A) && (play->roomCtx.curRoom.behaviorType1 != 2) &&
        (sFloorSpecialProperty != 7) &&
        (SurfaceType_GetSlope(&play->colCtx, this->actor.floorPoly, this->actor.floorBgId) != 1)) {
        relativeStickInput = this->relativeAnalogStickInputs[this->inputFrameCounter];

        if (relativeStickInput <= 0) {
            if (Player_IsZTargeting(this)) {
                if (this->actor.category != ACTORCAT_PLAYER || CVarGetInteger("gForwardJump", 0)) {
                    if (relativeStickInput < 0) {
                        Player_SetupJump(this, &gPlayerAnim_link_normal_jump, REG(69) / 100.0f, play);
                    } else {
                        if (CVarGetInteger("gForwardJump", 0)) {
                            Player_SetupBackflipOrSidehop(this, play, 0);
                        }
                        else {
                            Player_SetupRolling(this, play);
                        }
                    }
                } else {
                    if (Player_GetSwordHeld(this) && Player_CanUseItem(this)) {
                        Player_SetupJumpSlash(play, this, 17, 5.0f, 5.0f);
                    } else {
                        Player_SetupRolling(this, play);
                    }
                }
                return 1;
            }
        } else {
            Player_SetupBackflipOrSidehop(this, play, relativeStickInput);
            if (relativeStickInput == 1 || relativeStickInput == 3) {
                gSaveContext.sohStats.count[COUNT_SIDEHOPS]++;
            }
            if (relativeStickInput == 2) {
                gSaveContext.sohStats.count[COUNT_BACKFLIPS]++;
            }
            
            return 1;
        }
    }

    return 0;
}

void Player_EndRun(Player* this, PlayState* play) {
    LinkAnimationHeader* anim;
    f32 frame;

    frame = this->walkFrame - 3.0f;
    if (frame < 0.0f) {
        frame += 29.0f;
    }

    if (frame < 14.0f) {
        anim = GET_PLAYER_ANIM(PLAYER_ANIMGROUP_END_WALK_ON_LEFT_FOOT, this->modelAnimType);
        frame = 11.0f - frame;
        if (frame < 0.0f) {
            frame = 1.375f * -frame;
        }
        frame /= 11.0f;
    } else {
        anim = GET_PLAYER_ANIM(PLAYER_ANIMGROUP_END_WALK_ON_RIGHT_FOOT, this->modelAnimType);
        frame = 26.0f - frame;
        if (frame < 0.0f) {
            frame = 2 * -frame;
        }
        frame /= 12.0f;
    }

    LinkAnimation_Change(play, &this->skelAnime, anim, 1.0f, 0.0f, Animation_GetLastFrame(anim), ANIMMODE_ONCE,
                         4.0f * frame);
    this->currentYaw = this->actor.shape.rot.y;
}

void Player_SetupEndRun(Player* this, PlayState* play) {
    Player_ReturnToStandStill(this, play);
    Player_EndRun(this, play);
}

void Player_SetupStandingStillNoMorph(Player* this, PlayState* play) {
    Player_SetActionFunc(play, this, Player_StandingStill, 1);
    Player_PlayAnimOnce(play, this, Player_GetStandingStillAnim(this));
    this->currentYaw = this->actor.shape.rot.y;
}

void Player_ClearLookAndAttention(Player* this, PlayState* play) {
    if (!(this->stateFlags3 & PLAYER_STATE3_MOVING_ALONG_HOOKSHOT_PATH)) {
        Player_ResetLookAngles(this);
        if (this->stateFlags1 & PLAYER_STATE1_SWIMMING) {
            Player_SetupSwimIdle(play, this);
        } else {
            Player_SetupStandingStillType(this, play);
        }
        if (this->attentionMode < PLAYER_ATTENTIONMODE_ITEM_CUTSCENE) {
            this->attentionMode = PLAYER_ATTENTIONMODE_NONE;
        }
    }

    this->stateFlags1 &= ~(PLAYER_STATE1_HANGING_FROM_LEDGE_SLIP | PLAYER_STATE1_CLIMBING_ONTO_LEDGE |
                           PLAYER_STATE1_IN_FIRST_PERSON_MODE);
}

s32 Player_SetupRollOrPutAway(Player* this, PlayState* play) {
    if (CVarGetInteger("gSonicRoll", 0) || (CVarGetInteger("gChaosRedeem", 0) && Rand_ZeroOne() < 0.02f)) {
        Player_SetupRolling(this, play);
    } else if (CVarGetInteger("gPogoStick", 0) || (CVarGetInteger("gChaosRedeem", 0) && Rand_ZeroOne() < 0.02f)) {
        // Hopping
        Player_SetupJump(this, &gPlayerAnim_link_normal_jump, REG(69) / 100.0f, play);
        Player_SetupBackflipOrSidehop(this, play, 0);
    } else if (!Player_SetupStartUnfriendlyZTargeting(this) && (D_808535E0 == 0) &&
               !(this->stateFlags1 & PLAYER_STATE1_RIDING_HORSE) && CHECK_BTN_ALL(sControlInput->press.button, BTN_A)) {
        if (Player_CanRoll(this, play)) {
            return 1;
        }
        if ((this->putAwayTimer == 0) && (this->heldItemAction >= PLAYER_IA_SWORD_MASTER)) {
            Player_UseItem(play, this, ITEM_NONE);
        } else {
            this->stateFlags2 ^= PLAYER_STATE2_NAVI_IS_ACTIVE;
        }
    }

    return 0;
}

s32 Player_SetupDefend(Player* this, PlayState* play) {
    LinkAnimationHeader* anim;
    f32 frame;

    if ((play->shootingGalleryStatus == 0) && (this->currentShield != PLAYER_SHIELD_NONE) &&
        CHECK_BTN_ALL(sControlInput->cur.button, BTN_R) &&
        (Player_IsChildWithHylianShield(this) || (!Player_IsFriendlyZTargeting(this) && (this->targetActor == NULL)))) {

        Player_InactivateMeleeWeapon(this);
        Player_DetatchHeldActor(play, this);

        if (Player_SetActionFunc(play, this, Player_AimShieldCrouched, 0)) {
            this->stateFlags1 |= PLAYER_STATE1_SHIELDING;

            if (!Player_IsChildWithHylianShield(this)) {
                Player_SetModelsForHoldingShield(this);
                anim = GET_PLAYER_ANIM(PLAYER_ANIMGROUP_START_DEFENDING, this->modelAnimType);
            } else {
                anim = &gPlayerAnim_clink_normal_defense_ALL;
            }

            if (anim != this->skelAnime.animation) {
                if (Player_IsUnfriendlyZTargeting(this)) {
                    this->unk_86C = 1.0f;
                } else {
                    this->unk_86C = 0.0f;
                    Player_ResetLeftRightBlendWeight(this);
                }
                this->upperBodyRot.x = this->upperBodyRot.y = this->upperBodyRot.z = 0;
            }

            frame = Animation_GetLastFrame(anim);
            LinkAnimation_Change(play, &this->skelAnime, anim, 1.0f, frame, frame, ANIMMODE_ONCE, 0.0f);

            if (Player_IsChildWithHylianShield(this)) {
                Player_SetupAnimMovement(play, this, 4);
            }

            func_8002F7DC(&this->actor, NA_SE_IT_SHIELD_POSTURE);
        }

        return 1;
    }

    return 0;
}

s32 Player_SetupTurnAroundRunning(Player* this, f32* targetVelocity, s16* targetYaw) {
    s16 yawDiff = this->currentYaw - *targetYaw;

    if (ABS(yawDiff) > DEGF_TO_BINANG(135.0f)) {
        if (Player_StepLinearVelocityToZero(this)) {
            *targetVelocity = 0.0f;
            *targetYaw = this->currentYaw;
        } else {
            return 1;
        }
    }

    return 0;
}

void Player_SetupDeactivateComboTimer(Player* this) {
    if ((this->comboTimer > 0) && !CHECK_BTN_ALL(sControlInput->cur.button, BTN_B)) {
        this->comboTimer = -this->comboTimer;
    }
}

s32 Player_SetupStartChargeSpinAttack(Player* this, PlayState* play) {
    if (CHECK_BTN_ALL(sControlInput->cur.button, BTN_B)) {
        if (!(this->stateFlags1 & PLAYER_STATE1_SHIELDING) && (Player_GetSwordHeld(this) != 0) &&
            (this->comboTimer == 1) && (this->heldItemAction != PLAYER_IA_STICK)) {
            if ((this->heldItemAction != PLAYER_IA_SWORD_BGS) || (gSaveContext.swordHealth > 0.0f)) {
                Player_StartChargeSpinAttack(play, this);
                return 1;
            }
        }
    } else {
        Player_SetupDeactivateComboTimer(this);
    }

    return 0;
}

s32 Player_SetupThrowDekuNut(PlayState* play, Player* this) {
    if ((play->roomCtx.curRoom.behaviorType1 != ROOM_BEHAVIOR_TYPE1_2) && (this->actor.bgCheckFlags & 1) &&
        (AMMO(ITEM_NUT) != 0)) {
        Player_SetActionFunc(play, this, Player_ThrowDekuNut, 0);
        Player_PlayAnimOnce(play, this, &gPlayerAnim_link_normal_light_bom);
        this->attentionMode = 0;
        return 1;
    }

    return 0;
}

static BottleSwingAnimInfo sBottleSwingAnims[] = {
    { &gPlayerAnim_link_bottle_bug_miss, &gPlayerAnim_link_bottle_bug_in, 2, 3 },
    { &gPlayerAnim_link_bottle_fish_miss, &gPlayerAnim_link_bottle_fish_in, 5, 3 },
};

s32 Player_CanSwingBottleOrCastFishingRod(PlayState* play, Player* this) {
    Vec3f checkPos;

    if (sUsingItemAlreadyInHand) {
        if (Player_GetBottleHeld(this) >= 0) {
            Player_SetActionFunc(play, this, Player_SwingBottle, 0);

            if (this->actor.yDistToWater > 12.0f) {
                this->genericTimer = 1;
            }

            Player_PlayAnimOnceSlowed(play, this, sBottleSwingAnims[this->genericTimer].bottleSwingAnim);

            func_8002F7DC(&this->actor, NA_SE_IT_SWORD_SWING);
            Player_PlayVoiceSfxForAge(this, NA_SE_VO_LI_AUTO_JUMP);
            return 1;
        }

        if (this->heldItemAction == PLAYER_IA_FISHING_POLE) {
            checkPos = this->actor.world.pos;
            checkPos.y += 50.0f;

            if (CVarGetInteger("gHoverFishing", 0)
                    ? 0
                    : !(this->actor.bgCheckFlags & 1) || (this->actor.world.pos.z > 1300.0f) ||
                          BgCheck_SphVsFirstPoly(&play->colCtx, &checkPos, 20.0f)) {
                func_80078884(NA_SE_SY_ERROR);
                return 0;
            }

            Player_SetActionFunc(play, this, Player_CastFishingRod, 0);
            this->fpsItemType = 1;
            Player_StopMovement(this);
            Player_PlayAnimOnce(play, this, &gPlayerAnim_link_fishing_throw);
            return 1;
        } else {
            return 0;
        }
    }

    return 0;
}

void Player_SetupRun(Player* this, PlayState* play) {
    PlayerActionFunc func;

    if (Player_IsZTargeting(this)) {
        func = Player_ZTargetingRun;
    } else {
        func = Player_Run;
    }

    Player_SetActionFunc(play, this, func, 1);
    Player_ChangeAnimShortMorphLoop(play, this, GET_PLAYER_ANIM(PLAYER_ANIMGROUP_RUNNING, this->modelAnimType));

    this->walkAngleToFloorX = 0;
    this->unk_864 = this->walkFrame = 0.0f;
}

void Player_SetupZTargetRunning(Player* this, PlayState* play, s16 arg2) {
    this->actor.shape.rot.y = this->currentYaw = arg2;
    Player_SetupRun(this, play);
}

s32 Player_SetupDefaultSpawnBehavior(PlayState* play, Player* this, f32 arg2) {
    WaterBox* waterbox;
    f32 posY;

    posY = this->actor.world.pos.y;
    if (WaterBox_GetSurface1(play, &play->colCtx, this->actor.world.pos.x, this->actor.world.pos.z, &posY,
                             &waterbox) != 0 && !CVarGetInteger("gNoWater", 0)) {
        posY -= this->actor.world.pos.y;
        if (this->ageProperties->unk_24 <= posY) {
            Player_SetActionFunc(play, this, Player_SpawnSwimming, 0);
            Player_ChangeAnimLongMorphLoop(play, this, &gPlayerAnim_link_swimer_swim);
            this->stateFlags1 |= PLAYER_STATE1_SWIMMING | PLAYER_STATE1_IN_CUTSCENE;
            this->genericTimer = 20;
            this->linearVelocity = 2.0f;
            Player_SetBootData(play, this);
            return 0;
        }
    }

    Player_SetupMiniCsMovement(play, this, arg2, this->actor.shape.rot.y);
    this->stateFlags1 |= PLAYER_STATE1_IN_CUTSCENE;
    return 1;
}

void Player_SpawnNoMomentum(PlayState* play, Player* this) {
    if (Player_SetupDefaultSpawnBehavior(play, this, 180.0f)) {
        this->genericTimer = -20;
    }
}

void Player_SpawnWalkingSlow(PlayState* play, Player* this) {
    this->linearVelocity = 2.0f;
    gSaveContext.entranceSpeed = 2.0f;
    if (Player_SetupDefaultSpawnBehavior(play, this, 120.0f)) {
        this->genericTimer = -15;
    }
}

void Player_SpawnWalkingPreserveMomentum(PlayState* play, Player* this) {
    if (gSaveContext.entranceSpeed < 0.1f) {
        gSaveContext.entranceSpeed = 0.1f;
    }

    this->linearVelocity = gSaveContext.entranceSpeed;

    if (Player_SetupDefaultSpawnBehavior(play, this, 800.0f)) {
        this->genericTimer = -80 / this->linearVelocity;
        if (this->genericTimer < -20) {
            this->genericTimer = -20;
        }
    }
}

void Player_SetupFriendlyBackwalk(Player* this, s16 yaw, PlayState* play) {
    Player_SetActionFunc(play, this, Player_FriendlyBackwalk, 1);
    LinkAnimation_CopyJointToMorph(play, &this->skelAnime);
    this->unk_864 = this->walkFrame = 0.0f;
    this->currentYaw = yaw;
}

void Player_SetupFriendlySidewalk(Player* this, PlayState* play) {
    Player_SetActionFunc(play, this, Player_EndSidewalk, 1);
    Player_ChangeAnimShortMorphLoop(play, this, GET_PLAYER_ANIM(PLAYER_ANIMGROUP_WALKING, this->modelAnimType));
}

void Player_SetupUnfriendlyBackwalk(Player* this, s16 yaw, PlayState* play) {
    Player_SetActionFunc(play, this, Player_UnfriendlyBackwalk, 1);
    LinkAnimation_Change(play, &this->skelAnime, &gPlayerAnim_link_anchor_back_walk, 2.2f, 0.0f,
                         Animation_GetLastFrame(&gPlayerAnim_link_anchor_back_walk), ANIMMODE_ONCE, -6.0f);
    this->linearVelocity = 8.0f;
    this->currentYaw = yaw;
}

void Player_SetupSidewalk(Player* this, PlayState* play) {
    Player_SetActionFunc(play, this, Player_Sidewalk, 1);
    Player_ChangeAnimShortMorphLoop(play, this,
                                    GET_PLAYER_ANIM(PLAYER_ANIMGROUP_SIDEWALKING_RIGHT, this->modelAnimType));
    this->walkFrame = 0.0f;
}

void Player_SetupEndUnfriendlyBackwalk(Player* this, PlayState* play) {
    Player_SetActionFunc(play, this, Player_EndUnfriendlyBackwalk, 1);
    LinkAnimation_PlayOnceSetSpeed(play, &this->skelAnime, &gPlayerAnim_link_anchor_back_brake, 2.0f);
}

void Player_SetupTurn(PlayState* play, Player* this, s16 yaw) {
    this->currentYaw = yaw;
    Player_SetActionFunc(play, this, Player_Turn, 1);
    this->unk_87E = 1200;
    this->unk_87E *= sWaterSpeedScale;
    LinkAnimation_Change(play, &this->skelAnime,
                         GET_PLAYER_ANIM(PLAYER_ANIMGROUP_SHUFFLE_TURN, this->modelAnimType),
                         1.0f, 0.0f, 0.0f, ANIMMODE_LOOP, -6.0f);
}

void Player_EndUnfriendlyZTarget(Player* this, PlayState* play) {
    LinkAnimationHeader* anim;

    Player_SetActionFunc(play, this, Player_StandingStill, 1);

    if (this->leftRightBlendWeight < 0.5f) {
        anim = GET_PLAYER_ANIM(PLAYER_ANIMGROUP_END_FIGHTING_RIGHT_OF_ENEMY, this->modelAnimType);
    } else {
        anim = GET_PLAYER_ANIM(PLAYER_ANIMGROUP_END_FIGHTING_LEFT_OF_ENEMY, this->modelAnimType);
    }
    Player_PlayAnimOnce(play, this, anim);

    this->currentYaw = this->actor.shape.rot.y;
}

void Player_SetupUnfriendlyZTarget(Player* this, PlayState* play) {
    Player_SetActionFunc(play, this, Player_UnfriendlyZTargetStandingStill, 1);
    Player_ChangeAnimMorphToLastFrame(play, this,
                                      GET_PLAYER_ANIM(PLAYER_ANIMGROUP_START_FIGHTING, this->modelAnimType));
    this->genericTimer = 1;
}

void Player_SetupEndUnfriendlyZTarget(Player* this, PlayState* play) {
    if (this->linearVelocity != 0.0f) {
        Player_SetupRun(this, play);
    } else {
        Player_EndUnfriendlyZTarget(this, play);
    }
}

void Player_EndMiniCsMovement(Player* this, PlayState* play) {
    if (this->linearVelocity != 0.0f) {
        Player_SetupRun(this, play);
    } else {
        Player_SetupStandingStillType(this, play);
    }
}

s32 Player_SetupSpawnSplash(PlayState* play, Player* this, f32 yVelocity, s32 splashScale) {
    f32 yVelEnteringWater = fabsf(yVelocity);
    WaterBox* waterbox;
    f32 waterSurfaceY;
    Vec3f splashPos;
    s32 splashType;

    if (yVelEnteringWater > 2.0f) {
        splashPos.x = this->bodyPartsPos[PLAYER_BODYPART_WAIST].x;
        splashPos.z = this->bodyPartsPos[PLAYER_BODYPART_WAIST].z;
        waterSurfaceY = this->actor.world.pos.y;
        if (WaterBox_GetSurface1(play, &play->colCtx, splashPos.x, splashPos.z, &waterSurfaceY, &waterbox)) {
            if ((waterSurfaceY - this->actor.world.pos.y) < 100.0f) {
                splashType = (yVelEnteringWater <= 10.0f) ? 0 : 1;
                splashPos.y = waterSurfaceY;
                EffectSsGSplash_Spawn(play, &splashPos, NULL, NULL, splashType, splashScale);
                return 1;
            }
        }
    }

    return 0;
}

void Player_StartJumpOutOfWater(PlayState* play, Player* this, f32 yVelocity) {
    this->stateFlags1 |= PLAYER_STATE1_JUMPING;
    this->stateFlags1 &= ~PLAYER_STATE1_SWIMMING;

    Player_ResetSubCam(play, this);
    if (Player_SetupSpawnSplash(play, this, yVelocity, 500)) {
        func_8002F7DC(&this->actor, NA_SE_EV_JUMP_OUT_WATER);
    }

    Player_SetBootData(play, this);
}

s32 Player_SetupDive(PlayState* play, Player* this, Input* arg2) {
    if (!(this->stateFlags1 & PLAYER_STATE1_GETTING_ITEM) && !(this->stateFlags2 & PLAYER_STATE2_DIVING)) {
        if ((arg2 == NULL) || (CHECK_BTN_ALL(arg2->press.button, BTN_A) && (ABS(this->shapePitchOffset) < 12000) &&
                               (this->currentBoots != PLAYER_BOOTS_IRON))) {

            Player_SetActionFunc(play, this, Player_Dive, 0);
            Player_PlayAnimOnce(play, this, &gPlayerAnim_link_swimer_swim_deep_start);

            this->shapePitchOffset = 0;
            this->stateFlags2 |= PLAYER_STATE2_DIVING;
            this->actor.velocity.y = 0.0f;

            if (arg2 != NULL) {
                this->stateFlags2 |= PLAYER_STATE2_ENABLE_DIVE_CAMERA_AND_TIMER;
                func_8002F7DC(&this->actor, NA_SE_PL_DIVE_BUBBLE);
            }

            return 1;
        }
    }

    if ((this->stateFlags1 & PLAYER_STATE1_GETTING_ITEM) || (this->stateFlags2 & PLAYER_STATE2_DIVING)) {
        if (this->actor.velocity.y > 0.0f) {
            if (this->actor.yDistToWater < this->ageProperties->unk_30) {

                this->stateFlags2 &= ~PLAYER_STATE2_DIVING;

                if (arg2 != NULL) {
                    Player_SetActionFunc(play, this, Player_GetItemInWater, 1);

                    if (this->stateFlags1 & PLAYER_STATE1_GETTING_ITEM) {
                        this->stateFlags1 |=
                            PLAYER_STATE1_GETTING_ITEM | PLAYER_STATE1_HOLDING_ACTOR | PLAYER_STATE1_IN_CUTSCENE;
                    }

                    this->genericTimer = 2;
                }

                Player_ResetSubCam(play, this);
                Player_ChangeAnimMorphToLastFrame(play, this,
                    (this->stateFlags1 & PLAYER_STATE1_HOLDING_ACTOR) ? &gPlayerAnim_link_swimer_swim_get
                                                                      : &gPlayerAnim_link_swimer_swim_deep_end);

                if (Player_SetupSpawnSplash(play, this, this->actor.velocity.y, 500)) {
                    func_8002F7DC(&this->actor, NA_SE_PL_FACE_UP);
                }

                return 1;
            }
        }
    }

    return 0;
}

void Player_RiseFromDive(PlayState* play, Player* this) {
    Player_PlayAnimLoop(play, this, &gPlayerAnim_link_swimer_swim);
    this->shapePitchOffset = 16000;
    this->genericTimer = 1;
}

void func_8083D36C(PlayState* play, Player* this) {
    if (CVarGetInteger("gNoWater", 0)) {
        return;
    }
    if ((this->currentBoots != PLAYER_BOOTS_IRON) || !(this->actor.bgCheckFlags & 1)) {
        Player_ResetAttributesAndHeldActor(play, this);

        if ((this->currentBoots != PLAYER_BOOTS_IRON) && (this->stateFlags2 & PLAYER_STATE2_DIVING)) {
            this->stateFlags2 &= ~PLAYER_STATE2_DIVING;
            Player_SetupDive(play, this, 0);
            this->genericVar = 1;
        } else if (Player_FallingDive == this->actionFunc) {
            Player_SetActionFunc(play, this, Player_Dive, 0);
            Player_RiseFromDive(play, this);
        } else {
            Player_SetActionFunc(play, this, Player_UpdateSwimIdle, 1);
            Player_ChangeAnimMorphToLastFrame(play, this,
                (this->actor.bgCheckFlags & 1) ? &gPlayerAnim_link_swimer_wait2swim_wait
                                               : &gPlayerAnim_link_swimer_land2swim_wait);
        }
    }

    if (!(this->stateFlags1 & PLAYER_STATE1_SWIMMING) || (this->actor.yDistToWater < this->ageProperties->unk_2C)) {
        if (Player_SetupSpawnSplash(play, this, this->actor.velocity.y, 500)) {
            func_8002F7DC(&this->actor, NA_SE_EV_DIVE_INTO_WATER);

            if (this->fallDistance > 800.0f) {
                Player_PlayVoiceSfxForAge(this, NA_SE_VO_LI_CLIMB_END);
            }
        }
    }

    this->stateFlags1 |= PLAYER_STATE1_SWIMMING;
    this->stateFlags2 |= PLAYER_STATE2_DIVING;
    this->stateFlags1 &= ~(PLAYER_STATE1_JUMPING | PLAYER_STATE1_FREEFALLING);
    this->rippleTimer = 0.0f;

    Player_SetBootData(play, this);
}

void func_8083D53C(PlayState* play, Player* this) {
    if (this->actor.yDistToWater < this->ageProperties->unk_2C) {
        Audio_SetBaseFilter(0);
        this->unk_840 = 0;
    } else {
        Audio_SetBaseFilter(0x20);
        if (this->unk_840 < 300) {
            this->unk_840++;
        }
    }

    if ((Player_JumpUpToLedge != this->actionFunc) && (Player_ClimbOntoLedge != this->actionFunc)) {
        if (this->ageProperties->unk_2C < this->actor.yDistToWater) {
            if (!(this->stateFlags1 & PLAYER_STATE1_SWIMMING) ||
                (!((this->currentBoots == PLAYER_BOOTS_IRON) && (this->actor.bgCheckFlags & 1)) &&
                 (Player_DamagedSwim != this->actionFunc) && (Player_Drown != this->actionFunc) &&
                 (Player_UpdateSwimIdle != this->actionFunc) && (Player_Swim != this->actionFunc) &&
                 (Player_ZTargetSwimming != this->actionFunc) && (Player_Dive != this->actionFunc) &&
                 (Player_GetItemInWater != this->actionFunc) && (Player_SpawnSwimming != this->actionFunc))) {
                func_8083D36C(play, this);
                return;
            }
        } else if ((this->stateFlags1 & PLAYER_STATE1_SWIMMING) &&
                   (this->actor.yDistToWater < this->ageProperties->unk_24)) {
            if ((this->skelAnime.moveFlags == 0) && (this->currentBoots != PLAYER_BOOTS_IRON)) {
                Player_SetupTurn(play, this, this->actor.shape.rot.y);
            }
            Player_StartJumpOutOfWater(play, this, this->actor.velocity.y);
        }
    }
}

void func_8083D6EC(PlayState* play, Player* this) {
    Vec3f ripplePos;
    f32 unsinkSpeed;
    f32 maxSinkSpeed;
    f32 sinkSpeed;
    f32 posDiffMag;

    this->actor.minVelocityY = -20.0f;
    this->actor.gravity = REG(68) / 100.0f;

    if (Player_IsFloorSinkingSand(sFloorSpecialProperty) || CVarGetInteger("gSinkingFloor", 0)) {
        unsinkSpeed = fabsf(this->linearVelocity) * 20.0f;
        sinkSpeed = 0.0f;

        if (sFloorSpecialProperty == 4 || CVarGetInteger("gSinkingFloor", 0)) {
            if (this->shapeOffsetY > 1300.0f && !(CVarGetInteger("gSinkingFloor", 0) && sFloorSpecialProperty != 4)) {
                maxSinkSpeed = this->shapeOffsetY;
            } else {
                if (CVarGetInteger("gSinkingFloor", 0)) {
                    maxSinkSpeed = 4250.0f;
                }
                else {
                    maxSinkSpeed = 1300.0f;
                }
            }
            if (this->currentBoots == PLAYER_BOOTS_HOVER) {
                unsinkSpeed += unsinkSpeed;
            } else if (this->currentBoots == PLAYER_BOOTS_IRON) {
                unsinkSpeed *= 0.3f;
            }
        } else {
            maxSinkSpeed = 20000.0f;
            if (this->currentBoots != PLAYER_BOOTS_HOVER) {
                unsinkSpeed += unsinkSpeed;
            } else if ((sFloorSpecialProperty == 7) || (this->currentBoots == PLAYER_BOOTS_IRON)) {
                unsinkSpeed = 0;
            }
        }

        if (this->currentBoots != PLAYER_BOOTS_HOVER) {
            sinkSpeed = (maxSinkSpeed - this->shapeOffsetY) * 0.02f;
            sinkSpeed = CLAMP(sinkSpeed, 0.0f, 300.0f);
            if (this->currentBoots == PLAYER_BOOTS_IRON) {
                sinkSpeed += sinkSpeed;
            }
        }

        this->shapeOffsetY += sinkSpeed - unsinkSpeed;
        this->shapeOffsetY = CLAMP(this->shapeOffsetY, 0.0f, maxSinkSpeed);

        this->actor.gravity -= this->shapeOffsetY * 0.004f;
    } else {
        this->shapeOffsetY = 0.0f;
    }

    if (this->actor.bgCheckFlags & 0x20) {
        if (this->actor.yDistToWater < 50.0f) {
            posDiffMag = fabsf(this->bodyPartsPos[PLAYER_BODYPART_WAIST].x - this->prevWaistPos.x) +
                    fabsf(this->bodyPartsPos[PLAYER_BODYPART_WAIST].y - this->prevWaistPos.y) +
                    fabsf(this->bodyPartsPos[PLAYER_BODYPART_WAIST].z - this->prevWaistPos.z);
            if (posDiffMag > 4.0f) {
                posDiffMag = 4.0f;
            }
            this->rippleTimer += posDiffMag;

            if (this->rippleTimer > 15.0f) {
                this->rippleTimer = 0.0f;

                ripplePos.x = (Rand_ZeroOne() * 10.0f) + this->actor.world.pos.x;
                ripplePos.y = this->actor.world.pos.y + this->actor.yDistToWater;
                ripplePos.z = (Rand_ZeroOne() * 10.0f) + this->actor.world.pos.z;
                EffectSsGRipple_Spawn(play, &ripplePos, 100, 500, 0);

                if ((this->linearVelocity > 4.0f) && !Player_IsSwimming(this) &&
                    ((this->actor.world.pos.y + this->actor.yDistToWater) <
                     this->bodyPartsPos[PLAYER_BODYPART_WAIST].y)) {
                    Player_SetupSpawnSplash(play, this, 20.0f,
                                  (fabsf(this->linearVelocity) * 50.0f) + (this->actor.yDistToWater * 5.0f));
                }
            }
        }

        if (this->actor.yDistToWater > 40.0f) {
            s32 numBubbles = 0;
            s32 i;

            if ((this->actor.velocity.y > -1.0f) || (this->actor.bgCheckFlags & 1)) {
                if (Rand_ZeroOne() < 0.2f) {
                    numBubbles = 1;
                }
            } else {
                numBubbles = this->actor.velocity.y * -2.0f;
            }

            for (i = 0; i < numBubbles; i++) {
                EffectSsBubble_Spawn(play, &this->actor.world.pos, 20.0f, 10.0f, 20.0f, 0.13f);
            }
        }
    }
}

s32 Player_LookAtTargetActor(Player* this, s32 flag) {
    Actor* targetActor = this->targetActor;
    Vec3f sp30;
    s16 sp2E;
    s16 sp2C;

    sp30.x = this->actor.world.pos.x;
    sp30.y = this->bodyPartsPos[PLAYER_BODYPART_HEAD].y + 3.0f;
    sp30.z = this->actor.world.pos.z;
    sp2E = Math_Vec3f_Pitch(&sp30, &targetActor->focus.pos);
    sp2C = Math_Vec3f_Yaw(&sp30, &targetActor->focus.pos);
    Math_SmoothStepToS(&this->actor.focus.rot.y, sp2C, 4, 10000, 0);
    Math_SmoothStepToS(&this->actor.focus.rot.x, sp2E, 4, 10000, 0);
    this->lookFlags |= 2;

    return Player_UpdateLookAngles(this, flag);
}

static Vec3f D_8085456C = { 0.0f, 100.0f, 40.0f };

void func_8083DC54(Player* this, PlayState* play) {
    s16 sp46;
    s16 temp2;
    f32 temp1;
    Vec3f sp34;

    if (this->targetActor != NULL) {
        if (Actor_PlayerIsAimingReadyFpsItem(this) || Player_IsAimingReadyBoomerang(this)) {
            Player_LookAtTargetActor(this, 1);
        } else {
            Player_LookAtTargetActor(this, 0);
        }
        return;
    }

    if (sFloorSpecialProperty == 11) {
        Math_SmoothStepToS(&this->actor.focus.rot.x, -20000, 10, 4000, 800);
    } else {
        sp46 = 0;
        temp1 = Player_RaycastFloorWithOffset2(play, this, &D_8085456C, &sp34);
        if (temp1 > BGCHECK_Y_MIN) {
            temp2 = Math_Atan2S(40.0f, this->actor.world.pos.y - temp1);
            sp46 = CLAMP(temp2, -4000, 4000);
        }
        this->actor.focus.rot.y = this->actor.shape.rot.y;
        Math_SmoothStepToS(&this->actor.focus.rot.x, sp46, 14, 4000, 30);
    }

    Player_UpdateLookAngles(this, Actor_PlayerIsAimingReadyFpsItem(this) || Player_IsAimingReadyBoomerang(this));
}

void func_8083DDC8(Player* this, PlayState* play) {
    s16 temp1;
    s16 temp2;

    if (!Actor_PlayerIsAimingReadyFpsItem(this) && !Player_IsAimingReadyBoomerang(this) && (this->linearVelocity > 5.0f)) {
        temp1 = this->linearVelocity * 200.0f;
        temp2 = (s16)(this->currentYaw - this->actor.shape.rot.y) * this->linearVelocity * 0.1f;
        temp1 = CLAMP(temp1, -4000, 4000);
        temp2 = CLAMP(-temp2, -4000, 4000);
        Math_ScaledStepToS(&this->upperBodyRot.x, temp1, 900);
        this->headRot.x = -(f32)this->upperBodyRot.x * 0.5f;
        Math_ScaledStepToS(&this->headRot.z, temp2, 300);
        Math_ScaledStepToS(&this->upperBodyRot.z, temp2, 200);
        this->lookFlags |= 0x168;
    } else {
        func_8083DC54(this, play);
    }
}

void Player_SetRunVelocityAndYaw(Player* this, f32 arg1, s16 arg2) {
    Math_AsymStepToF(&this->linearVelocity, arg1, REG(19) / 100.0f, 1.5f);
    Math_ScaledStepToS(&this->currentYaw, arg2, REG(27));
}

void func_8083DFE0(Player* this, f32* arg1, s16* arg2) {
    s16 yawDiff = this->currentYaw - *arg2;

    if (this->swordState == 0) {
        float maxSpeed = R_RUN_SPEED_LIMIT / 100.0f;

        int32_t giSpeedModifier = GameInteractor_RunSpeedModifier();
        if (giSpeedModifier != 0) {
            if (giSpeedModifier > 0) {
                maxSpeed *= giSpeedModifier;
            } else {
                maxSpeed /= abs(giSpeedModifier);
            }
        }

        if (CVarGetInteger("gMMBunnyHood", BUNNY_HOOD_VANILLA) == BUNNY_HOOD_FAST_AND_JUMP && this->currentMask == PLAYER_MASK_BUNNY) {
            maxSpeed *= 1.5f;
        } 
        
        if (CVarGetInteger("gEnableWalkModify", 0) && !CVarGetInteger("gWalkModifierDoesntChangeJump", 0)) {
            if (CVarGetInteger("gWalkSpeedToggle", 0)) {
                if (gWalkSpeedToggle1) {
                    maxSpeed *= CVarGetFloat("gWalkModifierOne", 1.0f);
                } else if (gWalkSpeedToggle2) {
                    maxSpeed *= CVarGetFloat("gWalkModifierTwo", 1.0f);
                }
            } else {
                if (CHECK_BTN_ALL(sControlInput->cur.button, BTN_MODIFIER1)) {
                    maxSpeed *= CVarGetFloat("gWalkModifierOne", 1.0f);
                } else if (CHECK_BTN_ALL(sControlInput->cur.button, BTN_MODIFIER2)) {
                    maxSpeed *= CVarGetFloat("gWalkModifierTwo", 1.0f);
                }
            }
        }

        this->linearVelocity = CLAMP(this->linearVelocity, -maxSpeed, maxSpeed);
    }

    if (ABS(yawDiff) > 0x6000) {
        if (Math_StepToF(&this->linearVelocity, 0.0f, 1.0f)) {
            this->currentYaw = *arg2;
        }
    } else {
        Math_AsymStepToF(&this->linearVelocity, *arg1, 0.05f, 0.1f);
        Math_ScaledStepToS(&this->currentYaw, *arg2, 200);
    }
}

static struct_80854578 D_80854578[] = {
    { &gPlayerAnim_link_uma_left_up, 35.17f, 6.6099997f },
    { &gPlayerAnim_link_uma_right_up, -34.16f, 7.91f },
};

s32 Player_SetupMountHorse(Player* this, PlayState* play) {
    EnHorse* rideActor = (EnHorse*)this->rideActor;
    f32 unk_04;
    f32 unk_08;
    f32 sp38;
    f32 sp34;
    s32 temp;

    if ((rideActor != NULL) && CHECK_BTN_ALL(sControlInput->press.button, BTN_A)) {
        sp38 = Math_CosS(rideActor->actor.shape.rot.y);
        sp34 = Math_SinS(rideActor->actor.shape.rot.y);

        Player_SetupMiniCsFunc(play, this, Player_SetupRideHorse);

        this->stateFlags1 |= PLAYER_STATE1_RIDING_HORSE;
        this->actor.bgCheckFlags &= ~0x20;

        if (this->mountSide < 0) {
            temp = 0;
        } else {
            temp = 1;
        }

        unk_04 = D_80854578[temp].unk_04;
        unk_08 = D_80854578[temp].unk_08;
        this->actor.world.pos.x =
            rideActor->actor.world.pos.x + rideActor->riderPos.x + ((unk_04 * sp38) + (unk_08 * sp34));
        this->actor.world.pos.z =
            rideActor->actor.world.pos.z + rideActor->riderPos.z + ((unk_08 * sp38) - (unk_04 * sp34));

        this->rideOffsetY = rideActor->actor.world.pos.y - this->actor.world.pos.y;
        this->currentYaw = this->actor.shape.rot.y = rideActor->actor.shape.rot.y;

        Actor_MountHorse(play, this, &rideActor->actor);
        Player_PlayAnimOnce(play, this, D_80854578[temp].anim);
        Player_SetupAnimMovement(play, this, 0x9B);
        this->actor.parent = this->rideActor;
        Player_ClearAttentionModeAndStopMoving(this);
        Actor_DisableLens(play);
        return 1;
    }

    return 0;
}

void func_8083E298(CollisionPoly* arg0, Vec3f* arg1, s16* arg2) {
    arg1->x = COLPOLY_GET_NORMAL(arg0->normal.x);
    arg1->y = COLPOLY_GET_NORMAL(arg0->normal.y);
    arg1->z = COLPOLY_GET_NORMAL(arg0->normal.z);

    *arg2 = Math_Atan2S(arg1->z, arg1->x);
}

static LinkAnimationHeader* D_80854590[] = {
    &gPlayerAnim_link_normal_down_slope_slip,
    &gPlayerAnim_link_normal_up_slope_slip,
};

s32 func_8083E318(PlayState* play, Player* this, CollisionPoly* arg2) {
    s32 pad;
    s16 sp4A;
    Vec3f sp3C;
    s16 sp3A;
    f32 temp1;
    f32 temp2;
    s16 temp3;

    if (!Player_InBlockingCsMode(play, this) && (Player_SlipOnSlope != this->actionFunc) &&
        (SurfaceType_GetSlope(&play->colCtx, arg2, this->actor.floorBgId) == 1)) {
        sp4A = Math_Atan2S(this->actor.velocity.z, this->actor.velocity.x);
        func_8083E298(arg2, &sp3C, &sp3A);
        temp3 = sp3A - sp4A;

        if (ABS(temp3) > 16000) {
            temp1 = (1.0f - sp3C.y) * 40.0f;
            temp2 = (temp1 * temp1) * 0.015f;
            if (temp2 < 1.2f) {
                temp2 = 1.2f;
            }
            this->pushedYaw = sp3A;
            Math_StepToF(&this->pushedSpeed, temp1, temp2);
        } else {
            Player_SetActionFunc(play, this, Player_SlipOnSlope, 0);
            Player_ResetAttributesAndHeldActor(play, this);
            if (sAngleToFloorX >= 0) {
                this->genericVar = 1;
            }
            Player_ChangeAnimShortMorphLoop(play, this, D_80854590[this->genericVar]);
            this->linearVelocity = sqrtf(SQ(this->actor.velocity.x) + SQ(this->actor.velocity.z));
            this->currentYaw = sp4A;
            return 1;
        }
    }

    return 0;
}

// unknown data (unused)
static s32 D_80854598[] = {
    0xFFDB0871, 0xF8310000, 0x00940470, 0xF3980000, 0xFFB504A9, 0x0C9F0000, 0x08010402,
};

void Player_PickupItemDrop(PlayState* play, Player* this, GetItemEntry* giEntry) {
    s32 dropType = giEntry->field & 0x1F;

    if (!CVarGetInteger("gBanItemDropPickup", 0)) {
        if (!(giEntry->field & 0x80)) {
            Item_DropCollectible(play, &this->actor.world.pos, dropType | 0x8000);
            if ((dropType != 4) && (dropType != 8) && (dropType != 9) && (dropType != 0xA) && (dropType != 0) &&
                (dropType != 1) && (dropType != 2) && (dropType != 0x14) && (dropType != 0x13)) {
                Item_Give(play, giEntry->itemId);
            }
        } else {
            Item_Give(play, giEntry->itemId);
        }

        func_80078884((this->getItemId < 0 || this->getItemEntry.getItemId < 0) ? NA_SE_SY_GET_BOXITEM : NA_SE_SY_GET_ITEM);
    }
}

// Sets a flag according to which type of flag is specified in player->pendingFlag.flagType
// and which flag is specified in player->pendingFlag.flagID.
void Player_SetPendingFlag(Player* this, PlayState* play) {
    switch (this->pendingFlag.flagType) {
        case FLAG_SCENE_CLEAR:
            Flags_SetClear(play, this->pendingFlag.flagID);
            break;
        case FLAG_SCENE_COLLECTIBLE:
            Flags_SetCollectible(play, this->pendingFlag.flagID);
            break;
        case FLAG_SCENE_SWITCH:
            Flags_SetSwitch(play, this->pendingFlag.flagID);
            break;
        case FLAG_SCENE_TREASURE:
            Flags_SetTreasure(play, this->pendingFlag.flagID);
            break;
        case FLAG_RANDOMIZER_INF:
            Flags_SetRandomizerInf(this->pendingFlag.flagID);
            break;
        case FLAG_EVENT_CHECK_INF:
            Flags_SetEventChkInf(this->pendingFlag.flagID);
            break;
        case FLAG_NONE:
        default:
            break;
    }
    this->pendingFlag.flagType = FLAG_NONE;
    this->pendingFlag.flagID = 0;
}

s32 Player_SetupGetItemOrHoldBehavior(Player* this, PlayState* play) {
    Actor* interactedActor;

    if(gSaveContext.pendingIceTrapCount) {
        gSaveContext.pendingIceTrapCount--;
        this->stateFlags1 &= ~(PLAYER_STATE1_GETTING_ITEM | PLAYER_STATE1_HOLDING_ACTOR);
        this->actor.colChkInfo.damage = 0;
        Player_SetupDamage(play, this, 3, 0.0f, 0.0f, 0, 20);
        this->getItemId = GI_NONE;
        this->getItemEntry = (GetItemEntry) GET_ITEM_NONE;
        // Gameplay stats: Increment Ice Trap count
        gSaveContext.sohStats.count[COUNT_ICE_TRAPS]++;
        return 1;
    }

    if (iREG(67) || (((interactedActor = this->interactRangeActor) != NULL) &&
                     func_8002D53C(play, &play->actorCtx.titleCtx))) {
        if (iREG(67) || (this->getItemId > GI_NONE)) {
            if (iREG(67)) {
                this->getItemId = iREG(68);
            }

            GetItemEntry giEntry;
            if (this->getItemEntry.objectId == OBJECT_INVALID || (this->getItemId != this->getItemEntry.getItemId)) {
                giEntry = ItemTable_Retrieve(this->getItemId);
            } else {
                giEntry = this->getItemEntry;
            }
            if (giEntry.collectable) {
                if ((interactedActor != &this->actor) && !iREG(67)) {
                    interactedActor->parent = &this->actor;
                }

                iREG(67) = false;

                if (gSaveContext.n64ddFlag && giEntry.getItemId == RG_ICE_TRAP && giEntry.getItemFrom == ITEM_FROM_FREESTANDING) {
                    this->actor.freezeTimer = 30;
                    Player_SetPendingFlag(this, play);
                    Message_StartTextbox(play, 0xF8, NULL);
                    Audio_PlayFanfare(NA_BGM_SMALL_ITEM_GET);
                    GameInteractor_ExecuteOnItemReceiveHooks(this->getItemEntry);
                    gSaveContext.pendingIceTrapCount++;
                    return 1;
                }

                // Show the cutscene for picking up an item. In vanilla, this happens in bombchu bowling alley (because getting bombchus need to show the cutscene)
                // and whenever the player doesn't have the item yet. In rando, we're overruling this because we need to keep showing the cutscene
                // because those items can be randomized and thus it's important to keep showing the cutscene.
                uint8_t showItemCutscene = play->sceneNum == SCENE_BOWLING || Item_CheckObtainability(giEntry.itemId) == ITEM_NONE || gSaveContext.n64ddFlag;

                // Only skip cutscenes for drops when they're items/consumables from bushes/rocks/enemies.
                uint8_t isDropToSkip = (interactedActor->id == ACTOR_EN_ITEM00 && interactedActor->params != 6 && interactedActor->params != 17) || 
                                        interactedActor->id == ACTOR_EN_KAREBABA || 
                                        interactedActor->id == ACTOR_EN_DEKUBABA;

                // Skip cutscenes from picking up consumables with "Fast Pickup Text" enabled, even when the player never picked it up before.
                // But only for bushes/rocks/enemies because otherwise it can lead to softlocks in deku mask theatre and potentially other places.
                uint8_t skipItemCutscene = CVarGetInteger("gFastDrops", 0) && isDropToSkip;

                // Same as above but for rando. Rando is different because we want to enable cutscenes for items that the player already has because
                // those items could be a randomized item coming from scrubs, freestanding PoH's and keys. So we need to once again overrule
                // this specifically for items coming from bushes/rocks/enemies when the player has already picked that item up.
                uint8_t skipItemCutsceneRando = gSaveContext.n64ddFlag && Item_CheckObtainability(giEntry.itemId) != ITEM_NONE && isDropToSkip;

                // Show cutscene when picking up a item.
                if (showItemCutscene && !skipItemCutscene && !skipItemCutsceneRando) {

                    Player_DetatchHeldActor(play, this);
                    Player_LoadGetItemObject(this, giEntry.objectId);

                    if (!(this->stateFlags2 & PLAYER_STATE2_DIVING) || (this->currentBoots == PLAYER_BOOTS_IRON)) {
                        Player_SetupMiniCsFunc(play, this, Player_SetupGetItem);
                        Player_PlayAnimOnceSlowed(play, this, &gPlayerAnim_link_demo_get_itemB);
                        Player_SetCameraTurnAround(play, 9);
                    }

                    this->stateFlags1 |=
                        PLAYER_STATE1_GETTING_ITEM | PLAYER_STATE1_HOLDING_ACTOR | PLAYER_STATE1_IN_CUTSCENE;
                    Player_ClearAttentionModeAndStopMoving(this);
                    return 1;
                }

                // Don't show cutscene when picking up an item.
                Player_PickupItemDrop(play, this, &giEntry);
                this->getItemId = GI_NONE;
                this->getItemEntry = (GetItemEntry)GET_ITEM_NONE;
            }
        } else if (CHECK_BTN_ALL(sControlInput->press.button, BTN_A) &&
                   !(this->stateFlags1 & PLAYER_STATE1_HOLDING_ACTOR) && !(this->stateFlags2 & PLAYER_STATE2_DIVING)) {
            if (this->getItemId != GI_NONE) {
                GetItemEntry giEntry;
                if (this->getItemEntry.objectId == OBJECT_INVALID) {
                    giEntry = ItemTable_Retrieve(-this->getItemId);
                } else {
                    giEntry = this->getItemEntry;
                }
                EnBox* chest = (EnBox*)interactedActor;
                if (CVarGetInteger("gFastChests", 0) != 0) {
                    giEntry.gi = -1 * abs(giEntry.gi);
                }

                if (giEntry.itemId != ITEM_NONE) {
                    if (((Item_CheckObtainability(giEntry.itemId) == ITEM_NONE) && (giEntry.field & 0x40)) ||
                        ((Item_CheckObtainability(giEntry.itemId) != ITEM_NONE) && (giEntry.field & 0x20))) {
                        this->getItemId = -GI_RUPEE_BLUE;
                        giEntry = ItemTable_Retrieve(GI_RUPEE_BLUE);
                    }
                }

                Player_SetupMiniCsFunc(play, this, Player_SetupGetItem);
                this->stateFlags1 |=
                    PLAYER_STATE1_GETTING_ITEM | PLAYER_STATE1_HOLDING_ACTOR | PLAYER_STATE1_IN_CUTSCENE;
                Player_LoadGetItemObject(this, giEntry.objectId);
                this->actor.world.pos.x =
                    chest->dyna.actor.world.pos.x - (Math_SinS(chest->dyna.actor.shape.rot.y) * 29.4343f);
                this->actor.world.pos.z =
                    chest->dyna.actor.world.pos.z - (Math_CosS(chest->dyna.actor.shape.rot.y) * 29.4343f);
                this->currentYaw = this->actor.shape.rot.y = chest->dyna.actor.shape.rot.y;
                Player_ClearAttentionModeAndStopMoving(this);

                if ((giEntry.itemId != ITEM_NONE) && (giEntry.gi >= 0) &&
                    (Item_CheckObtainability(giEntry.itemId) == ITEM_NONE)) {
                    Player_PlayAnimOnceSlowed(play, this, this->ageProperties->unk_98);
                    Player_SetupAnimMovement(play, this, 0x28F);
                    chest->unk_1F4 = 1;
                    Camera_ChangeSetting(Play_GetCamera(play, 0), CAM_SET_SLOW_CHEST_CS);
                } else {
                    Player_PlayAnimOnce(play, this, &gPlayerAnim_link_normal_box_kick);
                    chest->unk_1F4 = -1;
                }

                return 1;
            }

            if ((this->heldActor == NULL) || Player_HoldsHookshot(this)) {
                if ((interactedActor->id == ACTOR_BG_TOKI_SWD) && LINK_IS_ADULT) {
                    s32 sp24 = this->itemAction;

                    this->itemAction = PLAYER_IA_NONE;
                    this->modelAnimType = PLAYER_ANIMTYPE_DEFAULT;
                    this->heldItemAction = this->itemAction;
                    Player_SetupMiniCsFunc(play, this, Player_SetupHoldActor);

                    if (sp24 == PLAYER_IA_SWORD_MASTER) {
                        this->nextModelGroup = Player_ActionToModelGroup(this, PLAYER_IA_LAST_USED);
                        Player_ChangeItem(play, this, PLAYER_IA_LAST_USED);
                    }
                    else {
                        Player_UseItem(play, this, ITEM_LAST_USED);
                    }
                } else {
                    s32 strength = Player_GetStrength();

                    if ((interactedActor->id == ACTOR_EN_ISHI) && ((interactedActor->params & 0xF) == 1) &&
                        (strength < PLAYER_STR_SILVER_G)) {
                        return 0;
                    }

                    Player_SetupMiniCsFunc(play, this, Player_SetupHoldActor);
                }

                Player_ClearAttentionModeAndStopMoving(this);
                this->stateFlags1 |= PLAYER_STATE1_HOLDING_ACTOR;
                return 1;
            }
        }
    }

    return 0;
}

void func_8083EA94(Player* this, PlayState* play) {
    Player_SetActionFunc(play, this, Player_StartThrowActor, 1);
    Player_PlayAnimOnce(play, this, GET_PLAYER_ANIM(PLAYER_ANIMGROUP_THROWING_OBJECT, this->modelAnimType));
}

s32 func_8083EAF0(Player* this, Actor* actor) {
    if ((actor != NULL) && !(actor->flags & ACTOR_FLAG_ALWAYS_THROWN) &&
        ((this->linearVelocity < 1.1f) || (actor->id == ACTOR_EN_BOM_CHU))) {
        return 0;
    }

    return 1;
}

s32 Player_SetupPutDownOrThrowActor(Player* this, PlayState* play) {
    u16 buttonsToCheck = BTN_A | BTN_B | BTN_CLEFT | BTN_CRIGHT | BTN_CDOWN;
    if (CVarGetInteger("gDpadEquips", 0) != 0) {
        buttonsToCheck |= BTN_DUP | BTN_DDOWN | BTN_DLEFT | BTN_DRIGHT;
    }
    if ((this->stateFlags1 & PLAYER_STATE1_HOLDING_ACTOR) && (this->heldActor != NULL) &&
        CHECK_BTN_ANY(sControlInput->press.button, buttonsToCheck)) {
        if (!Player_InterruptHoldingActor(play, this, this->heldActor)) {
            if (!func_8083EAF0(this, this->heldActor)) {
                Player_SetActionFunc(play, this, Player_SetupPutDownActor, 1);
                Player_PlayAnimOnce(play, this,
                                    GET_PLAYER_ANIM(PLAYER_ANIMGROUP_PUTTING_DOWN_OBJECT, this->modelAnimType));
            } else {
                func_8083EA94(this, play);
            }
        }
        return 1;
    }

    return 0;
}

s32 func_8083EC18(Player* this, PlayState* play, u32 arg2) {
    if (this->wallHeight >= 79.0f) {
        if (!(this->stateFlags1 & PLAYER_STATE1_SWIMMING) || (this->currentBoots == PLAYER_BOOTS_IRON) ||
            (this->actor.yDistToWater < this->ageProperties->unk_2C)) {
            s32 sp8C = (arg2 & 8) ? 2 : 0;

            if ((sp8C != 0) || (arg2 & 2) ||
                func_80041E4C(&play->colCtx, this->actor.wallPoly, this->actor.wallBgId)) {
                f32 phi_f20;
                CollisionPoly* sp84 = this->actor.wallPoly;
                f32 sp80;
                f32 sp7C;
                f32 phi_f12;
                f32 phi_f14;

                phi_f20 = phi_f12 = 0.0f;

                if (sp8C != 0) {
                    sp80 = this->actor.world.pos.x;
                    sp7C = this->actor.world.pos.z;
                } else {
                    Vec3f sp50[3];
                    s32 i;
                    f32 sp48;
                    Vec3f* sp44 = &sp50[0];
                    s32 pad;

                    CollisionPoly_GetVerticesByBgId(sp84, this->actor.wallBgId, &play->colCtx, sp50);

                    sp80 = phi_f12 = sp44->x;
                    sp7C = phi_f14 = sp44->z;
                    phi_f20 = sp44->y;
                    for (i = 1; i < 3; i++) {
                        sp44++;
                        if (sp80 > sp44->x) {
                            sp80 = sp44->x;
                        } else if (phi_f12 < sp44->x) {
                            phi_f12 = sp44->x;
                        }

                        if (sp7C > sp44->z) {
                            sp7C = sp44->z;
                        } else if (phi_f14 < sp44->z) {
                            phi_f14 = sp44->z;
                        }

                        if (phi_f20 > sp44->y) {
                            phi_f20 = sp44->y;
                        }
                    }

                    sp80 = (sp80 + phi_f12) * 0.5f;
                    sp7C = (sp7C + phi_f14) * 0.5f;

                    phi_f12 = ((this->actor.world.pos.x - sp80) * COLPOLY_GET_NORMAL(sp84->normal.z)) -
                              ((this->actor.world.pos.z - sp7C) * COLPOLY_GET_NORMAL(sp84->normal.x));
                    sp48 = this->actor.world.pos.y - phi_f20;

                    phi_f20 = ((f32)(s32)((sp48 / 15.000000223517418) + 0.5) * 15.000000223517418) - sp48;
                    phi_f12 = fabsf(phi_f12);
                }

                if (phi_f12 < 8.0f) {
                    f32 sp3C = COLPOLY_GET_NORMAL(sp84->normal.x);
                    f32 sp38 = COLPOLY_GET_NORMAL(sp84->normal.z);
                    f32 sp34 = this->wallDistance;
                    LinkAnimationHeader* sp30;

                    Player_SetupMiniCsFunc(play, this, Player_SetupClimbingWallOrDownLedge);
                    this->stateFlags1 |= PLAYER_STATE1_CLIMBING;
                    this->stateFlags1 &= ~PLAYER_STATE1_SWIMMING;

                    if ((sp8C != 0) || (arg2 & 2)) {
                        if ((this->genericVar = sp8C) != 0) {
                            if (this->actor.bgCheckFlags & 1) {
                                sp30 = &gPlayerAnim_link_normal_Fclimb_startA;
                            } else {
                                sp30 = &gPlayerAnim_link_normal_Fclimb_hold2upL;
                            }
                            sp34 = (this->ageProperties->unk_38 - 1.0f) - sp34;
                        } else {
                            sp30 = this->ageProperties->unk_A4;
                            sp34 = sp34 - 1.0f;
                        }
                        this->genericTimer = -2;
                        this->actor.world.pos.y += phi_f20;
                        this->actor.shape.rot.y = this->currentYaw = this->actor.wallYaw + 0x8000;
                    } else {
                        sp30 = this->ageProperties->unk_A8;
                        this->genericTimer = -4;
                        this->actor.shape.rot.y = this->currentYaw = this->actor.wallYaw;
                    }

                    this->actor.world.pos.x = (sp34 * sp3C) + sp80;
                    this->actor.world.pos.z = (sp34 * sp38) + sp7C;
                    Player_ClearAttentionModeAndStopMoving(this);
                    Math_Vec3f_Copy(&this->actor.prevPos, &this->actor.world.pos);
                    Player_PlayAnimOnce(play, this, sp30);
                    Player_SetupAnimMovement(play, this, 0x9F);

                    return 1;
                }
            }
        }
    }

    return 0;
}

void func_8083F070(Player* this, LinkAnimationHeader* anim, PlayState* play) {
    Player_SetActionFuncPreserveMoveFlags(play, this, Player_EndClimb, 0);
    LinkAnimation_PlayOnceSetSpeed(play, &this->skelAnime, anim, (4.0f / 3.0f));
}

s32 func_8083F0C8(Player* this, PlayState* play, u32 arg2) {
    CollisionPoly* wallPoly;
    Vec3f wallVertices[3];
    f32 tempX;
    f32 temp;
    f32 tempZ;
    f32 maxWallZ;
    s32 i;

    if (!LINK_IS_ADULT && !(this->stateFlags1 & PLAYER_STATE1_SWIMMING) && (arg2 & 0x30)) {
        wallPoly = this->actor.wallPoly;
        CollisionPoly_GetVerticesByBgId(wallPoly, this->actor.wallBgId, &play->colCtx, wallVertices);

        // compute min and max x/z of wall vertices
        tempX = temp = wallVertices[0].x;
        tempZ = maxWallZ = wallVertices[0].z;
        for (i = 1; i < 3; i++) {
            if (tempX > wallVertices[i].x) {
                tempX = wallVertices[i].x;
            } else if (temp < wallVertices[i].x) {
                temp = wallVertices[i].x;
            }

            if (tempZ > wallVertices[i].z) {
                tempZ = wallVertices[i].z;
            } else if (maxWallZ < wallVertices[i].z) {
                maxWallZ = wallVertices[i].z;
            }
        }

        // average min and max x/z of wall vertices
        tempX = (tempX + temp) * 0.5f;
        tempZ = (tempZ + maxWallZ) * 0.5f;

        temp = ((this->actor.world.pos.x - tempX) * COLPOLY_GET_NORMAL(wallPoly->normal.z)) -
               ((this->actor.world.pos.z - tempZ) * COLPOLY_GET_NORMAL(wallPoly->normal.x));

        if (fabsf(temp) < 8.0f) {
            this->stateFlags2 |= PLAYER_STATE2_CAN_ENTER_CRAWLSPACE;

            if (CHECK_BTN_ALL(sControlInput->press.button, BTN_A)) {
                f32 wallPolyNormX = COLPOLY_GET_NORMAL(wallPoly->normal.x);
                f32 wallPolyNormZ = COLPOLY_GET_NORMAL(wallPoly->normal.z);
                f32 wallDistance = this->wallDistance;

                Player_SetupMiniCsFunc(play, this, Player_SetupInsideCrawlspace);
                this->stateFlags2 |= PLAYER_STATE2_INSIDE_CRAWLSPACE;
                this->actor.shape.rot.y = this->currentYaw = this->actor.wallYaw + 0x8000;
                this->actor.world.pos.x = tempX + (wallDistance * wallPolyNormX);
                this->actor.world.pos.z = tempZ + (wallDistance * wallPolyNormZ);
                Player_ClearAttentionModeAndStopMoving(this);
                this->actor.prevPos = this->actor.world.pos;
                Player_PlayAnimOnce(play, this, &gPlayerAnim_link_child_tunnel_start);
                Player_SetupAnimMovement(play, this, 0x9D);

                return 1;
            }
        }
    }

    return 0;
}

s32 func_8083F360(PlayState* play, Player* this, f32 arg1, f32 arg2, f32 arg3, f32 arg4) {
    CollisionPoly* wallPoly;
    s32 sp78;
    Vec3f sp6C;
    Vec3f sp60;
    Vec3f sp54;
    f32 yawCos;
    f32 yawSin;
    s32 temp;
    f32 temp1;
    f32 temp2;

    yawCos = Math_CosS(this->actor.shape.rot.y);
    yawSin = Math_SinS(this->actor.shape.rot.y);

    sp6C.x = this->actor.world.pos.x + (arg4 * yawSin);
    sp6C.z = this->actor.world.pos.z + (arg4 * yawCos);
    sp60.x = this->actor.world.pos.x + (arg3 * yawSin);
    sp60.z = this->actor.world.pos.z + (arg3 * yawCos);
    sp60.y = sp6C.y = this->actor.world.pos.y + arg1;

    if (BgCheck_EntityLineTest1(&play->colCtx, &sp6C, &sp60, &sp54, &this->actor.wallPoly, true, false, false,
                                true, &sp78)) {
        wallPoly = this->actor.wallPoly;

        this->actor.bgCheckFlags |= 0x200;
        this->actor.wallBgId = sp78;

        sTouchedWallFlags = func_80041DB8(&play->colCtx, wallPoly, sp78);

        temp1 = COLPOLY_GET_NORMAL(wallPoly->normal.x);
        temp2 = COLPOLY_GET_NORMAL(wallPoly->normal.z);
        temp = Math_Atan2S(-temp2, -temp1);
        Math_ScaledStepToS(&this->actor.shape.rot.y, temp, 800);

        this->currentYaw = this->actor.shape.rot.y;
        this->actor.world.pos.x = sp54.x - (Math_SinS(this->actor.shape.rot.y) * arg2);
        this->actor.world.pos.z = sp54.z - (Math_CosS(this->actor.shape.rot.y) * arg2);

        return 1;
    }

    this->actor.bgCheckFlags &= ~0x200;

    return 0;
}

s32 func_8083F524(PlayState* play, Player* this) {
    return func_8083F360(play, this, 26.0f, this->ageProperties->unk_38 + 5.0f, 30.0f, 0.0f);
}

s32 func_8083F570(Player* this, PlayState* play) {
    s16 temp;

    if ((this->linearVelocity != 0.0f) && (this->actor.bgCheckFlags & 8) && (sTouchedWallFlags & 0x30)) {

        temp = this->actor.shape.rot.y - this->actor.wallYaw;
        if (this->linearVelocity < 0.0f) {
            temp += 0x8000;
        }

        if (ABS(temp) > 0x4000) {
            Player_SetActionFunc(play, this, Player_ExitCrawlspace, 0);

            if (this->linearVelocity > 0.0f) {
                this->actor.shape.rot.y = this->actor.wallYaw + 0x8000;
                Player_PlayAnimOnce(play, this, &gPlayerAnim_link_child_tunnel_end);
                Player_SetupAnimMovement(play, this, 0x9D);
                OnePointCutscene_Init(play, 9601, 999, NULL, MAIN_CAM);
            } else {
                this->actor.shape.rot.y = this->actor.wallYaw;
                LinkAnimation_Change(play, &this->skelAnime, &gPlayerAnim_link_child_tunnel_start, -1.0f,
                                     Animation_GetLastFrame(&gPlayerAnim_link_child_tunnel_start), 0.0f, ANIMMODE_ONCE,
                                     0.0f);
                Player_SetupAnimMovement(play, this, 0x9D);
                OnePointCutscene_Init(play, 9602, 999, NULL, MAIN_CAM);
            }

            this->currentYaw = this->actor.shape.rot.y;
            Player_StopMovement(this);

            return 1;
        }
    }

    return 0;
}

void func_8083F72C(Player* this, LinkAnimationHeader* anim, PlayState* play) {
    if (!Player_SetupMiniCsFunc(play, this, Player_SetupGrabPushPullWall)) {
        Player_SetActionFunc(play, this, Player_GrabPushPullWall, 0);
    }

    Player_PlayAnimOnce(play, this, anim);
    Player_ClearAttentionModeAndStopMoving(this);

    this->actor.shape.rot.y = this->currentYaw = this->actor.wallYaw + 0x8000;
}

s32 Player_SetupSpecialWallInteraction(Player* this, PlayState* play) {
    DynaPolyActor* wallPolyActor;

    if (!(this->stateFlags1 & PLAYER_STATE1_HOLDING_ACTOR) && (this->actor.bgCheckFlags & 0x200) &&
        (sYawToTouchedWall < 0x3000)) {

        if (((this->linearVelocity > 0.0f) && func_8083EC18(this, play, sTouchedWallFlags)) ||
            func_8083F0C8(this, play, sTouchedWallFlags)) {
            return 1;
        }

        if (!Player_IsSwimming(this) &&
            ((this->linearVelocity == 0.0f) || !(this->stateFlags2 & PLAYER_STATE2_CAN_CLIMB_PUSH_PULL_WALL)) &&
            (sTouchedWallFlags & 0x40) && (this->actor.bgCheckFlags & 1) && (this->wallHeight >= 39.0f)) {

            this->stateFlags2 |= PLAYER_STATE2_CAN_GRAB_PUSH_PULL_WALL;

            if (CHECK_BTN_ALL(sControlInput->cur.button, BTN_A)) {

                if ((this->actor.wallBgId != BGCHECK_SCENE) &&
                    ((wallPolyActor = DynaPoly_GetActor(&play->colCtx, this->actor.wallBgId)) != NULL)) {

                    if (wallPolyActor->actor.id == ACTOR_BG_HEAVY_BLOCK) {
                        if (Player_GetStrength() < PLAYER_STR_GOLD_G) {
                            return 0;
                        }

                        Player_SetupMiniCsFunc(play, this, Player_SetupHoldActor);
                        this->stateFlags1 |= PLAYER_STATE1_HOLDING_ACTOR;
                        this->interactRangeActor = &wallPolyActor->actor;
                        this->getItemId = GI_NONE;
                        this->getItemEntry = (GetItemEntry)GET_ITEM_NONE;
                        this->currentYaw = this->actor.wallYaw + 0x8000;
                        Player_ClearAttentionModeAndStopMoving(this);

                        return 1;
                    }

                    this->pushPullActor = &wallPolyActor->actor;
                } else {
                    this->pushPullActor = NULL;
                }

                func_8083F72C(this, &gPlayerAnim_link_normal_push_wait, play);

                return 1;
            }
        }
    }

    return 0;
}

s32 func_8083F9D0(PlayState* play, Player* this) {
    if ((this->actor.bgCheckFlags & 0x200) && ((this->stateFlags2 & PLAYER_STATE2_MOVING_PUSH_PULL_WALL) ||
                                               CHECK_BTN_ALL(sControlInput->cur.button, BTN_A))) {
        DynaPolyActor* wallPolyActor = NULL;

        if (this->actor.wallBgId != BGCHECK_SCENE) {
            wallPolyActor = DynaPoly_GetActor(&play->colCtx, this->actor.wallBgId);
        }

        if (&wallPolyActor->actor == this->pushPullActor) {
            if (this->stateFlags2 & PLAYER_STATE2_MOVING_PUSH_PULL_WALL) {
                return 1;
            } else {
                return 0;
            }
        }
    }

    Player_ReturnToStandStill(this, play);
    Player_PlayAnimOnce(play, this, &gPlayerAnim_link_normal_push_wait_end);
    this->stateFlags2 &= ~PLAYER_STATE2_MOVING_PUSH_PULL_WALL;
    return 1;
}

void func_8083FAB8(Player* this, PlayState* play) {
    Player_SetActionFunc(play, this, Player_PushWall, 0);
    this->stateFlags2 |= PLAYER_STATE2_MOVING_PUSH_PULL_WALL;
    Player_PlayAnimOnce(play, this, &gPlayerAnim_link_normal_push_start);
}

void func_8083FB14(Player* this, PlayState* play) {
    Player_SetActionFunc(play, this, Player_PullWall, 0);
    this->stateFlags2 |= PLAYER_STATE2_MOVING_PUSH_PULL_WALL;
    Player_PlayAnimOnce(play, this, GET_PLAYER_ANIM(PLAYER_ANIMGROUP_END_PULL_OBJECT, this->modelAnimType));
}

void func_8083FB7C(Player* this, PlayState* play) {
    this->stateFlags1 &= ~(PLAYER_STATE1_CLIMBING | PLAYER_STATE1_SWIMMING);
    Player_SetupFallFromLedge(this, play);
    this->linearVelocity = -0.4f;
}

s32 func_8083FBC0(Player* this, PlayState* play) {
    if (!CHECK_BTN_ALL(sControlInput->press.button, BTN_A) && (this->actor.bgCheckFlags & 0x200) &&
        ((sTouchedWallFlags & 8) || (sTouchedWallFlags & 2) ||
         func_80041E4C(&play->colCtx, this->actor.wallPoly, this->actor.wallBgId))) {
        return 0;
    }

    func_8083FB7C(this, play);
    Player_PlayVoiceSfxForAge(this, NA_SE_VO_LI_AUTO_JUMP);
    return 1;
}

s32 func_8083FC68(Player* this, f32 arg1, s16 arg2) {
    f32 sp1C = (s16)(arg2 - this->actor.shape.rot.y);
    f32 temp;

    if (this->targetActor != NULL) {
        Player_LookAtTargetActor(this, Actor_PlayerIsAimingReadyFpsItem(this) || Player_IsAimingReadyBoomerang(this));
    }

    temp = fabsf(sp1C) / 32768.0f;

    if (arg1 > (((temp * temp) * 50.0f) + 6.0f)) {
        return 1;
    } else if (arg1 > (((1.0f - temp) * 10.0f) + 6.8f)) {
        return -1;
    }

    return 0;
}

s32 func_8083FD78(Player* this, f32* arg1, s16* arg2, PlayState* play) {
    s16 sp2E = *arg2 - this->targetYaw;
    u16 sp2C = ABS(sp2E);

    if ((Actor_PlayerIsAimingReadyFpsItem(this) || Player_IsAimingReadyBoomerang(this)) && (this->targetActor == NULL)) {
        *arg1 *= Math_SinS(sp2C);

        if (*arg1 != 0.0f) {
            *arg2 = (((sp2E >= 0) ? 1 : -1) << 0xE) + this->actor.shape.rot.y;
        } else {
            *arg2 = this->actor.shape.rot.y;
        }

        if (this->targetActor != NULL) {
            Player_LookAtTargetActor(this, 1);
        } else {
            Math_SmoothStepToS(&this->actor.focus.rot.x, sControlInput->rel.stick_y * 240.0f, 14, 4000, 30);
            Player_UpdateLookAngles(this, 1);
        }
    } else {
        if (this->targetActor != NULL) {
            return func_8083FC68(this, *arg1, *arg2);
        } else {
            func_8083DC54(this, play);
            if ((*arg1 != 0.0f) && (sp2C < 6000)) {
                return 1;
            } else if (*arg1 > Math_SinS((0x4000 - (sp2C >> 1))) * 200.0f) {
                return -1;
            }
        }
    }

    return 0;
}

s32 func_8083FFB8(Player* this, f32* arg1, s16* arg2) {
    s16 temp1 = *arg2 - this->actor.shape.rot.y;
    u16 temp2 = ABS(temp1);
    f32 temp3 = Math_CosS(temp2);

    *arg1 *= temp3;

    if (*arg1 != 0.0f) {
        if (temp3 > 0) {
            return 1;
        } else {
            return -1;
        }
    }

    return 0;
}

s32 func_80840058(Player* this, f32* arg1, s16* arg2, PlayState* play) {
    func_8083DC54(this, play);

    if ((*arg1 != 0.0f) || (ABS(this->unk_87C) > 400)) {
        s16 temp1 = *arg2 - Camera_GetInputDirYaw(GET_ACTIVE_CAM(play));
        u16 temp2 = (ABS(temp1) - 0x2000) & 0xFFFF;

        if ((temp2 < 0x4000) || (this->unk_87C != 0)) {
            return -1;
        } else {
            return 1;
        }
    }

    return 0;
}

void func_80840138(Player* this, f32 arg1, s16 arg2) {
    s16 temp = arg2 - this->actor.shape.rot.y;

    if (arg1 > 0.0f) {
        if (temp < 0) {
            this->leftRightBlendWeightTarget = 0.0f;
        } else {
            this->leftRightBlendWeightTarget = 1.0f;
        }
    }

    Math_StepToF(&this->leftRightBlendWeight, this->leftRightBlendWeightTarget, 0.3f);
}

void func_808401B0(PlayState* play, Player* this) {
    LinkAnimation_BlendToJoint(play, &this->skelAnime, Player_GetFightingRightAnim(this), this->walkFrame,
                               Player_GetFightingLeftAnim(this), this->walkFrame, this->leftRightBlendWeight,
                               this->blendTable);
}

s32 func_8084021C(f32 arg0, f32 arg1, f32 arg2, f32 arg3) {
    f32 temp;

    if ((arg3 == 0.0f) && (arg1 > 0.0f)) {
        arg3 = arg2;
    }

    temp = (arg0 + arg1) - arg3;

    if (((temp * arg1) >= 0.0f) && (((temp - arg1) * arg1) < 0.0f)) {
        return 1;
    }

    return 0;
}

void Player_SetupWalkSfx(Player* this, f32 frameStep) {
    f32 updateScale = R_UPDATE_RATE * 0.5f;

    frameStep *= updateScale;
    if (frameStep < -7.25) {
        frameStep = -7.25;
    } else if (frameStep > 7.25f) {
        frameStep = 7.25f;
    }

    if ((this->currentBoots == PLAYER_BOOTS_HOVER ||
         (CVarGetInteger("gIvanCoopModeEnabled", 0) && this->ivanFloating)) &&
        !(this->actor.bgCheckFlags & 1) &&
        (this->hoverBootsTimer != 0 || (CVarGetInteger("gIvanCoopModeEnabled", 0) && this->ivanFloating))) {
        func_8002F8F0(&this->actor, NA_SE_PL_HOBBERBOOTS_LV - SFX_FLAG);
    } else if (func_8084021C(this->walkFrame, frameStep, 29.0f, 10.0f) ||
               func_8084021C(this->walkFrame, frameStep, 29.0f, 24.0f)) {
        Player_PlayWalkSfx(this, this->linearVelocity);
        if (this->linearVelocity > 4.0f) {
            this->stateFlags2 |= PLAYER_STATE2_MAKING_REACTABLE_NOISE;
        }
    }

    this->walkFrame += frameStep;

    if (this->walkFrame < 0.0f) {
        this->walkFrame += 29.0f;
    } else if (this->walkFrame >= 29.0f) {
        this->walkFrame -= 29.0f;
    }
}

void Player_UnfriendlyZTargetStandingStill(Player* this, PlayState* play) {
    f32 sp44;
    s16 sp42;
    s32 temp1;
    u32 temp2;
    s16 temp3;
    s32 temp4;

    if (this->stateFlags3 & PLAYER_STATE3_ENDING_MELEE_ATTACK) {
        if (Player_GetSwordHeld(this)) {
            this->stateFlags2 |=
                PLAYER_STATE2_DISABLE_MOVE_ROTATION_WHILE_Z_TARGETING | PLAYER_STATE2_ALWAYS_DISABLE_MOVE_ROTATION;
        } else {
            this->stateFlags3 &= ~PLAYER_STATE3_ENDING_MELEE_ATTACK;
        }
    }

    if (this->genericTimer != 0) {
        if (LinkAnimation_Update(play, &this->skelAnime)) {
            Player_EndAnimMovement(this);
            Player_PlayAnimLoop(play, this, Player_GetFightingRightAnim(this));
            this->genericTimer = 0;
            this->stateFlags3 &= ~PLAYER_STATE3_ENDING_MELEE_ATTACK;
        }
        Player_ResetLeftRightBlendWeight(this);
    } else {
        func_808401B0(play, this);
    }

    Player_StepLinearVelocityToZero(this);

    if (!Player_SetupSubAction(play, this, sTargetEnemyStandStillSubActions, 1)) {
        if (!Player_SetupStartUnfriendlyZTargeting(this) && (!Player_IsFriendlyZTargeting(this) || (Player_StandingDefend != this->upperActionFunc))) {
            Player_SetupEndUnfriendlyZTarget(this, play);
            return;
        }

        Player_GetTargetVelocityAndYaw(this, &sp44, &sp42, 0.0f, play);

        temp1 = func_8083FC68(this, sp44, sp42);

        if (temp1 > 0) {
            Player_SetupZTargetRunning(this, play, sp42);
            return;
        }

        if (temp1 < 0) {
            Player_SetupUnfriendlyBackwalk(this, sp42, play);
            return;
        }

        if (sp44 > 4.0f) {
            Player_SetupSidewalk(this, play);
            return;
        }

        Player_SetupWalkSfx(this, (this->linearVelocity * 0.3f) + 1.0f);
        func_80840138(this, sp44, sp42);

        temp2 = this->walkFrame;
        if ((temp2 < 6) || ((temp2 - 0xE) < 6)) {
            Math_StepToF(&this->linearVelocity, 0.0f, 1.5f);
            return;
        }

        temp3 = sp42 - this->currentYaw;
        temp4 = ABS(temp3);

        if (temp4 > 0x4000) {
            if (Math_StepToF(&this->linearVelocity, 0.0f, 1.5f)) {
                this->currentYaw = sp42;
            }
            return;
        }

        Math_AsymStepToF(&this->linearVelocity, sp44 * 0.3f, 2.0f, 1.5f);

        if (!(this->stateFlags3 & PLAYER_STATE3_ENDING_MELEE_ATTACK)) {
            Math_ScaledStepToS(&this->currentYaw, sp42, temp4 * 0.1f);
        }
    }
}

void Player_FriendlyZTargetStandingStill(Player* this, PlayState* play) {
    f32 sp3C;
    s16 sp3A;
    s32 temp1;
    s16 temp2;
    s32 temp3;

    if (LinkAnimation_Update(play, &this->skelAnime)) {
        Player_EndAnimMovement(this);
        Player_PlayAnimOnce(play, this, Player_GetStandingStillAnim(this));
    }

    Player_StepLinearVelocityToZero(this);

    if (!Player_SetupSubAction(play, this, sFriendlyTargetingStandStillSubActions, 1)) {
        if (Player_SetupStartUnfriendlyZTargeting(this)) {
            Player_SetupUnfriendlyZTarget(this, play);
            return;
        }

        if (!Player_IsFriendlyZTargeting(this)) {
            Player_SetActionFuncPreserveMoveFlags(play, this, Player_StandingStill, 1);
            this->currentYaw = this->actor.shape.rot.y;
            return;
        }

        if (Player_StandingDefend == this->upperActionFunc) {
            Player_SetupUnfriendlyZTarget(this, play);
            return;
        }

        Player_GetTargetVelocityAndYaw(this, &sp3C, &sp3A, 0.0f, play);

        temp1 = func_8083FD78(this, &sp3C, &sp3A, play);

        if (temp1 > 0) {
            Player_SetupZTargetRunning(this, play, sp3A);
            return;
        }

        if (temp1 < 0) {
            Player_SetupFriendlyBackwalk(this, sp3A, play);
            return;
        }

        if (sp3C > 4.9f) {
            Player_SetupSidewalk(this, play);
            Player_ResetLeftRightBlendWeight(this);
            return;
        }
        if (sp3C != 0.0f) {
            Player_SetupFriendlySidewalk(this, play);
            return;
        }

        temp2 = sp3A - this->actor.shape.rot.y;
        temp3 = ABS(temp2);

        if (temp3 > 800) {
            Player_SetupTurn(play, this, sp3A);
        }
    }
}

void Player_SetupIdleAnim(PlayState* play, Player* this) {
    LinkAnimationHeader* anim;
    LinkAnimationHeader** animPtr;
    s32 heathIsCritical;
    s32 sp38;
    s32 sp34;

    if ((this->targetActor != NULL) ||
        (!(heathIsCritical = HealthMeter_IsCritical()) && ((this->idleCounter = (this->idleCounter + 1) & 1) != 0))) {
        this->stateFlags2 &= ~PLAYER_STATE2_IDLING;
        anim = Player_GetStandingStillAnim(this);
    } else {
        this->stateFlags2 |= PLAYER_STATE2_IDLING;
        if (this->stateFlags1 & PLAYER_STATE1_HOLDING_ACTOR) {
            anim = Player_GetStandingStillAnim(this);
        } else {
            sp38 = play->roomCtx.curRoom.behaviorType2;
            if (heathIsCritical) {
                if (this->idleCounter >= 0) {
                    sp38 = 7;
                    this->idleCounter = -1;
                } else {
                    sp38 = 8;
                }
            } else {
                sp34 = Rand_ZeroOne() * 5.0f;
                if (sp34 < 4) {
                    if (((sp34 != 0) && (sp34 != 3)) || ((this->rightHandType == PLAYER_MODELTYPE_RH_SHIELD) &&
                                                         ((sp34 == 3) || Player_GetSwordHeld(this)))) {
                        if ((sp34 == 1) && Player_HoldsTwoHandedWeapon(this) && CVarGetInteger("gTwoHandedIdle", 0) == 1) {
                            sp34 = 4;
                        }
                        sp38 = sp34 + 9;
                    }
                }
            }
            animPtr = &sIdleAnims[sp38][0];
            if (this->modelAnimType != PLAYER_ANIMTYPE_HOLDING_ONE_HAND_WEAPON) {
                animPtr = &sIdleAnims[sp38][1];
            }
            anim = *animPtr;
        }
    }

    LinkAnimation_Change(play, &this->skelAnime, anim, (2.0f / 3.0f) * sWaterSpeedScale, 0.0f,
                         Animation_GetLastFrame(anim), ANIMMODE_ONCE, -6.0f);
}

void Player_StandingStill(Player* this, PlayState* play) {
    s32 idleAnimOffset;
    s32 animDone;
    f32 targetVelocity;
    s16 targetYaw;
    s16 targetYawDiff;

    idleAnimOffset = Player_IsPlayingIdleAnim(this);
    animDone = LinkAnimation_Update(play, &this->skelAnime);

    if (idleAnimOffset > 0) {
        Player_PlayIdleAnimSfx(this, idleAnimOffset - 1);
    }

    if (animDone != 0) {
        if (this->genericTimer != 0) {
            if (DECR(this->genericTimer) == 0) {
                this->skelAnime.endFrame = this->skelAnime.animLength - 1.0f;
            }
            this->skelAnime.jointTable[0].y = (this->skelAnime.jointTable[0].y + ((this->genericTimer & 1) * 0x50)) - 0x28;
        } else {
            Player_EndAnimMovement(this);
            Player_SetupIdleAnim(play, this);
        }
    }

    Player_StepLinearVelocityToZero(this);

    if (this->genericTimer == 0) {
        if (!Player_SetupSubAction(play, this, sStandStillSubActions, 1)) {
            if (Player_SetupStartUnfriendlyZTargeting(this)) {
                Player_SetupUnfriendlyZTarget(this, play);
                return;
            }

            if (Player_IsFriendlyZTargeting(this)) {
                Player_SetupFriendlyZTargetingStandStill(this, play);
                return;
            }

            Player_GetTargetVelocityAndYaw(this, &targetVelocity, &targetYaw, 0.018f, play);

            if (targetVelocity != 0.0f) {
                Player_SetupZTargetRunning(this, play, targetYaw);
                return;
            }

            targetYawDiff = targetYaw - this->actor.shape.rot.y;
            if (ABS(targetYawDiff) > 800) {
                Player_SetupTurn(play, this, targetYaw);
                return;
            }

            Math_ScaledStepToS(&this->actor.shape.rot.y, targetYaw, 1200);
            this->currentYaw = this->actor.shape.rot.y;
            if (Player_GetStandingStillAnim(this) == this->skelAnime.animation) {
                func_8083DC54(this, play);
            }
        }
    }
}

void Player_EndSidewalk(Player* this, PlayState* play) {
    f32 frames;
    f32 coeff;
    f32 sp44;
    s16 sp42;
    s32 temp1;
    s16 temp2;
    s32 temp3;
    s32 direction;

    this->skelAnime.mode = 0;
    LinkAnimation_SetUpdateFunction(&this->skelAnime);

    this->skelAnime.animation = Player_GetEndSidewalkAnim(this);

    if (this->skelAnime.animation == &gPlayerAnim_link_bow_side_walk) {
        frames = 24.0f;
        coeff = -(MREG(95) / 100.0f);
    } else {
        frames = 29.0f;
        coeff = MREG(95) / 100.0f;
    }

    this->skelAnime.animLength = frames;
    this->skelAnime.endFrame = frames - 1.0f;

    if ((s16)(this->currentYaw - this->actor.shape.rot.y) >= 0) {
        direction = 1;
    } else {
        direction = -1;
    }

    this->skelAnime.playSpeed = direction * (this->linearVelocity * coeff);

    LinkAnimation_Update(play, &this->skelAnime);

    if (LinkAnimation_OnFrame(&this->skelAnime, 0.0f) || LinkAnimation_OnFrame(&this->skelAnime, frames * 0.5f)) {
        Player_PlayWalkSfx(this, this->linearVelocity);
    }

    if (!Player_SetupSubAction(play, this, sEndSidewalkSubActions, 1)) {
        if (Player_SetupStartUnfriendlyZTargeting(this)) {
            Player_SetupUnfriendlyZTarget(this, play);
            return;
        }

        if (!Player_IsFriendlyZTargeting(this)) {
            Player_SetupStandingStillMorph(this, play);
            return;
        }

        Player_GetTargetVelocityAndYaw(this, &sp44, &sp42, 0.0f, play);
        temp1 = func_8083FD78(this, &sp44, &sp42, play);

        if (temp1 > 0) {
            Player_SetupZTargetRunning(this, play, sp42);
            return;
        }

        if (temp1 < 0) {
            Player_SetupFriendlyBackwalk(this, sp42, play);
            return;
        }

        if (sp44 > 4.9f) {
            Player_SetupSidewalk(this, play);
            Player_ResetLeftRightBlendWeight(this);
            return;
        }

        if ((sp44 == 0.0f) && (this->linearVelocity == 0.0f)) {
            Player_SetupFriendlyZTargetingStandStill(this, play);
            return;
        }

        temp2 = sp42 - this->currentYaw;
        temp3 = ABS(temp2);

        if (temp3 > 0x4000) {
            if (Math_StepToF(&this->linearVelocity, 0.0f, 1.5f)) {
                this->currentYaw = sp42;
            }
            return;
        }

        Math_AsymStepToF(&this->linearVelocity, sp44 * 0.4f, 1.5f, 1.5f);
        Math_ScaledStepToS(&this->currentYaw, sp42, temp3 * 0.1f);
    }
}

void func_80841138(Player* this, PlayState* play) {
    f32 temp1;
    f32 temp2;

    if (this->unk_864 < 1.0f) {
        temp1 = R_UPDATE_RATE * 0.5f;
        Player_SetupWalkSfx(this, REG(35) / 1000.0f);
        LinkAnimation_LoadToJoint(play, &this->skelAnime,
                                  GET_PLAYER_ANIM(PLAYER_ANIMGROUP_BACKWALKING, this->modelAnimType), this->walkFrame);
        this->unk_864 += 1 * temp1;
        if (this->unk_864 >= 1.0f) {
            this->unk_864 = 1.0f;
        }
        temp1 = this->unk_864;
    } else {
        temp2 = this->linearVelocity - (REG(48) / 100.0f);
        if (temp2 < 0.0f) {
            temp1 = 1.0f;
            Player_SetupWalkSfx(this, (REG(35) / 1000.0f) + ((REG(36) / 1000.0f) * this->linearVelocity));
            LinkAnimation_LoadToJoint(play, &this->skelAnime,
                                      GET_PLAYER_ANIM(PLAYER_ANIMGROUP_BACKWALKING, this->modelAnimType),
                                      this->walkFrame);
        } else {
            temp1 = (REG(37) / 1000.0f) * temp2;
            if (temp1 < 1.0f) {
                Player_SetupWalkSfx(this, (REG(35) / 1000.0f) + ((REG(36) / 1000.0f) * this->linearVelocity));
            } else {
                temp1 = 1.0f;
                Player_SetupWalkSfx(this, 1.2f + ((REG(38) / 1000.0f) * temp2));
            }
            LinkAnimation_LoadToMorph(play, &this->skelAnime,
                                      GET_PLAYER_ANIM(PLAYER_ANIMGROUP_BACKWALKING, this->modelAnimType),
                                      this->walkFrame);
            LinkAnimation_LoadToJoint(play, &this->skelAnime, &gPlayerAnim_link_normal_back_run,
                                      this->walkFrame * (16.0f / 29.0f));
        }
    }

    if (temp1 < 1.0f) {
        LinkAnimation_InterpJointMorph(play, &this->skelAnime, 1.0f - temp1);
    }
}

void func_8084140C(Player* this, PlayState* play) {
    Player_SetActionFunc(play, this, Player_HaltFriendlyBackwalk, 1);
    Player_ChangeAnimMorphToLastFrame(play, this, &gPlayerAnim_link_normal_back_brake);
}

s32 func_80841458(Player* this, f32* arg1, s16* arg2, PlayState* play) {
    if (this->linearVelocity > 6.0f) {
        func_8084140C(this, play);
        return 1;
    }

    if (*arg1 != 0.0f) {
        if (Player_StepLinearVelocityToZero(this)) {
            *arg1 = 0.0f;
            *arg2 = this->currentYaw;
        } else {
            return 1;
        }
    }

    return 0;
}

void Player_FriendlyBackwalk(Player* this, PlayState* play) {
    f32 sp34;
    s16 sp32;
    s32 sp2C;
    s16 sp2A;

    func_80841138(this, play);

    if (!Player_SetupSubAction(play, this, sFriendlyBackwalkSubActions, 1)) {
        if (!Player_IsZTargetingSetupStartUnfriendly(this)) {
            Player_SetupZTargetRunning(this, play, this->currentYaw);
            return;
        }

        Player_GetTargetVelocityAndYaw(this, &sp34, &sp32, 0.0f, play);
        sp2C = func_8083FD78(this, &sp34, &sp32, play);

        if (sp2C >= 0) {
            if (!func_80841458(this, &sp34, &sp32, play)) {
                if (sp2C != 0) {
                    Player_SetupRun(this, play);
                } else if (sp34 > 4.9f) {
                    Player_SetupSidewalk(this, play);
                } else {
                    Player_SetupFriendlySidewalk(this, play);
                }
            }
        } else {
            sp2A = sp32 - this->currentYaw;

            Math_AsymStepToF(&this->linearVelocity, sp34 * 1.5f, 1.5f, 2.0f);
            Math_ScaledStepToS(&this->currentYaw, sp32, sp2A * 0.1f);

            if ((sp34 == 0.0f) && (this->linearVelocity == 0.0f)) {
                Player_SetupFriendlyZTargetingStandStill(this, play);
            }
        }
    }
}

void func_808416C0(Player* this, PlayState* play) {
    Player_SetActionFunc(play, this, Player_EndHaltFriendlyBackwalk, 1);
    Player_PlayAnimOnce(play, this, &gPlayerAnim_link_normal_back_brake_end);
}

void Player_HaltFriendlyBackwalk(Player* this, PlayState* play) {
    s32 sp34;
    f32 sp30;
    s16 sp2E;

    sp34 = LinkAnimation_Update(play, &this->skelAnime);
    Player_StepLinearVelocityToZero(this);

    if (!Player_SetupSubAction(play, this, sFriendlyBackwalkSubActions, 1)) {
        Player_GetTargetVelocityAndYaw(this, &sp30, &sp2E, 0.0f, play);

        if (this->linearVelocity == 0.0f) {
            this->currentYaw = this->actor.shape.rot.y;

            if (func_8083FD78(this, &sp30, &sp2E, play) > 0) {
                Player_SetupRun(this, play);
            } else if ((sp30 != 0.0f) || (sp34 != 0)) {
                func_808416C0(this, play);
            }
        }
    }
}

void Player_EndHaltFriendlyBackwalk(Player* this, PlayState* play) {
    s32 sp1C;

    sp1C = LinkAnimation_Update(play, &this->skelAnime);

    if (!Player_SetupSubAction(play, this, sFriendlyBackwalkSubActions, 1)) {
        if (sp1C != 0) {
            Player_SetupFriendlyZTargetingStandStill(this, play);
        }
    }
}

void func_80841860(PlayState* play, Player* this) {
    f32 frame;
    LinkAnimationHeader* sidewalkLeftAnim = GET_PLAYER_ANIM(PLAYER_ANIMGROUP_SIDEWALKING_LEFT, this->modelAnimType);
    LinkAnimationHeader* sidewalkRightAnim = GET_PLAYER_ANIM(PLAYER_ANIMGROUP_SIDEWALKING_RIGHT, this->modelAnimType);

    this->skelAnime.animation = sidewalkLeftAnim;

    Player_SetupWalkSfx(this, (REG(30) / 1000.0f) + ((REG(32) / 1000.0f) * this->linearVelocity));

    frame = this->walkFrame * (16.0f / 29.0f);
    LinkAnimation_BlendToJoint(play, &this->skelAnime, sidewalkRightAnim, frame, sidewalkLeftAnim, frame,
                               this->leftRightBlendWeight, this->blendTable);
}

void Player_Sidewalk(Player* this, PlayState* play) {
    f32 sp3C;
    s16 sp3A;
    s32 temp1;
    s16 temp2;
    s32 temp3;

    func_80841860(play, this);

    if (!Player_SetupSubAction(play, this, sSidewalkSubActions, 1)) {
        if (!Player_IsZTargetingSetupStartUnfriendly(this)) {
            Player_SetupRun(this, play);
            return;
        }

        Player_GetTargetVelocityAndYaw(this, &sp3C, &sp3A, 0.0f, play);

        if (Player_IsFriendlyZTargeting(this)) {
            temp1 = func_8083FD78(this, &sp3C, &sp3A, play);
        } else {
            temp1 = func_8083FC68(this, sp3C, sp3A);
        }

        if (temp1 > 0) {
            Player_SetupRun(this, play);
            return;
        }

        if (temp1 < 0) {
            if (Player_IsFriendlyZTargeting(this)) {
                Player_SetupFriendlyBackwalk(this, sp3A, play);
            } else {
                Player_SetupUnfriendlyBackwalk(this, sp3A, play);
            }
            return;
        }

        if ((this->linearVelocity < 3.6f) && (sp3C < 4.0f)) {
            if (!Player_IsUnfriendlyZTargeting(this) && Player_IsFriendlyZTargeting(this)) {
                Player_SetupFriendlySidewalk(this, play);
            } else {
                Player_SetupStandingStillType(this, play);
            }
            return;
        }

        func_80840138(this, sp3C, sp3A);

        temp2 = sp3A - this->currentYaw;
        temp3 = ABS(temp2);

        if (temp3 > 0x4000) {
            if (Math_StepToF(&this->linearVelocity, 0.0f, 3.0f) != 0) {
                this->currentYaw = sp3A;
            }
            return;
        }

        sp3C *= 0.9f;
        Math_AsymStepToF(&this->linearVelocity, sp3C, 2.0f, 3.0f);
        Math_ScaledStepToS(&this->currentYaw, sp3A, temp3 * 0.1f);
    }
}

void Player_Turn(Player* this, PlayState* play) {
    f32 sp34;
    s16 sp32;

    LinkAnimation_Update(play, &this->skelAnime);

    if (Player_HoldsTwoHandedWeapon(this)) {
        AnimationContext_SetLoadFrame(play, Player_GetStandingStillAnim(this), 0, this->skelAnime.limbCount,
                                      this->skelAnime.morphTable);
        AnimationContext_SetCopyTrue(play, this->skelAnime.limbCount, this->skelAnime.jointTable,
                                     this->skelAnime.morphTable, D_80853410);
    }

    Player_GetTargetVelocityAndYaw(this, &sp34, &sp32, 0.018f, play);

    if (!Player_SetupSubAction(play, this, sTurnSubActions, 1)) {
        if (sp34 != 0.0f) {
            this->actor.shape.rot.y = sp32;
            Player_SetupRun(this, play);
        } else if (Math_ScaledStepToS(&this->actor.shape.rot.y, sp32, this->unk_87E)) {
            Player_SetupStandingStillNoMorph(this, play);
        }

        this->currentYaw = this->actor.shape.rot.y;
    }
}

void Player_BlendWalkAnims(Player* this, s32 blendToMorph, PlayState* play) {
    LinkAnimationHeader* anim;
    s16 targetPitch;
    f32 blendWeight;

    if (ABS(sAngleToFloorX) < 3640) {
        targetPitch = 0;
    } else {
        targetPitch = CLAMP(sAngleToFloorX, -10922, 10922);
    }

    Math_ScaledStepToS(&this->walkAngleToFloorX, targetPitch, 400);

    if ((this->modelAnimType == PLAYER_ANIMTYPE_HOLDING_TWO_HAND_WEAPON) ||
        ((this->walkAngleToFloorX == 0) && (this->shapeOffsetY <= 0.0f))) {
        if (blendToMorph == 0) {
            LinkAnimation_LoadToJoint(play, &this->skelAnime,
                                      GET_PLAYER_ANIM(PLAYER_ANIMGROUP_WALKING, this->modelAnimType), this->walkFrame);
        } else {
            LinkAnimation_LoadToMorph(play, &this->skelAnime,
                                      GET_PLAYER_ANIM(PLAYER_ANIMGROUP_WALKING, this->modelAnimType), this->walkFrame);
        }
        return;
    }

    if (this->walkAngleToFloorX != 0) {
        blendWeight = this->walkAngleToFloorX / 10922.0f;
    } else {
        blendWeight = this->shapeOffsetY * 0.0006f;
    }

    blendWeight *= fabsf(this->linearVelocity) * 0.5f;

    if (blendWeight > 1.0f) {
        blendWeight = 1.0f;
    }

    if (blendWeight < 0.0f) {
        anim = &gPlayerAnim_link_normal_climb_down;
        blendWeight = -blendWeight;
    } else {
        anim = &gPlayerAnim_link_normal_climb_up;
    }

    if (blendToMorph == 0) {
        LinkAnimation_BlendToJoint(play, &this->skelAnime,
                                   GET_PLAYER_ANIM(PLAYER_ANIMGROUP_WALKING, this->modelAnimType), this->walkFrame,
                                   anim, this->walkFrame, blendWeight, this->blendTable);
    } else {
        LinkAnimation_BlendToMorph(play, &this->skelAnime,
                                   GET_PLAYER_ANIM(PLAYER_ANIMGROUP_WALKING, this->modelAnimType), this->walkFrame,
                                   anim, this->walkFrame, blendWeight, this->blendTable);
    }
}

void func_80841EE4(Player* this, PlayState* play) {
    f32 temp1;
    f32 temp2;

    if (this->unk_864 < 1.0f) {
        temp1 = R_UPDATE_RATE * 0.5f;

        Player_SetupWalkSfx(this, REG(35) / 1000.0f);
        LinkAnimation_LoadToJoint(play, &this->skelAnime,
                                  GET_PLAYER_ANIM(PLAYER_ANIMGROUP_WALKING, this->modelAnimType), this->walkFrame);

        this->unk_864 += 1 * temp1;
        if (this->unk_864 >= 1.0f) {
            this->unk_864 = 1.0f;
        }

        temp1 = this->unk_864;
    } else {
        temp2 = this->linearVelocity - (REG(48) / 100.0f);

        if (temp2 < 0.0f) {
            temp1 = 1.0f;
            Player_SetupWalkSfx(this, (REG(35) / 1000.0f) + ((REG(36) / 1000.0f) * this->linearVelocity));

            Player_BlendWalkAnims(this, 0, play);
        } else {
            temp1 = (REG(37) / 1000.0f) * temp2;
            if (temp1 < 1.0f) {
                Player_SetupWalkSfx(this, (REG(35) / 1000.0f) + ((REG(36) / 1000.0f) * this->linearVelocity));
            } else {
                temp1 = 1.0f;
                Player_SetupWalkSfx(this, 1.2f + ((REG(38) / 1000.0f) * temp2));
            }

            Player_BlendWalkAnims(this, 1, play);

            LinkAnimation_LoadToJoint(play, &this->skelAnime, Player_GetRunningAnim(this),
                                      this->walkFrame * (20.0f / 29.0f));
        }
    }

    if (temp1 < 1.0f) {
        LinkAnimation_InterpJointMorph(play, &this->skelAnime, 1.0f - temp1);
    }
}

void Player_Run(Player* this, PlayState* play) {
    f32 sp2C;
    s16 sp2A;

    this->stateFlags2 |= PLAYER_STATE2_DISABLE_MOVE_ROTATION_WHILE_Z_TARGETING;
    func_80841EE4(this, play);

    if (!Player_SetupSubAction(play, this, sRunSubActions, 1)) {
        if (Player_IsZTargetingSetupStartUnfriendly(this)) {
            Player_SetupRun(this, play);
            return;
        }

        Player_GetTargetVelocityAndYaw(this, &sp2C, &sp2A, 0.018f, play);

        if (!Player_SetupTurnAroundRunning(this, &sp2C, &sp2A)) {
            int32_t giSpeedModifier = GameInteractor_RunSpeedModifier();
            if (giSpeedModifier != 0) {
                if (giSpeedModifier > 0) {
                    sp2C *= giSpeedModifier;
                } else {
                    sp2C /= abs(giSpeedModifier);
                }
            }

            if (CVarGetInteger("gMMBunnyHood", BUNNY_HOOD_VANILLA) != BUNNY_HOOD_VANILLA && this->currentMask == PLAYER_MASK_BUNNY) {
                sp2C *= 1.5f;
            }
            
            if (CVarGetInteger("gEnableWalkModify", 0)) {
                if (CVarGetInteger("gWalkSpeedToggle", 0)) {
                    if (gWalkSpeedToggle1) {
                        sp2C *= CVarGetFloat("gWalkModifierOne", 1.0f);
                    } else if (gWalkSpeedToggle2) {
                        sp2C *= CVarGetFloat("gWalkModifierTwo", 1.0f);
                    }
                } else {
                    if (CHECK_BTN_ALL(sControlInput->cur.button, BTN_MODIFIER1)) {
                        sp2C *= CVarGetFloat("gWalkModifierOne", 1.0f);
                    } else if (CHECK_BTN_ALL(sControlInput->cur.button, BTN_MODIFIER2)) {
                        sp2C *= CVarGetFloat("gWalkModifierTwo", 1.0f);
                    }
                }
            }

            Player_SetRunVelocityAndYaw(this, sp2C, sp2A);
            func_8083DDC8(this, play);

            if ((this->linearVelocity == 0.0f) && (sp2C == 0.0f)) {
                Player_SetupEndRun(this, play);
            }
        }
    }
}

void Player_ZTargetingRun(Player* this, PlayState* play) {
    f32 sp2C;
    s16 sp2A;

    this->stateFlags2 |= PLAYER_STATE2_DISABLE_MOVE_ROTATION_WHILE_Z_TARGETING;
    func_80841EE4(this, play);

    if (!Player_SetupSubAction(play, this, sTargetRunSubActions, 1)) {
        if (!Player_IsZTargetingSetupStartUnfriendly(this)) {
            Player_SetupRun(this, play);
            return;
        }

        Player_GetTargetVelocityAndYaw(this, &sp2C, &sp2A, 0.0f, play);

        if (!Player_SetupTurnAroundRunning(this, &sp2C, &sp2A)) {
            if ((Player_IsFriendlyZTargeting(this) && (sp2C != 0.0f) && (func_8083FD78(this, &sp2C, &sp2A, play) <= 0)) ||
                (!Player_IsFriendlyZTargeting(this) && (func_8083FC68(this, sp2C, sp2A) <= 0))) {
                Player_SetupStandingStillType(this, play);
                return;
            }

            Player_SetRunVelocityAndYaw(this, sp2C, sp2A);
            func_8083DDC8(this, play);

            if ((this->linearVelocity == 0) && (sp2C == 0)) {
                Player_SetupStandingStillType(this, play);
            }
        }
    }
}

void Player_UnfriendlyBackwalk(Player* this, PlayState* play) {
    s32 sp34;
    f32 sp30;
    s16 sp2E;

    sp34 = LinkAnimation_Update(play, &this->skelAnime);

    if (!Player_SetupSubAction(play, this, sSidewalkSubActions, 1)) {
        if (!Player_IsZTargetingSetupStartUnfriendly(this)) {
            Player_SetupRun(this, play);
            return;
        }

        Player_GetTargetVelocityAndYaw(this, &sp30, &sp2E, 0.0f, play);

        if ((this->skelAnime.morphWeight == 0.0f) && (this->skelAnime.curFrame > 5.0f)) {
            Player_StepLinearVelocityToZero(this);

            if ((this->skelAnime.curFrame > 10.0f) && (func_8083FC68(this, sp30, sp2E) < 0)) {
                Player_SetupUnfriendlyBackwalk(this, sp2E, play);
                return;
            }

            if (sp34 != 0) {
                Player_SetupEndUnfriendlyBackwalk(this, play);
            }
        }
    }
}

void Player_EndUnfriendlyBackwalk(Player* this, PlayState* play) {
    s32 sp34;
    f32 sp30;
    s16 sp2E;

    sp34 = LinkAnimation_Update(play, &this->skelAnime);

    Player_StepLinearVelocityToZero(this);

    if (!Player_SetupSubAction(play, this, sEndBackwalkSubActions, 1)) {
        Player_GetTargetVelocityAndYaw(this, &sp30, &sp2E, 0.0f, play);

        if (this->linearVelocity == 0.0f) {
            this->currentYaw = this->actor.shape.rot.y;

            if (func_8083FC68(this, sp30, sp2E) > 0) {
                Player_SetupRun(this, play);
                return;
            }

            if ((sp30 != 0.0f) || (sp34 != 0)) {
                Player_SetupStandingStillType(this, play);
            }
        }
    }
}

void func_8084260C(Vec3f* src, Vec3f* dest, f32 arg2, f32 arg3, f32 arg4) {
    dest->x = (Rand_ZeroOne() * arg3) + src->x;
    dest->y = (Rand_ZeroOne() * arg4) + (src->y + arg2);
    dest->z = (Rand_ZeroOne() * arg3) + src->z;
}

static Vec3f D_808545B4 = { 0.0f, 0.0f, 0.0f };
static Vec3f D_808545C0 = { 0.0f, 0.0f, 0.0f };

s32 Player_SetupSpawnDustAtFeet(PlayState* play, Player* this) {
    Vec3f sp2C;

    if ((this->surfaceMaterial == 0) || (this->surfaceMaterial == 1)) {
        func_8084260C(&this->actor.shape.feetPos[FOOT_LEFT], &sp2C,
                      this->actor.floorHeight - this->actor.shape.feetPos[FOOT_LEFT].y, 7.0f, 5.0f);
        func_800286CC(play, &sp2C, &D_808545B4, &D_808545C0, 50, 30);
        func_8084260C(&this->actor.shape.feetPos[FOOT_RIGHT], &sp2C,
                      this->actor.floorHeight - this->actor.shape.feetPos[FOOT_RIGHT].y, 7.0f, 5.0f);
        func_800286CC(play, &this->actor.shape.feetPos[FOOT_RIGHT], &D_808545B4, &D_808545C0, 50, 30);
        return 1;
    }

    return 0;
}

void func_8084279C(Player* this, PlayState* play) {
    Player_LoopAnimContinuously(play, this,
                                GET_PLAYER_ANIM(PLAYER_ANIMGROUP_CHECKING_OR_SPEAKING, this->modelAnimType));

    if (DECR(this->genericTimer) == 0) {
        if (!Player_SetupItemCutsceneOrFirstPerson(this, play)) {
            Player_SetupReturnToStandStillSetAnim(
                this, GET_PLAYER_ANIM(PLAYER_ANIMGROUP_END_CHECKING_OR_SPEAKING, this->modelAnimType), play);
        }

        this->actor.flags &= ~ACTOR_FLAG_PLAYER_TALKED_TO;
        func_8005B1A4(Play_GetCamera(play, 0));
    }
}

s32 func_8084285C(Player* this, f32 arg1, f32 arg2, f32 arg3) {
    if ((arg1 <= this->skelAnime.curFrame) && (this->skelAnime.curFrame <= arg3)) {
        Player_MeleeAttack(this, (arg2 <= this->skelAnime.curFrame) ? 1 : -1);
        return 1;
    }

    Player_InactivateMeleeWeapon(this);
    return 0;
}

s32 func_808428D8(Player* this, PlayState* play) {
    if (Player_IsChildWithHylianShield(this) || !Player_GetSwordHeld(this) || !sUsingItemAlreadyInHand) {
        return 0;
    }

    Player_PlayAnimOnce(play, this, &gPlayerAnim_link_normal_defense_kiru);
    this->genericVar = 1;
    this->meleeWeaponAnimation = 0xC;
    this->currentYaw = this->actor.shape.rot.y + this->upperBodyRot.y;

    if (!CVarGetInteger("gCrouchStabHammerFix", 0)) {
        return 1;
    }

    u32 swordId;
    if (Player_HoldsBrokenKnife(this)) {
        swordId = 1;
    } else {
        swordId = Player_GetSwordHeld(this) - 1;
    }

    if (swordId != 4 && !CVarGetInteger("gCrouchStabFix", 0)) { // 4 = Megaton Hammer
        return 1;
    }

    u32 flags = sMeleeWeaponDmgFlags[swordId][0];
    Player_SetupMeleeWeaponToucherFlags(this, 0, flags);
    Player_SetupMeleeWeaponToucherFlags(this, 1, flags);

    return 1;
}

s32 func_80842964(Player* this, PlayState* play) {
    return Player_SetupItemCutsceneOrFirstPerson(this, play) || Player_SetupSpeakOrCheck(this, play) ||
           Player_SetupGetItemOrHoldBehavior(this, play);
}

void func_808429B4(PlayState* play, s32 speed, s32 y, s32 countdown) {
    s32 quakeIdx = Quake_Add(Play_GetCamera(play, 0), 3);

    Quake_SetSpeed(quakeIdx, speed);
    Quake_SetQuakeValues(quakeIdx, y, 0, 0, 0);
    Quake_SetCountdown(quakeIdx, countdown);
}

void func_80842A28(PlayState* play, Player* this) {
    func_808429B4(play, 27767, 7, 20);
    play->actorCtx.unk_02 = 4;
    Player_RequestRumble(this, 255, 20, 150, 0);
    func_8002F7DC(&this->actor, NA_SE_IT_HAMMER_HIT);
}

void func_80842A88(PlayState* play, Player* this) {
    Inventory_ChangeAmmo(ITEM_STICK, -1);
    Player_UseItem(play, this, ITEM_NONE);
}

s32 func_80842AC4(PlayState* play, Player* this) {
    if ((this->heldItemAction == PLAYER_IA_STICK) && (this->unk_85C > 0.5f)) {
        if (AMMO(ITEM_STICK) != 0) {
            EffectSsStick_Spawn(play, &this->bodyPartsPos[PLAYER_BODYPART_R_HAND],
                                this->actor.shape.rot.y + 0x8000);
            this->unk_85C = 0.5f;
            func_80842A88(play, this);
            func_8002F7DC(&this->actor, NA_SE_IT_WOODSTICK_BROKEN);
        }

        return 1;
    }

    return 0;
}

s32 func_80842B7C(PlayState* play, Player* this) {
    if (this->heldItemAction == PLAYER_IA_SWORD_BGS) {
        if (!gSaveContext.bgsFlag && (gSaveContext.swordHealth > 0.0f)) {
            if ((gSaveContext.swordHealth -= 1.0f) <= 0.0f) {
                EffectSsStick_Spawn(play, &this->bodyPartsPos[PLAYER_BODYPART_R_HAND],
                                    this->actor.shape.rot.y + 0x8000);
                func_800849EC(play);
                func_8002F7DC(&this->actor, NA_SE_IT_MAJIN_SWORD_BROKEN);
            }
        }

        return 1;
    }

    return 0;
}

void func_80842CF0(PlayState* play, Player* this) {
    func_80842AC4(play, this);
    func_80842B7C(play, this);
}

static LinkAnimationHeader* D_808545CC[] = {
    &gPlayerAnim_link_fighter_rebound,
    &gPlayerAnim_link_fighter_rebound_long,
    &gPlayerAnim_link_fighter_reboundR,
    &gPlayerAnim_link_fighter_rebound_longR,
};

void func_80842D20(PlayState* play, Player* this) {
    s32 pad;
    s32 sp28;

    if (Player_AimShieldCrouched != this->actionFunc) {
        Player_ResetAttributes(play, this);
        Player_SetActionFunc(play, this, Player_MeleeWeaponRebound, 0);

        if (Player_IsUnfriendlyZTargeting(this)) {
            sp28 = 2;
        } else {
            sp28 = 0;
        }

        Player_PlayAnimOnceSlowed(play, this, D_808545CC[Player_HoldsTwoHandedWeapon(this) + sp28]);
    }

    Player_RequestRumble(this, 180, 20, 100, 0);
    this->linearVelocity = -18.0f;
    func_80842CF0(play, this);
}

s32 func_80842DF4(PlayState* play, Player* this) {
    f32 phi_f2;
    CollisionPoly* sp78;
    s32 sp74;
    Vec3f sp68;
    Vec3f sp5C;
    Vec3f sp50;
    s32 temp1;
    s32 sp48;

    if (this->swordState > 0) {
        if (this->meleeWeaponAnimation < 0x18) {
            if (!(this->meleeWeaponQuads[0].base.atFlags & AT_BOUNCED) && !(this->meleeWeaponQuads[1].base.atFlags & AT_BOUNCED)) {
                if (this->skelAnime.curFrame >= 2.0f) {

                    phi_f2 = Math_Vec3f_DistXYZAndStoreDiff(&this->meleeWeaponInfo[0].tip, &this->meleeWeaponInfo[0].base, &sp50);
                    if (phi_f2 != 0.0f) {
                        phi_f2 = (phi_f2 + 10.0f) / phi_f2;
                    }

                    sp68.x = this->meleeWeaponInfo[0].tip.x + (sp50.x * phi_f2);
                    sp68.y = this->meleeWeaponInfo[0].tip.y + (sp50.y * phi_f2);
                    sp68.z = this->meleeWeaponInfo[0].tip.z + (sp50.z * phi_f2);

                    if (BgCheck_EntityLineTest1(&play->colCtx, &sp68, &this->meleeWeaponInfo[0].tip, &sp5C, &sp78, true,
                                                false, false, true, &sp74) &&
                        !SurfaceType_IsIgnoredByEntities(&play->colCtx, sp78, sp74) &&
                        (func_80041D4C(&play->colCtx, sp78, sp74) != 6) &&
                        (func_8002F9EC(play, &this->actor, sp78, sp74, &sp5C) == 0)) {

                        if (this->heldItemAction == PLAYER_IA_HAMMER) {
                            Player_SetFreezeFlashTimer(play);
                            func_80842A28(play, this);
                            func_80842D20(play, this);
                            return 1;
                        }

                        if (this->linearVelocity >= 0.0f) {
                            sp48 = func_80041F10(&play->colCtx, sp78, sp74);

                            if (sp48 == 0xA) {
                                CollisionCheck_SpawnShieldParticlesWood(play, &sp5C, &this->actor.projectedPos);
                            } else {
                                CollisionCheck_SpawnShieldParticles(play, &sp5C);
                                if (sp48 == 0xB) {
                                    func_8002F7DC(&this->actor, NA_SE_IT_WALL_HIT_SOFT);
                                } else {
                                    func_8002F7DC(&this->actor, NA_SE_IT_WALL_HIT_HARD);
                                }
                            }

                            func_80842CF0(play, this);
                            this->linearVelocity = -14.0f;
                            Player_RequestRumble(this, 180, 20, 100, 0);
                        }
                    }
                }
            } else {
                func_80842D20(play, this);
                Player_SetFreezeFlashTimer(play);
                return 1;
            }
        }

        temp1 = (this->meleeWeaponQuads[0].base.atFlags & AT_HIT) || (this->meleeWeaponQuads[1].base.atFlags & AT_HIT);

        if (temp1) {
            if (this->meleeWeaponAnimation < 0x18) {
                Actor* at = this->meleeWeaponQuads[temp1 ? 1 : 0].base.at;

                if ((at != NULL) && (at->id != ACTOR_EN_KANBAN)) {
                    Player_SetFreezeFlashTimer(play);
                }
            }

            if ((func_80842AC4(play, this) == 0) && (this->heldItemAction != PLAYER_IA_HAMMER)) {
                func_80842B7C(play, this);

                if (this->actor.colChkInfo.atHitEffect == 1) {
                    this->actor.colChkInfo.damage = 8;
                    Player_SetupDamage(play, this, 4, 0.0f, 0.0f, this->actor.shape.rot.y, 20);
                    return 1;
                }
            }
        }
    }

    return 0;
}

void Player_AimShieldCrouched(Player* this, PlayState* play) {
    f32 sp54;
    f32 sp50;
    s16 sp4E;
    s16 sp4C;
    s16 sp4A;
    s16 sp48;
    s16 sp46;
    f32 sp40;

    if (LinkAnimation_Update(play, &this->skelAnime)) {
        if (!Player_IsChildWithHylianShield(this)) {
            Player_PlayAnimLoop(play, this, GET_PLAYER_ANIM(PLAYER_ANIMGROUP_DEFENDING, this->modelAnimType));
        }
        this->genericTimer = 1;
        this->genericVar = 0;
    }

    if (!Player_IsChildWithHylianShield(this)) {
        this->stateFlags1 |= PLAYER_STATE1_SHIELDING;
        Player_SetupCurrentUpperAction(this, play);
        this->stateFlags1 &= ~PLAYER_STATE1_SHIELDING;
    }

    Player_StepLinearVelocityToZero(this);

    if (this->genericTimer != 0) {
        sp54 = sControlInput->rel.stick_y * 100;
        sp50 = sControlInput->rel.stick_x * (CVarGetInteger("gMirroredWorld", 0) ? 120 : -120);
        sp4E = this->actor.shape.rot.y - Camera_GetInputDirYaw(GET_ACTIVE_CAM(play));

        sp40 = Math_CosS(sp4E);
        sp4C = (Math_SinS(sp4E) * sp50) + (sp54 * sp40);
        sp40 = Math_CosS(sp4E);
        sp4A = (sp50 * sp40) - (Math_SinS(sp4E) * sp54);

        if (sp4C > 3500) {
            sp4C = 3500;
        }

        sp48 = ABS(sp4C - this->actor.focus.rot.x) * 0.25f;
        if (sp48 < 100) {
            sp48 = 100;
        }

        sp46 = ABS(sp4A - this->upperBodyRot.y) * 0.25f;
        if (sp46 < 50) {
            sp46 = 50;
        }

        Math_ScaledStepToS(&this->actor.focus.rot.x, sp4C, sp48);
        this->upperBodyRot.x = this->actor.focus.rot.x;
        Math_ScaledStepToS(&this->upperBodyRot.y, sp4A, sp46);

        if (this->genericVar != 0) {
            if (!func_80842DF4(play, this)) {
                if (this->skelAnime.curFrame < 2.0f) {
                    Player_MeleeAttack(this, 1);
                }
            } else {
                this->genericTimer = 1;
                this->genericVar = 0;
            }
        } else if (!func_80842964(this, play)) {
            if (Player_SetupDefend(this, play)) {
                func_808428D8(this, play);
            } else {
                this->stateFlags1 &= ~PLAYER_STATE1_SHIELDING;
                Player_InactivateMeleeWeapon(this);

                if (Player_IsChildWithHylianShield(this)) {
                    Player_SetupReturnToStandStill(this, play);
                    LinkAnimation_Change(play, &this->skelAnime, &gPlayerAnim_clink_normal_defense_ALL, 1.0f,
                                         Animation_GetLastFrame(&gPlayerAnim_clink_normal_defense_ALL), 0.0f,
                                         ANIMMODE_ONCE, 0.0f);
                    Player_SetupAnimMovement(play, this, 4);
                } else {
                    if (this->itemAction < 0) {
                        Player_SetHeldItem(this);
                    }
                    Player_SetupReturnToStandStillSetAnim(
                        this, GET_PLAYER_ANIM(PLAYER_ANIMGROUP_END_DEFENDING, this->modelAnimType), play);
                }

                func_8002F7DC(&this->actor, NA_SE_IT_SHIELD_REMOVE);
                return;
            }
        } else {
            return;
        }
    }

    this->stateFlags1 |= PLAYER_STATE1_SHIELDING;
    Player_SetModelsForHoldingShield(this);

    this->lookFlags |= 0xC1;
}

void Player_DeflectAttackWithShield(Player* this, PlayState* play) {
    s32 temp;
    LinkAnimationHeader* anim;
    f32 frames;

    Player_StepLinearVelocityToZero(this);

    if (this->genericVar == 0) {
        D_808535E0 = Player_SetupCurrentUpperAction(this, play);
        if ((Player_StandingDefend == this->upperActionFunc) ||
            (Player_IsActionInterrupted(play, this, &this->skelAnimeUpper, 4.0f) > 0)) {
            Player_SetActionFunc(play, this, Player_UnfriendlyZTargetStandingStill, 1);
        }
    } else {
        temp = Player_IsActionInterrupted(play, this, &this->skelAnime, 4.0f);
        if ((temp != 0) && ((temp > 0) || LinkAnimation_Update(play, &this->skelAnime))) {
            Player_SetActionFunc(play, this, Player_AimShieldCrouched, 1);
            this->stateFlags1 |= PLAYER_STATE1_SHIELDING;
            Player_SetModelsForHoldingShield(this);
            anim = GET_PLAYER_ANIM(PLAYER_ANIMGROUP_START_DEFENDING, this->modelAnimType);
            frames = Animation_GetLastFrame(anim);
            LinkAnimation_Change(play, &this->skelAnime, anim, 1.0f, frames, frames, ANIMMODE_ONCE, 0.0f);
        }
    }
}

void func_8084370C(Player* this, PlayState* play) {
    s32 sp1C;

    Player_StepLinearVelocityToZero(this);

    sp1C = Player_IsActionInterrupted(play, this, &this->skelAnime, 16.0f);
    if ((sp1C != 0) && (LinkAnimation_Update(play, &this->skelAnime) || (sp1C > 0))) {
        Player_SetupStandingStillType(this, play);
    }
}

void Player_StartKnockback(Player* this, PlayState* play) {
    this->stateFlags2 |=
        PLAYER_STATE2_DISABLE_MOVE_ROTATION_WHILE_Z_TARGETING | PLAYER_STATE2_ALWAYS_DISABLE_MOVE_ROTATION;

    Player_RoundUpInvincibilityTimer(this);

    if (!(this->stateFlags1 & PLAYER_STATE1_IN_CUTSCENE) && (this->genericTimer == 0) && (this->damageEffect != 0)) {
        s16 temp = this->actor.shape.rot.y - this->damageYaw;

        this->currentYaw = this->actor.shape.rot.y = this->damageYaw;
        this->linearVelocity = this->knockbackVelXZ;

        if (ABS(temp) > 0x4000) {
            this->actor.shape.rot.y = this->damageYaw + 0x8000;
        }

        if (this->actor.velocity.y < 0.0f) {
            this->actor.gravity = 0.0f;
            this->actor.velocity.y = 0.0f;
        }
    }

    if (LinkAnimation_Update(play, &this->skelAnime) && (this->actor.bgCheckFlags & 1)) {
        if (this->genericTimer != 0) {
            this->genericTimer--;
            if (this->genericTimer == 0) {
                Player_SetupStandingStillMorph(this, play);
            }
        } else if ((this->stateFlags1 & PLAYER_STATE1_IN_CUTSCENE) ||
                   (!(this->cylinder.base.acFlags & AC_HIT) && (this->damageEffect == 0))) {
            if (this->stateFlags1 & PLAYER_STATE1_IN_CUTSCENE) {
                this->genericTimer++;
            } else {
                Player_SetActionFunc(play, this, Player_DownFromKnockback, 0);
                this->stateFlags1 |= PLAYER_STATE1_TAKING_DAMAGE;
            }

            Player_PlayAnimOnce(play, this,
                          (this->currentYaw != this->actor.shape.rot.y) ? &gPlayerAnim_link_normal_front_downB
                                                                        : &gPlayerAnim_link_normal_back_downB);
            Player_PlayVoiceSfxForAge(this, NA_SE_VO_LI_FREEZE);
        }
    }

    if (this->actor.bgCheckFlags & 2) {
        Player_PlayMoveSfx(this, NA_SE_PL_BOUND);
    }
}

void Player_DownFromKnockback(Player* this, PlayState* play) {
    this->stateFlags2 |=
        PLAYER_STATE2_DISABLE_MOVE_ROTATION_WHILE_Z_TARGETING | PLAYER_STATE2_ALWAYS_DISABLE_MOVE_ROTATION;
    Player_RoundUpInvincibilityTimer(this);

    Player_StepLinearVelocityToZero(this);

    if (LinkAnimation_Update(play, &this->skelAnime) && (this->linearVelocity == 0.0f)) {
        if (this->stateFlags1 & PLAYER_STATE1_IN_CUTSCENE) {
            this->genericTimer++;
        } else {
            Player_SetActionFunc(play, this, Player_GetUpFromKnockback, 0);
            this->stateFlags1 |= PLAYER_STATE1_TAKING_DAMAGE;
        }

        Player_PlayAnimOnceSlowed(play, this,
            (this->currentYaw != this->actor.shape.rot.y) ? &gPlayerAnim_link_normal_front_down_wake
                                                          : &gPlayerAnim_link_normal_back_down_wake);
        this->currentYaw = this->actor.shape.rot.y;
    }
}

static PlayerAnimSfxEntry D_808545DC[] = {
    { 0, 0x4014 },
    { 0, -0x401E },
};

void Player_GetUpFromKnockback(Player* this, PlayState* play) {
    s32 sp24;

    this->stateFlags2 |= PLAYER_STATE2_DISABLE_MOVE_ROTATION_WHILE_Z_TARGETING;
    Player_RoundUpInvincibilityTimer(this);

    if (this->stateFlags1 & PLAYER_STATE1_IN_CUTSCENE) {
        LinkAnimation_Update(play, &this->skelAnime);
    } else {
        sp24 = Player_IsActionInterrupted(play, this, &this->skelAnime, 16.0f);
        if ((sp24 != 0) && (LinkAnimation_Update(play, &this->skelAnime) || (sp24 > 0))) {
            Player_SetupStandingStillType(this, play);
        }
    }

    Player_PlayAnimSfx(this, D_808545DC);
}

static Vec3f D_808545E4 = { 0.0f, 0.0f, 5.0f };

void func_80843AE8(PlayState* play, Player* this) {
    if (this->genericTimer != 0) {
        if (this->genericTimer > 0) {
            this->genericTimer--;
            if (this->genericTimer == 0) {
                if (this->stateFlags1 & PLAYER_STATE1_SWIMMING) {
                    LinkAnimation_Change(play, &this->skelAnime, &gPlayerAnim_link_swimer_swim_wait, 1.0f, 0.0f,
                                         Animation_GetLastFrame(&gPlayerAnim_link_swimer_swim_wait), ANIMMODE_ONCE,
                                         -16.0f);
                } else {
                    LinkAnimation_Change(play, &this->skelAnime, &gPlayerAnim_link_derth_rebirth, 1.0f, 99.0f,
                                         Animation_GetLastFrame(&gPlayerAnim_link_derth_rebirth), ANIMMODE_ONCE, 0.0f);
                }
                if (CVarGetInteger("gFairyReviveEffect", 0)) {
                    if (CVarGetInteger("gFairyRevivePercentRestore", 0)) {
                        gSaveContext.healthAccumulator =
                            (gSaveContext.healthCapacity * CVarGetInteger("gFairyReviveHealth", 100) / 100 + 15) / 16 * 16;
                    } else {
                        gSaveContext.healthAccumulator = CVarGetInteger("gFairyReviveHealth", 20) * 16;
                    }
                } else {
                    gSaveContext.healthAccumulator = 0x140;
                }
                this->genericTimer = -1;
            }
        } else if (gSaveContext.healthAccumulator == 0) {
            this->stateFlags1 &= ~PLAYER_STATE1_IN_DEATH_CUTSCENE;
            if (this->stateFlags1 & PLAYER_STATE1_SWIMMING) {
                Player_SetupSwimIdle(play, this);
            } else {
                Player_SetupStandingStillMorph(this, play);
            }
            this->deathTimer = 20;
            Player_SetupInvincibilityNoDamageFlash(this, -20);
            func_800F47FC();
        }
    } else if (this->genericVar != 0) {
        this->genericTimer = 60;
        Player_SpawnFairy(play, this, &this->actor.world.pos, &D_808545E4, FAIRY_REVIVE_DEATH);
        func_8002F7DC(&this->actor, NA_SE_EV_FIATY_HEAL - SFX_FLAG);
        OnePointCutscene_Init(play, 9908, 125, &this->actor, MAIN_CAM);
    } else if (play->gameOverCtx.state == GAMEOVER_DEATH_WAIT_GROUND) {
        play->gameOverCtx.state = GAMEOVER_DEATH_DELAY_MENU;
        sMaskMemory = PLAYER_MASK_NONE;
    }
}

static PlayerAnimSfxEntry D_808545F0[] = {
    { NA_SE_PL_BOUND, 0x103C },
    { 0, 0x408C },
    { 0, 0x40A4 },
    { 0, -0x40AA },
};

void Player_Die(Player* this, PlayState* play) {
    if (this->currentTunic != PLAYER_TUNIC_GORON && CVarGetInteger("gSuperTunic", 0) == 0) {
        if ((play->roomCtx.curRoom.behaviorType2 == ROOM_BEHAVIOR_TYPE2_3) || (sFloorSpecialProperty == 9) ||
            ((Player_GetHurtFloorType(sFloorSpecialProperty) >= 0) &&
             !SurfaceType_IsWallDamage(&play->colCtx, this->actor.floorPoly, this->actor.floorBgId))) {
            Player_StartBurning(this);
        }
    }

    Player_StepLinearVelocityToZero(this);

    if (LinkAnimation_Update(play, &this->skelAnime)) {
        if (this->actor.category == ACTORCAT_PLAYER) {
            func_80843AE8(play, this);
        }
        return;
    }

    if (this->skelAnime.animation == &gPlayerAnim_link_derth_rebirth) {
        Player_PlayAnimSfx(this, D_808545F0);
    } else if (this->skelAnime.animation == &gPlayerAnim_link_normal_electric_shock_end) {
        if (LinkAnimation_OnFrame(&this->skelAnime, 88.0f)) {
            Player_PlayMoveSfx(this, NA_SE_PL_BOUND);
        }
    }
}

void func_80843E14(Player* this, u16 sfxId) {
    Player_PlayVoiceSfxForAge(this, sfxId);

    if ((this->heldActor != NULL) && (this->heldActor->id == ACTOR_EN_RU1)) {
        Audio_PlayActorSound2(this->heldActor, NA_SE_VO_RT_FALL);
    }
}

static FallImpactInfo D_80854600[] = {
    { -8, 180, 40, 100, NA_SE_VO_LI_LAND_DAMAGE_S },
    { -16, 255, 140, 150, NA_SE_VO_LI_LAND_DAMAGE_S },
};

s32 func_80843E64(PlayState* play, Player* this) {
    s32 sp34;

    if ((sFloorSpecialProperty == 6) || (sFloorSpecialProperty == 9)) {
        sp34 = 0;
    } else {
        sp34 = this->fallDistance;
    }

    Math_StepToF(&this->linearVelocity, 0.0f, 1.0f);

    this->stateFlags1 &= ~(PLAYER_STATE1_JUMPING | PLAYER_STATE1_FREEFALLING);

    if (sp34 >= 400) {
        s32 impactIndex;
        FallImpactInfo* impactInfo;

        if (this->fallDistance < 800) {
            impactIndex = 0;
        } else {
            impactIndex = 1;
        }

        impactInfo = &D_80854600[impactIndex];

        if (Player_InflictDamageModified(play, impactInfo->damage * (1 << CVarGetInteger("gFallDamageMul", 0)),
                                         false)) {
            return -1;
        }

        Player_SetupInvincibility(this, 40);
        func_808429B4(play, 32967, 2, 30);
        Player_RequestRumble(this, impactInfo->unk_01, impactInfo->unk_02, impactInfo->unk_03, 0);
        func_8002F7DC(&this->actor, NA_SE_PL_BODY_HIT);
        Player_PlayVoiceSfxForAge(this, impactInfo->sfxId);

        return impactIndex + 1;
    }

    if (sp34 > 200) {
        sp34 *= 2;

        if (sp34 > 255) {
            sp34 = 255;
        }

        Player_RequestRumble(this, (u8)sp34, (u8)(sp34 * 0.1f), (u8)sp34, 0);

        if (sFloorSpecialProperty == 6) {
            Player_PlayVoiceSfxForAge(this, NA_SE_VO_LI_CLIMB_END);
        }
    }

    Player_PlayLandingSfx(this);

    return 0;
}

void func_8084409C(PlayState* play, Player* this, f32 speedXZ, f32 velocityY) {
    Actor* heldActor = this->heldActor;

    if (!Player_InterruptHoldingActor(play, this, heldActor)) {
        heldActor->world.rot.y = this->actor.shape.rot.y;
        heldActor->speedXZ = speedXZ;
        heldActor->velocity.y = velocityY;
        Player_SetupHeldItemUpperActionFunc(play, this);
        func_8002F7DC(&this->actor, NA_SE_PL_THROW);
        Player_PlayVoiceSfxForAge(this, NA_SE_VO_LI_SWORD_N);
    }
}

void Player_UpdateMidair(Player* this, PlayState* play) {
    f32 sp4C;
    s16 sp4A;

    if (gSaveContext.respawn[RESPAWN_MODE_TOP].data > 40) {
        this->actor.gravity = 0.0f;
    } else if (Player_IsUnfriendlyZTargeting(this)) {
        this->actor.gravity = -1.2f;
    }

    Player_GetTargetVelocityAndYaw(this, &sp4C, &sp4A, 0.0f, play);

    if (!(this->actor.bgCheckFlags & 1)) {
        if (this->stateFlags1 & PLAYER_STATE1_HOLDING_ACTOR) {
            Actor* heldActor = this->heldActor;

            u16 buttonsToCheck = BTN_A | BTN_B | BTN_CLEFT | BTN_CRIGHT | BTN_CDOWN;
            if (CVarGetInteger("gDpadEquips", 0) != 0) {
                buttonsToCheck |= BTN_DUP | BTN_DDOWN | BTN_DLEFT | BTN_DRIGHT;
            }
            if (!Player_InterruptHoldingActor(play, this, heldActor) && (heldActor->id == ACTOR_EN_NIW) &&
                CHECK_BTN_ANY(sControlInput->press.button, buttonsToCheck)) {
                func_8084409C(play, this, this->linearVelocity + 2.0f, this->actor.velocity.y + 2.0f);
            }
        }

        LinkAnimation_Update(play, &this->skelAnime);

        if (!(this->stateFlags2 & PLAYER_STATE2_BACKFLIPPING_OR_SIDEHOPPING)) {
            func_8083DFE0(this, &sp4C, &sp4A);
        }

        Player_SetupCurrentUpperAction(this, play);

        if (((this->stateFlags2 & PLAYER_STATE2_BACKFLIPPING_OR_SIDEHOPPING) && (this->genericVar == 2)) ||
            !Player_SetupMidairJumpSlash(this, play)) {
            if (this->actor.velocity.y < 0.0f) {
                if (this->genericTimer >= 0) {
                    if ((this->actor.bgCheckFlags & 8) || (this->genericTimer == 0) || (this->fallDistance > 0)) {
                        if ((sPlayerYDistToFloor > 800.0f) || (this->stateFlags1 & PLAYER_STATE1_END_HOOKSHOT_MOVE)) {
                            func_80843E14(this, NA_SE_VO_LI_FALL_S);
                            this->stateFlags1 &= ~PLAYER_STATE1_END_HOOKSHOT_MOVE;
                        }

                        LinkAnimation_Change(play, &this->skelAnime, &gPlayerAnim_link_normal_landing, 1.0f, 0.0f,
                                             0.0f, ANIMMODE_ONCE, 8.0f);
                        this->genericTimer = -1;
                    }
                } else {
                    if ((this->genericTimer == -1) && (this->fallDistance > 120.0f) && (sPlayerYDistToFloor > 280.0f)) {
                        this->genericTimer = -2;
                        func_80843E14(this, NA_SE_VO_LI_FALL_L);
                    }

                    if (!GameInteractor_GetDisableLedgeGrabsActive() && (this->actor.bgCheckFlags & 0x200) &&
                        !(this->stateFlags2 & PLAYER_STATE2_BACKFLIPPING_OR_SIDEHOPPING) &&
                        !(this->stateFlags1 & (PLAYER_STATE1_HOLDING_ACTOR | PLAYER_STATE1_SWIMMING)) && (this->linearVelocity > 0.0f)) {
                        if ((this->wallHeight >= 150.0f) && (this->relativeAnalogStickInputs[this->inputFrameCounter] == 0)) {
                            func_8083EC18(this, play, sTouchedWallFlags);
                        } else if ((this->touchedWallJumpType >= 2) && (this->wallHeight < 150.0f) &&
                                   (((this->actor.world.pos.y - this->actor.floorHeight) + this->wallHeight) >
                                    (70.0f * this->ageProperties->unk_08))) {
                            AnimationContext_DisableQueue(play);
                            if (this->stateFlags1 & PLAYER_STATE1_END_HOOKSHOT_MOVE) {
                                Player_PlayVoiceSfxForAge(this, NA_SE_VO_LI_HOOKSHOT_HANG);
                            } else {
                                Player_PlayVoiceSfxForAge(this, NA_SE_VO_LI_HANG);
                            }
                            this->actor.world.pos.y += this->wallHeight;
                            Player_SetupGrabLedge(play, this, this->actor.wallPoly, this->wallDistance,
                                                  GET_PLAYER_ANIM(PLAYER_ANIMGROUP_HANGING_FROM_LEDGE, this->modelAnimType));
                            this->actor.shape.rot.y = this->currentYaw += 0x8000;
                            this->stateFlags1 |= PLAYER_STATE1_HANGING_FROM_LEDGE_SLIP;
                        }
                    }
                }
            }
        }
    } else {
        LinkAnimationHeader* anim = GET_PLAYER_ANIM(PLAYER_ANIMGROUP_TALL_JUMP_LANDING, this->modelAnimType);
        s32 sp3C;

        if (this->stateFlags2 & PLAYER_STATE2_BACKFLIPPING_OR_SIDEHOPPING) {
            if (Player_IsUnfriendlyZTargeting(this)) {
                anim = sManualJumpAnims[this->genericVar][2];
            } else {
                anim = sManualJumpAnims[this->genericVar][1];
            }
        } else if (this->skelAnime.animation == &gPlayerAnim_link_normal_run_jump) {
            anim = &gPlayerAnim_link_normal_run_jump_end;
        } else if (Player_IsUnfriendlyZTargeting(this)) {
            anim = &gPlayerAnim_link_anchor_landingR;
            Player_ResetLeftRightBlendWeight(this);
        } else if (this->fallDistance <= 80) {
            anim = GET_PLAYER_ANIM(PLAYER_ANIMGROUP_SHORT_JUMP_LANDING, this->modelAnimType);
        } else if ((this->fallDistance < 800) && (this->relativeAnalogStickInputs[this->inputFrameCounter] == 0) &&
                   !(this->stateFlags1 & PLAYER_STATE1_HOLDING_ACTOR)) {
            Player_SetupRolling(this, play);
            return;
        }

        sp3C = func_80843E64(play, this);

        if (sp3C > 0) {
            Player_SetupReturnToStandStillSetAnim(
                this, GET_PLAYER_ANIM(PLAYER_ANIMGROUP_TALL_JUMP_LANDING, this->modelAnimType), play);
            this->skelAnime.endFrame = 8.0f;
            if (sp3C == 1) {
                this->genericTimer = 10;
            } else {
                this->genericTimer = 20;
            }
        } else if (sp3C == 0) {
            Player_SetupReturnToStandStillSetAnim(this, anim, play);
        }
    }
}

static PlayerAnimSfxEntry D_8085460C[] = {
    { NA_SE_VO_LI_SWORD_N, 0x2001 },
    { NA_SE_PL_WALK_GROUND, 0x1806 },
    { NA_SE_PL_ROLL, 0x806 },
    { 0, -0x2812 },
};

void Player_Rolling(Player* this, PlayState* play) {
    Actor* cylinderOc;
    s32 temp;
    s32 sp44;
    DynaPolyActor* wallPolyActor;
    s32 pad;
    f32 targetVelocity;
    s16 targetYaw;

    this->stateFlags2 |= PLAYER_STATE2_DISABLE_MOVE_ROTATION_WHILE_Z_TARGETING;

    cylinderOc = NULL;
    sp44 = LinkAnimation_Update(play, &this->skelAnime);

    if (LinkAnimation_OnFrame(&this->skelAnime, 8.0f)) {
        Player_SetupInvincibilityNoDamageFlash(this, -10);
    }

    if (func_80842964(this, play) == 0) {
        if (this->genericTimer != 0) {
            if (!CVarGetInteger("gSonicRoll", 0)) {
                Math_StepToF(&this->linearVelocity, 0.0f, 2.0f);
            }

            temp = Player_IsActionInterrupted(play, this, &this->skelAnime, 5.0f);
            if ((temp != 0) && ((temp > 0) || sp44)) {
                Player_SetupReturnToStandStill(this, play);
            }
        } else {
            f32 rand = Rand_ZeroOne();
            uint8_t randomBonk = (rand <= .05) && GameInteractor_GetRandomBonksActive();
            if (this->linearVelocity >= 7.0f) {
                if (randomBonk || ((this->actor.bgCheckFlags & 0x200) && (sYawToTouchedWall2 < 0x2000)) ||
                    ((this->cylinder.base.ocFlags1 & OC1_HIT) &&
                     (cylinderOc = this->cylinder.base.oc,
                      ((cylinderOc->id == ACTOR_EN_WOOD02) &&
                       (ABS((s16)(this->actor.world.rot.y - cylinderOc->yawTowardsPlayer)) > 0x6000))))) {

                    if (cylinderOc != NULL) {
                        cylinderOc->home.rot.y = 1;
                    } else if (this->actor.wallBgId != BGCHECK_SCENE) {
                        wallPolyActor = DynaPoly_GetActor(&play->colCtx, this->actor.wallBgId);
                        if ((wallPolyActor != NULL) && (wallPolyActor->actor.id == ACTOR_OBJ_KIBAKO2)) {
                            wallPolyActor->actor.home.rot.z = 1;
                        }
                    }

                    Player_PlayAnimOnce(play, this,
                                        GET_PLAYER_ANIM(PLAYER_ANIMGROUP_ROLL_BONKING, this->modelAnimType));
                    this->linearVelocity = -this->linearVelocity;
                    func_808429B4(play, 33267, 3, 12);
                    Player_RequestRumble(this, 255, 20, 150, 0);
                    func_8002F7DC(&this->actor, NA_SE_PL_BODY_HIT);
                    Player_PlayVoiceSfxForAge(this, NA_SE_VO_LI_CLIMB_END);
                    this->genericTimer = 1;
                    gSaveContext.sohStats.count[COUNT_BONKS]++;
                    GameInteractor_ExecuteOnPlayerBonk();
                    return;
                }
            }

            if ((this->skelAnime.curFrame < 15.0f) || !Player_SetupStartMeleeWeaponAttack(this, play)) {
                if (this->skelAnime.curFrame >= 20.0f) {
                    if (CVarGetInteger("gSonicRoll", 0)) {
                        Player_SetupRolling(this, play);
                    } else {
                        Player_SetupReturnToStandStill(this, play);
                    }
                    return;
                }

                Player_GetTargetVelocityAndYaw(this, &targetVelocity, &targetYaw, 0.018f, play);

                targetVelocity *= 1.5f;
                if ((targetVelocity < 3.0f) || (this->relativeAnalogStickInputs[this->inputFrameCounter] != 0)) {
                    targetVelocity = 3.0f;
                }

                if (CVarGetInteger("gSonicRoll", 0)) {
                    targetVelocity *= 4.5f;
                    Player_SetRunVelocityAndYaw(this, targetVelocity, targetYaw);
                    if (this->linearVelocity < 30.0f) {
                        this->linearVelocity = 30.0f;
                    }
                }
                else {
                    Player_SetRunVelocityAndYaw(this, targetVelocity, this->actor.shape.rot.y);
                }

                if (Player_SetupSpawnDustAtFeet(play, this)) {
                    func_8002F8F0(&this->actor, NA_SE_PL_ROLL_DUST - SFX_FLAG);
                }

                Player_PlayAnimSfx(this, D_8085460C);
            }
        }
    }
}

void Player_FallingDive(Player* this, PlayState* play) {
    this->stateFlags2 |= PLAYER_STATE2_DISABLE_MOVE_ROTATION_WHILE_Z_TARGETING;

    if (LinkAnimation_Update(play, &this->skelAnime)) {
        Player_PlayAnimLoop(play, this, &gPlayerAnim_link_normal_run_jump_water_fall_wait);
    }

    Math_StepToF(&this->linearVelocity, 0.0f, 0.05f);

    if (this->actor.bgCheckFlags & 1) {
        this->actor.colChkInfo.damage = 0x10;
        Player_SetupDamage(play, this, 1, 4.0f, 5.0f, this->actor.shape.rot.y, 20);
    }
}

void Player_JumpSlash(Player* this, PlayState* play) {
    f32 sp2C;
    s16 sp2A;

    this->stateFlags2 |= PLAYER_STATE2_DISABLE_MOVE_ROTATION_WHILE_Z_TARGETING;

    this->actor.gravity = -1.2f;
    LinkAnimation_Update(play, &this->skelAnime);

    if (!func_80842DF4(play, this)) {
        func_8084285C(this, 6.0f, 7.0f, 99.0f);

        if (!(this->actor.bgCheckFlags & 1)) {
            Player_GetTargetVelocityAndYaw(this, &sp2C, &sp2A, 0.0f, play);
            func_8083DFE0(this, &sp2C, &this->currentYaw);
            return;
        }

        if (func_80843E64(play, this) >= 0) {
            this->meleeWeaponAnimation += 2;
            Player_StartMeleeWeaponAttack(play, this, this->meleeWeaponAnimation);
            this->slashCounter = 3;
            Player_PlayLandingSfx(this);
        }
    }
}

s32 func_80844BE4(Player* this, PlayState* play) {
    s32 temp;

    if (Player_SetupCutscene(play, this)) {
        this->stateFlags2 |= PLAYER_STATE2_RELEASING_SPIN_ATTACK;
    } else {
        if (!CHECK_BTN_ALL(sControlInput->cur.button, BTN_B)) {
            if ((this->unk_858 >= 0.85f) || Player_CanQuickspin(this)) {
                temp = D_80854384[Player_HoldsTwoHandedWeapon(this)];
            } else {
                temp = D_80854380[Player_HoldsTwoHandedWeapon(this)];
            }

            Player_StartMeleeWeaponAttack(play, this, temp);
            Player_SetupInvincibilityNoDamageFlash(this, -8);

            this->stateFlags2 |= PLAYER_STATE2_RELEASING_SPIN_ATTACK;
            if (this->relativeAnalogStickInputs[this->inputFrameCounter] == 0) {
                this->stateFlags2 |= PLAYER_STATE2_ENABLE_FORWARD_SLIDE_FROM_ATTACK;
            }
        } else {
            return 0;
        }
    }

    return 1;
}

void func_80844CF8(Player* this, PlayState* play) {
    Player_SetActionFunc(play, this, Player_WalkChargingSpinAttack, 1);
}

void func_80844D30(Player* this, PlayState* play) {
    Player_SetActionFunc(play, this, Player_SidewalkChargingSpinAttack, 1);
}

void func_80844D68(Player* this, PlayState* play) {
    Player_ReturnToStandStill(this, play);
    Player_InactivateMeleeWeapon(this);
    Player_ChangeAnimMorphToLastFrame(play, this, sCancelSpinAttackChargeAnims[Player_HoldsTwoHandedWeapon(this)]);
    this->currentYaw = this->actor.shape.rot.y;
}

void func_80844DC8(Player* this, PlayState* play) {
    Player_SetActionFunc(play, this, Player_ChargeSpinAttack, 1);
    this->walkFrame = 0.0f;
    Player_PlayAnimLoop(play, this, sSpinAttackChargeAnims[Player_HoldsTwoHandedWeapon(this)]);
    this->genericTimer = 1;
}

void func_80844E3C(Player* this) {
    Math_StepToF(&this->unk_858, 1.0f, 0.02f);
}

void Player_ChargeSpinAttack(Player* this, PlayState* play) {
    f32 sp34;
    s16 sp32;
    s32 temp;

    this->stateFlags1 |= PLAYER_STATE1_CHARGING_SPIN_ATTACK;
    this->stateFlags1 |= PLAYER_STATE1_CHARGING_SPIN_ATTACK;

    if (LinkAnimation_Update(play, &this->skelAnime)) {
        Player_EndAnimMovement(this);
        Player_SetZTargetFriendlyYaw(this);
        this->stateFlags1 &= ~PLAYER_STATE1_Z_TARGETING_FRIENDLY;
        Player_PlayAnimLoop(play, this, sSpinAttackChargeAnims[Player_HoldsTwoHandedWeapon(this)]);
        this->genericTimer = -1;
    }

    Player_StepLinearVelocityToZero(this);

    if (!func_80842964(this, play) && (this->genericTimer != 0)) {
        func_80844E3C(this);

        if (this->genericTimer < 0) {
            if (this->unk_858 >= 0.1f) {
                this->slashCounter = 0;
                this->genericTimer = 1;
            } else if (!CHECK_BTN_ALL(sControlInput->cur.button, BTN_B)) {
                func_80844D68(this, play);
            }
        } else if (!func_80844BE4(this, play)) {
            Player_GetTargetVelocityAndYaw(this, &sp34, &sp32, 0.0f, play);

            temp = func_80840058(this, &sp34, &sp32, play);
            if (temp > 0) {
                func_80844CF8(this, play);
            } else if (temp < 0) {
                func_80844D30(this, play);
            }
        }
    }
}

void Player_WalkChargingSpinAttack(Player* this, PlayState* play) {
    s16 temp1;
    s32 temp2;
    f32 sp5C;
    f32 sp58;
    f32 sp54;
    s16 sp52;
    s32 temp4;
    s16 temp5;
    s32 sp44;

    temp1 = this->currentYaw - this->actor.shape.rot.y;
    temp2 = ABS(temp1);

    sp5C = fabsf(this->linearVelocity);
    sp58 = sp5C * 1.5f;

    this->stateFlags1 |= PLAYER_STATE1_CHARGING_SPIN_ATTACK;

    if (sp58 < 1.5f) {
        sp58 = 1.5f;
    }

    sp58 = ((temp2 < 0x4000) ? -1.0f : 1.0f) * sp58;

    Player_SetupWalkSfx(this, sp58);

    sp58 = CLAMP(sp5C * 0.5f, 0.5f, 1.0f);

    LinkAnimation_BlendToJoint(play, &this->skelAnime, sSpinAttackChargeAnims[Player_HoldsTwoHandedWeapon(this)], 0.0f,
                               sSpinAttackChargeWalkAnims[Player_HoldsTwoHandedWeapon(this)], this->walkFrame * (21.0f / 29.0f), sp58,
                               this->blendTable);

    if (!func_80842964(this, play) && !func_80844BE4(this, play)) {
        func_80844E3C(this);
        Player_GetTargetVelocityAndYaw(this, &sp54, &sp52, 0.0f, play);

        temp4 = func_80840058(this, &sp54, &sp52, play);

        if (temp4 < 0) {
            func_80844D30(this, play);
            return;
        }

        if (temp4 == 0) {
            sp54 = 0.0f;
            sp52 = this->currentYaw;
        }

        temp5 = sp52 - this->currentYaw;
        sp44 = ABS(temp5);

        if (sp44 > 0x4000) {
            if (Math_StepToF(&this->linearVelocity, 0.0f, 1.0f)) {
                this->currentYaw = sp52;
            }
            return;
        }

        Math_AsymStepToF(&this->linearVelocity, sp54 * 0.2f, 1.0f, 0.5f);
        Math_ScaledStepToS(&this->currentYaw, sp52, sp44 * 0.1f);

        if ((sp54 == 0.0f) && (this->linearVelocity == 0.0f)) {
            func_80844DC8(this, play);
        }
    }
}

void Player_SidewalkChargingSpinAttack(Player* this, PlayState* play) {
    f32 sp5C;
    f32 sp58;
    f32 sp54;
    s16 sp52;
    s32 temp4;
    s16 temp5;
    s32 sp44;

    sp5C = fabsf(this->linearVelocity);

    this->stateFlags1 |= PLAYER_STATE1_CHARGING_SPIN_ATTACK;

    if (sp5C == 0.0f) {
        sp5C = ABS(this->unk_87C) * 0.0015f;
        if (sp5C < 400.0f) {
            sp5C = 0.0f;
        }
        Player_SetupWalkSfx(this, ((this->unk_87C >= 0) ? 1 : -1) * sp5C);
    } else {
        sp58 = sp5C * 1.5f;
        if (sp58 < 1.5f) {
            sp58 = 1.5f;
        }
        Player_SetupWalkSfx(this, sp58);
    }

    sp58 = CLAMP(sp5C * 0.5f, 0.5f, 1.0f);

    LinkAnimation_BlendToJoint(play, &this->skelAnime, sSpinAttackChargeAnims[Player_HoldsTwoHandedWeapon(this)], 0.0f,
                               sSpinAttackChargeSidewalkAnims[Player_HoldsTwoHandedWeapon(this)], this->walkFrame * (21.0f / 29.0f), sp58,
                               this->blendTable);

    if (!func_80842964(this, play) && !func_80844BE4(this, play)) {
        func_80844E3C(this);
        Player_GetTargetVelocityAndYaw(this, &sp54, &sp52, 0.0f, play);

        temp4 = func_80840058(this, &sp54, &sp52, play);

        if (temp4 > 0) {
            func_80844CF8(this, play);
            return;
        }

        if (temp4 == 0) {
            sp54 = 0.0f;
            sp52 = this->currentYaw;
        }

        temp5 = sp52 - this->currentYaw;
        sp44 = ABS(temp5);

        if (sp44 > 0x4000) {
            if (Math_StepToF(&this->linearVelocity, 0.0f, 1.0f)) {
                this->currentYaw = sp52;
            }
            return;
        }

        Math_AsymStepToF(&this->linearVelocity, sp54 * 0.2f, 1.0f, 0.5f);
        Math_ScaledStepToS(&this->currentYaw, sp52, sp44 * 0.1f);

        if ((sp54 == 0.0f) && (this->linearVelocity == 0.0f) && (sp5C == 0.0f)) {
            func_80844DC8(this, play);
        }
    }
}

void Player_JumpUpToLedge(Player* this, PlayState* play) {
    s32 sp3C;
    f32 temp1;
    s32 temp2;
    f32 temp3;

    this->stateFlags2 |= PLAYER_STATE2_DISABLE_MOVE_ROTATION_WHILE_Z_TARGETING;
    sp3C = LinkAnimation_Update(play, &this->skelAnime);

    if (this->skelAnime.animation == &gPlayerAnim_link_normal_250jump_start) {
        this->linearVelocity = 1.0f;

        if (LinkAnimation_OnFrame(&this->skelAnime, 8.0f)) {
            temp1 = this->wallHeight;

            if (temp1 > this->ageProperties->unk_0C) {
                temp1 = this->ageProperties->unk_0C;
            }

            if (this->stateFlags1 & PLAYER_STATE1_SWIMMING) {
                temp1 *= 0.085f;
            } else {
                temp1 *= 0.072f;
            }

            if (!LINK_IS_ADULT) {
                temp1 += 1.0f;
            }

            Player_SetupJumpWithSfx(this, NULL, temp1, play, NA_SE_VO_LI_AUTO_JUMP);
            this->genericTimer = -1;
            return;
        }
    } else {
        temp2 = Player_IsActionInterrupted(play, this, &this->skelAnime, 4.0f);

        if (temp2 == 0) {
            this->stateFlags1 &= ~(PLAYER_STATE1_CLIMBING_ONTO_LEDGE | PLAYER_STATE1_JUMPING);
            return;
        }

        if ((sp3C != 0) || (temp2 > 0)) {
            Player_SetupStandingStillNoMorph(this, play);
            this->stateFlags1 &= ~(PLAYER_STATE1_CLIMBING_ONTO_LEDGE | PLAYER_STATE1_JUMPING);
            return;
        }

        temp3 = 0.0f;

        if (this->skelAnime.animation == &gPlayerAnim_link_swimer_swim_15step_up) {
            if (LinkAnimation_OnFrame(&this->skelAnime, 30.0f)) {
                Player_StartJumpOutOfWater(play, this, 10.0f);
            }
            temp3 = 50.0f;
        } else if (this->skelAnime.animation == &gPlayerAnim_link_normal_150step_up) {
            temp3 = 30.0f;
        } else if (this->skelAnime.animation == &gPlayerAnim_link_normal_100step_up) {
            temp3 = 16.0f;
        }

        if (LinkAnimation_OnFrame(&this->skelAnime, temp3)) {
            Player_PlayLandingSfx(this);
            Player_PlayVoiceSfxForAge(this, NA_SE_VO_LI_CLIMB_END);
        }

        if ((this->skelAnime.animation == &gPlayerAnim_link_normal_100step_up) || (this->skelAnime.curFrame > 5.0f)) {
            if (this->genericTimer == 0) {
                Player_PlayJumpSfx(this);
                this->genericTimer = 1;
            }
            Math_StepToF(&this->actor.shape.yOffset, 0.0f, 150.0f);
        }
    }
}

void Player_RunMiniCutsceneFunc(Player* this, PlayState* play) {
    this->stateFlags2 |=
        PLAYER_STATE2_DISABLE_MOVE_ROTATION_WHILE_Z_TARGETING | PLAYER_STATE2_ALWAYS_DISABLE_MOVE_ROTATION;
    LinkAnimation_Update(play, &this->skelAnime);

    if (((this->stateFlags1 & PLAYER_STATE1_HOLDING_ACTOR) && (this->heldActor != NULL) &&
         (this->getItemId == GI_NONE)) ||
        !Player_SetupCurrentUpperAction(this, play)) {
        this->miniCsFunc(play, this);
    }
}

s32 func_80845964(PlayState* play, Player* this, CsCmdActorAction* arg2, f32 arg3, s16 arg4, s32 arg5) {
    if ((arg5 != 0) && (this->linearVelocity == 0.0f)) {
        return LinkAnimation_Update(play, &this->skelAnime);
    }

    if (arg5 != 2) {
        f32 sp34 = R_UPDATE_RATE * 0.5f;
        f32 selfDistX = arg2->endPos.x - this->actor.world.pos.x;
        f32 selfDistZ = arg2->endPos.z - this->actor.world.pos.z;
        f32 sp28 = sqrtf(SQ(selfDistX) + SQ(selfDistZ)) / sp34;
        s32 sp24 = (arg2->endFrame - play->csCtx.frames) + 1;

        arg4 = Math_Atan2S(selfDistZ, selfDistX);

        if (arg5 == 1) {
            f32 distX = arg2->endPos.x - arg2->startPos.x;
            f32 distZ = arg2->endPos.z - arg2->startPos.z;
            s32 temp = (((sqrtf(SQ(distX) + SQ(distZ)) / sp34) / (arg2->endFrame - arg2->startFrame)) / 1.5f) * 4.0f;

            if (temp >= sp24) {
                arg4 = this->actor.shape.rot.y;
                arg3 = 0.0f;
            } else {
                arg3 = sp28 / ((sp24 - temp) + 1);
            }
        } else {
            arg3 = sp28 / sp24;
        }
    }

    this->stateFlags2 |= PLAYER_STATE2_DISABLE_MOVE_ROTATION_WHILE_Z_TARGETING;
    func_80841EE4(this, play);
    Player_SetRunVelocityAndYaw(this, arg3, arg4);

    if ((arg3 == 0.0f) && (this->linearVelocity == 0.0f)) {
        Player_EndRun(this, play);
    }

    return 0;
}

s32 func_80845BA0(PlayState* play, Player* arg1, f32* arg2, s32 arg3) {
    f32 dx = arg1->csStartPos.x - arg1->actor.world.pos.x;
    f32 dz = arg1->csStartPos.z - arg1->actor.world.pos.z;
    s32 sp2C = sqrtf(SQ(dx) + SQ(dz));
    s16 yaw = Math_Vec3f_Yaw(&arg1->actor.world.pos, &arg1->csStartPos);

    if (sp2C < arg3) {
        *arg2 = 0.0f;
        yaw = arg1->actor.shape.rot.y;
    }

    if (func_80845964(play, arg1, NULL, *arg2, yaw, 2)) {
        return 0;
    }

    return sp2C;
}

s32 func_80845C68(PlayState* play, s32 arg1) {
    if (arg1 == 0) {
        Play_SetupRespawnPoint(play, RESPAWN_MODE_DOWN, 0xDFF);
    }
    gSaveContext.respawn[RESPAWN_MODE_DOWN].data = 0;
    return arg1;
}

void Player_MiniCsMovement(Player* this, PlayState* play) {
    f32 sp3C;
    s32 temp;
    f32 sp34;
    s32 sp30;
    s32 pad;

    if (!Player_SetupItemCutsceneOrFirstPerson(this, play)) {
        if (this->genericTimer == 0) {
            LinkAnimation_Update(play, &this->skelAnime);

            if (DECR(this->doorTimer) == 0) {
                this->linearVelocity = 0.1f;
                this->genericTimer = 1;
            }
        } else if (this->genericVar == 0) {
            sp3C = 5.0f * sWaterSpeedScale;

            if (func_80845BA0(play, this, &sp3C, -1) < 30) {
                this->genericVar = 1;
                this->stateFlags1 |= PLAYER_STATE1_IN_CUTSCENE;

                this->csStartPos.x = this->csEndPos.x;
                this->csStartPos.z = this->csEndPos.z;
            }
        } else {
            sp34 = 5.0f;
            sp30 = 20;

            if (this->stateFlags1 & PLAYER_STATE1_EXITING_SCENE) {
                sp34 = gSaveContext.entranceSpeed;

                if (sConveyorSpeedIndex != 0) {
                    this->csStartPos.x = (Math_SinS(sConveyorYaw) * 400.0f) + this->actor.world.pos.x;
                    this->csStartPos.z = (Math_CosS(sConveyorYaw) * 400.0f) + this->actor.world.pos.z;
                }
            } else if (this->genericTimer < 0) {
                this->genericTimer++;

                sp34 = gSaveContext.entranceSpeed;
                sp30 = -1;
            }

            temp = func_80845BA0(play, this, &sp34, sp30);

            if ((this->genericTimer == 0) ||
                ((temp == 0) && (this->linearVelocity == 0.0f) && (Play_GetCamera(play, 0)->unk_14C & 0x10))) {

                func_8005B1A4(Play_GetCamera(play, 0));
                func_80845C68(play, gSaveContext.respawn[RESPAWN_MODE_DOWN].data);

                if (!Player_SetupSpeakOrCheck(this, play)) {
                    Player_EndMiniCsMovement(this, play);
                }
            }
        }
    }

    if (this->stateFlags1 & PLAYER_STATE1_HOLDING_ACTOR) {
        Player_SetupCurrentUpperAction(this, play);
    }
}

void Player_OpenDoor(Player* this, PlayState* play) {
    s32 sp2C;

    this->stateFlags2 |= PLAYER_STATE2_DISABLE_MOVE_ROTATION_WHILE_Z_TARGETING;
    sp2C = LinkAnimation_Update(play, &this->skelAnime);

    Player_SetupCurrentUpperAction(this, play);

    if (sp2C) {
        if (this->genericTimer == 0) {
            if (DECR(this->doorTimer) == 0) {
                this->genericTimer = 1;
                this->skelAnime.endFrame = this->skelAnime.animLength - 1.0f;
            }
        } else {
            Player_SetupStandingStillNoMorph(this, play);
            if (play->roomCtx.prevRoom.num >= 0) {
                func_80097534(play, &play->roomCtx);
            }
            func_8005B1A4(Play_GetCamera(play, 0));
            Play_SetupRespawnPoint(play, 0, 0xDFF);
        }
        return;
    }

    if (!(this->stateFlags1 & PLAYER_STATE1_IN_CUTSCENE) && LinkAnimation_OnFrame(&this->skelAnime, 15.0f)) {
        play->func_11D54(this, play);
    }
}

void Player_LiftActor(Player* this, PlayState* play) {
    Player_StepLinearVelocityToZero(this);

    if (LinkAnimation_Update(play, &this->skelAnime)) {
        Player_SetupStandingStillType(this, play);
        Player_SetupHoldActorUpperAction(this, play);
        return;
    }

    if (LinkAnimation_OnFrame(&this->skelAnime, 4.0f)) {
        Actor* interactRangeActor = this->interactRangeActor;

        if (!Player_InterruptHoldingActor(play, this, interactRangeActor)) {
            this->heldActor = interactRangeActor;
            this->actor.child = interactRangeActor;
            interactRangeActor->parent = &this->actor;
            interactRangeActor->bgCheckFlags &= 0xFF00;
            this->leftHandRot.y = interactRangeActor->shape.rot.y - this->actor.shape.rot.y;
        }
        return;
    }

    Math_ScaledStepToS(&this->leftHandRot.y, 0, 4000);
}

static PlayerAnimSfxEntry D_8085461C[] = {
    { NA_SE_VO_LI_SWORD_L, 0x2031 },
    { NA_SE_VO_LI_SWORD_N, -0x20E6 },
};

void Player_ThrowStonePillar(Player* this, PlayState* play) {
    if (LinkAnimation_Update(play, &this->skelAnime) && (this->genericTimer++ > 20)) {
        if (!Player_SetupItemCutsceneOrFirstPerson(this, play)) {
            Player_SetupReturnToStandStillSetAnim(this, &gPlayerAnim_link_normal_heavy_carry_end, play);
        }
        return;
    }

    if (LinkAnimation_OnFrame(&this->skelAnime, 41.0f)) {
        BgHeavyBlock* heavyBlock = (BgHeavyBlock*)this->interactRangeActor;

        this->heldActor = &heavyBlock->dyna.actor;
        this->actor.child = &heavyBlock->dyna.actor;
        heavyBlock->dyna.actor.parent = &this->actor;
        func_8002DBD0(&heavyBlock->dyna.actor, &heavyBlock->unk_164, &this->leftHandPos);
        return;
    }

    if (LinkAnimation_OnFrame(&this->skelAnime, 229.0f)) {
        Actor* heldActor = this->heldActor;

        if (CVarGetInteger("gFasterHeavyBlockLift", 0)) {
            // This is the difference in rotation when the animation is sped up 5x
            heldActor->shape.rot.x -= 3510;
        }
        heldActor->speedXZ = Math_SinS(heldActor->shape.rot.x) * 40.0f;
        heldActor->velocity.y = Math_CosS(heldActor->shape.rot.x) * 40.0f;
        heldActor->gravity = -2.0f;
        heldActor->minVelocityY = -30.0f;
        Player_DetatchHeldActor(play, this);
        return;
    }

    Player_PlayAnimSfx(this, D_8085461C);
}

void Player_LiftSilverBoulder(Player* this, PlayState* play) {
    Player_StepLinearVelocityToZero(this);

    if (LinkAnimation_Update(play, &this->skelAnime)) {
        Player_PlayAnimLoop(play, this, &gPlayerAnim_link_silver_wait);
        this->genericTimer = 1;
        return;
    }

    u16 buttonsToCheck = BTN_A | BTN_B | BTN_CLEFT | BTN_CRIGHT | BTN_CDOWN;
    if (CVarGetInteger("gDpadEquips", 0) != 0) {
        buttonsToCheck |= BTN_DUP | BTN_DDOWN | BTN_DLEFT | BTN_DRIGHT;
    }
    if (this->genericTimer == 0) {
        if (LinkAnimation_OnFrame(&this->skelAnime, 27.0f)) {
            Actor* interactRangeActor = this->interactRangeActor;

            this->heldActor = interactRangeActor;
            this->actor.child = interactRangeActor;
            interactRangeActor->parent = &this->actor;
            return;
        }

        if (LinkAnimation_OnFrame(&this->skelAnime, 25.0f)) {
            Player_PlayVoiceSfxForAge(this, NA_SE_VO_LI_SWORD_L);
            return;
        }

    } else if (CHECK_BTN_ANY(sControlInput->press.button, buttonsToCheck)) {
        Player_SetActionFunc(play, this, Player_ThrowSilverBoulder, 1);
        Player_PlayAnimOnce(play, this, &gPlayerAnim_link_silver_throw);
    }
}

void Player_ThrowSilverBoulder(Player* this, PlayState* play) {
    if (LinkAnimation_Update(play, &this->skelAnime)) {
        Player_SetupStandingStillType(this, play);
        return;
    }

    if (LinkAnimation_OnFrame(&this->skelAnime, 6.0f)) {
        Actor* heldActor = this->heldActor;

        heldActor->world.rot.y = this->actor.shape.rot.y;
        heldActor->speedXZ = 10.0f;
        heldActor->velocity.y = 20.0f;
        Player_SetupHeldItemUpperActionFunc(play, this);
        func_8002F7DC(&this->actor, NA_SE_PL_THROW);
        Player_PlayVoiceSfxForAge(this, NA_SE_VO_LI_SWORD_N);
    }
}

void Player_FailToLiftActor(Player* this, PlayState* play) {
    if (LinkAnimation_Update(play, &this->skelAnime)) {
        Player_PlayAnimLoop(play, this, &gPlayerAnim_link_normal_nocarry_free_wait);
        this->genericTimer = 15;
        return;
    }

    if (this->genericTimer != 0) {
        this->genericTimer--;
        if (this->genericTimer == 0) {
            Player_SetupReturnToStandStillSetAnim(this, &gPlayerAnim_link_normal_nocarry_free_end, play);
            this->stateFlags1 &= ~PLAYER_STATE1_HOLDING_ACTOR;
            Player_PlayVoiceSfxForAge(this, NA_SE_VO_LI_DAMAGE_S);
        }
    }
}

void Player_SetupPutDownActor(Player* this, PlayState* play) {
    Player_StepLinearVelocityToZero(this);

    if (LinkAnimation_Update(play, &this->skelAnime)) {
        Player_SetupStandingStillType(this, play);
        return;
    }

    if (LinkAnimation_OnFrame(&this->skelAnime, 4.0f)) {
        Actor* heldActor = this->heldActor;

        if (!Player_InterruptHoldingActor(play, this, heldActor)) {
            heldActor->velocity.y = 0.0f;
            heldActor->speedXZ = 0.0f;
            Player_SetupHeldItemUpperActionFunc(play, this);
            if (heldActor->id == ACTOR_EN_BOM_CHU && !CVarGetInteger("gDisableFirstPersonChus", 0)) {
                Player_ForceFirstPerson(this, play);
            }
        }
    }
}

void Player_StartThrowActor(Player* this, PlayState* play) {
    f32 sp34;
    s16 sp32;

    Player_StepLinearVelocityToZero(this);

    if (LinkAnimation_Update(play, &this->skelAnime) ||
        ((this->skelAnime.curFrame >= 8.0f) && Player_GetTargetVelocityAndYaw(this, &sp34, &sp32, 0.018f, play))) {
        Player_SetupStandingStillType(this, play);
        return;
    }

    if (LinkAnimation_OnFrame(&this->skelAnime, 3.0f)) {
        func_8084409C(play, this, this->linearVelocity + 8.0f, 12.0f);
    }
}

static ColliderCylinderInit D_80854624 = {
    {
        COLTYPE_HIT5,
        AT_NONE,
        AC_ON | AC_TYPE_ENEMY,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_PLAYER,
        COLSHAPE_CYLINDER,
    },
    {
        ELEMTYPE_UNK1,
        { 0x00000000, 0x00, 0x00 },
        { 0xFFCFFFFF, 0x00, 0x00 },
        TOUCH_NONE,
        BUMP_ON,
        OCELEM_ON,
    },
    { 12, 60, 0, { 0, 0, 0 } },
};

static ColliderQuadInit D_80854650 = {
    {
        COLTYPE_NONE,
        AT_ON | AT_TYPE_PLAYER,
        AC_NONE,
        OC1_NONE,
        OC2_TYPE_PLAYER,
        COLSHAPE_QUAD,
    },
    {
        ELEMTYPE_UNK2,
        { 0x00000100, 0x00, 0x01 },
        { 0xFFCFFFFF, 0x00, 0x00 },
        TOUCH_ON | TOUCH_SFX_NORMAL,
        BUMP_NONE,
        OCELEM_NONE,
    },
    { { { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } } },
};

static ColliderQuadInit D_808546A0 = {
    {
        COLTYPE_METAL,
        AT_ON | AT_TYPE_PLAYER,
        AC_ON | AC_HARD | AC_TYPE_ENEMY,
        OC1_NONE,
        OC2_TYPE_PLAYER,
        COLSHAPE_QUAD,
    },
    {
        ELEMTYPE_UNK2,
        { 0x00100000, 0x00, 0x00 },
        { 0xDFCFFFFF, 0x00, 0x00 },
        TOUCH_ON | TOUCH_SFX_NORMAL,
        BUMP_ON,
        OCELEM_NONE,
    },
    { { { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } } },
};

void func_8084663C(Actor* thisx, PlayState* play) {
}

void Player_SpawnNoUpdateOrDraw(PlayState* play, Player* this) {
    this->actor.update = func_8084663C;
    this->actor.draw = NULL;
}

void Player_SetupSpawnFromBlueWarp(PlayState* play, Player* this) {
    Player_SetActionFunc(play, this, Player_SpawnFromBlueWarp, 0);
    if ((play->sceneNum == SCENE_SPOT06) && (gSaveContext.sceneSetupIndex >= 4)) {
        this->genericVar = 1;
    }
    this->stateFlags1 |= PLAYER_STATE1_IN_CUTSCENE;
    LinkAnimation_Change(play, &this->skelAnime, &gPlayerAnim_link_okarina_warp_goal, 2.0f / 3.0f, 0.0f, 24.0f,
                         ANIMMODE_ONCE, 0.0f);
    this->actor.world.pos.y += 800.0f;
}

static u8 D_808546F0[] = { ITEM_SWORD_MASTER, ITEM_SWORD_KOKIRI };

void func_80846720(PlayState* play, Player* this, s32 arg2) {
    s32 item = D_808546F0[(void)0, gSaveContext.linkAge];
    s32 actionParam = sItemActionParams[item];

    Player_PutAwayHookshot(this);
    Player_DetatchHeldActor(play, this);

    this->heldItemId = item;
    this->nextModelGroup = Player_ActionToModelGroup(this, actionParam);

    Player_ChangeItem(play, this, actionParam);
    Player_SetupHeldItemUpperActionFunc(play, this);

    if (arg2 != 0) {
        func_8002F7DC(&this->actor, NA_SE_IT_SWORD_PICKOUT);
    }
}

static Vec3f D_808546F4 = { -1.0f, 69.0f, 20.0f };

void Player_SpawnFromTimeTravel(PlayState* play, Player* this) {
    Player_SetActionFunc(play, this, Player_EndTimeTravel, 0);
    this->stateFlags1 |= PLAYER_STATE1_IN_CUTSCENE;
    Math_Vec3f_Copy(&this->actor.world.pos, &D_808546F4);
    this->currentYaw = this->actor.shape.rot.y = -0x8000;
    LinkAnimation_Change(play, &this->skelAnime, this->ageProperties->unk_A0, 2.0f / 3.0f, 0.0f, 0.0f,
                         ANIMMODE_ONCE, 0.0f);
    Player_SetupAnimMovement(play, this, 0x28F);
    if (LINK_IS_ADULT) {
        func_80846720(play, this, 0);
    }
    this->genericTimer = 20;
}

void Player_SpawnOpeningDoor(PlayState* play, Player* this) {
    Player_SetActionFunc(play, this, Player_SetupOpenDoorFromSpawn, 0);
    Player_SetupAnimMovement(play, this, 0x9B);
}

void Player_SpawnExitingGrotto(PlayState* play, Player* this) {
    Player_SetupJump(this, &gPlayerAnim_link_normal_jump, 12.0f, play);
    Player_SetActionFunc(play, this, Player_JumpFromGrotto, 0);
    this->stateFlags1 |= PLAYER_STATE1_IN_CUTSCENE;
    this->fallStartHeight = this->actor.world.pos.y;
    OnePointCutscene_Init(play, 5110, 40, &this->actor, MAIN_CAM);
}

void Player_SpawnWithKnockback(PlayState* play, Player* this) {
    Player_SetupDamage(play, this, 1, 2.0f, 2.0f, this->actor.shape.rot.y + 0x8000, 0);
}

void Player_SetupSpawnFromWarpSong(PlayState* play, Player* this) {
    Player_SetActionFunc(play, this, Player_SpawnFromWarpSong, 0);
    this->actor.draw = NULL;
    this->stateFlags1 |= PLAYER_STATE1_IN_CUTSCENE;
}

static s16 D_80854700[] = { ACTOR_MAGIC_WIND, ACTOR_MAGIC_DARK, ACTOR_MAGIC_FIRE };

Actor* func_80846A00(PlayState* play, Player* this, s32 arg2) {
    return Actor_Spawn(&play->actorCtx, play, D_80854700[arg2], this->actor.world.pos.x,
                       this->actor.world.pos.y, this->actor.world.pos.z, 0, 0, 0, 0, true);
}

void Player_SetupSpawnFromFaroresWind(PlayState* play, Player* this) {
    this->actor.draw = NULL;
    Player_SetActionFunc(play, this, Player_SpawnFromFaroresWind, 0);
    this->stateFlags1 |= PLAYER_STATE1_IN_CUTSCENE;
}

static InitChainEntry sInitChain[] = {
    ICHAIN_F32(targetArrowOffset, 500, ICHAIN_STOP),
};

static EffectBlureInit2 blureSword = {
    0, 8, 0, { 255, 255, 255, 255 }, { 255, 255, 255, 64 }, { 255, 255, 255, 0 }, { 255, 255, 255, 0 }, 4,
    0, 2, 0, { 255, 255, 255, 255 }, { 255, 255, 255, 64 }, 1,
};

static Vec3s D_80854730 = { -57, 3377, 0 };

void Player_InitCommon(Player* this, PlayState* play, FlexSkeletonHeader* skelHeader) {
    this->getItemEntry = (GetItemEntry)GET_ITEM_NONE;
    this->ageProperties = &sAgeProperties[gSaveContext.linkAge];
    Actor_ProcessInitChain(&this->actor, sInitChain);
    this->meleeWeaponEffectIndex = TOTAL_EFFECT_COUNT;
    this->currentYaw = this->actor.world.rot.y;
    Player_SetupHeldItemUpperActionFunc(play, this);

    SkelAnime_InitLink(play, &this->skelAnime, skelHeader,
                       GET_PLAYER_ANIM(PLAYER_ANIMGROUP_STANDING_STILL, this->modelAnimType), 9, this->jointTable,
                       this->morphTable, PLAYER_LIMB_MAX);
    this->skelAnime.baseTransl = D_80854730;
    SkelAnime_InitLink(play, &this->skelAnimeUpper, skelHeader, Player_GetStandingStillAnim(this), 9,
                       this->jointTableUpper, this->morphTableUpper, PLAYER_LIMB_MAX);
    this->skelAnimeUpper.baseTransl = D_80854730;

    Effect_Add(play, &this->meleeWeaponEffectIndex, EFFECT_BLURE2, 0, 0, &blureSword);
    ActorShape_Init(&this->actor.shape, 0.0f, ActorShadow_DrawFeet, this->ageProperties->unk_04);
    this->subCamId = SUBCAM_NONE;
    Collider_InitCylinder(play, &this->cylinder);
    Collider_SetCylinder(play, &this->cylinder, &this->actor, &D_80854624);
    Collider_InitQuad(play, &this->meleeWeaponQuads[0]);
    Collider_SetQuad(play, &this->meleeWeaponQuads[0], &this->actor, &D_80854650);
    Collider_InitQuad(play, &this->meleeWeaponQuads[1]);
    Collider_SetQuad(play, &this->meleeWeaponQuads[1], &this->actor, &D_80854650);
    Collider_InitQuad(play, &this->shieldQuad);
    Collider_SetQuad(play, &this->shieldQuad, &this->actor, &D_808546A0);

    this->ivanDamageMultiplier = 1;
}

static void (*D_80854738[])(PlayState* play, Player* this) = {
    Player_SpawnNoUpdateOrDraw,       Player_SpawnFromTimeTravel,
    Player_SetupSpawnFromBlueWarp,    Player_SpawnOpeningDoor,
    Player_SpawnExitingGrotto,        Player_SetupSpawnFromWarpSong,
    Player_SetupSpawnFromFaroresWind, Player_SpawnWithKnockback,
    Player_SpawnWalkingSlow,          Player_SpawnWalkingSlow,
    Player_SpawnWalkingSlow,          Player_SpawnWalkingSlow,
    Player_SpawnWalkingSlow,          Player_SpawnNoMomentum,
    Player_SpawnWalkingSlow,          Player_SpawnWalkingPreserveMomentum,
};

static Vec3f D_80854778 = { 0.0f, 50.0f, 0.0f };


// Chaos rando actor related
#define CUCCO_ATK_NUM_MAX 32

EnAttackNiw* attackCucco[CUCCO_ATK_NUM_MAX] = { NULL };
static Actor* explodeRupee = NULL;
static EnCow* cow[5] = { NULL, NULL, NULL, NULL, NULL };
static EnLight* ritualFlame = NULL;
static EnEncount2* fireRockSpawner = NULL;
static BgSpot15Saku* jail[4] = { NULL, NULL, NULL, NULL };
static EnAObj* jailFloor = NULL;

static u8 cowRitual = false;
static u8 fireRocksFalling = false;

static s16 cuccoAtkTimer = 0;
static s16 cuccoAtkNum = 0;

void Player_Init(Actor* thisx, PlayState* play2) {
    Player* this = (Player*)thisx;
    PlayState* play = play2;
    SceneTableEntry* scene = play->loadedScene;
    u32 titleFileSize;
    s32 initMode;
    s32 sp50;
    s32 sp4C;
    s16 i;

    // In ER, once Link has spawned we know the scene has loaded, so we can sanitize the last known entrance type
    if (gSaveContext.n64ddFlag && Randomizer_GetSettingValue(RSK_SHUFFLE_ENTRANCES)) {
        Grotto_SanitizeEntranceType();
    }

    play->shootingGalleryStatus = play->bombchuBowlingStatus = 0;

    play->playerInit = Player_InitCommon;
    play->playerUpdate = Player_UpdateCommon;
    play->isPlayerDroppingFish = Player_IsDroppingFish;
    play->startPlayerFishing = Player_StartFishing;
    play->grabPlayer = Player_SetupRestrainedByEnemy;
    play->startPlayerCutscene = Player_SetupPlayerCutscene;
    play->func_11D54 = Player_SetupStandingStillMorph;
    play->damagePlayer = Player_InflictDamage;
    play->talkWithPlayer = Player_StartTalkingWithActor;

    thisx->room = -1;
    this->ageProperties = &sAgeProperties[gSaveContext.linkAge];
    this->itemAction = this->heldItemAction = -1;
    this->heldItemId = ITEM_NONE;

    // Make chaos rando actor ptrs null to try to prevent use-after-free issues
    for (i = 0; i++; i < 5) {
        cow[i] = NULL;
    }
    for (i = 0; i++; i < CUCCO_ATK_NUM_MAX) {
        attackCucco[i] = NULL;
    }
    ritualFlame = NULL;
    fireRockSpawner = NULL;
    explodeRupee = NULL;

    // Set chaos event vars to false or zero to prevent crashing on scene change
    cowRitual = false;
    fireRocksFalling = false;

    cuccoAtkTimer = 0;
    cuccoAtkNum = 0;

    Player_UseItem(play, this, ITEM_NONE);
    Player_SetEquipmentData(play, this);
    this->prevBoots = this->currentBoots;
    if (CVarGetInteger("gMMBunnyHood", BUNNY_HOOD_VANILLA) != BUNNY_HOOD_VANILLA) {
        if (INV_CONTENT(ITEM_TRADE_CHILD) == ITEM_SOLD_OUT) {
            sMaskMemory = PLAYER_MASK_NONE;
        }
        this->currentMask = sMaskMemory;
        for (uint16_t cSlotIndex = 0; cSlotIndex < ARRAY_COUNT(gSaveContext.equips.cButtonSlots); cSlotIndex++) {
            if (gSaveContext.equips.cButtonSlots[cSlotIndex] == SLOT_TRADE_CHILD &&
                (gItemAgeReqs[gSaveContext.equips.buttonItems[cSlotIndex + 1]] != 9 && LINK_IS_ADULT &&
                 !CVarGetInteger("gTimelessEquipment", 0))) {
                gSaveContext.equips.cButtonSlots[cSlotIndex] = SLOT_NONE;
                gSaveContext.equips.buttonItems[cSlotIndex + 1] = ITEM_NONE;
            }
        }
    }
    Player_InitCommon(this, play, gPlayerSkelHeaders[((void)0, gSaveContext.linkAge)]);
    this->giObjectSegment = (void*)(((uintptr_t)ZELDA_ARENA_MALLOC_DEBUG(0x3008) + 8) & ~0xF);

    sp50 = gSaveContext.respawnFlag;

    if (sp50 != 0) {
        if (sp50 == -3) {
            thisx->params = gSaveContext.respawn[RESPAWN_MODE_RETURN].playerParams;
        } else {
            if ((sp50 == 1) || (sp50 == -1)) {
                this->voidRespawnCounter = -2;
            }

            if (sp50 < 0) {
                sp4C = 0;
            } else {
                sp4C = sp50 - 1;
                Math_Vec3f_Copy(&thisx->world.pos, &gSaveContext.respawn[sp50 - 1].pos);
                Math_Vec3f_Copy(&thisx->home.pos, &thisx->world.pos);
                Math_Vec3f_Copy(&thisx->prevPos, &thisx->world.pos);
                this->fallStartHeight = thisx->world.pos.y;
                this->currentYaw = thisx->shape.rot.y = gSaveContext.respawn[sp4C].yaw;
                thisx->params = gSaveContext.respawn[sp4C].playerParams;
            }

            play->actorCtx.flags.tempSwch = gSaveContext.respawn[sp4C].tempSwchFlags & 0xFFFFFF;
            play->actorCtx.flags.tempCollect = gSaveContext.respawn[sp4C].tempCollectFlags;
        }
    }

    if ((sp50 == 0) || (sp50 < -1)) {
        titleFileSize = scene->titleFile.vromEnd - scene->titleFile.vromStart;
        if (gSaveContext.showTitleCard) {
            if ((gSaveContext.sceneSetupIndex < 4) &&
                (gEntranceTable[((void)0, gSaveContext.entranceIndex) + ((void)0, gSaveContext.sceneSetupIndex)].field &
                 0x4000) &&
                ((play->sceneNum != SCENE_DDAN) || (gSaveContext.eventChkInf[11] & 1)) &&
                ((play->sceneNum != SCENE_NIGHT_SHOP) || (gSaveContext.eventChkInf[2] & 0x20))) {
                TitleCard_InitPlaceName(play, &play->actorCtx.titleCtx, this->giObjectSegment, 160, 120, 144,
                                        24, 20);
            }
        }
        gSaveContext.showTitleCard = true;
    }

    if (func_80845C68(play, (sp50 == 2) ? 1 : 0) == 0) {
        gSaveContext.respawn[RESPAWN_MODE_DOWN].playerParams = (thisx->params & 0xFF) | 0xD00;
    }

    gSaveContext.respawn[RESPAWN_MODE_DOWN].data = 1;

    if (play->sceneNum <= SCENE_GANONTIKA_SONOGO) {
        gSaveContext.infTable[26] |= gBitFlags[play->sceneNum];
    }

    initMode = (thisx->params & 0xF00) >> 8;
    if ((initMode == 5) || (initMode == 6)) {
        if (gSaveContext.cutsceneIndex >= 0xFFF0) {
            initMode = 13;
        }
    }

    D_80854738[initMode](play, this);

    if (initMode != 0) {
        if ((gSaveContext.gameMode == 0) || (gSaveContext.gameMode == 3)) {
            this->naviActor = Player_SpawnFairy(play, this, &thisx->world.pos, &D_80854778, FAIRY_NAVI);
            if (gSaveContext.dogParams != 0) {
                gSaveContext.dogParams |= 0x8000;
            }
        }
    }

    if (gSaveContext.nayrusLoveTimer != 0) {
        gSaveContext.magicState = 3;
        func_80846A00(play, this, 1);
        this->stateFlags3 &= ~PLAYER_STATE3_RESTORE_NAYRUS_LOVE;
    }

    if (gSaveContext.entranceSound != 0) {
        Audio_PlayActorSound2(&this->actor, ((void)0, gSaveContext.entranceSound));
        gSaveContext.entranceSound = 0;
    }

    Map_SavePlayerInitialInfo(play);
    MREG(64) = 0;
}

void func_808471F4(s16* pValue) {
    s16 step;

    step = (ABS(*pValue) * 100.0f) / 1000.0f;
    step = CLAMP(step, 400, 4000);

    Math_ScaledStepToS(pValue, 0, step);
}

void func_80847298(Player* this) {
    s16 sp26;

    if (!(this->lookFlags & 2)) {
        sp26 = this->actor.focus.rot.y - this->actor.shape.rot.y;
        func_808471F4(&sp26);
        this->actor.focus.rot.y = this->actor.shape.rot.y + sp26;
    }

    if (!(this->lookFlags & 1)) {
        func_808471F4(&this->actor.focus.rot.x);
    }

    if (!(this->lookFlags & 8)) {
        func_808471F4(&this->headRot.x);
    }

    if (!(this->lookFlags & 0x40)) {
        func_808471F4(&this->upperBodyRot.x);
    }

    if (!(this->lookFlags & 4)) {
        func_808471F4(&this->actor.focus.rot.z);
    }

    if (!(this->lookFlags & 0x10)) {
        func_808471F4(&this->headRot.y);
    }

    if (!(this->lookFlags & 0x20)) {
        func_808471F4(&this->headRot.z);
    }

    if (!(this->lookFlags & 0x80)) {
        if (this->upperBodyYawOffset != 0) {
            func_808471F4(&this->upperBodyYawOffset);
        } else {
            func_808471F4(&this->upperBodyRot.y);
        }
    }

    if (!(this->lookFlags & 0x100)) {
        func_808471F4(&this->upperBodyRot.z);
    }

    this->lookFlags = 0;
}

static f32 D_80854784[] = { 120.0f, 240.0f, 360.0f };

static u8 sDiveDoActions[] = { DO_ACTION_1, DO_ACTION_2, DO_ACTION_3, DO_ACTION_4,
                               DO_ACTION_5, DO_ACTION_6, DO_ACTION_7, DO_ACTION_8 };

void func_808473D4(PlayState* play, Player* this) {
    if ((Message_GetState(&play->msgCtx) == TEXT_STATE_NONE) && (this->actor.category == ACTORCAT_PLAYER)) {
        Actor* heldActor = this->heldActor;
        Actor* interactRangeActor = this->interactRangeActor;
        s32 sp24;
        s32 sp20 = this->relativeAnalogStickInputs[this->inputFrameCounter];
        s32 sp1C = Player_IsSwimming(this);
        s32 doAction = DO_ACTION_NONE;

        if (!Player_InBlockingCsMode(play, this)) {
            if (this->stateFlags1 & PLAYER_STATE1_IN_FIRST_PERSON_MODE) {
                doAction = DO_ACTION_RETURN;
            } else if ((this->heldItemAction == PLAYER_IA_FISHING_POLE) && (this->fpsItemType != 0)) {
                if (this->fpsItemType == 2) {
                    doAction = DO_ACTION_REEL;
                }
            } else if ((Player_PlayOcarina != this->actionFunc) && !(this->stateFlags2 & PLAYER_STATE2_INSIDE_CRAWLSPACE)) {
                if ((this->doorType != PLAYER_DOORTYPE_NONE) &&
                    (!(this->stateFlags1 & PLAYER_STATE1_HOLDING_ACTOR) ||
                     ((heldActor != NULL) && (heldActor->id == ACTOR_EN_RU1)))) {
                    doAction = DO_ACTION_OPEN;
                } else if ((!(this->stateFlags1 & PLAYER_STATE1_HOLDING_ACTOR) || (heldActor == NULL)) &&
                           (interactRangeActor != NULL) &&
                           ((!sp1C && (this->getItemId == GI_NONE)) ||
                            (this->getItemId < 0 && !(this->stateFlags1 & PLAYER_STATE1_SWIMMING)))) {
                    if (this->getItemId < 0) {
                        doAction = DO_ACTION_OPEN;
                    } else if ((interactRangeActor->id == ACTOR_BG_TOKI_SWD) && LINK_IS_ADULT) {
                        doAction = DO_ACTION_DROP;
                    } else {
                        doAction = DO_ACTION_GRAB;
                    }
                } else if (!sp1C && (this->stateFlags2 & PLAYER_STATE2_CAN_GRAB_PUSH_PULL_WALL)) {
                    doAction = DO_ACTION_GRAB;
                } else if ((this->stateFlags2 & PLAYER_STATE2_CAN_CLIMB_PUSH_PULL_WALL) ||
                           (!(this->stateFlags1 & PLAYER_STATE1_RIDING_HORSE) && (this->rideActor != NULL))) {
                    doAction = DO_ACTION_CLIMB;
                } else if ((this->stateFlags1 & PLAYER_STATE1_RIDING_HORSE) && !EN_HORSE_CHECK_4((EnHorse*)this->rideActor) &&
                           (Player_DismountHorse != this->actionFunc)) {
                    if ((this->stateFlags2 & PLAYER_STATE2_CAN_SPEAK_OR_CHECK) && (this->talkActor != NULL)) {
                        if (this->talkActor->category == ACTORCAT_NPC) {
                            doAction = DO_ACTION_SPEAK;
                        } else {
                            doAction = DO_ACTION_CHECK;
                        }
                    } else if (!Actor_PlayerIsAimingReadyFpsItem(this) && !(this->stateFlags1 & PLAYER_STATE1_IN_FIRST_PERSON_MODE)) {
                        doAction = DO_ACTION_FASTER;
                    }
                } else if ((this->stateFlags2 & PLAYER_STATE2_CAN_SPEAK_OR_CHECK) && (this->talkActor != NULL)) {
                    if (this->talkActor->category == ACTORCAT_NPC) {
                        doAction = DO_ACTION_SPEAK;
                    } else {
                        doAction = DO_ACTION_CHECK;
                    }
                } else if ((this->stateFlags1 & (PLAYER_STATE1_HANGING_FROM_LEDGE_SLIP | PLAYER_STATE1_CLIMBING)) ||
                           ((this->stateFlags1 & PLAYER_STATE1_RIDING_HORSE) &&
                           (this->stateFlags2 & PLAYER_STATE2_CAN_DISMOUNT_HORSE))) {
                    doAction = DO_ACTION_DOWN;
                } else if (this->stateFlags2 & PLAYER_STATE2_CAN_ENTER_CRAWLSPACE) {
                    doAction = DO_ACTION_ENTER;
                } else if ((this->stateFlags1 & PLAYER_STATE1_HOLDING_ACTOR) && (this->getItemId == GI_NONE) &&
                           (heldActor != NULL)) {
                    if ((this->actor.bgCheckFlags & 1) || (heldActor->id == ACTOR_EN_NIW)) {
                        if (func_8083EAF0(this, heldActor) == 0) {
                            doAction = DO_ACTION_DROP;
                        } else {
                            doAction = DO_ACTION_THROW;
                        }
                    }
                } else if (!(this->stateFlags1 & PLAYER_STATE1_SWIMMING) && Player_CanHoldActor(this) &&
                           (this->getItemId < GI_MAX)) {
                    doAction = DO_ACTION_GRAB;
                } else if (this->stateFlags2 & PLAYER_STATE2_ENABLE_DIVE_CAMERA_AND_TIMER) {
                    sp24 = (D_80854784[CUR_UPG_VALUE(UPG_SCALE)] - this->actor.yDistToWater) / 40.0f;
                    sp24 = CLAMP(sp24, 0, 7);
                    doAction = sDiveDoActions[sp24];
                } else if (sp1C && !(this->stateFlags2 & PLAYER_STATE2_DIVING)) {
                    doAction = DO_ACTION_DIVE;
                } else if (!sp1C && (!(this->stateFlags1 & PLAYER_STATE1_SHIELDING) || Player_IsZTargeting(this) ||
                                     !Player_IsChildWithHylianShield(this))) {
                    if ((!(this->stateFlags1 & PLAYER_STATE1_CLIMBING_ONTO_LEDGE) && (sp20 <= 0) &&
                         (Player_IsUnfriendlyZTargeting(this) ||
                          ((sFloorSpecialProperty != 7) && (Player_IsFriendlyZTargeting(this) ||
                            ((play->roomCtx.curRoom.behaviorType1 != ROOM_BEHAVIOR_TYPE1_2) &&
                            !(this->stateFlags1 & PLAYER_STATE1_SHIELDING) && (sp20 == 0))))))) {
                        doAction = DO_ACTION_ATTACK;
                    } else if ((play->roomCtx.curRoom.behaviorType1 != ROOM_BEHAVIOR_TYPE1_2) &&
                               Player_IsZTargeting(this) && (sp20 > 0)) {
                        doAction = DO_ACTION_JUMP;
                    } else if ((this->heldItemAction >= PLAYER_IA_SWORD_MASTER) ||
                               ((this->stateFlags2 & PLAYER_STATE2_NAVI_IS_ACTIVE) &&
                                (play->actorCtx.targetCtx.arrowPointedActor == NULL))) {
                        doAction = DO_ACTION_PUTAWAY;
                    }
                }
            }
        }

        if (doAction != DO_ACTION_PUTAWAY) {
            this->putAwayTimer = 20;
        } else if (this->putAwayTimer != 0) {
            if (CVarGetInteger("gInstantPutaway", 0) != 0) {
                this->putAwayTimer = 0;
            } else {
                doAction = DO_ACTION_NONE;
                this->putAwayTimer--;
            }
        }

        Interface_SetDoAction(play, doAction);

        if (this->stateFlags2 & PLAYER_STATE2_NAVI_REQUESTING_TALK) {
            if (this->targetActor != NULL) {
                Interface_SetNaviCall(play, 0x1E);
            } else {
                Interface_SetNaviCall(play, 0x1D);
            }
            Interface_SetNaviCall(play, 0x1E);
        } else {
            Interface_SetNaviCall(play, 0x1F);
        }
    }
}

s32 func_80847A78(Player* this) {
    s32 cond;

    if ((this->currentBoots == PLAYER_BOOTS_HOVER ||
         (CVarGetInteger("gIvanCoopModeEnabled", 0) && this->ivanFloating)) &&
        (this->hoverBootsTimer != 0)) {
        this->hoverBootsTimer--;
    } else {
        this->hoverBootsTimer = 0;
    }

    cond = (this->currentBoots == PLAYER_BOOTS_HOVER ||
            (CVarGetInteger("gIvanCoopModeEnabled", 0) && this->ivanFloating)) &&
           ((this->actor.yDistToWater >= 0.0f) || (Player_GetHurtFloorType(sFloorSpecialProperty) >= 0) ||
            Player_IsFloorSinkingSand(sFloorSpecialProperty));

    if (cond && (this->actor.bgCheckFlags & 1) && (this->hoverBootsTimer != 0)) {
        this->actor.bgCheckFlags &= ~1;
    }

    if (this->actor.bgCheckFlags & 1) {
        if (!cond) {
            this->hoverBootsTimer = 19;
        }
        return 0;
    }

    sFloorSpecialProperty = 0;
    this->angleToFloorX = this->angleToFloorY = sAngleToFloorX = 0;

    return 1;
}

static Vec3f D_80854798 = { 0.0f, 18.0f, 0.0f };

void func_80847BA0(PlayState* play, Player* this) {
    u8 spC7 = 0;
    CollisionPoly* spC0;
    Vec3f spB4;
    f32 spB0;
    f32 spAC;
    f32 spA8;
    u32 spA4;

    sFloorProperty = this->floorProperty;

    if (this->stateFlags2 & PLAYER_STATE2_INSIDE_CRAWLSPACE) {
        spB0 = 10.0f;
        spAC = 15.0f;
        spA8 = 30.0f;
    } else {
        spB0 = this->ageProperties->unk_38;
        spAC = 26.0f;
        spA8 = this->ageProperties->unk_00;
    }

    if (this->stateFlags1 & (PLAYER_STATE1_IN_CUTSCENE | PLAYER_STATE1_FALLING_INTO_GROTTO_OR_VOID)) {
        if (this->stateFlags1 & PLAYER_STATE1_FALLING_INTO_GROTTO_OR_VOID) {
            this->actor.bgCheckFlags &= ~1;
            spA4 = 0x38;
        } else if ((this->stateFlags1 & PLAYER_STATE1_EXITING_SCENE) &&
                   ((this->sceneExitPosY - (s32)this->actor.world.pos.y) >= 100)) {
            spA4 = 0x39;
        } else if (!(this->stateFlags1 & PLAYER_STATE1_EXITING_SCENE) &&
                   ((Player_OpenDoor == this->actionFunc) || (Player_MiniCsMovement == this->actionFunc))) {
            this->actor.bgCheckFlags &= ~0x208;
            spA4 = 0x3C;
        } else {
            spA4 = 0x3F;
        }
    } else {
        spA4 = 0x3F;
    }

    if (this->stateFlags3 & PLAYER_STATE3_IGNORE_CEILING_FLOOR_AND_WATER) {
        spA4 &= ~6;
    }

    if (spA4 & 4) {
        this->stateFlags3 |= PLAYER_STATE3_CHECKING_FLOOR_AND_WATER_COLLISION;
    }

    Math_Vec3f_Copy(&spB4, &this->actor.world.pos);
    Actor_UpdateBgCheckInfo(play, &this->actor, spAC, spB0, spA8, spA4);

    if (this->actor.bgCheckFlags & 0x10) {
        this->actor.velocity.y = 0.0f;
    }

    sPlayerYDistToFloor = this->actor.world.pos.y - this->actor.floorHeight;
    sConveyorSpeedIndex = 0;

    spC0 = this->actor.floorPoly;

    if (spC0 != NULL) {
        this->floorProperty = func_80041EA4(&play->colCtx, spC0, this->actor.floorBgId);
        this->prevSurfaceMaterial = this->surfaceMaterial;

        if (this->actor.bgCheckFlags & 0x20) {
            if (this->actor.yDistToWater < 20.0f) {
                this->surfaceMaterial = 4;
            } else {
                this->surfaceMaterial = 5;
            }
        } else {
            if (this->stateFlags2 & PLAYER_STATE2_SPAWN_DUST_AT_FEET) {
                this->surfaceMaterial = 1;
            } else {
                this->surfaceMaterial = SurfaceType_GetSfx(&play->colCtx, spC0, this->actor.floorBgId);
            }
        }

        if (this->actor.category == ACTORCAT_PLAYER) {
            Audio_SetCodeReverb(SurfaceType_GetEcho(&play->colCtx, spC0, this->actor.floorBgId));

            if (this->actor.floorBgId == BGCHECK_SCENE) {
                func_80074CE8(play,
                              SurfaceType_GetLightSettingIndex(&play->colCtx, spC0, this->actor.floorBgId));
            } else {
                func_80043508(&play->colCtx, this->actor.floorBgId);
            }
        }

        sConveyorSpeedIndex = SurfaceType_GetConveyorSpeed(&play->colCtx, spC0, this->actor.floorBgId);
        if (sConveyorSpeedIndex != 0) {
            sIsFloorConveyor = SurfaceType_IsConveyor(&play->colCtx, spC0, this->actor.floorBgId);
            if (((sIsFloorConveyor == 0) && (this->actor.yDistToWater > 20.0f) &&
                 (this->currentBoots != PLAYER_BOOTS_IRON)) ||
                ((sIsFloorConveyor != 0) && (this->actor.bgCheckFlags & 1))) {
                sConveyorYaw = SurfaceType_GetConveyorDirection(&play->colCtx, spC0, this->actor.floorBgId) << 10;
            } else {
                sConveyorSpeedIndex = 0;
            }
        }
    }

    Player_SetupExit(play, this, spC0, this->actor.floorBgId);

    this->actor.bgCheckFlags &= ~0x200;

    if (this->actor.bgCheckFlags & 8) {
        CollisionPoly* spA0;
        s32 sp9C;
        s16 sp9A;
        s32 pad;

        D_80854798.y = 18.0f;
        D_80854798.z = this->ageProperties->unk_38 + 10.0f;

        if (!(this->stateFlags2 & PLAYER_STATE2_INSIDE_CRAWLSPACE) &&
            Player_WallLineTestWithOffset(play, this, &D_80854798, &spA0, &sp9C, &sWallIntersectPos)) {
            this->actor.bgCheckFlags |= 0x200;
            if (this->actor.wallPoly != spA0) {
                this->actor.wallPoly = spA0;
                this->actor.wallBgId = sp9C;
                this->actor.wallYaw = Math_Atan2S(spA0->normal.z, spA0->normal.x);
            }
        }

        sp9A = this->actor.shape.rot.y - (s16)(this->actor.wallYaw + 0x8000);

        sTouchedWallFlags = func_80041DB8(&play->colCtx, this->actor.wallPoly, this->actor.wallBgId);

        sYawToTouchedWall = ABS(sp9A);

        sp9A = this->currentYaw - (s16)(this->actor.wallYaw + 0x8000);

        sYawToTouchedWall2 = ABS(sp9A);

        spB0 = sYawToTouchedWall2 * 0.00008f;
        if (!(this->actor.bgCheckFlags & 1) || spB0 >= 1.0f) {
            this->speedLimit = R_RUN_SPEED_LIMIT / 100.0f;
        } else {
            spAC = (R_RUN_SPEED_LIMIT / 100.0f * spB0);
            this->speedLimit = spAC;
            if (spAC < 0.1f) {
                this->speedLimit = 0.1f;
            }
        }

        if ((this->actor.bgCheckFlags & 0x200) && (sYawToTouchedWall < 0x3000)) {
            CollisionPoly* wallPoly = this->actor.wallPoly;

            if ((ABS(wallPoly->normal.y) < 600) || (CVarGetInteger("gClimbEverything", 0) != 0)) {
                f32 sp8C = COLPOLY_GET_NORMAL(wallPoly->normal.x);
                f32 sp88 = COLPOLY_GET_NORMAL(wallPoly->normal.y);
                f32 sp84 = COLPOLY_GET_NORMAL(wallPoly->normal.z);
                f32 wallHeight;
                CollisionPoly* sp7C;
                CollisionPoly* sp78;
                s32 sp74;
                Vec3f sp68;
                f32 sp64;
                f32 sp60;
                s32 temp3;

                this->wallDistance = Math3D_UDistPlaneToPos(sp8C, sp88, sp84, wallPoly->dist, &this->actor.world.pos);

                spB0 = this->wallDistance + 10.0f;
                sp68.x = this->actor.world.pos.x - (spB0 * sp8C);
                sp68.z = this->actor.world.pos.z - (spB0 * sp84);
                sp68.y = this->actor.world.pos.y + this->ageProperties->unk_0C;

                sp64 = BgCheck_EntityRaycastFloor1(&play->colCtx, &sp7C, &sp68);
                wallHeight = sp64 - this->actor.world.pos.y;
                this->wallHeight = wallHeight;

                if ((this->wallHeight < 18.0f) ||
                    BgCheck_EntityCheckCeiling(&play->colCtx, &sp60, &this->actor.world.pos,
                                               (sp64 - this->actor.world.pos.y) + 20.0f, &sp78, &sp74, &this->actor)) {
                    this->wallHeight = 399.96002f;
                } else {
                    D_80854798.y = (sp64 + 5.0f) - this->actor.world.pos.y;

                    if (Player_WallLineTestWithOffset(play, this, &D_80854798, &sp78, &sp74, &sWallIntersectPos) &&
                        (temp3 = this->actor.wallYaw - Math_Atan2S(sp78->normal.z, sp78->normal.x),
                         ABS(temp3) < 0x4000) &&
                        !func_80041E18(&play->colCtx, sp78, sp74)) {
                        this->wallHeight = 399.96002f;
                    } else if (func_80041DE4(&play->colCtx, wallPoly, this->actor.wallBgId) == 0) {
                        if (this->ageProperties->unk_1C <= this->wallHeight) {
                            if (ABS(sp7C->normal.y) > 28000) {
                                if (this->ageProperties->unk_14 <= this->wallHeight) {
                                    spC7 = 4;
                                } else if (this->ageProperties->unk_18 <= this->wallHeight) {
                                    spC7 = 3;
                                } else {
                                    spC7 = 2;
                                }
                            }
                        } else {
                            spC7 = 1;
                        }
                    }
                }
            }
        }
    } else {
        this->speedLimit = R_RUN_SPEED_LIMIT / 100.0f;
        this->wallTouchTimer = 0;
        this->wallHeight = 0.0f;
    }

    if (spC7 == this->touchedWallJumpType) {
        if ((this->linearVelocity != 0.0f) && (this->wallTouchTimer < 100)) {
            this->wallTouchTimer++;
        }
    } else {
        this->touchedWallJumpType = spC7;
        this->wallTouchTimer = 0;
    }

    if (this->actor.bgCheckFlags & 1) {
        sFloorSpecialProperty = func_80041D4C(&play->colCtx, spC0, this->actor.floorBgId);

        if (!func_80847A78(this)) {
            f32 sp58;
            f32 sp54;
            f32 sp50;
            f32 sp4C;
            s32 pad2;
            f32 sp44;
            s32 pad3;

            if (this->actor.floorBgId != BGCHECK_SCENE) {
                func_800434C8(&play->colCtx, this->actor.floorBgId);
            }

            sp58 = COLPOLY_GET_NORMAL(spC0->normal.x);
            sp54 = 1.0f / COLPOLY_GET_NORMAL(spC0->normal.y);
            sp50 = COLPOLY_GET_NORMAL(spC0->normal.z);

            sp4C = Math_SinS(this->currentYaw);
            sp44 = Math_CosS(this->currentYaw);

            this->angleToFloorX = Math_Atan2S(1.0f, (-(sp58 * sp4C) - (sp50 * sp44)) * sp54);
            this->angleToFloorY = Math_Atan2S(1.0f, (-(sp58 * sp44) - (sp50 * sp4C)) * sp54);

            sp4C = Math_SinS(this->actor.shape.rot.y);
            sp44 = Math_CosS(this->actor.shape.rot.y);

            sAngleToFloorX = Math_Atan2S(1.0f, (-(sp58 * sp4C) - (sp50 * sp44)) * sp54);

            func_8083E318(play, this, spC0);
        }
    } else {
        func_80847A78(this);
    }

    if (this->prevFloorSpecialProperty == sFloorSpecialProperty) {
        this->hurtFloorTimer++;
    } else {
        this->prevFloorSpecialProperty = sFloorSpecialProperty;
        this->hurtFloorTimer = 0;
    }
}

void Player_UpdateCamAndSeqModes(PlayState* play, Player* this) {
    u8 seqMode;
    s32 pad;
    Actor* targetActor;
    s32 camMode;

    if (this->actor.category == ACTORCAT_PLAYER) {
        seqMode = SEQ_MODE_DEFAULT;

        if (this->csMode != 0) {
            Camera_ChangeMode(Play_GetCamera(play, 0), CAM_MODE_NORMAL);
        } else if (!(this->stateFlags1 & PLAYER_STATE1_IN_FIRST_PERSON_MODE)) {
            if ((this->actor.parent != NULL) && (this->stateFlags3 & PLAYER_STATE3_MOVING_ALONG_HOOKSHOT_PATH)) {
                camMode = CAM_MODE_HOOKSHOT;
                Camera_SetParam(Play_GetCamera(play, 0), 8, this->actor.parent);
            } else if (Player_StartKnockback == this->actionFunc) {
                camMode = CAM_MODE_STILL;
            } else if (this->stateFlags2 & PLAYER_STATE2_ENABLE_PUSH_PULL_CAM) {
                camMode = CAM_MODE_PUSHPULL;
            } else if ((targetActor = this->targetActor) != NULL) {
                if (CHECK_FLAG_ALL(this->actor.flags, ACTOR_FLAG_PLAYER_TALKED_TO)) {
                    camMode = CAM_MODE_TALK;
                } else if (this->stateFlags1 & PLAYER_STATE1_FORCE_STRAFING) {
                    if (this->stateFlags1 & PLAYER_STATE1_AWAITING_THROWN_BOOMERANG) {
                        camMode = CAM_MODE_FOLLOWBOOMERANG;
                    } else {
                        camMode = CAM_MODE_FOLLOWTARGET;
                    }
                } else {
                    camMode = CAM_MODE_BATTLE;
                }
                Camera_SetParam(Play_GetCamera(play, 0), 8, targetActor);
            } else if (this->stateFlags1 & PLAYER_STATE1_CHARGING_SPIN_ATTACK) {
                camMode = CAM_MODE_CHARGE;
            } else if (this->stateFlags1 & PLAYER_STATE1_AWAITING_THROWN_BOOMERANG) {
                camMode = CAM_MODE_FOLLOWBOOMERANG;
                Camera_SetParam(Play_GetCamera(play, 0), 8, this->boomerangActor);
            } else if (this->stateFlags1 & (PLAYER_STATE1_HANGING_FROM_LEDGE_SLIP | PLAYER_STATE1_CLIMBING_ONTO_LEDGE)) {
                if (Player_IsFriendlyZTargeting(this)) {
                    camMode = CAM_MODE_HANGZ;
                } else {
                    camMode = CAM_MODE_HANG;
                }
            } else if (this->stateFlags1 & (PLAYER_STATE1_Z_TARGETING_FRIENDLY | PLAYER_STATE1_30)) {
                if ((Actor_PlayerIsAimingReadyFpsItem(this) || Player_IsAimingReadyBoomerang(this)) && !CVarGetInteger("gDisableFPSView", 0)) {
                    camMode = CAM_MODE_BOWARROWZ;
                } else if (this->stateFlags1 & PLAYER_STATE1_CLIMBING) {
                    camMode = CAM_MODE_CLIMBZ;
                } else {
                    camMode = CAM_MODE_TARGET;
                }
            } else if (this->stateFlags1 & (PLAYER_STATE1_JUMPING | PLAYER_STATE1_CLIMBING)) {
                if ((Player_JumpUpToLedge == this->actionFunc) || (this->stateFlags1 & PLAYER_STATE1_CLIMBING)) {
                    camMode = CAM_MODE_CLIMB;
                } else {
                    camMode = CAM_MODE_JUMP;
                }
            } else if (this->stateFlags1 & PLAYER_STATE1_FREEFALLING) {
                camMode = CAM_MODE_FREEFALL;
            } else if ((this->swordState != 0) && (this->meleeWeaponAnimation >= 0) && (this->meleeWeaponAnimation < 0x18)) {
                camMode = CAM_MODE_STILL;
            } else {
                camMode = CAM_MODE_NORMAL;
                if ((this->linearVelocity == 0.0f) &&
                    (!(this->stateFlags1 & PLAYER_STATE1_RIDING_HORSE) || (this->rideActor->speedXZ == 0.0f))) {
                    // not moving
                    seqMode = SEQ_MODE_STILL;
                }
            }

            Camera_ChangeMode(Play_GetCamera(play, 0), camMode);
        } else {
            // First person mode
            seqMode = SEQ_MODE_STILL;
        }

        if (play->actorCtx.targetCtx.bgmEnemy != NULL && !CVarGetInteger("gEnemyBGMDisable", 0)) {
            seqMode = SEQ_MODE_ENEMY;
            Audio_SetBgmEnemyVolume(sqrtf(play->actorCtx.targetCtx.bgmEnemy->xyzDistToPlayerSq));
        }

        if (play->sceneNum != SCENE_TURIBORI) {
            Audio_SetSequenceMode(seqMode);
        }
    }
}

static Vec3f D_808547A4 = { 0.0f, 0.5f, 0.0f };
static Vec3f D_808547B0 = { 0.0f, 0.5f, 0.0f };

static Color_RGBA8 D_808547BC = { 255, 255, 100, 255 };
static Color_RGBA8 D_808547C0 = { 255, 50, 0, 0 };

void func_80848A04(PlayState* play, Player* this) {
    f32 temp;

    if (this->unk_85C == 0.0f) {
        Player_UseItem(play, this, 0xFF);
        return;
    }

    temp = 1.0f;
    if (DECR(this->fpsItemType) == 0) {
        Inventory_ChangeAmmo(ITEM_STICK, -1);
        this->fpsItemType = 1;
        temp = 0.0f;
        this->unk_85C = temp;
    } else if (this->fpsItemType > 200) {
        temp = (210 - this->fpsItemType) / 10.0f;
    } else if (this->fpsItemType < 20) {
        temp = this->fpsItemType / 20.0f;
        this->unk_85C = temp;
    }

    func_8002836C(play, &this->meleeWeaponInfo[0].tip, &D_808547A4, &D_808547B0, &D_808547BC, &D_808547C0, temp * 200.0f,
                  0, 8);
}

void func_80848B44(PlayState* play, Player* this) {
    Vec3f shockPos;
    Vec3f* randBodyPart;
    s32 shockScale;

    this->shockTimer--;
    this->unk_892 += this->shockTimer;

    if (this->unk_892 > 20) {
        shockScale = this->shockTimer * 2;
        this->unk_892 -= 20;

        if (shockScale > 40) {
            shockScale = 40;
        }

        randBodyPart = this->bodyPartsPos + (s32)Rand_ZeroFloat(ARRAY_COUNT(this->bodyPartsPos) - 0.1f);
        shockPos.x = (Rand_CenteredFloat(5.0f) + randBodyPart->x) - this->actor.world.pos.x;
        shockPos.y = (Rand_CenteredFloat(5.0f) + randBodyPart->y) - this->actor.world.pos.y;
        shockPos.z = (Rand_CenteredFloat(5.0f) + randBodyPart->z) - this->actor.world.pos.z;

        EffectSsFhgFlash_SpawnShock(play, &this->actor, &shockPos, shockScale, FHGFLASH_SHOCK_PLAYER);
        func_8002F8F0(&this->actor, NA_SE_PL_SPARK - SFX_FLAG);
    }
}

void func_80848C74(PlayState* play, Player* this) {
    s32 spawnedFlame;
    u8* timerPtr;
    s32 timerStep;
    f32 flameScale;
    f32 flameIntensity;
    s32 dmgCooldown;
    s32 i;
    s32 sp58;
    s32 sp54;

    if (this->currentTunic == PLAYER_TUNIC_GORON || CVarGetInteger("gSuperTunic", 0) != 0) {
        sp54 = 20;
    } else {
        sp54 = (s32)(this->linearVelocity * 0.4f) + 1;
    }

    spawnedFlame = false;
    timerPtr = this->flameTimers;

    if (this->stateFlags2 & PLAYER_STATE2_MAKING_REACTABLE_NOISE) {
        sp58 = 100;
    } else {
        sp58 = 0;
    }

    Player_BurnDekuShield(this, play);

    for (i = 0; i < PLAYER_BODYPART_MAX; i++, timerPtr++) {
        timerStep = sp58 + sp54;

        if (*timerPtr <= timerStep) {
            *timerPtr = 0;
        } else {
            spawnedFlame = true;
            *timerPtr -= timerStep;

            if (*timerPtr > 20.0f) {
                flameIntensity = (*timerPtr - 20.0f) * 0.01f;
                flameScale = CLAMP(flameIntensity, 0.19999999f, 0.2f);
            } else {
                flameScale = *timerPtr * 0.01f;
            }

            flameIntensity = (*timerPtr - 25.0f) * 0.02f;
            flameIntensity = CLAMP(flameIntensity, 0.0f, 1.0f);
            EffectSsFireTail_SpawnFlameOnPlayer(play, flameScale, i, flameIntensity);
        }
    }

    if (spawnedFlame) {
        func_8002F7DC(&this->actor, NA_SE_EV_TORCH - SFX_FLAG);

        if (play->sceneNum == SCENE_JYASINBOSS) {
            dmgCooldown = 0;
        } else {
            dmgCooldown = 7;
        }

        if ((dmgCooldown & play->gameplayFrames) == 0) {
            Player_InflictDamage(play, -1);
        }
    } else {
        this->isBurning = false;
    }
}

void func_80848EF8(Player* this, PlayState* play) {
    if (CHECK_QUEST_ITEM(QUEST_STONE_OF_AGONY)) {
        f32 temp = 200000.0f - (this->stoneOfAgonyActorDistSq * 5.0f);

        if (temp < 0.0f) {
            temp = 0.0f;
        }

        this->stoneOfAgonyRumbleTimer += temp;

        /*Prevent it on horse, while jumping and on title screen.
        If you fly around no stone of agony for you! */
        Color_RGB8 stoneOfAgonyColor = { 255, 255, 255 };
        if (CVarGetInteger("gCosmetics.Hud_StoneOfAgony.Changed", 0)) {
            stoneOfAgonyColor = CVarGetColor24("gCosmetics.Hud_StoneOfAgony.Value", stoneOfAgonyColor);
        }
        if (CVarGetInteger("gVisualAgony", 0) != 0 && !this->stateFlags1) {
            s16 Top_Margins = (CVarGetInteger("gHUDMargin_T", 0) * -1);
            s16 Left_Margins = CVarGetInteger("gHUDMargin_L", 0);
            s16 Right_Margins = CVarGetInteger("gHUDMargin_R", 0);
            s16 X_Margins_VSOA;
            s16 Y_Margins_VSOA;
            if (CVarGetInteger("gVSOAUseMargins", 0) != 0) {
                if (CVarGetInteger("gVSOAPosType", 0) == 0) {
                    X_Margins_VSOA = Left_Margins;
                };
                Y_Margins_VSOA = Top_Margins;
            } else {
                X_Margins_VSOA = 0;
                Y_Margins_VSOA = 0;
            }
            s16 PosX_VSOA_ori = OTRGetRectDimensionFromLeftEdge(26) + X_Margins_VSOA;
            s16 PosY_VSOA_ori = 60 + Y_Margins_VSOA;
            s16 PosX_VSOA;
            s16 PosY_VSOA;
            if (CVarGetInteger("gVSOAPosType", 0) != 0) {
                PosY_VSOA = CVarGetInteger("gVSOAPosY", 0) + Y_Margins_VSOA;
                if (CVarGetInteger("gVSOAPosType", 0) == 1) { // Anchor Left
                    if (CVarGetInteger("gVSOAUseMargins", 0) != 0) {
                        X_Margins_VSOA = Left_Margins;
                    };
                    PosX_VSOA = OTRGetDimensionFromLeftEdge(CVarGetInteger("gVSOAPosX", 0) + X_Margins_VSOA);
                } else if (CVarGetInteger("gVSOAPosType", 0) == 2) { // Anchor Right
                    if (CVarGetInteger("gVSOAUseMargins", 0) != 0) {
                        X_Margins_VSOA = Right_Margins;
                    };
                    PosX_VSOA = OTRGetDimensionFromRightEdge(CVarGetInteger("gVSOAPosX", 0) + X_Margins_VSOA);
                } else if (CVarGetInteger("gVSOAPosType", 0) == 3) { // Anchor None
                    PosX_VSOA = CVarGetInteger("gVSOAPosX", 0);
                } else if (CVarGetInteger("gVSOAPosType", 0) == 4) { // Hidden
                    PosX_VSOA = -9999;
                }
            } else {
                PosY_VSOA = PosY_VSOA_ori;
                PosX_VSOA = PosX_VSOA_ori;
            }

            int rectLeft = PosX_VSOA; // Left X Pos
            int rectTop = PosY_VSOA;  // Top Y Pos
            int rectWidth = 24;       // Texture Width
            int rectHeight = 24;      // Texture Heigh
            int DefaultIconA = 50;    // Default icon alpha (55 on 255)

            OPEN_DISPS(play->state.gfxCtx);
            gDPPipeSync(OVERLAY_DISP++);

            gDPSetPrimColor(OVERLAY_DISP++, 0, 0, stoneOfAgonyColor.r, stoneOfAgonyColor.g, stoneOfAgonyColor.b, DefaultIconA);

            gDPSetCombineLERP(OVERLAY_DISP++, PRIMITIVE, ENVIRONMENT, TEXEL0, ENVIRONMENT, TEXEL0, 0, PRIMITIVE, 0,
                              PRIMITIVE, ENVIRONMENT, TEXEL0, ENVIRONMENT, TEXEL0, 0, PRIMITIVE, 0);
            if (this->stoneOfAgonyRumbleTimer > 4000000.0f) {
                gDPSetPrimColor(OVERLAY_DISP++, 0, 0, stoneOfAgonyColor.r, stoneOfAgonyColor.g, stoneOfAgonyColor.b, 255);
            } else {
                gDPSetPrimColor(OVERLAY_DISP++, 0, 0, stoneOfAgonyColor.r, stoneOfAgonyColor.g, stoneOfAgonyColor.b, DefaultIconA);
            }
            if (temp == 0 || temp <= 0.1f) {
                /*Fail check, it is used to draw off the icon when
                link is standing out range but do not refresh stoneOfAgonyRumbleTimer.
                Also used to make a default value in my case.*/
                gDPSetPrimColor(OVERLAY_DISP++, 0, 0, stoneOfAgonyColor.r, stoneOfAgonyColor.g, stoneOfAgonyColor.b, DefaultIconA);
            }
            gDPSetEnvColor(OVERLAY_DISP++, 0, 0, 0, 255);
            gDPSetOtherMode(OVERLAY_DISP++,
                            G_AD_DISABLE | G_CD_DISABLE | G_CK_NONE | G_TC_FILT | G_TF_POINT | G_TT_IA16 | G_TL_TILE |
                                G_TD_CLAMP | G_TP_NONE | G_CYC_1CYCLE | G_PM_NPRIMITIVE,
                            G_AC_NONE | G_ZS_PRIM | G_RM_XLU_SURF | G_RM_XLU_SURF2);
            gDPLoadTextureBlock(OVERLAY_DISP++, gStoneOfAgonyIconTex, G_IM_FMT_RGBA, G_IM_SIZ_32b, 24, 24, 0,
                                G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK,
                                G_TX_NOLOD, G_TX_NOLOD);
            gDPSetOtherMode(OVERLAY_DISP++,
                            G_AD_DISABLE | G_CD_DISABLE | G_CK_NONE | G_TC_FILT | G_TF_BILERP | G_TT_IA16 | G_TL_TILE |
                                G_TD_CLAMP | G_TP_NONE | G_CYC_1CYCLE | G_PM_NPRIMITIVE,
                            G_AC_NONE | G_ZS_PRIM | G_RM_XLU_SURF | G_RM_XLU_SURF2);
            gSPWideTextureRectangle(OVERLAY_DISP++, rectLeft << 2, rectTop << 2, (rectLeft + rectWidth) << 2,
                                    (rectTop + rectHeight) << 2, G_TX_RENDERTILE, 0, 0, 1 << 10, 1 << 10);
            CLOSE_DISPS(play->state.gfxCtx);
        }

        if (this->stoneOfAgonyRumbleTimer > 4000000.0f) {
            this->stoneOfAgonyRumbleTimer = 0.0f;
            if (CVarGetInteger("gVisualAgony", 0) != 0 && !this->stateFlags1) {
                // This audio is placed here and not in previous CVar check to prevent ears ra.. :)
                Audio_PlaySoundGeneral(NA_SE_SY_MESSAGE_WOMAN, &D_801333D4, 4, &D_801333E0, &D_801333E0, &D_801333E0);
            }
            Player_RequestRumble(this, 120, 20, 10, 0);
        }
    }
}

static s8 D_808547C4[] = {
    0,  3,  3,  5,   4,   8,   9,   13, 14, 15, 16, 17, 18, -22, 23, 24, 25,  26, 27,  28,  29, 31, 32, 33, 34, -35,
    30, 36, 38, -39, -40, -41, 42,  43, 45, 46, 0,  0,  0,  67,  48, 47, -50, 51, -52, -53, 54, 55, 56, 57, 58, 59,
    60, 61, 62, 63,  64,  -65, -66, 68, 11, 69, 70, 71, 8,  8,   72, 73, 78,  79, 80,  89,  90, 91, 92, 77, 19, 94,
};

static Vec3f D_80854814 = { 0.0f, 0.0f, 200.0f };

static f32 D_80854820[] = { 2.0f, 4.0f, 7.0f };
static f32 D_8085482C[] = { 0.5f, 1.0f, 3.0f };

void Player_SetupSwim(PlayState* play, Player* this, s16 yaw);

void Player_SpawnExplosion(PlayState* play, Player* this) {
    EnBom* bomb = (EnBom*)Actor_Spawn(&play->actorCtx, play, ACTOR_EN_BOM, this->actor.world.pos.x,
                               this->actor.world.pos.y, this->actor.world.pos.z, 0, 0, 6, BOMB_BODY, false);
    if (bomb != NULL) {
        bomb->timer = 0;
    }

    Player_Damage(play, this, -16);
    Player_SetupDamage(play, this, PLAYER_DMGREACTION_KNOCKBACK, 0.0f, 0.0f, 0, 20);
}

#define SET_NEXT_GAMESTATE(curState, newInit, newStruct) \
    do {                                                 \
        (curState)->init = newInit;                      \
        (curState)->size = sizeof(newStruct);            \
    } while (0)

void Player_UpdateCommon(Player* this, PlayState* play, Input* input) {
    s32 pad;

    if (CVarGetInteger("gForceUnequip", 0) || (CVarGetInteger("gChaosRedeem", 0) && Rand_ZeroOne() < 0.02f)) {
        Player_UnequipItem(play, this);
        for (s32 i = 1; i < ARRAY_COUNT(gSaveContext.equips.buttonItems); i++) {
            gSaveContext.equips.buttonItems[i] = ITEM_NONE;
        }
        CVarSetInteger("gForceUnequip", 0);
    }

    if (CVarGetInteger("gShuffleItems", 0) || (CVarGetInteger("gChaosRedeem", 0) && Rand_ZeroOne() < 0.02f)) {
        u8 items[3] = { ITEM_NONE };
        u8 originalItems[3] = { ITEM_NONE };
        u8 unshuffledItems =  0;
        s32 i;

        for (i = 0; i < 3; i++) {
            originalItems[i] = gSaveContext.equips.buttonItems[i + 1];
        }

        for (i = 0; i < 3; i++) {
            items[i] = gSaveContext.equips.buttonItems[i + 1];
        }
        for (i = ARRAY_COUNT(items) - 1; i > 0; i--) {
            // Pick a random index from 0 to i
            int j = Rand_Next() % (i + 1);

            int temp = items[i];
            items[i] = items[j];
            items[j] = temp;
        }
        for (i = 0; i < 3; i++) {
            gSaveContext.equips.buttonItems[i + 1] = items[i];
            if (items[i] == originalItems[i]) {
                unshuffledItems++;
            }
        }
        
        Player_UnequipItem(play, this);
        // Never allow the items to remain in the exact same order!
        if (unshuffledItems < 3) {
            CVarSetInteger("gShuffleItems", 0);
        }
    }

    if ((CVarGetInteger("gRandoMagic", 0) || (CVarGetInteger("gChaosRedeem", 0) && Rand_ZeroOne() < 0.02f)) && gSaveContext.magicLevel > 0) {
        // Randomize magic based on max
        if (gSaveContext.isDoubleMagicAcquired) {
            gSaveContext.magic = 0x60 * Rand_ZeroOne();
        } else {
            gSaveContext.magic = 0x30 * Rand_ZeroOne();
        }
        CVarSetInteger("gRandoMagic", 0);
    }

    if (CVarGetInteger("gSunsSong", 0)) {
        switch (play->sceneNum) {
            case SCENE_YDAN:
            case SCENE_YDAN_BOSS:
            case SCENE_DDAN:
            case SCENE_DDAN_BOSS:
            case SCENE_BDAN:
            case SCENE_BDAN_BOSS:
            case SCENE_BMORI1:
            case SCENE_MORIBOSSROOM:
            case SCENE_HIDAN:
            case SCENE_FIRE_BS:
            case SCENE_MIZUSIN:
            case SCENE_MIZUSIN_BS:
            case SCENE_JYASINZOU:
            case SCENE_JYASINBOSS:
            case SCENE_HAKADAN:
            case SCENE_HAKADAN_BS:
            case SCENE_HAKADANCH:
            case SCENE_ICE_DOUKUTO:
            case SCENE_GANON:
            case SCENE_GANON_BOSS:
            case SCENE_GANON_FINAL:
            case SCENE_MEN:
            case SCENE_GERUDOWAY:
            case SCENE_GANONTIKA:
            case SCENE_GANON_SONOGO:
            case SCENE_GANON_DEMO:
            case SCENE_GANONTIKA_SONOGO:
            case SCENE_TAKARAYA:
                CVarSetInteger("gSunsSong", 0);
                break;
            
            default:
                gSaveContext.sunsSongState = SUNSSONG_START;
                CVarSetInteger("gSunsSong", 0);
                break;
        }
    }

    static s16 redoTimer = 0;
    static s16 titleTimer = 0;
    
    #define FLASH_TIME 120
    static s16 flashTimer = FLASH_TIME;

    if (CVarGetInteger("gFlashbang", 0) || (CVarGetInteger("gChaosRedeem", 0) && Rand_ZeroOne() < 0.02f)) {
        if (flashTimer == FLASH_TIME) {
            Audio_PlayActorSound2(&this->actor, NA_SE_IT_EXPLOSION_LIGHT);
            play->envCtx.fillScreen = true;
            play->envCtx.screenFillColor[0] = 255;
            play->envCtx.screenFillColor[1] = 255;
            play->envCtx.screenFillColor[2] = 255;
            play->envCtx.screenFillColor[3] = 0;
        }
        if (flashTimer > (FLASH_TIME / 2)) {
            play->envCtx.screenFillColor[3] = CLAMP_MAX(play->envCtx.screenFillColor[3] + 15, 220);
        }
        DECR(flashTimer);
        // Reset flashbang if timer reaches zero, OR 1% chance to do another flashbang
        if (flashTimer == 0 || Rand_ZeroOne() < 0.01f) {
            flashTimer = FLASH_TIME;
        }
    } else {
        play->envCtx.fillScreen = false;
        play->envCtx.screenFillColor[3] = 0;
        flashTimer = FLASH_TIME;
    }

    if (CVarGetInteger("gRedoRando", 0)) {
        if (redoTimer == 0) {
            // Play ReDead scream SFX
            Audio_PlaySoundGeneral(NA_SE_EN_REDEAD_AIM, &D_801333D4, 4, &D_801333E0, &D_801333E0, &D_801333E8);
            // Freeze player in place and disable inputs
            this->stateFlags2 |= PLAYER_STATE2_PAUSE_MOST_UPDATING;
        } else if (redoTimer == 25) {
            // Erase all three save files
            Save_DeleteFile(0);
            Save_DeleteFile(1);
            Save_DeleteFile(2);
            // Reset screen color
            D_801614B0.r = 0;
            D_801614B0.g = 0;
            D_801614B0.b = 0;
            D_801614B0.a = 0;
            // Reset the game
            redoTimer = 0;
            play->state.running = false;
            CVarSetInteger("gRedoRando", 0);
            this->stateFlags2 &= ~PLAYER_STATE2_PAUSE_MOST_UPDATING;
            SET_NEXT_GAMESTATE(&play->state, FileChoose_Init, FileChooseContext);
            return;
        } else if (redoTimer < 25) {
            // Turn screen red
            if (D_801614B0.r < 255) {
                D_801614B0.r += 15;
            }
            D_801614B0.g = 0;
            D_801614B0.b = 0;
            D_801614B0.a = 255;
        }
        redoTimer++;
    }
    else {
        redoTimer = 0;
    }

    if (CVarGetInteger("gBackToHome", 0)) {
        if (titleTimer == 0) {
            // Play ReDead scream SFX
            Audio_PlaySoundGeneral(NA_SE_VO_ST_DAMAGE, &D_801333D4, 4, &D_801333E0, &D_801333E0, &D_801333E8);
            // Freeze player in place and disable inputs
            this->stateFlags2 |= PLAYER_STATE2_PAUSE_MOST_UPDATING;
        } else if (titleTimer == 25) {
            // Reset screen color
            D_801614B0.r = 0;
            D_801614B0.g = 0;
            D_801614B0.b = 0;
            D_801614B0.a = 0;
            // Reset the game
            titleTimer = 0;
            play->state.running = false;
            this->stateFlags2 &= ~PLAYER_STATE2_PAUSE_MOST_UPDATING;
            SET_NEXT_GAMESTATE(&play->state, FileChoose_Init, FileChooseContext);
            return;
        } else if (titleTimer < 25) {
            // Turn screen red
            if (D_801614B0.r < 255) {
                D_801614B0.r += 15;
            }
            D_801614B0.g = 0;
            D_801614B0.b = 0;
            D_801614B0.a = 255;
        }
        titleTimer++;
    }
    else {
        titleTimer = 0;
    }

    sControlInput = input;

    if (this->voidRespawnCounter < 0) {
        this->voidRespawnCounter++;
        if (this->voidRespawnCounter == 0) {
            this->voidRespawnCounter = 1;
            func_80078884(NA_SE_OC_REVENGE);
        }
    }

    static u8 inJail = false;
    static u8 respawnJail = false;
    s16 i;

    #define PLAYER_JAIL_DIST 75
    #define PLAYER_JAIL_FLOOR_DIST 150

    if (CVarGetInteger("gJailTime", 0) && !respawnJail) {
        if (!inJail) {
            jail[0] = (BgSpot15Saku*)Actor_Spawn(&play->actorCtx, play, ACTOR_BG_SPOT15_SAKU,
                                               this->actor.world.pos.x + PLAYER_JAIL_DIST, this->actor.world.pos.y - 1,
                                               this->actor.world.pos.z, 0, DEGF_TO_BINANG(90.0f), 0, 1, false);
            jail[1] = (BgSpot15Saku*)Actor_Spawn(
                &play->actorCtx, play, ACTOR_BG_SPOT15_SAKU,
                                               this->actor.world.pos.x - PLAYER_JAIL_DIST, this->actor.world.pos.y - 1,
                                               this->actor.world.pos.z, 0, DEGF_TO_BINANG(90.0f), 0, 1, false);
            jail[2] = (BgSpot15Saku*)Actor_Spawn(&play->actorCtx, play, ACTOR_BG_SPOT15_SAKU,
                                               this->actor.world.pos.x, this->actor.world.pos.y - 1,
                                               this->actor.world.pos.z + PLAYER_JAIL_DIST, 0, 0, 0, 1, false);
            jail[3] = (BgSpot15Saku*)Actor_Spawn(&play->actorCtx, play, ACTOR_BG_SPOT15_SAKU,
                                               this->actor.world.pos.x, this->actor.world.pos.y - 1,
                                               this->actor.world.pos.z - PLAYER_JAIL_DIST, 0, 0, 0, 1, false);
            jailFloor = (EnAObj*)Actor_Spawn(&play->actorCtx, play, ACTOR_EN_A_OBJ, this->actor.world.pos.x,
                                         this->actor.world.pos.y - PLAYER_JAIL_FLOOR_DIST - 1, this->actor.world.pos.z,
                                         0, 0, 0, 5, false);
            for (i = 0; i < 4; i++) {
                jail[i]->dyna.actor.room = -1;
            }
            jailFloor->dyna.actor.room = -1;

            inJail = true;
        }
        else {
            if (jailFloor->dyna.actor.xzDistToPlayer > PLAYER_JAIL_DIST) {
                // Forces jail to despawn, it will respawn at Link's new position to prevent cheesing the jail
                respawnJail = true;
            }
        }
    } else {
        if (inJail) {
            for (i = 0; i < 4; i++) {
                if (jail[i] != NULL) {
                    Actor_Kill(jail[i]);
                    jail[i] = NULL;
                }
            }
            if (jailFloor != NULL) {
                Actor_Kill(jailFloor);
                jailFloor = NULL;
            }
            respawnJail = false;
            inJail = false;
        }
    }

    if (CVarGetInteger("gCowRitual", 0)) {
        if (!cowRitual) {
            Vec3f origin = this->actor.world.pos;
            Vec3f vtx[5];
            s16 cowYaw[5];

            vtx[0].x = origin.x + 100.0f;
            vtx[0].z = origin.z;
            cowYaw[0] = DEGF_TO_BINANG(0.0f);

            for (i = 1; i < 5; i++) {
                vtx[i].x = origin.x + (vtx[i - 1].x - origin.x) * Math_CosS(DEGF_TO_BINANG(72.0f)) -
                           (vtx[i - 1].z - origin.z) * Math_SinS(DEGF_TO_BINANG(72.0f));
                vtx[i].z = origin.z + (vtx[i - 1].x - origin.x) * Math_SinS(DEGF_TO_BINANG(72.0f)) +
                           (vtx[i - 1].z - origin.z) * Math_CosS(DEGF_TO_BINANG(72.0f));

                cowYaw[i] = cowYaw[i - 1] - DEGF_TO_BINANG(72.0f);
            }

            ritualFlame =
                (EnLight*)Actor_Spawn(&play->actorCtx, play, ACTOR_EN_LIGHT, this->actor.world.pos.x,
                                      this->actor.world.pos.y, this->actor.world.pos.z, 0, 0, 0, 0, false);
            cow[0] = (EnCow*)Actor_Spawn(&play->actorCtx, play, ACTOR_EN_COW, vtx[0].x, this->actor.world.pos.y,
                                       vtx[0].z, 0, cowYaw[0], 0, 0, false);
            cow[1] = (EnCow*)Actor_Spawn(&play->actorCtx, play, ACTOR_EN_COW, vtx[1].x, this->actor.world.pos.y,
                                       vtx[1].z, 0, cowYaw[1], 0, 0, false);
            cow[2] = (EnCow*)Actor_Spawn(&play->actorCtx, play, ACTOR_EN_COW, vtx[2].x, this->actor.world.pos.y,
                                       vtx[2].z, 0, cowYaw[2], 0, 0, false);
            cow[3] = (EnCow*)Actor_Spawn(&play->actorCtx, play, ACTOR_EN_COW, vtx[3].x, this->actor.world.pos.y,
                                       vtx[3].z, 0, cowYaw[3], 0, 0, false);
            cow[4] = (EnCow*)Actor_Spawn(&play->actorCtx, play, ACTOR_EN_COW, vtx[4].x, this->actor.world.pos.y,
                                       vtx[4].z, 0, cowYaw[4], 0, 0, false);

            for (i = 0; i < 5; i++) {
                // Persistent across rooms
                cow[i]->actor.room = -1;
                if (cow[i]->actor.child != NULL) {
                    Actor_Kill(cow[i]->actor.child);
                }
            }
            ritualFlame->actor.room = -1;
            cowRitual = true;
        } else {
            Vec3f origin = this->actor.world.pos;
            Vec3f vtx[5];
            s16 cowYaw[5];

            vtx[0].x = origin.x + 100.0f;
            vtx[0].z = origin.z;
            cowYaw[0] = DEGF_TO_BINANG(0.0f);

            for (i = 1; i < 5; i++) {
                vtx[i].x = origin.x + (vtx[i - 1].x - origin.x) * Math_CosS(DEGF_TO_BINANG(72.0f)) -
                           (vtx[i - 1].z - origin.z) * Math_SinS(DEGF_TO_BINANG(72.0f));
                vtx[i].z = origin.z + (vtx[i - 1].x - origin.x) * Math_SinS(DEGF_TO_BINANG(72.0f)) +
                           (vtx[i - 1].z - origin.z) * Math_CosS(DEGF_TO_BINANG(72.0f));

                cowYaw[i] = cowYaw[i - 1] - DEGF_TO_BINANG(72.0f);
            }

            ritualFlame->actor.world.pos = this->actor.world.pos;
            for (i = 0; i < 5; i++) {
                cow[i]->actor.world.pos.x = vtx[i].x;
                cow[i]->actor.world.pos.y = this->actor.world.pos.y;
                cow[i]->actor.world.pos.z = vtx[i].z;
                cow[i]->actor.world.rot.z = cowYaw[i];
            }
        }
    } else {
        if (cowRitual) {
            for (i = 0; i < 5; i++) {
                if (cow[i] != NULL) {
                    Actor_Kill(cow[i]);
                    cow[i] = NULL;
                }
            }
            if (ritualFlame != NULL) {
                Actor_Kill(ritualFlame);
                ritualFlame = NULL;
            }
            cowRitual = false;
        }
    }
    
    if (CVarGetInteger("gFireRockRain", 0)) {
        if (!fireRocksFalling) {
            fireRockSpawner =
                (EnEncount2*)Actor_Spawn(&play->actorCtx, play, ACTOR_EN_ENCOUNT2, this->actor.world.pos.x,
                                         this->actor.world.pos.y, this->actor.world.pos.z, 0, 0, 0, 0, false);
            fireRockSpawner->actor.room = -1;
            fireRocksFalling = true;
        } else if (fireRockSpawner != NULL) {
            fireRockSpawner->actor.world.pos = this->actor.world.pos;
        }
    }
    else {
        if (fireRocksFalling != false) {
            if (fireRockSpawner != NULL) {
                Actor_Kill(fireRockSpawner);
                fireRockSpawner = NULL;
            }
            fireRocksFalling = false;
        }
    }

    if (CVarGetInteger("gCuccoAttack", 0)) {
        f32 viewX;
        f32 viewY;
        f32 viewZ;
        Vec3f attackCuccoPos;

        if ((cuccoAtkTimer == 0) && (cuccoAtkNum < CUCCO_ATK_NUM_MAX)) {
            viewX = play->view.lookAt.x - play->view.eye.x;
            viewY = play->view.lookAt.y - play->view.eye.y;
            viewZ = play->view.lookAt.z - play->view.eye.z;
            attackCuccoPos.x = ((Rand_ZeroOne() - 0.5f) * viewX) + play->view.eye.x;
            attackCuccoPos.y = Rand_CenteredFloat(0.3f) + ((play->view.eye.y + 50.0f) + (viewY * 0.5f));
            attackCuccoPos.z = ((Rand_ZeroOne() - 0.5f) * viewZ) + play->view.eye.z;
            attackCucco[cuccoAtkNum] =
                (EnAttackNiw*)Actor_Spawn(&play->actorCtx, play, ACTOR_EN_ATTACK_NIW, attackCuccoPos.x,
                                          attackCuccoPos.y, attackCuccoPos.z, 0, 0, 0, 0, false);

            attackCucco[cuccoAtkNum]->actor.room = -1;

            if (attackCucco != NULL) {
                cuccoAtkNum++;
                cuccoAtkTimer = 10;
            }
        }
        else if (cuccoAtkNum == CUCCO_ATK_NUM_MAX) {
            for (i = 0; i < CUCCO_ATK_NUM_MAX; i++) {
                if (attackCucco[i] != NULL) {
                    Actor_Kill(attackCucco[i]);
                }
            }
            cuccoAtkNum = 0;
        }
        DECR(cuccoAtkTimer);
    } else {
        for (i = 0; i < CUCCO_ATK_NUM_MAX; i++) {
            if (attackCucco[i] != NULL) {
                Actor_Kill(attackCucco[i]);
                attackCucco[i] = NULL;
            }
        }
        cuccoAtkNum = 0;
    }

    static f32 j = 0;
    static Vec3f rupeeOrigin = { 0 };
    static u8 rupeeStartTimer = 0;

    if (CVarGetInteger("gExplodingRupeeChallenge", 0)) {
        if (explodeRupee == NULL) {
            explodeRupee =
                Actor_Spawn(&play->actorCtx, play, ACTOR_EN_EX_RUPPY, this->actor.world.pos.x + 150.0f,
                            this->actor.world.pos.y, this->actor.world.pos.z, 0, 0, 0, 1, false);
            explodeRupee->room = -1;
            rupeeOrigin = this->actor.world.pos;
            Audio_PlayActorSound2(&this->actor, NA_SE_SY_START_SHOT);
            rupeeStartTimer = 40;
        } else {
            if (rupeeStartTimer != 0) {
                // freeze Link in place
                this->stateFlags2 |= PLAYER_STATE2_PAUSE_MOST_UPDATING;
            } else {
                // unfreeze Link
                this->stateFlags2 &= ~PLAYER_STATE2_PAUSE_MOST_UPDATING;
            }
            if (this->damageEffect == PLAYER_DMGEFFECT_KNOCKBACK) {
                // HALF HEART, BABY!!!
                if (gSaveContext.health < 8) {
                    gSaveContext.health = 8;
                } else {
                    gSaveContext.health = 0;
                }
                Player_SetupInvincibilityNoDamageFlash(this, -20);
                func_80078884(NA_SE_SY_ERROR);
                CVarSetInteger("gExplodingRupeeChallenge", 0);
                rupeeStartTimer = 0;
            } else if (Math_Vec3f_DistXZ(&rupeeOrigin, &this->actor.world.pos) >= 160.0f) {
                Audio_PlayFanfare(NA_BGM_SMALL_ITEM_GET | 0x900);
                CVarSetInteger("gExplodingRupeeChallenge", 0);
                rupeeStartTimer = 0;
            }
            explodeRupee->world.pos.x = 150.0f * Math_CosF(j) + rupeeOrigin.x;
            explodeRupee->world.pos.z = 150.0f * Math_SinF(j) + rupeeOrigin.z;
            explodeRupee->world.pos.y = this->actor.world.pos.y;
            if (j < DEGF_TO_RADF(360.0f)) {
                j += DEGF_TO_RADF(36.0f);
            } else if (j >= DEGF_TO_RADF(360.0f)) {
                j = DEGF_TO_RADF(0.0f);
            }
        }
        DECR(rupeeStartTimer);
    }
    else {
        if (explodeRupee != NULL) {
            Actor_Kill(explodeRupee);
            explodeRupee = NULL;
        }
    }

    #define SPACE_ROT_ACCEL_TARGET 90.0f

    static u8 onSpaceTrip = false;
    static s16 spaceTimer = 0;
    static f32 rotAccel = 0.0f;
    static Vec3f spaceFlameVelocity = { 0.0f, 0.5f, 0.0f };
    static Vec3f spaceFlameAccel = { 0.0f, 0.5f, 0.0f };
    static Color_RGBA8 spaceFlamePrimColor = { 255, 255, 100, 255 };
    static Color_RGBA8 spaceFlameEnvColor = { 255, 50, 0, 0 };
    static Vec3f dustAccel = { 0.0f, -0.3f, 0.0f };
    static Color_RGBA8 dustPrim = { 200, 200, 200, 128 };
    static Color_RGBA8 dustEnv = { 100, 100, 100, 0 };

    if (CVarGetInteger("gTripToSpace", 0) || onSpaceTrip) {
        Vec3f dustVelocity;
        Vec3f dustPos;

        if (onSpaceTrip == false) {
            CVarSetInteger("gTripToSpace", 0);
            onSpaceTrip = true;
        }

        // If spent more than 15 seconds going to space, spawn explosion and fall
        if (spaceTimer > 15 * 20) {
            spaceTimer = 0;
            rotAccel = 0;
            onSpaceTrip = false;
            this->stateFlags2 &= ~PLAYER_STATE2_PAUSE_MOST_UPDATING;
            this->actor.gravity = 1.0f;
            Player_SetupReturnToStandStill(this, play);
            Player_SpawnExplosion(play, this);
            // JUST ONE HEART, BABY!!!
            if (gSaveContext.health < 16) {
                gSaveContext.health = 16;
            } else {
                gSaveContext.health = 0;
            }
            Player_SetupInvincibilityNoDamageFlash(this, -20);
        }
        // If player hits a ceiling, go back to normal gameplay
        else if (this->actor.bgCheckFlags & (1 << 4)) {
            spaceTimer = 0;
            rotAccel = 0;
            onSpaceTrip = false;
            this->stateFlags2 &= ~PLAYER_STATE2_PAUSE_MOST_UPDATING;
            this->actor.gravity = 1.0f;
            Player_SetupReturnToStandStill(this, play);
            Player_SpawnExplosion(play, this);
        } else {
            this->stateFlags2 |= PLAYER_STATE2_PAUSE_MOST_UPDATING;
            this->actor.shape.rot.y += DEGF_TO_BINANG(rotAccel);

            if (rotAccel == SPACE_ROT_ACCEL_TARGET) {
                this->actor.gravity = 0.0f;
                this->actor.world.pos.y += 20.0f;
                func_8002836C(play, &this->actor.world.pos, &spaceFlameVelocity, &spaceFlameAccel,
                              &spaceFlamePrimColor, &spaceFlameEnvColor, 200.0f, 0, 8);
                func_8002F974(&this->actor, NA_SE_EV_STONE_LAUNCH - SFX_FLAG);
            }
            else {
                for (s16 i = 0; i < 3; i++) {
                    dustVelocity.x = Rand_CenteredFloat(15.0f);
                    dustVelocity.y = Rand_ZeroFloat(-1.0f);
                    dustVelocity.z = Rand_CenteredFloat(15.0f);
                    dustPos.x = this->actor.world.pos.x + (dustVelocity.x + dustVelocity.x);
                    dustPos.y = this->actor.world.pos.y + 7.0f;
                    dustPos.z = this->actor.world.pos.z + (dustVelocity.z + dustVelocity.z);
                    func_8002836C(play, &dustPos, &dustVelocity, &dustAccel, &dustPrim, &dustEnv,
                                  (s16)Rand_ZeroFloat(50.0f) + 200, 40, 15);
                }
                func_8002F974(&this->actor, NA_SE_EV_FIRE_PILLAR - SFX_FLAG);
                Math_SmoothStepToF(&rotAccel, SPACE_ROT_ACCEL_TARGET, 0.1f, 1.0f, 0.01f);
            }

            spaceTimer++;
        }
    }

    static u8 forcedSandstormOn = false;

    if (CVarGetInteger("gSandstorm", 0) || (CVarGetInteger("gChaosRedeem", 0) && Rand_ZeroOne() < 0.02f)) {
        play->envCtx.sandstormState = 3;
        forcedSandstormOn = true;
    }
    else if (forcedSandstormOn) {
        play->envCtx.sandstormState = 0;
        forcedSandstormOn = false;
    }

    if (CVarGetInteger("gNaviSpam", 0) || (CVarGetInteger("gChaosRedeem", 0) && Rand_ZeroOne() < 0.02f)) {
        if (!(play->state.frames % 10)) {
            Audio_PlaySoundGeneral(NA_SE_VO_NAVY_CALL, &D_801333D4, 4, &D_801333E0, &D_801333E0, &D_801333E8);
        }
        if (!(play->state.frames % 24)) {
            Audio_PlaySoundGeneral(NA_SE_VO_NAVY_HELLO, &D_801333D4, 4, &D_801333E0, &D_801333E0, &D_801333E8);
        }
    }

    if (CVarGetInteger("gSpawnExplosion", 0)) {
        Player_SpawnExplosion(play, this);
        CVarSetInteger("gSpawnExplosion", 0);
    };

    if ((CVarGetInteger("gDisableEnemyDraw", 0) || (CVarGetInteger("gChaosRedeem", 0) && Rand_ZeroOne() < 0.02f)) && this->targetActor != NULL) {
        if (this->targetActor->category == ACTORCAT_ENEMY) {
            play->actorCtx.targetCtx.arrowPointedActor = NULL;
            this->targetActor = NULL;
        }
    }

    static u8 adjustLight = false;
    static f32 lightIntensity = -1;

    if (adjustLight == true) {
        Environment_AdjustLights(play, lightIntensity, this->actor.projectedPos.z + 600.0f, 0.2f, 0.5f);
    }

    if (CVarGetInteger("gDarkenArea", 0) || (CVarGetInteger("gChaosRedeem", 0) && Rand_ZeroOne() < 0.02f)) {
        adjustLight = true;
        Math_SmoothStepToF(&lightIntensity, 1.0f, 0.5f, 0.2f, 0.01f);
    }
    else {
        if (lightIntensity == 0) {
            adjustLight = false;
        }
        else if (lightIntensity > 0) {
            Math_SmoothStepToF(&lightIntensity, 0.0f, 0.5f, 0.2f, 0.01f);
        }
    }

    if (CVarGetInteger("gInvisPlayer", 0) || (CVarGetInteger("gChaosRedeem", 0) && Rand_ZeroOne() < 0.02f)) {
        this->actor.draw = NULL;
    }
    else {
        this->actor.draw = Player_Draw;
    }

    if (CVarGetInteger("gChaosSpin", 0) || (CVarGetInteger("gChaosRedeem", 0) && Rand_ZeroOne() < 0.02f)) {
        this->actor.shape.rot.x += DEGF_TO_BINANG(5.0f);
        this->actor.shape.rot.y += DEGF_TO_BINANG(9.0f);
        this->actor.shape.rot.z += DEGF_TO_BINANG(15.0f);
        if (this->actor.shape.rot.x > DEGF_TO_BINANG(90.0f)) {
            this->actor.shape.rot.x = DEGF_TO_BINANG(270.0f);
        }
        if (this->actor.shape.rot.z > DEGF_TO_BINANG(90.0f)) {
            this->actor.shape.rot.z = DEGF_TO_BINANG(270.0f);
        }
    }
    else {
        this->actor.shape.rot.x = 0;
        this->actor.shape.rot.z = 0;
    }

    #define SCREEN_COLOR_RATE 15

    static u8 colorMode = 0;

    if (CVarGetInteger("gRaveMode", 0) || (CVarGetInteger("gChaosRedeem", 0) && Rand_ZeroOne() < 0.02f)) {
        switch (colorMode) {
            case 0:
                D_801614B0.r = 255;
                D_801614B0.g += SCREEN_COLOR_RATE;
                D_801614B0.b = 0;
                D_801614B0.a = 255;

                if (D_801614B0.g == 255) {
                    colorMode = 1;
                }
                break;

            case 1:
                D_801614B0.r -= SCREEN_COLOR_RATE;
                D_801614B0.g = 255;
                D_801614B0.b = 0;
                D_801614B0.a = 255;

                if (D_801614B0.r == 0) {
                    colorMode = 2;
                }
                break;
        
            case 2:
                D_801614B0.r = 0;
                D_801614B0.g = 255;
                D_801614B0.b += SCREEN_COLOR_RATE;
                D_801614B0.a = 255;

                if (D_801614B0.b == 255) {
                    colorMode = 3;
                }
                break;

            case 3:
                D_801614B0.r = 0;
                D_801614B0.g -= SCREEN_COLOR_RATE;
                D_801614B0.b = 255;
                D_801614B0.a = 255;

                if (D_801614B0.g == 0) {
                    colorMode = 4;
                }
                break;

            case 4:
                D_801614B0.r += SCREEN_COLOR_RATE;
                D_801614B0.g = 0;
                D_801614B0.b = 255;
                D_801614B0.a = 255;

                if (D_801614B0.r == 255) {
                    colorMode = 5;
                }
                break;

            case 5:
                D_801614B0.r = 255;
                D_801614B0.g = 0;
                D_801614B0.b -= SCREEN_COLOR_RATE;
                D_801614B0.a = 255;

                if (D_801614B0.b == 0) {
                    colorMode = 0;
                }
                break;
        }
    } else if (!CVarGetInteger("gRedoRando", 0) && !CVarGetInteger("gBackToTitle", 0)) {
        D_801614B0.r = 0;
        D_801614B0.g = 0;
        D_801614B0.b = 0;
        D_801614B0.a = 0;
        colorMode = 0;
    }

    if (CVarGetInteger("gRestrainLink", 0)) {
        Player_SetupRestrainedByEnemy(play, this);
        CVarSetInteger("gRestrainLink", 0);
    }

    Math_Vec3f_Copy(&this->actor.prevPos, &this->actor.home.pos);

    if (this->fpsItemShotTimer != 0) {
        this->fpsItemShotTimer--;
    }

    if (this->endTalkTimer != 0) {
        this->endTalkTimer--;
    }

    if (this->deathTimer != 0) {
        this->deathTimer--;
    }

    if (this->invincibilityTimer < 0) {
        this->invincibilityTimer++;
    } else if (this->invincibilityTimer > 0) {
        this->invincibilityTimer--;
    }

    if (this->runDamageTimer != 0) {
        this->runDamageTimer--;
    }

    func_808473D4(play, this);
    Player_SetupZTargeting(this, play);


    if ((this->heldItemAction == PLAYER_IA_STICK) && (this->fpsItemType != 0)) {
        func_80848A04(play, this);
    } else if ((this->heldItemAction == PLAYER_IA_FISHING_POLE) && (this->fpsItemType < 0)) {
        this->fpsItemType++;
    }

    if (this->shockTimer != 0) {
        func_80848B44(play, this);
    }

    if (this->isBurning) {
        func_80848C74(play, this);
    }

    if ((this->stateFlags3 & PLAYER_STATE3_RESTORE_NAYRUS_LOVE) && (gSaveContext.nayrusLoveTimer != 0) && (gSaveContext.magicState == 0)) {
        gSaveContext.magicState = 3;
        func_80846A00(play, this, 1);
        this->stateFlags3 &= ~PLAYER_STATE3_RESTORE_NAYRUS_LOVE;
    }

    if (this->stateFlags2 & PLAYER_STATE2_PAUSE_MOST_UPDATING || CVarGetInteger("gOnHold", 0)) {
        if (!(this->actor.bgCheckFlags & 1)) {
            Player_StopMovement(this);
            Actor_MoveForward(&this->actor);
        }

        func_80847BA0(play, this);
    } else {
        f32 temp_f0;
        f32 phi_f12;

        if (this->currentBoots != this->prevBoots) {
            if (this->currentBoots == PLAYER_BOOTS_IRON) {
                if (this->stateFlags1 & PLAYER_STATE1_SWIMMING) {
                    Player_ResetSubCam(play, this);
                    if (this->ageProperties->unk_2C < this->actor.yDistToWater) {
                        this->stateFlags2 |= PLAYER_STATE2_DIVING;
                    }
                }
            } else {
                if (this->stateFlags1 & PLAYER_STATE1_SWIMMING) {
                    if ((this->prevBoots == PLAYER_BOOTS_IRON) || (this->actor.bgCheckFlags & 1)) {
                        func_8083D36C(play, this);
                        this->stateFlags2 &= ~PLAYER_STATE2_DIVING;
                    }
                }
            }

            this->prevBoots = this->currentBoots;
        }

        if ((this->actor.parent == NULL) && (this->stateFlags1 & PLAYER_STATE1_RIDING_HORSE)) {
            this->actor.parent = this->rideActor;
            Player_SetupRideHorse(play, this);
            this->stateFlags1 |= PLAYER_STATE1_RIDING_HORSE;
            Player_PlayAnimOnce(play, this, &gPlayerAnim_link_uma_wait_1);
            Player_SetupAnimMovement(play, this, 0x9B);
            this->genericTimer = 99;
        }

        if (this->comboTimer == 0) {
            this->slashCounter = 0;
        } else if (this->comboTimer < 0) {
            this->comboTimer++;
        } else {
            this->comboTimer--;
        }

        Math_ScaledStepToS(&this->shapePitchOffset, 0, 400);
        func_80032CB4(this->unk_3A8, 20, 80, 6);

        this->actor.shape.face = this->unk_3A8[0] + ((play->gameplayFrames & 32) ? 0 : 3);

        if (this->currentMask == PLAYER_MASK_BUNNY) {
            Player_BunnyHoodPhysics(this);
        }

        if (Actor_PlayerIsAimingFpsItem(this) != 0) {
            Player_BowStringMoveAfterShot(this);
        }

        if (!(this->skelAnime.moveFlags & 0x80)) {
            if (CVarGetInteger("gSlipperyFloor", 0) ||
                ((this->actor.bgCheckFlags & 1) &&
                 (sFloorSpecialProperty == 5) && (this->currentBoots != PLAYER_BOOTS_IRON)) ||
                  ((this->currentBoots == PLAYER_BOOTS_HOVER || GameInteractor_GetSlipperyFloorActive()) &&
                 !(this->stateFlags1 & (PLAYER_STATE1_SWIMMING | PLAYER_STATE1_IN_CUTSCENE)))) {
                f32 sp70 = this->linearVelocity;
                s16 sp6E = this->currentYaw;
                s16 yawDiff = this->actor.world.rot.y - sp6E;
                s32 pad;

                if ((ABS(yawDiff) > 0x6000) && (this->actor.speedXZ != 0.0f)) {
                    sp70 = 0.0f;
                    sp6E += 0x8000;
                }

                if (Math_StepToF(&this->actor.speedXZ, sp70, CVarGetInteger("gSlipperyFloor", 0) ? 0.15f: 0.35f) &&
                    (sp70 == 0.0f)) {
                    this->actor.world.rot.y = this->currentYaw;
                }

                if (this->linearVelocity != 0.0f) {
                    s32 phi_v0;

                    phi_v0 = (fabsf(this->linearVelocity) * 700.0f) - (fabsf(this->actor.speedXZ) * 100.0f);
                    phi_v0 = CLAMP(phi_v0, 0, 1350);

                    Math_ScaledStepToS(&this->actor.world.rot.y, sp6E, phi_v0);
                }

                if ((this->linearVelocity == 0.0f) && (this->actor.speedXZ != 0.0f)) {
                    func_800F4138(&this->actor.projectedPos, 0xD0, this->actor.speedXZ);
                }
            } else {
                this->actor.speedXZ = this->linearVelocity;
                this->actor.world.rot.y = this->currentYaw;
            }

            func_8002D868(&this->actor);

            if ((this->pushedSpeed != 0.0f) && !Player_InCsMode(play) &&
                !(this->stateFlags1 & (PLAYER_STATE1_HANGING_FROM_LEDGE_SLIP | PLAYER_STATE1_CLIMBING_ONTO_LEDGE |
                                       PLAYER_STATE1_CLIMBING)) &&
                (Player_JumpUpToLedge != this->actionFunc) && (Player_UpdateMagicSpell != this->actionFunc)) {
                this->actor.velocity.x += this->pushedSpeed * Math_SinS(this->pushedYaw);
                this->actor.velocity.z += this->pushedSpeed * Math_CosS(this->pushedYaw);
            }

            func_8002D7EC(&this->actor);
            func_80847BA0(play, this);
        } else {
            sFloorSpecialProperty = 0;
            this->floorProperty = 0;

            if (!(this->stateFlags1 & PLAYER_STATE1_EXITING_SCENE) &&
                (this->stateFlags1 & PLAYER_STATE1_RIDING_HORSE)) {
                EnHorse* rideActor = (EnHorse*)this->rideActor;
                CollisionPoly* sp5C;
                s32 sp58;
                Vec3f sp4C;

                if (!(rideActor->actor.bgCheckFlags & 1)) {
                    Player_RaycastFloorWithOffset(play, this, &D_80854814, &sp4C, &sp5C, &sp58);
                } else {
                    sp5C = rideActor->actor.floorPoly;
                    sp58 = rideActor->actor.floorBgId;
                }

                if ((sp5C != NULL) && Player_SetupExit(play, this, sp5C, sp58)) {
                    if (DREG(25) != 0) {
                        DREG(25) = 0;
                    } else {
                        AREG(6) = 1;
                    }
                }
            }

            sConveyorSpeedIndex = 0;
            this->pushedSpeed = 0.0f;
        }

        if ((sConveyorSpeedIndex != 0) && (this->currentBoots != PLAYER_BOOTS_IRON)) {
            f32 sp48;

            sConveyorSpeedIndex--;

            if (sIsFloorConveyor == 0) {
                sp48 = D_80854820[sConveyorSpeedIndex];

                if (!(this->stateFlags1 & PLAYER_STATE1_SWIMMING)) {
                    sp48 *= 0.25f;
                }
            } else {
                sp48 = D_8085482C[sConveyorSpeedIndex];
            }

            Math_StepToF(&this->pushedSpeed, sp48, sp48 * 0.1f);

            Math_ScaledStepToS(&this->pushedYaw, sConveyorYaw,
                               ((this->stateFlags1 & PLAYER_STATE1_SWIMMING) ? 400.0f : 800.0f) * sp48);
        } else if (this->pushedSpeed != 0.0f) {
            Math_StepToF(&this->pushedSpeed, 0.0f, (this->stateFlags1 & PLAYER_STATE1_SWIMMING) ? 0.5f : 1.0f);
        }

        if (!Player_InBlockingCsMode(play, this) && !(this->stateFlags2 & PLAYER_STATE2_INSIDE_CRAWLSPACE)) {
            func_8083D53C(play, this);

            if ((this->actor.category == ACTORCAT_PLAYER) && (gSaveContext.health == 0)) {
                if (this->stateFlags1 & (PLAYER_STATE1_HANGING_FROM_LEDGE_SLIP | PLAYER_STATE1_CLIMBING_ONTO_LEDGE |
                    PLAYER_STATE1_CLIMBING)) {
                    Player_ResetAttributes(play, this);
                    Player_SetupFallFromLedge(this, play);
                } else if ((this->actor.bgCheckFlags & 1) || (this->stateFlags1 & PLAYER_STATE1_SWIMMING)) {
                    Player_SetupDie(play, this,
                                    Player_IsSwimming(this)   ? &gPlayerAnim_link_swimer_swim_down
                                    : (this->shockTimer != 0) ? &gPlayerAnim_link_normal_electric_shock_end
                                                              : &gPlayerAnim_link_derth_rebirth);
                }
            } else {
                if ((this->actor.parent == NULL) &&
                    ((play->sceneLoadFlag == 0x14) || (this->deathTimer != 0) || !Player_UpdateDamage(this, play))) {
                    if (!CVarGetInteger("gDisableLedgeJump", 0)) {
                        Player_SetupMidairBehavior(this, play);
                    }
                } else {
                    this->fallStartHeight = this->actor.world.pos.y;
                }
                func_80848EF8(this, play);
            }
        }

        if ((play->csCtx.state != CS_STATE_IDLE) && (this->csMode != 6) &&
            !(this->stateFlags1 & PLAYER_STATE1_RIDING_HORSE) &&
            !(this->stateFlags2 & PLAYER_STATE2_RESTRAINED_BY_ENEMY) && (this->actor.category == ACTORCAT_PLAYER)) {
            CsCmdActorAction* linkActionCsCmd = play->csCtx.linkAction;

            if ((linkActionCsCmd != NULL) && (D_808547C4[linkActionCsCmd->action] != 0)) {
                func_8002DF54(play, NULL, 6);
                Player_StopMovement(this);
            } else if ((this->csMode == 0) && !(this->stateFlags2 & PLAYER_STATE2_DIVING) &&
                       (play->csCtx.state != CS_STATE_UNSKIPPABLE_INIT)) {
                func_8002DF54(play, NULL, 0x31);
                Player_StopMovement(this);
            }
        }

        if (this->csMode != 0) {
            if ((this->csMode != 7) ||
                !(this->stateFlags1 & (PLAYER_STATE1_HANGING_FROM_LEDGE_SLIP | PLAYER_STATE1_CLIMBING_ONTO_LEDGE |
                PLAYER_STATE1_CLIMBING | PLAYER_STATE1_TAKING_DAMAGE))) {
                this->attentionMode = 3;
            } else if (Player_StartCutscene != this->actionFunc) {
                Player_CutsceneEnd(play, this, NULL);
            }
        } else {
            this->prevCsMode = 0;
        }

        func_8083D6EC(play, this);

        if ((this->targetActor == NULL) && (this->naviTextId == 0)) {
            this->stateFlags2 &= ~(PLAYER_STATE2_CAN_SPEAK_OR_CHECK | PLAYER_STATE2_NAVI_REQUESTING_TALK);
        }

        this->stateFlags1 &= ~(PLAYER_STATE1_SWINGING_BOTTLE | PLAYER_STATE1_READY_TO_SHOOT |
                               PLAYER_STATE1_CHARGING_SPIN_ATTACK | PLAYER_STATE1_SHIELDING);
        this->stateFlags2 &=
            ~(PLAYER_STATE2_CAN_GRAB_PUSH_PULL_WALL | PLAYER_STATE2_CAN_CLIMB_PUSH_PULL_WALL |
              PLAYER_STATE2_MAKING_REACTABLE_NOISE | PLAYER_STATE2_DISABLE_MOVE_ROTATION_WHILE_Z_TARGETING |
              PLAYER_STATE2_ALWAYS_DISABLE_MOVE_ROTATION | PLAYER_STATE2_ENABLE_PUSH_PULL_CAM |
              PLAYER_STATE2_SPAWN_DUST_AT_FEET | PLAYER_STATE2_IDLE_WHILE_CLIMBING | PLAYER_STATE2_FROZEN_IN_ICE |
              PLAYER_STATE2_CAN_ENTER_CRAWLSPACE | PLAYER_STATE2_CAN_DISMOUNT_HORSE | PLAYER_STATE2_ENABLE_REFLECTION);
        this->stateFlags3 &= ~PLAYER_STATE3_CHECKING_FLOOR_AND_WATER_COLLISION;

        if (CVarGetInteger("gDisableTurning", 0)) {
            this->stateFlags2 |= PLAYER_STATE2_ALWAYS_DISABLE_MOVE_ROTATION;
        }

        func_80847298(this);
        Player_StoreAnalogStickInput(play, this);

        if (this->stateFlags1 & PLAYER_STATE1_SWIMMING) {
            sWaterSpeedScale = 0.5f;
        } else {
            sWaterSpeedScale = 1.0f;
        }

        sInvertedWaterSpeedScale = 1.0f / sWaterSpeedScale;
        sUsingItemAlreadyInHand = sUsingItemAlreadyInHand2 = 0;
        sCurrentMask = this->currentMask;

        if (!(this->stateFlags3 & PLAYER_STATE3_PAUSE_ACTION_FUNC)) {
            this->actionFunc(this, play);
        }

        Player_UpdateCamAndSeqModes(play, this);

        if (this->skelAnime.moveFlags & 8) {
            AnimationContext_SetMoveActor(play, &this->actor, &this->skelAnime,
                                          (this->skelAnime.moveFlags & 4) ? 1.0f : this->ageProperties->unk_08);
        }

        Player_UpdateYaw(this, play);

        if (CHECK_FLAG_ALL(this->actor.flags, ACTOR_FLAG_PLAYER_TALKED_TO)) {
            this->talkActorDistance = 0.0f;
        } else {
            this->talkActor = NULL;
            this->talkActorDistance = FLT_MAX;
            this->exchangeItemId = EXCH_ITEM_NONE;
        }

        if (!(this->stateFlags1 & PLAYER_STATE1_HOLDING_ACTOR)) {
            this->interactRangeActor = NULL;
            this->getItemDirection = 0x6000;
        }

        if (this->actor.parent == NULL) {
            this->rideActor = NULL;
        }

        this->naviTextId = 0;

        if (!(this->stateFlags2 & PLAYER_STATE2_PLAYING_OCARINA_FOR_ACTOR)) {
            this->ocarinaActor = NULL;
        }

        this->stateFlags2 &= ~PLAYER_STATE2_NEAR_OCARINA_ACTOR;
        this->stoneOfAgonyActorDistSq = FLT_MAX;

        temp_f0 = this->actor.world.pos.y - this->actor.prevPos.y;

        this->doorType = PLAYER_DOORTYPE_NONE;
        this->damageEffect = 0;
        this->forcedTargetActor = NULL;

        phi_f12 =
            ((this->bodyPartsPos[PLAYER_BODYPART_L_FOOT].y + this->bodyPartsPos[PLAYER_BODYPART_R_FOOT].y) * 0.5f) +
            temp_f0;
        temp_f0 += this->bodyPartsPos[PLAYER_BODYPART_HEAD].y + 10.0f;

        this->cylinder.dim.height = temp_f0 - phi_f12;

        if (this->cylinder.dim.height < 0) {
            phi_f12 = temp_f0;
            this->cylinder.dim.height = -this->cylinder.dim.height;
        }

        this->cylinder.dim.yShift = phi_f12 - this->actor.world.pos.y;

        if (this->stateFlags1 & PLAYER_STATE1_SHIELDING) {
            this->cylinder.dim.height = this->cylinder.dim.height * 0.8f;
        }

        Collider_UpdateCylinder(&this->actor, &this->cylinder);

        if (!(this->stateFlags2 & PLAYER_STATE2_FROZEN_IN_ICE)) {
            if (!(this->stateFlags1 & (PLAYER_STATE1_IN_DEATH_CUTSCENE | PLAYER_STATE1_HANGING_FROM_LEDGE_SLIP |
                                       PLAYER_STATE1_CLIMBING_ONTO_LEDGE | PLAYER_STATE1_RIDING_HORSE))) {
                CollisionCheck_SetOC(play, &play->colChkCtx, &this->cylinder.base);
            }

            if (!(this->stateFlags1 & (PLAYER_STATE1_IN_DEATH_CUTSCENE | PLAYER_STATE1_TAKING_DAMAGE)) &&
                (this->invincibilityTimer <= 0)) {
                CollisionCheck_SetAC(play, &play->colChkCtx, &this->cylinder.base);

                if (this->invincibilityTimer < 0) {
                    CollisionCheck_SetAT(play, &play->colChkCtx, &this->cylinder.base);
                }
            }
        }

        AnimationContext_SetNextQueue(play);
    }

    Math_Vec3f_Copy(&this->actor.home.pos, &this->actor.world.pos);
    Math_Vec3f_Copy(&this->prevWaistPos, &this->bodyPartsPos[PLAYER_BODYPART_WAIST]);

    if (this->stateFlags1 &
        (PLAYER_STATE1_IN_DEATH_CUTSCENE | PLAYER_STATE1_SKIP_OTHER_ACTORS_UPDATE | PLAYER_STATE1_IN_CUTSCENE)) {
        this->actor.colChkInfo.mass = MASS_IMMOVABLE;
    } else {
        this->actor.colChkInfo.mass = 50;
    }

    this->stateFlags3 &= ~PLAYER_STATE3_PAUSE_ACTION_FUNC;

    if (CVarGetInteger("gMegaLetterbox", 0)) {
        ShrinkWindow_SetVal(110);
    }

    if (CVarGetInteger("gPlayerGravity", 0) != 0) {
        this->actor.gravity += CVarGetInteger("gPlayerGravity", 0) * 0.1f;
    }

    Collider_ResetCylinderAC(play, &this->cylinder.base);

    Collider_ResetQuadAT(play, &this->meleeWeaponQuads[0].base);
    Collider_ResetQuadAT(play, &this->meleeWeaponQuads[1].base);

    Collider_ResetQuadAC(play, &this->shieldQuad.base);
    Collider_ResetQuadAT(play, &this->shieldQuad.base);
}

static Vec3f D_80854838 = { 0.0f, 0.0f, -30.0f };

void Player_Update(Actor* thisx, PlayState* play) {
    static Vec3f sDogSpawnPos;
    Player* this = (Player*)thisx;
    s32 dogParams;
    s32 pad;
    Input sp44;
    Actor* dog;

    if (Player_CheckNoDebugModeCombo(this, play)) {
        if (gSaveContext.dogParams < 0) {
            // Disable object dependency to prevent losing dog in scenes other than market
            if (Object_GetIndex(&play->objectCtx, OBJECT_DOG) < 0 && !CVarGetInteger("gDogFollowsEverywhere", 0)) {
                gSaveContext.dogParams = 0;
            } else {
                gSaveContext.dogParams &= 0x7FFF;
                Player_GetWorldPosRelativeToPlayer(this, &this->actor.world.pos, &D_80854838, &sDogSpawnPos);
                dogParams = gSaveContext.dogParams;

                dog = Actor_Spawn(&play->actorCtx, play, ACTOR_EN_DOG, sDogSpawnPos.x, sDogSpawnPos.y,
                                  sDogSpawnPos.z, 0, this->actor.shape.rot.y, 0, dogParams | 0x8000, true);
                if (dog != NULL) {
                    // Room -1 allows actor to cross between rooms, similar to Navi
                    dog->room = CVarGetInteger("gDogFollowsEverywhere", 0) ? -1 : 0;
                }
            }
        }

        if ((this->interactRangeActor != NULL) && (this->interactRangeActor->update == NULL)) {
            this->interactRangeActor = NULL;
        }

        if ((this->heldActor != NULL) && (this->heldActor->update == NULL)) {
            Player_DetatchHeldActor(play, this);
        }

        if (this->stateFlags1 & (PLAYER_STATE1_INPUT_DISABLED | PLAYER_STATE1_IN_CUTSCENE)) {
            memset(&sp44, 0, sizeof(sp44));
        } else {
            sp44 = play->state.input[0];
            if (this->endTalkTimer != 0) {
                sp44.cur.button &= ~(BTN_A | BTN_B | BTN_CUP);
                sp44.press.button &= ~(BTN_A | BTN_B | BTN_CUP);
            }
        }

        if (CVarGetInteger("gEnableWalkModify", 0) && CVarGetInteger("gWalkSpeedToggle", 0)) {
            if (CHECK_BTN_ALL(sControlInput->press.button, BTN_MODIFIER1)) {
                gWalkSpeedToggle1 = !gWalkSpeedToggle1;
            }
            if (CHECK_BTN_ALL(sControlInput->press.button, BTN_MODIFIER2)) {
                gWalkSpeedToggle2 = !gWalkSpeedToggle2;
            }
        }

        Player_UpdateCommon(this, play, &sp44);
    }

    MREG(52) = this->actor.world.pos.x;
    MREG(53) = this->actor.world.pos.y;
    MREG(54) = this->actor.world.pos.z;
    MREG(55) = this->actor.world.rot.y;

    // Make Link normal size when going through doors and crawlspaces and when climbing ladders.
    // Otherwise Link can glitch out, being in unloaded rooms or falling OoB.
    if (this->stateFlags1 & PLAYER_STATE1_CLIMBING || this->stateFlags1 & PLAYER_STATE1_IN_CUTSCENE ||
        this->stateFlags2 & PLAYER_STATE2_INSIDE_CRAWLSPACE) {
        this->actor.scale.x = 0.01f;
        this->actor.scale.y = 0.01f;
        this->actor.scale.z = 0.01f;
    } else {
        switch (GameInteractor_GetLinkSize()) {
            case GI_LINK_SIZE_RESET:
                this->actor.scale.x = 0.01f;
                this->actor.scale.y = 0.01f;
                this->actor.scale.z = 0.01f;
                GameInteractor_SetLinkSize(GI_LINK_SIZE_NORMAL);
                break;
            case GI_LINK_SIZE_GIANT:
                this->actor.scale.x = 0.02f;
                this->actor.scale.y = 0.02f;
                this->actor.scale.z = 0.02f;
                break;
            case GI_LINK_SIZE_MINISH:
                this->actor.scale.x = 0.001f;
                this->actor.scale.y = 0.001f;
                this->actor.scale.z = 0.001f;
                break;
            case GI_LINK_SIZE_PAPER:
                this->actor.scale.x = 0.001f;
                this->actor.scale.y = 0.01f;
                this->actor.scale.z = 0.01f;
                break;
            case GI_LINK_SIZE_SQUISHED:
                this->actor.scale.x = 0.015f;
                this->actor.scale.y = 0.001f;
                this->actor.scale.z = 0.015f;
                break;
            case GI_LINK_SIZE_NORMAL:
            default:
                break;
        }
    }

    // Don't apply gravity when Link is in water, otherwise
    // it makes him sink instead of float.
    if (!(this->stateFlags1 & PLAYER_STATE1_SWIMMING)) {
        switch (GameInteractor_GravityLevel()) {
            case GI_GRAVITY_LEVEL_HEAVY:
                this->actor.gravity = -4.0f;
                break;
            case GI_GRAVITY_LEVEL_LIGHT:
                this->actor.gravity = -0.3f;
                break;
            default:
                break;
        }
    }

    if (GameInteractor_GetRandomWindActive()) {
        Player* player = GET_PLAYER(play);
        player->pushedSpeed = 3.0f;
        // Play fan sound (too annoying)
        //func_8002F974(&player->actor, NA_SE_EV_WIND_TRAP - SFX_FLAG);
    }
    
    GameInteractor_ExecuteOnPlayerUpdate();

    // Chaos
    f32 customScale = GameInteractor_CustomLinkScale();
    this->actor.scale.x = 0.01f * customScale;
    this->actor.scale.y = 0.01f * customScale;
    this->actor.scale.z = 0.01f * customScale;
    if (GameInteractor_GetLinkSize() == GI_LINK_SIZE_PAPER) {
        this->actor.scale.x /= 10.0f;
    }
}

static struct_80858AC8 D_80858AC8;
static Vec3s D_80858AD8[25];

static Gfx* sMaskDlists[PLAYER_MASK_MAX - 1] = {
    gLinkChildKeatonMaskDL, gLinkChildSkullMaskDL, gLinkChildSpookyMaskDL, gLinkChildBunnyHoodDL,
    gLinkChildGoronMaskDL,  gLinkChildZoraMaskDL,  gLinkChildGerudoMaskDL, gLinkChildMaskOfTruthDL,
};

static Vec3s D_80854864 = { 0, 0, 0 };

void Player_DrawGameplay(PlayState* play, Player* this, s32 lod, Gfx* cullDList,
                         OverrideLimbDrawOpa overrideLimbDraw) {
    static s32 D_8085486C = 255;

    OPEN_DISPS(play->state.gfxCtx);

    gSPSegment(POLY_OPA_DISP++, 0x0C, cullDList);
    gSPSegment(POLY_XLU_DISP++, 0x0C, cullDList);

    func_8008F470(play, this->skelAnime.skeleton, this->skelAnime.jointTable, this->skelAnime.dListCount, lod,
                  this->currentTunic, this->currentBoots, this->actor.shape.face, overrideLimbDraw, func_80090D20,
                  this);

    if ((overrideLimbDraw == func_80090014) && (this->currentMask != PLAYER_MASK_NONE)) {
        // Fixes a bug in vanilla where ice traps are rendered extremely large while wearing a bunny hood
        if (CVarGetInteger("gFixIceTrapWithBunnyHood", 1)) Matrix_Push();
        Mtx* sp70 = Graph_Alloc(play->state.gfxCtx, 2 * sizeof(Mtx));

        if (this->currentMask == PLAYER_MASK_BUNNY) {
            Vec3s sp68;

            FrameInterpolation_RecordActorPosRotMatrix();
            gSPSegment(POLY_OPA_DISP++, 0x0B, sp70);

            sp68.x = D_80858AC8.unk_02 + 0x3E2;
            sp68.y = D_80858AC8.unk_04 + 0xDBE;
            sp68.z = D_80858AC8.unk_00 - 0x348A;
            Matrix_SetTranslateRotateYXZ(97.0f, -1203.0f - CVarGetFloat("gCosmetics.BunnyHood_EarLength", 0.0f), -240.0f - CVarGetFloat("gCosmetics.BunnyHood_EarSpread", 0.0f), &sp68);
            MATRIX_TOMTX(sp70++);

            sp68.x = D_80858AC8.unk_02 - 0x3E2;
            sp68.y = -0xDBE - D_80858AC8.unk_04;
            sp68.z = D_80858AC8.unk_00 - 0x348A;
            Matrix_SetTranslateRotateYXZ(97.0f, -1203.0f - CVarGetFloat("gCosmetics.BunnyHood_EarLength", 0.0f), 240.0f + CVarGetFloat("gCosmetics.BunnyHood_EarSpread", 0.0f), &sp68);
            MATRIX_TOMTX(sp70);
        }

        gSPDisplayList(POLY_OPA_DISP++, sMaskDlists[this->currentMask - 1]);
        if (CVarGetInteger("gFixIceTrapWithBunnyHood", 1)) Matrix_Pop();
    }

    if ((this->currentBoots == PLAYER_BOOTS_HOVER ||
         (CVarGetInteger("gIvanCoopModeEnabled", 0) && this->ivanFloating)) &&
        !(this->actor.bgCheckFlags & 1) &&
        !(this->stateFlags1 & PLAYER_STATE1_RIDING_HORSE) && (this->hoverBootsTimer != 0)) {
        s32 sp5C;
        s32 hoverBootsTimer = this->hoverBootsTimer;

        if (this->hoverBootsTimer < 19) {
            if (hoverBootsTimer >= 15) {
                D_8085486C = (19 - hoverBootsTimer) * 51.0f;
            } else if (hoverBootsTimer < 19) {
                sp5C = hoverBootsTimer;

                if (sp5C > 9) {
                    sp5C = 9;
                }

                D_8085486C = (-sp5C * 4) + 36;
                D_8085486C = D_8085486C * D_8085486C;
                D_8085486C = (s32)((Math_CosS(D_8085486C) * 100.0f) + 100.0f) + 55.0f;
                D_8085486C = D_8085486C * (sp5C * (1.0f / 9.0f));
            }

            FrameInterpolation_RecordActorPosRotMatrix();
            Matrix_SetTranslateRotateYXZ(this->actor.world.pos.x, this->actor.world.pos.y + 2.0f,
                                         this->actor.world.pos.z, &D_80854864);
            Matrix_Scale(4.0f, 4.0f, 4.0f, MTXMODE_APPLY);

            gSPMatrix(POLY_XLU_DISP++, MATRIX_NEWMTX(play->state.gfxCtx),
                      G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
            gSPSegment(POLY_XLU_DISP++, 0x08,
                       Gfx_TwoTexScroll(play->state.gfxCtx, 0, 0, 0, 16, 32, 1, 0,
                                        (play->gameplayFrames * -15) % 128, 16, 32));
            gDPSetPrimColor(POLY_XLU_DISP++, 0x80, 0x80, 255, 255, 255, D_8085486C);
            gDPSetEnvColor(POLY_XLU_DISP++, 120, 90, 30, 128);
            gSPDisplayList(POLY_XLU_DISP++, gHoverBootsCircleDL);
        }
    }

    CLOSE_DISPS(play->state.gfxCtx);
}

void Player_Draw(Actor* thisx, PlayState* play2) {
    PlayState* play = play2;
    Player* this = (Player*)thisx;

    Vec3f pos;
    Vec3s rot;
    f32 scale;

    // OTRTODO: This is crashing randomly, so its temporarily been disabled
    // return;

    if (LINK_AGE_IN_YEARS == YEARS_CHILD) {
        pos.x = 2.0f;
        pos.y = -130.0f;
        pos.z = -150.0f;
        scale = 0.046f;
    } else if (CUR_EQUIP_VALUE(EQUIP_SWORD) != 2) {
        pos.x = 25.0f;
        pos.y = -228.0f;
        pos.z = 60.0f;
        scale = 0.056f;
    } else {
        pos.x = 20.0f;
        pos.y = -180.0f;
        pos.z = -40.0f;
        scale = 0.047f;
    }

    rot.y = 32300;
    rot.x = rot.z = 0;

    OPEN_DISPS(play->state.gfxCtx);

    if (!(this->stateFlags2 & PLAYER_STATE2_DISABLE_DRAW)) {
        OverrideLimbDrawOpa overrideLimbDraw = func_80090014;
        s32 lod;
        s32 pad;

        if ((this->csMode != 0) || (Player_IsUnfriendlyZTargeting(this) && 0) ||
            (this->actor.projectedPos.z < 160.0f)) {
            lod = 0;
        } else {
            lod = 1;
        }

        if (CVarGetInteger("gDisableLOD", 0) != 0) {
            lod = 0;
        }

        func_80093C80(play);
        Gfx_SetupDL_25Xlu(play->state.gfxCtx);

        if (this->invincibilityTimer > 0) {
            this->damageFlashTimer += CLAMP(50 - this->invincibilityTimer, 8, 40);
            POLY_OPA_DISP = Gfx_SetFog2(POLY_OPA_DISP, 255, 0, 0, 0, 0,
                                        4000 - (s32)(Math_CosS(this->damageFlashTimer * 256) * 2000.0f));
        }

        func_8002EBCC(&this->actor, play, 0);
        func_8002ED80(&this->actor, play, 0);

        if (this->attentionMode != 0) {
            Vec3f projectedHeadPos;

            SkinMatrix_Vec3fMtxFMultXYZ(&play->viewProjectionMtxF, &this->actor.focus.pos, &projectedHeadPos);
            if (projectedHeadPos.z < -4.0f) {
                overrideLimbDraw = func_800902F0;
            }
        } else if (this->stateFlags2 & PLAYER_STATE2_INSIDE_CRAWLSPACE) {
            if (this->actor.projectedPos.z < 0.0f) {
                overrideLimbDraw = func_80090440;
            }
        }

        if (this->stateFlags2 & PLAYER_STATE2_ENABLE_REFLECTION) {
            f32 sp78 = ((u16)(play->gameplayFrames * 600) * M_PI) / 0x8000;
            f32 sp74 = ((u16)(play->gameplayFrames * 1000) * M_PI) / 0x8000;

            Matrix_Push();
            this->actor.scale.y = -this->actor.scale.y;
            Matrix_SetTranslateRotateYXZ(
                this->actor.world.pos.x,
                (this->actor.floorHeight + (this->actor.floorHeight - this->actor.world.pos.y)) +
                    (this->actor.shape.yOffset * this->actor.scale.y),
                this->actor.world.pos.z, &this->actor.shape.rot);
            Matrix_Scale(this->actor.scale.x, this->actor.scale.y, this->actor.scale.z, MTXMODE_APPLY);
            Matrix_RotateX(sp78, MTXMODE_APPLY);
            Matrix_RotateY(sp74, MTXMODE_APPLY);
            Matrix_Scale(1.1f, 0.95f, 1.05f, MTXMODE_APPLY);
            Matrix_RotateY(-sp74, MTXMODE_APPLY);
            Matrix_RotateX(-sp78, MTXMODE_APPLY);
            Player_DrawGameplay(play, this, lod, gCullFrontDList, overrideLimbDraw);
            this->actor.scale.y = -this->actor.scale.y;
            Matrix_Pop();
        }

        gSPClearGeometryMode(POLY_OPA_DISP++, G_CULL_BOTH);
        gSPClearGeometryMode(POLY_XLU_DISP++, G_CULL_BOTH);

        Player_DrawGameplay(play, this, lod, gCullBackDList, overrideLimbDraw);

        if (this->invincibilityTimer > 0) {
            POLY_OPA_DISP = Play_SetFog(play, POLY_OPA_DISP);
        }

        if (this->stateFlags2 & PLAYER_STATE2_FROZEN_IN_ICE) {
            f32 scale = (this->genericVar >> 1) * 22.0f;

            gSPSegment(POLY_XLU_DISP++, 0x08,
                       Gfx_TwoTexScroll(play->state.gfxCtx, 0, 0, (0 - play->gameplayFrames) % 128, 32, 32, 1,
                                        0, (play->gameplayFrames * -2) % 128, 32, 32));

            Matrix_Scale(scale, scale, scale, MTXMODE_APPLY);
            gSPMatrix(POLY_XLU_DISP++, MATRIX_NEWMTX(play->state.gfxCtx),
                      G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
            gDPSetEnvColor(POLY_XLU_DISP++, 0, 50, 100, 255);
            gSPDisplayList(POLY_XLU_DISP++, gEffIceFragment3DL);
        }

        if (this->giDrawIdPlusOne > 0) {
            Player_DrawGetItem(play, this);
        }
    }

    CLOSE_DISPS(play->state.gfxCtx);
}

void Player_Destroy(Actor* thisx, PlayState* play) {
    Player* this = (Player*)thisx;

    Effect_Delete(play, this->meleeWeaponEffectIndex);

    Collider_DestroyCylinder(play, &this->cylinder);
    Collider_DestroyQuad(play, &this->meleeWeaponQuads[0]);
    Collider_DestroyQuad(play, &this->meleeWeaponQuads[1]);
    Collider_DestroyQuad(play, &this->shieldQuad);

    func_800876C8(play);

    gSaveContext.linkAge = play->linkAgeOnLoad;

    ResourceMgr_UnregisterSkeleton(&this->skelAnime);
    ResourceMgr_UnregisterSkeleton(&this->skelAnimeUpper);
}

//first person manipulate player actor
s16 func_8084ABD8(PlayState* play, Player* this, s32 arg2, s16 arg3) {
    s32 temp1;
    s16 temp2;
    s16 temp3;
    bool gInvertAimingXAxis = (CVarGetInteger("gInvertAimingXAxis", 0) && !CVarGetInteger("gMirroredWorld", 0)) || (!CVarGetInteger("gInvertAimingXAxis", 0) && CVarGetInteger("gMirroredWorld", 0));

    if (!Actor_PlayerIsAimingReadyFpsItem(this) && !Player_IsAimingReadyBoomerang(this) && (arg2 == 0) && !CVarGetInteger("gDisableAutoCenterViewFirstPerson", 0)) {
        temp2 = sControlInput->rel.stick_y * 240.0f * (CVarGetInteger("gInvertAimingYAxis", 1) ? 1 : -1); // Sensitivity not applied here because higher than default sensitivies will allow the camera to escape the autocentering, and glitch out massively
        Math_SmoothStepToS(&this->actor.focus.rot.x, temp2, 14, 4000, 30);

        temp2 = sControlInput->rel.stick_x * -16.0f * (gInvertAimingXAxis ? -1 : 1) * (CVarGetFloat("gFirstPersonCameraSensitivityX", 1.0f));
        temp2 = CLAMP(temp2, -3000, 3000);
        this->actor.focus.rot.y += temp2;
    } else {
        temp1 = (this->stateFlags1 & PLAYER_STATE1_RIDING_HORSE) ? 3500 : 14000;
        temp3 = ((sControlInput->rel.stick_y >= 0) ? 1 : -1) *
                (s32)((1.0f - Math_CosS(sControlInput->rel.stick_y * 200)) * 1500.0f *
                        (CVarGetInteger("gInvertAimingYAxis", 1) ? 1 : -1)) * (CVarGetFloat("gFirstPersonCameraSensitivityY", 1.0f));
        this->actor.focus.rot.x += temp3;

        if (fabsf(sControlInput->cur.gyro_x) > 0.01f) {
            this->actor.focus.rot.x -= (sControlInput->cur.gyro_x) * 750.0f;
        }

        if (fabsf(sControlInput->cur.right_stick_y) > 15.0f && CVarGetInteger("gRightStickAiming", 0) != 0) {
            this->actor.focus.rot.x -=
                (sControlInput->cur.right_stick_y) * 10.0f * (CVarGetInteger("gInvertAimingYAxis", 1) ? -1 : 1) * (CVarGetFloat("gFirstPersonCameraSensitivityY", 1.0f));
        }

        this->actor.focus.rot.x = CLAMP(this->actor.focus.rot.x, -temp1, temp1);

        temp1 = 19114;
        temp2 = this->actor.focus.rot.y - this->actor.shape.rot.y;
        temp3 = ((sControlInput->rel.stick_x >= 0) ? 1 : -1) *
                (s32)((1.0f - Math_CosS(sControlInput->rel.stick_x * 200)) * -1500.0f *
                        (gInvertAimingXAxis ? -1 : 1)) * (CVarGetFloat("gFirstPersonCameraSensitivityX", 1.0f));
        temp2 += temp3;

        this->actor.focus.rot.y = CLAMP(temp2, -temp1, temp1) + this->actor.shape.rot.y;

        if (fabsf(sControlInput->cur.gyro_y) > 0.01f) {
            this->actor.focus.rot.y += (sControlInput->cur.gyro_y) * 750.0f * (CVarGetInteger("gMirroredWorld", 0) ? -1 : 1);
        }

        if (fabsf(sControlInput->cur.right_stick_x) > 15.0f && CVarGetInteger("gRightStickAiming", 0) != 0) {
            this->actor.focus.rot.y +=
                (sControlInput->cur.right_stick_x) * 10.0f * (gInvertAimingXAxis ? 1 : -1) * (CVarGetFloat("gFirstPersonCameraSensitivityX", 1.0f));
        }
    }

    this->lookFlags |= 2;
    return Player_UpdateLookAngles(this, (play->shootingGalleryStatus != 0) || Actor_PlayerIsAimingReadyFpsItem(this) || Player_IsAimingReadyBoomerang(this)) -
           arg3;
}

void Player_UpdateSwimMovement(Player* this, f32* arg1, f32 arg2, s16 arg3) {
    f32 temp1;
    f32 temp2;

    temp1 = this->skelAnime.curFrame - 10.0f;

    temp2 = (R_RUN_SPEED_LIMIT / 100.0f) * 0.8f;
    if (*arg1 > temp2) {
        *arg1 = temp2;
    }

    if ((0.0f < temp1) && (temp1 < 10.0f)) {
        temp1 *= 6.0f;
    } else {
        temp1 = 0.0f;
        arg2 = 0.0f;
    }

    Math_AsymStepToF(arg1, arg2 * 0.8f, temp1, (fabsf(*arg1) * 0.02f) + 0.05f);
    Math_ScaledStepToS(&this->currentYaw, arg3, 1600);
}

void func_8084B000(Player* this) {
    f32 phi_f18;
    f32 phi_f16;
    f32 phi_f14;
    f32 yDistToWater;

    phi_f14 = -5.0f;

    phi_f16 = this->ageProperties->unk_28;
    if (this->actor.velocity.y < 0.0f) {
        phi_f16 += 1.0f;
    }

    if (this->actor.yDistToWater < phi_f16) {
        if (this->actor.velocity.y <= 0.0f) {
            phi_f16 = 0.0f;
        } else {
            phi_f16 = this->actor.velocity.y * 0.5f;
        }
        phi_f18 = -0.1f - phi_f16;
    } else {
        if (!(this->stateFlags1 & PLAYER_STATE1_IN_DEATH_CUTSCENE) && (this->currentBoots == PLAYER_BOOTS_IRON) &&
            (this->actor.velocity.y >= -3.0f)) {
            phi_f18 = -0.2f;
        } else {
            phi_f14 = 2.0f;
            if (this->actor.velocity.y >= 0.0f) {
                phi_f16 = 0.0f;
            } else {
                phi_f16 = this->actor.velocity.y * -0.3f;
            }
            phi_f18 = phi_f16 + 0.1f;
        }

        yDistToWater = this->actor.yDistToWater;
        if (yDistToWater > 100.0f) {
            this->stateFlags2 |= PLAYER_STATE2_DIVING;
        }
    }

    this->actor.velocity.y += phi_f18;

    if (((this->actor.velocity.y - phi_f14) * phi_f18) > 0) {
        this->actor.velocity.y = phi_f14;
    }

    this->actor.gravity = 0.0f;
}

void func_8084B158(PlayState* play, Player* this, Input* input, f32 arg3) {
    f32 temp;

    if ((input != NULL) && CHECK_BTN_ANY(input->press.button, BTN_A | BTN_B)) {
        temp = 1.0f;
    } else {
        temp = 0.5f;
    }

    temp *= arg3;

    if (temp < 1.0f) {
        temp = 1.0f;
    }

    this->skelAnime.playSpeed = temp;
    LinkAnimation_Update(play, &this->skelAnime);
}

void Player_FirstPersonAiming(Player* this, PlayState* play) {
    if (this->stateFlags1 & PLAYER_STATE1_SWIMMING) {
        func_8084B000(this);
        Player_UpdateSwimMovement(this, &this->linearVelocity, 0, this->actor.shape.rot.y);
    } else {
        Player_StepLinearVelocityToZero(this);
    }

    if ((this->attentionMode == 2) && (Actor_PlayerIsAimingFpsItem(this) || Player_IsAimingBoomerang(this))) {
        Player_SetupCurrentUpperAction(this, play);
    }

    u16 buttonsToCheck = BTN_A | BTN_B | BTN_R | BTN_CUP | BTN_CLEFT | BTN_CRIGHT | BTN_CDOWN;
    if (CVarGetInteger("gDpadEquips", 0) != 0) {
        buttonsToCheck |= BTN_DUP | BTN_DDOWN | BTN_DLEFT | BTN_DRIGHT;
    }
    if ((this->csMode != 0) || (this->attentionMode == 0) || (this->attentionMode >= 4) || Player_SetupStartUnfriendlyZTargeting(this) ||
        (this->targetActor != NULL) || !Player_SetupCameraMode(play, this) ||
        (((this->attentionMode == 2) && (CHECK_BTN_ANY(sControlInput->press.button, BTN_A | BTN_B | BTN_R) ||
                                         Player_IsFriendlyZTargeting(this) || (!Actor_PlayerIsAimingReadyFpsItem(this) && !Player_IsAimingReadyBoomerang(this)))) ||
         ((this->attentionMode == 1) && CHECK_BTN_ANY(sControlInput->press.button, buttonsToCheck)))) {
        Player_ClearLookAndAttention(this, play);
        func_80078884(NA_SE_SY_CAMERA_ZOOM_UP);
    } else if ((DECR(this->genericTimer) == 0) || (this->attentionMode != 2)) {
        if (Player_IsShootingHookshot(this)) {
            this->lookFlags |= 0x43;
        } else {
            this->actor.shape.rot.y = func_8084ABD8(play, this, 0, 0);
        }
    }

    this->currentYaw = this->actor.shape.rot.y;
}

s32 func_8084B3CC(PlayState* play, Player* this) {
    if (play->shootingGalleryStatus != 0) {
        Player_ResetAttributesAndHeldActor(play, this);
        Player_SetActionFunc(play, this, Player_ShootingGalleryPlay, 0);

        if (!Actor_PlayerIsAimingFpsItem(this) || Player_HoldsHookshot(this)) {
            s32 projectileItemToUse = ITEM_BOW;
            if(CVarGetInteger("gBowSlingShotAmmoFix", 0)){
                projectileItemToUse = LINK_IS_ADULT ? ITEM_BOW : ITEM_SLINGSHOT;
            }

            Player_UseItem(play, this, projectileItemToUse);
        }

        this->stateFlags1 |= PLAYER_STATE1_IN_FIRST_PERSON_MODE;
        Player_PlayAnimOnce(play, this, Player_GetStandingStillAnim(this));
        Player_StopMovement(this);
        Player_ResetLookAngles(this);
        return 1;
    }

    return 0;
}

void func_8084B498(Player* this) {
    this->itemAction =
        (INV_CONTENT(ITEM_OCARINA_FAIRY) == ITEM_OCARINA_FAIRY) ? PLAYER_IA_OCARINA_FAIRY : PLAYER_IA_OCARINA_TIME;
}

s32 func_8084B4D4(PlayState* play, Player* this) {
    if (this->stateFlags3 & PLAYER_STATE3_FORCE_PULL_OCARINA) {
        this->stateFlags3 &= ~PLAYER_STATE3_FORCE_PULL_OCARINA;
        func_8084B498(this);
        this->attentionMode = 4;
        Player_SetupItemCutsceneOrFirstPerson(this, play);
        return 1;
    }

    return 0;
}

void Player_TalkWithActor(Player* this, PlayState* play) {
    this->stateFlags2 |= PLAYER_STATE2_DISABLE_MOVE_ROTATION_WHILE_Z_TARGETING;
    Player_SetupCurrentUpperAction(this, play);

    if (Message_GetState(&play->msgCtx) == TEXT_STATE_CLOSING) {
        this->actor.flags &= ~ACTOR_FLAG_PLAYER_TALKED_TO;

        if (!CHECK_FLAG_ALL(this->talkActor->flags, ACTOR_FLAG_TARGETABLE | ACTOR_FLAG_HOSTILE)) {
            this->stateFlags2 &= ~PLAYER_STATE2_USING_SWITCH_Z_TARGETING;
        }

        func_8005B1A4(Play_GetCamera(play, 0));

        if (!func_8084B4D4(play, this) && !func_8084B3CC(play, this) && !Player_SetupCutscene(play, this)) {
            if ((this->talkActor != this->interactRangeActor) || !Player_SetupGetItemOrHoldBehavior(this, play)) {
                if (this->stateFlags1 & PLAYER_STATE1_RIDING_HORSE) {
                    s32 sp24 = this->genericTimer;
                    Player_SetupRideHorse(play, this);
                    this->genericTimer = sp24;
                } else if (Player_IsSwimming(this)) {
                    Player_SetupSwimIdle(play, this);
                } else {
                    Player_SetupStandingStillMorph(this, play);
                }
            }
        }

        this->endTalkTimer = 10;
        return;
    }

    if (this->stateFlags1 & PLAYER_STATE1_RIDING_HORSE) {
        Player_RideHorse(this, play);
    } else if (Player_IsSwimming(this)) {
        Player_UpdateSwimIdle(this, play);
    } else if (!Player_IsUnfriendlyZTargeting(this) && LinkAnimation_Update(play, &this->skelAnime)) {
        if (this->skelAnime.moveFlags != 0) {
            Player_EndAnimMovement(this);
            if ((this->talkActor->category == ACTORCAT_NPC) &&
                (this->heldItemAction != PLAYER_IA_FISHING_POLE)) {
                Player_PlayAnimOnceSlowed(play, this, &gPlayerAnim_link_normal_talk_free);
            } else {
                Player_PlayAnimLoop(play, this, Player_GetStandingStillAnim(this));
            }
        } else {
            Player_PlayAnimLoopSlowed(play, this, &gPlayerAnim_link_normal_talk_free_wait);
        }
    }

    if (this->targetActor != NULL) {
        this->currentYaw = this->actor.shape.rot.y = Player_LookAtTargetActor(this, 0);
    }
}

void Player_GrabPushPullWall(Player* this, PlayState* play) {
    f32 sp34;
    s16 sp32;
    s32 temp;

    this->stateFlags2 |= PLAYER_STATE2_CAN_GRAB_PUSH_PULL_WALL | PLAYER_STATE2_ALWAYS_DISABLE_MOVE_ROTATION |
                         PLAYER_STATE2_ENABLE_PUSH_PULL_CAM;
    func_8083F524(play, this);

    if (LinkAnimation_Update(play, &this->skelAnime)) {
        if (!func_8083F9D0(play, this)) {
            Player_GetTargetVelocityAndYaw(this, &sp34, &sp32, 0.0f, play);
            temp = func_8083FFB8(this, &sp34, &sp32);
            if (temp > 0) {
                func_8083FAB8(this, play);
            } else if (temp < 0) {
                func_8083FB14(this, play);
            }
        }
    }
}

void func_8084B840(PlayState* play, Player* this, f32 arg2) {
    if (this->actor.wallBgId != BGCHECK_SCENE) {
        DynaPolyActor* dynaPolyActor = DynaPoly_GetActor(&play->colCtx, this->actor.wallBgId);

        if (dynaPolyActor != NULL) {
            func_8002DFA4(dynaPolyActor, arg2, this->actor.world.rot.y);
        }
    }
}

static PlayerAnimSfxEntry D_80854870[] = {
    { NA_SE_PL_SLIP, 0x1003 },
    { NA_SE_PL_SLIP, -0x1015 },
};

void Player_PushWall(Player* this, PlayState* play) {
    f32 sp34;
    s16 sp32;
    s32 temp;

    this->stateFlags2 |= PLAYER_STATE2_CAN_GRAB_PUSH_PULL_WALL | PLAYER_STATE2_ALWAYS_DISABLE_MOVE_ROTATION |
                         PLAYER_STATE2_ENABLE_PUSH_PULL_CAM;

    if (Player_LoopAnimContinuously(play, this, &gPlayerAnim_link_normal_pushing)) {
        this->genericTimer = 1;
    } else if (this->genericTimer == 0) {
        if (LinkAnimation_OnFrame(&this->skelAnime, 11.0f)) {
            Player_PlayVoiceSfxForAge(this, NA_SE_VO_LI_PUSH);
        }
    }

    Player_PlayAnimSfx(this, D_80854870);
    func_8083F524(play, this);

    if (!func_8083F9D0(play, this)) {
        Player_GetTargetVelocityAndYaw(this, &sp34, &sp32, 0.0f, play);
        temp = func_8083FFB8(this, &sp34, &sp32);
        if (temp < 0) {
            func_8083FB14(this, play);
        } else if (temp == 0) {
            func_8083F72C(this, &gPlayerAnim_link_normal_push_end, play);
        } else {
            this->stateFlags2 |= PLAYER_STATE2_MOVING_PUSH_PULL_WALL;
        }
    }

    if (this->stateFlags2 & PLAYER_STATE2_MOVING_PUSH_PULL_WALL) {
        func_8084B840(play, this, 2.0f);
        this->linearVelocity = 2.0f;
    }
}

static PlayerAnimSfxEntry D_80854878[] = {
    { NA_SE_PL_SLIP, 0x1004 },
    { NA_SE_PL_SLIP, -0x1018 },
};

static Vec3f D_80854880 = { 0.0f, 26.0f, -40.0f };

void Player_PullWall(Player* this, PlayState* play) {
    LinkAnimationHeader* anim;
    f32 sp70;
    s16 sp6E;
    s32 temp1;
    Vec3f sp5C;
    f32 temp2;
    CollisionPoly* sp54;
    s32 sp50;
    Vec3f sp44;
    Vec3f sp38;

    anim = GET_PLAYER_ANIM(PLAYER_ANIMGROUP_PULL_OBJECT, this->modelAnimType);
    this->stateFlags2 |= PLAYER_STATE2_CAN_GRAB_PUSH_PULL_WALL | PLAYER_STATE2_ALWAYS_DISABLE_MOVE_ROTATION |
                         PLAYER_STATE2_ENABLE_PUSH_PULL_CAM;

    if (Player_LoopAnimContinuously(play, this, anim)) {
        this->genericTimer = 1;
    } else {
        if (this->genericTimer == 0) {
            if (LinkAnimation_OnFrame(&this->skelAnime, 11.0f)) {
                Player_PlayVoiceSfxForAge(this, NA_SE_VO_LI_PUSH);
            }
        } else {
            Player_PlayAnimSfx(this, D_80854878);
        }
    }

    func_8083F524(play, this);

    if (!func_8083F9D0(play, this)) {
        Player_GetTargetVelocityAndYaw(this, &sp70, &sp6E, 0.0f, play);
        temp1 = func_8083FFB8(this, &sp70, &sp6E);
        if (temp1 > 0) {
            func_8083FAB8(this, play);
        } else if (temp1 == 0) {
            func_8083F72C(this, GET_PLAYER_ANIM(PLAYER_ANIMGROUP_PUSH_OBJECT, this->modelAnimType), play);
        } else {
            this->stateFlags2 |= PLAYER_STATE2_MOVING_PUSH_PULL_WALL;
        }
    }

    if (this->stateFlags2 & PLAYER_STATE2_MOVING_PUSH_PULL_WALL) {
        temp2 = Player_RaycastFloorWithOffset2(play, this, &D_80854880, &sp5C) - this->actor.world.pos.y;
        if (fabsf(temp2) < 20.0f) {
            sp44.x = this->actor.world.pos.x;
            sp44.z = this->actor.world.pos.z;
            sp44.y = sp5C.y;
            if (!BgCheck_EntityLineTest1(&play->colCtx, &sp44, &sp5C, &sp38, &sp54, true, false, false, true,
                                         &sp50)) {
                func_8084B840(play, this, -2.0f);
                return;
            }
        }
        this->stateFlags2 &= ~PLAYER_STATE2_MOVING_PUSH_PULL_WALL;
    }
}

void Player_GrabLedge(Player* this, PlayState* play) {
    f32 sp3C;
    s16 sp3A;
    LinkAnimationHeader* anim;
    f32 temp;

    this->stateFlags2 |= PLAYER_STATE2_ALWAYS_DISABLE_MOVE_ROTATION;

    if (LinkAnimation_Update(play, &this->skelAnime)) {
        // clang-format off
        anim = (this->genericVar > 0) ? &gPlayerAnim_link_normal_fall_wait : GET_PLAYER_ANIM(PLAYER_ANIMGROUP_CLIMBING_IDLE, this->modelAnimType); Player_PlayAnimLoop(play, this, anim);
        // clang-format on
    } else if (this->genericVar == 0) {
        if (this->skelAnime.animation == &gPlayerAnim_link_normal_fall) {
            temp = 11.0f;
        } else {
            temp = 1.0f;
        }

        if (LinkAnimation_OnFrame(&this->skelAnime, temp)) {
            Player_PlayMoveSfx(this, NA_SE_PL_WALK_GROUND);
            if (this->skelAnime.animation == &gPlayerAnim_link_normal_fall) {
                this->genericVar = 1;
            } else {
                this->genericVar = -1;
            }
        }
    }

    Math_ScaledStepToS(&this->actor.shape.rot.y, this->currentYaw, 0x800);

    if (this->genericVar != 0) {
        Player_GetTargetVelocityAndYaw(this, &sp3C, &sp3A, 0.0f, play);
        if (this->analogStickInputs[this->inputFrameCounter] >= 0) {
            if (this->genericVar > 0) {
                anim = GET_PLAYER_ANIM(PLAYER_ANIMGROUP_KNOCKED_FROM_CLIMBING, this->modelAnimType);
            } else {
                anim = GET_PLAYER_ANIM(PLAYER_ANIMGROUP_CLIMBING, this->modelAnimType);
            }
            Player_SetupClimbOntoLedge(this, anim, play);
            return;
        }

        if (CHECK_BTN_ALL(sControlInput->cur.button, BTN_A) || (this->actor.shape.feetFloorFlags != 0)) {
            Player_SetLedgeGrabPosition(this);
            if (this->genericVar < 0) {
                this->linearVelocity = -0.8f;
            } else {
                this->linearVelocity = 0.8f;
            }
            Player_SetupFallFromLedge(this, play);
            this->stateFlags1 &= ~(PLAYER_STATE1_HANGING_FROM_LEDGE_SLIP | PLAYER_STATE1_CLIMBING_ONTO_LEDGE);
        }
    }
}

void Player_ClimbOntoLedge(Player* this, PlayState* play) {
    this->stateFlags2 |= PLAYER_STATE2_ALWAYS_DISABLE_MOVE_ROTATION;

    if (LinkAnimation_Update(play, &this->skelAnime)) {
        Player_UpdateAnimMovement(this, 1);
        Player_SetupStandingStillNoMorph(this, play);
        return;
    }

    if (LinkAnimation_OnFrame(&this->skelAnime, this->skelAnime.endFrame - 6.0f)) {
        Player_PlayLandingSfx(this);
    } else if (LinkAnimation_OnFrame(&this->skelAnime, this->skelAnime.endFrame - 34.0f)) {
        this->stateFlags1 &= ~(PLAYER_STATE1_HANGING_FROM_LEDGE_SLIP | PLAYER_STATE1_CLIMBING_ONTO_LEDGE);
        func_8002F7DC(&this->actor, NA_SE_PL_CLIMB_CLIFF);
        Player_PlayVoiceSfxForAge(this, NA_SE_VO_LI_CLIMB_END);
    }
}

void func_8084BEE4(Player* this) {
    func_8002F7DC(&this->actor, (this->genericVar != 0) ? NA_SE_PL_WALK_WALL : NA_SE_PL_WALK_LADDER);
}

void Player_ClimbingWallOrDownLedge(Player* this, PlayState* play) {
    static Vec3f D_8085488C = { 0.0f, 0.0f, 26.0f };
    s32 sp84;
    s32 sp80;
    f32 phi_f0;
    f32 phi_f2;
    Vec3f sp6C;
    s32 sp68;
    Vec3f sp5C;
    f32 temp_f0;
    LinkAnimationHeader* anim1;
    LinkAnimationHeader* anim2;

    sp84 = sControlInput->rel.stick_y;
    sp80 = sControlInput->rel.stick_x;

    this->fallStartHeight = this->actor.world.pos.y;
    this->stateFlags2 |= PLAYER_STATE2_ALWAYS_DISABLE_MOVE_ROTATION;

    if ((this->genericVar != 0) && (ABS(sp84) < ABS(sp80))) {
        phi_f0 = ABS(sp80) * 0.0325f;
        sp84 = 0;
    } else {
        phi_f0 = ABS(sp84) * 0.05f;
        sp80 = 0;
    }

    if (phi_f0 < 1.0f) {
        phi_f0 = 1.0f;
    } else if (phi_f0 > 3.35f) {
        phi_f0 = 3.35f;
    }

    if (this->skelAnime.playSpeed >= 0.0f) {
        phi_f2 = 1.0f;
    } else {
        phi_f2 = -1.0f;
    }

    if (CVarGetInteger("gChaosClimbSpeed", 0) != 0) {
        this->skelAnime.playSpeed = (phi_f2 * phi_f0) * (1.0f - CVarGetInteger("gChaosClimbSpeed", 0) * 0.1f);
    }
    else {
        this->skelAnime.playSpeed = phi_f2 * phi_f0 + phi_f2 * CVarGetInteger("gClimbSpeed", 0);
    }

    if (this->genericTimer >= 0) {
        if ((this->actor.wallPoly != NULL) && (this->actor.wallBgId != BGCHECK_SCENE)) {
            DynaPolyActor* wallPolyActor = DynaPoly_GetActor(&play->colCtx, this->actor.wallBgId);
            if (wallPolyActor != NULL) {
                Math_Vec3f_Diff(&wallPolyActor->actor.world.pos, &wallPolyActor->actor.prevPos, &sp6C);
                Math_Vec3f_Sum(&this->actor.world.pos, &sp6C, &this->actor.world.pos);
            }
        }

        Actor_UpdateBgCheckInfo(play, &this->actor, 26.0f, 6.0f, this->ageProperties->unk_00, 7);
        func_8083F360(play, this, 26.0f, this->ageProperties->unk_3C, 50.0f, -20.0f);
    }

    if ((this->genericTimer < 0) || !func_8083FBC0(this, play)) {
        if (LinkAnimation_Update(play, &this->skelAnime) != 0) {
            if (this->genericTimer < 0) {
                this->genericTimer = ABS(this->genericTimer) & 1;
                return;
            }

            if (sp84 != 0) {
                sp68 = this->genericVar + this->genericTimer;

                if (sp84 > 0) {
                    D_8085488C.y = this->ageProperties->unk_40;
                    temp_f0 = Player_RaycastFloorWithOffset2(play, this, &D_8085488C, &sp5C);

                    if (this->actor.world.pos.y < temp_f0) {
                        if (this->genericVar != 0) {
                            this->actor.world.pos.y = temp_f0;
                            this->stateFlags1 &= ~PLAYER_STATE1_CLIMBING;
                            Player_SetupGrabLedge(play, this, this->actor.wallPoly, this->ageProperties->unk_3C,
                                          &gPlayerAnim_link_normal_jump_climb_up_free);
                            this->currentYaw += 0x8000;
                            this->actor.shape.rot.y = this->currentYaw;
                            Player_SetupClimbOntoLedge(this, &gPlayerAnim_link_normal_jump_climb_up_free, play);
                            this->stateFlags1 |= PLAYER_STATE1_CLIMBING_ONTO_LEDGE;
                        } else {
                            func_8083F070(this, this->ageProperties->unk_CC[this->genericTimer], play);
                        }
                    } else {
                        this->skelAnime.prevTransl = this->ageProperties->unk_4A[sp68];
                        Player_PlayAnimOnce(play, this, this->ageProperties->unk_AC[sp68]);
                    }
                } else {
                    if ((this->actor.world.pos.y - this->actor.floorHeight) < 15.0f) {
                        if (this->genericVar != 0) {
                            func_8083FB7C(this, play);
                        } else {
                            if (this->genericTimer != 0) {
                                this->skelAnime.prevTransl = this->ageProperties->unk_44;
                            }
                            func_8083F070(this, this->ageProperties->unk_C4[this->genericTimer], play);
                            this->genericTimer = 1;
                        }
                    } else {
                        sp68 ^= 1;
                        this->skelAnime.prevTransl = this->ageProperties->unk_62[sp68];
                        anim1 = this->ageProperties->unk_AC[sp68];
                        LinkAnimation_Change(play, &this->skelAnime, anim1, -1.0f, Animation_GetLastFrame(anim1),
                                             0.0f, ANIMMODE_ONCE, 0.0f);
                    }
                }
                this->genericTimer ^= 1;
            } else {
                if ((this->genericVar != 0) && (sp80 != 0)) {
                    anim2 = this->ageProperties->unk_BC[this->genericTimer];

                    if (CVarGetInteger("gMirroredWorld", 0) ? (sp80 < 0) : (sp80 > 0)) {
                        this->skelAnime.prevTransl = this->ageProperties->unk_7A[this->genericTimer];
                        Player_PlayAnimOnce(play, this, anim2);
                    } else {
                        this->skelAnime.prevTransl = this->ageProperties->unk_86[this->genericTimer];
                        LinkAnimation_Change(play, &this->skelAnime, anim2, -1.0f, Animation_GetLastFrame(anim2),
                                             0.0f, ANIMMODE_ONCE, 0.0f);
                    }
                } else {
                    this->stateFlags2 |= PLAYER_STATE2_IDLE_WHILE_CLIMBING;
                }
            }

            return;
        }
    }

    if (this->genericTimer < 0) {
        if (((this->genericTimer == -2) &&
             (LinkAnimation_OnFrame(&this->skelAnime, 14.0f) || LinkAnimation_OnFrame(&this->skelAnime, 29.0f))) ||
            ((this->genericTimer == -4) &&
             (LinkAnimation_OnFrame(&this->skelAnime, 22.0f) || LinkAnimation_OnFrame(&this->skelAnime, 35.0f) ||
              LinkAnimation_OnFrame(&this->skelAnime, 49.0f) || LinkAnimation_OnFrame(&this->skelAnime, 55.0f)))) {
            func_8084BEE4(this);
        }
        return;
    }

    if (LinkAnimation_OnFrame(&this->skelAnime, (this->skelAnime.playSpeed > 0.0f) ? 20.0f : 0.0f)) {
        func_8084BEE4(this);
    }
}

static f32 D_80854898[] = { 10.0f, 20.0f };
static f32 D_808548A0[] = { 40.0f, 50.0f };

static PlayerAnimSfxEntry D_808548A8[] = {
    { NA_SE_PL_WALK_LADDER, 0x80A },
    { NA_SE_PL_WALK_LADDER, 0x814 },
    { NA_SE_PL_WALK_LADDER, -0x81E },
};

void Player_EndClimb(Player* this, PlayState* play) {
    s32 temp;
    f32* sp38;
    CollisionPoly* sp34;
    s32 sp30;
    Vec3f sp24;

    this->stateFlags2 |= PLAYER_STATE2_ALWAYS_DISABLE_MOVE_ROTATION;

    temp = Player_IsActionInterrupted(play, this, &this->skelAnime, 4.0f);

    if (temp == 0) {
        this->stateFlags1 &= ~PLAYER_STATE1_CLIMBING;
        return;
    }

    if ((temp > 0) || LinkAnimation_Update(play, &this->skelAnime)) {
        Player_SetupStandingStillNoMorph(this, play);
        this->stateFlags1 &= ~PLAYER_STATE1_CLIMBING;
        return;
    }

    sp38 = D_80854898;

    if (this->genericTimer != 0) {
        Player_PlayAnimSfx(this, D_808548A8);
        sp38 = D_808548A0;
    }

    if (LinkAnimation_OnFrame(&this->skelAnime, sp38[0]) || LinkAnimation_OnFrame(&this->skelAnime, sp38[1])) {
        sp24.x = this->actor.world.pos.x;
        sp24.y = this->actor.world.pos.y + 20.0f;
        sp24.z = this->actor.world.pos.z;
        if (BgCheck_EntityRaycastFloor3(&play->colCtx, &sp34, &sp30, &sp24) != 0.0f) {
            this->surfaceMaterial = func_80041F10(&play->colCtx, sp34, sp30);
            Player_PlayLandingSfx(this);
        }
    }
}

static PlayerAnimSfxEntry D_808548B4[] = {
    { 0, 0x3028 }, { 0, 0x3030 }, { 0, 0x3038 }, { 0, 0x3040 },  { 0, 0x3048 },
    { 0, 0x3050 }, { 0, 0x3058 }, { 0, 0x3060 }, { 0, -0x3068 },
};

void Player_InsideCrawlspace(Player* this, PlayState* play) {
    this->stateFlags2 |= PLAYER_STATE2_ALWAYS_DISABLE_MOVE_ROTATION;

    if (LinkAnimation_Update(play, &this->skelAnime)) {
        if (!(this->stateFlags1 & PLAYER_STATE1_EXITING_SCENE)) {
            if (this->skelAnime.moveFlags != 0) {
                this->skelAnime.moveFlags = 0;
                return;
            }

            if (!func_8083F570(this, play)) {
                this->linearVelocity = sControlInput->rel.stick_y * 0.03f;
            }
        }
        return;
    }

    Player_PlayAnimSfx(this, D_808548B4);
}

static PlayerAnimSfxEntry D_808548D8[] = {
    { 0, 0x300A }, { 0, 0x3012 }, { 0, 0x301A }, { 0, 0x3022 },  { 0, 0x3034 },
    { 0, 0x303C }, { 0, 0x3044 }, { 0, 0x304C }, { 0, -0x3054 },
};

void Player_ExitCrawlspace(Player* this, PlayState* play) {
    this->stateFlags2 |= PLAYER_STATE2_ALWAYS_DISABLE_MOVE_ROTATION;

    if (LinkAnimation_Update(play, &this->skelAnime)) {
        Player_SetupStandingStillNoMorph(this, play);
        this->stateFlags2 &= ~PLAYER_STATE2_INSIDE_CRAWLSPACE;
        return;
    }

    Player_PlayAnimSfx(this, D_808548D8);
}

static Vec3f D_808548FC[] = {
    { 40.0f, 0.0f, 0.0f },
    { -40.0f, 0.0f, 0.0f },
};

static Vec3f D_80854914[] = {
    { 60.0f, 20.0f, 0.0f },
    { -60.0f, 20.0f, 0.0f },
};

static Vec3f D_8085492C[] = {
    { 60.0f, -20.0f, 0.0f },
    { -60.0f, -20.0f, 0.0f },
};

s32 func_8084C89C(PlayState* play, Player* this, s32 arg2, f32* arg3) {
    EnHorse* rideActor = (EnHorse*)this->rideActor;
    f32 sp50;
    f32 sp4C;
    Vec3f sp40;
    Vec3f sp34;
    CollisionPoly* sp30;
    s32 sp2C;

    sp50 = rideActor->actor.world.pos.y + 20.0f;
    sp4C = rideActor->actor.world.pos.y - 20.0f;

    *arg3 = Player_RaycastFloorWithOffset2(play, this, &D_808548FC[arg2], &sp40);

    return (sp4C < *arg3) && (*arg3 < sp50) &&
           !Player_WallLineTestWithOffset(play, this, &D_80854914[arg2], &sp30, &sp2C, &sp34) &&
           !Player_WallLineTestWithOffset(play, this, &D_8085492C[arg2], &sp30, &sp2C, &sp34);
}

s32 func_8084C9BC(Player* this, PlayState* play) {
    EnHorse* rideActor = (EnHorse*)this->rideActor;
    s32 sp38;
    f32 sp34;

    if (this->genericTimer < 0) {
        this->genericTimer = 99;
    } else {
        sp38 = (this->mountSide < 0) ? 0 : 1;
        if (!func_8084C89C(play, this, sp38, &sp34)) {
            sp38 ^= 1;
            if (!func_8084C89C(play, this, sp38, &sp34)) {
                return 0;
            } else {
                this->mountSide = -this->mountSide;
            }
        }

        if ((play->csCtx.state == CS_STATE_IDLE) && (play->transitionMode == 0) &&
            (EN_HORSE_CHECK_1(rideActor) || EN_HORSE_CHECK_4(rideActor))) {
            this->stateFlags2 |= PLAYER_STATE2_CAN_DISMOUNT_HORSE;

            if (EN_HORSE_CHECK_1(rideActor) ||
                (EN_HORSE_CHECK_4(rideActor) && CHECK_BTN_ALL(sControlInput->press.button, BTN_A))) {
                rideActor->actor.child = NULL;
                Player_SetActionFuncPreserveMoveFlags(play, this, Player_DismountHorse, 0);
                this->rideOffsetY = sp34 - rideActor->actor.world.pos.y;
                Player_PlayAnimOnce(play, this,
                              (this->mountSide < 0) ? &gPlayerAnim_link_uma_left_down
                                                    : &gPlayerAnim_link_uma_right_down);
                return 1;
            }
        }
    }

    return 0;
}

void func_8084CBF4(Player* this, f32 arg1, f32 arg2) {
    f32 temp;
    f32 dir;

    if ((this->rideOffsetY != 0.0f) && (arg2 <= this->skelAnime.curFrame)) {
        if (arg1 < fabsf(this->rideOffsetY)) {
            if (this->rideOffsetY >= 0.0f) {
                dir = 1;
            } else {
                dir = -1;
            }
            temp = dir * arg1;
        } else {
            temp = this->rideOffsetY;
        }
        this->actor.world.pos.y += temp;
        this->rideOffsetY -= temp;
    }
}

static LinkAnimationHeader* D_80854944[] = {
    &gPlayerAnim_link_uma_anim_stop,
    &gPlayerAnim_link_uma_anim_stand,
    &gPlayerAnim_link_uma_anim_walk,
    &gPlayerAnim_link_uma_anim_slowrun,
    &gPlayerAnim_link_uma_anim_fastrun,
    &gPlayerAnim_link_uma_anim_jump100,
    &gPlayerAnim_link_uma_anim_jump200,
    NULL,
    NULL,
};

static LinkAnimationHeader* D_80854968[] = {
    &gPlayerAnim_link_uma_anim_walk_muti,
    &gPlayerAnim_link_uma_anim_walk_muti,
    &gPlayerAnim_link_uma_anim_walk_muti,
    &gPlayerAnim_link_uma_anim_slowrun_muti,
    &gPlayerAnim_link_uma_anim_fastrun_muti,
    &gPlayerAnim_link_uma_anim_fastrun_muti,
    &gPlayerAnim_link_uma_anim_fastrun_muti,
    NULL,
    NULL,
};

static LinkAnimationHeader* D_8085498C[] = {
    &gPlayerAnim_link_uma_wait_3,
    &gPlayerAnim_link_uma_wait_1,
    &gPlayerAnim_link_uma_wait_2,
};

static u8 D_80854998[2][2] = {
    { 32, 58 },
    { 25, 42 },
};

static Vec3s D_8085499C = { -69, 7146, -266 };

static PlayerAnimSfxEntry D_808549A4[] = {
    { NA_SE_PL_CALM_HIT, 0x830 }, { NA_SE_PL_CALM_HIT, 0x83A },  { NA_SE_PL_CALM_HIT, 0x844 },
    { NA_SE_PL_CALM_PAT, 0x85C }, { NA_SE_PL_CALM_PAT, 0x86E },  { NA_SE_PL_CALM_PAT, 0x87E },
    { NA_SE_PL_CALM_PAT, 0x884 }, { NA_SE_PL_CALM_PAT, -0x888 },
};

void Player_RideHorse(Player* this, PlayState* play) {
    EnHorse* rideActor = (EnHorse*)this->rideActor;
    u8* arr;

    this->stateFlags2 |= PLAYER_STATE2_ALWAYS_DISABLE_MOVE_ROTATION;

    func_8084CBF4(this, 1.0f, 10.0f);

    if (this->genericTimer == 0) {
        if (LinkAnimation_Update(play, &this->skelAnime)) {
            this->skelAnime.animation = &gPlayerAnim_link_uma_wait_1;
            this->genericTimer = 99;
            return;
        }

        arr = D_80854998[(this->mountSide < 0) ? 0 : 1];

        if (LinkAnimation_OnFrame(&this->skelAnime, arr[0])) {
            func_8002F7DC(&this->actor, NA_SE_PL_CLIMB_CLIFF);
            return;
        }

        if (LinkAnimation_OnFrame(&this->skelAnime, arr[1])) {
            func_8002DE74(play, this);
            func_8002F7DC(&this->actor, NA_SE_PL_SIT_ON_HORSE);
            return;
        }

        return;
    }

    func_8002DE74(play, this);
    this->skelAnime.prevTransl = D_8085499C;

    if ((rideActor->animationIdx != this->genericTimer) &&
        ((rideActor->animationIdx >= 2) || (this->genericTimer >= 2))) {
        if ((this->genericTimer = rideActor->animationIdx) < 2) {
            f32 rand = Rand_ZeroOne();
            s32 temp = 0;

            this->genericTimer = 1;

            if (rand < 0.1f) {
                temp = 2;
            } else if (rand < 0.2f) {
                temp = 1;
            }
            Player_PlayAnimOnce(play, this, D_8085498C[temp]);
        } else {
            this->skelAnime.animation = D_80854944[this->genericTimer - 2];
            Animation_SetMorph(play, &this->skelAnime, 8.0f);
            if (this->genericTimer < 4) {
                Player_SetupHeldItemUpperActionFunc(play, this);
                this->genericVar = 0;
            }
        }
    }

    if (this->genericTimer == 1) {
        if ((D_808535E0 != 0) || Player_CheckActorTalkRequested(play)) {
            Player_PlayAnimOnce(play, this, &gPlayerAnim_link_uma_wait_3);
        } else if (LinkAnimation_Update(play, &this->skelAnime)) {
            this->genericTimer = 99;
        } else if (this->skelAnime.animation == &gPlayerAnim_link_uma_wait_1) {
            Player_PlayAnimSfx(this, D_808549A4);
        }
    } else {
        this->skelAnime.curFrame = rideActor->curFrame;
        LinkAnimation_AnimateFrame(play, &this->skelAnime);
    }

    AnimationContext_SetCopyAll(play, this->skelAnime.limbCount, this->skelAnime.morphTable,
                                this->skelAnime.jointTable);

    if ((play->csCtx.state != CS_STATE_IDLE) || (this->csMode != 0)) {
        if (this->csMode == 7) {
            this->csMode = 0;
        }
        this->attentionMode = 0;
        this->genericVar = 0;
    } else if ((this->genericTimer < 2) || (this->genericTimer >= 4)) {
        D_808535E0 = Player_SetupCurrentUpperAction(this, play);
        if (D_808535E0 != 0) {
            this->genericVar = 0;
        }
    }

    this->actor.world.pos.x = rideActor->actor.world.pos.x + rideActor->riderPos.x;
    this->actor.world.pos.y = (rideActor->actor.world.pos.y + rideActor->riderPos.y) - 27.0f;
    this->actor.world.pos.z = rideActor->actor.world.pos.z + rideActor->riderPos.z;

    this->currentYaw = this->actor.shape.rot.y = rideActor->actor.shape.rot.y;

    if ((this->csMode != 0) ||
        (!Player_CheckActorTalkRequested(play) && ((rideActor->actor.speedXZ != 0.0f) || !Player_SetupSpeakOrCheck(this, play)) &&
         !Player_SetupRollOrPutAway(this, play))) {
        if (D_808535E0 == 0) {
            if (this->genericVar != 0) {
                if (LinkAnimation_Update(play, &this->skelAnimeUpper)) {
                    rideActor->stateFlags &= ~ENHORSE_FLAG_8;
                    this->genericVar = 0;
                }

                if (this->skelAnimeUpper.animation == &gPlayerAnim_link_uma_stop_muti) {
                    if (LinkAnimation_OnFrame(&this->skelAnimeUpper, 23.0f)) {
                        func_8002F7DC(&this->actor, NA_SE_IT_LASH);
                        Player_PlayVoiceSfxForAge(this, NA_SE_VO_LI_LASH);
                    }

                    AnimationContext_SetCopyAll(play, this->skelAnime.limbCount, this->skelAnime.jointTable,
                                                this->skelAnimeUpper.jointTable);
                } else {
                    if (LinkAnimation_OnFrame(&this->skelAnimeUpper, 10.0f)) {
                        func_8002F7DC(&this->actor, NA_SE_IT_LASH);
                        Player_PlayVoiceSfxForAge(this, NA_SE_VO_LI_LASH);
                    }

                    AnimationContext_SetCopyTrue(play, this->skelAnime.limbCount, this->skelAnime.jointTable,
                                                 this->skelAnimeUpper.jointTable, D_80853410);
                }
            } else {
                LinkAnimationHeader* anim = NULL;

                if (EN_HORSE_CHECK_3(rideActor)) {
                    anim = &gPlayerAnim_link_uma_stop_muti;
                } else if (EN_HORSE_CHECK_2(rideActor)) {
                    if ((this->genericTimer >= 2) && (this->genericTimer != 99)) {
                        anim = D_80854968[this->genericTimer - 2];
                    }
                }

                if (anim != NULL) {
                    LinkAnimation_PlayOnce(play, &this->skelAnimeUpper, anim);
                    this->genericVar = 1;
                }
            }
        }

        if (this->stateFlags1 & PLAYER_STATE1_IN_FIRST_PERSON_MODE) {
            if (!Player_SetupCameraMode(play, this) || CHECK_BTN_ANY(sControlInput->press.button, BTN_A) ||
                Player_IsZTargeting(this)) {
                this->attentionMode = 0;
                this->stateFlags1 &= ~PLAYER_STATE1_IN_FIRST_PERSON_MODE;
            } else {
                this->upperBodyRot.y = func_8084ABD8(play, this, 1, -5000) - this->actor.shape.rot.y;
                this->upperBodyRot.y += 5000;
                this->upperBodyYawOffset = -5000;
            }
            return;
        }

        if ((this->csMode != 0) || (!func_8084C9BC(this, play) && !Player_SetupItemCutsceneOrFirstPerson(this, play))) {
            if (this->targetActor != NULL) {
                if (Actor_PlayerIsAimingReadyFpsItem(this) != 0) {
                    this->upperBodyRot.y = Player_LookAtTargetActor(this, 1) - this->actor.shape.rot.y;
                    this->upperBodyRot.y = CLAMP(this->upperBodyRot.y, -0x4AAA, 0x4AAA);
                    this->actor.focus.rot.y = this->actor.shape.rot.y + this->upperBodyRot.y;
                    this->upperBodyRot.y += 5000;
                    this->lookFlags |= 0x80;
                } else {
                    Player_LookAtTargetActor(this, 0);
                }
            } else {
                if (Actor_PlayerIsAimingReadyFpsItem(this) != 0) {
                    this->upperBodyRot.y = func_8084ABD8(play, this, 1, -5000) - this->actor.shape.rot.y;
                    this->upperBodyRot.y += 5000;
                    this->upperBodyYawOffset = -5000;
                }
            }
        }
    }
}

static PlayerAnimSfxEntry D_808549C4[] = {
    { 0, 0x2800 },
    { NA_SE_PL_GET_OFF_HORSE, 0x80A },
    { NA_SE_PL_SLIPDOWN, -0x819 },
};

void Player_DismountHorse(Player* this, PlayState* play) {
    this->stateFlags2 |= PLAYER_STATE2_ALWAYS_DISABLE_MOVE_ROTATION;
    func_8084CBF4(this, 1.0f, 10.0f);

    if (LinkAnimation_Update(play, &this->skelAnime)) {
        EnHorse* rideActor = (EnHorse*)this->rideActor;

        Player_SetupStandingStillNoMorph(this, play);
        this->stateFlags1 &= ~PLAYER_STATE1_RIDING_HORSE;
        this->actor.parent = NULL;
        AREG(6) = 0;

        if (Flags_GetEventChkInf(0x18) || (DREG(1) != 0)) {
            gSaveContext.horseData.pos.x = rideActor->actor.world.pos.x;
            gSaveContext.horseData.pos.y = rideActor->actor.world.pos.y;
            gSaveContext.horseData.pos.z = rideActor->actor.world.pos.z;
            gSaveContext.horseData.angle = rideActor->actor.shape.rot.y;
        }
    } else {
        Camera_ChangeSetting(Play_GetCamera(play, 0), CAM_SET_NORMAL0);

        if (this->mountSide < 0) {
            D_808549C4[0].field = 0x2828;
        } else {
            D_808549C4[0].field = 0x281D;
        }
        Player_PlayAnimSfx(this, D_808549C4);
    }
}

static PlayerAnimSfxEntry sSwimAnimSfx[] = {
    { NA_SE_PL_SWIM, -0x800 },
};

void Player_SetupSwimMovement(Player* this, f32* arg1, f32 arg2, s16 arg3) {
    Player_UpdateSwimMovement(this, arg1, arg2, arg3);
    Player_PlayAnimSfx(this, sSwimAnimSfx);
}

void Player_SetupSwim(PlayState* play, Player* this, s16 arg2) {
    Player_SetActionFunc(play, this, Player_Swim, 0);
    this->actor.shape.rot.y = this->currentYaw = arg2;
    Player_ChangeAnimLongMorphLoop(play, this, &gPlayerAnim_link_swimer_swim);
}

void Player_SetupZTargetSwimming(PlayState* play, Player* this) {
    Player_SetActionFunc(play, this, Player_ZTargetSwimming, 0);
    Player_ChangeAnimLongMorphLoop(play, this, &gPlayerAnim_link_swimer_swim);
}

void Player_UpdateSwimIdle(Player* this, PlayState* play) {
    f32 sp34;
    s16 sp32;

    if (CVarGetInteger("gNoWater", 0)) {
        Player_SetupReturnToStandStill(this, play);
        return;
    }

    Player_LoopAnimContinuously(play, this, &gPlayerAnim_link_swimer_swim_wait);
    func_8084B000(this);

    if (!Player_CheckActorTalkRequested(play) && !Player_SetupSubAction(play, this, sSwimSubActions, 1) &&
        !Player_SetupDive(play, this, sControlInput)) {
        if (this->attentionMode != 1) {
            this->attentionMode = 0;
        }

        if (this->currentBoots == PLAYER_BOOTS_IRON) {
            sp34 = 0.0f;
            sp32 = this->actor.shape.rot.y;

            if (this->actor.bgCheckFlags & 1) {
                Player_SetupReturnToStandStillSetAnim(
                    this, GET_PLAYER_ANIM(PLAYER_ANIMGROUP_SHORT_JUMP_LANDING, this->modelAnimType), play);
                Player_PlayLandingSfx(this);
            }
        } else {
            Player_GetTargetVelocityAndYaw(this, &sp34, &sp32, 0.0f, play);

            if (sp34 != 0.0f) {
                s16 temp = this->actor.shape.rot.y - sp32;

                if ((ABS(temp) > 0x6000) && !Math_StepToF(&this->linearVelocity, 0.0f, 1.0f)) {
                    return;
                }

                if (Player_IsZTargetingSetupStartUnfriendly(this)) {
                    Player_SetupZTargetSwimming(play, this);
                } else {
                    Player_SetupSwim(play, this, sp32);
                }
            }
        }

        Player_UpdateSwimMovement(this, &this->linearVelocity, sp34, sp32);
    }
}

void Player_SpawnSwimming(Player* this, PlayState* play) {
    if (!Player_SetupItemCutsceneOrFirstPerson(this, play)) {
        this->stateFlags2 |= PLAYER_STATE2_DISABLE_MOVE_ROTATION_WHILE_Z_TARGETING;

        func_8084B158(play, this, NULL, this->linearVelocity);
        func_8084B000(this);

        if (DECR(this->genericTimer) == 0) {
            Player_SetupSwimIdle(play, this);
        }
    }
}

void Player_Swim(Player* this, PlayState* play) {
    f32 sp34;
    s16 sp32;
    s16 temp;

    if (CVarGetInteger("gNoWater", 0)) {
        Player_SetupReturnToStandStill(this, play);
        return;
    }

    this->stateFlags2 |= PLAYER_STATE2_DISABLE_MOVE_ROTATION_WHILE_Z_TARGETING;

    func_8084B158(play, this, sControlInput, this->linearVelocity);
    func_8084B000(this);

    if (!Player_SetupSubAction(play, this, sSwimSubActions, 1) && !Player_SetupDive(play, this, sControlInput)) {
        Player_GetTargetVelocityAndYaw(this, &sp34, &sp32, 0.0f, play);

        temp = this->actor.shape.rot.y - sp32;
        if ((sp34 == 0.0f) || (ABS(temp) > 0x6000) || (this->currentBoots == PLAYER_BOOTS_IRON)) {
            Player_SetupSwimIdle(play, this);
        } else if (Player_IsZTargetingSetupStartUnfriendly(this)) {
            Player_SetupZTargetSwimming(play, this);
        }

        Player_SetupSwimMovement(this, &this->linearVelocity, sp34, sp32);
    }
}

s32 func_8084D980(PlayState* play, Player* this, f32* arg2, s16* arg3) {
    LinkAnimationHeader* anim;
    s16 temp1;
    s32 temp2;

    temp1 = this->currentYaw - *arg3;

    if (ABS(temp1) > 0x6000) {
        anim = &gPlayerAnim_link_swimer_swim_wait;

        if (Math_StepToF(&this->linearVelocity, 0.0f, 1.0f)) {
            this->currentYaw = *arg3;
        } else {
            *arg2 = 0.0f;
            *arg3 = this->currentYaw;
        }
    } else {
        temp2 = func_8083FD78(this, arg2, arg3, play);

        if (temp2 > 0) {
            anim = &gPlayerAnim_link_swimer_swim;
        } else if (temp2 < 0) {
            anim = &gPlayerAnim_link_swimer_back_swim;
        } else if ((temp1 = this->actor.shape.rot.y - *arg3) > 0) {
            anim = &gPlayerAnim_link_swimer_Rside_swim;
        } else {
            anim = &gPlayerAnim_link_swimer_Lside_swim;
        }
    }

    if (anim != this->skelAnime.animation) {
        Player_ChangeAnimLongMorphLoop(play, this, anim);
        return 1;
    }

    return 0;
}

void Player_ZTargetSwimming(Player* this, PlayState* play) {
    f32 sp2C;
    s16 sp2A;

    func_8084B158(play, this, sControlInput, this->linearVelocity);
    func_8084B000(this);

    if (!Player_SetupSubAction(play, this, sSwimSubActions, 1) && !Player_SetupDive(play, this, sControlInput)) {
        Player_GetTargetVelocityAndYaw(this, &sp2C, &sp2A, 0.0f, play);

        if (sp2C == 0.0f) {
            Player_SetupSwimIdle(play, this);
        } else if (!Player_IsZTargetingSetupStartUnfriendly(this)) {
            Player_SetupSwim(play, this, sp2A);
        } else {
            func_8084D980(play, this, &sp2C, &sp2A);
        }

        Player_SetupSwimMovement(this, &this->linearVelocity, sp2C, sp2A);
    }
}

void func_8084DBC4(PlayState* play, Player* this, f32 arg2) {
    f32 sp2C;
    s16 sp2A;

    Player_GetTargetVelocityAndYaw(this, &sp2C, &sp2A, 0.0f, play);
    Player_UpdateSwimMovement(this, &this->linearVelocity, sp2C * 0.5f, sp2A);
    Player_UpdateSwimMovement(this, &this->actor.velocity.y, arg2, this->currentYaw);
}

void Player_Dive(Player* this, PlayState* play) {
    f32 sp2C;

    this->stateFlags2 |= PLAYER_STATE2_DISABLE_MOVE_ROTATION_WHILE_Z_TARGETING;
    this->actor.gravity = 0.0f;
    Player_SetupCurrentUpperAction(this, play);

    if (!Player_SetupItemCutsceneOrFirstPerson(this, play)) {
        if (this->currentBoots == PLAYER_BOOTS_IRON) {
            Player_SetupSwimIdle(play, this);
            return;
        }

        if (this->genericVar == 0) {
            if (this->genericTimer == 0) {
                if (LinkAnimation_Update(play, &this->skelAnime) ||
                    ((this->skelAnime.curFrame >= 22.0f) && !CHECK_BTN_ALL(sControlInput->cur.button, BTN_A))) {
                    Player_RiseFromDive(play, this);
                } else if (LinkAnimation_OnFrame(&this->skelAnime, 20.0f) != 0) {
                    this->actor.velocity.y = -2.0f;
                }

                Player_StepLinearVelocityToZero(this);
                return;
            }

            func_8084B158(play, this, sControlInput, this->actor.velocity.y);
            this->shapePitchOffset = 16000;

            if (CHECK_BTN_ALL(sControlInput->cur.button, BTN_A) &&
                !Player_SetupGetItemOrHoldBehavior(this, play) && !(this->actor.bgCheckFlags & 1) &&
                (this->actor.yDistToWater < D_80854784[CUR_UPG_VALUE(UPG_SCALE)])) {
                func_8084DBC4(play, this, -2.0f);
            } else {
                this->genericVar++;
                Player_ChangeAnimLongMorphLoop(play, this, &gPlayerAnim_link_swimer_swim_wait);
            }
        } else if (this->genericVar == 1) {
            LinkAnimation_Update(play, &this->skelAnime);
            func_8084B000(this);

            if (this->shapePitchOffset < 10000) {
                this->genericVar++;
                this->genericTimer = this->actor.yDistToWater;
                Player_ChangeAnimLongMorphLoop(play, this, &gPlayerAnim_link_swimer_swim);
            }
        } else if (!Player_SetupDive(play, this, sControlInput)) {
            sp2C = (this->genericTimer * 0.018f) + 4.0f;

            if (this->stateFlags1 & PLAYER_STATE1_HOLDING_ACTOR) {
                sControlInput = NULL;
            }

            func_8084B158(play, this, sControlInput, fabsf(this->actor.velocity.y));
            Math_ScaledStepToS(&this->shapePitchOffset, -10000, 800);

            if (sp2C > 8.0f) {
                sp2C = 8.0f;
            }

            func_8084DBC4(play, this, sp2C);
        }
    }
}

void func_8084DF6C(PlayState* play, Player* this) {
    this->giDrawIdPlusOne = 0;
    this->stateFlags1 &= ~(PLAYER_STATE1_GETTING_ITEM | PLAYER_STATE1_HOLDING_ACTOR);
    this->getItemId = GI_NONE;
    this->getItemEntry = (GetItemEntry)GET_ITEM_NONE;
    func_8005B1A4(Play_GetCamera(play, 0));
}

void func_8084DFAC(PlayState* play, Player* this) {
    func_8084DF6C(play, this);
    Player_AddRootYawToShapeYaw(this);
    Player_SetupStandingStillNoMorph(this, play);
    this->currentYaw = this->actor.shape.rot.y;
}

s32 Player_SetupGetItemText(PlayState* play, Player* this) {
    GetItemEntry giEntry;
    s32 temp1;
    s32 temp2;
    static s32 equipItem;
    static bool equipNow;

    if (this->getItemId == GI_NONE && this->getItemEntry.objectId == OBJECT_INVALID) {
        return 1;
    }

    if (this->genericVar == 0) {
        if (this->getItemEntry.objectId == OBJECT_INVALID || (this->getItemId != this->getItemEntry.getItemId)) {
            giEntry = ItemTable_Retrieve(this->getItemId);
        } else {
            giEntry = this->getItemEntry;
        }
        this->genericVar = 1;
        equipItem = giEntry.itemId;
        equipNow = CVarGetInteger("gAskToEquip", 0) && giEntry.modIndex == MOD_NONE &&
                    equipItem >= ITEM_SWORD_KOKIRI && equipItem <= ITEM_TUNIC_ZORA &&
                   ((gItemAgeReqs[equipItem] == 9 || gItemAgeReqs[equipItem] == gSaveContext.linkAge) ||
                    CVarGetInteger("gTimelessEquipment", 0));

        Message_StartTextbox(play, giEntry.textId, &this->actor);
        // RANDOTODO: Macro this boolean check.
        if (!(giEntry.modIndex == MOD_RANDOMIZER && giEntry.itemId == RG_ICE_TRAP)) {
            if (giEntry.modIndex == MOD_NONE) {
                // RANDOTOD: Move this into Item_Give() or some other more central location
                if (giEntry.getItemId == GI_SWORD_BGS) {
                    gSaveContext.bgsFlag = true;
                    gSaveContext.swordHealth = 8;
                }
                Item_Give(play, giEntry.itemId);
            } else {
                Randomizer_Item_Give(play, giEntry);
            }
            Player_SetPendingFlag(this, play);
        }

        // Use this if we do have a getItemEntry
        if (giEntry.modIndex == MOD_NONE) {
            if (gSaveContext.n64ddFlag) {
                Audio_PlayFanfare_Rando(giEntry);
            } else if (((giEntry.itemId >= ITEM_RUPEE_GREEN) && (giEntry.itemId <= ITEM_RUPEE_RED)) ||
                        ((giEntry.itemId >= ITEM_RUPEE_PURPLE) && (giEntry.itemId <= ITEM_RUPEE_GOLD)) ||
                        (giEntry.itemId == ITEM_HEART)) {
                Audio_PlaySoundGeneral(NA_SE_SY_GET_BOXITEM, &D_801333D4, 4, &D_801333E0, &D_801333E0, &D_801333E8);
            } else {
                if ((giEntry.itemId == ITEM_HEART_CONTAINER) ||
                    ((giEntry.itemId == ITEM_HEART_PIECE_2) &&
                        ((gSaveContext.inventory.questItems & 0xF0000000) == 0x40000000))) {
                    temp1 = NA_BGM_HEART_GET | 0x900;
                } else {
                    temp1 = temp2 =
                        (giEntry.itemId == ITEM_HEART_PIECE_2) ? NA_BGM_SMALL_ITEM_GET : NA_BGM_ITEM_GET | 0x900;
                }
                Audio_PlayFanfare(temp1);
            }
        } else if (giEntry.modIndex == MOD_RANDOMIZER) {
            if (gSaveContext.n64ddFlag) {
                Audio_PlayFanfare_Rando(giEntry);
            } else if (giEntry.itemId == RG_DOUBLE_DEFENSE || giEntry.itemId == RG_MAGIC_SINGLE ||
                        giEntry.itemId == RG_MAGIC_DOUBLE) {
                Audio_PlayFanfare(NA_BGM_HEART_GET | 0x900);
            } else {
                // Just in case something weird happens with MOD_INDEX
                Audio_PlayFanfare(NA_BGM_ITEM_GET | 0x900);
            }
        } else {
            // Just in case something weird happens with modIndex.
            Audio_PlayFanfare(NA_BGM_ITEM_GET | 0x900);
        }
    }
    else if (equipNow && Message_ShouldAdvanceSilent(play) &&
             Message_GetState(&play->msgCtx) == TEXT_STATE_CHOICE) {
        if (play->msgCtx.choiceIndex == 0) { // Equip now? Yes

            if (equipItem >= ITEM_SWORD_KOKIRI && equipItem <= ITEM_SWORD_BGS) {
                gSaveContext.equips.buttonItems[0] = equipItem;
                Inventory_ChangeEquipment(EQUIP_SWORD, equipItem - ITEM_SWORD_KOKIRI + 1);
                Player_PlayReactableSfx(this, NA_SE_IT_SWORD_PUTAWAY);

            } else if (equipItem >= ITEM_SHIELD_DEKU && equipItem <= ITEM_SHIELD_MIRROR) {
                Inventory_ChangeEquipment(EQUIP_SHIELD, equipItem - ITEM_SHIELD_DEKU + 1);
                Player_PlayReactableSfx(&this->actor, NA_SE_IT_SHIELD_REMOVE);
                Player_SetEquipmentData(play, this);

            } else if (equipItem == ITEM_TUNIC_GORON || equipItem == ITEM_TUNIC_ZORA) {
                Inventory_ChangeEquipment(EQUIP_TUNIC, equipItem - ITEM_TUNIC_KOKIRI + 1);
                Player_PlayReactableSfx(this, NA_SE_PL_CHANGE_ARMS);
                Player_SetEquipmentData(play, this);
            }
        }
        equipNow = false;
        Message_CloseTextbox(play);
        play->msgCtx.msgMode = MSGMODE_TEXT_DONE;
    } else {
        if (Message_GetState(&play->msgCtx) == TEXT_STATE_CLOSING) {
            if (this->getItemId == GI_GAUNTLETS_SILVER && !gSaveContext.n64ddFlag) {
                play->nextEntranceIndex = 0x0123;
                play->sceneLoadFlag = 0x14;
                gSaveContext.nextCutsceneIndex = 0xFFF1;
                play->fadeTransition = 0xF;
                this->stateFlags1 &= ~PLAYER_STATE1_IN_CUTSCENE;
                Player_SetupPlayerCutscene(play, NULL, 8);
            }

            // Set giDrawIdPlusOne to 0 early to not have the game draw non-custom colored models for a split second.
            // This unk is what the game normally uses to decide what item to draw when holding up an item above Link's head.
            // Only do this when the item actually has a custom draw function.
            if (this->getItemEntry.drawFunc != NULL) {
                this->giDrawIdPlusOne = 0;
            }

            if (this->getItemEntry.itemId == RG_ICE_TRAP && this->getItemEntry.modIndex == MOD_RANDOMIZER) {
                this->giDrawIdPlusOne = 0;
                gSaveContext.pendingIceTrapCount++;
                Player_SetPendingFlag(this, play);
                GameInteractor_ExecuteOnItemReceiveHooks(giEntry);
            }

            this->getItemId = GI_NONE;
            this->getItemEntry = (GetItemEntry)GET_ITEM_NONE;
        }
    }

    return 0;
}

void Player_GetItemInWater(Player* this, PlayState* play) {
    this->stateFlags2 |= PLAYER_STATE2_DISABLE_MOVE_ROTATION_WHILE_Z_TARGETING;

    if (LinkAnimation_Update(play, &this->skelAnime)) {
        if (!(this->stateFlags1 & PLAYER_STATE1_GETTING_ITEM) || Player_SetupGetItemText(play, this)) {
            func_8084DF6C(play, this);
            Player_SetupSwimIdle(play, this);
            Player_ResetSubCam(play, this);
        }
    } else {
        if ((this->stateFlags1 & PLAYER_STATE1_GETTING_ITEM) && LinkAnimation_OnFrame(&this->skelAnime, 10.0f)) {
            Player_SetGetItemDrawIdPlusOne(this, play);
            Player_ResetSubCam(play, this);
            Player_SetCameraTurnAround(play, 8);
        } else if (LinkAnimation_OnFrame(&this->skelAnime, 5.0f)) {
            Player_PlayVoiceSfxForAge(this, NA_SE_VO_LI_BREATH_DRINK);
        }
    }

    func_8084B000(this);
    Player_UpdateSwimMovement(this, &this->linearVelocity, 0.0f, this->actor.shape.rot.y);
}

void Player_DamagedSwim(Player* this, PlayState* play) {
    func_8084B000(this);

    if (LinkAnimation_Update(play, &this->skelAnime)) {
        Player_SetupSwimIdle(play, this);
    }

    Player_UpdateSwimMovement(this, &this->linearVelocity, 0.0f, this->actor.shape.rot.y);
}

void Player_Drown(Player* this, PlayState* play) {
    func_8084B000(this);

    if (LinkAnimation_Update(play, &this->skelAnime)) {
        func_80843AE8(play, this);
    }

    Player_UpdateSwimMovement(this, &this->linearVelocity, 0.0f, this->actor.shape.rot.y);
}

static s16 D_808549D4[] = { 0x0600, 0x04F6, 0x0604, 0x01F1, 0x0568, 0x05F4 };

void Player_PlayOcarina(Player* this, PlayState* play) {
    if (LinkAnimation_Update(play, &this->skelAnime)) {
        Player_PlayAnimLoopSlowed(play, this, &gPlayerAnim_link_normal_okarina_swing);
        this->genericTimer = 1;
        if (this->stateFlags2 & (PLAYER_STATE2_NEAR_OCARINA_ACTOR | PLAYER_STATE2_PLAYING_OCARINA_FOR_ACTOR)) {
            this->stateFlags2 |= PLAYER_STATE2_ATTEMPT_PLAY_OCARINA_FOR_ACTOR;
        } else {
            func_8010BD58(play, OCARINA_ACTION_FREE_PLAY);
        }
        return;
    }

    if (this->genericTimer == 0) {
        return;
    }

    if (play->msgCtx.ocarinaMode == OCARINA_MODE_04) {
        func_8005B1A4(Play_GetCamera(play, 0));

        if ((this->talkActor != NULL) && (this->talkActor == this->ocarinaActor)) {
            Player_StartTalkingWithActor(play, this->talkActor);
        } else if (this->naviTextId < 0) {
            this->talkActor = this->naviActor;
            this->naviActor->textId = -this->naviTextId;
            Player_StartTalkingWithActor(play, this->talkActor);
        } else if (!Player_SetupItemCutsceneOrFirstPerson(this, play)) {
            Player_SetupReturnToStandStillSetAnim(this, &gPlayerAnim_link_normal_okarina_end, play);
        }

        this->stateFlags2 &= ~(PLAYER_STATE2_NEAR_OCARINA_ACTOR | PLAYER_STATE2_ATTEMPT_PLAY_OCARINA_FOR_ACTOR |
                               PLAYER_STATE2_PLAYING_OCARINA_FOR_ACTOR);
        this->ocarinaActor = NULL;
    } else if (play->msgCtx.ocarinaMode == OCARINA_MODE_02) {
        gSaveContext.respawn[RESPAWN_MODE_RETURN].entranceIndex = D_808549D4[play->msgCtx.lastPlayedSong];
        gSaveContext.respawn[RESPAWN_MODE_RETURN].playerParams = 0x5FF;
        gSaveContext.respawn[RESPAWN_MODE_RETURN].data = play->msgCtx.lastPlayedSong;

        this->csMode = 0;
        this->stateFlags1 &= ~PLAYER_STATE1_IN_CUTSCENE;
        this->stateFlags1 &= ~PLAYER_STATE1_IN_CUTSCENE;

        Player_SetupPlayerCutscene(play, NULL, 8);
        play->mainCamera.unk_14C &= ~8;

        this->stateFlags1 |= PLAYER_STATE1_SKIP_OTHER_ACTORS_UPDATE | PLAYER_STATE1_IN_CUTSCENE;
        this->stateFlags2 |= PLAYER_STATE2_PLAYING_OCARINA_GENERAL;

        if (Actor_Spawn(&play->actorCtx, play, ACTOR_DEMO_KANKYO, 0.0f, 0.0f, 0.0f, 0, 0, 0, 0xF, true) == NULL) {
            Environment_WarpSongLeave(play);
        }

        gSaveContext.seqId = (u8)NA_BGM_DISABLED;
        gSaveContext.natureAmbienceId = NATURE_ID_DISABLED;
    }
}

void Player_ThrowDekuNut(Player* this, PlayState* play) {
    if (LinkAnimation_Update(play, &this->skelAnime)) {
        Player_SetupReturnToStandStillSetAnim(this, &gPlayerAnim_link_normal_light_bom_end, play);
    } else if (LinkAnimation_OnFrame(&this->skelAnime, 3.0f)) {
        Inventory_ChangeAmmo(ITEM_NUT, -1);
        Actor_Spawn(&play->actorCtx, play, ACTOR_EN_ARROW, this->bodyPartsPos[PLAYER_BODYPART_R_HAND].x,
                    this->bodyPartsPos[PLAYER_BODYPART_R_HAND].y, this->bodyPartsPos[PLAYER_BODYPART_R_HAND].z, 4000,
                    this->actor.shape.rot.y, 0, ARROW_NUT, true);
        Player_PlayVoiceSfxForAge(this, NA_SE_VO_LI_SWORD_N);
    }

    Player_StepLinearVelocityToZero(this);
}

static PlayerAnimSfxEntry D_808549E0[] = {
    { 0, 0x3857 },
    { NA_SE_VO_LI_CLIMB_END, 0x2057 },
    { NA_SE_VO_LI_AUTO_JUMP, 0x2045 },
    { 0, -0x287B },
};

void Player_GetItem(Player* this, PlayState* play) {
    s32 cond;

    if (LinkAnimation_Update(play, &this->skelAnime)) {
        if (this->genericTimer != 0) {
            if (this->genericTimer >= 2) {
                this->genericTimer--;
            }

            if (Player_SetupGetItemText(play, this) && (this->genericTimer == 1)) {
                cond = ((this->talkActor != NULL) && (this->exchangeItemId < 0)) ||
                       (this->stateFlags3 & PLAYER_STATE3_FORCE_PULL_OCARINA);

                if (cond || (gSaveContext.healthAccumulator == 0)) {
                    if (cond) {
                        func_8084DF6C(play, this);
                        this->exchangeItemId = EXCH_ITEM_NONE;

                        if (func_8084B4D4(play, this) == 0) {
                            Player_StartTalkingWithActor(play, this->talkActor);
                        }
                    } else {
                        func_8084DFAC(play, this);
                    }
                }
            }
        } else {
            Player_EndAnimMovement(this);

            if ((this->getItemId == GI_ICE_TRAP && !gSaveContext.n64ddFlag) ||
                (gSaveContext.n64ddFlag && (this->getItemId == RG_ICE_TRAP || this->getItemEntry.getItemId == RG_ICE_TRAP))) {
                this->stateFlags1 &= ~(PLAYER_STATE1_GETTING_ITEM | PLAYER_STATE1_HOLDING_ACTOR);

                if ((this->getItemId != GI_ICE_TRAP && !gSaveContext.n64ddFlag) ||
                    (gSaveContext.n64ddFlag && (this->getItemId != RG_ICE_TRAP || this->getItemEntry.getItemId != RG_ICE_TRAP))) {
                    Actor_Spawn(&play->actorCtx, play, ACTOR_EN_CLEAR_TAG, this->actor.world.pos.x,
                                this->actor.world.pos.y + 100.0f, this->actor.world.pos.z, 0, 0, 0, 0, true);
                    Player_SetupStandingStillNoMorph(this, play);
                    GameInteractor_ExecuteOnItemReceiveHooks(this->getItemEntry);
                } else {
                    this->actor.colChkInfo.damage = 0;
                    Player_SetupDamage(play, this, 3, 0.0f, 0.0f, 0, 20);
                    GameInteractor_ExecuteOnItemReceiveHooks(this->getItemEntry);
                    this->getItemId = GI_NONE;
                    this->getItemEntry = (GetItemEntry)GET_ITEM_NONE;
                    // Gameplay stats: Increment Ice Trap count
                    gSaveContext.sohStats.count[COUNT_ICE_TRAPS]++;
                }
                return;
            }

            if (this->skelAnime.animation == &gPlayerAnim_link_normal_box_kick) {
                Player_PlayAnimOnceSlowed(play, this, &gPlayerAnim_link_demo_get_itemB);
            } else {
                Player_PlayAnimOnceSlowed(play, this, &gPlayerAnim_link_demo_get_itemA);
            }

            this->genericTimer = 2;
            Player_SetCameraTurnAround(play, 9);
        }
    } else {
        if (this->genericTimer == 0) {
            if (!LINK_IS_ADULT) {
                Player_PlayAnimSfx(this, D_808549E0);
            }
            return;
        }

        if (this->skelAnime.animation == &gPlayerAnim_link_demo_get_itemB) {
            Math_ScaledStepToS(&this->actor.shape.rot.y, Camera_GetCamDirYaw(GET_ACTIVE_CAM(play)) + 0x8000, 4000);
        }

        if (LinkAnimation_OnFrame(&this->skelAnime, 21.0f)) {
            Player_SetGetItemDrawIdPlusOne(this, play);
        }
    }
}

static PlayerAnimSfxEntry D_808549F0[] = {
    { NA_SE_IT_MASTER_SWORD_SWING, -0x83C },
};

void func_8084E988(Player* this) {
    Player_PlayAnimSfx(this, D_808549F0);
}

static PlayerAnimSfxEntry D_808549F4[] = {
    { NA_SE_VO_LI_AUTO_JUMP, 0x2005 },
    { 0, -0x280F },
};

void Player_EndTimeTravel(Player* this, PlayState* play) {
    if (LinkAnimation_Update(play, &this->skelAnime)) {
        if (this->genericVar == 0) {
            if (DECR(this->genericTimer) == 0) {
                this->genericVar = 1;
                this->skelAnime.endFrame = this->skelAnime.animLength - 1.0f;
            }
        } else {
            Player_SetupStandingStillNoMorph(this, play);
        }
    } else {
        if (LINK_IS_ADULT && LinkAnimation_OnFrame(&this->skelAnime, 158.0f)) {
            Player_PlayVoiceSfxForAge(this, NA_SE_VO_LI_SWORD_N);
            return;
        }

        if (!LINK_IS_ADULT) {
            Player_PlayAnimSfx(this, D_808549F4);
        } else {
            func_8084E988(this);
        }
    }
}

static u8 D_808549FC[] = {
    0x01, 0x03, 0x02, 0x04, 0x04,
};

void Player_DrinkFromBottle(Player* this, PlayState* play) {
    if (LinkAnimation_Update(play, &this->skelAnime)) {
        if (this->genericTimer == 0) {
            if (this->itemAction == PLAYER_IA_BOTTLE_POE) {
                s32 rand = Rand_S16Offset(-1, 3);

                if (rand == 0) {
                    rand = 3;
                }

                if ((rand < 0) && (gSaveContext.health <= 0x10)) {
                    rand = 3;
                }

                if (rand < 0) {
                    Health_ChangeBy(play, -0x10);
                } else {
                    gSaveContext.healthAccumulator = rand * 0x10;
                }
            } else {
                s32 sp28 = D_808549FC[this->itemAction - PLAYER_IA_BOTTLE_POTION_RED];

                if (CVarGetInteger("gRedPotionEffect", 0) && this->itemAction == PLAYER_IA_BOTTLE_POTION_RED) {
                    if (CVarGetInteger("gRedPercentRestore", 0)) {
                        gSaveContext.healthAccumulator =
                            (gSaveContext.healthCapacity * CVarGetInteger("gRedPotionHealth", 100) / 100 + 15) / 16 * 16;
                    } else {
                        gSaveContext.healthAccumulator = CVarGetInteger("gRedPotionHealth", 20) * 16;
                    }
                } else if (CVarGetInteger("gBluePotionEffects", 0) &&
                           this->itemAction == PLAYER_IA_BOTTLE_POTION_BLUE) {
                    if (CVarGetInteger("gBlueHealthPercentRestore", 0)) {
                        gSaveContext.healthAccumulator =
                            (gSaveContext.healthCapacity * CVarGetInteger("gBluePotionHealth", 100) / 100 + 15) / 16 * 16;
                    } else {
                        gSaveContext.healthAccumulator = CVarGetInteger("gBluePotionHealth", 20) * 16;
                    }

                    if (CVarGetInteger("gBlueManaPercentRestore", 0)) {
                        if (gSaveContext.magicState != 10) {
                            Magic_Fill(play);
                        }

                        func_80087708(play,
                                      (gSaveContext.magicLevel * 48 * CVarGetInteger("gBluePotionMana", 100) / 100 + 15) /
                                          16 * 16,
                                      5);
                    } else {
                        if (gSaveContext.magicState != 10) {
                            Magic_Fill(play);
                        }

                        func_80087708(play, CVarGetInteger("gBluePotionMana", 100), 5);
                        ;
                    }
                } else if (CVarGetInteger("gGreenPotionEffect", 0) &&
                           this->itemAction == PLAYER_IA_BOTTLE_POTION_GREEN) {
                    if (CVarGetInteger("gGreenPercentRestore", 0)) {
                        if (gSaveContext.magicState != 10) {
                            Magic_Fill(play);
                        }

                        func_80087708(play,
                                      (gSaveContext.magicLevel * 48 * CVarGetInteger("gGreenPotionMana", 100) / 100 + 15) /
                                          16 * 16,
                                      5);
                    } else {
                        if (gSaveContext.magicState != 10) {
                            Magic_Fill(play);
                        }

                        func_80087708(play, CVarGetInteger("gGreenPotionMana", 100), 5);
                        ;
                    }
                } else if (CVarGetInteger("gMilkEffect", 0) && (this->itemAction == PLAYER_IA_BOTTLE_MILK ||
                                                             this->itemAction == PLAYER_IA_BOTTLE_MILK_HALF)) {
                    if (CVarGetInteger("gMilkPercentRestore", 0)) {
                        gSaveContext.healthAccumulator =
                            (gSaveContext.healthCapacity * CVarGetInteger("gMilkHealth", 100) / 100 + 15) / 16 * 16;
                    } else {
                        gSaveContext.healthAccumulator = CVarGetInteger("gMilkHealth", 5) * 16;
                    }
                    if (CVarGetInteger("gSeparateHalfMilkEffect", 0) &&
                        this->itemAction == PLAYER_IA_BOTTLE_MILK_HALF) {
                        if (CVarGetInteger("gHalfMilkPercentRestore", 0)) {
                            gSaveContext.healthAccumulator =
                                (gSaveContext.healthCapacity * CVarGetInteger("gHalfMilkHealth", 100) / 100 + 15) / 16 *
                                16;
                        } else {
                            gSaveContext.healthAccumulator = CVarGetInteger("gHalfMilkHealth", 5) * 16;
                        }
                    }
                } else {
                    if (sp28 & 1) {
                        gSaveContext.healthAccumulator = 0x140;
                    }

                    if (sp28 & 2) {
                        Magic_Fill(play);
                    }

                    if (sp28 & 4) {
                        gSaveContext.healthAccumulator = 0x50;
                    }
                }
            }

            Player_PlayAnimLoopSlowed(play, this, &gPlayerAnim_link_bottle_drink_demo_wait);
            this->genericTimer = 1;
            return;
        }

        Player_SetupStandingStillNoMorph(this, play);
        func_8005B1A4(Play_GetCamera(play, 0));
    } else if (this->genericTimer == 1) {
        if ((gSaveContext.healthAccumulator == 0) && (gSaveContext.magicState != 9)) {
            Player_ChangeAnimSlowedMorphToLastFrame(play, this, &gPlayerAnim_link_bottle_drink_demo_end);
            this->genericTimer = 2;
            Player_UpdateBottleHeld(play, this, ITEM_BOTTLE, PLAYER_IA_BOTTLE);
        }
        Player_PlayVoiceSfxForAge(this, NA_SE_VO_LI_DRINK - SFX_FLAG);
    } else if ((this->genericTimer == 2) && LinkAnimation_OnFrame(&this->skelAnime, 29.0f)) {
        Player_PlayVoiceSfxForAge(this, NA_SE_VO_LI_BREATH_DRINK);
    }
}

static BottleCatchInfo D_80854A04[] = {
    { ACTOR_EN_ELF, ITEM_FAIRY, 0x2A, 0x46 },
    { ACTOR_EN_FISH, ITEM_FISH, 0x1F, 0x47 },
    { ACTOR_EN_ICE_HONO, ITEM_BLUE_FIRE, 0x20, 0x5D },
    { ACTOR_EN_INSECT, ITEM_BUG, 0x21, 0x7A },
};

void Player_SwingBottle(Player* this, PlayState* play) {
    BottleSwingAnimInfo* sp24;
    BottleCatchInfo* catchInfo;
    s32 temp;
    s32 i;

    sp24 = &sBottleSwingAnims[this->genericTimer];
    Player_StepLinearVelocityToZero(this);

    if (LinkAnimation_Update(play, &this->skelAnime)) {
        if (this->genericVar != 0) {
            if (this->genericTimer == 0) {
                if (CVarGetInteger("gFastDrops", 0)) {
                    this->genericVar = 0;
                } else {
                    Message_StartTextbox(play, D_80854A04[this->genericVar - 1].textId, &this->actor);
                }
                Audio_PlayFanfare(NA_BGM_ITEM_GET | 0x900);
                this->genericTimer = 1;
            } else if (Message_GetState(&play->msgCtx) == TEXT_STATE_CLOSING) {
                this->genericVar = 0;
                func_8005B1A4(Play_GetCamera(play, 0));
            }
        } else {
            Player_SetupStandingStillNoMorph(this, play);
        }
    } else {
        if (this->genericVar == 0) {
            temp = this->skelAnime.curFrame - sp24->unk_08;

            if (temp >= 0) {
                if (sp24->unk_09 >= temp) {
                    if (this->genericTimer != 0) {
                        if (temp == 0) {
                            func_8002F7DC(&this->actor, NA_SE_IT_SCOOP_UP_WATER);
                        }
                    }

                    if (this->interactRangeActor != NULL) {
                        catchInfo = &D_80854A04[0];
                        for (i = 0; i < 4; i++, catchInfo++) {
                            if (this->interactRangeActor->id == catchInfo->actorId) {
                                break;
                            }
                        }

                        if (i < 4) {
                            this->genericVar = i + 1;
                            this->genericTimer = 0;
                            this->interactRangeActor->parent = &this->actor;
                            Player_UpdateBottleHeld(play, this, catchInfo->itemId, ABS(catchInfo->actionParam));
                            if (!CVarGetInteger("gFastDrops", 0)) {
                                this->stateFlags1 |= PLAYER_STATE1_SKIP_OTHER_ACTORS_UPDATE | PLAYER_STATE1_IN_CUTSCENE;
                                Player_PlayAnimOnceSlowed(play, this, sp24->bottleCatchAnim);
                                Player_SetCameraTurnAround(play, 4);
                            }
                        }
                    }
                }
            }
        }
    }

    if (this->skelAnime.curFrame <= 7.0f) {
        this->stateFlags1 |= PLAYER_STATE1_SWINGING_BOTTLE;
    }
}

static Vec3f D_80854A1C = { 0.0f, 0.0f, 5.0f };

void Player_HealWithFairy(Player* this, PlayState* play) {
    if (LinkAnimation_Update(play, &this->skelAnime)) {
        Player_SetupStandingStillNoMorph(this, play);
        func_8005B1A4(Play_GetCamera(play, 0));
        return;
    }

    if (LinkAnimation_OnFrame(&this->skelAnime, 37.0f)) {
        Player_SpawnFairy(play, this, &this->leftHandPos, &D_80854A1C, FAIRY_REVIVE_BOTTLE);
        Player_UpdateBottleHeld(play, this, ITEM_BOTTLE, PLAYER_IA_BOTTLE);
        func_8002F7DC(&this->actor, NA_SE_EV_BOTTLE_CAP_OPEN);
        func_8002F7DC(&this->actor, NA_SE_EV_FIATY_HEAL - SFX_FLAG);
    } else if (LinkAnimation_OnFrame(&this->skelAnime, 47.0f)) {
        if (CVarGetInteger("gFairyEffect", 0)) {
            if (CVarGetInteger("gFairyPercentRestore", 0)) {
                gSaveContext.healthAccumulator =
                    (gSaveContext.healthCapacity * CVarGetInteger("gFairyHealth", 100) / 100 + 15) / 16 * 16;
            } else {
                gSaveContext.healthAccumulator = CVarGetInteger("gFairyHealth", 8) * 16;
            }
        } else {
            gSaveContext.healthAccumulator = 0x140;
        }
    }
}

static BottleDropInfo D_80854A28[] = {
    { ACTOR_EN_FISH, FISH_DROPPED },
    { ACTOR_EN_ICE_HONO, 0 },
    { ACTOR_EN_INSECT, 2 },
};

static PlayerAnimSfxEntry D_80854A34[] = {
    { NA_SE_VO_LI_AUTO_JUMP, 0x2026 },
    { NA_SE_EV_BOTTLE_CAP_OPEN, -0x828 },
};

void Player_DropItemFromBottle(Player* this, PlayState* play) {
    Player_StepLinearVelocityToZero(this);

    if (LinkAnimation_Update(play, &this->skelAnime)) {
        Player_SetupStandingStillNoMorph(this, play);
        func_8005B1A4(Play_GetCamera(play, 0));
        return;
    }

    if (LinkAnimation_OnFrame(&this->skelAnime, 76.0f)) {
        BottleDropInfo* dropInfo = &D_80854A28[this->itemAction - PLAYER_IA_BOTTLE_FISH];

        Actor_Spawn(&play->actorCtx, play, dropInfo->actorId,
                    (Math_SinS(this->actor.shape.rot.y) * 5.0f) + this->leftHandPos.x, this->leftHandPos.y,
                    (Math_CosS(this->actor.shape.rot.y) * 5.0f) + this->leftHandPos.z, 0x4000, this->actor.shape.rot.y,
                    0, dropInfo->actorParams, true);

        Player_UpdateBottleHeld(play, this, ITEM_BOTTLE, PLAYER_IA_BOTTLE);
        return;
    }

    Player_PlayAnimSfx(this, D_80854A34);
}

static PlayerAnimSfxEntry D_80854A3C[] = {
    { NA_SE_PL_PUT_OUT_ITEM, -0x81E },
};

void Player_PresentExchangeItem(Player* this, PlayState* play) {
    this->stateFlags2 |= PLAYER_STATE2_DISABLE_MOVE_ROTATION_WHILE_Z_TARGETING;

    if (LinkAnimation_Update(play, &this->skelAnime)) {
        if (this->genericTimer < 0) {
            Player_SetupStandingStillNoMorph(this, play);
        } else if (this->exchangeItemId == EXCH_ITEM_NONE) {
            Actor* targetActor = this->talkActor;

            this->giDrawIdPlusOne = 0;
            if (targetActor->textId != 0xFFFF) {
                this->actor.flags |= ACTOR_FLAG_PLAYER_TALKED_TO;
            }

            Player_StartTalkingWithActor(play, targetActor);
        } else {
            GetItemEntry giEntry = ItemTable_Retrieve(sExchangeGetItemIDs[this->exchangeItemId - 1]);

            if (this->itemAction >= PLAYER_IA_LETTER_ZELDA) {
                if (giEntry.gi >= 0) {
                    this->giDrawIdPlusOne = giEntry.gi;
                } else {
                    this->giDrawIdPlusOne = -giEntry.gi;
                }
            }

            if (this->genericTimer == 0) {
                Message_StartTextbox(play, this->actor.textId, &this->actor);

                if ((this->itemAction == PLAYER_IA_CHICKEN) || (this->itemAction == PLAYER_IA_POCKET_CUCCO)) {
                    func_8002F7DC(&this->actor, NA_SE_EV_CHICKEN_CRY_M);
                }

                this->genericTimer = 1;
            } else if (Message_GetState(&play->msgCtx) == TEXT_STATE_CLOSING) {
                this->actor.flags &= ~ACTOR_FLAG_PLAYER_TALKED_TO;
                this->giDrawIdPlusOne = 0;

                if (this->genericVar == 1) {
                    Player_PlayAnimOnce(play, this, &gPlayerAnim_link_bottle_read_end);
                    this->genericTimer = -1;
                } else {
                    Player_SetupStandingStillNoMorph(this, play);
                }

                func_8005B1A4(Play_GetCamera(play, 0));
            }
        }
    } else if (this->genericTimer >= 0) {
        Player_PlayAnimSfx(this, D_80854A3C);
    }

    if ((this->genericVar == 0) && (this->targetActor != NULL)) {
        this->currentYaw = this->actor.shape.rot.y = Player_LookAtTargetActor(this, 0);
    }
}

void func_8084F308(Player* this, PlayState* play) {
    this->stateFlags2 |=
        PLAYER_STATE2_DISABLE_MOVE_ROTATION_WHILE_Z_TARGETING | PLAYER_STATE2_ALWAYS_DISABLE_MOVE_ROTATION;

    if (LinkAnimation_Update(play, &this->skelAnime)) {
        Player_PlayAnimLoop(play, this, &gPlayerAnim_link_normal_re_dead_attack_wait);
    }

    if (Player_MashTimerThresholdExceeded(this, 0, 100)) {
        Player_SetupStandingStillType(this, play);
        this->stateFlags2 &= ~PLAYER_STATE2_RESTRAINED_BY_ENEMY;
    }
}

void Player_SlipOnSlope(Player* this, PlayState* play) {
    CollisionPoly* floorPoly;
    f32 sp50;
    f32 sp4C;
    f32 sp48;
    s16 sp46;
    s16 sp44;
    Vec3f sp38;

    this->stateFlags2 |=
        PLAYER_STATE2_DISABLE_MOVE_ROTATION_WHILE_Z_TARGETING | PLAYER_STATE2_ALWAYS_DISABLE_MOVE_ROTATION;
    LinkAnimation_Update(play, &this->skelAnime);
    Player_SetupSpawnDustAtFeet(play, this);
    func_800F4138(&this->actor.projectedPos, NA_SE_PL_SLIP_LEVEL - SFX_FLAG, this->actor.speedXZ);

    if (Player_SetupItemCutsceneOrFirstPerson(this, play) == 0) {
        floorPoly = this->actor.floorPoly;

        if (floorPoly == NULL) {
            Player_SetupFallFromLedge(this, play);
            return;
        }

        func_8083E298(floorPoly, &sp38, &sp46);

        sp44 = sp46;
        if (this->genericVar != 0) {
            sp44 = sp46 + 0x8000;
        }

        if (this->linearVelocity < 0) {
            sp46 += 0x8000;
        }

        sp50 = (1.0f - sp38.y) * 40.0f;
        sp50 = CLAMP(sp50, 0, 10.0f);
        sp4C = (sp50 * sp50) * 0.015f;
        sp48 = sp38.y * 0.01f;

        if (SurfaceType_GetSlope(&play->colCtx, floorPoly, this->actor.floorBgId) != 1) {
            sp50 = 0;
            sp48 = sp38.y * 10.0f;
        }

        if (sp4C < 1.0f) {
            sp4C = 1.0f;
        }

        if (Math_AsymStepToF(&this->linearVelocity, sp50, sp4C, sp48) && (sp50 == 0)) {
            LinkAnimationHeader* anim;
            if (this->genericVar == 0) {
                anim = GET_PLAYER_ANIM(PLAYER_ANIMGROUP_SLIDING_DOWN_SLOPE, this->modelAnimType);
            } else {
                anim = GET_PLAYER_ANIM(PLAYER_ANIMGROUP_END_SLIDING_DOWN_SLOPE, this->modelAnimType);
            }
            Player_SetupReturnToStandStillSetAnim(this, anim, play);
        }

        Math_SmoothStepToS(&this->currentYaw, sp46, 10, 4000, 800);
        Math_ScaledStepToS(&this->actor.shape.rot.y, sp44, 2000);
    }
}

void Player_SetDrawAndStartCutsceneAfterTimer(Player* this, PlayState* play) {
    if ((DECR(this->genericTimer) == 0) && Player_SetupCutscene(play, this)) {
        Player_CutsceneSetDraw(play, this, NULL);
        Player_SetActionFunc(play, this, Player_StartCutscene, 0);
        Player_StartCutscene(this, play);
    }
}

void Player_SpawnFromWarpSong(Player* this, PlayState* play) {
    Player_SetActionFunc(play, this, Player_SetDrawAndStartCutsceneAfterTimer, 0);
    this->genericTimer = 40;
    Actor_Spawn(&play->actorCtx, play, ACTOR_DEMO_KANKYO, 0.0f, 0.0f, 0.0f, 0, 0, 0, 0x10, true);
}

void Player_SpawnFromBlueWarp(Player* this, PlayState* play) {
    s32 pad;

    if ((this->genericVar != 0) && (play->csCtx.frames < 0x131)) {
        this->actor.gravity = 0.0f;
        this->actor.velocity.y = 0.0f;
    } else if (sPlayerYDistToFloor < 150.0f) {
        if (LinkAnimation_Update(play, &this->skelAnime)) {
            if (this->genericTimer == 0) {
                if (this->actor.bgCheckFlags & 1) {
                    this->skelAnime.endFrame = this->skelAnime.animLength - 1.0f;
                    Player_PlayLandingSfx(this);
                    this->genericTimer = 1;
                }
            } else {
                if ((play->sceneNum == SCENE_SPOT04) && Player_SetupCutscene(play, this)) {
                    return;
                }
                Player_SetupStandingStillMorph(this, play);
            }
        }
        Math_SmoothStepToF(&this->actor.velocity.y, 2.0f, 0.3f, 8.0f, 0.5f);
    }

    if ((play->sceneNum == SCENE_KENJYANOMA) && Player_SetupCutscene(play, this)) {
        return;
    }

    if ((play->csCtx.state != CS_STATE_IDLE) && (play->csCtx.linkAction != NULL)) {
        f32 sp28 = this->actor.world.pos.y;
        Player_CutsceneSetPosAndYaw(play, this, play->csCtx.linkAction);
        this->actor.world.pos.y = sp28;
    }
}

void Player_EnterGrotto(Player* this, PlayState* play) {
    LinkAnimation_Update(play, &this->skelAnime);

    if ((this->genericTimer++ > 8) && (play->sceneLoadFlag == 0)) {

        if (this->genericVar != 0) {
            if (play->sceneNum == 9) {
                Play_TriggerRespawn(play);
                play->nextEntranceIndex = 0x0088;
            } else if (this->genericVar < 0) {
                Play_TriggerRespawn(play);
                // In ER, handle DMT and other special void outs to respawn from last entrance from grotto 
                if (gSaveContext.n64ddFlag && Randomizer_GetSettingValue(RSK_SHUFFLE_ENTRANCES)) {
                    Grotto_ForceRegularVoidOut();
                }
            } else {
                Play_TriggerVoidOut(play);
            }

            play->fadeTransition = 4;
            func_80078884(NA_SE_OC_ABYSS);
        } else {
            play->fadeTransition = 2;
            gSaveContext.nextTransitionType = 2;
            gSaveContext.seqId = (u8)NA_BGM_DISABLED;
            gSaveContext.natureAmbienceId = 0xFF;
        }

        play->sceneLoadFlag = 0x14;
    }
}

void Player_SetupOpenDoorFromSpawn(Player* this, PlayState* play) {
    Player_SetupOpenDoor(this, play);
}

void Player_JumpFromGrotto(Player* this, PlayState* play) {
    this->actor.gravity = -1.0f;

    LinkAnimation_Update(play, &this->skelAnime);

    if (this->actor.velocity.y < 0.0f) {
        Player_SetupFallFromLedge(this, play);
    } else if (this->actor.velocity.y < 6.0f) {
        Math_StepToF(&this->linearVelocity, 3.0f, 0.5f);
    }
}

void Player_ShootingGalleryPlay(Player* this, PlayState* play) {
    this->attentionMode = 2;

    Player_SetupCameraMode(play, this);
    LinkAnimation_Update(play, &this->skelAnime);
    Player_SetupCurrentUpperAction(this, play);

    this->upperBodyRot.y = func_8084ABD8(play, this, 1, 0) - this->actor.shape.rot.y;
    this->lookFlags |= 0x80;

    if (play->shootingGalleryStatus < 0) {
        play->shootingGalleryStatus++;
        if (play->shootingGalleryStatus == 0) {
            Player_ClearLookAndAttention(this, play);
        }
    }
}

void Player_FrozenInIce(Player* this, PlayState* play) {
    if (this->genericVar >= 0) {
        if (this->genericVar < 6) {
            this->genericVar++;
        }

        if (Player_MashTimerThresholdExceeded(this, 1, 100)) {
            this->genericVar = -1;
            EffectSsIcePiece_SpawnBurst(play, &this->actor.world.pos, this->actor.scale.x);
            func_8002F7DC(&this->actor, NA_SE_PL_ICE_BROKEN);
        } else {
            this->stateFlags2 |= PLAYER_STATE2_FROZEN_IN_ICE;
        }

        if ((play->gameplayFrames % 4) == 0) {
            Player_InflictDamage(play, -1);
        }
    } else {
        if (LinkAnimation_Update(play, &this->skelAnime)) {
            Player_SetupStandingStillType(this, play);
            Player_SetupInvincibilityNoDamageFlash(this, -20);
        }
    }
}

void Player_SetupElectricShock(Player* this, PlayState* play) {
    LinkAnimation_Update(play, &this->skelAnime);
    Player_RoundUpInvincibilityTimer(this);

    if (((this->genericTimer % 25) != 0) || Player_Damage(play, this, -1)) {
        if (DECR(this->genericTimer) == 0) {
            Player_SetupStandingStillType(this, play);
        }
    }

    this->shockTimer = 40;
    func_8002F8F0(&this->actor, NA_SE_VO_LI_TAKEN_AWAY - SFX_FLAG + this->ageProperties->unk_92);
}

s32 Player_CheckNoDebugModeCombo(Player* this, PlayState* play) {
    sControlInput = &play->state.input[0];

    if (CVarGetInteger("gDebugEnabled", 0) &&
        ((CHECK_BTN_ALL(sControlInput->cur.button, BTN_A | BTN_L | BTN_R) &&
          CHECK_BTN_ALL(sControlInput->press.button, BTN_B)) ||
         (CHECK_BTN_ALL(sControlInput->cur.button, BTN_L) && CHECK_BTN_ALL(sControlInput->press.button, BTN_DRIGHT)))) {

        sDebugModeFlag ^= 1;

        if (sDebugModeFlag) {
            Camera_ChangeMode(Play_GetCamera(play, 0), CAM_MODE_BOWARROWZ);
        }
    }

    if (sDebugModeFlag) {
        f32 speed;

        if (CHECK_BTN_ALL(sControlInput->cur.button, BTN_R)) {
            speed = 100.0f;
        } else {
            speed = 20.0f;
        }

        func_8006375C(3, 2, "DEBUG MODE");

        if (!CHECK_BTN_ALL(sControlInput->cur.button, BTN_L)) {
            if (CHECK_BTN_ALL(sControlInput->cur.button, BTN_B)) {
                this->actor.world.pos.y += speed;
            } else if (CHECK_BTN_ALL(sControlInput->cur.button, BTN_A)) {
                this->actor.world.pos.y -= speed;
            }

            if (CHECK_BTN_ANY(sControlInput->cur.button, BTN_DUP | BTN_DLEFT | BTN_DDOWN | BTN_DRIGHT)) {
                s16 angle;
                s16 temp;

                angle = temp = Camera_GetInputDirYaw(GET_ACTIVE_CAM(play));

                if (CHECK_BTN_ALL(sControlInput->cur.button, BTN_DDOWN)) {
                    angle = temp + 0x8000;
                } else if (CHECK_BTN_ALL(sControlInput->cur.button, BTN_DLEFT)) {
                    angle = temp + (0x4000 * (CVarGetInteger("gMirroredWorld", 0) ? -1 : 1));
                } else if (CHECK_BTN_ALL(sControlInput->cur.button, BTN_DRIGHT)) {
                    angle = temp - (0x4000 * (CVarGetInteger("gMirroredWorld", 0) ? -1 : 1));
                }

                this->actor.world.pos.x += speed * Math_SinS(angle);
                this->actor.world.pos.z += speed * Math_CosS(angle);
            }
        }

        Player_StopMovement(this);

        this->actor.gravity = 0.0f;
        this->actor.velocity.z = 0.0f;
        this->actor.velocity.y = 0.0f;
        this->actor.velocity.x = 0.0f;

        if (CHECK_BTN_ALL(sControlInput->cur.button, BTN_L) && CHECK_BTN_ALL(sControlInput->press.button, BTN_DLEFT)) {
            Flags_SetTempClear(play, play->roomCtx.curRoom.num);
        }

        Math_Vec3f_Copy(&this->actor.home.pos, &this->actor.world.pos);

        return 0;
    }

    return 1;
}

void Player_BowStringMoveAfterShot(Player* this) {
    this->unk_858 += this->unk_85C;
    this->unk_85C -= this->unk_858 * 5.0f;
    this->unk_85C *= 0.3f;

    if (ABS(this->unk_85C) < 0.00001f) {
        this->unk_85C = 0.0f;
        if (ABS(this->unk_858) < 0.00001f) {
            this->unk_858 = 0.0f;
        }
    }
}

void Player_BunnyHoodPhysics(Player* this) {
    s32 pad;
    s16 sp2A;
    s16 sp28;
    s16 sp26;

    D_80858AC8.unk_06 -= D_80858AC8.unk_06 >> 3;
    D_80858AC8.unk_08 -= D_80858AC8.unk_08 >> 3;
    D_80858AC8.unk_06 += -D_80858AC8.unk_00 >> 2;
    D_80858AC8.unk_08 += -D_80858AC8.unk_02 >> 2;

    sp26 = this->actor.world.rot.y - this->actor.shape.rot.y;

    sp28 = (s32)(this->actor.speedXZ * -200.0f * Math_CosS(sp26) * (Rand_CenteredFloat(2.0f) + 10.0f)) & 0xFFFF;
    sp2A = (s32)(this->actor.speedXZ * 100.0f * Math_SinS(sp26) * (Rand_CenteredFloat(2.0f) + 10.0f)) & 0xFFFF;

    D_80858AC8.unk_06 += sp28 >> 2;
    D_80858AC8.unk_08 += sp2A >> 2;

    if (D_80858AC8.unk_06 > 6000) {
        D_80858AC8.unk_06 = 6000;
    } else if (D_80858AC8.unk_06 < -6000) {
        D_80858AC8.unk_06 = -6000;
    }

    if (D_80858AC8.unk_08 > 6000) {
        D_80858AC8.unk_08 = 6000;
    } else if (D_80858AC8.unk_08 < -6000) {
        D_80858AC8.unk_08 = -6000;
    }

    D_80858AC8.unk_00 += D_80858AC8.unk_06;
    D_80858AC8.unk_02 += D_80858AC8.unk_08;

    if (D_80858AC8.unk_00 < 0) {
        D_80858AC8.unk_04 = D_80858AC8.unk_00 >> 1;
    } else {
        D_80858AC8.unk_04 = 0;
    }
}

s32 Player_SetupStartMeleeWeaponAttack(Player* this, PlayState* play) {
    if (Player_CanSwingBottleOrCastFishingRod(play, this) == 0) {
        if (Player_CanJumpSlash(this) != 0) {
            s32 sp24 = Player_GetMeleeAttackAnim(this);

            Player_StartMeleeWeaponAttack(play, this, sp24);

            if (sp24 >= 0x18) {
                this->stateFlags2 |= PLAYER_STATE2_RELEASING_SPIN_ATTACK;
                Player_SetupSpinAttackActor(play, this, 0);
                return 1;
            }
        } else {
            return 0;
        }
    }

    return 1;
}

static Vec3f D_80854A40 = { 0.0f, 40.0f, 45.0f };

void Player_MeleeWeaponAttack(Player* this, PlayState* play) {
    MeleeAttackAnimInfo* sp44 = &sMeleeAttackAnims[this->meleeWeaponAnimation];

    this->stateFlags2 |= PLAYER_STATE2_DISABLE_MOVE_ROTATION_WHILE_Z_TARGETING;

    if (!func_80842DF4(play, this)) {
        func_8084285C(this, 0.0f, sp44->unk_0C, sp44->unk_0D);

        if ((this->stateFlags2 & PLAYER_STATE2_ENABLE_FORWARD_SLIDE_FROM_ATTACK) &&
            (this->heldItemAction != PLAYER_IA_HAMMER) && LinkAnimation_OnFrame(&this->skelAnime, 0.0f)) {
            this->linearVelocity = 15.0f;
            this->stateFlags2 &= ~PLAYER_STATE2_ENABLE_FORWARD_SLIDE_FROM_ATTACK;
        }

        if (this->linearVelocity > 12.0f) {
            Player_SetupSpawnDustAtFeet(play, this);
        }

        Math_StepToF(&this->linearVelocity, 0.0f, 5.0f);
        Player_SetupDeactivateComboTimer(this);

        if (LinkAnimation_Update(play, &this->skelAnime)) {
            if (!Player_SetupStartMeleeWeaponAttack(this, play)) {
                u8 sp43 = this->skelAnime.moveFlags;
                LinkAnimationHeader* sp3C;

                if (Player_IsUnfriendlyZTargeting(this)) {
                    sp3C = sp44->unk_08;
                } else {
                    sp3C = sp44->unk_04;
                }

                Player_InactivateMeleeWeapon(this);
                this->skelAnime.moveFlags = 0;

                if ((sp3C == &gPlayerAnim_link_fighter_Lpower_jump_kiru_end) &&
                    (this->modelAnimType != PLAYER_ANIMTYPE_HOLDING_TWO_HAND_WEAPON)) {
                    sp3C = &gPlayerAnim_link_fighter_power_jump_kiru_end;
                }

                Player_SetupReturnToStandStillSetAnim(this, sp3C, play);

                this->skelAnime.moveFlags = sp43;
                this->stateFlags3 |= PLAYER_STATE3_ENDING_MELEE_ATTACK;
            }
        } else if (this->heldItemAction == PLAYER_IA_HAMMER) {
            if ((this->meleeWeaponAnimation == 0x16) || (this->meleeWeaponAnimation == 0x13)) {
                static Vec3f zeroVec = { 0.0f, 0.0f, 0.0f };
                Vec3f shockwavePos;
                f32 sp2C;

                shockwavePos.y = Player_RaycastFloorWithOffset2(play, this, &D_80854A40, &shockwavePos);
                sp2C = this->actor.world.pos.y - shockwavePos.y;

                Math_ScaledStepToS(&this->actor.focus.rot.x, Math_Atan2S(45.0f, sp2C), 800);
                Player_UpdateLookAngles(this, 1);

                if ((((this->meleeWeaponAnimation == 0x16) && LinkAnimation_OnFrame(&this->skelAnime, 7.0f)) ||
                     ((this->meleeWeaponAnimation == 0x13) && LinkAnimation_OnFrame(&this->skelAnime, 2.0f))) &&
                    (sp2C > -40.0f) && (sp2C < 40.0f)) {
                    func_80842A28(play, this);
                    EffectSsBlast_SpawnWhiteShockwave(play, &shockwavePos, &zeroVec, &zeroVec);
                }
            }
        }
    }
}

void Player_MeleeWeaponRebound(Player* this, PlayState* play) {
    LinkAnimation_Update(play, &this->skelAnime);
    Player_StepLinearVelocityToZero(this);

    if (this->skelAnime.curFrame >= 6.0f) {
        Player_ReturnToStandStill(this, play);
    }
}

void Player_ChooseFaroresWindOption(Player* this, PlayState* play) {
    this->stateFlags2 |= PLAYER_STATE2_DISABLE_MOVE_ROTATION_WHILE_Z_TARGETING;

    LinkAnimation_Update(play, &this->skelAnime);
    Player_SetupCurrentUpperAction(this, play);

    if (this->genericTimer == 0) {
        Message_StartTextbox(play, 0x3B, &this->actor);
        this->genericTimer = 1;
        return;
    }

    if (Message_GetState(&play->msgCtx) == TEXT_STATE_CLOSING) {
        s32 respawnData = gSaveContext.respawn[RESPAWN_MODE_TOP].data;

        if (play->msgCtx.choiceIndex == 0) { //Returns to FW
            gSaveContext.respawnFlag = 3;
            play->sceneLoadFlag = 0x14;
            play->nextEntranceIndex = gSaveContext.respawn[RESPAWN_MODE_TOP].entranceIndex;
            play->fadeTransition = 5;
            func_80088AF0(play);
            return;
        }

        if (play->msgCtx.choiceIndex == 1) { //Unsets FW
            gSaveContext.respawn[RESPAWN_MODE_TOP].data = -respawnData;
            gSaveContext.fw.set = 0;
            func_80078914(&gSaveContext.respawn[RESPAWN_MODE_TOP].pos, NA_SE_PL_MAGIC_WIND_VANISH);
        }

        Player_SetupStandingStillMorph(this, play);
        func_8005B1A4(Play_GetCamera(play, 0));
    }
}

void Player_SpawnFromFaroresWind(Player* this, PlayState* play) {
    s32 respawnData = gSaveContext.respawn[RESPAWN_MODE_TOP].data;

    if (this->genericTimer > 20) {
        this->actor.draw = Player_Draw;
        this->actor.world.pos.y += 60.0f;
        Player_SetupFallFromLedge(this, play);
        return;
    }

    if (this->genericTimer++ == 20) {
        gSaveContext.respawn[RESPAWN_MODE_TOP].data = respawnData + 1;
        func_80078914(&gSaveContext.respawn[RESPAWN_MODE_TOP].pos, NA_SE_PL_MAGIC_WIND_WARP);
    }
}

static LinkAnimationHeader* D_80854A58[] = {
    &gPlayerAnim_link_magic_kaze1,
    &gPlayerAnim_link_magic_honoo1,
    &gPlayerAnim_link_magic_tamashii1,
};

static LinkAnimationHeader* D_80854A64[] = {
    &gPlayerAnim_link_magic_kaze2,
    &gPlayerAnim_link_magic_honoo2,
    &gPlayerAnim_link_magic_tamashii2,
};

static LinkAnimationHeader* D_80854A70[] = {
    &gPlayerAnim_link_magic_kaze3,
    &gPlayerAnim_link_magic_honoo3,
    &gPlayerAnim_link_magic_tamashii3,
};

static u8 D_80854A7C[] = { 70, 10, 10 };

static PlayerAnimSfxEntry D_80854A80[] = {
    { NA_SE_PL_SKIP, 0x814 },
    { NA_SE_VO_LI_SWORD_N, 0x2014 },
    { 0, -0x301A },
};

static PlayerAnimSfxEntry D_80854A8C[][2] = {
    {
        { 0, 0x4014 },
        { NA_SE_VO_LI_MAGIC_FROL, -0x201E },
    },
    {
        { 0, 0x4014 },
        { NA_SE_VO_LI_MAGIC_NALE, -0x202C },
    },
    {
        { NA_SE_VO_LI_MAGIC_ATTACK, 0x2014 },
        { NA_SE_IT_SWORD_SWING_HARD, -0x814 },
    },
};

void Player_UpdateMagicSpell(Player* this, PlayState* play) {
    u8 isFastFarores = CVarGetInteger("gFastFarores", 0) && this->itemAction == PLAYER_IA_FARORES_WIND;
    if (LinkAnimation_Update(play, &this->skelAnime)) {
        if (this->genericVar < 0) {
            if ((this->itemAction == PLAYER_IA_NAYRUS_LOVE) || isFastFarores || (gSaveContext.magicState == 0)) {
                Player_ReturnToStandStill(this, play);
                func_8005B1A4(Play_GetCamera(play, 0));
            }
        } else {
            if (this->genericTimer == 0) {
                LinkAnimation_PlayOnceSetSpeed(play, &this->skelAnime, D_80854A58[this->genericVar], 0.83f * (isFastFarores ? 2 : 1));

                if (func_80846A00(play, this, this->genericVar) != NULL) {
                    this->stateFlags1 |= PLAYER_STATE1_SKIP_OTHER_ACTORS_UPDATE | PLAYER_STATE1_IN_CUTSCENE;
                    if ((this->genericVar != 0) || (gSaveContext.respawn[RESPAWN_MODE_TOP].data <= 0)) {
                        gSaveContext.magicState = 1;
                    }
                } else {
                    func_800876C8(play);
                }
            } else {
                LinkAnimation_PlayLoopSetSpeed(play, &this->skelAnime, D_80854A64[this->genericVar], 0.83f * (isFastFarores ? 2 : 1));

                if (this->genericVar == 0) {
                    this->genericTimer = -10;
                }
            }

            this->genericTimer++;
        }
    } else {
        if (this->genericTimer < 0) {
            this->genericTimer++;

            if (this->genericTimer == 0) {
                gSaveContext.respawn[RESPAWN_MODE_TOP].data = 1;
                Play_SetupRespawnPoint(play, RESPAWN_MODE_TOP, 0x6FF);
                gSaveContext.fw.set = 1;
                gSaveContext.fw.pos.x = gSaveContext.respawn[RESPAWN_MODE_DOWN].pos.x;
                gSaveContext.fw.pos.y = gSaveContext.respawn[RESPAWN_MODE_DOWN].pos.y;
                gSaveContext.fw.pos.z = gSaveContext.respawn[RESPAWN_MODE_DOWN].pos.z;
                gSaveContext.fw.yaw = gSaveContext.respawn[RESPAWN_MODE_DOWN].yaw;
                gSaveContext.fw.playerParams = 0x6FF;
                gSaveContext.fw.entranceIndex = gSaveContext.respawn[RESPAWN_MODE_DOWN].entranceIndex;
                gSaveContext.fw.roomIndex = gSaveContext.respawn[RESPAWN_MODE_DOWN].roomIndex;
                gSaveContext.fw.tempSwchFlags = gSaveContext.respawn[RESPAWN_MODE_DOWN].tempSwchFlags;
                gSaveContext.fw.tempCollectFlags = gSaveContext.respawn[RESPAWN_MODE_DOWN].tempCollectFlags;
                this->genericTimer = 2;
            }
        } else if (this->genericVar >= 0) {
            if (this->genericTimer == 0) {
                Player_PlayAnimSfx(this, D_80854A80);
            } else if (this->genericTimer == 1) {
                Player_PlayAnimSfx(this, D_80854A8C[this->genericVar]);
                if ((this->genericVar == 2) && LinkAnimation_OnFrame(&this->skelAnime, 30.0f)) {
                    this->stateFlags1 &= ~(PLAYER_STATE1_SKIP_OTHER_ACTORS_UPDATE | PLAYER_STATE1_IN_CUTSCENE);
                }
            } else if ((isFastFarores ? 10 : D_80854A7C[this->genericVar]) < this->genericTimer++) {
                LinkAnimation_PlayOnceSetSpeed(play, &this->skelAnime, D_80854A70[this->genericVar], 0.83f * (isFastFarores ? 2 : 1));
                this->currentYaw = this->actor.shape.rot.y;
                this->genericVar = -1;
            }
        }
    }

    Player_StepLinearVelocityToZero(this);
}

void Player_MoveAlongHookshotPath(Player* this, PlayState* play) {
    f32 temp;

    this->stateFlags2 |= PLAYER_STATE2_DISABLE_MOVE_ROTATION_WHILE_Z_TARGETING;

    if (LinkAnimation_Update(play, &this->skelAnime)) {
        Player_PlayAnimLoop(play, this, &gPlayerAnim_link_hook_fly_wait);
    }

    Math_Vec3f_Sum(&this->actor.world.pos, &this->actor.velocity, &this->actor.world.pos);

    if (Player_EndHookshotMove(this)) {
        Math_Vec3f_Copy(&this->actor.prevPos, &this->actor.world.pos);
        func_80847BA0(play, this);

        temp = this->actor.world.pos.y - this->actor.floorHeight;
        if (temp > 20.0f) {
            temp = 20.0f;
        }

        this->actor.world.rot.x = this->actor.shape.rot.x = 0;
        this->actor.world.pos.y -= temp;
        this->linearVelocity = 1.0f;
        this->actor.velocity.y = 0.0f;
        Player_SetupFallFromLedge(this, play);
        this->stateFlags2 &= ~PLAYER_STATE2_DIVING;
        this->actor.bgCheckFlags |= 1;
        this->stateFlags1 |= PLAYER_STATE1_END_HOOKSHOT_MOVE;
        return;
    }

    if ((this->skelAnime.animation != &gPlayerAnim_link_hook_fly_start) || (4.0f <= this->skelAnime.curFrame)) {
        this->actor.gravity = 0.0f;
        Math_ScaledStepToS(&this->actor.shape.rot.x, this->actor.world.rot.x, 0x800);
        Player_RequestRumble(this, 100, 2, 100, 0);
    }
}

void Player_CastFishingRod(Player* this, PlayState* play) {
    if ((this->genericTimer != 0) && ((this->unk_858 != 0.0f) || (this->unk_85C != 0.0f))) {
        f32 updateScale = R_UPDATE_RATE * 0.5f;

        this->skelAnime.curFrame += this->skelAnime.playSpeed * updateScale;
        if (this->skelAnime.curFrame >= this->skelAnime.animLength) {
            this->skelAnime.curFrame -= this->skelAnime.animLength;
        }

        LinkAnimation_BlendToJoint(
            play, &this->skelAnime, &gPlayerAnim_link_fishing_wait, this->skelAnime.curFrame,
            (this->unk_858 < 0.0f) ? &gPlayerAnim_link_fishing_reel_left : &gPlayerAnim_link_fishing_reel_right, 5.0f,
            fabsf(this->unk_858), this->blendTable);
        LinkAnimation_BlendToMorph(
            play, &this->skelAnime, &gPlayerAnim_link_fishing_wait, this->skelAnime.curFrame,
            (this->unk_85C < 0.0f) ? &gPlayerAnim_link_fishing_reel_up : &gPlayerAnim_link_fishing_reel_down, 5.0f,
            fabsf(this->unk_85C), D_80858AD8);
        LinkAnimation_InterpJointMorph(play, &this->skelAnime, 0.5f);
    } else if (LinkAnimation_Update(play, &this->skelAnime)) {
        this->fpsItemType = 2;
        Player_PlayAnimLoop(play, this, &gPlayerAnim_link_fishing_wait);
        this->genericTimer = 1;
    }

    Player_StepLinearVelocityToZero(this);

    if (this->fpsItemType == 0) {
        Player_SetupStandingStillMorph(this, play);
    } else if (this->fpsItemType == 3) {
        Player_SetActionFunc(play, this, Player_ReleaseCaughtFish, 0);
        Player_ChangeAnimMorphToLastFrame(play, this, &gPlayerAnim_link_fishing_fish_catch);
    }
}

void Player_ReleaseCaughtFish(Player* this, PlayState* play) {
    if (LinkAnimation_Update(play, &this->skelAnime) && (this->fpsItemType == 0)) {
        Player_SetupReturnToStandStillSetAnim(this, &gPlayerAnim_link_fishing_fish_catch_end, play);
    }
}

static void (*D_80854AA4[])(PlayState*, Player*, void*) = {
    NULL,
    Player_AnimPlaybackType0,
    Player_AnimPlaybackType1,
    Player_AnimPlaybackType2,
    Player_AnimPlaybackType3,
    Player_AnimPlaybackType4,
    Player_AnimPlaybackType5,
    Player_AnimPlaybackType6,
    Player_AnimPlaybackType7,
    Player_AnimPlaybackType8,
    Player_AnimPlaybackType9,
    Player_AnimPlaybackType10,
    Player_AnimPlaybackType11,
    Player_AnimPlaybackType12,
    Player_AnimPlaybackType13,
    Player_AnimPlaybackType14,
    Player_AnimPlaybackType15,
    Player_AnimPlaybackType16,
    Player_AnimPlaybackType17,
};

static PlayerAnimSfxEntry D_80854AF0[] = {
    { 0, 0x2822 },
    { NA_SE_PL_CALM_HIT, 0x82D },
    { NA_SE_PL_CALM_HIT, 0x833 },
    { NA_SE_PL_CALM_HIT, -0x840 },
};

static PlayerAnimSfxEntry D_80854B00[] = {
    { NA_SE_VO_LI_SURPRISE, 0x2003 }, { 0, 0x300F }, { 0, 0x3018 }, { 0, 0x301E }, { NA_SE_VO_LI_FALL_L, -0x201F },
};

static PlayerAnimSfxEntry D_80854B14[] = {
    { 0, -0x300A },
};

static struct_80854B18 D_80854B18[] = {
    { 0, NULL },
    { -1, Player_CutsceneSetupIdle },
    { 2, &gPlayerAnim_link_demo_goma_furimuki },
    { 0, NULL },
    { 0, NULL },
    { 3, &gPlayerAnim_link_demo_bikkuri },
    { 0, NULL },
    { 0, NULL },
    { -1, Player_CutsceneSetupIdle },
    { 2, &gPlayerAnim_link_demo_furimuki },
    { -1, Player_CutsceneSetupEnterWarp },
    { 3, &gPlayerAnim_link_demo_warp },
    { -1, Player_CutsceneSetupFightStance },
    { 7, &gPlayerAnim_clink_demo_get1 },
    { 5, &gPlayerAnim_clink_demo_get2 },
    { 5, &gPlayerAnim_clink_demo_get3 },
    { 5, &gPlayerAnim_clink_demo_standup },
    { 7, &gPlayerAnim_clink_demo_standup_wait },
    { -1, Player_CutsceneSetupSwordPedestal },
    { 2, &gPlayerAnim_link_demo_baru_op1 },
    { 2, &gPlayerAnim_link_demo_baru_op3 },
    { 0, NULL },
    { -1, Player_CutsceneSetupWarpToSages },
    { 3, &gPlayerAnim_link_demo_jibunmiru },
    { 9, &gPlayerAnim_link_normal_back_downA },
    { 2, &gPlayerAnim_link_normal_back_down_wake },
    { -1, Player_CutsceneSetupStartPlayOcarina },
    { 2, &gPlayerAnim_link_normal_okarina_end },
    { 3, &gPlayerAnim_link_demo_get_itemA },
    { -1, Player_CutsceneSetupIdle },
    { 2, &gPlayerAnim_link_normal_normal2fighter_free },
    { 0, NULL },
    { 0, NULL },
    { 5, &gPlayerAnim_clink_demo_atozusari },
    { -1, Player_CutsceneSetupSwimIdle },
    { -1, Player_CutsceneSetupGetItemInWater },
    { 5, &gPlayerAnim_clink_demo_bashi },
    { 16, &gPlayerAnim_link_normal_hang_up_down },
    { -1, Player_CutsceneSetupSleepingRestless },
    { -1, Player_CutsceneSetupSleeping },
    { 6, &gPlayerAnim_clink_op3_okiagari },
    { 6, &gPlayerAnim_clink_op3_tatiagari },
    { -1, Player_CutsceneSetupBlownBackward },
    { 5, &gPlayerAnim_clink_demo_miokuri },
    { -1, Player_CutsceneSetupIdle3 },
    { -1, Player_CutsceneSetupStop },
    { -1, Player_CutsceneSetDraw },
    { 5, &gPlayerAnim_clink_demo_nozoki },
    { 5, &gPlayerAnim_clink_demo_koutai },
    { -1, Player_CutsceneSetupIdle },
    { 5, &gPlayerAnim_clink_demo_koutai_kennuki },
    { 5, &gPlayerAnim_link_demo_kakeyori },
    { 5, &gPlayerAnim_link_demo_kakeyori_mimawasi },
    { 5, &gPlayerAnim_link_demo_kakeyori_miokuri },
    { 3, &gPlayerAnim_link_demo_furimuki2 },
    { 3, &gPlayerAnim_link_demo_kaoage },
    { 4, &gPlayerAnim_link_demo_kaoage_wait },
    { 3, &gPlayerAnim_clink_demo_mimawasi },
    { 3, &gPlayerAnim_link_demo_nozokikomi },
    { 6, &gPlayerAnim_kolink_odoroki_demo },
    { 6, &gPlayerAnim_link_shagamu_demo },
    { 14, &gPlayerAnim_link_okiru_demo },
    { 3, &gPlayerAnim_link_okiru_demo },
    { 5, &gPlayerAnim_link_fighter_power_kiru_start },
    { 16, &gPlayerAnim_demo_link_nwait },
    { 15, &gPlayerAnim_demo_link_tewatashi },
    { 15, &gPlayerAnim_demo_link_orosuu },
    { 3, &gPlayerAnim_d_link_orooro },
    { 3, &gPlayerAnim_d_link_imanodare },
    { 3, &gPlayerAnim_link_hatto_demo },
    { 6, &gPlayerAnim_o_get_mae },
    { 6, &gPlayerAnim_o_get_ato },
    { 6, &gPlayerAnim_om_get_mae },
    { 6, &gPlayerAnim_nw_modoru },
    { 3, &gPlayerAnim_link_demo_gurad },
    { 3, &gPlayerAnim_link_demo_look_hand },
    { 4, &gPlayerAnim_link_demo_sita_wait },
    { 3, &gPlayerAnim_link_demo_ue },
    { 3, &gPlayerAnim_Link_muku },
    { 3, &gPlayerAnim_Link_miageru },
    { 6, &gPlayerAnim_Link_ha },
    { 3, &gPlayerAnim_L_1kyoro },
    { 3, &gPlayerAnim_L_2kyoro },
    { 3, &gPlayerAnim_L_sagaru },
    { 3, &gPlayerAnim_L_bouzen },
    { 3, &gPlayerAnim_L_kamaeru },
    { 3, &gPlayerAnim_L_hajikareru },
    { 3, &gPlayerAnim_L_ken_miru },
    { 3, &gPlayerAnim_L_mukinaoru },
    { -1, Player_CutsceneSetupSpinAttackIdle },
    { 3, &gPlayerAnim_link_wait_itemD1_20f },
    { -1, Player_SetupDoNothing4 },
    { -1, Player_CutsceneSetupKnockedToGroundDamaged },
    { 3, &gPlayerAnim_link_normal_wait_typeB_20f },
    { -1, Player_CutsceneSetupGetSwordBack },
    { 3, &gPlayerAnim_link_demo_kousan },
    { 3, &gPlayerAnim_link_demo_return_to_past },
    { 3, &gPlayerAnim_link_last_hit_motion1 },
    { 3, &gPlayerAnim_link_last_hit_motion2 },
    { 3, &gPlayerAnim_link_demo_zeldamiru },
    { 3, &gPlayerAnim_link_demo_kenmiru1 },
    { 3, &gPlayerAnim_link_demo_kenmiru2 },
    { 3, &gPlayerAnim_link_demo_kenmiru2_modori },
};

static struct_80854B18 D_80854E50[] = {
    { 0, NULL },
    { -1, Player_CutsceneIdle },
    { -1, Player_CutsceneTurnAroundSurprisedShort },
    { -1, Player_CutsceneUnk3Update },
    { -1, Player_CutsceneUnk4Update },
    { 11, NULL },
    { -1, Player_CutsceneUnk6Update },
    { -1, Player_CutsceneEnd },
    { -1, Player_CutsceneWait },
    { -1, Player_CutsceneTurnAroundSurprisedLong },
    { -1, Player_CutsceneEnterWarp },
    { -1, Player_CutsceneRaisedByWarp },
    { -1, Player_CutsceneFightStance },
    { 11, NULL },
    { 11, NULL },
    { 11, NULL },
    { 18, D_80854AF0 },
    { 11, NULL },
    { -1, Player_CutsceneSwordPedestal },
    { 12, &gPlayerAnim_link_demo_baru_op2 },
    { 11, NULL },
    { 0, NULL },
    { -1, Player_CutsceneWarpToSages },
    { 11, NULL },
    { -1, Player_CutsceneKnockedToGround },
    { 11, NULL },
    { 17, &gPlayerAnim_link_normal_okarina_swing },
    { 11, NULL },
    { 11, NULL },
    { 11, NULL },
    { -1, Player_CutsceneDrawAndBrandishSword },
    { -1, Player_CutsceneCloseEyes },
    { -1, Player_CutsceneOpenEyes },
    { 18, D_80854B00 },
    { -1, Player_CutsceneSurfaceFromDive },
    { 11, NULL },
    { 11, NULL },
    { 11, NULL },
    { 11, NULL },
    { -1, Player_CutsceneSleeping },
    { -1, Player_CutsceneAwaken },
    { -1, Player_CutsceneGetOffBed },
    { -1, Player_CutsceneBlownBackward },
    { 13, &gPlayerAnim_clink_demo_miokuri_wait },
    { -1, Player_CutsceneIdle3 },
    { 0, NULL },
    { 0, NULL },
    { 11, NULL },
    { -1, Player_CutsceneStepBackCautiously },
    { -1, Player_CutsceneWait },
    { -1, Player_CutsceneDrawSwordChild },
    { 13, &gPlayerAnim_link_demo_kakeyori_wait },
    { -1, Player_CutsceneDesperateLookAtZeldasCrystal },
    { 13, &gPlayerAnim_link_demo_kakeyori_miokuri_wait },
    { -1, Player_CutsceneTurnAroundSlowly },
    { 11, NULL },
    { 11, NULL },
    { 12, &gPlayerAnim_clink_demo_mimawasi_wait },
    { -1, Player_CutsceneInspectGroundCarefully },
    { 11, NULL },
    { 18, D_80854B14 },
    { 11, NULL },
    { 11, NULL },
    { 11, NULL },
    { 11, NULL },
    { -1, Player_CutsceneStartPassOcarina },
    { 17, &gPlayerAnim_demo_link_nwait },
    { 12, &gPlayerAnim_d_link_orowait },
    { 12, &gPlayerAnim_demo_link_nwait },
    { 11, NULL },
    { -1, Player_LearnOcarinaSong },
    { 17, &gPlayerAnim_sude_nwait },
    { -1, Player_LearnOcarinaSong },
    { 17, &gPlayerAnim_sude_nwait },
    { 12, &gPlayerAnim_link_demo_gurad_wait },
    { 12, &gPlayerAnim_link_demo_look_hand_wait },
    { 11, NULL },
    { 12, &gPlayerAnim_link_demo_ue_wait },
    { 12, &gPlayerAnim_Link_m_wait },
    { 13, &gPlayerAnim_Link_ue_wait },
    { 12, &gPlayerAnim_Link_otituku_w },
    { 12, &gPlayerAnim_L_kw },
    { 11, NULL },
    { 11, NULL },
    { 11, NULL },
    { 11, NULL },
    { -1, Player_CutsceneSwordKnockedFromHand },
    { 11, NULL },
    { 12, &gPlayerAnim_L_kennasi_w },
    { -1, Player_CutsceneSpinAttackIdle },
    { -1, Player_CutsceneInspectWeapon },
    { -1, Player_DoNothing5 },
    { -1, Player_CutsceneKnockedToGroundDamaged },
    { 11, NULL },
    { 11, NULL },
    { 11, NULL },
    { -1, Player_CutsceneGetSwordBack },
    { -1, Player_CutsceneGanonKillCombo },
    { -1, Player_CutsceneGanonKillCombo },
    { 12, &gPlayerAnim_link_demo_zeldamiru_wait },
    { 12, &gPlayerAnim_link_demo_kenmiru1_wait },
    { 12, &gPlayerAnim_link_demo_kenmiru2_wait },
    { 12, &gPlayerAnim_demo_link_nwait },
};

void func_80850ED8(PlayState* play, Player* this, LinkAnimationHeader* anim) {
    Player_ClearRootLimbPosY(this);
    Player_ChangeAnimMorphToLastFrame(play, this, anim);
    Player_StopMovement(this);
}

void func_80850F1C(PlayState* play, Player* this, LinkAnimationHeader* anim) {
    Player_ClearRootLimbPosY(this);
    LinkAnimation_Change(play, &this->skelAnime, anim, (2.0f / 3.0f), 0.0f, Animation_GetLastFrame(anim),
                         ANIMMODE_ONCE, -8.0f);
    Player_StopMovement(this);
}

void func_80850F9C(PlayState* play, Player* this, LinkAnimationHeader* anim) {
    Player_ClearRootLimbPosY(this);
    LinkAnimation_Change(play, &this->skelAnime, anim, (2.0f / 3.0f), 0.0f, 0.0f, ANIMMODE_LOOP, -8.0f);
    Player_StopMovement(this);
}

void Player_AnimPlaybackType0(PlayState* play, Player* this, void* anim) {
    Player_StopMovement(this);
}

void Player_AnimPlaybackType1(PlayState* play, Player* this, void* anim) {
    func_80850ED8(play, this, anim);
}

void Player_AnimPlaybackType13(PlayState* play, Player* this, void* anim) {
    Player_ClearRootLimbPosY(this);
    Player_ChangeAnimOnce(play, this, anim);
    Player_StopMovement(this);
}

void Player_AnimPlaybackType2(PlayState* play, Player* this, void* anim) {
    func_80850F1C(play, this, anim);
}

void Player_AnimPlaybackType3(PlayState* play, Player* this, void* anim) {
    func_80850F9C(play, this, anim);
}

void Player_AnimPlaybackType4(PlayState* play, Player* this, void* anim) {
    Player_PlayAnimOnceWithMovementPresetFlagsSlowed(play, this, anim);
}

void Player_AnimPlaybackType5(PlayState* play, Player* this, void* anim) {
    Player_PlayAnimOnceWithMovement(play, this, anim, 0x9C);
}

void Player_AnimPlaybackType6(PlayState* play, Player* this, void* anim) {
    Player_PlayAnimLoopWithMovementPresetFlagsSlowed(play, this, anim);
}

void Player_AnimPlaybackType7(PlayState* play, Player* this, void* anim) {
    Player_PlayAnimLoopWithMovement(play, this, anim, 0x9C);
}

void Player_AnimPlaybackType8(PlayState* play, Player* this, void* anim) {
    Player_PlayAnimOnce(play, this, anim);
}

void Player_AnimPlaybackType9(PlayState* play, Player* this, void* anim) {
    Player_PlayAnimLoop(play, this, anim);
}

void Player_AnimPlaybackType14(PlayState* play, Player* this, void* anim) {
    Player_PlayAnimOnceSlowed(play, this, anim);
}

void Player_AnimPlaybackType15(PlayState* play, Player* this, void* anim) {
    Player_PlayAnimLoopSlowed(play, this, anim);
}

void Player_AnimPlaybackType10(PlayState* play, Player* this, void* anim) {
    LinkAnimation_Update(play, &this->skelAnime);
}

void Player_AnimPlaybackType11(PlayState* play, Player* this, void* anim) {
    if (LinkAnimation_Update(play, &this->skelAnime)) {
        func_80850F9C(play, this, anim);
        this->genericTimer = 1;
    }
}

void Player_AnimPlaybackType16(PlayState* play, Player* this, void* anim) {
    if (LinkAnimation_Update(play, &this->skelAnime)) {
        Player_EndAnimMovement(this);
        Player_PlayAnimLoopSlowed(play, this, anim);
    }
}

void Player_AnimPlaybackType12(PlayState* play, Player* this, void* anim) {
    if (LinkAnimation_Update(play, &this->skelAnime)) {
        Player_PlayAnimLoopWithMovementPresetFlagsSlowed(play, this, anim);
        this->genericTimer = 1;
    }
}

void Player_AnimPlaybackType17(PlayState* play, Player* this, void* arg2) {
    LinkAnimation_Update(play, &this->skelAnime);
    Player_PlayAnimSfx(this, arg2);
}

void func_80851314(Player* this) {
    if ((this->csTargetActor == NULL) || (this->csTargetActor->update == NULL)) {
        this->csTargetActor = NULL;
    }

    this->targetActor = this->csTargetActor;

    if (this->targetActor != NULL) {
        this->actor.shape.rot.y = Player_LookAtTargetActor(this, 0);
    }
}

void Player_CutsceneSetupSwimIdle(PlayState* play, Player* this, CsCmdActorAction* arg2) {
    this->stateFlags1 |= PLAYER_STATE1_SWIMMING;
    this->stateFlags2 |= PLAYER_STATE2_DIVING;
    this->stateFlags1 &= ~(PLAYER_STATE1_JUMPING | PLAYER_STATE1_FREEFALLING);

    Player_PlayAnimLoop(play, this, &gPlayerAnim_link_swimer_swim);
}

void Player_CutsceneSurfaceFromDive(PlayState* play, Player* this, CsCmdActorAction* arg2) {
    this->actor.gravity = 0.0f;

    if (this->genericVar == 0) {
        if (Player_SetupDive(play, this, NULL)) {
            this->genericVar = 1;
        } else {
            func_8084B158(play, this, NULL, fabsf(this->actor.velocity.y));
            Math_ScaledStepToS(&this->shapePitchOffset, -10000, 800);
            Player_UpdateSwimMovement(this, &this->actor.velocity.y, 4.0f, this->currentYaw);
        }
        return;
    }

    if (LinkAnimation_Update(play, &this->skelAnime)) {
        if (this->genericVar == 1) {
            Player_ChangeAnimLongMorphLoop(play, this, &gPlayerAnim_link_swimer_swim_wait);
        } else {
            Player_PlayAnimLoop(play, this, &gPlayerAnim_link_swimer_swim_wait);
        }
    }

    func_8084B000(this);
    Player_UpdateSwimMovement(this, &this->linearVelocity, 0.0f, this->actor.shape.rot.y);
}

void Player_CutsceneIdle(PlayState* play, Player* this, CsCmdActorAction* arg2) {
    func_80851314(this);

    if (Player_IsSwimming(this)) {
        Player_CutsceneSurfaceFromDive(play, this, 0);
        return;
    }

    LinkAnimation_Update(play, &this->skelAnime);

    if (Player_IsShootingHookshot(this) || (this->stateFlags1 & PLAYER_STATE1_HOLDING_ACTOR)) {
        Player_SetupCurrentUpperAction(this, play);
        return;
    }

    if ((this->interactRangeActor != NULL) && (this->interactRangeActor->textId == 0xFFFF)) {
        Player_SetupGetItemOrHoldBehavior(this, play);
    }
}

void Player_CutsceneTurnAroundSurprisedShort(PlayState* play, Player* this, CsCmdActorAction* arg2) {
    LinkAnimation_Update(play, &this->skelAnime);
}

void Player_CutsceneSetupIdle(PlayState* play, Player* this, CsCmdActorAction* arg2) {
    LinkAnimationHeader* anim;

    if (Player_IsSwimming(this)) {
        Player_CutsceneSetupSwimIdle(play, this, 0);
        return;
    }

    anim = GET_PLAYER_ANIM(PLAYER_ANIMGROUP_RELAX, this->modelAnimType);

    if ((this->csAction == 6) || (this->csAction == 0x2E)) {
        Player_PlayAnimOnce(play, this, anim);
    } else {
        Player_ClearRootLimbPosY(this);
        LinkAnimation_Change(play, &this->skelAnime, anim, (2.0f / 3.0f), 0.0f, Animation_GetLastFrame(anim),
                             ANIMMODE_LOOP, -4.0f);
    }

    Player_StopMovement(this);
}

void Player_CutsceneWait(PlayState* play, Player* this, CsCmdActorAction* arg2) {
    if (func_8084B3CC(play, this) == 0) {
        if ((this->csMode == 0x31) && (play->csCtx.state == CS_STATE_IDLE)) {
            func_8002DF54(play, NULL, 7);
            return;
        }

        if (Player_IsSwimming(this) != 0) {
            Player_CutsceneSurfaceFromDive(play, this, 0);
            return;
        }

        LinkAnimation_Update(play, &this->skelAnime);

        if (Player_IsShootingHookshot(this) || (this->stateFlags1 & PLAYER_STATE1_HOLDING_ACTOR)) {
            Player_SetupCurrentUpperAction(this, play);
        }
    }
}

static PlayerAnimSfxEntry D_80855188[] = {
    { 0, 0x302A },
    { 0, -0x3030 },
};

void Player_CutsceneTurnAroundSurprisedLong(PlayState* play, Player* this, CsCmdActorAction* arg2) {
    LinkAnimation_Update(play, &this->skelAnime);
    Player_PlayAnimSfx(this, D_80855188);
}

void Player_CutsceneSetupEnterWarp(PlayState* play, Player* this, CsCmdActorAction* arg2) {
    this->stateFlags1 &= ~PLAYER_STATE1_AWAITING_THROWN_BOOMERANG;

    this->currentYaw = this->actor.shape.rot.y = this->actor.world.rot.y =
        Math_Vec3f_Yaw(&this->actor.world.pos, &this->csStartPos);

    if (this->linearVelocity <= 0.0f) {
        this->linearVelocity = 0.1f;
    } else if (this->linearVelocity > 2.5f) {
        this->linearVelocity = 2.5f;
    }
}

void Player_CutsceneEnterWarp(PlayState* play, Player* this, CsCmdActorAction* arg2) {
    f32 sp1C = 2.5f;

    func_80845BA0(play, this, &sp1C, 10);

    if (play->sceneNum == SCENE_BDAN_BOSS) {
        if (this->genericTimer == 0) {
            if (Message_GetState(&play->msgCtx) == TEXT_STATE_NONE) {
                return;
            }
        } else {
            if (Message_GetState(&play->msgCtx) != TEXT_STATE_NONE) {
                return;
            }
        }
    }

    this->genericTimer++;
    if (this->genericTimer > 20) {
        this->csMode = 0xB;
    }
}

void Player_CutsceneSetupFightStance(PlayState* play, Player* this, CsCmdActorAction* arg2) {
    Player_SetupUnfriendlyZTarget(this, play);
}

void Player_CutsceneFightStance(PlayState* play, Player* this, CsCmdActorAction* arg2) {
    func_80851314(this);

    if (this->genericTimer != 0) {
        if (LinkAnimation_Update(play, &this->skelAnime)) {
            Player_PlayAnimLoop(play, this, Player_GetFightingRightAnim(this));
            this->genericTimer = 0;
        }

        Player_ResetLeftRightBlendWeight(this);
    } else {
        func_808401B0(play, this);
    }
}

void Player_CutsceneUnk3Update(PlayState* play, Player* this, CsCmdActorAction* arg2) {
    func_80845964(play, this, arg2, 0.0f, 0, 0);
}

void Player_CutsceneUnk4Update(PlayState* play, Player* this, CsCmdActorAction* arg2) {
    func_80845964(play, this, arg2, 0.0f, 0, 1);
}

// unused
static LinkAnimationHeader* D_80855190[] = {
    &gPlayerAnim_link_demo_back_to_past,
    &gPlayerAnim_clink_demo_goto_future,
};

static Vec3f D_80855198 = { -1.0f, 70.0f, 20.0f };

void Player_CutsceneSetupSwordPedestal(PlayState* play, Player* this, CsCmdActorAction* arg2) {
    Math_Vec3f_Copy(&this->actor.world.pos, &D_80855198);
    this->actor.shape.rot.y = -0x8000;
    Player_PlayAnimOnceSlowed(play, this, this->ageProperties->unk_9C);
    Player_SetupAnimMovement(play, this, 0x28F);
}

static struct_808551A4 D_808551A4[] = {
    { NA_SE_IT_SWORD_PUTAWAY_STN, 0 },
    { NA_SE_IT_SWORD_STICK_STN, NA_SE_VO_LI_SWORD_N },
};

static PlayerAnimSfxEntry D_808551AC[] = {
    { 0, 0x401D },
    { 0, -0x4027 },
};

void Player_CutsceneSwordPedestal(PlayState* play, Player* this, CsCmdActorAction* arg2) {
    struct_808551A4* sp2C;
    Gfx** dLists;

    LinkAnimation_Update(play, &this->skelAnime);

    if ((LINK_IS_ADULT && LinkAnimation_OnFrame(&this->skelAnime, 70.0f)) ||
        (!LINK_IS_ADULT && LinkAnimation_OnFrame(&this->skelAnime, 87.0f))) {
        sp2C = &D_808551A4[gSaveContext.linkAge];
        this->interactRangeActor->parent = &this->actor;

        if (!LINK_IS_ADULT) {
            dLists = D_80125DE8;
        } else {
            dLists = D_80125E18;
        }
        this->leftHandDLists = &dLists[gSaveContext.linkAge];

        func_8002F7DC(&this->actor, sp2C->unk_00);
        if (!LINK_IS_ADULT) {
            Player_PlayVoiceSfxForAge(this, sp2C->unk_02);
        }
    } else if (LINK_IS_ADULT) {
        if (LinkAnimation_OnFrame(&this->skelAnime, 66.0f)) {
            Player_PlayVoiceSfxForAge(this, NA_SE_VO_LI_SWORD_L);
        }
    } else {
        Player_PlayAnimSfx(this, D_808551AC);
    }
}

void Player_CutsceneSetupWarpToSages(PlayState* play, Player* this, CsCmdActorAction* arg2) {
    LinkAnimation_Change(play, &this->skelAnime, &gPlayerAnim_link_demo_warp, -(2.0f / 3.0f), 12.0f, 12.0f,
                         ANIMMODE_ONCE, 0.0f);
}

static PlayerAnimSfxEntry D_808551B4[] = {
    { 0, -0x281E },
};

void Player_CutsceneWarpToSages(PlayState* play, Player* this, CsCmdActorAction* arg2) {
    LinkAnimation_Update(play, &this->skelAnime);

    this->genericTimer++;

    if (this->genericTimer >= 180) {
        if (this->genericTimer == 180) {
            LinkAnimation_Change(play, &this->skelAnime, &gPlayerAnim_link_okarina_warp_goal, (2.0f / 3.0f), 10.0f,
                                 Animation_GetLastFrame(&gPlayerAnim_link_okarina_warp_goal), ANIMMODE_ONCE, -8.0f);
        }
        Player_PlayAnimSfx(this, D_808551B4);
    }
}

void Player_CutsceneKnockedToGround(PlayState* play, Player* this, CsCmdActorAction* arg2) {
    if (LinkAnimation_Update(play, &this->skelAnime) && (this->genericTimer == 0) && (this->actor.bgCheckFlags & 1)) {
        Player_PlayAnimOnce(play, this, &gPlayerAnim_link_normal_back_downB);
        this->genericTimer = 1;
    }

    if (this->genericTimer != 0) {
        Player_StepLinearVelocityToZero(this);
    }
}

void Player_CutsceneSetupStartPlayOcarina(PlayState* play, Player* this, CsCmdActorAction* arg2) {
    func_80850F1C(play, this, &gPlayerAnim_link_normal_okarina_start);
    func_8084B498(this);
    Player_SetModels(this, Player_ActionToModelGroup(this, this->itemAction));
}

static PlayerAnimSfxEntry D_808551B8[] = {
    { NA_SE_IT_SWORD_PICKOUT, -0x80C },
};

void Player_CutsceneDrawAndBrandishSword(PlayState* play, Player* this, CsCmdActorAction* arg2) {
    LinkAnimation_Update(play, &this->skelAnime);

    if (LinkAnimation_OnFrame(&this->skelAnime, 6.0f)) {
        func_80846720(play, this, 0);
    } else {
        Player_PlayAnimSfx(this, D_808551B8);
    }
}

void Player_CutsceneCloseEyes(PlayState* play, Player* this, CsCmdActorAction* arg2) {
    LinkAnimation_Update(play, &this->skelAnime);
    Math_StepToS(&this->actor.shape.face, 0, 1);
}

void Player_CutsceneOpenEyes(PlayState* play, Player* this, CsCmdActorAction* arg2) {
    LinkAnimation_Update(play, &this->skelAnime);
    Math_StepToS(&this->actor.shape.face, 2, 1);
}

void Player_CutsceneSetupGetItemInWater(PlayState* play, Player* this, CsCmdActorAction* arg2) {
    Player_PlayAnimOnceWithMovementSlowed(play, this, &gPlayerAnim_link_swimer_swim_get, 0x98);
}

void Player_CutsceneSetupSleeping(PlayState* play, Player* this, CsCmdActorAction* arg2) {
    Player_PlayAnimOnceWithMovement(play, this, &gPlayerAnim_clink_op3_negaeri, 0x9C);
    Player_PlayVoiceSfxForAge(this, NA_SE_VO_LI_GROAN);
}

void Player_CutsceneSleeping(PlayState* play, Player* this, CsCmdActorAction* arg2) {
    if (LinkAnimation_Update(play, &this->skelAnime)) {
        Player_PlayAnimLoopWithMovement(play, this, &gPlayerAnim_clink_op3_wait2, 0x9C);
    }
}

void func_80851F14(PlayState* play, Player* this, LinkAnimationHeader* anim, PlayerAnimSfxEntry* arg3) {
    if (LinkAnimation_Update(play, &this->skelAnime)) {
        Player_PlayAnimLoopSlowed(play, this, anim);
        this->genericTimer = 1;
    } else if (this->genericTimer == 0) {
        Player_PlayAnimSfx(this, arg3);
    }
}

void Player_CutsceneSetupSleepingRestless(PlayState* play, Player* this, CsCmdActorAction* arg2) {
    this->actor.shape.shadowDraw = NULL;
    Player_AnimPlaybackType7(play, this, &gPlayerAnim_clink_op3_wait1);
}

static PlayerAnimSfxEntry D_808551BC[] = {
    { NA_SE_VO_LI_RELAX, 0x2023 },
    { NA_SE_PL_SLIPDOWN, 0x8EC },
    { NA_SE_PL_SLIPDOWN, -0x900 },
};

void Player_CutsceneAwaken(PlayState* play, Player* this, CsCmdActorAction* arg2) {
    if (LinkAnimation_Update(play, &this->skelAnime)) {
        Player_PlayAnimLoopWithMovement(play, this, &gPlayerAnim_clink_op3_wait3, 0x9C);
        this->genericTimer = 1;
    } else if (this->genericTimer == 0) {
        Player_PlayAnimSfx(this, D_808551BC);
        if (LinkAnimation_OnFrame(&this->skelAnime, 240.0f)) {
            this->actor.shape.shadowDraw = ActorShadow_DrawFeet;
        }
    }
}

static PlayerAnimSfxEntry D_808551C8[] = {
    { NA_SE_PL_LAND_LADDER, 0x843 },
    { 0, 0x4854 },
    { 0, 0x485A },
    { 0, -0x4860 },
};

void Player_CutsceneGetOffBed(PlayState* play, Player* this, CsCmdActorAction* arg2) {
    LinkAnimation_Update(play, &this->skelAnime);
    Player_PlayAnimSfx(this, D_808551C8);
}

void Player_CutsceneSetupBlownBackward(PlayState* play, Player* this, CsCmdActorAction* arg2) {
    Player_PlayAnimOnceWithMovementSlowed(play, this, &gPlayerAnim_clink_demo_futtobi, 0x9D);
    Player_PlayVoiceSfxForAge(this, NA_SE_VO_LI_FALL_L);
}

void func_808520BC(PlayState* play, Player* this, CsCmdActorAction* arg2) {
    f32 startX = arg2->startPos.x;
    f32 startY = arg2->startPos.y;
    f32 startZ = arg2->startPos.z;
    f32 distX = (arg2->endPos.x - startX);
    f32 distY = (arg2->endPos.y - startY);
    f32 distZ = (arg2->endPos.z - startZ);
    f32 sp4 = (f32)(play->csCtx.frames - arg2->startFrame) / (f32)(arg2->endFrame - arg2->startFrame);

    this->actor.world.pos.x = distX * sp4 + startX;
    this->actor.world.pos.y = distY * sp4 + startY;
    this->actor.world.pos.z = distZ * sp4 + startZ;
}

static PlayerAnimSfxEntry D_808551D8[] = {
    { NA_SE_PL_BOUND, 0x1014 },
    { NA_SE_PL_BOUND, -0x101E },
};

void Player_CutsceneBlownBackward(PlayState* play, Player* this, CsCmdActorAction* arg2) {
    func_808520BC(play, this, arg2);
    LinkAnimation_Update(play, &this->skelAnime);
    Player_PlayAnimSfx(this, D_808551D8);
}

void Player_CutsceneRaisedByWarp(PlayState* play, Player* this, CsCmdActorAction* arg2) {
    if (arg2 != NULL) {
        func_808520BC(play, this, arg2);
    }
    LinkAnimation_Update(play, &this->skelAnime);
}

void Player_CutsceneSetupIdle3(PlayState* play, Player* this, CsCmdActorAction* arg2) {
    Player_ChangeAnimMorphToLastFrame(play, this, GET_PLAYER_ANIM(PLAYER_ANIMGROUP_RELAX, this->modelAnimType));
    Player_StopMovement(this);
}

void Player_CutsceneIdle3(PlayState* play, Player* this, CsCmdActorAction* arg2) {
    LinkAnimation_Update(play, &this->skelAnime);
}

void Player_CutsceneSetupStop(PlayState* play, Player* this, CsCmdActorAction* arg2) {
    Player_SetupAnimMovement(play, this, 0x98);
}

void Player_CutsceneSetDraw(PlayState* play, Player* this, CsCmdActorAction* arg2) {
    this->actor.draw = Player_Draw;
}

void Player_CutsceneDrawSwordChild(PlayState* play, Player* this, CsCmdActorAction* arg2) {
    if (LinkAnimation_Update(play, &this->skelAnime)) {
        Player_PlayAnimLoopWithMovementPresetFlagsSlowed(play, this, &gPlayerAnim_clink_demo_koutai_wait);
        this->genericTimer = 1;
    } else if (this->genericTimer == 0) {
        if (LinkAnimation_OnFrame(&this->skelAnime, 10.0f)) {
            func_80846720(play, this, 1);
        }
    }
}

static PlayerAnimSfxEntry D_808551E0[] = {
    { 0, 0x300A },
    { 0, -0x3018 },
};

void Player_CutsceneTurnAroundSlowly(PlayState* play, Player* this, CsCmdActorAction* arg2) {
    func_80851F14(play, this, &gPlayerAnim_link_demo_furimuki2_wait, D_808551E0);
}

static PlayerAnimSfxEntry D_808551E8[] = {
    { 0, 0x400F },
    { 0, -0x4023 },
};

void Player_CutsceneInspectGroundCarefully(PlayState* play, Player* this, CsCmdActorAction* arg2) {
    func_80851F14(play, this, &gPlayerAnim_link_demo_nozokikomi_wait, D_808551E8);
}

void Player_CutsceneStartPassOcarina(PlayState* play, Player* this, CsCmdActorAction* arg2) {
    if (LinkAnimation_Update(play, &this->skelAnime)) {
        Player_PlayAnimLoopSlowed(play, this, &gPlayerAnim_demo_link_twait);
        this->genericTimer = 1;
    }

    if ((this->genericTimer != 0) && (play->csCtx.frames >= 900)) {
        this->rightHandType = PLAYER_MODELTYPE_LH_OPEN;
    } else {
        this->rightHandType = PLAYER_MODELTYPE_RH_FF;
    }
}

void func_80852414(PlayState* play, Player* this, LinkAnimationHeader* anim, PlayerAnimSfxEntry* arg3) {
    Player_AnimPlaybackType12(play, this, anim);
    if (this->genericTimer == 0) {
        Player_PlayAnimSfx(this, arg3);
    }
}

static PlayerAnimSfxEntry D_808551F0[] = {
    { 0, 0x300F },
    { 0, -0x3021 },
};

void Player_CutsceneStepBackCautiously(PlayState* play, Player* this, CsCmdActorAction* arg2) {
    func_80852414(play, this, &gPlayerAnim_clink_demo_koutai_wait, D_808551F0);
}

static PlayerAnimSfxEntry D_808551F8[] = {
    { NA_SE_PL_KNOCK, -0x84E },
};

void Player_CutsceneDesperateLookAtZeldasCrystal(PlayState* play, Player* this, CsCmdActorAction* arg2) {
    func_80852414(play, this, &gPlayerAnim_link_demo_kakeyori_wait, D_808551F8);
}

void Player_CutsceneSetupSpinAttackIdle(PlayState* play, Player* this, CsCmdActorAction* arg2) {
    Player_SetupSpinAttackAnims(play, this);
}

void Player_CutsceneSpinAttackIdle(PlayState* play, Player* this, CsCmdActorAction* arg2) {
    sControlInput->press.button |= BTN_B;

    Player_ChargeSpinAttack(this, play);
}

void Player_CutsceneInspectWeapon(PlayState* play, Player* this, CsCmdActorAction* arg2) {
    Player_ChargeSpinAttack(this, play);
}

void Player_SetupDoNothing4(PlayState* play, Player* this, CsCmdActorAction* arg2) {
}

void Player_DoNothing5(PlayState* play, Player* this, CsCmdActorAction* arg2) {
}

void Player_CutsceneSetupKnockedToGroundDamaged(PlayState* play, Player* this, CsCmdActorAction* arg2) {
    this->stateFlags3 |= PLAYER_STATE3_MIDAIR;
    this->linearVelocity = 2.0f;
    this->actor.velocity.y = -1.0f;

    Player_PlayAnimOnce(play, this, &gPlayerAnim_link_normal_back_downA);
    Player_PlayVoiceSfxForAge(this, NA_SE_VO_LI_FALL_L);
}

static void (*D_808551FC[])(Player* this, PlayState* play) = {
    Player_StartKnockback,
    Player_DownFromKnockback,
    Player_GetUpFromKnockback,
};

void Player_CutsceneKnockedToGroundDamaged(PlayState* play, Player* this, CsCmdActorAction* arg2) {
    D_808551FC[this->genericTimer](this, play);
}

void Player_CutsceneSetupGetSwordBack(PlayState* play, Player* this, CsCmdActorAction* arg2) {
    func_80846720(play, this, 0);
    Player_PlayAnimOnceSlowed(play, this, &gPlayerAnim_link_demo_return_to_past);
}

void Player_CutsceneSwordKnockedFromHand(PlayState* play, Player* this, CsCmdActorAction* arg2) {
    LinkAnimation_Update(play, &this->skelAnime);

    if (LinkAnimation_OnFrame(&this->skelAnime, 10.0f)) {
        this->heldItemAction = this->itemAction = PLAYER_IA_NONE;
        this->heldItemId = ITEM_NONE;
        this->modelGroup = this->nextModelGroup = Player_ActionToModelGroup(this, PLAYER_IA_NONE);
        this->leftHandDLists = D_80125E08;
        Inventory_ChangeEquipment(EQUIP_SWORD, 2);
        gSaveContext.equips.buttonItems[0] = ITEM_SWORD_MASTER;
        Inventory_DeleteEquipment(play, 0);
    }
}

static LinkAnimationHeader* D_80855208[] = {
    &gPlayerAnim_L_okarina_get,
    &gPlayerAnim_om_get,
};

static Vec3s D_80855210[2][2] = {
    { { -200, 700, 100 }, { 800, 600, 800 } },
    { { -200, 500, 0 }, { 600, 400, 600 } },
};

void Player_LearnOcarinaSong(PlayState* play, Player* this, CsCmdActorAction* arg2) {
    static Vec3f zeroVec = { 0.0f, 0.0f, 0.0f };
    static Color_RGBA8 primColor = { 255, 255, 255, 0 };
    static Color_RGBA8 envColor = { 0, 128, 128, 0 };
    s32 age = gSaveContext.linkAge;
    Vec3f sparklePos;
    Vec3f sp34;
    Vec3s* ptr;

    Player_AnimPlaybackType12(play, this, D_80855208[age]);

    if (this->rightHandType != PLAYER_MODELTYPE_RH_FF) {
        this->rightHandType = PLAYER_MODELTYPE_RH_FF;
        return;
    }

    ptr = D_80855210[gSaveContext.linkAge];

    sp34.x = ptr[0].x + Rand_CenteredFloat(ptr[1].x);
    sp34.y = ptr[0].y + Rand_CenteredFloat(ptr[1].y);
    sp34.z = ptr[0].z + Rand_CenteredFloat(ptr[1].z);

    SkinMatrix_Vec3fMtxFMultXYZ(&this->shieldMf, &sp34, &sparklePos);

    EffectSsKiraKira_SpawnDispersed(play, &sparklePos, &zeroVec, &zeroVec, &primColor, &envColor, 600, -10);
}

void Player_CutsceneGetSwordBack(PlayState* play, Player* this, CsCmdActorAction* arg2) {
    if (LinkAnimation_Update(play, &this->skelAnime)) {
        Player_CutsceneEnd(play, this, arg2);
    } else if (this->genericTimer == 0) {
        Item_Give(play, ITEM_SWORD_MASTER);
        func_80846720(play, this, 0);
    } else {
        func_8084E988(this);
    }
}

void Player_CutsceneGanonKillCombo(PlayState* play, Player* this, CsCmdActorAction* arg2) {
    if (LinkAnimation_Update(play, &this->skelAnime)) {
        func_8084285C(this, 0.0f, 99.0f, this->skelAnime.endFrame - 8.0f);
    }

    if (this->heldItemAction != PLAYER_IA_SWORD_MASTER) {
        func_80846720(play, this, 1);
    }
}

void Player_CutsceneEnd(PlayState* play, Player* this, CsCmdActorAction* arg2) {
    if (Player_IsSwimming(this)) {
        Player_SetupSwimIdle(play, this);
        Player_ResetSubCam(play, this);
    } else {
        Player_ClearLookAndAttention(this, play);
        if (!Player_SetupSpeakOrCheck(this, play)) {
            Player_SetupGetItemOrHoldBehavior(this, play);
        }
    }

    this->csMode = 0;
    this->attentionMode = 0;
}

void Player_CutsceneSetPosAndYaw(PlayState* play, Player* this, CsCmdActorAction* arg2) {
    this->actor.world.pos.x = arg2->startPos.x;
    this->actor.world.pos.y = arg2->startPos.y;
    if ((play->sceneNum == SCENE_SPOT04) && !LINK_IS_ADULT) {
        this->actor.world.pos.y -= 1.0f;
    }
    this->actor.world.pos.z = arg2->startPos.z;
    this->currentYaw = this->actor.shape.rot.y = arg2->rot.y;
}

void func_80852A54(PlayState* play, Player* this, CsCmdActorAction* arg2) {
    f32 dx = arg2->startPos.x - (s32)this->actor.world.pos.x;
    f32 dy = arg2->startPos.y - (s32)this->actor.world.pos.y;
    f32 dz = arg2->startPos.z - (s32)this->actor.world.pos.z;
    f32 dist = sqrtf(SQ(dx) + SQ(dy) + SQ(dz));
    s16 yawDiff = arg2->rot.y - this->actor.shape.rot.y;

    if ((this->linearVelocity == 0.0f) && ((dist > 50.0f) || (ABS(yawDiff) > 0x4000))) {
        Player_CutsceneSetPosAndYaw(play, this, arg2);
    }

    this->skelAnime.moveFlags = 0;
    Player_ClearRootLimbPosY(this);
}

void func_80852B4C(PlayState* play, Player* this, CsCmdActorAction* arg2, struct_80854B18* arg3) {
    if (arg3->type > 0) {
        D_80854AA4[arg3->type](play, this, arg3->ptr);
    } else if (arg3->type < 0) {
        arg3->func(play, this, arg2);
    }

    if ((sPrevSkelAnimeMoveFlags & 4) && !(this->skelAnime.moveFlags & 4)) {
        this->skelAnime.morphTable[0].y /= this->ageProperties->unk_08;
        sPrevSkelAnimeMoveFlags = 0;
    }
}

void func_80852C0C(PlayState* play, Player* this, s32 csMode) {
    if ((csMode != 1) && (csMode != 8) && (csMode != 0x31) && (csMode != 7)) {
        Player_DetatchHeldActor(play, this);
    }
}

void Player_CutsceneUnk6Update(PlayState* play, Player* this, CsCmdActorAction* arg2) {
    CsCmdActorAction* linkCsAction = play->csCtx.linkAction;
    s32 pad;
    s32 sp24;

    if (play->csCtx.state == CS_STATE_UNSKIPPABLE_INIT) {
        func_8002DF54(play, NULL, 7);
        this->csAction = 0;
        Player_StopMovement(this);
        return;
    }

    if (linkCsAction == NULL) {
        this->actor.flags &= ~ACTOR_FLAG_ACTIVE;
        return;
    }

    if (this->csAction != linkCsAction->action) {
        sp24 = D_808547C4[linkCsAction->action];
        if (sp24 >= 0) {
            if ((sp24 == 3) || (sp24 == 4)) {
                func_80852A54(play, this, linkCsAction);
            } else {
                Player_CutsceneSetPosAndYaw(play, this, linkCsAction);
            }
        }

        sPrevSkelAnimeMoveFlags = this->skelAnime.moveFlags;

        Player_EndAnimMovement(this);
        osSyncPrintf("TOOL MODE=%d\n", sp24);
        func_80852C0C(play, this, ABS(sp24));
        func_80852B4C(play, this, linkCsAction, &D_80854B18[ABS(sp24)]);

        this->genericTimer = 0;
        this->genericVar = 0;
        this->csAction = linkCsAction->action;
    }

    sp24 = D_808547C4[this->csAction];
    func_80852B4C(play, this, linkCsAction, &D_80854E50[ABS(sp24)]);
}

void Player_StartCutscene(Player* this, PlayState* play) {
    if (this->csMode != this->prevCsMode) {
        sPrevSkelAnimeMoveFlags = this->skelAnime.moveFlags;

        Player_EndAnimMovement(this);
        this->prevCsMode = this->csMode;
        osSyncPrintf("DEMO MODE=%d\n", this->csMode);
        func_80852C0C(play, this, this->csMode);
        func_80852B4C(play, this, NULL, &D_80854B18[this->csMode]);
    }

    func_80852B4C(play, this, NULL, &D_80854E50[this->csMode]);
}

s32 Player_IsDroppingFish(PlayState* play) {
    Player* this = GET_PLAYER(play);

    return (Player_DropItemFromBottle == this->actionFunc) && (this->itemAction == PLAYER_IA_BOTTLE_FISH);
}

s32 Player_StartFishing(PlayState* play) {
    Player* this = GET_PLAYER(play);

    if (gSaveContext.linkAge == 1) {
        if (!CHECK_OWNED_EQUIP(EQUIP_SWORD, 0)) {
            gSaveContext.temporaryWeapon = true;
        }
        if (this->heldItemId == ITEM_NONE) {
            this->currentSwordItemId = ITEM_SWORD_KOKIRI;
            gSaveContext.equips.buttonItems[0] = ITEM_SWORD_KOKIRI;
            Inventory_ChangeEquipment(EQUIP_SWORD, PLAYER_SWORD_KOKIRI);
        }
    }

    Player_ResetAttributesAndHeldActor(play, this);
    Player_UseItem(play, this, ITEM_FISHING_POLE);
    return 1;
}

s32 Player_SetupRestrainedByEnemy(PlayState* play, Player* this) {
    if (!Player_InBlockingCsMode(play, this) && (this->invincibilityTimer >= 0) &&
        !Player_IsShootingHookshot(this) && !(this->stateFlags3 & PLAYER_STATE3_MOVING_ALONG_HOOKSHOT_PATH)) {
        Player_ResetAttributesAndHeldActor(play, this);
        Player_SetActionFunc(play, this, func_8084F308, 0);
        Player_PlayAnimOnce(play, this, &gPlayerAnim_link_normal_re_dead_attack);
        this->stateFlags2 |= PLAYER_STATE2_RESTRAINED_BY_ENEMY;
        Player_ClearAttentionModeAndStopMoving(this);
        Player_PlayVoiceSfxForAge(this, NA_SE_VO_LI_HELD);
        return true;
    }

    return false;
}

// Sets up player cutscene
s32 Player_SetupPlayerCutscene(PlayState* play, Actor* actor, s32 csMode) {
    Player* this = GET_PLAYER(play);

    if (!Player_InBlockingCsMode(play, this)) {
        Player_ResetAttributesAndHeldActor(play, this);
        Player_SetActionFunc(play, this, Player_StartCutscene, 0);
        this->csMode = csMode;
        this->csTargetActor = actor;
        Player_ClearAttentionModeAndStopMoving(this);
        return 1;
    }

    return 0;
}

void Player_SetupStandingStillMorph(Player* this, PlayState* play) {
    Player_SetActionFunc(play, this, Player_StandingStill, 1);
    Player_ChangeAnimMorphToLastFrame(play, this, Player_GetStandingStillAnim(this));
    this->currentYaw = this->actor.shape.rot.y;
}

s32 Player_InflictDamage(PlayState* play, s32 damage) {
    return Player_InflictDamageModified(play, damage, true);
}

s32 Player_InflictDamageModified(PlayState* play, s32 damage, u8 modified) {
    Player* this = GET_PLAYER(play);

    if (!Player_InBlockingCsMode(play, this) && !Player_Damage_modified(play, this, damage, modified)) {
        this->stateFlags2 &= ~PLAYER_STATE2_RESTRAINED_BY_ENEMY;
        return 1;
    }

    return 0;
}

// Start talking with the given actor
void Player_StartTalkingWithActor(PlayState* play, Actor* actor) {
    Player* this = GET_PLAYER(play);
    s32 pad;

    if ((this->talkActor != NULL) || (actor == this->naviActor) ||
        CHECK_FLAG_ALL(actor->flags, ACTOR_FLAG_TARGETABLE | ACTOR_FLAG_NAVI_HAS_INFO)) {
        actor->flags |= ACTOR_FLAG_PLAYER_TALKED_TO;
    }

    this->talkActor = actor;
    this->exchangeItemId = EXCH_ITEM_NONE;

    if (actor->textId == 0xFFFF) {
        func_8002DF54(play, actor, 1);
        actor->flags |= ACTOR_FLAG_PLAYER_TALKED_TO;
        Player_UnequipItem(play, this);
    } else {
        if (this->actor.flags & ACTOR_FLAG_PLAYER_TALKED_TO) {
            this->actor.textId = 0;
        } else {
            this->actor.flags |= ACTOR_FLAG_PLAYER_TALKED_TO;
            this->actor.textId = actor->textId;
        }

        if (this->stateFlags1 & PLAYER_STATE1_RIDING_HORSE) {
            s32 sp24 = this->genericTimer;

            Player_UnequipItem(play, this);
            Player_SetupTalkWithActor(play, this);

            this->genericTimer = sp24;
        } else {
            if (Player_IsSwimming(this)) {
                Player_SetupMiniCsFunc(play, this, Player_SetupTalkWithActor);
                Player_ChangeAnimLongMorphLoop(play, this, &gPlayerAnim_link_swimer_swim_wait);
            } else if ((actor->category != ACTORCAT_NPC) || (this->heldItemAction == PLAYER_IA_FISHING_POLE)) {
                Player_SetupTalkWithActor(play, this);

                if (!Player_IsUnfriendlyZTargeting(this)) {
                    if ((actor != this->naviActor) && (actor->xzDistToPlayer < 40.0f)) {
                        Player_PlayAnimOnceSlowed(play, this, &gPlayerAnim_link_normal_backspace);
                    } else {
                        Player_PlayAnimLoop(play, this, Player_GetStandingStillAnim(this));
                    }
                }
            } else {
                Player_SetupMiniCsFunc(play, this, Player_SetupTalkWithActor);
                Player_PlayAnimOnceSlowed(play, this,
                                          (actor->xzDistToPlayer < 40.0f) ? &gPlayerAnim_link_normal_backspace
                                                                          : &gPlayerAnim_link_normal_talk_free);
            }

            if (this->skelAnime.animation == &gPlayerAnim_link_normal_backspace) {
                Player_SetupAnimMovement(play, this, 0x19);
            }

            Player_ClearAttentionModeAndStopMoving(this);
        }

        this->stateFlags1 |= PLAYER_STATE1_TALKING | PLAYER_STATE1_IN_CUTSCENE;
    }

    if ((this->naviActor == this->talkActor) && ((this->talkActor->textId & 0xFF00) != 0x200)) {
        this->naviActor->flags |= ACTOR_FLAG_PLAYER_TALKED_TO;
        Player_SetCameraTurnAround(play, 0xB);
    }
}
