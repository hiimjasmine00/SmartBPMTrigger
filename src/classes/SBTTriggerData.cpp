#include "SBTTriggerData.hpp"
#include "../SmartBPMTrigger.hpp"

using namespace geode::prelude;

SBTTriggerData* SBTTriggerData::create(const std::string& str, int beats) {
    auto ret = new SBTTriggerData();
    ret->m_beats = beats;

    if (!str.empty()) {
        auto split = string::split(str, ",");

        for (auto it = split.begin(); it != split.end(); it += 2) {
            auto& type = it[0];
            auto& value = it[1];

            if (type == "1") {
                auto colors = string::split(value, "~");
                for (int i = 0; i < beats && i < colors.size(); i++) {
                    auto hexColor = numFromString<uint32_t>(colors[i]).unwrapOr(0);
                    ret->m_colors.push_back({
                        (uint8_t)((hexColor >> 24) & 255),
                        (uint8_t)((hexColor >> 16) & 255),
                        (uint8_t)((hexColor >> 8) & 255),
                        (uint8_t)(hexColor & 255)
                    });
                }
            }
            else if (type == "2") {
                auto widths = string::split(value, "~");
                for (int i = 0; i < beats && i < widths.size(); i++) {
                    ret->m_widths.push_back(numFromString<float>(widths[i]).unwrapOr(0.0f));
                }
            }
        }
    }

    auto mod = Mod::get();

    if (ret->m_colors.size() < beats) {
        auto bpmColor = SmartBPMTrigger::bpmColor(mod);
        auto bpbColor = SmartBPMTrigger::bpbColor(mod);
        for (int i = ret->m_colors.size(); i < beats; i++) {
            ret->m_colors.push_back(i == 0 ? bpmColor : bpbColor);
        }
    }

    if (ret->m_widths.size() < beats) {
        auto bpmWidth = SmartBPMTrigger::bpmWidth(mod);
        auto bpbWidth = SmartBPMTrigger::bpbWidth(mod);
        for (int i = ret->m_widths.size(); i < beats; i++) {
            ret->m_widths.push_back(i == 0 ? bpmWidth : bpbWidth);
        }
    }

    ret->autorelease();
    return ret;
}

std::string SBTTriggerData::getSaveString() {
    std::string ret;

    if (!m_colors.empty()) {
        ret += "1,";
        std::string colors;
        for (int i = 0; i < m_beats; i++) {
            if (!colors.empty()) colors += '~';
            auto& [r, g, b, a] = m_colors[i];
            colors += fmt::to_string<uint32_t>(r << 24 | g << 16 | b << 8 | a);
        }
        ret += colors;
    }

    if (!m_widths.empty()) {
        if (!ret.empty()) ret += ',';
        ret += "2,";
        std::string widths;
        for (int i = 0; i < m_beats; i++) {
            if (!widths.empty()) widths += '~';
            widths += fmt::to_string(m_widths[i]);
        }
        ret += widths;
    }

    return ret;
}
