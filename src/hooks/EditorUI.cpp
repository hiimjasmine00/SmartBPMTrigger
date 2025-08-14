#include "../SmartBPMTrigger.hpp"
#include "../classes/SBTSettingsPopup.hpp"
#include <Geode/binding/EditButtonBar.hpp>
#include <Geode/binding/GameManager.hpp>
#include <Geode/modify/EditorUI.hpp>

using namespace geode::prelude;

class $modify(SBTEditorUI, EditorUI) {
    static void onModify(ModifyBase<ModifyDerive<SBTEditorUI, EditorUI>>& self) {
        SmartBPMTrigger::modify(self.m_hooks);
    }

    void createMoveMenu() {
        EditorUI::createMoveMenu();

        auto dict = SmartBPMTrigger::getGameManager()->m_valueKeeper;
        auto bpmPlusButton = getSpriteButton(CCSprite::create("SBT_bpmPlus_001.png"_spr),
            menu_selector(SBTEditorUI::onBPMPlus), nullptr, 0.9f, 1, { 0.0f, -2.0f });
        bpmPlusButton->setID("bpm-plus-button"_spr);
        m_editButtonBar->m_buttonArray->addObject(bpmPlusButton);
        m_editButtonBar->reloadItems(dict ? dict->valueForKey("gv_0049")->intValue() : 6, dict ? dict->valueForKey("gv_0050")->intValue() : 2);
    }

    void onBPMPlus(CCObject*) {
        SBTSettingsPopup::create(m_editorLayer)->show();
    }
};
