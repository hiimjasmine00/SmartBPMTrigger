#include "SBTTriggerData.hpp"
#include "../SmartBPMTrigger.hpp"

using namespace geode::prelude;

SBTTriggerData* SBTTriggerData::create(const std::string& str, int beats) {
    auto ret = new SBTTriggerData();
    ret->m_beats = beats;

    auto mod = Mod::get();
    auto bpmColor = SmartBPMTrigger::get<"beats-per-minute-color", ccColor4B>(mod);
    auto bpmWidth = SmartBPMTrigger::get<"beats-per-minute-width", float>(mod);
    auto bpbColor = SmartBPMTrigger::get<"beats-per-bar-color", ccColor4B>(mod);
    auto bpbWidth = SmartBPMTrigger::get<"beats-per-bar-width", float>(mod);

    if (!str.empty()) {
        auto split = string::split(str, ",");

        auto hasChanged = false;
        for (auto it = split.begin(); it < split.end() - 1; it += 2) {
            auto& type = it[0];
            auto& value = it[1];

            if (type.size() != 1 || value.empty()) continue;

            switch (type[0]) {
                case '1': {
                    auto colors = string::split(value, "~");
                    for (int i = 0; i < beats && i < colors.size(); i++) {
                        auto hexColor = numFromString<uint32_t>(colors[i]).unwrapOr(0);
                        ret->m_colors.emplace_back(
                            (uint8_t)((hexColor >> 24) & 255),
                            (uint8_t)((hexColor >> 16) & 255),
                            (uint8_t)((hexColor >> 8) & 255),
                            (uint8_t)(hexColor & 255)
                        );
                    }
                    break;
                }
                case '2': {
                    auto widths = string::split(value, "~");
                    for (int i = 0; i < beats && i < widths.size(); i++) {
                        ret->m_widths.push_back(numFromString<float>(widths[i]).unwrapOr(0.0f));
                    }
                    break;
                }
                case '3': {
                    ret->m_disabled = numFromString<uint32_t>(value).unwrapOr(0) > 0;
                    break;
                }
                case '4': {
                    hasChanged = true;
                    ret->m_changed = numFromString<uint32_t>(value).unwrapOr(0) > 0;
                    break;
                }
            }
        }

        if (!hasChanged) {
            for (int i = 0; i < ret->m_colors.size(); i++) {
                if (i == 0 && ret->m_colors[i] != bpmColor) {
                    ret->m_changed = true;
                    break;
                }
                else if (i != 0 && ret->m_colors[i] != bpbColor) {
                    ret->m_changed = true;
                    break;
                }
            }

            if (!ret->m_changed) {
                for (int i = 0; i < ret->m_widths.size(); i++) {
                    if (i == 0 && ret->m_widths[i] != bpmWidth) {
                        ret->m_changed = true;
                        break;
                    }
                    else if (i != 0 && ret->m_widths[i] != bpbWidth) {
                        ret->m_changed = true;
                        break;
                    }
                }
            }
        }
    }

    if (ret->m_colors.size() < beats) {
        for (int i = ret->m_colors.size(); i < beats; i++) {
            ret->m_colors.push_back(i == 0 ? bpmColor : bpbColor);
        }
    }

    if (ret->m_widths.size() < beats) {
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

    if (m_disabled) {
        if (!ret.empty()) ret += ',';
        ret += "3,1";
    }

    if (m_changed) {
        if (!ret.empty()) ret += ',';
        ret += "4,1";
    }

    return ret;
}
