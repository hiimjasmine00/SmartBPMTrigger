#include "../SmartBPMTrigger.hpp"
#include <Geode/binding/ButtonSprite.hpp>
#include <Geode/binding/EditButtonBar.hpp>
#include <Geode/binding/GameObject.hpp>
#include <Geode/binding/LevelEditorLayer.hpp>
#include <Geode/binding/UndoObject.hpp>
#include <Geode/modify/EditorUI.hpp>
#include <Geode/utils/ranges.hpp>

using namespace geode::prelude;

class $modify(SBTEditorUI, EditorUI) {
    struct Fields {
        CCMenuItemSpriteExtra* m_snapButton;
    };

    SBT_MODIFY(EditorUI)

    void createMoveMenu() {
        EditorUI::createMoveMenu();

        auto f = m_fields.self();
        f->m_snapButton = getSpriteButton(CCSprite::create("SBT_bpmPlus_001.png"_spr),
            menu_selector(SBTEditorUI::onSnapGuideline), nullptr, 0.9f, 1, { 0.0f, -2.0f });
        f->m_snapButton->setID("snap-bpm-button"_spr);
        m_editButtonBar->m_buttonArray->addObject(f->m_snapButton);
        m_editButtonBar->reloadItems(SmartBPMTrigger::variable<"0049", int>(), SmartBPMTrigger::variable<"0050", int>());
    }

    void onSnapGuideline(CCObject*) {
        auto guidelines = SmartBPMTrigger::getGuidelines(m_editorLayer->m_drawGridLayer);
        if (guidelines.empty()) return;

        auto x = m_selectedObject ? m_selectedObject->getPositionX() : getGroupCenter(m_selectedObjects, false).x;
        auto absolutes = ranges::map<std::vector<float>>(guidelines, [x](float guideline) { return std::abs(guideline - x); });
        auto it = std::ranges::min_element(absolutes);
        if (it == absolutes.end()) return;
        auto dx = guidelines[std::distance(absolutes.begin(), it)] - x;

        if (m_selectedObject) {
            auto undoObject = UndoObject::create(m_selectedObject, UndoCommand::Transform);
            m_editorLayer->m_redoObjects->removeAllObjects();
            if (m_editorLayer->m_undoObjects->count() >= (m_editorLayer->m_increaseMaxUndoRedo ? 1000 : 200))
                m_editorLayer->m_undoObjects->removeObjectAtIndex(0);
            m_editorLayer->m_undoObjects->addObject(undoObject);
            return moveObject(m_selectedObject, { dx, 0.0f });
        }

        createUndoObject(UndoCommand::Transform, false);

        for (auto object : CCArrayExt<GameObject*>(m_selectedObjects)) {
            moveObject(object, { dx, 0.0f });
        }
    }
};
