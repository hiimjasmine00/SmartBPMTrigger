#include "../SmartBPMTrigger.hpp"
#include "../classes/SBTOptionsPopup.hpp"
#include <Geode/binding/AudioLineGuideGameObject.hpp>
#include <Geode/modify/SetupAudioLineGuidePopup.hpp>

using namespace geode::prelude;

class $modify(SBTSetupAudioLineGuidePopup, SetupAudioLineGuidePopup) {
    static void onModify(ModifyBase<ModifyDerive<SBTSetupAudioLineGuidePopup, SetupAudioLineGuidePopup>>& self) {
        (void)self.getHook("SetupAudioLineGuidePopup::init").map([](Hook* hook) {
            auto mod = Mod::get();
            hook->setAutoEnable(SmartBPMTrigger::enabled(mod));

            SmartBPMTrigger::settingListener<"enabled", bool>([hook](bool value) {
                (void)(value ? hook->enable().mapErr([](const std::string& err) {
                    return log::error("Failed to enable SetupAudioLineGuidePopup::init hook: {}", err), err;
                }) : hook->disable().mapErr([](const std::string& err) {
                    return log::error("Failed to disable SetupAudioLineGuidePopup::init hook: {}", err), err;
                }));
            }, mod);

            return hook;
        }).mapErr([](const std::string& err) {
            return log::error("Failed to get SetupAudioLineGuidePopup::init hook: {}", err), err;
        });
    }

    bool init(AudioLineGuideGameObject* object, CCArray* objects) {
        if (!SetupAudioLineGuidePopup::init(object, objects)) return false;

        for (auto node : CCArrayExt<CCNode*>(m_mainLayer->getChildren())) {
            if (auto label = typeinfo_cast<CCLabelBMFont*>(node)) {
                if (strcmp(label->getString(), "Speed:") == 0) {
                    if (object) {
                        label->setString("Settings");
                        label->setPositionY(label->getPositionY() - 15.0f);
                    }
                    else label->setVisible(false);
                    break;
                }
            }
        }

        if (!object) {
            m_speedSprite->getParent()->setVisible(false);
            return true;
        }

        m_speedSprite->setDisplayFrame(CCSpriteFrameCache::get()->spriteFrameByName("GJ_optionsBtn_001.png"));

        auto settingsButton = static_cast<CCMenuItemSpriteExtra*>(m_speedSprite->getParent());
        settingsButton->setPositionY(settingsButton->getPositionY() - 7.0f);
        settingsButton->setTarget(this, menu_selector(SBTSetupAudioLineGuidePopup::onSettings));
        settingsButton->updateSprite();

        return true;
    }

    void onSettings(CCObject*) {
        if (m_gameObject) SBTOptionsPopup::create(static_cast<SBTTriggerData*>(m_gameObject->getUserObject("trigger-data"_spr)))->show();
    }
};
