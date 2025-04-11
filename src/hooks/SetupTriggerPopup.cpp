#include "../SmartBPMTrigger.hpp"
#include "../classes/SBTOptionsPopup.hpp"
#include <Geode/binding/EffectGameObject.hpp>
#include <Geode/modify/SetupTriggerPopup.hpp>

using namespace geode::prelude;

class $modify(SBTSetupTriggerPopup, SetupTriggerPopup) {
    static void onModify(ModifyBase<ModifyDerive<SBTSetupTriggerPopup, SetupTriggerPopup>>& self) {
        (void)self.getHook("SetupTriggerPopup::valueChanged").map([](Hook* hook) {
            auto mod = Mod::get();
            hook->setAutoEnable(SmartBPMTrigger::enabled(mod));

            SmartBPMTrigger::settingListener<"enabled", bool>([hook](bool value) {
                (void)(value ? hook->enable().mapErr([](const std::string& err) {
                    return log::error("Failed to enable SetupTriggerPopup::valueChanged hook: {}", err), err;
                }) : hook->disable().mapErr([](const std::string& err) {
                    return log::error("Failed to disable SetupTriggerPopup::valueChanged hook: {}", err), err;
                }));
            }, mod);

            return hook;
        }).mapErr([](const std::string& err) {
            return log::error("Failed to get SetupTriggerPopup::valueChanged hook: {}", err), err;
        });
    }

    void valueChanged(int property, float value) {
        SetupTriggerPopup::valueChanged(property, value);

        if (property != 501) return;

        if (m_gameObject && m_gameObject->m_objectID == 3642)
            updateBeats(static_cast<SBTTriggerData*>(m_gameObject->getUserObject("trigger-data"_spr)), value);
        else if (m_gameObjects) {
            for (auto object : CCArrayExt<EffectGameObject*>(m_gameObjects)) {
                if (object->m_objectID == 3642) updateBeats(static_cast<SBTTriggerData*>(object->getUserObject("trigger-data"_spr)), value);
            }
        }
    }

    static void updateBeats(SBTTriggerData* data, int beats) {
        if (!data) return;

        auto mod = Mod::get();
        auto& colors = data->m_colors;
        if (colors.size() < beats) {
            auto bpmColor = SmartBPMTrigger::bpmColor(mod);
            auto bpbColor = SmartBPMTrigger::bpbColor(mod);
            for (int i = colors.size(); i < beats; i++) {
                colors.push_back(i == 0 ? bpmColor : bpbColor);
            }
        }
        auto& widths = data->m_widths;
        if (widths.size() < beats) {
            auto bpmWidth = SmartBPMTrigger::bpmWidth(mod);
            auto bpbWidth = SmartBPMTrigger::bpbWidth(mod);
            for (int i = widths.size(); i < beats; i++) {
                widths.push_back(i == 0 ? bpmWidth : bpbWidth);
            }
        }
        data->m_beats = beats;
    }
};
