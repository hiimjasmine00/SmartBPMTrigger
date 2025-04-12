#include "../SmartBPMTrigger.hpp"
#include "../classes/SBTOptionsPopup.hpp"
#include <Geode/binding/EffectGameObject.hpp>
#include <Geode/modify/SetupTriggerPopup.hpp>

using namespace geode::prelude;

class $modify(SBTSetupTriggerPopup, SetupTriggerPopup) {
    SBT_MODIFY(SetupTriggerPopup)

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
            auto bpmColor = SmartBPMTrigger::getColor(GuidelineType::BPM, mod);
            auto bpbColor = SmartBPMTrigger::getColor(GuidelineType::BPB, mod);
            for (int i = colors.size(); i < beats; i++) {
                colors.push_back(i == 0 ? bpmColor : bpbColor);
            }
        }
        auto& widths = data->m_widths;
        if (widths.size() < beats) {
            auto bpmWidth = SmartBPMTrigger::getWidth(GuidelineType::BPM, mod);
            auto bpbWidth = SmartBPMTrigger::getWidth(GuidelineType::BPB, mod);
            for (int i = widths.size(); i < beats; i++) {
                widths.push_back(i == 0 ? bpmWidth : bpbWidth);
            }
        }
        data->m_beats = beats;
    }
};
