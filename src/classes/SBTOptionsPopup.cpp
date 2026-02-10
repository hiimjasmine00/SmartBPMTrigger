#include "SBTOptionsPopup.hpp"
#include "SBTTriggerData.hpp"
#include "../SmartBPMTrigger.hpp"
#include <Geode/binding/ButtonSprite.hpp>
#include <jasmine/setting.hpp>

using namespace geode::prelude;

SBTOptionsPopup* SBTOptionsPopup::create(SBTTriggerData* triggerData) {
    auto popup = new SBTOptionsPopup();
    if (popup->init(triggerData)) {
        popup->autorelease();
        return popup;
    }
    delete popup;
    return nullptr;
}

bool SBTOptionsPopup::init(SBTTriggerData* triggerData) {
    if (!Popup::init(400.0f, 290.0f)) return false;

    setID("SBTOptionsPopup");
    setTitle("Trigger Settings");
    m_title->setID("trigger-settings-title");
    m_mainLayer->setID("main-layer");
    m_buttonMenu->setID("button-menu");
    m_bgSprite->setID("background");
    m_closeBtn->setID("close-button");
    m_noElasticity = true;

    m_triggerData = triggerData;
    m_colors = triggerData->m_colors;
    m_widths = triggerData->m_widths;
    m_index = -1;

    m_barSpriteMenu = CCMenu::create();
    m_barSpriteMenu->setContentSize({ 400.0f, 200.0f });
    m_barSpriteMenu->setPosition({ 200.0f, 150.0f });
    m_barSpriteMenu->setLayout(RowLayout::create()->setGap(30.0f)->setAutoScale(false));
    m_barSpriteMenu->setID("bar-sprite-menu");
    m_mainLayer->addChild(m_barSpriteMenu);

    auto [squareTexture, squareRect] = SmartBPMTrigger::getSquare();

    m_barSprite = CCSprite::createWithTexture(squareTexture, squareRect);
    m_barSprite->setPosition({ 90.0f, 150.0f });
    m_barSprite->setScaleX(0.0f);
    m_barSprite->setScaleY(200.0f / squareRect.size.height);
    m_barSprite->setVisible(false);
    m_barSprite->setColor({ 0, 0, 0 });
    m_barSprite->setOpacity(0);
    m_barSprite->setID("bar-sprite");
    m_mainLayer->addChild(m_barSprite);

    m_colorWidget = SBTColorWidget::create({ 0, 0, 0, 0 }, 0.0f, [this](const ccColor4B& color, float width) {
        m_barSprite->setColor({ color.r, color.g, color.b });
        m_barSprite->setOpacity(color.a);
        m_barSprite->setScaleX(width / 5.0f);
    });
    m_colorWidget->setPosition({ 305.0f, 150.0f });
    m_colorWidget->setScale(0.75f);
    m_colorWidget->setID("color-widget");

    for (int i = 0; i < triggerData->m_beats; i++) {
        auto& [r, g, b, a] = m_colors[i];
        auto barSprite = CCSprite::createWithTexture(squareTexture, squareRect);
        barSprite->setScaleX(m_widths[i] / 5.0f);
        barSprite->setScaleY(200.0f / squareRect.size.height);
        barSprite->setColor({ r, g, b });
        barSprite->setOpacity(a);

        auto barButton = CCMenuItemSpriteExtra::create(barSprite, this, menu_selector(SBTOptionsPopup::onBar));
        barSprite->setPositionX(squareRect.size.width / 2.0f);
        barButton->m_scaleMultiplier = 1.0f;
        barButton->setContentSize({ squareRect.size.width, 200.0f });
        barButton->setID(fmt::format("bar-button-{}", i + 1));
        barButton->setTag(i);
        m_barSpriteMenu->addChild(barButton);
    }

    m_barSpriteMenu->updateLayout();

    m_resetMenu = CCMenu::create();
    m_resetMenu->setPosition({ 200.0f, 250.0f });
    m_resetMenu->setContentSize({ 25.0f, 100.0f });
    m_resetMenu->setAnchorPoint({ 0.5f, 1.0f });
    m_resetMenu->setVisible(false);
    m_resetMenu->setEnabled(false);
    m_resetMenu->setLayout(ColumnLayout::create()->setGap(5.0f)->setAxisReverse(true)->setAxisAlignment(AxisAlignment::End));
    m_resetMenu->setID("reset-menu");
    m_mainLayer->addChild(m_resetMenu);

    auto softResetButtonSprite = ButtonSprite::create(
        CCSprite::createWithSpriteFrameName("geode.loader/reset-gold.png"), 32, true, 0.0f, "GJ_button_01.png", 1.25f);
    softResetButtonSprite->setScale(0.6f);
    softResetButtonSprite->setCascadeOpacityEnabled(true);
    auto softResetButton = CCMenuItemSpriteExtra::create(softResetButtonSprite, this, menu_selector(SBTOptionsPopup::onSoftReset));
    softResetButton->setID("soft-reset-button");
    m_resetMenu->addChild(softResetButton);

    auto hardResetButtonSprite = ButtonSprite::create(
        CCSprite::createWithSpriteFrameName("geode.loader/reset-gold.png"), 32, true, 0.0f, "GJ_button_02.png", 1.25f);
    hardResetButtonSprite->setScale(0.6f);
    hardResetButtonSprite->setCascadeOpacityEnabled(true);
    auto hardResetButton = CCMenuItemSpriteExtra::create(hardResetButtonSprite, this, menu_selector(SBTOptionsPopup::onHardReset));
    hardResetButton->setID("hard-reset-button");
    m_resetMenu->addChild(hardResetButton);

    m_resetMenu->setOpacity(0);
    m_resetMenu->updateLayout();

    auto saveButton = CCMenuItemSpriteExtra::create(ButtonSprite::create("Save", 0.8f), this, menu_selector(SBTOptionsPopup::onSave));
    saveButton->setPosition({ 245.0f, 25.0f });
    saveButton->setID("save-button");
    m_buttonMenu->addChild(saveButton);

    auto resetButton = CCMenuItemSpriteExtra::create(ButtonSprite::create("Reset", 0.8f), this, menu_selector(SBTOptionsPopup::onReset));
    resetButton->setPosition({ 160.0f, 25.0f });
    resetButton->setID("reset-button");
    m_buttonMenu->addChild(resetButton);

    return true;
}

void SBTOptionsPopup::onBar(CCObject* sender) {
    auto tag = sender->getTag();
    auto node = static_cast<CCNode*>(sender);
    m_index = tag;
    m_barSprite->setPositionX(m_mainLayer->convertToNodeSpace(m_barSpriteMenu->convertToWorldSpace(node->getPosition())).x);
    m_barSprite->setVisible(true);
    node->setVisible(false);
    m_colorWidget->setValues(m_colors[tag], m_widths[tag]);
    m_mainLayer->addChild(m_colorWidget);
    auto normalImage = static_cast<CCSprite*>(m_closeBtn->getNormalImage());
    normalImage->setDisplayFrame(SmartBPMTrigger::getSpriteFrameCache()->spriteFrameByName("GJ_backBtn_001.png"));
    normalImage->setScale(0.74f);
    m_barSprite->stopAllActions();
    m_barSprite->runAction(CCMoveTo::create(0.2f, { 90.0f, 150.0f }));
    m_barSpriteMenu->stopAllActions();
    m_barSpriteMenu->setEnabled(false);
    m_barSpriteMenu->runAction(CCSequence::createWithTwoActions(CCFadeOut::create(0.2f), CCHide::create()));
    m_resetMenu->stopAllActions();
    m_resetMenu->setVisible(true);
    m_resetMenu->runAction(CCSequence::createWithTwoActions(CCFadeIn::create(0.2f), CallFuncExt::create([this] {
        m_resetMenu->setEnabled(true);
    })));
    m_colorWidget->prepareActions(true);
}

void SBTOptionsPopup::onSoftReset(CCObject* sender) {
    if (m_index < 0) return;
    m_queuedColor = m_colors[m_index];
    m_queuedWidth = m_widths[m_index];
    if (m_colorWidget->getColor() != m_queuedColor || m_colorWidget->getWidth() != m_queuedWidth) {
        auto alert = FLAlertLayer::create(
            this,
            "Soft Reset",
            "Are you sure you want to <cy>soft reset</c> this line's <cg>color</c> and <cl>width</c>?",
            "No",
            "Yes",
            350.0f
        );
        alert->setTag(2);
        alert->show();
    }
}

void SBTOptionsPopup::onHardReset(CCObject* sender) {
    if (m_index < 0) return;
    m_queuedColor = m_triggerData->m_colors[m_index];
    m_queuedWidth = m_triggerData->m_widths[m_index];
    if (m_colorWidget->getColor() != m_queuedColor || m_colorWidget->getWidth() != m_queuedWidth) {
        auto alert = FLAlertLayer::create(
            this,
            "Hard Reset",
            "Are you sure you want to <cr>hard reset</c> this line's <cg>color</c> and <cl>width</c>?",
            "No",
            "Yes",
            350.0f
        );
        alert->setTag(2);
        alert->show();
    }
}

void SBTOptionsPopup::onSave(CCObject* sender) {
    if (m_index >= 0) {
        auto& oldColor = m_colors[m_index];
        auto& oldWidth = m_widths[m_index];
        if (m_colorWidget->getColor() != oldColor || m_colorWidget->getWidth() != oldWidth) {
            auto barSprite = static_cast<CCSprite*>(static_cast<CCMenuItemSprite*>(m_barSpriteMenu->getChildByTag(m_index))->getNormalImage());
            auto& color = m_colorWidget->getColor();
            oldColor = color;
            barSprite->setColor({ color.r, color.g, color.b });
            barSprite->setOpacity(color.a);
            auto width = m_colorWidget->getWidth();
            oldWidth = width;
            barSprite->setScaleX(width / 5.0f);
        }
        closeBar();
    }
    else {
        auto changed = m_triggerData->m_colors != m_colors || m_triggerData->m_widths != m_widths;
        m_triggerData->m_colors = m_colors;
        m_triggerData->m_widths = m_widths;
        if (!m_triggerData->m_changed) m_triggerData->m_changed = changed;
        Popup::onClose(nullptr);
    }
}

void SBTOptionsPopup::onReset(CCObject* sender) {
    auto bpmColor = jasmine::setting::getValue<ccColor4B>("beats-per-minute-color");
    auto bpbColor = jasmine::setting::getValue<ccColor4B>("beats-per-bar-color");
    auto bpmWidth = jasmine::setting::getValue<float>("beats-per-minute-width");
    auto bpbWidth = jasmine::setting::getValue<float>("beats-per-bar-width");
    if (m_index >= 0) {
        m_queuedColor = m_index == 0 ? bpmColor : bpbColor;
        m_queuedWidth = m_index == 0 ? bpmWidth : bpbWidth;
        if (m_colorWidget->getColor() != m_queuedColor || m_colorWidget->getWidth() != m_queuedWidth) {
            auto alert = FLAlertLayer::create(
                this,
                "Reset",
                "Are you sure you want to <cr>reset</c> this line's <cg>color</c> and <cl>width</c>?",
                "No",
                "Yes",
                350.0f
            );
            alert->setTag(2);
            alert->show();
        }
    }
    else {
        m_queuedColors.reserve(m_triggerData->m_beats);
        m_queuedWidths.reserve(m_triggerData->m_beats);
        for (int i = 0; i < m_triggerData->m_beats; i++) {
            m_queuedColors.push_back(i == 0 ? bpmColor : bpbColor);
            m_queuedWidths.push_back(i == 0 ? bpmWidth : bpbWidth);
        }
        if (m_queuedColors != m_triggerData->m_colors || m_queuedWidths != m_triggerData->m_widths) {
            auto alert = FLAlertLayer::create(
                this,
                "Reset",
                "Are you sure you want to <cr>reset</c> all lines' <cg>color</c> and <cl>width</c>?",
                "No",
                "Yes",
                350.0f
            );
            alert->setTag(3);
            alert->show();
        }
        else {
            m_queuedColors.clear();
            m_queuedWidths.clear();
        }
    }
}

void SBTOptionsPopup::onClose(CCObject* sender) {
    if (m_index >= 0) {
        if (m_colorWidget->getColor() != m_colors[m_index] || m_colorWidget->getWidth() != m_widths[m_index]) {
            auto alert = FLAlertLayer::create(
                this,
                "Close Bar Editor",
                "You have <cy>unsaved changes</c>. Are you sure you want to <cr>close without saving</c>?",
                "No",
                "Yes",
                350.0f
            );
            alert->setTag(0);
            alert->show();
        }
        else closeBar();
    }
    else if (m_triggerData->m_colors != m_colors || m_triggerData->m_widths != m_widths) {
        auto alert = FLAlertLayer::create(
            this,
            "Close Popup",
            "You have <cy>unsaved changes</c>. Are you sure you want to <cr>close without saving</c>?",
            "No",
            "Yes",
            350.0f
        );
        alert->setTag(1);
        alert->show();
    }
    else Popup::onClose(nullptr);
}

void SBTOptionsPopup::FLAlert_Clicked(FLAlertLayer* layer, bool btn2) {
    if (!btn2) return;

    switch (layer->getTag()) {
        case 0:
            closeBar();
            break;
        case 1:
            Popup::onClose(nullptr);
            break;
        case 2:
            m_colorWidget->setValues(m_queuedColor, m_queuedWidth);
            break;
        case 3:
            m_triggerData->m_colors = std::move(m_queuedColors);
            m_triggerData->m_widths = std::move(m_queuedWidths);
            m_triggerData->m_changed = false;
            Popup::onClose(nullptr);
            break;
    }
}

void SBTOptionsPopup::closeBar() {
    auto barButton = static_cast<CCMenuItemSprite*>(m_barSpriteMenu->getChildByTag(m_index));
    auto barSprite = static_cast<CCNodeRGBA*>(barButton->getNormalImage());
    auto normalImage = static_cast<CCSprite*>(m_closeBtn->getNormalImage());
    normalImage->setDisplayFrame(SmartBPMTrigger::getSpriteFrameCache()->spriteFrameByName("GJ_closeBtn_001.png"));
    normalImage->setScale(0.8f);
    m_barSprite->stopAllActions();
    auto scaleX = barSprite->getScaleX();
    if (m_barSprite->getScaleX() != scaleX) {
        m_barSprite->runAction(CCScaleTo::create(0.2f, scaleX, barSprite->getScaleY()));
    }
    auto& color = barSprite->getColor();
    if (m_barSprite->getColor() != color) {
        m_barSprite->runAction(CCTintTo::create(0.2f, color.r, color.g, color.b));
    }
    auto opacity = barSprite->getOpacity();
    if (m_barSprite->getOpacity() != opacity) {
        m_barSprite->runAction(CCFadeTo::create(0.2f, opacity));
    }
    m_barSprite->runAction(CCSequence::createWithTwoActions(CCMoveTo::create(0.2f, {
        m_mainLayer->convertToNodeSpace(m_barSpriteMenu->convertToWorldSpace(barButton->getPosition())).x,
        150.0f
    }), CCHide::create()));
    barButton->stopAllActions();
    barButton->runAction(CCSequence::createWithTwoActions(CCDelayTime::create(0.2f), CCShow::create()));
    m_barSpriteMenu->stopAllActions();
    m_barSpriteMenu->setVisible(true);
    m_barSpriteMenu->runAction(CCSequence::createWithTwoActions(CCFadeIn::create(0.2f), CallFuncExt::create([this] {
        m_barSpriteMenu->setEnabled(true);
    })));
    m_resetMenu->stopAllActions();
    m_resetMenu->setEnabled(false);
    m_resetMenu->runAction(CCSequence::createWithTwoActions(CCFadeOut::create(0.2f), CCHide::create()));
    m_colorWidget->prepareActions(false);
    m_index = -1;
}
