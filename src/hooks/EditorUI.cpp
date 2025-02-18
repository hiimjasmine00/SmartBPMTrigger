#include "../SmartBPMTrigger.hpp"
#include <Geode/binding/ButtonSprite.hpp>
#include <Geode/binding/EditButtonBar.hpp>
#include <Geode/binding/GameManager.hpp>
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

    static void onModify(ModifyBase<ModifyDerive<SBTEditorUI, EditorUI>>& self) {
        auto createMoveMenuRes = self.getHook("EditorUI::createMoveMenu");
        if (createMoveMenuRes.isErr()) log::error("Failed to get EditorUI::createMoveMenu hook: {}", createMoveMenuRes.unwrapErr());
        auto updateButtonsRes = self.getHook("EditorUI::updateButtons");
        if (updateButtonsRes.isErr()) log::error("Failed to get EditorUI::updateButtons hook: {}", updateButtonsRes.unwrapErr());

        auto mod = Mod::get();
        auto createMoveMenuHook = createMoveMenuRes.unwrapOr(nullptr);
        if (createMoveMenuHook) createMoveMenuHook->setAutoEnable(SmartBPMTrigger::enabled(mod));
        auto updateButtonsHook = updateButtonsRes.unwrapOr(nullptr);
        if (updateButtonsHook) updateButtonsHook->setAutoEnable(SmartBPMTrigger::enabled(mod));

        listenForSettingChanges<bool>("enabled", [createMoveMenuHook, updateButtonsHook](bool value) {
            if (createMoveMenuHook) {
                auto changeRes = value ? createMoveMenuHook->enable() : createMoveMenuHook->disable();
                if (changeRes.isErr()) log::error("Failed to {} EditorUI::createMoveMenu hook: {}", value ? "enable" : "disable", changeRes.unwrapErr());
            }
            if (updateButtonsHook) {
                auto changeRes = value ? updateButtonsHook->enable() : updateButtonsHook->disable();
                if (changeRes.isErr()) log::error("Failed to {} EditorUI::updateButtons hook: {}", value ? "enable" : "disable", changeRes.unwrapErr());
            }
        });
    }

    void createMoveMenu() {
        EditorUI::createMoveMenu();

        auto f = m_fields.self();
        f->m_snapButton = getSpriteButton(CCSprite::create("SBT_snapBPM_001.png"_spr), menu_selector(SBTEditorUI::onSnapGuideline), nullptr, 0.9f, 1, { 0.0f, -2.0f });
        f->m_snapButton->setID("snap-bpm-button"_spr);
        m_editButtonBar->m_buttonArray->addObject(f->m_snapButton);
        auto valueKeeper = SmartBPMTrigger::GAME_MANAGER->m_valueKeeper;
        m_editButtonBar->reloadItems(valueKeeper->valueForKey("gv_0049")->intValue(), valueKeeper->valueForKey("gv_0050")->intValue());
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
            if (m_editorLayer->m_undoObjects->count() >= (m_editorLayer->m_increaseMaxUndoRedo ? 1000 : 200)) m_editorLayer->m_undoObjects->removeObjectAtIndex(0);
            m_editorLayer->m_undoObjects->addObject(undoObject);
            moveObject(m_selectedObject, { dx, 0.0f });
            return;
        }

        createUndoObject(UndoCommand::Transform, false);

        for (auto object : CCArrayExt<GameObject*>(m_selectedObjects)) {
            moveObject(object, { dx, 0.0f });
        }
    }

    void updateButtons() {
        EditorUI::updateButtons();

        auto f = m_fields.self();
        if (!f->m_snapButton) return;
        
        auto enabled = m_selectedObject != nullptr || (m_selectedObjects && m_selectedObjects->count() > 0);
        f->m_snapButton->setEnabled(enabled);
        static_cast<ButtonSprite*>(f->m_snapButton->getNormalImage())->setColor(enabled ? ccColor3B { 255, 255, 255 } : ccColor3B { 166, 166, 166 });
    }
};
