#include "../SmartBPMTrigger.hpp"
#include "../classes/SBTOptionsPopup.hpp"
#include <Geode/binding/AudioLineGuideGameObject.hpp>
#include <Geode/binding/GameToolbox.hpp>
#include <Geode/binding/SelectSettingLayer.hpp>
#include <Geode/modify/SetupAudioLineGuidePopup.hpp>

using namespace geode::prelude;

class $modify(SBTSetupAudioLineGuidePopup, SetupAudioLineGuidePopup) {
    struct Fields {
        CCLabelBMFont* m_speedLabel;
    };

    SBT_MODIFY

    bool init(AudioLineGuideGameObject* object, CCArray* objects) {
        if (!SetupAudioLineGuidePopup::init(object, objects)) return false;

        auto f = m_fields.self();
        for (auto node : CCArrayExt<CCNode*>(m_mainLayer->getChildren())) {
            if (auto label = typeinfo_cast<CCLabelBMFont*>(node)) {
                if (label->m_sInitialStringUTF8 == "Speed:") {
                    f->m_speedLabel = label;
                    break;
                }
            }
        }

        if (!f->m_speedLabel) return true;

        auto enabled = true;
        if (object) {
            if (auto triggerData = static_cast<SBTTriggerData*>(object->getUserObject("trigger-data"_spr)))
                enabled = !triggerData->m_disabled;
        }
        else if (objects) {
            for (auto object : CCArrayExt<CCNode*>(objects)) {
                auto triggerData = static_cast<SBTTriggerData*>(object->getUserObject("trigger-data"_spr));
                if (triggerData && triggerData->m_disabled) {
                    enabled = false;
                    break;
                }
            }
        }

        toggleState(enabled);

        auto toggleArray = CCArray::create();
        GameToolbox::createToggleButton("Smart", menu_selector(SBTSetupAudioLineGuidePopup::onSmart), enabled, m_buttonMenu,
            CCDirector::get()->getWinSize() / 2.0f + CCPoint { m_width / 2.0f - 85.0f, 25.0f - m_height / 2.0f },
            this, m_mainLayer, 0.7f, 0.35f, 110.0f, { 0.0f, 4.0f }, "bigFont.fnt", true, 0, toggleArray);
        static_cast<CCNode*>(toggleArray->objectAtIndex(0))->setID("smart-toggle"_spr);
        static_cast<CCNode*>(toggleArray->objectAtIndex(1))->setID("smart-label"_spr);

        return true;
    }

    void onSmart(CCObject* sender) {
        auto enabled = !static_cast<CCMenuItemToggler*>(sender)->m_toggled;
        if (m_gameObject) {
            if (auto triggerData = static_cast<SBTTriggerData*>(m_gameObject->getUserObject("trigger-data"_spr)))
                triggerData->m_disabled = !enabled;
        }
        else if (m_gameObjects) {
            for (auto object : CCArrayExt<CCNode*>(m_gameObjects)) {
                if (auto triggerData = static_cast<SBTTriggerData*>(object->getUserObject("trigger-data"_spr)))
                    triggerData->m_disabled = !enabled;
            }
        }
        toggleState(enabled);
    }

    void toggleState(bool state) {
        auto f = m_fields.self();
        auto center = CCDirector::get()->getWinSize().height / 2.0f - m_height / 2.0f;
        auto speedButton = static_cast<CCMenuItemSpriteExtra*>(m_speedSprite->getParent());
        if (m_gameObject) {
            f->m_speedLabel->setString(state ? "Settings" : "Speed:");
            f->m_speedLabel->setPositionY(center + 70.0f - (state * 15.0f));
            m_speedSprite->setDisplayFrame(CCSpriteFrameCache::get()->spriteFrameByName(state ? "GJ_optionsBtn_001.png" :
                SelectSettingLayer::frameForValue(SelectSettingType::StartingSpeed, getValue(499)).c_str()));
            speedButton->setPositionY(center + 36.0f - (state * 7.0f) - m_buttonMenu->getPositionY());
            speedButton->updateSprite();
        }
        else {
            f->m_speedLabel->setVisible(!state);
            speedButton->setVisible(!state);
        }
    }

    void onSpeed(CCObject* sender) {
        if (m_gameObject) {
            auto triggerData = static_cast<SBTTriggerData*>(m_gameObject->getUserObject("trigger-data"_spr));
            if (triggerData && !triggerData->m_disabled) return SBTOptionsPopup::create(triggerData)->show();
        }

        SetupAudioLineGuidePopup::onSpeed(sender);
    }
};
