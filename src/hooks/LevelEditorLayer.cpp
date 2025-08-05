#include "../SmartBPMTrigger.hpp"
#include "../classes/SBTTriggerData.hpp"
#include <Geode/binding/AudioLineGuideGameObject.hpp>
#include <Geode/binding/DrawGridLayer.hpp>
#include <Geode/modify/LevelEditorLayer.hpp>
#include <Geode/utils/base64.hpp>

using namespace geode::prelude;

class $modify(SBTLevelEditorLayer, LevelEditorLayer) {
    static void onModify(ModifyBase<ModifyDerive<SBTLevelEditorLayer, LevelEditorLayer>>& self) {
        SmartBPMTrigger::modify(self);
    }

    void createObjectsFromSetup(gd::string& gdSetup) {
        std::string_view setup = gdSetup;
        std::string decodedText;

        auto textObject = setup.find(";1,914,2,-9000,3,-1590,");
        if (textObject != std::string_view::npos) {
            auto nextSemicolon = setup.find(";", textObject + 1);
            if (nextSemicolon == std::string_view::npos) nextSemicolon = setup.size();
            auto beforeNextSemicolon = std::string(setup, 0, nextSemicolon);
            auto base64Text = beforeNextSemicolon.find("31,", textObject + 1);
            if (base64Text != std::string_view::npos) {
                auto nextComma = beforeNextSemicolon.find(",", base64Text + 3);
                if (nextComma == std::string_view::npos) nextComma = nextSemicolon;
                decodedText = base64::decodeString(std::string(setup, base64Text + 3, nextComma - base64Text - 3)).unwrapOr("");
            }
        }

        LevelEditorLayer::createObjectsFromSetup(gdSetup);

        auto& audioLineObjects = m_drawGridLayer->m_audioLineObjects;
        if (audioLineObjects.empty() || decodedText.empty()) return;

        std::vector<int> keys;
        for (auto& pair : audioLineObjects) {
            keys.push_back(pair.first);
        }
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
