#include <libultraship/libultra.h>
#include "global.h"
#include "vt.h"

typedef struct {
    /* 0x00 */ u16 sfxId;
    /* 0x04 */ Vec3f* pos;
    /* 0x08 */ u8 token;
    /* 0x0C */ f32* freqScale;
    /* 0x10 */ f32* vol;
    /* 0x14 */ s8* reverbAdd;
} SoundRequest; // size = 0x18

typedef struct {
    /* 0x00 */ f32 value;
    /* 0x04 */ f32 target;
    /* 0x08 */ f32 step;
    /* 0x0C */ u16 remainingFrames;
} UnusedBankLerp; // size = 0x10

// rodata for Audio_ProcessSoundRequest (this file)
// (probably moved to .data due to -use_readwrite_const)
char D_80133340[] = "SE";

// rodata for Audio_ChooseActiveSounds (this file)
char D_80133344[] = VT_COL(RED, WHITE) "<INAGAKI CHECK> dist over! flag:%04X ptr:%08X pos:%f-%f-%f" VT_RST "\n";

// file padding
s32 D_8013338C = 0;

// rodata for Audio_ProcessSeqCmd (code_800F9280.c)
char D_80133390[] = "SEQ H";
char D_80133398[] = "    L";

SoundBankEntry D_8016BAD0[9];
SoundBankEntry D_8016BC80[12];
SoundBankEntry D_8016BEC0[22];
SoundBankEntry D_8016C2E0[20];
SoundBankEntry D_8016C6A0[8];
SoundBankEntry D_8016C820[3];
SoundBankEntry D_8016C8B0[5];
SoundRequest sSoundRequests[0x100];
u8 sSoundBankListEnd[7];
u8 sSoundBankFreeListStart[7];
u8 sSoundBankUnused[7];
ActiveSound gActiveSounds[7][3];
u8 sCurSfxPlayerChannelIdx;
u8 gSoundBankMuted[7];
UnusedBankLerp sUnusedBankLerp[7];
u16 gAudioSfxSwapSource[10];
u16 gAudioSfxSwapTarget[10];
u8 gAudioSfxSwapMode[10];

// sSoundRequests ring buffer endpoints. read index <= write index, wrapping around mod 256.
u8 sSoundRequestWriteIndex = 0;
u8 sSoundRequestReadIndex = 0;

/**
 * Array of pointers to arrays of SoundBankEntry of sizes: 9, 12, 22, 20, 8, 3, 5
 *
 * 0 : Player Bank          size 9
 * 1 : Item Bank            size 12
 * 2 : Environment Bank     size 22
 * 3 : Enemy Bank           size 20
 * 4 : System Bank          size 8
 * 5 : Ocarina Bank         size 3
 * 6 : Voice Bank           size 5
 */
SoundBankEntry* gSoundBanks[7] = {
    D_8016BAD0, D_8016BC80, D_8016BEC0, D_8016C2E0, D_8016C6A0, D_8016C820, D_8016C8B0,
};

u8 sBankSizes[ARRAY_COUNT(gSoundBanks)] = {
    ARRAY_COUNT(D_8016BAD0), ARRAY_COUNT(D_8016BC80), ARRAY_COUNT(D_8016BEC0), ARRAY_COUNT(D_8016C2E0),
    ARRAY_COUNT(D_8016C6A0), ARRAY_COUNT(D_8016C820), ARRAY_COUNT(D_8016C8B0),
};

u8 gSfxChannelLayout = 0;

u16 D_801333D0 = 0;

Vec3f D_801333D4 = { 0.0f, 0.0f, 0.0f }; // default pos

f32 D_801333E0 = 1.0f; // default freqScale

s32 D_801333E4 = 0; // unused

s8 D_801333E8 = 0; // default reverbAdd

s32 D_801333EC = 0; // unused

u8 D_801333F0 = 0;

u8 gAudioSfxSwapOff = 0;

u8 D_801333F8 = 0;

void Audio_SetSoundBanksMute(u16 muteMask) {
    u8 bankId;

    for (bankId = 0; bankId < ARRAY_COUNT(gSoundBanks); bankId++) {
        if (muteMask & 1) {
            gSoundBankMuted[bankId] = true;
        } else {
            gSoundBankMuted[bankId] = false;
        }
        muteMask = muteMask >> 1;
    }
}

void Audio_QueueSeqCmdMute(u8 channelIdx) {
    D_801333D0 |= (1 << channelIdx);
    Audio_SetVolScale(SEQ_PLAYER_BGM_MAIN, 2, 0x40, 0xF);
    Audio_SetVolScale(SEQ_PLAYER_BGM_SUB, 2, 0x40, 0xF);
}

void Audio_ClearBGMMute(u8 channelIdx) {
    D_801333D0 &= ((1 << channelIdx) ^ 0xFFFF);
    if (D_801333D0 == 0) {
        Audio_SetVolScale(SEQ_PLAYER_BGM_MAIN, 2, 0x7F, 0xF);
        Audio_SetVolScale(SEQ_PLAYER_BGM_SUB, 2, 0x7F, 0xF);
    }
}

static u16 randSfxTable[] = {
    NA_SE_PL_WALK_GROUND,
    NA_SE_PL_WALK_GROUND,
    NA_SE_PL_WALK_GROUND,
    NA_SE_PL_WALK_SAND,
    NA_SE_PL_WALK_CONCRETE,
    NA_SE_PL_WALK_DIRT,
    NA_SE_PL_WALK_WATER0,
    NA_SE_PL_WALK_WATER1,
    NA_SE_PL_WALK_WATER2,
    NA_SE_PL_WALK_MAGMA,
    NA_SE_PL_WALK_GRASS,
    NA_SE_PL_WALK_GLASS,
    NA_SE_PL_WALK_LADDER,
    NA_SE_PL_WALK_GLASS,
    NA_SE_PL_WALK_WALL,
    NA_SE_PL_WALK_HEAVYBOOTS,
    NA_SE_PL_WALK_ICE,
    NA_SE_PL_JUMP,
    NA_SE_PL_JUMP,
    NA_SE_PL_JUMP_SAND,
    NA_SE_PL_JUMP_CONCRETE,
    NA_SE_PL_JUMP_DIRT,
    NA_SE_PL_JUMP_WATER0,
    NA_SE_PL_JUMP_WATER1,
    NA_SE_PL_JUMP_WATER2,
    NA_SE_PL_JUMP_MAGMA,
    NA_SE_PL_JUMP_GRASS,
    NA_SE_PL_JUMP_GLASS,
    NA_SE_PL_JUMP_LADDER,
    NA_SE_PL_JUMP_GLASS,
    NA_SE_PL_JUMP_HEAVYBOOTS,
    NA_SE_PL_JUMP_ICE,
    NA_SE_PL_LAND,
    NA_SE_PL_LAND,
    NA_SE_PL_LAND_SAND,
    NA_SE_PL_LAND_CONCRETE,
    NA_SE_PL_LAND_DIRT,
    NA_SE_PL_LAND_WATER0,
    NA_SE_PL_LAND_WATER1,
    NA_SE_PL_LAND_WATER2,
    NA_SE_PL_LAND_MAGMA,
    NA_SE_PL_LAND_GRASS,
    NA_SE_PL_LAND_GLASS,
    NA_SE_PL_LAND_LADDER,
    NA_SE_PL_LAND_GLASS,
    NA_SE_PL_LAND_HEAVYBOOTS,
    NA_SE_PL_LAND_ICE,
    NA_SE_PL_SLIPDOWN,
    NA_SE_PL_CLIMB_CLIFF,
    NA_SE_PL_CLIMB_CLIFF,
    NA_SE_PL_SIT_ON_HORSE,
    NA_SE_PL_GET_OFF_HORSE,
    NA_SE_PL_TAKE_OUT_SHIELD,
    NA_SE_PL_TAKE_OUT_SHIELD,
    NA_SE_PL_TAKE_OUT_SHIELD,
    NA_SE_PL_TAKE_OUT_SHIELD,
    NA_SE_PL_TAKE_OUT_SHIELD,
    NA_SE_PL_TAKE_OUT_SHIELD,
    NA_SE_PL_CHANGE_ARMS,
    NA_SE_PL_CHANGE_ARMS,
    NA_SE_PL_CHANGE_ARMS,
    NA_SE_PL_CHANGE_ARMS,
    NA_SE_PL_CHANGE_ARMS,
    NA_SE_PL_CHANGE_ARMS,
    NA_SE_PL_CATCH_BOOMERANG,
    NA_SE_EV_DIVE_INTO_WATER,
    NA_SE_EV_JUMP_OUT_WATER,
    NA_SE_PL_SWIM,
    NA_SE_PL_THROW,
    NA_SE_PL_BODY_BOUND,
    NA_SE_PL_ROLL,
    NA_SE_PL_SKIP,
    NA_SE_PL_BODY_HIT,
    NA_SE_PL_DAMAGE,
    NA_SE_PL_SLIP,
    NA_SE_PL_SLIP,
    NA_SE_PL_SLIP,
    NA_SE_PL_SLIP_SAND,
    NA_SE_PL_SLIP_CONCRETE,
    NA_SE_PL_SLIP_DIRT,
    NA_SE_PL_SLIP_WATER0,
    NA_SE_PL_SLIP_WATER1,
    NA_SE_PL_SLIP_WATER2,
    NA_SE_PL_SLIP_MAGMA,
    NA_SE_PL_SLIP_GRASS,
    NA_SE_PL_SLIP_GLASS,
    NA_SE_PL_SLIP_LADDER,
    NA_SE_PL_SLIP_GLASS,
    NA_SE_PL_SLIP_HEAVYBOOTS,
    NA_SE_PL_SLIP_ICE,
    NA_SE_PL_BOUND,
    NA_SE_PL_BOUND,
    NA_SE_PL_BOUND_SAND,
    NA_SE_PL_BOUND_CONCRETE,
    NA_SE_PL_BOUND_DIRT,
    NA_SE_PL_BOUND_WATER0,
    NA_SE_PL_BOUND_WATER1,
    NA_SE_PL_BOUND_WATER2,
    NA_SE_PL_BOUND_MAGMA,
    NA_SE_PL_BOUND_GRASS,
    NA_SE_PL_BOUND_WOOD,
    NA_SE_PL_BOUND_LADDER,
    NA_SE_PL_BOUND_WOOD,
    NA_SE_PL_BOUND_HEAVYBOOTS,
    NA_SE_PL_BOUND_ICE,
    NA_SE_PL_FACE_UP,
    NA_SE_PL_DIVE_BUBBLE,
    NA_SE_PL_MOVE_BUBBLE,
    NA_SE_PL_METALEFFECT_KID,
    NA_SE_PL_METALEFFECT_ADULT,
    NA_SE_PL_SPARK - SFX_FLAG,
    NA_SE_IT_SWORD_IMPACT,
    NA_SE_IT_SWORD_SWING,
    NA_SE_IT_SWORD_PUTAWAY,
    NA_SE_IT_SWORD_PICKOUT,
    NA_SE_IT_ARROW_SHOT,
    NA_SE_IT_BOOMERANG_THROW,
    NA_SE_IT_SHIELD_BOUND,
    NA_SE_IT_SHIELD_BOUND,
    NA_SE_IT_BOW_DRAW,
    NA_SE_IT_SHIELD_REFLECT_SW,
    NA_SE_IT_ARROW_STICK_HRAD,
    NA_SE_IT_HAMMER_HIT,
    NA_SE_IT_HOOKSHOT_CHAIN - SFX_FLAG,
    NA_SE_IT_SHIELD_REFLECT_MG,
    NA_SE_IT_BOMB_IGNIT - SFX_FLAG,
    NA_SE_IT_BOMB_EXPLOSION,
    NA_SE_IT_BOMB_UNEXPLOSION,
    NA_SE_IT_BOOMERANG_FLY - SFX_FLAG,
    NA_SE_IT_SWORD_STRIKE,
    NA_SE_IT_HAMMER_SWING,
    NA_SE_IT_HOOKSHOT_REFLECT,
    NA_SE_IT_ARROW_STICK_CRE,
    NA_SE_IT_ARROW_STICK_CRE,
    NA_SE_IT_ARROW_STICK_OBJ,
    NA_SE_PL_WALK_GROUND - SFX_FLAG,
    NA_SE_PL_WALK_GROUND - SFX_FLAG,
    NA_SE_IT_SWORD_SWING_HARD,
    NA_SE_IT_WALL_HIT_HARD,
    NA_SE_IT_WALL_HIT_SOFT,
    NA_SE_IT_WALL_HIT_SOFT,
    NA_SE_IT_STONE_HIT,
    NA_SE_IT_WOODSTICK_BROKEN,
    NA_SE_IT_LASH,
    NA_SE_IT_SHIELD_POSTURE,
    NA_SE_IT_SLING_SHOT,
    NA_SE_IT_SLING_DRAW,
    NA_SE_IT_SWORD_CHARGE - SFX_FLAG,
    NA_SE_IT_ROLLING_CUT,
    NA_SE_IT_SWORD_STRIKE_HARD,
    NA_SE_IT_SLING_REFLECT,
    NA_SE_IT_SHIELD_REMOVE,
    NA_SE_IT_HOOKSHOT_READY,
    NA_SE_IT_HOOKSHOT_RECEIVE,
    NA_SE_IT_HOOKSHOT_STICK_OBJ,
    NA_SE_IT_SWORD_REFLECT_MG,
    NA_SE_IT_DEKU,
    NA_SE_IT_BOW_FLICK,
    NA_SE_IT_BOW_FLICK,
    NA_SE_IT_BOW_FLICK,
    NA_SE_IT_BOMBCHU_MOVE,
    NA_SE_IT_SHIELD_CHARGE_LV1,
    NA_SE_IT_SHIELD_CHARGE_LV2,
    NA_SE_IT_SHIELD_CHARGE_LV3,
    NA_SE_IT_SLING_FLICK,
    NA_SE_IT_SWORD_STICK_STN,
    NA_SE_IT_REFLECTION_WOOD,
    NA_SE_IT_SHIELD_REFLECT_MG2,
    NA_SE_IT_MAGIC_ARROW_SHOT,
    NA_SE_IT_EXPLOSION_FRAME,
    NA_SE_IT_EXPLOSION_ICE,
    NA_SE_IT_YOBI19 - SFX_FLAG,
    NA_SE_FISHING_REEL_SLOW2 - SFX_FLAG,
    NA_SE_OC_DOOR_OPEN,
    NA_SE_EV_DOOR_CLOSE,
    NA_SE_EV_EXPLOSION,
    NA_SE_EV_HORSE_WALK,
    NA_SE_EV_HORSE_RUN,
    NA_SE_EV_HORSE_NEIGH,
    NA_SE_EV_RIVER_STREAM - SFX_FLAG,
    NA_SE_EV_WATER_WALL_BIG - SFX_FLAG,
    NA_SE_EV_DIVE_WATER,
    NA_SE_EV_OUT_OF_WATER,
    NA_SE_EV_ROCK_SLIDE - SFX_FLAG,
    NA_SE_EV_MAGMA_LEVEL - SFX_FLAG,
    NA_SE_EV_MAGMA_LEVEL - SFX_FLAG,
    NA_SE_EV_BRIDGE_OPEN - SFX_FLAG,
    NA_SE_EV_BRIDGE_CLOSE - SFX_FLAG,
    NA_SE_EV_BRIDGE_OPEN_STOP,
    NA_SE_EV_BRIDGE_CLOSE_STOP,
    NA_SE_EV_WALL_BROKEN,
    NA_SE_EV_CHICKEN_CRY_N,
    NA_SE_EV_CHICKEN_CRY_A,
    NA_SE_EV_CHICKEN_CRY_M,
    NA_SE_EV_SLIDE_DOOR_OPEN,
    NA_SE_EV_FOOT_SWITCH,
    NA_SE_EV_HORSE_GROAN,
    NA_SE_EV_BOMB_DROP_WATER,
    NA_SE_EV_BOMB_DROP_WATER,
    NA_SE_EV_HORSE_JUMP,
    NA_SE_EV_HORSE_LAND,
    NA_SE_EV_HORSE_SLIP,
    NA_SE_EV_FAIRY_DASH,
    NA_SE_EV_SLIDE_DOOR_CLOSE,
    NA_SE_EV_STONE_BOUND,
    NA_SE_EV_STONE_STATUE_OPEN - SFX_FLAG,
    NA_SE_EV_TBOX_UNLOCK,
    NA_SE_EV_TBOX_OPEN,
    NA_SE_SY_TIMER - SFX_FLAG,
    NA_SE_EV_FLAME_IGNITION,
    NA_SE_EV_SPEAR_HIT,
    NA_SE_EV_ELEVATOR_MOVE - SFX_FLAG,
    NA_SE_EV_WARP_HOLE - SFX_FLAG,
    NA_SE_EV_LINK_WARP,
    NA_SE_EV_PILLAR_SINK - SFX_FLAG,
    NA_SE_EV_WATER_WALL - SFX_FLAG,
    NA_SE_EV_RIVER_STREAM_S - SFX_FLAG,
    NA_SE_EV_RIVER_STREAM_F - SFX_FLAG,
    NA_SE_EV_HORSE_LAND2,
    NA_SE_EV_HORSE_SANDDUST,
    NA_SE_EV_BOMB_BOUND,
    NA_SE_EV_BOMB_BOUND,
    NA_SE_EV_WATERDROP - SFX_FLAG,
    NA_SE_EV_TORCH - SFX_FLAG,
    NA_SE_EV_MAGMA_LEVEL_M - SFX_FLAG,
    NA_SE_EV_FIRE_PILLAR - SFX_FLAG,
    NA_SE_EV_FIRE_PLATE - SFX_FLAG,
    NA_SE_EV_BLOCK_BOUND,
    NA_SE_EV_METALDOOR_SLIDE - SFX_FLAG,
    NA_SE_EV_METALDOOR_STOP,
    NA_SE_EV_BLOCK_SHAKE,
    NA_SE_EV_BOX_BREAK,
    NA_SE_EV_HAMMER_SWITCH,
    NA_SE_EV_MAGMA_LEVEL_L - SFX_FLAG,
    NA_SE_EV_SPEAR_FENCE,
    NA_SE_EV_GANON_HORSE_NEIGH,
    NA_SE_EV_GANON_HORSE_GROAN,
    NA_SE_EV_FANTOM_WARP_S,
    NA_SE_EV_FANTOM_WARP_L - SFX_FLAG,
    NA_SE_EV_FOUNTAIN - SFX_FLAG,
    NA_SE_EV_KID_HORSE_WALK,
    NA_SE_EV_KID_HORSE_RUN,
    NA_SE_EV_KID_HORSE_NEIGH,
    NA_SE_EV_KID_HORSE_GROAN,
    NA_SE_EV_WHITE_OUT,
    NA_SE_EV_LIGHT_GATHER - SFX_FLAG,
    NA_SE_EV_TREE_CUT,
    NA_SE_EV_WATERDROP,
    NA_SE_EV_TORCH,
    NA_SE_PL_WALK_GROUND - SFX_FLAG,
    NA_SE_PL_WALK_GROUND - SFX_FLAG,
    NA_SE_EN_DODO_J_WALK,
    NA_SE_EN_DODO_J_CRY,
    NA_SE_EN_DODO_J_FIRE - SFX_FLAG,
    NA_SE_EN_DODO_J_DAMAGE,
    NA_SE_EN_DODO_J_DEAD,
    NA_SE_EN_DODO_K_CRY,
    NA_SE_EN_DODO_K_DAMAGE,
    NA_SE_EN_DODO_K_DEAD,
    NA_SE_EN_DODO_K_WALK,
    NA_SE_EN_DODO_K_FIRE - SFX_FLAG,
    NA_SE_EN_GOMA_WALK,
    NA_SE_EN_GOMA_HIGH,
    NA_SE_EN_GOMA_CLIM,
    NA_SE_EN_GOMA_DOWN,
    NA_SE_EN_GOMA_CRY1,
    NA_SE_EN_GOMA_CRY2,
    NA_SE_EN_GOMA_DAM1,
    NA_SE_EN_GOMA_DAM2,
    NA_SE_EN_GOMA_DEAD,
    NA_SE_EN_GOMA_UNARI,
    NA_SE_EN_GOMA_EGG1,
    NA_SE_EN_GOMA_EGG2,
    NA_SE_EN_GOMA_JR_WALK,
    NA_SE_EN_GOMA_JR_CRY,
    NA_SE_EN_GOMA_JR_DAM1,
    NA_SE_EN_GOMA_JR_DAM2,
    NA_SE_EN_GOMA_JR_DEAD,
    NA_SE_EN_GOMA_DEMO_EYE,
    NA_SE_EN_GOMA_LAST - SFX_FLAG,
    NA_SE_EN_GOMA_UNARI2,
    NA_SE_EN_DODO_M_CRY,
    NA_SE_EN_DODO_M_DEAD,
    NA_SE_EN_DODO_M_MOVE,
    NA_SE_EN_DODO_M_DOWN,
    NA_SE_EN_DODO_M_UP,
    NA_SE_EN_DODO_M_GND,
    NA_SE_EN_RIZA_CRY,
    NA_SE_EN_RIZA_ATTACK,
    NA_SE_EN_RIZA_DAMAGE,
    NA_SE_EN_RIZA_WARAU,
    NA_SE_EN_RIZA_DEAD,
    NA_SE_EN_RIZA_WALK,
    NA_SE_EN_RIZA_JUMP,
    NA_SE_EN_RIZA_ONGND,
    NA_SE_EN_RIZA_DOWN,
    NA_SE_EN_STAL_WARAU,
    NA_SE_EN_STAL_SAKEBI,
    NA_SE_EN_STAL_DAMAGE,
    NA_SE_EN_STAL_DEAD,
    NA_SE_EN_STAL_JUMP,
    NA_SE_EN_STAL_WALK,
    NA_SE_EN_RIZA_DOWN,
    NA_SE_EN_FFLY_ATTACK,
    NA_SE_EN_FFLY_FLY,
    NA_SE_EN_FFLY_DEAD,
    NA_SE_EN_AMOS_WALK,
    NA_SE_EN_AMOS_WAVE,
    NA_SE_EN_AMOS_DEAD,
    NA_SE_EN_AMOS_DAMAGE,
    NA_SE_EN_AMOS_VOICE,
    NA_SE_EN_DODO_K_COLI,
    NA_SE_EN_DODO_K_COLI2,
    NA_SE_EN_DODO_K_ROLL - SFX_FLAG,
    NA_SE_EN_DODO_K_BREATH - SFX_FLAG,
    NA_SE_EN_DODO_K_DRINK,
    NA_SE_EN_DODO_K_DOWN - SFX_FLAG,
    NA_SE_EN_DODO_K_OTAKEBI,
    NA_SE_EN_DODO_K_END,
    NA_SE_EN_DODO_K_LAST - SFX_FLAG,
    NA_SE_EN_DODO_K_LAVA,
    NA_SE_EN_DODO_J_BREATH - SFX_FLAG,
    NA_SE_EN_DODO_J_TAIL,
    NA_SE_EN_RIZA_DOWN,
    NA_SE_EN_DEKU_MOUTH,
    NA_SE_EN_DEKU_ATTACK,
    NA_SE_EN_DEKU_DAMAGE,
    NA_SE_EN_DEKU_DEAD,
    NA_SE_EN_DEKU_JR_MOUTH,
    NA_SE_EN_DEKU_JR_ATTACK,
    NA_SE_EN_DEKU_JR_DEAD,
    NA_SE_EN_DODO_M_GND,
    NA_SE_EN_TAIL_FLY - SFX_FLAG,
    NA_SE_EN_TAIL_CRY,
    NA_SE_EN_STALTU_DOWN,
    NA_SE_EN_STALTU_UP,
    NA_SE_EN_STALTU_LAUGH,
    NA_SE_EN_STALTU_DAMAGE,
    NA_SE_EN_STAL_JUMP,
    NA_SE_EN_DODO_M_GND,
    NA_SE_EN_TEKU_DEAD,
    NA_SE_EN_TEKU_WALK,
    NA_SE_EN_PO_KANTERA,
    NA_SE_EN_PO_FLY - SFX_FLAG,
    NA_SE_EN_PO_AWAY - SFX_FLAG,
    NA_SE_EN_PO_APPEAR,
    NA_SE_EN_PO_DISAPPEAR,
    NA_SE_EN_PO_DAMAGE,
    NA_SE_EN_PO_DEAD,
    NA_SE_EN_PO_DEAD2,
    NA_SE_EN_EXTINCT,
    NA_SE_EN_NUTS_UP,
    NA_SE_EN_NUTS_DOWN,
    NA_SE_EN_NUTS_THROW,
    NA_SE_EN_NUTS_WALK,
    NA_SE_EN_NUTS_DAMAGE,
    NA_SE_EN_NUTS_DEAD,
    NA_SE_EN_STALTU_ROLL,
    NA_SE_EN_STALWALL_DEAD,
    NA_SE_EN_TEKU_DAMAGE,
    NA_SE_EN_FALL_AIM,
    NA_SE_EN_FALL_UP,
    NA_SE_EN_FALL_CATCH,
    NA_SE_EN_FALL_LAND,
    NA_SE_EN_FALL_WALK,
    NA_SE_EN_FALL_DAMAGE,
    NA_SE_EN_BIRI_FLY,
    NA_SE_EN_BIRI_JUMP,
    NA_SE_EN_BIRI_SPARK - SFX_FLAG,
    NA_SE_EN_FANTOM_TRANSFORM,
    NA_SE_EN_FANTOM_TRANSFORM,
    NA_SE_EN_FANTOM_THUNDER,
    NA_SE_EN_FANTOM_SPARK,
    NA_SE_EN_FANTOM_FLOAT - SFX_FLAG,
    NA_SE_EN_FANTOM_MASIC1,
    NA_SE_EN_FANTOM_MASIC2,
    NA_SE_EN_FANTOM_FIRE - SFX_FLAG,
    NA_SE_EN_FANTOM_HIT_THUNDER,
    NA_SE_EN_FANTOM_ATTACK,
    NA_SE_EN_FANTOM_STICK,
    NA_SE_EN_FANTOM_EYE,
    NA_SE_EN_FANTOM_LAST,
    NA_SE_EN_FANTOM_THUNDER_GND,
    NA_SE_EN_FANTOM_DAMAGE,
    NA_SE_EN_FANTOM_DEAD,
    NA_SE_EN_FANTOM_LAUGH,
    NA_SE_EN_FANTOM_DAMAGE2,
    NA_SE_EN_FANTOM_VOICE,
    NA_SE_EN_MORIBLIN_WALK,
    NA_SE_EN_MORIBLIN_SLIDE,
    NA_SE_EN_MORIBLIN_ATTACK,
    NA_SE_EN_MORIBLIN_VOICE,
    NA_SE_EN_MORIBLIN_SPEAR_AT,
    NA_SE_EN_MORIBLIN_SPEAR_NORM,
    NA_SE_EN_MORIBLIN_DEAD,
    NA_SE_EN_NUTS_THROW,
    NA_SE_EN_OCTAROCK_FLOAT,
    NA_SE_EN_OCTAROCK_JUMP,
    NA_SE_EN_OCTAROCK_LAND,
    NA_SE_EN_OCTAROCK_SINK,
    NA_SE_EN_OCTAROCK_BUBLE,
    NA_SE_PL_WALK_GROUND - SFX_FLAG,
    NA_SE_PL_WALK_GROUND - SFX_FLAG,
    NA_SE_PL_WALK_GROUND - SFX_FLAG,
    NA_SE_PL_WALK_GROUND - SFX_FLAG,
    NA_SE_PL_WALK_GROUND - SFX_FLAG,
    NA_SE_PL_WALK_GROUND - SFX_FLAG,
    NA_SE_PL_WALK_GROUND - SFX_FLAG,
    NA_SE_SY_WIN_OPEN,
    NA_SE_SY_WIN_CLOSE,
    NA_SE_SY_CORRECT_CHIME,
    NA_SE_SY_GET_RUPY,
    NA_SE_SY_MESSAGE_WOMAN,
    NA_SE_SY_MESSAGE_MAN,
    NA_SE_SY_ERROR,
    NA_SE_SY_TRE_BOX_APPEAR,
    NA_SE_SY_TRE_BOX_APPEAR,
    NA_SE_SY_DECIDE,
    NA_SE_SY_CURSOR,
    NA_SE_SY_CANCEL,
    NA_SE_SY_HP_RECOVER,
    NA_SE_SY_ATTENTION_ON,
    NA_SE_SY_ATTENTION_ON,
    NA_SE_PL_WALK_GROUND - SFX_FLAG,
    NA_SE_SY_LOCK_ON,
    NA_SE_SY_LOCK_ON,
    NA_SE_SY_LOCK_OFF,
    NA_SE_SY_LOCK_ON_HUMAN,
    NA_SE_SY_CAMERA_ZOOM_UP,
    NA_SE_SY_CAMERA_ZOOM_DOWN,
    NA_SE_SY_ATTENTION_ON_OLD,
    NA_SE_SY_ATTENTION_URGENCY,
    NA_SE_SY_MESSAGE_PASS,
    NA_SE_PL_WALK_GROUND - SFX_FLAG,
    NA_SE_PL_WALK_GROUND - SFX_FLAG,
    NA_SE_PL_WALK_GROUND - SFX_FLAG,
    NA_SE_SY_PIECE_OF_HEART,
    NA_SE_SY_GET_ITEM,
    NA_SE_SY_WIN_SCROLL_LEFT,
    NA_SE_SY_WIN_SCROLL_RIGHT,
    NA_SE_SY_OCARINA_ERROR,
    NA_SE_SY_CAMERA_ZOOM_UP_2,
    NA_SE_SY_CAMERA_ZOOM_DOWN_2,
    NA_SE_SY_GLASSMODE_ON,
    NA_SE_SY_GLASSMODE_OFF,
    NA_SE_SY_ATTENTION_ON,
    NA_SE_SY_ATTENTION_URGENCY,
    NA_SE_OC_OCARINA,
    NA_SE_PL_WALK_GROUND - SFX_FLAG,
    NA_SE_PL_LAND - SFX_FLAG,
    NA_SE_VO_LI_SWORD_N,
    NA_SE_VO_LI_SWORD_N,
    NA_SE_VO_LI_SWORD_N,
    NA_SE_VO_LI_SWORD_N,
    NA_SE_VO_LI_SWORD_N,
    NA_SE_VO_LI_SWORD_N,
    NA_SE_VO_LI_SWORD_N,
    NA_SE_VO_LI_SWORD_L,
    NA_SE_VO_LI_SWORD_L,
    NA_SE_VO_LI_MAGIC_ATTACK,
    NA_SE_VO_LI_LASH,
    NA_SE_VO_LI_HANG,
    NA_SE_VO_LI_AUTO_JUMP,
    NA_SE_VO_LI_CLIMB_END,
    NA_SE_VO_LI_CLIMB_END,
    NA_SE_VO_LI_CLIMB_END,
    NA_SE_VO_LI_CLIMB_END,
    NA_SE_VO_LI_DAMAGE_S,
    NA_SE_VO_LI_DAMAGE_S,
    NA_SE_VO_LI_FALL_L,
    NA_SE_VO_LI_FALL_S,
    NA_SE_VO_LI_FALL_L,
    NA_SE_VO_LI_FALL_L,
    NA_SE_VO_LI_BREATH_REST,
    NA_SE_VO_LI_BREATH_REST,
    NA_SE_VO_LI_DOWN,
    NA_SE_VO_LI_TAKEN_AWAY,
    NA_SE_VO_LI_HELD,
    NA_SE_VO_NAVY_HELLO,
    NA_SE_VO_NAVY_HEAR,
    NA_SE_VO_NAVY_ENEMY,
    NA_SE_VO_NAVY_HELLO,
    NA_SE_VO_NAVY_HEAR,
    NA_SE_VO_NAVY_ENEMY,
    NA_SE_VO_TA_SLEEP,
    NA_SE_EN_VALVAISA_APPEAR - SFX_FLAG,
    NA_SE_EN_VALVAISA_ROAR,
    NA_SE_EN_VALVAISA_MAHI1,
    NA_SE_EN_VALVAISA_MAHI2,
    NA_SE_EN_VALVAISA_KNOCKOUT,
    NA_SE_EN_VALVAISA_DAMAGE1,
    NA_SE_EN_VALVAISA_DAMAGE2,
    NA_SE_EN_VALVAISA_ROCK,
    NA_SE_EN_VALVAISA_LAND,
    NA_SE_EN_VALVAISA_DEAD,
    NA_SE_EN_VALVAISA_BURN - SFX_FLAG,
    NA_SE_EN_VALVAISA_FIRE - SFX_FLAG,
    NA_SE_EN_VALVAISA_LAND2,
    NA_SE_EN_MONBLIN_HAM_LAND,
    NA_SE_EN_MONBLIN_HAM_DOWN,
    NA_SE_EN_MONBLIN_HAM_UP,
    NA_SE_EN_REDEAD_CRY,
    NA_SE_EN_REDEAD_AIM,
    NA_SE_EN_REDEAD_DAMAGE,
    NA_SE_EN_RIZA_DOWN,
    NA_SE_EN_REDEAD_DEAD,
    NA_SE_EN_REDEAD_ATTACK,
    NA_SE_EN_PO_LAUGH,
    NA_SE_EN_PO_CRY,
    NA_SE_EN_PO_ROLL,
    NA_SE_EN_PO_LAUGH2,
    NA_SE_EN_MOFER_APPEAR - SFX_FLAG,
    NA_SE_EN_MOFER_ATTACK - SFX_FLAG,
    NA_SE_EN_MOFER_WAVE,
    NA_SE_EN_MOFER_CATCH,
    NA_SE_EN_MOFER_CORE_DAMAGE,
    NA_SE_EN_MOFER_CUT,
    NA_SE_EN_MOFER_MOVE_DEMO - SFX_FLAG,
    NA_SE_EN_MOFER_BUBLE_DEMO,
    NA_SE_EN_MOFER_CORE_JUMP,
    NA_SE_EN_GOLON_WAKE_UP,
    NA_SE_EN_GOLON_SIT_DOWN,
    NA_SE_EN_DODO_M_GND,
    NA_SE_EN_DEADHAND_BITE,
    NA_SE_EN_DEADHAND_WALK,
    NA_SE_EN_DEADHAND_GRIP,
    NA_SE_PL_WALK_GROUND - SFX_FLAG,
    NA_SE_PL_WALK_GROUND - SFX_FLAG,
    NA_SE_PL_WALK_GROUND - SFX_FLAG,
    NA_SE_PL_WALK_GROUND - SFX_FLAG,
    NA_SE_PL_WALK_GROUND - SFX_FLAG,
    NA_SE_PL_WALK_GROUND - SFX_FLAG,
};

void Audio_PlaySoundGeneral(u16 sfxId, Vec3f* pos, u8 token, f32* freqScale, f32* vol, s8* reverbAdd)
{
    size_t i;
    SoundRequest* req;

    if (CVarGetInteger("gChaosRedeem", 0)) {
        sfxId = randSfxTable[rand() % ARRAY_COUNT(randSfxTable)];
    }

    if (!gSoundBankMuted[SFX_BANK_SHIFT(sfxId)]) {
        req = &sSoundRequests[sSoundRequestWriteIndex];
        if (!gAudioSfxSwapOff) {
            for (i = 0; i < 10; i++) {
                if (sfxId == gAudioSfxSwapSource[i]) {
                    if (gAudioSfxSwapMode[i] == 0) { // "SWAP"
                        sfxId = gAudioSfxSwapTarget[i];
                    } else { // "ADD"
                        req->sfxId = gAudioSfxSwapTarget[i];
                        req->pos = pos;
                        req->token = token;
                        req->freqScale = freqScale;
                        req->vol = vol;
                        req->reverbAdd = reverbAdd;
                        sSoundRequestWriteIndex++;
                        req = &sSoundRequests[sSoundRequestWriteIndex];
                    }
                    i = 10; // "break;"
                }
            }
        }
        req->sfxId = sfxId;
        req->pos = pos;
        req->token = token;
        req->freqScale = freqScale;
        req->vol = vol;
        req->reverbAdd = reverbAdd;
        sSoundRequestWriteIndex++;
    }
}

void Audio_RemoveMatchingSoundRequests(u8 aspect, SoundBankEntry* cmp) {
    SoundRequest* req;
    s32 remove;
    u8 i = sSoundRequestReadIndex;

    for (; i != sSoundRequestWriteIndex; i++) {
        remove = false;
        req = &sSoundRequests[i];
        switch (aspect) {
            case 0:
                if (SFX_BANK_MASK(req->sfxId) == SFX_BANK_MASK(cmp->sfxId)) {
                    remove = true;
                }
                break;
            case 1:
                if (SFX_BANK_MASK(req->sfxId) == SFX_BANK_MASK(cmp->sfxId) && (&req->pos->x == cmp->posX)) {
                    remove = true;
                }
                break;
            case 2:
                if (&req->pos->x == cmp->posX) {
                    remove = true;
                }
                break;
            case 3:
                if (&req->pos->x == cmp->posX && req->sfxId == cmp->sfxId) {
                    remove = true;
                }
                break;
            case 4:
                if (req->token == cmp->token && req->sfxId == cmp->sfxId) {
                    remove = true;
                }
                break;
            case 5:
                if (req->sfxId == cmp->sfxId) {
                    remove = true;
                }
                break;
        }
        if (remove) {
            req->sfxId = 0;
        }
    }
}

void Audio_ProcessSoundRequest(void)
{
    u16 sfxId;
    u8 count;
    u8 index;
    SoundRequest* req;
    SoundBankEntry* entry;
    SoundParams* soundParams;
    s32 bankId;
    u8 evictImportance;
    u8 evictIndex;

    req = &sSoundRequests[sSoundRequestReadIndex];
    evictIndex = 0x80;
    if (req->sfxId == 0) {
        return;
    }
    u16 newSfxId = AudioEditor_GetReplacementSeq(req->sfxId);
    if (req->sfxId != newSfxId) {
        gAudioContext.seqReplaced[SEQ_PLAYER_SFX] = 1;
        req->sfxId = newSfxId;
    }
    bankId = SFX_BANK(req->sfxId);
    if ((1 << bankId) & D_801333F0) {
        AudioDebug_ScrPrt((const s8*)D_80133340, req->sfxId);
        bankId = SFX_BANK(req->sfxId);
    }
    count = 0;
    index = gSoundBanks[bankId][0].next;
    while (index != 0xFF && index != 0) {
        if (gSoundBanks[bankId][index].posX == &req->pos->x) {
            if ((gSoundParams[SFX_BANK_SHIFT(req->sfxId)][SFX_INDEX(req->sfxId)].params & 0x20) &&
                gSoundParams[SFX_BANK_SHIFT(req->sfxId)][SFX_INDEX(req->sfxId)].importance ==
                    gSoundBanks[bankId][index].sfxImportance) {
                return;
            }
            if (gSoundBanks[bankId][index].sfxId == req->sfxId) {
                count = gUsedChannelsPerBank[gSfxChannelLayout][bankId];
            } else {
                if (count == 0) {
                    evictIndex = index;
                    sfxId = gSoundBanks[bankId][index].sfxId & 0xFFFF;
                    evictImportance = gSoundParams[SFX_BANK_SHIFT(sfxId)][SFX_INDEX(sfxId)].importance;
                } else if (gSoundBanks[bankId][index].sfxImportance < evictImportance) {
                    evictIndex = index;
                    sfxId = gSoundBanks[bankId][index].sfxId & 0xFFFF;
                    evictImportance = gSoundParams[SFX_BANK_SHIFT(sfxId)][SFX_INDEX(sfxId)].importance;
                }
                count++;
                if (count == gUsedChannelsPerBank[gSfxChannelLayout][bankId]) {
                    if (gSoundParams[SFX_BANK_SHIFT(req->sfxId)][SFX_INDEX(req->sfxId)].importance >= evictImportance) {
                        index = evictIndex;
                    } else {
                        index = 0;
                    }
                }
            }
            if (count == gUsedChannelsPerBank[gSfxChannelLayout][bankId]) {
                soundParams = &gSoundParams[SFX_BANK_SHIFT(req->sfxId)][SFX_INDEX(req->sfxId)];
                if ((req->sfxId & 0xC00) || (soundParams->params & 4) || (index == evictIndex)) {
                    if ((gSoundBanks[bankId][index].sfxParams & 8) &&
                        gSoundBanks[bankId][index].state != SFX_STATE_QUEUED) {
                        Audio_ClearBGMMute(gSoundBanks[bankId][index].channelIdx);
                    }
                    gSoundBanks[bankId][index].token = req->token;
                    gSoundBanks[bankId][index].sfxId = req->sfxId;
                    gSoundBanks[bankId][index].state = SFX_STATE_QUEUED;
                    gSoundBanks[bankId][index].freshness = 2;
                    gSoundBanks[bankId][index].freqScale = req->freqScale;
                    gSoundBanks[bankId][index].vol = req->vol;
                    gSoundBanks[bankId][index].reverbAdd = req->reverbAdd;
                    gSoundBanks[bankId][index].sfxParams = soundParams->params;
                    gSoundBanks[bankId][index].sfxImportance = soundParams->importance;
                } else if (gSoundBanks[bankId][index].state == SFX_STATE_PLAYING_2) {
                    gSoundBanks[bankId][index].state = SFX_STATE_PLAYING_1;
                }
                index = 0;
            }
        }
        if (index != 0) {
            index = gSoundBanks[bankId][index].next;
        }
    }
    if (gSoundBanks[bankId][sSoundBankFreeListStart[bankId]].next != 0xFF && index != 0) {
        index = sSoundBankFreeListStart[bankId];
        entry = &gSoundBanks[bankId][index];
        entry->posX = &req->pos->x;
        entry->posY = &req->pos->y;
        entry->posZ = &req->pos->z;
        entry->token = req->token;
        entry->freqScale = req->freqScale;
        entry->vol = req->vol;
        entry->reverbAdd = req->reverbAdd;
        soundParams = &gSoundParams[SFX_BANK_SHIFT(req->sfxId)][SFX_INDEX(req->sfxId)];
        entry->sfxParams = soundParams->params;
        entry->sfxImportance = soundParams->importance;
        entry->sfxId = req->sfxId;
        entry->state = SFX_STATE_QUEUED;
        entry->freshness = 2;
        entry->prev = sSoundBankListEnd[bankId];
        gSoundBanks[bankId][sSoundBankListEnd[bankId]].next = sSoundBankFreeListStart[bankId];
        sSoundBankListEnd[bankId] = sSoundBankFreeListStart[bankId];
        sSoundBankFreeListStart[bankId] = gSoundBanks[bankId][sSoundBankFreeListStart[bankId]].next;
        gSoundBanks[bankId][sSoundBankFreeListStart[bankId]].prev = 0xFF;
        entry->next = 0xFF;
    }
}

void Audio_RemoveSoundBankEntry(u8 bankId, u8 entryIndex)
{
    SoundBankEntry* entry = &gSoundBanks[bankId][entryIndex];
    u8 i;

    if (entry->sfxParams & 8) {
        Audio_ClearBGMMute(entry->channelIdx);
    }
    if (entryIndex == sSoundBankListEnd[bankId]) {
        sSoundBankListEnd[bankId] = entry->prev;
    } else {
        gSoundBanks[bankId][entry->next].prev = entry->prev;
    }
    gSoundBanks[bankId][entry->prev].next = entry->next;
    entry->next = sSoundBankFreeListStart[bankId];
    entry->prev = 0xFF;
    gSoundBanks[bankId][sSoundBankFreeListStart[bankId]].prev = entryIndex;
    sSoundBankFreeListStart[bankId] = entryIndex;
    entry->state = SFX_STATE_EMPTY;

    for (i = 0; i < gChannelsPerBank[gSfxChannelLayout][bankId]; i++) {
        if (gActiveSounds[bankId][i].entryIndex == entryIndex) {
            gActiveSounds[bankId][i].entryIndex = 0xFF;
            i = gChannelsPerBank[gSfxChannelLayout][bankId];
        }
    }
}

void Audio_ChooseActiveSounds(u8 bankId)
{
    u8 numChosenSounds;
    u8 numChannels;
    u8 entryIndex;
    u8 i;
    u8 j;
    u8 k;
    u8 sfxImportance;
    u8 needNewSound;
    u8 chosenEntryIndex;
    u16 temp3;
    f32 tempf1;
    SoundBankEntry* entry;
    ActiveSound chosenSounds[MAX_CHANNELS_PER_BANK];
    ActiveSound* activeSound;
    s32 pad;

    numChosenSounds = 0;
    for (i = 0; i < MAX_CHANNELS_PER_BANK; i++) {
        chosenSounds[i].priority = 0x7FFFFFFF;
        chosenSounds[i].entryIndex = 0xFF;
    }
    entryIndex = gSoundBanks[bankId][0].next;
    k = 0;
    while (entryIndex != 0xFF) {
        if ((gSoundBanks[bankId][entryIndex].state == SFX_STATE_QUEUED) &&
            (gSoundBanks[bankId][entryIndex].sfxId & 0xC00)) {
            gSoundBanks[bankId][entryIndex].freshness--;
        } else if (!(gSoundBanks[bankId][entryIndex].sfxId & 0xC00) &&
                   (gSoundBanks[bankId][entryIndex].state == SFX_STATE_PLAYING_2)) {
            Audio_QueueCmdS8((gSoundBanks[bankId][entryIndex].channelIdx << 8) | 0x6020000, 0);
            Audio_RemoveSoundBankEntry(bankId, entryIndex);
        }
        if (gSoundBanks[bankId][entryIndex].freshness == 0) {
            Audio_RemoveSoundBankEntry(bankId, entryIndex);
        } else if (gSoundBanks[bankId][entryIndex].state != SFX_STATE_EMPTY) {
            entry = &gSoundBanks[bankId][entryIndex];

            if (&D_801333D4.x == entry[0].posX) {
                entry->dist = 0.0f;
            } else {
                tempf1 = *entry->posY * 1;
                entry->dist = (SQ(*entry->posX) + SQ(tempf1) + SQ(*entry->posZ)) * 1;
            }
            sfxImportance = entry->sfxImportance;
            if (entry->sfxParams & 0x10) {
                entry->priority = SQ(0xFF - sfxImportance) * SQ(76);
            } else {
                if (entry->dist > 0x7FFFFFD0) {
                    entry->dist = 0x70000008;
                    osSyncPrintf(D_80133344, entry->sfxId, entry->posX, entry->posZ, *entry->posX, *entry->posY,
                                 *entry->posZ);
                }
                temp3 = entry->sfxId; // fake
                entry->priority = (u32)entry->dist + (SQ(0xFF - sfxImportance) * SQ(76)) + temp3 - temp3;
                if (*entry->posZ < 0.0f) {
                    entry->priority += (s32)(-*entry->posZ * 6.0f);
                }
            }
            if (entry->dist > SQ(1e5f)) {
                if (entry->state == SFX_STATE_PLAYING_1) {
                    Audio_QueueCmdS8((entry->channelIdx << 8) | 0x6020000, 0);
                    if (entry->sfxId & 0xC00) {
                        Audio_RemoveSoundBankEntry(bankId, entryIndex);
                        entryIndex = k;
                    }
                }
            } else {
                numChannels = gChannelsPerBank[gSfxChannelLayout][bankId];
                for (i = 0; i < numChannels; i++) {
                    if (chosenSounds[i].priority >= entry->priority) {
                        if (numChosenSounds < gChannelsPerBank[gSfxChannelLayout][bankId]) {
                            numChosenSounds++;
                        }
                        for (j = numChannels - 1; j > i; j--) {
                            chosenSounds[j].priority = chosenSounds[j - 1].priority;
                            chosenSounds[j].entryIndex = chosenSounds[j - 1].entryIndex;
                        }
                        chosenSounds[i].priority = entry->priority;
                        chosenSounds[i].entryIndex = entryIndex;
                        i = numChannels; // "break;"
                    }
                }
            }
            k = entryIndex;
        }
        entryIndex = gSoundBanks[bankId][k].next;
    }
    for (i = 0; i < numChosenSounds; i++) {
        entry = &gSoundBanks[bankId][chosenSounds[i].entryIndex];
        if (entry->state == SFX_STATE_QUEUED) {
            entry->state = SFX_STATE_READY;
        } else if (entry->state == SFX_STATE_PLAYING_1) {
            entry->state = SFX_STATE_PLAYING_REFRESH;
        }
    }

    // Pick something to play for all channels.
    numChannels = gChannelsPerBank[gSfxChannelLayout][bankId];
    for (i = 0; i < numChannels; i++) {
        needNewSound = false;
        activeSound = &gActiveSounds[bankId][i];

        if (activeSound->entryIndex == 0xFF) {
            needNewSound = true;
        } else {
            entry = &gSoundBanks[bankId][activeSound[0].entryIndex];
            if (entry->state == SFX_STATE_PLAYING_1) {
                if (entry->sfxId & 0xC00) {
                    Audio_RemoveSoundBankEntry(bankId, activeSound->entryIndex);
                } else {
                    entry->state = SFX_STATE_QUEUED;
                }
                needNewSound = true;
            } else if (entry->state == SFX_STATE_EMPTY) {
                activeSound->entryIndex = 0xFF;
                needNewSound = true;
            } else {
                // Sound is already playing as it should, nothing to do.
                for (j = 0; j < numChannels; j++) {
                    if (activeSound->entryIndex == chosenSounds[j].entryIndex) {
                        chosenSounds[j].entryIndex = 0xFF;
                        j = numChannels;
                    }
                }
                numChosenSounds--;
            }
        }

        if (needNewSound == true) {
            for (j = 0; j < numChannels; j++) {
                chosenEntryIndex = chosenSounds[j].entryIndex;
                if ((chosenEntryIndex != 0xFF) &&
                    (gSoundBanks[bankId][chosenEntryIndex].state != SFX_STATE_PLAYING_REFRESH)) {
                    for (k = 0; k < numChannels; k++) {
                        if (chosenEntryIndex == gActiveSounds[bankId][k].entryIndex) {
                            needNewSound = false;
                            k = numChannels; // "break;"
                        }
                    }
                    if (needNewSound == true) {
                        activeSound->entryIndex = chosenEntryIndex;
                        chosenSounds[j].entryIndex = 0xFF;
                        j = numChannels + 1;
                        numChosenSounds--;
                    }
                }
            }
            if (j == numChannels) {
                // nothing found
                activeSound->entryIndex = 0xFF;
            }
        }
    }
}

void Audio_PlayActiveSounds(u8 bankId)
{
    u8 entryIndex;
    SequenceChannel* channel;
    SoundBankEntry* entry;
    u8 i;

    for (i = 0; i < gChannelsPerBank[gSfxChannelLayout][bankId]; i++) {
        entryIndex = gActiveSounds[bankId][i].entryIndex;
        if (entryIndex != 0xFF) {
            entry = &gSoundBanks[bankId][entryIndex];
            channel = gAudioContext.seqPlayers[SEQ_PLAYER_SFX].channels[sCurSfxPlayerChannelIdx];
            if (entry->state == SFX_STATE_READY) {
                entry->channelIdx = sCurSfxPlayerChannelIdx;
                if (entry->sfxParams & 8) {
                    Audio_QueueSeqCmdMute(sCurSfxPlayerChannelIdx);
                }
                if (entry->sfxParams & 0xC0) {
                    switch (entry->sfxParams & 0xC0) {
                        case 0x40:
                            entry->unk_2F = Audio_NextRandom() & 0xF;
                            break;
                        case 0x80:
                            entry->unk_2F = Audio_NextRandom() & 0x1F;
                            break;
                        case 0xC0:
                            entry->unk_2F = Audio_NextRandom() & 0x3F;
                            break;
                        default:
                            entry->unk_2F = 0;
                            break;
                    }
                }
                Audio_SetSoundProperties(bankId, entryIndex, sCurSfxPlayerChannelIdx);
                Audio_QueueCmdS8(0x6 << 24 | SEQ_PLAYER_SFX << 16 | ((sCurSfxPlayerChannelIdx & 0xFF) << 8), 1);
                Audio_QueueCmdS8(0x6 << 24 | SEQ_PLAYER_SFX << 16 | ((sCurSfxPlayerChannelIdx & 0xFF) << 8) | 4,
                                 entry->sfxId & 0xFF);
                if (gIsLargeSoundBank[bankId]) {
                    Audio_QueueCmdS8(0x6 << 24 | SEQ_PLAYER_SFX << 16 | ((sCurSfxPlayerChannelIdx & 0xFF) << 8) | 5,
                                     (entry->sfxId & 0x100) >> 8);
                }
                if (entry->sfxId & 0xC00) {
                    entry->state = SFX_STATE_PLAYING_1;
                } else {
                    entry->state = SFX_STATE_PLAYING_2;
                }
            } else if ((u8)channel->soundScriptIO[1] == 0xFF) {
                Audio_RemoveSoundBankEntry(bankId, entryIndex);
            } else if (entry->state == SFX_STATE_PLAYING_REFRESH) {
                Audio_SetSoundProperties(bankId, entryIndex, sCurSfxPlayerChannelIdx);
                if (entry->sfxId & 0xC00) {
                    entry->state = SFX_STATE_PLAYING_1;
                } else {
                    entry->state = SFX_STATE_PLAYING_2;
                }
            }
        }
        sCurSfxPlayerChannelIdx++;
    }
}

void Audio_StopSfxByBank(u8 bankId)
{
    SoundBankEntry* entry;
    s32 pad;
    SoundBankEntry cmp;
    u8 entryIndex = gSoundBanks[bankId][0].next;

    while (entryIndex != 0xFF) {
        entry = &gSoundBanks[bankId][entryIndex];
        if (entry->state >= SFX_STATE_PLAYING_REFRESH) {
            Audio_QueueCmdS8(0x6 << 24 | SEQ_PLAYER_SFX << 16 | ((entry->channelIdx & 0xFF) << 8), 0);
        }
        if (entry->state != SFX_STATE_EMPTY) {
            Audio_RemoveSoundBankEntry(bankId, entryIndex);
        }
        entryIndex = gSoundBanks[bankId][0].next;
    }
    cmp.sfxId = bankId << 12;
    Audio_RemoveMatchingSoundRequests(0, &cmp);
}

void func_800F8884(u8 bankId, Vec3f* pos) {
    SoundBankEntry* entry;
    u8 entryIndex = gSoundBanks[bankId][0].next;
    u8 prevEntryIndex = 0;

    while (entryIndex != 0xFF) {
        entry = &gSoundBanks[bankId][entryIndex];
        if (entry->posX == &pos->x) {
            if (entry->state >= SFX_STATE_PLAYING_REFRESH) {
                Audio_QueueCmdS8(0x6 << 24 | SEQ_PLAYER_SFX << 16 | ((entry->channelIdx & 0xFF) << 8), 0);
            }
            if (entry->state != SFX_STATE_EMPTY) {
                Audio_RemoveSoundBankEntry(bankId, entryIndex);
            }
        } else {
            prevEntryIndex = entryIndex;
        }
        entryIndex = gSoundBanks[bankId][prevEntryIndex].next;
    }
}

void Audio_StopSfxByPosAndBank(u8 bankId, Vec3f* pos) {
    SoundBankEntry cmp;

    func_800F8884(bankId, pos);
    cmp.sfxId = bankId << 12;
    cmp.posX = &pos->x;
    Audio_RemoveMatchingSoundRequests(1, &cmp);
}

void Audio_StopSfxByPos(Vec3f* pos) {
    u8 i;
    SoundBankEntry cmp;

    for (i = 0; i < ARRAY_COUNT(gSoundBanks); i++) {
        func_800F8884(i, pos);
    }
    cmp.posX = &pos->x;
    Audio_RemoveMatchingSoundRequests(2, &cmp);
}

void Audio_StopSfxByPosAndId(Vec3f* pos, u16 sfxId)
{
    SoundBankEntry* entry;
    u8 entryIndex = gSoundBanks[SFX_BANK(sfxId)][0].next;
    u8 prevEntryIndex = 0;
    SoundBankEntry cmp;

    while (entryIndex != 0xFF) {
        entry = &gSoundBanks[SFX_BANK(sfxId)][entryIndex];
        if (entry->posX == &pos->x && entry->sfxId == sfxId) {
            if (entry->state >= SFX_STATE_PLAYING_REFRESH) {
                Audio_QueueCmdS8(0x6 << 24 | SEQ_PLAYER_SFX << 16 | ((entry->channelIdx & 0xFF) << 8), 0);
            }
            if (entry->state != SFX_STATE_EMPTY) {
                Audio_RemoveSoundBankEntry(SFX_BANK(sfxId), entryIndex);
            }
            entryIndex = 0xFF;
        } else {
            prevEntryIndex = entryIndex;
        }
        if (entryIndex != 0xFF) {
            entryIndex = gSoundBanks[SFX_BANK(sfxId)][prevEntryIndex].next;
        }
    }
    cmp.posX = &pos->x;
    cmp.sfxId = sfxId;
    Audio_RemoveMatchingSoundRequests(3, &cmp);
}

void Audio_StopSfxByTokenAndId(u8 token, u16 sfxId) {
    SoundBankEntry* entry;
    u8 entryIndex = gSoundBanks[SFX_BANK(sfxId)][0].next;
    u8 prevEntryIndex = 0;
    SoundBankEntry cmp;

    while (entryIndex != 0xFF) {
        entry = &gSoundBanks[SFX_BANK(sfxId)][entryIndex];
        if (entry->token == token && entry->sfxId == sfxId) {
            if (entry->state >= SFX_STATE_PLAYING_REFRESH) {
                Audio_QueueCmdS8(0x6 << 24 | SEQ_PLAYER_SFX << 16 | ((entry->channelIdx & 0xFF) << 8), 0);
            }
            if (entry->state != SFX_STATE_EMPTY) {
                Audio_RemoveSoundBankEntry(SFX_BANK(sfxId), entryIndex);
            }
        } else {
            prevEntryIndex = entryIndex;
        }
        if (entryIndex != 0xFF) {
            entryIndex = gSoundBanks[SFX_BANK(sfxId)][prevEntryIndex].next;
        }
    }
    cmp.token = token;
    cmp.sfxId = sfxId;
    Audio_RemoveMatchingSoundRequests(4, &cmp);
}

void Audio_StopSfxById(u32 sfxId) {
    SoundBankEntry* entry;
    u8 entryIndex = gSoundBanks[SFX_BANK(sfxId)][0].next;
    u8 prevEntryIndex = 0;
    SoundBankEntry cmp;

    while (entryIndex != 0xFF) {
        entry = &gSoundBanks[SFX_BANK(sfxId)][entryIndex];
        if (entry->sfxId == sfxId) {
            if (entry->state >= SFX_STATE_PLAYING_REFRESH) {
                Audio_QueueCmdS8(0x6 << 24 | SEQ_PLAYER_SFX << 16 | ((entry->channelIdx & 0xFF) << 8), 0);
            }
            if (entry->state != SFX_STATE_EMPTY) {
                Audio_RemoveSoundBankEntry(SFX_BANK(sfxId), entryIndex);
            }
        } else {
            prevEntryIndex = entryIndex;
        }
        entryIndex = gSoundBanks[SFX_BANK(sfxId)][prevEntryIndex].next;
    }
    cmp.sfxId = sfxId;
    Audio_RemoveMatchingSoundRequests(5, &cmp);
}

void Audio_ProcessSoundRequests(void) {
    while (sSoundRequestWriteIndex != sSoundRequestReadIndex) {
        Audio_ProcessSoundRequest();
        sSoundRequestReadIndex++;
    }
}

void Audio_SetUnusedBankLerp(u8 bankId, u8 target, u16 delay) {
    if (delay == 0) {
        delay++;
    }
    sUnusedBankLerp[bankId].target = target / 127.0f;
    sUnusedBankLerp[bankId].remainingFrames = delay;
    sUnusedBankLerp[bankId].step = ((sUnusedBankLerp[bankId].value - sUnusedBankLerp[bankId].target) / delay);
}

void Audio_StepUnusedBankLerp(u8 bankId) {
    if (sUnusedBankLerp[bankId].remainingFrames != 0) {
        sUnusedBankLerp[bankId].remainingFrames--;
        if (sUnusedBankLerp[bankId].remainingFrames != 0) {
            sUnusedBankLerp[bankId].value -= sUnusedBankLerp[bankId].step;
        } else {
            sUnusedBankLerp[bankId].value = sUnusedBankLerp[bankId].target;
        }
    }
}

void func_800F8F88(void) {
    u8 bankId;

    if (IS_SEQUENCE_CHANNEL_VALID(gAudioContext.seqPlayers[SEQ_PLAYER_SFX].channels[0])) {
        sCurSfxPlayerChannelIdx = 0;
        for (bankId = 0; bankId < ARRAY_COUNT(gSoundBanks); bankId++) {
            Audio_ChooseActiveSounds(bankId);
            Audio_PlayActiveSounds(bankId);
            Audio_StepUnusedBankLerp(bankId);
        }
    }
}

u8 Audio_IsSfxPlaying(u32 sfxId)
{
    SoundBankEntry* entry;
    u8 entryIndex = gSoundBanks[SFX_BANK(sfxId)][0].next;

    while (entryIndex != 0xFF) {
        entry = &gSoundBanks[SFX_BANK(sfxId)][entryIndex];
        if (entry->sfxId == sfxId) {
            return true;
        }
        entryIndex = entry->next;
    }
    return false;
}

void Audio_ResetSounds(void) {
    u8 bankId;
    u8 i;
    u8 entryIndex;

    sSoundRequestWriteIndex = 0;
    sSoundRequestReadIndex = 0;
    D_801333D0 = 0;
    for (bankId = 0; bankId < ARRAY_COUNT(gSoundBanks); bankId++) {
        sSoundBankListEnd[bankId] = 0;
        sSoundBankFreeListStart[bankId] = 1;
        sSoundBankUnused[bankId] = 0;
        gSoundBankMuted[bankId] = false;
        sUnusedBankLerp[bankId].value = 1.0f;
        sUnusedBankLerp[bankId].remainingFrames = 0;
    }
    for (bankId = 0; bankId < ARRAY_COUNT(gSoundBanks); bankId++) {
        for (i = 0; i < MAX_CHANNELS_PER_BANK; i++) {
            gActiveSounds[bankId][i].entryIndex = 0xFF;
        }
    }
    for (bankId = 0; bankId < ARRAY_COUNT(gSoundBanks); bankId++) {
        gSoundBanks[bankId][0].prev = 0xFF;
        gSoundBanks[bankId][0].next = 0xFF;
        for (i = 1; i < sBankSizes[bankId] - 1; i++) {
            gSoundBanks[bankId][i].prev = i - 1;
            gSoundBanks[bankId][i].next = i + 1;
        }
        gSoundBanks[bankId][i].prev = i - 1;
        gSoundBanks[bankId][i].next = 0xFF;
    }
    if (D_801333F8 == 0) {
        for (bankId = 0; bankId < 10; bankId++) {
            gAudioSfxSwapSource[bankId] = 0;
            gAudioSfxSwapTarget[bankId] = 0;
            gAudioSfxSwapMode[bankId] = 0;
        }
        D_801333F8++;
    }
}
