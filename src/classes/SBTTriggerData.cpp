#include "SBTTriggerData.hpp"
#include "../SmartBPMTrigger.hpp"

using namespace geode::prelude;

SBTTriggerData* SBTTriggerData::create(const std::string& str, int beats) {
    auto ret = new SBTTriggerData();
    ret->m_beats = beats;

    auto bpmColor = SmartBPMTrigger::get<ccColor4B>("beats-per-minute-color");
    auto bpmWidth = SmartBPMTrigger::get<float>("beats-per-minute-width");
    auto bpbColor = SmartBPMTrigger::get<ccColor4B>("beats-per-bar-color");
    auto bpbWidth = SmartBPMTrigger::get<float>("beats-per-bar-width");

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
                        auto& color = colors[i];
                        auto hexColor = 0u;
                        std::from_chars(color.data(), color.data() + color.size(), hexColor);
                        ret->m_colors.emplace_back((hexColor >> 24) & 255, (hexColor >> 16) & 255, (hexColor >> 8) & 255, hexColor & 255);
                    }
                    break;
                }
                case '2': {
                    auto widths = string::split(value, "~");
                    for (int i = 0; i < beats && i < widths.size(); i++) {
                        auto& width = widths[i];
                        auto widthValue = 0.0f;
                        #ifdef __cpp_lib_to_chars
                        std::from_chars(width.data(), width.data() + width.size(), widthValue);
                        #else
                        if (auto num = numFromString<float>(width).ok()) widthValue = *num;
                        #endif
                        ret->m_widths.push_back(widthValue);
                    }
                    break;
                }
                case '3': {
                    auto disabledValue = 0u;
                    std::from_chars(value.data(), value.data() + value.size(), disabledValue);
                    ret->m_disabled = disabledValue > 0;
                    break;
                }
                case '4': {
                    hasChanged = true;
                    auto changedValue = 0u;
                    std::from_chars(value.data(), value.data() + value.size(), changedValue);
                    ret->m_changed = changedValue > 0;
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
