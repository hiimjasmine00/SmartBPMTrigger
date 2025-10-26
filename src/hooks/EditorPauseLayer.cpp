#include "../SmartBPMTrigger.hpp"
#include "../classes/SBTTriggerData.hpp"
#include <Geode/binding/AudioLineGuideGameObject.hpp>
#include <Geode/binding/DrawGridLayer.hpp>
#include <Geode/binding/LevelEditorLayer.hpp>
#include <Geode/binding/TextGameObject.hpp>
#include <Geode/modify/EditorPauseLayer.hpp>

using namespace geode::prelude;

class $modify(SBTEditorPauseLayer, EditorPauseLayer) {
    static void onModify(ModifyBase<ModifyDerive<SBTEditorPauseLayer, EditorPauseLayer>>& self) {
        SmartBPMTrigger::modify(self.m_hooks);
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

        std::vector<int> keys;
        for (auto& pair : audioLineObjects) {
            keys.push_back(pair.first);
        }
        std::ranges::sort(keys);

        #ifdef GEODE_IS_ANDROID
        std::string saveString;
        #else
        auto& saveString = saveObject->m_text;
        saveString.clear();
        #endif
        for (auto k : keys) {
            if (!saveString.empty()) saveString += ';';
            if (auto triggerData = static_cast<SBTTriggerData*>(audioLineObjects[k]->getUserObject("trigger-data"_spr))) {
                saveString += triggerData->getSaveString();
            }
        }
        GEODE_ANDROID(saveObject->m_text = saveString;)

        EditorPauseLayer::saveLevel();
    }
};
