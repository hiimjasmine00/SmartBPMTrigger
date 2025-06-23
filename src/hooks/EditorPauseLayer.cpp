#include "../SmartBPMTrigger.hpp"
#include "../classes/SBTTriggerData.hpp"
#include <Geode/binding/AudioLineGuideGameObject.hpp>
#include <Geode/binding/DrawGridLayer.hpp>
#include <Geode/binding/LevelEditorLayer.hpp>
#include <Geode/binding/TextGameObject.hpp>
#include <Geode/modify/EditorPauseLayer.hpp>
#include <Geode/utils/ranges.hpp>

using namespace geode::prelude;

class $modify(SBTEditorPauseLayer, EditorPauseLayer) {
    SBT_MODIFY

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

        auto keys = ranges::map<std::vector<int>>(audioLineObjects, [](const gd::pair<int, AudioLineGuideGameObject*>& pair) {
            return pair.first;
        });
        std::ranges::sort(keys);

        std::string saveString;
        for (auto& k : keys) {
            if (!saveString.empty()) saveString += ';';
            if (auto triggerData = static_cast<SBTTriggerData*>(audioLineObjects[k]->getUserObject("trigger-data"_spr)))
                saveString += triggerData->getSaveString();
        }
        saveObject->m_text = saveString;

        EditorPauseLayer::saveLevel();
    }
};
