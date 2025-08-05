#include "SBTOptionsPopup.hpp"
#include "../SmartBPMTrigger.hpp"
#include <Geode/binding/ButtonSprite.hpp>
#include <Geode/binding/ColorAction.hpp>
#include <Geode/binding/ColorSelectPopup.hpp>

using namespace geode::prelude;

SBTOptionsPopup* SBTOptionsPopup::create(SBTTriggerData* triggerData) {
    auto popup = new SBTOptionsPopup();
    if (popup->initAnchored(250.0f, 170.0f, triggerData)) {
        popup->autorelease();
        return popup;
    }
    delete popup;
    return nullptr;
}

bool SBTOptionsPopup::setup(SBTTriggerData* triggerData) {
    setID("SBTOptionsPopup");
    setTitle("Trigger Settings");
    m_title->setID("trigger-settings-title");
    m_mainLayer->setID("main-layer");
    m_buttonMenu->setID("button-menu");
    m_bgSprite->setID("background");
    m_closeBtn->setID("close-button");
    m_noElasticity = true;

    m_colors = triggerData->m_colors;
    m_widths = triggerData->m_widths;

    for (int i = 0; i < triggerData->m_beats; i++) {
        auto& [r, g, b, a] = m_colors[i];
        auto channelSprite = CCSprite::createWithSpriteFrameName("GJ_colorBtn_001.png");
        channelSprite->setColor({ r, g, b });
        channelSprite->setOpacity(a);

        auto widthLabel = CCLabelBMFont::create(fmt::format("{:.02f}", m_widths[i]).c_str(), "bigFont.fnt");
        widthLabel->setPosition(channelSprite->getContentSize() / 2.0f - CCPoint { 0.0f, 9.5f });
        widthLabel->setScale(0.3f);
        channelSprite->addChild(widthLabel);

        auto channelButton = CCMenuItemExt::createSpriteExtra(channelSprite, [this, i](auto) {
            SmartBPMTrigger::createColorPopup(m_colors[i], m_widths[i], i, this)->show();
        });
        channelButton->setPosition({ 30.0f + (i % 5) * 47.5f, 115.0f - (int)(i / 5) * 47.5f });
        channelButton->setID(fmt::format("color-button-{}", i + 1));
        channelButton->setTag(i);
        m_buttonMenu->addChild(channelButton);
    }

    auto saveButton = CCMenuItemExt::createSpriteExtra(ButtonSprite::create("Save", 0.8f), [this, triggerData](auto) {
        triggerData->m_colors = m_colors;
        triggerData->m_widths = m_widths;
        triggerData->m_changed = m_changed;
        onClose(nullptr);
    });
    saveButton->setPosition({ 125.0f, 25.0f });
    saveButton->setID("save-button");
    m_buttonMenu->addChild(saveButton);

    return true;
}

void SBTOptionsPopup::colorSelectClosed(CCNode* node) {
    auto action = static_cast<ColorSelectPopup*>(node)->m_colorAction;
    auto index = node->getTag();
    ccColor4B newColor = { action->m_fromColor.r, action->m_fromColor.g, action->m_fromColor.b, (uint8_t)action->m_fromOpacity };
    m_changed = m_changed || m_colors[index] != newColor || m_widths[index] != action->m_toOpacity;
    m_colors[index] = newColor;
    m_widths[index] = action->m_toOpacity;
    auto sprite = static_cast<CCNodeRGBA*>(static_cast<CCMenuItemSprite*>(m_buttonMenu->getChildByTag(index))->getNormalImage());
    sprite->setColor(action->m_fromColor);
    sprite->setOpacity(action->m_fromOpacity);
    sprite->getChildByIndex<CCLabelBMFont>(0)->setString(fmt::format("{:.02f}", action->m_toOpacity).c_str());
}
