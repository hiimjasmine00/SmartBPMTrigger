#include "SBTTriggerData.hpp"
#include <jasmine/convert.hpp>
#include <jasmine/setting.hpp>
#include <jasmine/string.hpp>

using namespace geode::prelude;

SBTTriggerData* SBTTriggerData::create(std::string_view str, int beats) {
    auto ret = new SBTTriggerData();
    ret->m_beats = beats;

    auto bpmColor = jasmine::setting::getValue<ccColor4B>("beats-per-minute-color");
    auto bpmWidth = jasmine::setting::getValue<float>("beats-per-minute-width");
    auto bpbColor = jasmine::setting::getValue<ccColor4B>("beats-per-bar-color");
    auto bpbWidth = jasmine::setting::getValue<float>("beats-per-bar-width");

    if (!str.empty()) {
        auto split = jasmine::string::split(str, ',');

        auto hasChanged = false;
        for (auto it = split.begin(); it < split.end() - 1; it += 2) {
            auto& type = it[0];
            auto& value = it[1];

            if (type.size() != 1 || value.empty()) continue;

            switch (type[0]) {
                case '1': {
                    auto colors = jasmine::string::split(value, '~');
                    for (int i = 0; i < beats && i < colors.size(); i++) {
                        auto& color = colors[i];
                        auto hexColor = jasmine::convert::getInt<uint32_t>(color).value_or(0);
                        ret->m_colors.emplace_back((hexColor >> 24) & 255, (hexColor >> 16) & 255, (hexColor >> 8) & 255, hexColor & 255);
                    }
                    break;
                }
                case '2': {
                    auto widths = jasmine::string::split(value, '~');
                    for (int i = 0; i < beats && i < widths.size(); i++) {
                        auto& width = widths[i];
                        auto widthValue = jasmine::convert::getFloat<float>(width).value_or(0.0f);
                        ret->m_widths.push_back(widthValue);
                    }
                    break;
                }
                case '3': {
                    auto disabledValue = jasmine::convert::getInt<uint32_t>(value).value_or(0);
                    ret->m_disabled = disabledValue > 0;
                    break;
                }
                case '4': {
                    hasChanged = true;
                    auto changedValue = jasmine::convert::getInt<uint32_t>(value).value_or(0);
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
    fmt::memory_buffer ret;

    if (!m_colors.empty()) {
        fmt::format_to(std::back_inserter(ret), "1,");
        for (int i = 0; i < m_beats; i++) {
            if (i > 0) ret.push_back('~');
            auto& [r, g, b, a] = m_colors[i];
            fmt::format_to(std::back_inserter(ret), "{}", fmt::to_string<uint32_t>(r << 24 | g << 16 | b << 8 | a));
        }
    }

    if (!m_widths.empty()) {
        if (ret.size() > 0) ret.push_back(',');
        fmt::format_to(std::back_inserter(ret), "2,");
        for (int i = 0; i < m_beats; i++) {
            if (i > 0) ret.push_back('~');
            fmt::format_to(std::back_inserter(ret), "{}", m_widths[i]);
        }
    }

    if (m_disabled) {
        if (ret.size() > 0) ret.push_back(',');
        fmt::format_to(std::back_inserter(ret), "3,1");
    }

    if (m_changed) {
        if (ret.size() > 0) ret.push_back(',');
        fmt::format_to(std::back_inserter(ret), "4,1");
    }

    return fmt::to_string(ret);
}
