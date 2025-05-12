#include <spdlog/sinks/basic_file_sink.h>
#include <iostream>
#include <algorithm>

namespace logger = SKSE::log;


class cGlobalVariables : public RE::BSTEventSink<RE::MenuOpenCloseEvent>,
                         public RE::BSTEventSink<RE::LevelIncrease::Event>,
                         public RE::BSTEventSink<RE::SkillIncrease::Event>{ public:

    RE::TESGlobal *careStr = nullptr;
    RE::TESGlobal *careFor = nullptr;
    RE::TESGlobal *careEnd = nullptr;
    RE::TESGlobal *careInt = nullptr;
    RE::TESGlobal *careWis = nullptr;
    RE::TESGlobal *carePer = nullptr;
    RE::TESGlobal *careAgi = nullptr;
    RE::TESGlobal *careDex = nullptr;
    RE::TESGlobal *careCha = nullptr;
    RE::TESGlobal *careLuk = nullptr;
    RE::TESGlobal *careLukPerkChance = nullptr;
    RE::BGSPerk *carePERK = nullptr;
    RE::SpellItem *careSPEL = nullptr;
    RE::Setting *skyrimSprintStaminaDrain = nullptr;
    RE::TESGlobal *skyrimFavorGoldRewardSmall = nullptr;
    RE::TESGlobal *skyrimFavorRewardBribe = nullptr;
    RE::TESGlobal *skyrimFavorRewardIntimidate = nullptr;
    RE::TESGlobal *skyrimFavorRewardLarge = nullptr;
    RE::TESGlobal *skyrimFavorRewardMedium = nullptr;
    RE::TESGlobal *skyrimFavorRewardSmall = nullptr;
    RE::TESGlobal *skyrimMGR11Reward01 = nullptr;
    RE::TESGlobal *skyrimMGR11Reward02 = nullptr;
    RE::TESGlobal *skyrimMGR11Reward03 = nullptr;
    RE::TESGlobal *skyrimMGR11Reward04 = nullptr;
    RE::TESGlobal *skyrimMGR11Reward05 = nullptr;
    RE::TESGlobal *skyrimQuestReward01Small = nullptr;
    RE::TESGlobal *skyrimQuestReward02Medium = nullptr;
    RE::TESGlobal *skyrimQuestReward03Large = nullptr;
    RE::TESGlobal *skyrimQuestReward04Wow = nullptr;
    float baseSprintStaminaDrain;
    float baseFavorGoldRewardSmall;
    float baseFavorRewardBribe;
    float baseFavorRewardIntimidate;
    float baseFavorRewardLarge;
    float baseFavorRewardMedium;
    float baseFavorRewardSmall;
    float baseMGR11Reward01;
    float baseMGR11Reward02;
    float baseMGR11Reward03;
    float baseMGR11Reward04;
    float baseMGR11Reward05;
    float baseQuestReward01Small;
    float baseQuestReward02Medium;
    float baseQuestReward03Large;
    float baseQuestReward04Wow;
    int careSkillTreshold = 50;
    int careMaximumAttributeLevel = 20;
    int carePopUpEnabled = 1;
    const char *careExplanationText = "You broke the CARE INI file.";
    std::vector<std::string> skillAttributes;
    CSimpleIniA iniFile;
    std::vector<std::string> careBaseAttributes = {"9", "9", "9", "9", "9", "9", "9", "9", "9", "9"};
    std::vector<std::string> careRaceAttributes = {"0", "0", "0", "0", "0", "0", "0", "0", "0", "0"};
    std::vector<std::string> careMaleAttributes = {"1", "1", "0", "0", "1", "0", "0", "1", "0", "1"};
    std::vector<std::string> careFemaleAttributes = {"0", "0", "1", "1", "0", "1", "1", "0", "1", "0"};
    
    
    

    std::vector<std::string> split(const std::string &str, char delim) {
        std::vector<std::string> tokens;
        std::stringstream ss(str);
        std::string item;
        while (std::getline(ss, item, delim)) {
            tokens.push_back(item);
        };
        return tokens;
    };

    const char *floatToChar(float f) {
        static char buffer[32];
        snprintf(buffer, sizeof(buffer), "%.2f", f);
        return buffer;
    }

    const char *get_careRaceSettings(const char *szFormEditorID) {
        return iniFile.GetValue("RaceSettings", szFormEditorID, "0, 0, 0, 0, 0, 0, 0, 0, 0, 0");
    };

    const char *get_careSkillAttributes(RE::ActorValue skillName) {
        return iniFile.GetValue("SkillSettings", get_ActorValueFullName(skillName), "Luck, Luck, Luck");
    };

    const char *get_ActorValueFullName(RE::ActorValue actorValue) {
        auto &actorValueFullName = RE::ActorValueList::GetSingleton()->GetActorValue(actorValue)->fullName;
        return actorValueFullName.c_str();
    };

    void SetupLog() {
        auto logsFolder = SKSE::log::log_directory();
        if (!logsFolder) SKSE::stl::report_and_fail("SKSE log_directory not provided, logs disabled.");
        auto pluginName = SKSE::PluginDeclaration::GetSingleton()->GetName();
        auto logFilePath = *logsFolder / std::format("{}.log", pluginName);
        auto fileLoggerPtr = std::make_shared<spdlog::sinks::basic_file_sink_mt>(logFilePath.string(), true);
        auto loggerPtr = std::make_shared<spdlog::logger>("log", std::move(fileLoggerPtr));
        spdlog::set_default_logger(std::move(loggerPtr));
        spdlog::set_level(spdlog::level::trace);
        spdlog::flush_on(spdlog::level::trace);
    };


    void LoadSettings() {
        iniFile.LoadFile(L"Data/SKSE/Plugins/CARE.ini");
        const char *careBaseSettings = iniFile.GetValue("BaseSettings", "BaseAttributes");
        if (careBaseSettings) {
            careBaseAttributes = split(careBaseSettings, ',');
        }
        const char *careMaleSettings = iniFile.GetValue("SexSettings", "Male");
        if (careMaleSettings) {
            careMaleAttributes = split(careMaleSettings, ',');
        }
        const char *careFemaleSettings = iniFile.GetValue("SexSettings", "Female");
        if (careFemaleSettings) {
            careFemaleAttributes = split(careFemaleSettings, ',');
        }
        if (iniFile.GetValue("BaseSettings", "SkillThreshold")) {
            careSkillTreshold = std::atoi(iniFile.GetValue("BaseSettings", "SkillThreshold"));
        }
        if (iniFile.GetValue("BaseSettings", "MaximumAttributeLevel")) {
            careMaximumAttributeLevel = std::atoi(iniFile.GetValue("BaseSettings", "MaximumAttributeLevel"));
        }
        if (iniFile.GetValue("BaseSettings", "PopUpEnabled")) {
            carePopUpEnabled = std::atoi(iniFile.GetValue("BaseSettings", "PopUpEnabled"));
        }
        if (iniFile.GetValue("BaseSettings", "ExplanationText")) {
            careExplanationText = iniFile.GetValue("BaseSettings", "ExplanationText");
        }
        
        // Get reward values from ini file
        baseSprintStaminaDrain = std::stof(iniFile.GetValue("GameSettings", "skyrimSprintStaminaDrain"));
        baseFavorGoldRewardSmall = std::stof(iniFile.GetValue("GameSettings", "skyrimFavorGoldRewardSmall"));
        baseFavorRewardBribe = std::stof(iniFile.GetValue("GameSettings", "skyrimFavorRewardBribe"));
        baseFavorRewardIntimidate = std::stof(iniFile.GetValue("GameSettings", "skyrimFavorRewardIntimidate"));
        baseFavorRewardLarge = std::stof(iniFile.GetValue("GameSettings", "skyrimFavorRewardLarge"));
        baseFavorRewardMedium = std::stof(iniFile.GetValue("GameSettings", "skyrimFavorRewardMedium"));
        baseFavorRewardSmall = std::stof(iniFile.GetValue("GameSettings", "skyrimFavorRewardSmall"));
        baseMGR11Reward01 = std::stof(iniFile.GetValue("GameSettings", "skyrimMGR11Reward01"));
        baseMGR11Reward02 = std::stof(iniFile.GetValue("GameSettings", "skyrimMGR11Reward02"));
        baseMGR11Reward03 = std::stof(iniFile.GetValue("GameSettings", "skyrimMGR11Reward03"));
        baseMGR11Reward04 = std::stof(iniFile.GetValue("GameSettings", "skyrimMGR11Reward04"));
        baseMGR11Reward05 = std::stof(iniFile.GetValue("GameSettings", "skyrimMGR11Reward05"));
        baseQuestReward01Small = std::stof(iniFile.GetValue("GameSettings", "skyrimQuestReward01Small"));
        baseQuestReward02Medium = std::stof(iniFile.GetValue("GameSettings", "skyrimQuestReward02Medium"));
        baseQuestReward03Large = std::stof(iniFile.GetValue("GameSettings", "skyrimQuestReward03Large"));
        baseQuestReward04Wow = std::stof(iniFile.GetValue("GameSettings", "skyrimQuestReward04Wow"));
    };

    void LoadForms() {
        iniFile.LoadFile(L"Data/SKSE/Plugins/CARE.ini");
        if (const auto dataHandler = RE::TESDataHandler::GetSingleton()) {
            careStr = dataHandler->LookupForm<RE::TESGlobal>(0x800, "Core Attribute Remaster & Expansion.esp"sv);
            careFor = dataHandler->LookupForm<RE::TESGlobal>(0x801, "Core Attribute Remaster & Expansion.esp"sv);
            careEnd = dataHandler->LookupForm<RE::TESGlobal>(0x802, "Core Attribute Remaster & Expansion.esp"sv);
            careInt = dataHandler->LookupForm<RE::TESGlobal>(0x803, "Core Attribute Remaster & Expansion.esp"sv);
            careWis = dataHandler->LookupForm<RE::TESGlobal>(0x804, "Core Attribute Remaster & Expansion.esp"sv);
            carePer = dataHandler->LookupForm<RE::TESGlobal>(0x805, "Core Attribute Remaster & Expansion.esp"sv);
            careAgi = dataHandler->LookupForm<RE::TESGlobal>(0x806, "Core Attribute Remaster & Expansion.esp"sv);
            careDex = dataHandler->LookupForm<RE::TESGlobal>(0x807, "Core Attribute Remaster & Expansion.esp"sv);
            careCha = dataHandler->LookupForm<RE::TESGlobal>(0x808, "Core Attribute Remaster & Expansion.esp"sv);
            careLuk = dataHandler->LookupForm<RE::TESGlobal>(0x809, "Core Attribute Remaster & Expansion.esp"sv);
            carePERK = dataHandler->LookupForm<RE::BGSPerk>(0x80A, "Core Attribute Remaster & Expansion.esp"sv);
            careSPEL = dataHandler->LookupForm<RE::SpellItem>(0x80B, "Core Attribute Remaster & Expansion.esp"sv);
            careLukPerkChance = dataHandler->LookupForm<RE::TESGlobal>(0x818, "Core Attribute Remaster & Expansion.esp"sv);

            // Get reward values
            skyrimFavorGoldRewardSmall = dataHandler->LookupForm<RE::TESGlobal>(0x057F01, "Skyrim.esm");
            skyrimFavorRewardBribe =dataHandler->LookupForm<RE::TESGlobal>(0x0510C7, "Skyrim.esm");
            skyrimFavorRewardIntimidate = dataHandler->LookupForm<RE::TESGlobal>(0x0510C8, "Skyrim.esm");
            skyrimFavorRewardLarge = dataHandler->LookupForm<RE::TESGlobal>(0x035217, "Skyrim.esm");
            skyrimFavorRewardMedium = dataHandler->LookupForm<RE::TESGlobal>(0x035216, "Skyrim.esm");
            skyrimFavorRewardSmall = dataHandler->LookupForm<RE::TESGlobal>(0x035215, "Skyrim.esm");
            skyrimMGR11Reward01 = dataHandler->LookupForm<RE::TESGlobal>(0x01A4A8, "Skyrim.esm");
            skyrimMGR11Reward02 = dataHandler->LookupForm<RE::TESGlobal>(0x01AD77, "Skyrim.esm");
            skyrimMGR11Reward03 = dataHandler->LookupForm<RE::TESGlobal>(0x01B567, "Skyrim.esm");
            skyrimMGR11Reward04 = dataHandler->LookupForm<RE::TESGlobal>(0x01B723, "Skyrim.esm");
            skyrimMGR11Reward05 = dataHandler->LookupForm<RE::TESGlobal>(0x01B724, "Skyrim.esm");
            skyrimQuestReward01Small = dataHandler->LookupForm<RE::TESGlobal>(0x0BCD0B, "Skyrim.esm");
            skyrimQuestReward02Medium = dataHandler->LookupForm<RE::TESGlobal>(0x0BCD0D, "Skyrim.esm");
            skyrimQuestReward03Large = dataHandler->LookupForm<RE::TESGlobal>(0x0BCD0F, "Skyrim.esm");
            skyrimQuestReward04Wow = dataHandler->LookupForm<RE::TESGlobal>(0x0BCD0C, "Skyrim.esm");

            // Set reward values for later retrival, as the original ones will be modified
            iniFile.SetValue("GameSettings", "skyrimFavorGoldRewardSmall", floatToChar(skyrimFavorGoldRewardSmall->value));
            iniFile.SetValue("GameSettings", "skyrimFavorRewardBribe", floatToChar(skyrimFavorRewardBribe->value));
            iniFile.SetValue("GameSettings", "skyrimFavorRewardIntimidate", floatToChar(skyrimFavorRewardIntimidate->value));
            iniFile.SetValue("GameSettings", "skyrimFavorRewardLarge", floatToChar(skyrimFavorRewardLarge->value));
            iniFile.SetValue("GameSettings", "skyrimFavorRewardMedium", floatToChar(skyrimFavorRewardMedium->value));
            iniFile.SetValue("GameSettings", "skyrimFavorRewardSmall", floatToChar(skyrimFavorRewardSmall->value));
            iniFile.SetValue("GameSettings", "skyrimMGR11Reward01", floatToChar(skyrimMGR11Reward01->value));
            iniFile.SetValue("GameSettings", "skyrimMGR11Reward02", floatToChar(skyrimMGR11Reward02->value));
            iniFile.SetValue("GameSettings", "skyrimMGR11Reward03", floatToChar(skyrimMGR11Reward03->value));
            iniFile.SetValue("GameSettings", "skyrimMGR11Reward04", floatToChar(skyrimMGR11Reward04->value));
            iniFile.SetValue("GameSettings", "skyrimMGR11Reward05", floatToChar(skyrimMGR11Reward05->value));
            iniFile.SetValue("GameSettings", "skyrimQuestReward01Small", floatToChar(skyrimQuestReward01Small->value));
            iniFile.SetValue("GameSettings", "skyrimQuestReward02Medium", floatToChar(skyrimQuestReward02Medium->value));
            iniFile.SetValue("GameSettings", "skyrimQuestReward03Large", floatToChar(skyrimQuestReward03Large->value));
            iniFile.SetValue("GameSettings", "skyrimQuestReward04Wow", floatToChar(skyrimQuestReward04Wow->value));
        };
        if (auto settingCollection = RE::GameSettingCollection::GetSingleton()) {
            skyrimSprintStaminaDrain = settingCollection->GetSetting("fSprintStaminaDrainMult");
            iniFile.SetValue("GameSettings", "skyrimSprintStaminaDrain", floatToChar(skyrimSprintStaminaDrain->data.f));
        };
        iniFile.SaveFile(L"Data/SKSE/Plugins/CARE.ini");
    };

    void SetupAttributes() {
        std::vector<std::string> careSexAttributes;
        auto *player = RE::PlayerCharacter::GetSingleton();
        if (!player) {
            return;
        }
        bool isMale = player->GetActorBase()->GetSex() == RE::SEX::kMale;
        if (isMale) {
            careSexAttributes = careMaleAttributes;
        } else {
            careSexAttributes = careFemaleAttributes;
        }
        
        const char *careRaceSettings = get_careRaceSettings(player->GetRace()->GetFormEditorID());
        if (careRaceSettings) {
            careRaceAttributes = split(careRaceSettings, ',');
        }

        careStr->value = std::stof(careBaseAttributes[0]) + std::stof(careRaceAttributes[0]) + std::stof(careSexAttributes[0]);
        careFor->value = std::stof(careBaseAttributes[1]) + std::stof(careRaceAttributes[1]) + std::stof(careSexAttributes[1]);
        careEnd->value = std::stof(careBaseAttributes[2]) + std::stof(careRaceAttributes[2]) + std::stof(careSexAttributes[2]);
        careInt->value = std::stof(careBaseAttributes[3]) + std::stof(careRaceAttributes[3]) + std::stof(careSexAttributes[3]);
        careWis->value = std::stof(careBaseAttributes[4]) + std::stof(careRaceAttributes[4]) + std::stof(careSexAttributes[4]);
        carePer->value = std::stof(careBaseAttributes[5]) + std::stof(careRaceAttributes[5]) + std::stof(careSexAttributes[5]);
        careAgi->value = std::stof(careBaseAttributes[6]) + std::stof(careRaceAttributes[6]) + std::stof(careSexAttributes[6]);
        careDex->value = std::stof(careBaseAttributes[7]) + std::stof(careRaceAttributes[7]) + std::stof(careSexAttributes[7]);
        careCha->value = std::stof(careBaseAttributes[8]) + std::stof(careRaceAttributes[8]) + std::stof(careSexAttributes[8]);
        careLuk->value = std::stof(careBaseAttributes[9]) + std::stof(careRaceAttributes[9]) + std::stof(careSexAttributes[9]);

        player->AddPerk(carePERK);
        
        UpdateAttributes();
    };

    void UpdateAttributes(int playerLevel = 0) {
        auto *player = RE::PlayerCharacter::GetSingleton();
        if (!player) {
            return;
        }
        player->RemoveSpell(careSPEL);
        if (playerLevel == 0) {
            playerLevel = player->GetLevel();
        }
        float careForOffset = (careFor->value - 10.0f) / 2.0f;
        float careEndOffset = (careFor->value - 10.0f) / 2.0f;
        float careIntOffset = (careInt->value - 10.0f) / 2.0f;
        float careAgiOffset = (careAgi->value - 10.0f) / 2.0f;


        // Strength
        // Melee Damage is via Perk
        // Power Attack Cost is via Perk
        careSPEL->effects[0]->effectItem.magnitude = (careStr->value - 10.0f); // Modify Unarmed

        // Fortitude
        careSPEL->effects[1]->effectItem.magnitude = (std::max(0.0f, careForOffset) * playerLevel); // Fortify HP
        careSPEL->effects[2]->effectItem.magnitude = ((careFor->value - 10.0f) * 5.0f); // Modify Disease Resist
        careSPEL->effects[3]->effectItem.magnitude = ((careFor->value - 10.0f) * 5.0f); // Modify Poison Resist
        // Physical Resistance is via Perk

        // Endurance
        careSPEL->effects[4]->effectItem.magnitude = ((careEnd->value - 10.0f) * 5.0f); // Modify Health Regeneration
        careSPEL->effects[5]->effectItem.magnitude = (std::max(0.0f, careEndOffset) * playerLevel); // Fortify Carry Weight
        skyrimSprintStaminaDrain->data.f = (baseSprintStaminaDrain * ((careEnd->value * -5.0f) + 150) / 100.0f); // Modify Sprint Stamina Cost


        // Intelligence
        careSPEL->effects[6]->effectItem.magnitude = (std::max(0.0f, careIntOffset) * playerLevel); // Fortify Magicka
        // Spell Duration is via Perk
        // Spell Magnitude is via Perk
        
        // Wisdom
        careSPEL->effects[7]->effectItem.magnitude = ((careWis->value - 10.0f) * 5.0f); // Modify Magicka Regeneration
        careSPEL->effects[8]->effectItem.magnitude = ((careWis->value - 10.0f) * 5.0f); // Modify Magic Resist
        // Spell Cost is via Perk

        // Perception
        // Critical Damage is via Perk
        // Evade Attack Chance is via Perk
        // Power Attack Damage is via Perk


        // Agility
        careSPEL->effects[9]->effectItem.magnitude = (std::max(0.0f, careAgiOffset) * playerLevel);  // Fortify Stamina
        // Sneak Attack is via Perk
        careSPEL->effects[10]->effectItem.magnitude = ((careAgi->value - 10.0f) * 3.0f); // Modify Speed

        // Dexterity
        careSPEL->effects[11]->effectItem.magnitude = ((careDex->value - 10.0f) * 5.0f);  // Modify Stamina Regeneration
        // Ranged Damage is via Perk
        // Lockpicking is via Perk

        // Charisma
        // Bribe Cost is via Perk
        // Intimidate Chance Mult is via Perk
        // Sell Price is via Perk

        // Luck
        // Crit Chance is via Perk
        // Extra Perk is via function
        skyrimFavorGoldRewardSmall->value = (baseFavorGoldRewardSmall * ((((careLuk->value - 10.0f) * 5.0f) / 100.0f) + 1.0f));
        skyrimFavorRewardBribe->value = (baseFavorRewardBribe * ((((careLuk->value - 10.0f) * 5.0f) / 100.0f) + 1.0f));
        skyrimFavorRewardIntimidate->value = (baseFavorRewardIntimidate * ((((careLuk->value - 10.0f) * 5.0f) / 100.0f) + 1.0f));
        skyrimFavorRewardLarge->value = (baseFavorRewardLarge * ((((careLuk->value - 10.0f) * 5.0f) / 100.0f) + 1.0f));
        skyrimFavorRewardMedium->value = (baseFavorRewardMedium * ((((careLuk->value - 10.0f) * 5.0f) / 100.0f) + 1.0f));
        skyrimFavorRewardSmall->value = (baseFavorRewardSmall * ((((careLuk->value - 10.0f) * 5.0f) / 100.0f) + 1.0f));
        skyrimMGR11Reward01->value = (baseMGR11Reward01 * ((((careLuk->value - 10.0f) * 5.0f) / 100.0f) + 1.0f));
        skyrimMGR11Reward02->value = (baseMGR11Reward02 * ((((careLuk->value - 10.0f) * 5.0f) / 100.0f) + 1.0f));
        skyrimMGR11Reward03->value = (baseMGR11Reward03 * ((((careLuk->value - 10.0f) * 5.0f) / 100.0f) + 1.0f));
        skyrimMGR11Reward04->value = (baseMGR11Reward04 * ((((careLuk->value - 10.0f) * 5.0f) / 100.0f) + 1.0f));
        skyrimMGR11Reward05->value = (baseMGR11Reward05 * ((((careLuk->value - 10.0f) * 5.0f) / 100.0f) + 1.0f));
        skyrimQuestReward01Small->value = (baseQuestReward01Small * ((((careLuk->value - 10.0f) * 5.0f) / 100.0f) + 1.0f));
        skyrimQuestReward02Medium->value = (baseQuestReward02Medium * ((((careLuk->value - 10.0f) * 5.0f) / 100.0f) + 1.0f));
        skyrimQuestReward03Large->value = (baseQuestReward03Large * ((((careLuk->value - 10.0f) * 5.0f) / 100.0f) + 1.0f));
        skyrimQuestReward04Wow->value = (baseQuestReward04Wow * ((((careLuk->value - 10.0f) * 5.0f) / 100.0f) + 1.0f));
        player->AddSpell(careSPEL);
    };

    void ExtraPerkRoll(){
        auto *player = RE::PlayerCharacter::GetSingleton();
        if (!player) {
            return;
        }
        std::random_device dev;
        std::mt19937 rng(dev());
        std::uniform_int_distribution<std::mt19937::result_type> careLukPerkChanceRoll(1, 100);
        if (careLukPerkChance->value >= careLukPerkChanceRoll(rng)) {
            player->GetGameStatsData().perkCount += 1;
            RE::DebugNotification("You are so lucky, you have received an extra Perk Point!");
            careLukPerkChance->value = ((careLuk->value - 10.0f) * 1.5f);
        } else {
            careLukPerkChance->value += ((careLuk->value - 10.0f) * 1.5f);
        }
    };

    class MessageBoxCallback : public RE::IMessageBoxCallback {
    public:
        MessageBoxCallback(std::function<void(unsigned int)> callback) : callback{callback} {}
        ~MessageBoxCallback() override {}
        void Run(RE::IMessageBoxCallback::Message message) override { callback(static_cast<unsigned int>(message)); }
    private:
        std::function<void(unsigned int)> callback;
    };

    void ShowMessageBox(const std::string &a_message, std::vector<std::string> options, std::function<void(unsigned int)> callback) {
        auto factoryManager = RE::MessageDataFactoryManager::GetSingleton();
        auto uiStrHolder = RE::InterfaceStrings::GetSingleton();
        if (factoryManager && uiStrHolder) {
            auto factory = factoryManager->GetCreator<RE::MessageBoxData>(uiStrHolder->messageBoxData);
            if (factory) {
                auto messageBox = factory->Create();
                if (messageBox) {
                    messageBox->bodyText = a_message.c_str();
                    messageBox->callback = RE::make_smart<MessageBoxCallback>(callback);
                    for (std::string &text : options) {
                        messageBox->buttonText.push_back(text.c_str());
                    }
                    messageBox->QueueMessage();
                }
            }
        }
    };

    void AttributeSelect(int selection) {
        std::string selectedAttribute = skillAttributes[selection];
        if (selectedAttribute == "Strength" && careStr->value < careMaximumAttributeLevel) {
            careStr->value += 1;
            RE::DebugNotification("My Strength has increased!");
        } else if (selectedAttribute == "Fortitude" && careFor->value < careMaximumAttributeLevel) {
            careFor->value += 1;
            RE::DebugNotification("My Fortitude has increased!");
        } else if (selectedAttribute == "Endurance" && careEnd->value < careMaximumAttributeLevel) {
            careEnd->value += 1;
            RE::DebugNotification("My Endurance has increased!");
        } else if (selectedAttribute == "Intelligence" && careInt->value < careMaximumAttributeLevel) {
            careInt->value += 1;
            RE::DebugNotification("My Intelligence has increased!");
        } else if (selectedAttribute == "Wisdom" && careWis->value < careMaximumAttributeLevel) {
            careWis->value += 1;
            RE::DebugNotification("My Wisdom has increased!");
        } else if (selectedAttribute == "Perception" && carePer->value < careMaximumAttributeLevel) {
            carePer->value += 1;
            RE::DebugNotification("My Perception has increased!");
        } else if (selectedAttribute == "Agility" && careAgi->value < careMaximumAttributeLevel) {
            careAgi->value += 1;
            RE::DebugNotification("My Agility has increased!");
        } else if (selectedAttribute == "Dexterity" && careDex->value < careMaximumAttributeLevel) {
            careDex->value += 1;
            RE::DebugNotification("My Dexterity has increased!");
        } else if (selectedAttribute == "Charisma" && careCha->value < careMaximumAttributeLevel) {
            careCha->value += 1;
            RE::DebugNotification("My Charisma has increased!");
        } else if (selectedAttribute == "Luck" && careLuk->value < careMaximumAttributeLevel) {
            careLuk->value += 1;
            RE::DebugNotification("My Luck has increased!");
        } else if (selectedAttribute == "None") {
            RE::DebugNotification("No Attribute has increased!");
        } else {
            RE::DebugNotification("You cannot increase an Attribute above 20!");
            ShowMessageBox("Your skill increased! Select the Attribute to increase!", skillAttributes, [&](unsigned int result) { AttributeSelect(result); });
        }
        UpdateAttributes();
    };


    RE::BSEventNotifyControl ProcessEvent(const RE::MenuOpenCloseEvent *event, RE::BSTEventSource<RE::MenuOpenCloseEvent> *) {
        auto *player = RE::PlayerCharacter::GetSingleton();
        if (event->menuName == "RaceSex Menu" && !event->opening && player->GetLevel() == 1) {
            if (carePopUpEnabled == 1) {
                RE::DebugMessageBox(careExplanationText);
            } else {
                RE::DebugNotification("CARE initialized!");
            }
            SetupAttributes();
        };

        return RE::BSEventNotifyControl::kContinue;
    }

    RE::BSEventNotifyControl ProcessEvent(const RE::LevelIncrease::Event *event, RE::BSTEventSource<RE::LevelIncrease::Event> *) {
        UpdateAttributes(event->newLevel);
        ExtraPerkRoll();
        return RE::BSEventNotifyControl::kContinue;
    };

    RE::BSEventNotifyControl ProcessEvent(const RE::SkillIncrease::Event *event, RE::BSTEventSource<RE::SkillIncrease::Event> *) {
        auto *player = RE::PlayerCharacter::GetSingleton();
        if (player) {
            float increasedSkill = player->AsActorValueOwner()->GetBaseActorValue(event->actorValue);
            if (static_cast<int>(increasedSkill) % careSkillTreshold == 0) {
                skillAttributes = split(get_careSkillAttributes(event->actorValue), ',');
                skillAttributes.push_back(std::string("None"));
                ShowMessageBox("Your skill increased! Select the Attribute to increase!", skillAttributes, [&](unsigned int result) { AttributeSelect(result); });
            }
        }
        return RE::BSEventNotifyControl::kContinue;
    };

};


cGlobalVariables gMyModInstance;


SKSEPluginLoad(const SKSE::LoadInterface *skse) {
    SKSE::Init(skse);
    gMyModInstance.SetupLog();
        RE::LevelIncrease::GetEventSource()->AddEventSink<RE::LevelIncrease::Event>(&gMyModInstance);
    RE::SkillIncrease::GetEventSource()->AddEventSink<RE::SkillIncrease::Event>(&gMyModInstance);
    RE::UI::GetSingleton()->AddEventSink<RE::MenuOpenCloseEvent>(&gMyModInstance);
    
    SKSE::GetMessagingInterface()->RegisterListener([](SKSE::MessagingInterface::Message *message) {
        if (message->type == SKSE::MessagingInterface::kDataLoaded) {
            gMyModInstance.LoadForms();
            gMyModInstance.LoadSettings();
        }
    });
    return true;
}