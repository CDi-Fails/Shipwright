#include "SaveManager.h"
#include "OTRGlobals.h"

#include "z64.h"
#include "functions.h"
#include "macros.h"
#include "Cvar.h"

#define NOGDI // avoid various windows defines that conflict with things in z64.h
#include "spdlog/spdlog.h"

#include <fstream>
#include <filesystem>
#include <array>

extern "C" SaveContext gSaveContext;

std::filesystem::path SaveManager::GetFileName(int fileNum) {
    const std::filesystem::path sSavePath(Ship::GlobalCtx2::GetPathRelativeToAppDirectory("Save"));
    return sSavePath / ("file" + std::to_string(fileNum + 1) + ".sav");
}

SaveManager::SaveManager() {
    AddLoadFunction("base", 1, LoadBaseVersion1);
    AddLoadFunction("base", 2, LoadBaseVersion2);
    AddSaveFunction("base", 2, SaveBase);

    AddLoadFunction("randomizer", 1, LoadRandomizerVersion1);
    AddSaveFunction("randomizer", 1, SaveRandomizer);

    AddInitFunction(InitFileImpl);

    for (SaveFileMetaInfo& info : fileMetaInfo) {
        info.valid = false;
        info.deaths = 0;
        for (int i = 0; i < ARRAY_COUNT(info.playerName); i++) {
            info.playerName[i] = '\0';
        }
        info.healthCapacity = 0;
        info.questItems = 0;
        info.defense = 0;
        info.health = 0;

        for (int i = 0; i < ARRAY_COUNT(info.seedHash); i++) {
            info.seedHash[i] = 0;
        }

        info.randoSave = 0;
    }
}

void SaveManager::LoadRandomizerVersion1() {
    if(!CVar_GetS32("gRandomizer", 0)) return;

    for (int i = 0; i < ARRAY_COUNT(gSaveContext.itemLocations); i++) {
        SaveManager::Instance->LoadData("get" + std::to_string(i), gSaveContext.itemLocations[i].get);
        SaveManager::Instance->LoadData("check" + std::to_string(i), gSaveContext.itemLocations[i].check);
    }

    for (int i = 0; i < ARRAY_COUNT(gSaveContext.seedIcons); i++) {
        SaveManager::Instance->LoadData("seed" + std::to_string(i), gSaveContext.seedIcons[i]);
    }

    for (int i = 0; i < ARRAY_COUNT(gSaveContext.randoSettings); i++) {
        SaveManager::Instance->LoadData("sk" + std::to_string(i), gSaveContext.randoSettings[i].key);
        SaveManager::Instance->LoadData("sv" + std::to_string(i), gSaveContext.randoSettings[i].value);
    }

    for (int i = 0; i < ARRAY_COUNT(gSaveContext.hintLocations); i++) {
        SaveManager::Instance->LoadData("hc" + std::to_string(i), gSaveContext.hintLocations[i].check);
        for (int j = 0; j < ARRAY_COUNT(gSaveContext.hintLocations[i].hintText); j++) {
            SaveManager::Instance->LoadData("ht" + std::to_string(i) + "-" + std::to_string(j), gSaveContext.hintLocations[i].hintText[j]);
        }
    }

    for (int i = 0; i < ARRAY_COUNT(gSaveContext.childAltarText); i++) {
        SaveManager::Instance->LoadData("cat" + std::to_string(i), gSaveContext.childAltarText[i]);
    }

    for (int i = 0; i < ARRAY_COUNT(gSaveContext.adultAltarText); i++) {
        SaveManager::Instance->LoadData("aat" + std::to_string(i), gSaveContext.adultAltarText[i]);
    }

    for (int i = 0; i < ARRAY_COUNT(gSaveContext.ganonHintText); i++) {
        SaveManager::Instance->LoadData("ght" + std::to_string(i), gSaveContext.ganonHintText[i]);
    }

    for (int i = 0; i < ARRAY_COUNT(gSaveContext.ganonText); i++) {
        SaveManager::Instance->LoadData("gt" + std::to_string(i), gSaveContext.ganonText[i]);
    }
}

void SaveManager::SaveRandomizer() {

    if(!gSaveContext.n64ddFlag) return;

    for (int i = 0; i < ARRAY_COUNT(gSaveContext.itemLocations); i++) {
        SaveManager::Instance->SaveData("get" + std::to_string(i), gSaveContext.itemLocations[i].get);
        SaveManager::Instance->SaveData("check" + std::to_string(i), gSaveContext.itemLocations[i].check);
    }

    for (int i = 0; i < ARRAY_COUNT(gSaveContext.seedIcons); i++) {
        SaveManager::Instance->SaveData("seed" + std::to_string(i), gSaveContext.seedIcons[i]);
    }

    for (int i = 0; i < ARRAY_COUNT(gSaveContext.randoSettings); i++) {
        SaveManager::Instance->SaveData("sk" + std::to_string(i), gSaveContext.randoSettings[i].key);
        SaveManager::Instance->SaveData("sv" + std::to_string(i), gSaveContext.randoSettings[i].value);
    }

    for (int i = 0; i < ARRAY_COUNT(gSaveContext.hintLocations); i++) {
        SaveManager::Instance->SaveData("hc" + std::to_string(i), gSaveContext.hintLocations[i].check);
        for (int j = 0; j < ARRAY_COUNT(gSaveContext.hintLocations[i].hintText); j++) {
            SaveManager::Instance->SaveData("ht" + std::to_string(i) + "-" + std::to_string(j), gSaveContext.hintLocations[i].hintText[j]);
        }
    }

    for (int i = 0; i < ARRAY_COUNT(gSaveContext.childAltarText); i++) {
        SaveManager::Instance->SaveData("cat" + std::to_string(i), gSaveContext.childAltarText[i]);
    }

    for (int i = 0; i < ARRAY_COUNT(gSaveContext.adultAltarText); i++) {
        SaveManager::Instance->SaveData("aat" + std::to_string(i), gSaveContext.adultAltarText[i]);
    }

    for (int i = 0; i < ARRAY_COUNT(gSaveContext.ganonHintText); i++) {
        SaveManager::Instance->SaveData("ght" + std::to_string(i), gSaveContext.ganonHintText[i]);
    }

    for (int i = 0; i < ARRAY_COUNT(gSaveContext.ganonText); i++) {
        SaveManager::Instance->SaveData("gt" + std::to_string(i), gSaveContext.ganonText[i]);
    }
}

void SaveManager::Init() {
    const std::filesystem::path sSavePath(Ship::GlobalCtx2::GetPathRelativeToAppDirectory("Save"));
    const std::filesystem::path sGlobalPath = sSavePath / std::string("global.sav");
    auto sOldSavePath = Ship::GlobalCtx2::GetPathRelativeToAppDirectory("oot_save.sav");
    auto sOldBackupSavePath = Ship::GlobalCtx2::GetPathRelativeToAppDirectory("oot_save.bak");

    // If the save directory does not exist, create it
    if (!std::filesystem::exists(sSavePath)) {
        std::filesystem::create_directory(sSavePath);
    }

    // If there is a lingering unversioned save, convert it
    if (std::filesystem::exists(sOldSavePath)) {
        ConvertFromUnversioned();
        std::filesystem::rename(sOldSavePath, sOldBackupSavePath);
    }

    // If the global save file exist, load it. Otherwise, create it.
    if (std::filesystem::exists(sGlobalPath)) {
        std::ifstream input(sGlobalPath);
        nlohmann::json globalBlock;
        input >> globalBlock;

        if (!globalBlock.contains("version")) {
            SPDLOG_WARN("Global save does not contain a version. We are reconstructing it.");
            CreateDefaultGlobal();
            return;
        }

        switch (globalBlock["version"].get<int>()) {
            case 1:
                currentJsonContext = &globalBlock;
                LoadData("audioSetting", gSaveContext.audioSetting);
                LoadData("zTargetSetting", gSaveContext.zTargetSetting);
                LoadData("language", gSaveContext.language);
                break;
            default:
                SPDLOG_WARN("Global save has a unrecognized version. We are reconstructing it.");
                CreateDefaultGlobal();
                break;
        }
    } else {
        CreateDefaultGlobal();
    }

    // Load files to initialize metadata
    for (int fileNum = 0; fileNum < MaxFiles; fileNum++) {
        if (std::filesystem::exists(GetFileName(fileNum))) {
            LoadFile(fileNum);
        }

    }
}

void SaveManager::InitMeta(int fileNum) {
    fileMetaInfo[fileNum].valid = true;
    fileMetaInfo[fileNum].deaths = gSaveContext.deaths;
    for (int i = 0; i < ARRAY_COUNT(fileMetaInfo[fileNum].playerName); i++) {
        fileMetaInfo[fileNum].playerName[i] = gSaveContext.playerName[i];
    }
    fileMetaInfo[fileNum].healthCapacity = gSaveContext.healthCapacity;
    fileMetaInfo[fileNum].questItems = gSaveContext.inventory.questItems;
    fileMetaInfo[fileNum].defense = gSaveContext.inventory.defenseHearts;
    fileMetaInfo[fileNum].health = gSaveContext.health;

    for (int i = 0; i < ARRAY_COUNT(fileMetaInfo[fileNum].seedHash); i++) {
        fileMetaInfo[fileNum].seedHash[i] = gSaveContext.seedIcons[i];
    }

    fileMetaInfo[fileNum].randoSave = gSaveContext.n64ddFlag;
}

void SaveManager::InitFile(bool isDebug) {
    for (InitFunc& func : initFuncs) {
        func(isDebug);
    }
}

void SaveManager::InitFileImpl(bool isDebug) {
    if (isDebug) {
        InitFileDebug();
    } else {
        InitFileNormal();
    }
}

void SaveManager::InitFileNormal() {
    gSaveContext.totalDays = 0;
    gSaveContext.bgsDayCount = 0;

    gSaveContext.deaths = 0;
    for (int i = 0; i < ARRAY_COUNT(gSaveContext.playerName); i++) {
        gSaveContext.playerName[i] = 0x3E;
    }
    gSaveContext.n64ddFlag = 0;
    gSaveContext.healthCapacity = 0x30;
    gSaveContext.health = 0x30;
    gSaveContext.magicLevel = 0;
    gSaveContext.magic = 0x30;
    gSaveContext.rupees = 0;
    gSaveContext.swordHealth = 0;
    gSaveContext.naviTimer = 0;
    gSaveContext.magicAcquired = 0;
    gSaveContext.doubleMagic = 0;
    gSaveContext.doubleDefense = 0;
    gSaveContext.bgsFlag = 0;
    gSaveContext.ocarinaGameRoundNum = 0;
    for (int button = 0; button < ARRAY_COUNT(gSaveContext.childEquips.buttonItems); button++) {
        gSaveContext.childEquips.buttonItems[button] = ITEM_NONE;
    }
    for (int button = 0; button < ARRAY_COUNT(gSaveContext.childEquips.cButtonSlots); button++) {
        gSaveContext.childEquips.cButtonSlots[button] = SLOT_NONE;
    }
    gSaveContext.childEquips.equipment = 0;
    for (int button = 0; button < ARRAY_COUNT(gSaveContext.adultEquips.buttonItems); button++) {
        gSaveContext.adultEquips.buttonItems[button] = ITEM_NONE;
    }
    for (int button = 0; button < ARRAY_COUNT(gSaveContext.adultEquips.cButtonSlots); button++) {
        gSaveContext.adultEquips.cButtonSlots[button] = SLOT_NONE;
    }
    gSaveContext.adultEquips.equipment = 0;
    gSaveContext.unk_54 = 0;
    gSaveContext.savedSceneNum = 0x34;

    // Equipment
    for (int button = 0; button < ARRAY_COUNT(gSaveContext.equips.buttonItems); button++) {
        gSaveContext.equips.buttonItems[button] = ITEM_NONE;
    }
    for (int button = 0; button < ARRAY_COUNT(gSaveContext.equips.cButtonSlots); button++) {
        gSaveContext.equips.cButtonSlots[button] = SLOT_NONE;
    }
    gSaveContext.equips.equipment = 0x1100;

    // Inventory
    for (int item = 0; item < ARRAY_COUNT(gSaveContext.inventory.items); item++) {
        gSaveContext.inventory.items[item] = ITEM_NONE;
    }
    for (int ammo = 0; ammo < ARRAY_COUNT(gSaveContext.inventory.ammo); ammo++) {
        gSaveContext.inventory.ammo[ammo] = 0;
    }
    gSaveContext.inventory.equipment = 0x1100;
    gSaveContext.inventory.upgrades = 0;
    gSaveContext.inventory.questItems = 0;
    for (int dungeon = 0; dungeon < ARRAY_COUNT(gSaveContext.inventory.dungeonItems); dungeon++) {
        gSaveContext.inventory.dungeonItems[dungeon] = 0;
    }
    for (int dungeon = 0; dungeon < ARRAY_COUNT(gSaveContext.inventory.dungeonKeys); dungeon++) {
        gSaveContext.inventory.dungeonKeys[dungeon] = 0xFF;
    }
    gSaveContext.inventory.defenseHearts = 0;
    gSaveContext.inventory.gsTokens = 0;
    for (int scene = 0; scene < ARRAY_COUNT(gSaveContext.sceneFlags); scene++) {
        gSaveContext.sceneFlags[scene].chest = 0;
        gSaveContext.sceneFlags[scene].swch = 0;
        gSaveContext.sceneFlags[scene].clear = 0;
        gSaveContext.sceneFlags[scene].collect = 0;
        gSaveContext.sceneFlags[scene].unk = 0;
        gSaveContext.sceneFlags[scene].rooms = 0;
        gSaveContext.sceneFlags[scene].floors = 0;
    }
    gSaveContext.fw.pos.x = 0;
    gSaveContext.fw.pos.y = 0;
    gSaveContext.fw.pos.z = 0;
    gSaveContext.fw.yaw = 0;
    gSaveContext.fw.playerParams = 0;
    gSaveContext.fw.entranceIndex = 0;
    gSaveContext.fw.roomIndex = 0;
    gSaveContext.fw.set = 0;
    gSaveContext.fw.tempSwchFlags = 0;
    gSaveContext.fw.tempCollectFlags = 0;
    for (int flag = 0; flag < ARRAY_COUNT(gSaveContext.gsFlags); flag++) {
        gSaveContext.gsFlags[flag] = 0;
    }
    for (int highscore = 0; highscore < ARRAY_COUNT(gSaveContext.highScores); highscore++) {
        gSaveContext.highScores[highscore] = 0;
    }
    for (int flag = 0; flag < ARRAY_COUNT(gSaveContext.eventChkInf); flag++) {
        gSaveContext.eventChkInf[flag] = 0;
    }
    for (int flag = 0; flag < ARRAY_COUNT(gSaveContext.itemGetInf); flag++) {
        gSaveContext.itemGetInf[flag] = 0;
    }
    for (int flag = 0; flag < ARRAY_COUNT(gSaveContext.infTable); flag++) {
        gSaveContext.infTable[flag] = 0;
    }
    gSaveContext.worldMapAreaData = 0;
    gSaveContext.scarecrowCustomSongSet = 0;
    for (int i = 0; i < ARRAY_COUNT(gSaveContext.scarecrowCustomSong); i++) {
        gSaveContext.scarecrowCustomSong[i].noteIdx = 0;
        gSaveContext.scarecrowCustomSong[i].unk_01 = 0;
        gSaveContext.scarecrowCustomSong[i].unk_02 = 0;
        gSaveContext.scarecrowCustomSong[i].volume = 0;
        gSaveContext.scarecrowCustomSong[i].vibrato = 0;
        gSaveContext.scarecrowCustomSong[i].tone = 0;
        gSaveContext.scarecrowCustomSong[i].semitone = 0;
    }
    gSaveContext.scarecrowSpawnSongSet = 0;
    for (int i = 0; i < ARRAY_COUNT(gSaveContext.scarecrowSpawnSong); i++) {
        gSaveContext.scarecrowSpawnSong[i].noteIdx = 0;
        gSaveContext.scarecrowSpawnSong[i].unk_01 = 0;
        gSaveContext.scarecrowSpawnSong[i].unk_02 = 0;
        gSaveContext.scarecrowSpawnSong[i].volume = 0;
        gSaveContext.scarecrowSpawnSong[i].vibrato = 0;
        gSaveContext.scarecrowSpawnSong[i].tone = 0;
        gSaveContext.scarecrowSpawnSong[i].semitone = 0;
    }

    gSaveContext.horseData.scene = SCENE_SPOT00;
    gSaveContext.horseData.pos.x = -1840;
    gSaveContext.horseData.pos.y = 72;
    gSaveContext.horseData.pos.z = 5497;
    gSaveContext.horseData.angle = -0x6AD9;
    gSaveContext.magicLevel = 0;
    gSaveContext.infTable[29] = 1;
    gSaveContext.sceneFlags[5].swch = 0x40000000;

    //RANDOTODO (ADD ITEMLOCATIONS TO GSAVECONTEXT)
}

void SaveManager::InitFileDebug() {
    InitFileNormal();

    gSaveContext.totalDays = 0;
    gSaveContext.bgsDayCount = 0;

    gSaveContext.deaths = 0;
    static std::array<char, 8> sPlayerName = { 0x15, 0x12, 0x17, 0x14, 0x3E, 0x3E, 0x3E, 0x3E };
    for (int i = 0; i < ARRAY_COUNT(gSaveContext.playerName); i++) {
        gSaveContext.playerName[i] = sPlayerName[i];
    }
    gSaveContext.n64ddFlag = 0;
    gSaveContext.healthCapacity = 0xE0;
    gSaveContext.health = 0xE0;
    gSaveContext.magicLevel = 0;
    gSaveContext.magic = 0x30;
    gSaveContext.rupees = 150;
    gSaveContext.swordHealth = 8;
    gSaveContext.naviTimer = 0;
    gSaveContext.magicAcquired = 1;
    gSaveContext.doubleMagic = 0;
    gSaveContext.doubleDefense = 0;
    gSaveContext.bgsFlag = 0;
    gSaveContext.ocarinaGameRoundNum = 0;
    for (int button = 0; button < ARRAY_COUNT(gSaveContext.childEquips.buttonItems); button++) {
        gSaveContext.childEquips.buttonItems[button] = ITEM_NONE;
    }
    for (int button = 0; button < ARRAY_COUNT(gSaveContext.childEquips.cButtonSlots); button++) {
        gSaveContext.childEquips.cButtonSlots[button] = SLOT_NONE;
    }
    gSaveContext.childEquips.equipment = 0;
    for (int button = 0; button < ARRAY_COUNT(gSaveContext.adultEquips.buttonItems); button++) {
        gSaveContext.adultEquips.buttonItems[button] = ITEM_NONE;
    }
    for (int button = 0; button < ARRAY_COUNT(gSaveContext.adultEquips.cButtonSlots); button++) {
        gSaveContext.adultEquips.cButtonSlots[button] = SLOT_NONE;
    }
    gSaveContext.adultEquips.equipment = 0;
    gSaveContext.unk_54 = 0;
    gSaveContext.savedSceneNum = 0x51;

    // Equipment
    static std::array<u8, 8> sButtonItems = { ITEM_SWORD_MASTER, ITEM_BOW, ITEM_BOMB, ITEM_OCARINA_FAIRY, ITEM_NONE, ITEM_NONE, ITEM_NONE, ITEM_NONE };
    for (int button = 0; button < ARRAY_COUNT(gSaveContext.equips.buttonItems); button++) {
        gSaveContext.equips.buttonItems[button] = sButtonItems[button];
    }
    static std::array<u8, 7> sCButtonSlots = { SLOT_BOW, SLOT_BOMB, SLOT_OCARINA, SLOT_NONE, SLOT_NONE, SLOT_NONE, SLOT_NONE };
    for (int button = 0; button < ARRAY_COUNT(gSaveContext.equips.cButtonSlots); button++) {
        gSaveContext.equips.cButtonSlots[button] = sCButtonSlots[button];
    }
    gSaveContext.equips.equipment = 0x1122;

    // Inventory
    static std::array<u8, 24> sItems = {
        ITEM_STICK,     ITEM_NUT,           ITEM_BOMB,         ITEM_BOW,         ITEM_ARROW_FIRE,  ITEM_DINS_FIRE,
        ITEM_SLINGSHOT, ITEM_OCARINA_FAIRY, ITEM_BOMBCHU,      ITEM_HOOKSHOT,    ITEM_ARROW_ICE,   ITEM_FARORES_WIND,
        ITEM_BOOMERANG, ITEM_LENS,          ITEM_BEAN,         ITEM_HAMMER,      ITEM_ARROW_LIGHT, ITEM_NAYRUS_LOVE,
        ITEM_BOTTLE,    ITEM_POTION_RED,    ITEM_POTION_GREEN, ITEM_POTION_BLUE, ITEM_POCKET_EGG,  ITEM_WEIRD_EGG,
    };
    for (int item = 0; item < ARRAY_COUNT(gSaveContext.inventory.items); item++) {
        gSaveContext.inventory.items[item] = sItems[item];
    }
    static std::array<s8, 16> sAmmo = { 50, 50, 10, 30, 1, 1, 30, 1, 50, 1, 1, 1, 1, 1, 1, 1 };
    for (int ammo = 0; ammo < ARRAY_COUNT(gSaveContext.inventory.ammo); ammo++) {
        gSaveContext.inventory.ammo[ammo] = sAmmo[ammo];
    }
    gSaveContext.inventory.equipment = 0x7777;
    gSaveContext.inventory.upgrades = 0x125249;
    gSaveContext.inventory.questItems = 0x1E3FFFF;
    static std::array<u8, 20> sDungeonItems = { 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
    for (int dungeon = 0; dungeon < ARRAY_COUNT(gSaveContext.inventory.dungeonItems); dungeon++) {
        gSaveContext.inventory.dungeonItems[dungeon] = sDungeonItems[dungeon];
    }
    for (int dungeon = 0; dungeon < ARRAY_COUNT(gSaveContext.inventory.dungeonKeys); dungeon++) {
        gSaveContext.inventory.dungeonKeys[dungeon] = 8;
    }
    gSaveContext.inventory.defenseHearts = 0;
    gSaveContext.inventory.gsTokens = 0;

    gSaveContext.horseData.scene = SCENE_SPOT00;
    gSaveContext.horseData.pos.x = -1840;
    gSaveContext.horseData.pos.y = 72;
    gSaveContext.horseData.pos.z = 5497;
    gSaveContext.horseData.angle = -0x6AD9;
    gSaveContext.infTable[0] |= 0x5009;
    gSaveContext.infTable[29] = 0; // unset flag from normal file setup
    gSaveContext.eventChkInf[0] |= 0x123F;
    gSaveContext.eventChkInf[8] |= 1;
    gSaveContext.eventChkInf[12] |= 0x10;

    if (LINK_AGE_IN_YEARS == YEARS_CHILD) {
        gSaveContext.equips.buttonItems[0] = ITEM_SWORD_KOKIRI;
        Inventory_ChangeEquipment(EQUIP_SWORD, 1);
        if (gSaveContext.fileNum == 0xFF) {
            gSaveContext.equips.buttonItems[1] = ITEM_SLINGSHOT;
            gSaveContext.equips.cButtonSlots[0] = SLOT_SLINGSHOT;
            Inventory_ChangeEquipment(EQUIP_SHIELD, 1);
        }
    }

    gSaveContext.entranceIndex = 0xCD;
    gSaveContext.magicLevel = 0;
    gSaveContext.sceneFlags[5].swch = 0x40000000;
}

void SaveManager::SaveFile(int fileNum) {
    if (fileNum == 0xFF) {
        return;
    }

    nlohmann::json baseBlock;

    baseBlock["version"] = 1;
    baseBlock["sections"] = nlohmann::json::object();
    for (auto& section : sectionSaveHandlers) {
        nlohmann::json& sectionBlock = baseBlock["sections"][section.first];
        sectionBlock["version"] = section.second.first;

        currentJsonContext = &sectionBlock["data"];
        section.second.second();
    }

    std::ofstream output(GetFileName(fileNum));
    output << std::setw(4) << baseBlock << std::endl;

    InitMeta(fileNum);
}

void SaveManager::SaveGlobal() {
    nlohmann::json globalBlock;
    globalBlock["version"] = 1;
    globalBlock["audioSetting"] = gSaveContext.audioSetting;
    globalBlock["zTargetSetting"] = gSaveContext.zTargetSetting;
    globalBlock["language"] = gSaveContext.language;
    std::ofstream output("Save/global.sav");
    output << std::setw(4) << globalBlock << std::endl;
}

void SaveManager::LoadFile(int fileNum) {
    assert(std::filesystem::exists(GetFileName(fileNum)));
    InitFile(false);

    std::ifstream input(GetFileName(fileNum));
    nlohmann::json saveBlock;
    input >> saveBlock;
    if (!saveBlock.contains("version")) {
        SPDLOG_ERROR("Save at " + GetFileName(fileNum).string() + " contains no version");
        assert(false);
    }
    switch (saveBlock["version"].get<int>()) {
        case 1:
            for (auto& block : saveBlock["sections"].items()) {
                int sectionVersion = block.value()["version"];
                std::string sectionName = block.key();
                if (!sectionLoadHandlers.contains(sectionName)) {
                    // Unloadable sections aren't necessarily errors, they are probably mods that were unloaded
                    // TODO report in a more noticeable manner
                    SPDLOG_WARN("Save " + GetFileName(fileNum).string() + " contains unloadable section " + sectionName);
                    continue;
                }
                SectionLoadHandler& handler = sectionLoadHandlers[sectionName];
                if (!handler.contains(sectionVersion)) {
                    // A section that has a loader without a handler for the specific version means that the user has a mod
                    // at an earlier version than the save has. In this case, the user probably wants to load the save.
                    // Report the error so that the user can rectify the error.
                    // TODO report in a more noticeable manner
                    SPDLOG_ERROR("Save " + GetFileName(fileNum).string() + " contains section " + sectionName +
                                 " with an unloadable version " + std::to_string(sectionVersion));
                    assert(false);
                    continue;
                }
                currentJsonContext = &block.value()["data"];
                handler[sectionVersion]();
            }
            break;
        default:
            SPDLOG_ERROR("Unrecognized save version " + std::to_string(saveBlock["version"].get<int>()) + " in " +
                         GetFileName(fileNum).string());
            assert(false);
            break;
    }
    InitMeta(fileNum);
}

bool SaveManager::SaveFile_Exist(int fileNum) {
    
    try {
        std::filesystem::exists(GetFileName(fileNum));
        printf("File[%d] - exist \n",fileNum);
        return true;
    }
    catch(std::filesystem::filesystem_error const& ex) {
        printf("File[%d] - do not exist \n",fileNum);
        return false;
    }
}

void SaveManager::AddInitFunction(InitFunc func) {
    initFuncs.emplace_back(func);
}

void SaveManager::AddLoadFunction(const std::string& name, int version, LoadFunc func) {
    if (!sectionLoadHandlers.contains(name)) {
        sectionLoadHandlers[name] = SectionLoadHandler();
    }

    if (sectionLoadHandlers[name].contains(version)) {
        SPDLOG_ERROR("Adding load function for section and version that already has one: " + name + ", " + std::to_string(version));
        assert(false);
        return;
    }

    sectionLoadHandlers[name][version] = func;
}

void SaveManager::AddSaveFunction(const std::string& name, int version, SaveFunc func) {
    if (sectionSaveHandlers.contains(name)) {
        SPDLOG_ERROR("Adding save function for section that already has one: " + name);
        assert(false);
        return;
    }

    sectionSaveHandlers[name] = std::make_pair(version, func);
}

void SaveManager::AddPostFunction(const std::string& name, PostFunc func) {
    if (postHandlers.contains(name)) {
        SPDLOG_ERROR("Adding post function for section that already has one: " + name);
        assert(false);
        return;
    }

    postHandlers[name] = func;
}

void SaveManager::CreateDefaultGlobal() {
    gSaveContext.audioSetting = 0;
    gSaveContext.zTargetSetting = 0;
    gSaveContext.language = CVar_GetS32("gLanguages", LANGUAGE_ENG);

    SaveGlobal();
}

void SaveManager::LoadBaseVersion1() {
    SaveManager::Instance->LoadData("entranceIndex", gSaveContext.entranceIndex);
    SaveManager::Instance->LoadData("linkAge", gSaveContext.linkAge);
    SaveManager::Instance->LoadData("cutsceneIndex", gSaveContext.cutsceneIndex);
    SaveManager::Instance->LoadData("dayTime", gSaveContext.dayTime);
    SaveManager::Instance->LoadData("nightFlag", gSaveContext.nightFlag);
    SaveManager::Instance->LoadData("totalDays", gSaveContext.totalDays);
    SaveManager::Instance->LoadData("bgsDayCount", gSaveContext.bgsDayCount);
    SaveManager::Instance->LoadData("deaths", gSaveContext.deaths);
    SaveManager::Instance->LoadArray("playerName", ARRAY_COUNT(gSaveContext.playerName), [](size_t i) {
        SaveManager::Instance->LoadData("", gSaveContext.playerName[i]);
    });
    SaveManager::Instance->LoadData("n64ddFlag", gSaveContext.n64ddFlag);
    SaveManager::Instance->LoadData("healthCapacity", gSaveContext.healthCapacity);
    SaveManager::Instance->LoadData("health", gSaveContext.health);
    SaveManager::Instance->LoadData("magicLevel", gSaveContext.magicLevel);
    SaveManager::Instance->LoadData("magic", gSaveContext.magic);
    SaveManager::Instance->LoadData("rupees", gSaveContext.rupees);
    SaveManager::Instance->LoadData("swordHealth", gSaveContext.swordHealth);
    SaveManager::Instance->LoadData("naviTimer", gSaveContext.naviTimer);
    SaveManager::Instance->LoadData("magicAcquired", gSaveContext.magicAcquired);
    SaveManager::Instance->LoadData("doubleMagic", gSaveContext.doubleMagic);
    SaveManager::Instance->LoadData("doubleDefense", gSaveContext.doubleDefense);
    SaveManager::Instance->LoadData("bgsFlag", gSaveContext.bgsFlag);
    SaveManager::Instance->LoadData("ocarinaGameRoundNum", gSaveContext.ocarinaGameRoundNum);
    SaveManager::Instance->LoadStruct("childEquips", []() {
        SaveManager::Instance->LoadArray("buttonItems", ARRAY_COUNT(gSaveContext.childEquips.buttonItems), [](size_t i) {
                SaveManager::Instance->LoadData("", gSaveContext.childEquips.buttonItems[i],
                                                static_cast<uint8_t>(ITEM_NONE));
        });
        SaveManager::Instance->LoadArray("cButtonSlots", ARRAY_COUNT(gSaveContext.childEquips.cButtonSlots), [](size_t i) {
                SaveManager::Instance->LoadData("", gSaveContext.childEquips.cButtonSlots[i],
                                                static_cast<uint8_t>(SLOT_NONE));
        });
        SaveManager::Instance->LoadData("equipment", gSaveContext.childEquips.equipment);
    });
    SaveManager::Instance->LoadStruct("adultEquips", []() {
        SaveManager::Instance->LoadArray("buttonItems", ARRAY_COUNT(gSaveContext.adultEquips.buttonItems), [](size_t i) {
                SaveManager::Instance->LoadData("", gSaveContext.adultEquips.buttonItems[i],
                                                static_cast<uint8_t>(ITEM_NONE));
        });
        SaveManager::Instance->LoadArray("cButtonSlots", ARRAY_COUNT(gSaveContext.adultEquips.cButtonSlots), [](size_t i) {
                SaveManager::Instance->LoadData("", gSaveContext.adultEquips.cButtonSlots[i],
                                                static_cast<uint8_t>(SLOT_NONE));
        });
        SaveManager::Instance->LoadData("equipment", gSaveContext.adultEquips.equipment);
    });
    SaveManager::Instance->LoadData("unk_54", gSaveContext.unk_54);
    SaveManager::Instance->LoadData("savedSceneNum", gSaveContext.savedSceneNum);
    SaveManager::Instance->LoadStruct("equips", []() {
        SaveManager::Instance->LoadArray("buttonItems", ARRAY_COUNT(gSaveContext.equips.buttonItems), [](size_t i) {
            SaveManager::Instance->LoadData("", gSaveContext.equips.buttonItems[i], static_cast<uint8_t>(ITEM_NONE));
        });
        SaveManager::Instance->LoadArray("cButtonSlots", ARRAY_COUNT(gSaveContext.equips.cButtonSlots), [](size_t i) {
            SaveManager::Instance->LoadData("", gSaveContext.equips.cButtonSlots[i], static_cast<uint8_t>(SLOT_NONE));
        });
        SaveManager::Instance->LoadData("equipment", gSaveContext.equips.equipment);
    });
    SaveManager::Instance->LoadStruct("inventory", []() {
        SaveManager::Instance->LoadArray("items", ARRAY_COUNT(gSaveContext.inventory.items), [](size_t i) {
            SaveManager::Instance->LoadData("", gSaveContext.inventory.items[i]);
        });
        SaveManager::Instance->LoadArray("ammo", ARRAY_COUNT(gSaveContext.inventory.ammo), [](size_t i) {
            SaveManager::Instance->LoadData("", gSaveContext.inventory.ammo[i]);
        });
        SaveManager::Instance->LoadData("equipment", gSaveContext.inventory.equipment);
        SaveManager::Instance->LoadData("upgrades", gSaveContext.inventory.upgrades);
        SaveManager::Instance->LoadData("questItems", gSaveContext.inventory.questItems);
        SaveManager::Instance->LoadArray("dungeonItems", ARRAY_COUNT(gSaveContext.inventory.dungeonItems), [](size_t i) {
            SaveManager::Instance->LoadData("", gSaveContext.inventory.dungeonItems[i]);
        });
        SaveManager::Instance->LoadArray("dungeonKeys", ARRAY_COUNT(gSaveContext.inventory.dungeonKeys), [](size_t i) {
            SaveManager::Instance->LoadData("", gSaveContext.inventory.dungeonKeys[i]);
        });
        SaveManager::Instance->LoadData("defenseHearts", gSaveContext.inventory.defenseHearts);
        SaveManager::Instance->LoadData("gsTokens", gSaveContext.inventory.gsTokens);
    });
    SaveManager::Instance->LoadArray("sceneFlags", ARRAY_COUNT(gSaveContext.sceneFlags), [](size_t i) {
        SaveManager::Instance->LoadStruct("", [&i]() {
            SaveManager::Instance->LoadData("chest", gSaveContext.sceneFlags[i].chest);
            SaveManager::Instance->LoadData("swch", gSaveContext.sceneFlags[i].swch);
            SaveManager::Instance->LoadData("clear", gSaveContext.sceneFlags[i].clear);
            SaveManager::Instance->LoadData("collect", gSaveContext.sceneFlags[i].collect);
            SaveManager::Instance->LoadData("unk", gSaveContext.sceneFlags[i].unk);
            SaveManager::Instance->LoadData("rooms", gSaveContext.sceneFlags[i].rooms);
            SaveManager::Instance->LoadData("floors", gSaveContext.sceneFlags[i].floors);
        });
    });
    SaveManager::Instance->LoadStruct("fw", []() {
        SaveManager::Instance->LoadStruct("pos", []() {
            SaveManager::Instance->LoadData("x", gSaveContext.fw.pos.x);
            SaveManager::Instance->LoadData("y", gSaveContext.fw.pos.y);
            SaveManager::Instance->LoadData("z", gSaveContext.fw.pos.z);
        });
        SaveManager::Instance->LoadData("yaw", gSaveContext.fw.yaw);
        SaveManager::Instance->LoadData("playerParams", gSaveContext.fw.playerParams);
        SaveManager::Instance->LoadData("entranceIndex", gSaveContext.fw.entranceIndex);
        SaveManager::Instance->LoadData("roomIndex", gSaveContext.fw.roomIndex);
        SaveManager::Instance->LoadData("set", gSaveContext.fw.set);
        SaveManager::Instance->LoadData("tempSwchFlags", gSaveContext.fw.tempSwchFlags);
        SaveManager::Instance->LoadData("tempCollectFlags", gSaveContext.fw.tempCollectFlags);
    });
    SaveManager::Instance->LoadArray("gsFlags", ARRAY_COUNT(gSaveContext.gsFlags), [](size_t i) {
        SaveManager::Instance->LoadData("", gSaveContext.gsFlags[i]);
    });
    SaveManager::Instance->LoadArray("highScores", ARRAY_COUNT(gSaveContext.highScores), [](size_t i) {
        SaveManager::Instance->LoadData("", gSaveContext.highScores[i]);
    });
    SaveManager::Instance->LoadArray("eventChkInf", ARRAY_COUNT(gSaveContext.eventChkInf), [](size_t i) {
        SaveManager::Instance->LoadData("", gSaveContext.eventChkInf[i]);
    });
    SaveManager::Instance->LoadArray("itemGetInf", ARRAY_COUNT(gSaveContext.itemGetInf), [](size_t i) {
        SaveManager::Instance->LoadData("", gSaveContext.itemGetInf[i]);
    });
    SaveManager::Instance->LoadArray("infTable", ARRAY_COUNT(gSaveContext.infTable), [](size_t i) {
        SaveManager::Instance->LoadData("", gSaveContext.infTable[i]);
    });
    SaveManager::Instance->LoadData("worldMapAreaData", gSaveContext.worldMapAreaData);
    SaveManager::Instance->LoadData("scarecrowCustomSongSet", gSaveContext.scarecrowCustomSongSet);
    SaveManager::Instance->LoadArray("scarecrowCustomSong", sizeof(gSaveContext.scarecrowCustomSong), [](size_t i) {
        SaveManager::Instance->LoadData("", ((u8*)&gSaveContext.scarecrowCustomSong)[i]);
    });
    SaveManager::Instance->LoadData("scarecrowSpawnSongSet", gSaveContext.scarecrowSpawnSongSet);
    SaveManager::Instance->LoadArray("scarecrowSpawnSong", sizeof(gSaveContext.scarecrowSpawnSong), [](size_t i) {
        SaveManager::Instance->LoadData("", ((u8*)&gSaveContext.scarecrowSpawnSong)[i]);
    });
    SaveManager::Instance->LoadStruct("horseData", []() {
        SaveManager::Instance->LoadData("scene", gSaveContext.horseData.scene);
        SaveManager::Instance->LoadStruct("pos", []() {
            SaveManager::Instance->LoadData("x", gSaveContext.horseData.pos.x);
            SaveManager::Instance->LoadData("y", gSaveContext.horseData.pos.y);
            SaveManager::Instance->LoadData("z", gSaveContext.horseData.pos.z);
        });
        SaveManager::Instance->LoadData("angle", gSaveContext.horseData.angle);
    });

    SaveManager::Instance->LoadArray("dungeonsDone", ARRAY_COUNT(gSaveContext.dungeonsDone), [](size_t i) {
        SaveManager::Instance->LoadData("", gSaveContext.dungeonsDone[i]);
    });

    SaveManager::Instance->LoadArray("trialsDone", ARRAY_COUNT(gSaveContext.trialsDone),
                                     [](size_t i) { SaveManager::Instance->LoadData("", gSaveContext.trialsDone[i]); });

    SaveManager::Instance->LoadArray("cowsMilked", ARRAY_COUNT(gSaveContext.cowsMilked), [](size_t i) {
        SaveManager::Instance->LoadData("", gSaveContext.cowsMilked[i]);
    });
}

void SaveManager::LoadBaseVersion2() {
    SaveManager::Instance->LoadData("entranceIndex", gSaveContext.entranceIndex);
    SaveManager::Instance->LoadData("linkAge", gSaveContext.linkAge);
    SaveManager::Instance->LoadData("cutsceneIndex", gSaveContext.cutsceneIndex);
    SaveManager::Instance->LoadData("dayTime", gSaveContext.dayTime);
    SaveManager::Instance->LoadData("nightFlag", gSaveContext.nightFlag);
    SaveManager::Instance->LoadData("totalDays", gSaveContext.totalDays);
    SaveManager::Instance->LoadData("bgsDayCount", gSaveContext.bgsDayCount);
    SaveManager::Instance->LoadData("deaths", gSaveContext.deaths);
    SaveManager::Instance->LoadArray("playerName", ARRAY_COUNT(gSaveContext.playerName), [](size_t i) {
        SaveManager::Instance->LoadData("", gSaveContext.playerName[i]);
    });
    SaveManager::Instance->LoadData("n64ddFlag", gSaveContext.n64ddFlag);
    SaveManager::Instance->LoadData("healthCapacity", gSaveContext.healthCapacity);
    SaveManager::Instance->LoadData("health", gSaveContext.health);
    SaveManager::Instance->LoadData("magicLevel", gSaveContext.magicLevel);
    SaveManager::Instance->LoadData("magic", gSaveContext.magic);
    SaveManager::Instance->LoadData("rupees", gSaveContext.rupees);
    SaveManager::Instance->LoadData("swordHealth", gSaveContext.swordHealth);
    SaveManager::Instance->LoadData("naviTimer", gSaveContext.naviTimer);
    SaveManager::Instance->LoadData("magicAcquired", gSaveContext.magicAcquired);
    SaveManager::Instance->LoadData("doubleMagic", gSaveContext.doubleMagic);
    SaveManager::Instance->LoadData("doubleDefense", gSaveContext.doubleDefense);
    SaveManager::Instance->LoadData("bgsFlag", gSaveContext.bgsFlag);
    SaveManager::Instance->LoadData("ocarinaGameRoundNum", gSaveContext.ocarinaGameRoundNum);
    SaveManager::Instance->LoadStruct("childEquips", []() {
        SaveManager::Instance->LoadArray("buttonItems", ARRAY_COUNT(gSaveContext.childEquips.buttonItems), [](size_t i) {
                SaveManager::Instance->LoadData("", gSaveContext.childEquips.buttonItems[i],
                                                static_cast<uint8_t>(ITEM_NONE));
        });
        SaveManager::Instance->LoadArray("cButtonSlots", ARRAY_COUNT(gSaveContext.childEquips.cButtonSlots), [](size_t i) {
                SaveManager::Instance->LoadData("", gSaveContext.childEquips.cButtonSlots[i],
                                                static_cast<uint8_t>(SLOT_NONE));
        });
        SaveManager::Instance->LoadData("equipment", gSaveContext.childEquips.equipment);
    });
    SaveManager::Instance->LoadStruct("adultEquips", []() {
        SaveManager::Instance->LoadArray("buttonItems", ARRAY_COUNT(gSaveContext.adultEquips.buttonItems), [](size_t i) {
                SaveManager::Instance->LoadData("", gSaveContext.adultEquips.buttonItems[i],
                                                static_cast<uint8_t>(ITEM_NONE));
        });
        SaveManager::Instance->LoadArray("cButtonSlots", ARRAY_COUNT(gSaveContext.adultEquips.cButtonSlots), [](size_t i) {
                SaveManager::Instance->LoadData("", gSaveContext.adultEquips.cButtonSlots[i],
                                                static_cast<uint8_t>(SLOT_NONE));
        });
        SaveManager::Instance->LoadData("equipment", gSaveContext.adultEquips.equipment);
    });
    SaveManager::Instance->LoadData("unk_54", gSaveContext.unk_54);
    SaveManager::Instance->LoadData("savedSceneNum", gSaveContext.savedSceneNum);
    SaveManager::Instance->LoadStruct("equips", []() {
        SaveManager::Instance->LoadArray("buttonItems", ARRAY_COUNT(gSaveContext.equips.buttonItems), [](size_t i) {
            SaveManager::Instance->LoadData("", gSaveContext.equips.buttonItems[i], static_cast<uint8_t>(ITEM_NONE));
        });
        SaveManager::Instance->LoadArray("cButtonSlots", ARRAY_COUNT(gSaveContext.equips.cButtonSlots), [](size_t i) {
            SaveManager::Instance->LoadData("", gSaveContext.equips.cButtonSlots[i], static_cast<uint8_t>(SLOT_NONE));
        });
        SaveManager::Instance->LoadData("equipment", gSaveContext.equips.equipment);
    });
    SaveManager::Instance->LoadStruct("inventory", []() {
        SaveManager::Instance->LoadArray("items", ARRAY_COUNT(gSaveContext.inventory.items), [](size_t i) {
            SaveManager::Instance->LoadData("", gSaveContext.inventory.items[i]);
        });
        SaveManager::Instance->LoadArray("ammo", ARRAY_COUNT(gSaveContext.inventory.ammo), [](size_t i) {
            SaveManager::Instance->LoadData("", gSaveContext.inventory.ammo[i]);
        });
        SaveManager::Instance->LoadData("equipment", gSaveContext.inventory.equipment);
        SaveManager::Instance->LoadData("upgrades", gSaveContext.inventory.upgrades);
        SaveManager::Instance->LoadData("questItems", gSaveContext.inventory.questItems);
        SaveManager::Instance->LoadArray("dungeonItems", ARRAY_COUNT(gSaveContext.inventory.dungeonItems), [](size_t i) {
            SaveManager::Instance->LoadData("", gSaveContext.inventory.dungeonItems[i]);
        });
        SaveManager::Instance->LoadArray("dungeonKeys", ARRAY_COUNT(gSaveContext.inventory.dungeonKeys), [](size_t i) {
            SaveManager::Instance->LoadData("", gSaveContext.inventory.dungeonKeys[i]);
        });
        SaveManager::Instance->LoadData("defenseHearts", gSaveContext.inventory.defenseHearts);
        SaveManager::Instance->LoadData("gsTokens", gSaveContext.inventory.gsTokens);
    });
    SaveManager::Instance->LoadArray("sceneFlags", ARRAY_COUNT(gSaveContext.sceneFlags), [](size_t i) {
        SaveManager::Instance->LoadStruct("", [&i]() {
            SaveManager::Instance->LoadData("chest", gSaveContext.sceneFlags[i].chest);
            SaveManager::Instance->LoadData("swch", gSaveContext.sceneFlags[i].swch);
            SaveManager::Instance->LoadData("clear", gSaveContext.sceneFlags[i].clear);
            SaveManager::Instance->LoadData("collect", gSaveContext.sceneFlags[i].collect);
            SaveManager::Instance->LoadData("unk", gSaveContext.sceneFlags[i].unk);
            SaveManager::Instance->LoadData("rooms", gSaveContext.sceneFlags[i].rooms);
            SaveManager::Instance->LoadData("floors", gSaveContext.sceneFlags[i].floors);
        });
    });
    SaveManager::Instance->LoadStruct("fw", []() {
        SaveManager::Instance->LoadStruct("pos", []() {
            SaveManager::Instance->LoadData("x", gSaveContext.fw.pos.x);
            SaveManager::Instance->LoadData("y", gSaveContext.fw.pos.y);
            SaveManager::Instance->LoadData("z", gSaveContext.fw.pos.z);
        });
        SaveManager::Instance->LoadData("yaw", gSaveContext.fw.yaw);
        SaveManager::Instance->LoadData("playerParams", gSaveContext.fw.playerParams);
        SaveManager::Instance->LoadData("entranceIndex", gSaveContext.fw.entranceIndex);
        SaveManager::Instance->LoadData("roomIndex", gSaveContext.fw.roomIndex);
        SaveManager::Instance->LoadData("set", gSaveContext.fw.set);
        SaveManager::Instance->LoadData("tempSwchFlags", gSaveContext.fw.tempSwchFlags);
        SaveManager::Instance->LoadData("tempCollectFlags", gSaveContext.fw.tempCollectFlags);
    });
    SaveManager::Instance->LoadArray("gsFlags", ARRAY_COUNT(gSaveContext.gsFlags), [](size_t i) {
        SaveManager::Instance->LoadData("", gSaveContext.gsFlags[i]);
    });
    SaveManager::Instance->LoadArray("highScores", ARRAY_COUNT(gSaveContext.highScores), [](size_t i) {
        SaveManager::Instance->LoadData("", gSaveContext.highScores[i]);
    });
    SaveManager::Instance->LoadArray("eventChkInf", ARRAY_COUNT(gSaveContext.eventChkInf), [](size_t i) {
        SaveManager::Instance->LoadData("", gSaveContext.eventChkInf[i]);
    });
    SaveManager::Instance->LoadArray("itemGetInf", ARRAY_COUNT(gSaveContext.itemGetInf), [](size_t i) {
        SaveManager::Instance->LoadData("", gSaveContext.itemGetInf[i]);
    });
    SaveManager::Instance->LoadArray("infTable", ARRAY_COUNT(gSaveContext.infTable), [](size_t i) {
        SaveManager::Instance->LoadData("", gSaveContext.infTable[i]);
    });
    SaveManager::Instance->LoadData("worldMapAreaData", gSaveContext.worldMapAreaData);
    SaveManager::Instance->LoadData("scarecrowCustomSongSet", gSaveContext.scarecrowCustomSongSet);
    SaveManager::Instance->LoadArray("scarecrowCustomSong", ARRAY_COUNT(gSaveContext.scarecrowCustomSong), [](size_t i) {
        SaveManager::Instance->LoadStruct("", [&i]() {
            SaveManager::Instance->LoadData("noteIdx", gSaveContext.scarecrowCustomSong[i].noteIdx);
            SaveManager::Instance->LoadData("unk_01", gSaveContext.scarecrowCustomSong[i].unk_01);
            SaveManager::Instance->LoadData("unk_02", gSaveContext.scarecrowCustomSong[i].unk_02);
            SaveManager::Instance->LoadData("volume", gSaveContext.scarecrowCustomSong[i].volume);
            SaveManager::Instance->LoadData("vibrato", gSaveContext.scarecrowCustomSong[i].vibrato);
            SaveManager::Instance->LoadData("tone", gSaveContext.scarecrowCustomSong[i].tone);
            SaveManager::Instance->LoadData("semitone", gSaveContext.scarecrowCustomSong[i].semitone);
        });
    });
    SaveManager::Instance->LoadData("scarecrowSpawnSongSet", gSaveContext.scarecrowSpawnSongSet);
    SaveManager::Instance->LoadArray("scarecrowSpawnSong", ARRAY_COUNT(gSaveContext.scarecrowSpawnSong), [](size_t i) {
        SaveManager::Instance->LoadStruct("", [&i]() {
            SaveManager::Instance->LoadData("noteIdx", gSaveContext.scarecrowSpawnSong[i].noteIdx);
            SaveManager::Instance->LoadData("unk_01", gSaveContext.scarecrowSpawnSong[i].unk_01);
            SaveManager::Instance->LoadData("unk_02", gSaveContext.scarecrowSpawnSong[i].unk_02);
            SaveManager::Instance->LoadData("volume", gSaveContext.scarecrowSpawnSong[i].volume);
            SaveManager::Instance->LoadData("vibrato", gSaveContext.scarecrowSpawnSong[i].vibrato);
            SaveManager::Instance->LoadData("tone", gSaveContext.scarecrowSpawnSong[i].tone);
            SaveManager::Instance->LoadData("semitone", gSaveContext.scarecrowSpawnSong[i].semitone);
        });
    });
    SaveManager::Instance->LoadStruct("horseData", []() {
        SaveManager::Instance->LoadData("scene", gSaveContext.horseData.scene);
        SaveManager::Instance->LoadStruct("pos", []() {
            SaveManager::Instance->LoadData("x", gSaveContext.horseData.pos.x);
            SaveManager::Instance->LoadData("y", gSaveContext.horseData.pos.y);
            SaveManager::Instance->LoadData("z", gSaveContext.horseData.pos.z);
        });
        SaveManager::Instance->LoadData("angle", gSaveContext.horseData.angle);
    });

    SaveManager::Instance->LoadArray("dungeonsDone", ARRAY_COUNT(gSaveContext.dungeonsDone), [](size_t i) {
        SaveManager::Instance->LoadData("", gSaveContext.dungeonsDone[i]);
    });

    SaveManager::Instance->LoadArray("trialsDone", ARRAY_COUNT(gSaveContext.trialsDone),
                                     [](size_t i) { SaveManager::Instance->LoadData("", gSaveContext.trialsDone[i]); });

    SaveManager::Instance->LoadArray("cowsMilked", ARRAY_COUNT(gSaveContext.cowsMilked), [](size_t i) {
        SaveManager::Instance->LoadData("", gSaveContext.cowsMilked[i]);
    });
}

void SaveManager::SaveBase() {
    SaveManager::Instance->SaveData("entranceIndex", gSaveContext.entranceIndex);
    SaveManager::Instance->SaveData("linkAge", gSaveContext.linkAge);
    SaveManager::Instance->SaveData("cutsceneIndex", gSaveContext.cutsceneIndex);
    SaveManager::Instance->SaveData("dayTime", gSaveContext.dayTime);
    SaveManager::Instance->SaveData("nightFlag", gSaveContext.nightFlag);
    SaveManager::Instance->SaveData("totalDays", gSaveContext.totalDays);
    SaveManager::Instance->SaveData("bgsDayCount", gSaveContext.bgsDayCount);
    SaveManager::Instance->SaveData("deaths", gSaveContext.deaths);
    SaveManager::Instance->SaveArray("playerName", ARRAY_COUNT(gSaveContext.playerName), [](size_t i) {
        SaveManager::Instance->SaveData("", gSaveContext.playerName[i]);
    });
    SaveManager::Instance->SaveData("n64ddFlag", gSaveContext.n64ddFlag);
    SaveManager::Instance->SaveData("healthCapacity", gSaveContext.healthCapacity);
    SaveManager::Instance->SaveData("health", gSaveContext.health);
    SaveManager::Instance->SaveData("magicLevel", gSaveContext.magicLevel);
    SaveManager::Instance->SaveData("magic", gSaveContext.magic);
    SaveManager::Instance->SaveData("rupees", gSaveContext.rupees);
    SaveManager::Instance->SaveData("swordHealth", gSaveContext.swordHealth);
    SaveManager::Instance->SaveData("naviTimer", gSaveContext.naviTimer);
    SaveManager::Instance->SaveData("magicAcquired", gSaveContext.magicAcquired);
    SaveManager::Instance->SaveData("doubleMagic", gSaveContext.doubleMagic);
    SaveManager::Instance->SaveData("doubleDefense", gSaveContext.doubleDefense);
    SaveManager::Instance->SaveData("bgsFlag", gSaveContext.bgsFlag);
    SaveManager::Instance->SaveData("ocarinaGameRoundNum", gSaveContext.ocarinaGameRoundNum);
    SaveManager::Instance->SaveStruct("childEquips", []() {
        SaveManager::Instance->SaveArray("buttonItems", ARRAY_COUNT(gSaveContext.childEquips.buttonItems), [](size_t i) {
            SaveManager::Instance->SaveData("", gSaveContext.childEquips.buttonItems[i]);
        });
        SaveManager::Instance->SaveArray("cButtonSlots", ARRAY_COUNT(gSaveContext.childEquips.cButtonSlots), [](size_t i) {
            SaveManager::Instance->SaveData("", gSaveContext.childEquips.cButtonSlots[i]);
        });
        SaveManager::Instance->SaveData("equipment", gSaveContext.childEquips.equipment);
    });
    SaveManager::Instance->SaveStruct("adultEquips", []() {
        SaveManager::Instance->SaveArray("buttonItems", ARRAY_COUNT(gSaveContext.adultEquips.buttonItems), [](size_t i) {
            SaveManager::Instance->SaveData("", gSaveContext.adultEquips.buttonItems[i]);
        });
        SaveManager::Instance->SaveArray("cButtonSlots", ARRAY_COUNT(gSaveContext.adultEquips.cButtonSlots), [](size_t i) {
            SaveManager::Instance->SaveData("", gSaveContext.adultEquips.cButtonSlots[i]);
        });
        SaveManager::Instance->SaveData("equipment", gSaveContext.adultEquips.equipment);
    });
    SaveManager::Instance->SaveData("unk_54", gSaveContext.unk_54);
    SaveManager::Instance->SaveData("savedSceneNum", gSaveContext.savedSceneNum);
    SaveManager::Instance->SaveStruct("equips", []() {
        SaveManager::Instance->SaveArray("buttonItems", ARRAY_COUNT(gSaveContext.equips.buttonItems), [](size_t i) {
            SaveManager::Instance->SaveData("", gSaveContext.equips.buttonItems[i]);
        });
        SaveManager::Instance->SaveArray("cButtonSlots", ARRAY_COUNT(gSaveContext.equips.cButtonSlots), [](size_t i) {
            SaveManager::Instance->SaveData("", gSaveContext.equips.cButtonSlots[i]);
        });
        SaveManager::Instance->SaveData("equipment", gSaveContext.equips.equipment);
    });
    SaveManager::Instance->SaveStruct("inventory", []() {
        SaveManager::Instance->SaveArray("items", ARRAY_COUNT(gSaveContext.inventory.items), [](size_t i) {
            SaveManager::Instance->SaveData("", gSaveContext.inventory.items[i]);
        });
        SaveManager::Instance->SaveArray("ammo", ARRAY_COUNT(gSaveContext.inventory.ammo), [](size_t i) {
            SaveManager::Instance->SaveData("", gSaveContext.inventory.ammo[i]);
        });
        SaveManager::Instance->SaveData("equipment", gSaveContext.inventory.equipment);
        SaveManager::Instance->SaveData("upgrades", gSaveContext.inventory.upgrades);
        SaveManager::Instance->SaveData("questItems", gSaveContext.inventory.questItems);
        SaveManager::Instance->SaveArray("dungeonItems", ARRAY_COUNT(gSaveContext.inventory.dungeonItems), [](size_t i) {
            SaveManager::Instance->SaveData("", gSaveContext.inventory.dungeonItems[i]);
        });
        SaveManager::Instance->SaveArray("dungeonKeys", ARRAY_COUNT(gSaveContext.inventory.dungeonKeys), [](size_t i) {
            SaveManager::Instance->SaveData("", gSaveContext.inventory.dungeonKeys[i]);
        });
        SaveManager::Instance->SaveData("defenseHearts", gSaveContext.inventory.defenseHearts);
        SaveManager::Instance->SaveData("gsTokens", gSaveContext.inventory.gsTokens);
    });
    SaveManager::Instance->SaveArray("sceneFlags", ARRAY_COUNT(gSaveContext.sceneFlags), [](size_t i) {
        SaveManager::Instance->SaveStruct("", [&i]() {
            SaveManager::Instance->SaveData("chest", gSaveContext.sceneFlags[i].chest);
            SaveManager::Instance->SaveData("swch", gSaveContext.sceneFlags[i].swch);
            SaveManager::Instance->SaveData("clear", gSaveContext.sceneFlags[i].clear);
            SaveManager::Instance->SaveData("collect", gSaveContext.sceneFlags[i].collect);
            SaveManager::Instance->SaveData("unk", gSaveContext.sceneFlags[i].unk);
            SaveManager::Instance->SaveData("rooms", gSaveContext.sceneFlags[i].rooms);
            SaveManager::Instance->SaveData("floors", gSaveContext.sceneFlags[i].floors);
        });
    });
    SaveManager::Instance->SaveStruct("fw", []() {
        SaveManager::Instance->SaveStruct("pos", []() {
            SaveManager::Instance->SaveData("x", gSaveContext.fw.pos.x);
            SaveManager::Instance->SaveData("y", gSaveContext.fw.pos.y);
            SaveManager::Instance->SaveData("z", gSaveContext.fw.pos.z);
        });
        SaveManager::Instance->SaveData("yaw", gSaveContext.fw.yaw);
        SaveManager::Instance->SaveData("playerParams", gSaveContext.fw.playerParams);
        SaveManager::Instance->SaveData("entranceIndex", gSaveContext.fw.entranceIndex);
        SaveManager::Instance->SaveData("roomIndex", gSaveContext.fw.roomIndex);
        SaveManager::Instance->SaveData("set", gSaveContext.fw.set);
        SaveManager::Instance->SaveData("tempSwchFlags", gSaveContext.fw.tempSwchFlags);
        SaveManager::Instance->SaveData("tempCollectFlags", gSaveContext.fw.tempCollectFlags);
    });
    SaveManager::Instance->SaveArray("gsFlags", ARRAY_COUNT(gSaveContext.gsFlags), [](size_t i) {
        SaveManager::Instance->SaveData("", gSaveContext.gsFlags[i]);
    });
    SaveManager::Instance->SaveArray("highScores", ARRAY_COUNT(gSaveContext.highScores), [](size_t i) {
        SaveManager::Instance->SaveData("", gSaveContext.highScores[i]);
    });
    SaveManager::Instance->SaveArray("eventChkInf", ARRAY_COUNT(gSaveContext.eventChkInf), [](size_t i) {
        SaveManager::Instance->SaveData("", gSaveContext.eventChkInf[i]);
    });
    SaveManager::Instance->SaveArray("itemGetInf", ARRAY_COUNT(gSaveContext.itemGetInf), [](size_t i) {
        SaveManager::Instance->SaveData("", gSaveContext.itemGetInf[i]);
    });
    SaveManager::Instance->SaveArray("infTable", ARRAY_COUNT(gSaveContext.infTable), [](size_t i) {
        SaveManager::Instance->SaveData("", gSaveContext.infTable[i]);
    });
    SaveManager::Instance->SaveData("worldMapAreaData", gSaveContext.worldMapAreaData);
    SaveManager::Instance->SaveData("scarecrowCustomSongSet", gSaveContext.scarecrowCustomSongSet);
    SaveManager::Instance->SaveArray("scarecrowCustomSong", ARRAY_COUNT(gSaveContext.scarecrowCustomSong), [](size_t i) {
        SaveManager::Instance->SaveStruct("", [&i]() {
            SaveManager::Instance->SaveData("noteIdx", gSaveContext.scarecrowCustomSong[i].noteIdx);
            SaveManager::Instance->SaveData("unk_01", gSaveContext.scarecrowCustomSong[i].unk_01);
            SaveManager::Instance->SaveData("unk_02", gSaveContext.scarecrowCustomSong[i].unk_02);
            SaveManager::Instance->SaveData("volume", gSaveContext.scarecrowCustomSong[i].volume);
            SaveManager::Instance->SaveData("vibrato", gSaveContext.scarecrowCustomSong[i].vibrato);
            SaveManager::Instance->SaveData("tone", gSaveContext.scarecrowCustomSong[i].tone);
            SaveManager::Instance->SaveData("semitone", gSaveContext.scarecrowCustomSong[i].semitone);
        });
    });
    SaveManager::Instance->SaveData("scarecrowSpawnSongSet", gSaveContext.scarecrowSpawnSongSet);
    SaveManager::Instance->SaveArray("scarecrowSpawnSong", ARRAY_COUNT(gSaveContext.scarecrowSpawnSong), [](size_t i) {
        SaveManager::Instance->SaveStruct("", [&i]() {
            SaveManager::Instance->SaveData("noteIdx", gSaveContext.scarecrowSpawnSong[i].noteIdx);
            SaveManager::Instance->SaveData("unk_01", gSaveContext.scarecrowSpawnSong[i].unk_01);
            SaveManager::Instance->SaveData("unk_02", gSaveContext.scarecrowSpawnSong[i].unk_02);
            SaveManager::Instance->SaveData("volume", gSaveContext.scarecrowSpawnSong[i].volume);
            SaveManager::Instance->SaveData("vibrato", gSaveContext.scarecrowSpawnSong[i].vibrato);
            SaveManager::Instance->SaveData("tone", gSaveContext.scarecrowSpawnSong[i].tone);
            SaveManager::Instance->SaveData("semitone", gSaveContext.scarecrowSpawnSong[i].semitone);
        });
    });
    SaveManager::Instance->SaveStruct("horseData", []() {
        SaveManager::Instance->SaveData("scene", gSaveContext.horseData.scene);
        SaveManager::Instance->SaveStruct("pos", []() {
            SaveManager::Instance->SaveData("x", gSaveContext.horseData.pos.x);
            SaveManager::Instance->SaveData("y", gSaveContext.horseData.pos.y);
            SaveManager::Instance->SaveData("z", gSaveContext.horseData.pos.z);
        });
        SaveManager::Instance->SaveData("angle", gSaveContext.horseData.angle);
    });

    SaveManager::Instance->SaveArray("dungeonsDone", ARRAY_COUNT(gSaveContext.dungeonsDone), [](size_t i) {
        SaveManager::Instance->SaveData("", gSaveContext.dungeonsDone[i]);
    });

    SaveManager::Instance->SaveArray("trialsDone", ARRAY_COUNT(gSaveContext.trialsDone),
                                     [](size_t i) { SaveManager::Instance->SaveData("", gSaveContext.trialsDone[i]); });

    SaveManager::Instance->SaveArray("cowsMilked", ARRAY_COUNT(gSaveContext.cowsMilked), [](size_t i) {
        SaveManager::Instance->SaveData("", gSaveContext.cowsMilked[i]);
    });
}

void SaveManager::SaveArray(const std::string& name, const size_t size, SaveArrayFunc func) {
    // Create an empty array and set it as the current save context, then call the function that saves an array entry.
    nlohmann::json* saveJsonContext = currentJsonContext;
    currentJsonContext = &(*currentJsonContext)[name.c_str()];
    *currentJsonContext = nlohmann::json::array();
    for (size_t i = 0; i < size; i++) {
        func(i);
    }
    currentJsonContext = saveJsonContext;
}

void SaveManager::SaveStruct(const std::string& name, SaveStructFunc func) {
    // Create an empty struct and set it as the current save context, then call the function that saves the struct.
    // If it is an array entry, save it to the array instead.
    if (name == "") {
        nlohmann::json* saveJsonContext = currentJsonContext;
        nlohmann::json object = nlohmann::json::object();
        currentJsonContext = &object;
        func();
        currentJsonContext = saveJsonContext;
        (*currentJsonContext).push_back(object);
    } else {
        nlohmann::json* saveJsonContext = currentJsonContext;
        currentJsonContext = &(*currentJsonContext)[name.c_str()];
        *currentJsonContext = nlohmann::json::object();
        func();
        currentJsonContext = saveJsonContext;
    }
}

void SaveManager::LoadArray(const std::string& name, const size_t size, LoadArrayFunc func) {
    // Create an empty array and set it as the current save context, then call the function that loads an array entry.
    nlohmann::json* saveJsonContext = currentJsonContext;
    currentJsonContext = &(*currentJsonContext)[name.c_str()];
    currentJsonArrayContext = currentJsonContext->begin();
    size_t i = 0;
    for (; (currentJsonArrayContext != currentJsonContext->end()) && (i < size); i++, currentJsonArrayContext++) {
        func(i);
    }
    // Handle remainer of items. Either this was data that was manually deleted, or a later version extended the size of the array.
    // The later members will be default constructed.
    for (; i < size; i++) {
        func(i);
    }
    currentJsonContext = saveJsonContext;
}


void SaveManager::LoadStruct(const std::string& name, LoadStructFunc func) {
    // Create an empty struct and set it as the current load context, then call the function that loads the struct.
    // If it is an array entry, load it from the array instead.
    if (name == "") {
        nlohmann::json* saveJsonContext = currentJsonContext;
        nlohmann::json emptyObject = nlohmann::json::object();
        if (currentJsonArrayContext != currentJsonContext->end()) {
            currentJsonContext = &currentJsonArrayContext.value();
        } else {
            // This array member is past the data in the json file. Therefore, default construct it.
            // By assigning an empty object here, all attempts to load data members of it will default construct them.
            currentJsonContext = &emptyObject;
        }
        func();
        currentJsonContext = saveJsonContext;
    } else {
        nlohmann::json* saveJsonContext = currentJsonContext;
        currentJsonContext = &(*currentJsonContext)[name.c_str()];
        func();
        currentJsonContext = saveJsonContext;
    }
}

void SaveManager::CopyZeldaFile(int from, int to) {
    assert(std::filesystem::exists(GetFileName(from)));
    DeleteZeldaFile(to);
    std::filesystem::copy_file(GetFileName(from), GetFileName(to));
    fileMetaInfo[to].valid = true;
    fileMetaInfo[to].deaths = fileMetaInfo[from].deaths;
    for (int i = 0; i < ARRAY_COUNT(fileMetaInfo[to].playerName); i++) {
        fileMetaInfo[to].playerName[i] = fileMetaInfo[from].playerName[i];
    }
    for (int i = 0; i < ARRAY_COUNT(fileMetaInfo[to].seedHash); i++) {
        fileMetaInfo[to].seedHash[i] = fileMetaInfo[from].seedHash[i];
    }
    fileMetaInfo[to].healthCapacity = fileMetaInfo[from].healthCapacity;
    fileMetaInfo[to].questItems = fileMetaInfo[from].questItems;
    fileMetaInfo[to].defense = fileMetaInfo[from].defense;
    fileMetaInfo[to].health = fileMetaInfo[from].health;
    fileMetaInfo[to].randoSave = fileMetaInfo[from].randoSave;
}

void SaveManager::DeleteZeldaFile(int fileNum) {
    if (std::filesystem::exists(GetFileName(fileNum))) {
        std::filesystem::remove(GetFileName(fileNum));
    }
    fileMetaInfo[fileNum].valid = false;
    fileMetaInfo[fileNum].randoSave = false;
}

// Functionality required to convert old saves into versioned saves

// DO NOT EDIT ANY OF THE FOLLOWING STRUCTS
// They MUST remain unchanged to handle parsing the binary saves of old

typedef struct {
    /* 0x00 */ u8 buttonItems[4];
    /* 0x04 */ u8 cButtonSlots[3];
    /* 0x08 */ u16 equipment;
} ItemEquips_v0; // size = 0x0A

typedef struct {
    /* 0x00 */ u8 items[24];
    /* 0x18 */ s8 ammo[16];
    /* 0x28 */ u16 equipment;
    /* 0x2C */ u32 upgrades;
    /* 0x30 */ u32 questItems;
    /* 0x34 */ u8 dungeonItems[20];
    /* 0x48 */ s8 dungeonKeys[19];
    /* 0x5B */ s8 defenseHearts;
    /* 0x5C */ s16 gsTokens;
} Inventory_v0; // size = 0x5E

typedef struct {
    /* 0x00 */ u32 chest;
    /* 0x04 */ u32 swch;
    /* 0x08 */ u32 clear;
    /* 0x0C */ u32 collect;
    /* 0x10 */ u32 unk;
    /* 0x14 */ u32 rooms;
    /* 0x18 */ u32 floors;
} SavedSceneFlags_v0; // size = 0x1C

typedef struct {
    s32 x, y, z;
} Vec3i_v0; // size = 0x0C

typedef struct {
    /* 0x00 */ Vec3i_v0 pos;
    /* 0x0C */ s32 yaw;
    /* 0x10 */ s32 playerParams;
    /* 0x14 */ s32 entranceIndex;
    /* 0x18 */ s32 roomIndex;
    /* 0x1C */ s32 set;
    /* 0x20 */ s32 tempSwchFlags;
    /* 0x24 */ s32 tempCollectFlags;
} FaroresWindData_v0; // size = 0x28

typedef struct {
    s16 x, y, z;
} Vec3s_v0; // size = 0x06

typedef struct {
    /* 0x00 */ s16 scene;
    /* 0x02 */ Vec3s_v0 pos;
    /* 0x08 */ s16 angle;
} HorseData_v0; // size = 0x0A

typedef struct {
    f32 x, y, z;
} Vec3f_v0; // size = 0x0C

typedef struct {
    /* 0x00 */ Vec3f_v0 pos;
    /* 0x0C */ s16 yaw;
    /* 0x0E */ s16 playerParams;
    /* 0x10 */ s16 entranceIndex;
    /* 0x12 */ u8 roomIndex;
    /* 0x13 */ s8 data;
    /* 0x14 */ u32 tempSwchFlags;
    /* 0x18 */ u32 tempCollectFlags;
} RespawnData_v0; // size = 0x1C

typedef struct {
    /* 0x0000 */ s32 entranceIndex; // start of `save` substruct, originally called "memory"
    /* 0x0004 */ s32 linkAge;       // 0: Adult; 1: Child
    /* 0x0008 */ s32 cutsceneIndex;
    /* 0x000C */ u16 dayTime; // "zelda_time"
    /* 0x0010 */ s32 nightFlag;
    /* 0x0014 */ s32 totalDays;
    /* 0x0018 */ s32 bgsDayCount; // increments with totalDays, can be cleared with `Environment_ClearBgsDayCount`
    /* 0x001C */ char newf[6];    // string "ZELDAZ". start of `info` substruct, originally called "information"
    /* 0x0022 */ u16 deaths;
    /* 0x0024 */ char playerName[8];
    /* 0x002C */ s16 n64ddFlag;
    /* 0x002E */ s16 healthCapacity; // "max_life"
    /* 0x0030 */ s16 health;         // "now_life"
    /* 0x0032 */ s8 magicLevel;
    /* 0x0033 */ s8 magic;
    /* 0x0034 */ s16 rupees;
    /* 0x0036 */ u16 swordHealth;
    /* 0x0038 */ u16 naviTimer;
    /* 0x003A */ u8 magicAcquired;
    /* 0x003B */ char unk_3B[0x01];
    /* 0x003C */ u8 doubleMagic;
    /* 0x003D */ u8 doubleDefense;
    /* 0x003E */ u8 bgsFlag;
    /* 0x003F */ u8 ocarinaGameRoundNum;
    /* 0x0040 */ ItemEquips_v0 childEquips;
    /* 0x004A */ ItemEquips_v0 adultEquips;
    /* 0x0054 */ u32 unk_54; // this may be incorrect, currently used for alignement
    /* 0x0058 */ char unk_58[0x0E];
    /* 0x0066 */ s16 savedSceneNum;
    /* 0x0068 */ ItemEquips_v0 equips;
    /* 0x0074 */ Inventory_v0 inventory;
    /* 0x00D4 */ SavedSceneFlags_v0 sceneFlags[124];
    /* 0x0E64 */ FaroresWindData_v0 fw;
    /* 0x0E8C */ char unk_E8C[0x10];
    /* 0x0E9C */ s32 gsFlags[6];
    /* 0x0EB4 */ char unk_EB4[0x4];
    /* 0x0EB8 */ s32 highScores[7];
    /* 0x0ED4 */ u16 eventChkInf[14]; // "event_chk_inf"
    /* 0x0EF0 */ u16 itemGetInf[4];   // "item_get_inf"
    /* 0x0EF8 */ u16 infTable[30];    // "inf_table"
    /* 0x0F34 */ char unk_F34[0x04];
    /* 0x0F38 */ u32 worldMapAreaData; // "area_arrival"
    /* 0x0F3C */ char unk_F3C[0x4];
    /* 0x0F40 */ u8 scarecrowCustomSongSet;
    /* 0x0F41 */ u8 scarecrowCustomSong[0x360];
    /* 0x12A1 */ char unk_12A1[0x24];
    /* 0x12C5 */ u8 scarecrowSpawnSongSet;
    /* 0x12C6 */ u8 scarecrowSpawnSong[0x80];
    /* 0x1346 */ char unk_1346[0x02];
    /* 0x1348 */ HorseData_v0 horseData;
    /* 0x1352 */ u16 checksum; // "check_sum"
    /* 0x1354 */ s32 fileNum;  // "file_no"
    /* 0x1358 */ char unk_1358[0x0004];
    /* 0x135C */ s32 gameMode;
    /* 0x1360 */ s32 sceneSetupIndex;
    /* 0x1364 */ s32 respawnFlag;           // "restart_flag"
    /* 0x1368 */ RespawnData_v0 respawn[3]; // "restart_data"
    /* 0x13BC */ f32 entranceSpeed;
    /* 0x13C0 */ u16 entranceSound;
    /* 0x13C2 */ char unk_13C2[0x0001];
    /* 0x13C3 */ u8 unk_13C3;
    /* 0x13C4 */ s16 dogParams;
    /* 0x13C6 */ u8 textTriggerFlags;
    /* 0x13C7 */ u8 showTitleCard;
    /* 0x13C8 */ s16 nayrusLoveTimer;
    /* 0x13CA */ char unk_13CA[0x0002];
    /* 0x13CC */ s16 rupeeAccumulator;
    /* 0x13CE */ s16 timer1State;
    /* 0x13D0 */ s16 timer1Value;
    /* 0x13D2 */ s16 timer2State;
    /* 0x13D4 */ s16 timer2Value;
    /* 0x13D6 */ s16 timerX[2];
    /* 0x13DA */ s16 timerY[2];
    /* 0x13DE */ char unk_13DE[0x0002];
    /* 0x13E0 */ u8 seqId;
    /* 0x13E1 */ u8 natureAmbienceId;
    /* 0x13E2 */ u8 buttonStatus[5];
    /* 0x13E7 */ u8 unk_13E7;     // alpha related
    /* 0x13E8 */ u16 unk_13E8;    // alpha type?
    /* 0x13EA */ u16 unk_13EA;    // also alpha type?
    /* 0x13EC */ u16 unk_13EC;    // alpha type counter?
    /* 0x13EE */ u16 unk_13EE;    // previous alpha type?
    /* 0x13F0 */ s16 unk_13F0;    // magic related
    /* 0x13F2 */ s16 unk_13F2;    // magic related
    /* 0x13F4 */ s16 unk_13F4;    // magic related
    /* 0x13F6 */ s16 unk_13F6;    // magic related
    /* 0x13F8 */ s16 unk_13F8;    // magic related
    /* 0x13FA */ u16 eventInf[4]; // "event_inf"
    /* 0x1402 */ u16 mapIndex;    // intended for maps/minimaps but commonly used as the dungeon index
    /* 0x1404 */ u16 minigameState;
    /* 0x1406 */ u16 minigameScore; // "yabusame_total"
    /* 0x1408 */ char unk_1408[0x0001];
    /* 0x1409 */ u8 language; // NTSC 0: Japanese; 1: English | PAL 0: English; 1: German; 2: French
    /* 0x140A */ u8 audioSetting;
    /* 0x140B */ char unk_140B[0x0001];
    /* 0x140C */ u8 zTargetSetting; // 0: Switch; 1: Hold
    /* 0x140E */ u16 forcedSeqId;   // immediately start playing the sequence if set
    /* 0x1410 */ u8 unk_1410;       // transition related
    /* 0x1411 */ char unk_1411[0x0001];
    /* 0x1412 */ u16 nextCutsceneIndex;
    /* 0x1414 */ u8 cutsceneTrigger;
    /* 0x1415 */ u8 chamberCutsceneNum;
    /* 0x1416 */ u16 nextDayTime; // "next_zelda_time"
    /* 0x1418 */ u8 fadeDuration;
    /* 0x1419 */ u8 unk_1419; // transition related
    /* 0x141A */ u16 skyboxTime;
    /* 0x141C */ u8 dogIsLost;
    /* 0x141D */ u8 nextTransition;
    /* 0x141E */ char unk_141E[0x0002];
    /* 0x1420 */ s16 worldMapArea;
    /* 0x1422 */ s16 sunsSongState; // controls the effects of suns song
    /* 0x1424 */ s16 healthAccumulator;
} SaveContext_v0; // size = 0x1428

void CopyV0Save(SaveContext_v0& src, SaveContext& dst) {
    dst.entranceIndex = src.entranceIndex;
    dst.linkAge = src.linkAge;
    dst.cutsceneIndex = src.cutsceneIndex;
    dst.dayTime = src.dayTime;
    dst.nightFlag = src.nightFlag;
    dst.totalDays = src.totalDays;
    dst.bgsDayCount = src.bgsDayCount;
    dst.deaths = src.deaths;
    for (size_t i = 0; i < ARRAY_COUNT(src.playerName); i++) {
        dst.playerName[i] = src.playerName[i];
    }
    dst.n64ddFlag = src.n64ddFlag;
    dst.healthCapacity = src.healthCapacity;
    dst.health = src.health;
    dst.magicLevel = src.magicLevel;
    dst.magic = src.magic;
    dst.rupees = src.rupees;
    dst.swordHealth = src.swordHealth;
    dst.naviTimer = src.naviTimer;
    dst.magicAcquired = src.magicAcquired;
    dst.doubleMagic = src.doubleMagic;
    dst.doubleDefense = src.doubleDefense;
    dst.bgsFlag = src.bgsFlag;
    dst.ocarinaGameRoundNum = src.ocarinaGameRoundNum;
    for (size_t i = 0; i < ARRAY_COUNT(src.childEquips.buttonItems); i++) {
        dst.childEquips.buttonItems[i] = src.childEquips.buttonItems[i];
    }
    for (size_t i = 0; i < ARRAY_COUNT(src.childEquips.cButtonSlots); i++) {
        dst.childEquips.cButtonSlots[i] = src.childEquips.cButtonSlots[i];
    }
    dst.childEquips.equipment = src.childEquips.equipment;
    for (size_t i = 0; i < ARRAY_COUNT(src.adultEquips.buttonItems); i++) {
        dst.adultEquips.buttonItems[i] = src.adultEquips.buttonItems[i];
    }
    for (size_t i = 0; i < ARRAY_COUNT(src.adultEquips.cButtonSlots); i++) {
        dst.adultEquips.cButtonSlots[i] = src.adultEquips.cButtonSlots[i];
    }
    dst.adultEquips.equipment = src.adultEquips.equipment;
    dst.unk_54 = src.unk_54;
    dst.savedSceneNum = src.savedSceneNum;
    for (size_t i = 0; i < ARRAY_COUNT(src.equips.buttonItems); i++) {
        dst.equips.buttonItems[i] = src.equips.buttonItems[i];
    }
    for (size_t i = 0; i < ARRAY_COUNT(src.equips.cButtonSlots); i++) {
        dst.equips.cButtonSlots[i] = src.equips.cButtonSlots[i];
    }
    dst.equips.equipment = src.equips.equipment;
    for (size_t i = 0; i < ARRAY_COUNT(src.inventory.items); i++) {
        dst.inventory.items[i] = src.inventory.items[i];
    }
    for (size_t i = 0; i < ARRAY_COUNT(src.inventory.ammo); i++) {
        dst.inventory.ammo[i] = src.inventory.ammo[i];
    }
    dst.inventory.equipment = src.inventory.equipment;
    dst.inventory.upgrades = src.inventory.upgrades;
    dst.inventory.questItems = src.inventory.questItems;
    for (size_t i = 0; i < ARRAY_COUNT(src.inventory.dungeonItems); i++) {
        dst.inventory.dungeonItems[i] = src.inventory.dungeonItems[i];
    }
    for (size_t i = 0; i < ARRAY_COUNT(src.inventory.dungeonKeys); i++) {
        dst.inventory.dungeonKeys[i] = src.inventory.dungeonKeys[i];
    }
    dst.inventory.defenseHearts = src.inventory.defenseHearts;
    dst.inventory.gsTokens = src.inventory.gsTokens;
    for (size_t i = 0; i < ARRAY_COUNT(src.sceneFlags); i++) {
        dst.sceneFlags[i].chest = src.sceneFlags[i].chest;
        dst.sceneFlags[i].swch = src.sceneFlags[i].swch;
        dst.sceneFlags[i].clear = src.sceneFlags[i].clear;
        dst.sceneFlags[i].collect = src.sceneFlags[i].collect;
        dst.sceneFlags[i].unk = src.sceneFlags[i].unk;
        dst.sceneFlags[i].rooms = src.sceneFlags[i].rooms;
        dst.sceneFlags[i].floors = src.sceneFlags[i].floors;
    }
    dst.fw.pos.x = src.fw.pos.x;
    dst.fw.pos.y = src.fw.pos.y;
    dst.fw.pos.z = src.fw.pos.z;
    dst.fw.yaw = src.fw.yaw;
    dst.fw.playerParams = src.fw.playerParams;
    dst.fw.entranceIndex = src.fw.entranceIndex;
    dst.fw.roomIndex = src.fw.roomIndex;
    dst.fw.set = src.fw.set;
    dst.fw.tempSwchFlags = src.fw.tempSwchFlags;
    dst.fw.tempCollectFlags = src.fw.tempCollectFlags;
    for (size_t i = 0; i < ARRAY_COUNT(src.gsFlags); i++) {
        dst.gsFlags[i] = src.gsFlags[i];
    }
    for (size_t i = 0; i < ARRAY_COUNT(src.highScores); i++) {
        dst.highScores[i] = src.highScores[i];
    }
    for (size_t i = 0; i < ARRAY_COUNT(src.eventChkInf); i++) {
        dst.eventChkInf[i] = src.eventChkInf[i];
    }
    for (size_t i = 0; i < ARRAY_COUNT(src.itemGetInf); i++) {
        dst.itemGetInf[i] = src.itemGetInf[i];
    }
    for (size_t i = 0; i < ARRAY_COUNT(src.infTable); i++) {
        dst.infTable[i] = src.infTable[i];
    }
    dst.worldMapAreaData = src.worldMapAreaData;
    dst.scarecrowCustomSongSet = src.scarecrowCustomSongSet;
    memcpy(&dst.scarecrowCustomSong[0], &src.scarecrowCustomSong[0], sizeof(src.scarecrowCustomSong));
    dst.scarecrowSpawnSongSet = src.scarecrowSpawnSongSet;
    memcpy(&dst.scarecrowSpawnSong[0], &src.scarecrowSpawnSong[0], sizeof(src.scarecrowSpawnSong));
    dst.horseData.scene = src.horseData.scene;
    dst.horseData.pos.x = src.horseData.pos.x;
    dst.horseData.pos.y = src.horseData.pos.y;
    dst.horseData.pos.z = src.horseData.pos.z;
    dst.horseData.angle = src.horseData.angle;
}

void SaveManager::ConvertFromUnversioned() {
    static char sZeldaMagic[] = { '\0', '\0', '\0', '\x98', '\x09', '\x10', '\x21', 'Z', 'E', 'L', 'D', 'A' };
#define SLOT_SIZE (sizeof(SaveContext_v0) + 0x28)
#define SLOT_OFFSET(index) (SRAM_HEADER_SIZE + 0x10 + (index * SLOT_SIZE))

    std::ifstream input("oot_save.sav", std::ios::binary);
    std::vector<char> data(std::istreambuf_iterator<char>(input), {});
    input.close();

    for (size_t i = 0; i < ARRAY_COUNT(sZeldaMagic) - 3; i++) {
        if (sZeldaMagic[i + SRAM_HEADER_MAGIC] != data[i + SRAM_HEADER_MAGIC]) {
            CreateDefaultGlobal();
            return;
        }
    }

    gSaveContext.audioSetting = data[SRAM_HEADER_SOUND] & 3;
    gSaveContext.zTargetSetting = data[SRAM_HEADER_ZTARGET] & 1;
    gSaveContext.language = data[SRAM_HEADER_LANGUAGE];
    if (gSaveContext.language >= LANGUAGE_MAX) {
        gSaveContext.language = CVar_GetS32("gLanguages", LANGUAGE_ENG);
    }
    SaveGlobal();

    for (int fileNum = 0; fileNum < 3; fileNum++) {
        SaveContext_v0* file = reinterpret_cast<SaveContext_v0*>(&data[SLOT_OFFSET(fileNum)]);
        if ((file->newf[0] == 'Z') && (file->newf[1] == 'E') && (file->newf[2] == 'L') && (file->newf[3] == 'D') &&
            (file->newf[4] == 'A') && (file->newf[5] == 'Z')) {
            // If a save is valid, convert the save by storing the current save context, converting the file, loading
            // it, saving it, then restoring the save context.
            static SaveContext saveContextSave = gSaveContext;
            InitFile(false);
            CopyV0Save(*file, gSaveContext);
            SaveFile(fileNum);
            InitMeta(fileNum);
            gSaveContext = saveContextSave;
        }
    }

#undef SLOT_SIZE
#undef SLOT_OFFSET
}

// C to C++ bridge

extern "C" void Save_Init(void) {
    SaveManager::Instance->Init();
}

extern "C" void Save_InitFile(int isDebug) {
    SaveManager::Instance->InitFile(isDebug != 0);
}

extern "C" void Save_SaveFile(void) {
    SaveManager::Instance->SaveFile(gSaveContext.fileNum);
}

extern "C" void Save_SaveGlobal(void) {
    SaveManager::Instance->SaveGlobal();
}

extern "C" void Save_LoadFile(void) {
    SaveManager::Instance->LoadFile(gSaveContext.fileNum);
}

extern "C" void Save_AddLoadFunction(char* name, int version, SaveManager::LoadFunc func) {
    SaveManager::Instance->AddLoadFunction(name, version, func);
}

extern "C" void Save_AddSaveFunction(char* name, int version, SaveManager::SaveFunc func) {
    SaveManager::Instance->AddSaveFunction(name, version, func);
}

extern "C" SaveFileMetaInfo* Save_GetSaveMetaInfo(int fileNum) {
    return &SaveManager::Instance->fileMetaInfo[fileNum];
}

extern "C" void Save_CopyFile(int from, int to) {
    SaveManager::Instance->CopyZeldaFile(from, to);
}

extern "C" void Save_DeleteFile(int fileNum) {
    SaveManager::Instance->DeleteZeldaFile(fileNum);
}

extern "C" bool Save_Exist(int fileNum) {
    return SaveManager::Instance->SaveFile_Exist(fileNum);
}