#include "../SmartBPMTrigger.hpp"
#include <Geode/binding/CCTextInputNode.hpp>
#include <Geode/binding/ColorAction.hpp>
#include <Geode/binding/Slider.hpp>
#include <Geode/modify/ColorSelectPopup.hpp>

using namespace geode::prelude;

class $modify(SBTColorSelectPopup, ColorSelectPopup) {
    struct Fields {
        bool m_customSelect;
        CCLabelBMFont* m_opacityLabel;
        CCTextInputNode* m_widthInput;
        Slider* m_widthSlider;
    };

    SBT_MODIFY(ColorSelectPopup)

    bool init(EffectGameObject* object, CCArray* objects, ColorAction* action) {
        if (!ColorSelectPopup::init(object, objects, action)) return false;

        auto f = m_fields.self();
        if (!f->m_customSelect) return true;

        m_originalColorSprite->setOpacity(action->m_fromOpacity);
        m_currentColorSprite->setOpacity(action->m_fromOpacity);

        auto center = CCDirector::get()->getWinSize() / 2.0f;

        f->m_opacityLabel = CCLabelBMFont::create(fmt::format("Opacity: {}", action->m_fromOpacity).c_str(), "goldFont.fnt");
        f->m_opacityLabel->setPosition(center + CCPoint { 150.0f, -15.0f });
        f->m_opacityLabel->setScale(0.65f);
        f->m_opacityLabel->setID("opacity-label"_spr);
        m_mainLayer->addChild(f->m_opacityLabel);

        auto opacitySlider = Slider::create(this, menu_selector(SBTColorSelectPopup::onOpacitySlider), 0.55f);
        opacitySlider->m_sliderBar->setVisible(false);
        opacitySlider->m_touchLogic->m_thumb->setValue(action->m_fromOpacity / 255.0f);
        opacitySlider->setPosition(center + CCPoint { 150.0f, -35.0f });
        opacitySlider->setID("opacity-slider"_spr);
        m_mainLayer->addChild(opacitySlider);

        auto widthLabel = CCLabelBMFont::create("Width:", "goldFont.fnt");
        widthLabel->setPosition(center - CCPoint { 0.0f, 68.0f });
        widthLabel->setScale(0.7f);
        widthLabel->setAnchorPoint({ 1.0f, 0.5f });
        widthLabel->setID("width-label"_spr);
        m_mainLayer->addChild(widthLabel);

        auto widthBackground = CCScale9Sprite::create("square02_small.png", { 0.0f, 0.0f, 40.0f, 40.0f });
        widthBackground->setPosition(center + CCPoint { 45.0f, -68.0f });
        widthBackground->setContentSize({ 70.0f, 30.0f });
        widthBackground->setOpacity(100);
        widthBackground->setID("width-background"_spr);
        m_mainLayer->addChild(widthBackground);

        m_disableTextDelegate = true;

        f->m_widthInput = CCTextInputNode::create(50.0f, 40.0f, "Num", "bigFont.fnt");
        f->m_widthInput->setPosition(center + CCPoint { 45.0f, -68.0f });
        f->m_widthInput->setMaxLabelScale(0.6f);
        f->m_widthInput->setMaxLabelWidth(200.0f);
        f->m_widthInput->setMaxLabelLength(5);
        f->m_widthInput->setAllowedChars("0123456789.");
        f->m_widthInput->setLabelPlaceholderColor({ 120, 170, 240 });
        f->m_widthInput->setLabelPlaceholderScale(0.5f);
        f->m_widthInput->setDelegate(this);
        f->m_widthInput->setString(fmt::format("{:.2f}", action->m_toOpacity).c_str());
        f->m_widthInput->setID("width-input"_spr);
        m_mainLayer->addChild(f->m_widthInput);

        m_disableTextDelegate = false;

        f->m_widthSlider = Slider::create(this, menu_selector(SBTColorSelectPopup::onWidthSlider), 0.8f);
        f->m_widthSlider->m_sliderBar->setVisible(false);
        f->m_widthSlider->m_touchLogic->m_thumb->setValue(action->m_toOpacity / 5.0f);
        f->m_widthSlider->setPosition(center - CCPoint { 0.0f, 96.0f });
        f->m_widthSlider->setID("width-slider"_spr);
        m_mainLayer->addChild(f->m_widthSlider);

        return true;
    }

    void textChanged(CCTextInputNode* input) {
        ColorSelectPopup::textChanged(input);

        auto f = m_fields.self();
        if (m_disableTextDelegate || input != f->m_widthInput || !f->m_customSelect) return;

        auto width = std::clamp(numFromString<float>(input->getString()).unwrapOr(0.0f), 0.0f, 5.0f);
        m_colorAction->m_toOpacity = width;
        m_fields->m_widthSlider->m_touchLogic->m_thumb->setValue(width / 5.0f);
    }

    void closeColorSelect(CCObject* sender) {
        auto f = m_fields.self();
        if (!f->m_customSelect) return;

        f->m_widthInput->onClickTrackNode(false);
        f->m_widthInput->setDelegate(nullptr);

        ColorSelectPopup::closeColorSelect(sender);
    }

    void onOpacitySlider(CCObject* sender) {
        int opacity = roundf(static_cast<SliderThumb*>(sender)->getValue() * 255.0f);
        m_colorAction->m_fromOpacity = opacity;
        m_fields->m_opacityLabel->setString(fmt::format("Opacity: {}", opacity).c_str());
        m_currentColorSprite->setOpacity(opacity);
    }

    void onWidthSlider(CCObject* sender) {
        auto width = roundf(static_cast<SliderThumb*>(sender)->getValue() * 500.0f) / 100.0f;
        m_colorAction->m_toOpacity = width;
        m_disableTextDelegate = true;
        m_fields->m_widthInput->setString(fmt::format("{:.2f}", width).c_str());
        m_disableTextDelegate = false;
    }
};

ColorSelectPopup* SmartBPMTrigger::createColorPopup(const ccColor4B& color, float width, int index, ColorSelectDelegate* delegate) {
    auto popup = new ColorSelectPopup();
    static_cast<SBTColorSelectPopup*>(popup)->m_fields->m_customSelect = true;
    auto action = ColorAction::create();
    action->m_fromColor.r = color.r;
    action->m_fromColor.g = color.g;
    action->m_fromColor.b = color.b;
    action->m_fromOpacity = color.a;
    action->m_toOpacity = width;
    if (popup->init(nullptr, nullptr, action)) {
        popup->autorelease();
        popup->setTag(index);
        popup->m_delegate = delegate;
        return popup;
    }
    delete popup;
    return nullptr;
}
