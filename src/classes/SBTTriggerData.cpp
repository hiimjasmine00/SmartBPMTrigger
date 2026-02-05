#include "SBTTriggerData.hpp"
#include <Geode/utils/string.hpp>
#include <Geode/utils/StringBuffer.hpp>
#include <jasmine/convert.hpp>
#include <jasmine/setting.hpp>

using namespace geode::prelude;

SBTTriggerData* SBTTriggerData::create(std::string_view str, int beats) {
    auto ret = new SBTTriggerData();
    ret->m_beats = beats;

    auto bpmColor = jasmine::setting::getValue<ccColor4B>("beats-per-minute-color");
    auto bpmWidth = jasmine::setting::getValue<float>("beats-per-minute-width");
    auto bpbColor = jasmine::setting::getValue<ccColor4B>("beats-per-bar-color");
    auto bpbWidth = jasmine::setting::getValue<float>("beats-per-bar-width");

    if (!str.empty()) {
        auto split = string::splitView(str, ",");

        auto hasChanged = false;
        for (auto it = split.begin(); it < split.end() - 1; it += 2) {
            auto& type = it[0];
            auto& value = it[1];

            if (type.size() != 1 || value.empty()) continue;

            switch (type[0]) {
                case '1': {
                    auto colors = string::splitView(value, "~");
                    for (int i = 0; i < beats && i < colors.size(); i++) {
                        auto& color = colors[i];
                        auto hexColor = jasmine::convert::getOr(color, 0u);
                        ret->m_colors.emplace_back((hexColor >> 24) & 255, (hexColor >> 16) & 255, (hexColor >> 8) & 255, hexColor & 255);
                    }
                    break;
                }
                case '2': {
                    auto widths = string::splitView(value, "~");
                    for (int i = 0; i < beats && i < widths.size(); i++) {
                        auto& width = widths[i];
                        auto widthValue = jasmine::convert::getOr(width, 0.0f);
                        ret->m_widths.push_back(widthValue);
                    }
                    break;
                }
                case '3': {
                    auto disabledValue = jasmine::convert::getOr(value, 0u);
                    ret->m_disabled = disabledValue > 0;
                    break;
                }
                case '4': {
                    hasChanged = true;
                    auto changedValue = jasmine::convert::getOr(value, 0u);
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
    StringBuffer ret;

    if (!m_colors.empty()) {
        ret.append("1,");
        for (int i = 0; i < m_beats; i++) {
            if (i > 0) ret.append('~');
            auto& [r, g, b, a] = m_colors[i];
            ret.append(fmt::to_string<uint32_t>(r << 24 | g << 16 | b << 8 | a));
        }
    }

    if (!m_widths.empty()) {
        if (ret.size() > 0) ret.append(',');
        ret.append("2,");
        for (int i = 0; i < m_beats; i++) {
            if (i > 0) ret.append('~');
            ret.append(fmt::to_string(m_widths[i]));
        }
    }

    if (m_disabled) {
        if (ret.size() > 0) ret.append(',');
        ret.append("3,1");
    }

    if (m_changed) {
        if (ret.size() > 0) ret.append(',');
        ret.append("4,1");
    }

    return ret.str();
}
