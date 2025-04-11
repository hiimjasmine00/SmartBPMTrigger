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

    static void onModify(ModifyBase<ModifyDerive<SBTEditorUI, EditorUI>>& self) {
        auto mod = Mod::get();
        auto enabled = SmartBPMTrigger::enabled(mod);

        auto createMoveMenuHook = self.getHook("EditorUI::createMoveMenu").map([enabled](Hook* hook) {
            return hook->setAutoEnable(enabled), hook;
        }).mapErr([](const std::string& err) {
            return log::error("Failed to get EditorUI::createMoveMenu hook: {}", err), err;
        }).unwrapOr(nullptr);

        auto updateButtonsHook = self.getHook("EditorUI::updateButtons").map([enabled](Hook* hook) {
            return hook->setAutoEnable(enabled), hook;
        }).mapErr([](const std::string& err) {
            return log::error("Failed to get EditorUI::updateButtons hook: {}", err), err;
        }).unwrapOr(nullptr);

        SmartBPMTrigger::settingListener<"enabled", bool>([createMoveMenuHook, updateButtonsHook](bool value) {
            if (createMoveMenuHook) (void)(value ? createMoveMenuHook->enable().mapErr([](const std::string& err) {
                return log::error("Failed to enable EditorUI::createMoveMenu hook: {}", err), err;
            }) : createMoveMenuHook->disable().mapErr([](const std::string& err) {
                return log::error("Failed to disable EditorUI::createMoveMenu hook: {}", err), err;
            }));
            if (updateButtonsHook) (void)(value ? updateButtonsHook->enable().mapErr([](const std::string& err) {
                return log::error("Failed to enable EditorUI::updateButtons hook: {}", err), err;
            }) : updateButtonsHook->disable().mapErr([](const std::string& err) {
                return log::error("Failed to disable EditorUI::updateButtons hook: {}", err), err;
            }));
        }, mod);
    }

    void createMoveMenu() {
        EditorUI::createMoveMenu();

        auto f = m_fields.self();
        f->m_snapButton = getSpriteButton(CCSprite::create("SBT_snapBPM_001.png"_spr),
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

    void updateButtons() {
        EditorUI::updateButtons();

        auto f = m_fields.self();
        if (!f->m_snapButton) return;

        auto enabled = m_selectedObject != nullptr || (m_selectedObjects && m_selectedObjects->count() > 0);
        f->m_snapButton->setEnabled(enabled);
        static_cast<ButtonSprite*>(f->m_snapButton->getNormalImage())->setColor(enabled ? ccColor3B { 255, 255, 255 } : ccColor3B { 166, 166, 166 });
    }
};
