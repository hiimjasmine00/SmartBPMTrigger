#include "../classes/SBTTriggerData.hpp"
#include <Geode/binding/AudioLineGuideGameObject.hpp>
#include <Geode/binding/DrawGridLayer.hpp>
#include <Geode/modify/LevelEditorLayer.hpp>
#include <Geode/utils/base64.hpp>
#include <jasmine/hook.hpp>
#include <jasmine/string.hpp>

using namespace geode::prelude;

class $modify(SBTLevelEditorLayer, LevelEditorLayer) {
    static void onModify(ModifyBase<ModifyDerive<SBTLevelEditorLayer, LevelEditorLayer>>& self) {
        jasmine::hook::modify(self.m_hooks, "enabled");
    }

    void createObjectsFromSetup(gd::string& gdSetup) {
        std::string_view setup = gdSetup;
        std::string decodedText;

        auto textObject = setup.find(";1,914,2,-9000,3,-1590,");
        if (textObject != std::string_view::npos) {
            auto nextSemicolon = setup.find(';', textObject + 1);
            if (nextSemicolon == std::string_view::npos) nextSemicolon = setup.size();
            auto beforeNextSemicolon = setup.substr(0, nextSemicolon);
            auto base64Text = beforeNextSemicolon.find("31,", textObject + 1);
            if (base64Text != std::string_view::npos) {
                auto nextComma = beforeNextSemicolon.find(',', base64Text + 3);
                if (nextComma == std::string_view::npos) nextComma = nextSemicolon;
                if (auto str = base64::decodeString(setup.substr(base64Text + 3, nextComma - base64Text - 3))) {
                    decodedText = std::move(str).unwrap();
                }
            }
        }

        LevelEditorLayer::createObjectsFromSetup(gdSetup);

        auto& audioLineObjects = m_drawGridLayer->m_audioLineObjects;
        if (audioLineObjects.empty() || decodedText.empty()) return;

        std::vector<AudioLineGuideGameObject*> objects;
        for (auto& pair : audioLineObjects) {
            objects.insert(std::ranges::upper_bound(objects, pair.second, [](AudioLineGuideGameObject* a, AudioLineGuideGameObject* b) {
                return a->m_uniqueID < b->m_uniqueID;
            }), pair.second);
        }

        auto splitData = string::splitView(decodedText, ";");
        for (int i = 0; i < objects.size() && i < splitData.size(); i++) {
            auto object = objects[i];
            object->setUserObject("trigger-data"_spr, SBTTriggerData::create(splitData[i], object->m_beatsPerBar));
        }
    }

    void addSpecial(GameObject* object) {
        LevelEditorLayer::addSpecial(object);

        if (object->m_objectID == 3642 && !object->getUserObject("trigger-data"_spr)) {
            object->setUserObject("trigger-data"_spr, SBTTriggerData::create("", static_cast<AudioLineGuideGameObject*>(object)->m_beatsPerBar));
        }
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
