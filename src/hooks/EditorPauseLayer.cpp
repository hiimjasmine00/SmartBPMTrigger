#include "../classes/SBTTriggerData.hpp"
#include <Geode/binding/AudioLineGuideGameObject.hpp>
#include <Geode/binding/DrawGridLayer.hpp>
#include <Geode/binding/LevelEditorLayer.hpp>
#include <Geode/binding/TextGameObject.hpp>
#include <Geode/modify/EditorPauseLayer.hpp>
#include <jasmine/hook.hpp>

using namespace geode::prelude;

class $modify(SBTEditorPauseLayer, EditorPauseLayer) {
    static void onModify(ModifyBase<ModifyDerive<SBTEditorPauseLayer, EditorPauseLayer>>& self) {
        jasmine::hook::modify(self.m_hooks, "EditorPauseLayer::saveLevel", "enabled");
    }

    void saveLevel() {
        auto& audioLineObjects = m_editorLayer->m_drawGridLayer->m_audioLineObjects;
        if (audioLineObjects.empty()) return EditorPauseLayer::saveLevel();

        auto saveObject = static_cast<TextGameObject*>(m_editorLayer->objectAtPosition({ -9000.0f, -1500.0f }));
        if (!saveObject) {
            auto currentLayer = m_editorLayer->m_currentLayer;
            m_editorLayer->m_currentLayer = -1;
            saveObject = static_cast<TextGameObject*>(m_editorLayer->createObject(914, { 0.0f, 0.0f }, true));
            m_editorLayer->m_currentLayer = currentLayer;
            saveObject->updateTextObject("", false);
            m_editorLayer->removeObjectFromSection(saveObject);
            saveObject->setPosition({ -9000.0f, -1500.0f });
            saveObject->setScale(0.01f);
            m_editorLayer->addToSection(saveObject);
        }

        std::vector<GameObject*> objects;
        for (auto& pair : audioLineObjects) {
            objects.insert(std::ranges::upper_bound(objects, pair.second, [](GameObject* a, GameObject* b) {
                return a->m_uniqueID < b->m_uniqueID;
            }), pair.second);
        }

        fmt::memory_buffer saveString;
        for (auto it = objects.begin(); it < objects.end(); ++it) {
            if (it > objects.begin()) saveString.push_back(';');
            if (auto triggerData = static_cast<SBTTriggerData*>((*it)->getUserObject("trigger-data"_spr))) {
                fmt::format_to(std::back_inserter(saveString), "{}", triggerData->getSaveString());
            }
        }
        saveObject->m_text = fmt::to_string(saveString);

        EditorPauseLayer::saveLevel();
    }
};
