#include "../SmartBPMTrigger.hpp"
#include "../classes/SBTOptionsPopup.hpp"
#include <Geode/binding/EffectGameObject.hpp>
#include <Geode/modify/SetupTriggerPopup.hpp>

using namespace geode::prelude;

class $modify(SBTSetupTriggerPopup, SetupTriggerPopup) {
    static void onModify(ModifyBase<ModifyDerive<SBTSetupTriggerPopup, SetupTriggerPopup>>& self) {
        SmartBPMTrigger::modify(self);
    }

    void valueChanged(int property, float value) {
        SetupTriggerPopup::valueChanged(property, value);

        if (property != 501) return;

        if (m_gameObject && m_gameObject->m_objectID == 3642)
            updateBeats(static_cast<SBTTriggerData*>(m_gameObject->getUserObject("trigger-data"_spr)), value);
        else if (m_gameObjects) {
            for (auto object : CCArrayExt<GameObject*>(m_gameObjects)) {
                if (object->m_objectID == 3642) updateBeats(static_cast<SBTTriggerData*>(object->getUserObject("trigger-data"_spr)), value);
            }
        }
    }

    static void updateBeats(SBTTriggerData* data, int beats) {
        if (!data) return;

        auto mod = Mod::get();
        auto& colors = data->m_colors;
        if (colors.size() < beats) {
            auto bpmColor = SmartBPMTrigger::get<"beats-per-minute-color", ccColor4B>(mod);
            auto bpbColor = SmartBPMTrigger::get<"beats-per-bar-color", ccColor4B>(mod);
            for (int i = colors.size(); i < beats; i++) {
                colors.push_back(i == 0 ? bpmColor : bpbColor);
            }
        }
        auto& widths = data->m_widths;
        if (widths.size() < beats) {
            auto bpmWidth = SmartBPMTrigger::get<"beats-per-minute-width", float>(mod);
            auto bpbWidth = SmartBPMTrigger::get<"beats-per-bar-width", float>(mod);
            for (int i = widths.size(); i < beats; i++) {
                widths.push_back(i == 0 ? bpmWidth : bpbWidth);
            }
        }
        data->m_beats = beats;
    }
};
