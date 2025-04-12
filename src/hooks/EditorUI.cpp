#include "../SmartBPMTrigger.hpp"
#include "../classes/SBTSettingsPopup.hpp"
#include <Geode/binding/EditButtonBar.hpp>
#include <Geode/modify/EditorUI.hpp>

using namespace geode::prelude;

class $modify(SBTEditorUI, EditorUI) {
    SBT_MODIFY(EditorUI)

    void createMoveMenu() {
        EditorUI::createMoveMenu();

        auto bpmPlusButton = getSpriteButton(CCSprite::create("SBT_bpmPlus_001.png"_spr),
            menu_selector(SBTEditorUI::onBPMPlus), nullptr, 0.9f, 1, { 0.0f, -2.0f });
        bpmPlusButton->setID("bpm-plus-button"_spr);
        m_editButtonBar->m_buttonArray->addObject(bpmPlusButton);
        m_editButtonBar->reloadItems(SmartBPMTrigger::variable<"0049", int>(), SmartBPMTrigger::variable<"0050", int>());
    }

    void onBPMPlus(CCObject*) {
        SBTSettingsPopup::create(m_editorLayer)->show();
    }
};
