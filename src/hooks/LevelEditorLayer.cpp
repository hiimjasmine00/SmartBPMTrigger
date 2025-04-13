#include "../SmartBPMTrigger.hpp"
#include "../classes/SBTTriggerData.hpp"
#include <Geode/binding/AudioLineGuideGameObject.hpp>
#include <Geode/binding/DrawGridLayer.hpp>
#include <Geode/modify/LevelEditorLayer.hpp>
#include <Geode/utils/ranges.hpp>

using namespace geode::prelude;

// https://github.com/SpaghettDev/NamedEditorGroups/blob/067f8d6841dd3cd6bc5a2950c02e231a58ac104e/src/base64/base64.cpp#L43
std::string base64URLDecode(const std::string& input) {
    std::string decoded;
    decoded.reserve((input.size() / 4) * 3);

    auto val = 0;
    auto valb = -8;

    static std::string_view chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-_";
    for (auto& c : input) {
        if (c == '=') break;

        auto i = chars.find(c);
        if (i == std::string_view::npos) return "";

        val = (val << 6) + i;
        valb += 6;

        if (valb >= 0) {
            decoded += (val >> valb) & 255;
            valb -= 8;
        }
    }

    return decoded;
}

class $modify(SBTLevelEditorLayer, LevelEditorLayer) {
    SBT_MODIFY(LevelEditorLayer)

    void createObjectsFromSetup(gd::string& setup) {
        #ifdef GEODE_IS_ANDROID
        std::string stringSetup = setup;
        #else
        auto& stringSetup = setup;
        #endif

        std::string decodedText;

        auto textObject = stringSetup.find(";1,914,2,-9000,3,-1590,");
        if (textObject != std::string::npos) {
            auto nextSemicolon = stringSetup.find(";", textObject + 1);
            if (nextSemicolon == std::string::npos) nextSemicolon = stringSetup.size();
            auto beforeNextSemicolon = stringSetup.substr(0, nextSemicolon);
            auto base64Text = beforeNextSemicolon.find("31,", textObject + 1);
            if (base64Text != std::string::npos) {
                auto nextComma = beforeNextSemicolon.find(",", base64Text + 3);
                if (nextComma == std::string::npos) nextComma = nextSemicolon;
                decodedText = base64URLDecode(stringSetup.substr(base64Text + 3, nextComma - base64Text - 3));
            }
        }

        LevelEditorLayer::createObjectsFromSetup(setup);

        auto& audioLineObjects = m_drawGridLayer->m_audioLineObjects;
        if (audioLineObjects.empty() || decodedText.empty()) return;

        auto keys = ranges::map<std::vector<int>>(audioLineObjects, [](const std::pair<int, AudioLineGuideGameObject*>& pair) {
            return pair.first;
        });
        std::ranges::sort(keys);

        auto splitData = string::split(decodedText, ";");
        for (int i = 0; i < keys.size() && i < splitData.size(); i++) {
            auto object = audioLineObjects[keys[i]];
            object->setUserObject("trigger-data"_spr, SBTTriggerData::create(splitData[i], object->m_beatsPerBar));
        }
    }

    void addSpecial(GameObject* object) {
        LevelEditorLayer::addSpecial(object);

        if (object->m_objectID == 3642 && !object->getUserObject("trigger-data"_spr))
            object->setUserObject("trigger-data"_spr, SBTTriggerData::create("", static_cast<AudioLineGuideGameObject*>(object)->m_beatsPerBar));
    }

    void removeSpecial(GameObject* object) {
        LevelEditorLayer::removeSpecial(object);

        // This hook fixes a bug where the object is not removed from the grid layer when deleted.
        // In the vanilla game, it is removed by the unique ID, which is not the same when the object is deleted by the "Undo" or "Redo" buttons.
        // This bug is very dangerous, because it can cause a crash when the game tries to access the object that has been deleted.

        if (object->m_objectID == 3642) {
            auto& audioLineObjects = m_drawGridLayer->m_audioLineObjects;
            for (auto it = audioLineObjects.begin(); it != audioLineObjects.end(); ++it) {
                if (it->second == object) {
                    audioLineObjects.erase(it);
                    break;
                }
            }
        }
    }
};
