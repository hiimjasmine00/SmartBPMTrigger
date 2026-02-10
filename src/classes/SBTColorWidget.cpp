#include "SBTColorWidget.hpp"
#include "../SmartBPMTrigger.hpp"
#include <Geode/binding/Slider.hpp>
#include <Geode/loader/Mod.hpp>
#include <jasmine/hook.hpp>

using namespace geode::prelude;

SBTColorWidget* SBTColorWidget::create(const ccColor4B& color, float width, Function<void(const ccColor4B&, float)> callback) {
    auto ret = new SBTColorWidget();
    if (ret->init(color, width, std::move(callback))) {
        ret->autorelease();
        return ret;
    }
    delete ret;
    return nullptr;
}

bool SBTColorWidget::init(const ccColor4B& color, float width, Function<void(const ccColor4B&, float)> callback) {
    if (!CCLayer::init()) return false;

    setID("SBTColorWidget");
    setContentSize({ 230.0f, 290.0f });
    ignoreAnchorPointForPosition(false);

    m_color = color;
    m_width = width;
    m_callback = std::move(callback);

    m_colorPicker = CCControlColourPicker::colourPicker();
    m_colorPicker->setPosition({ 100.0f, 205.0f - Loader::get()->isModLoaded("flow.betterpicker") * 10.0f });
    m_colorPicker->setScale(0.8f);
    m_colorPicker->setColorValue({ m_color.r, m_color.g, m_color.b });
    m_colorPicker->setDelegate(this);
    m_colorPicker->setID("color-picker");
    addChild(m_colorPicker);

    for (auto child : CCArrayExt<CCNode*>(m_colorPicker->getChildren())) {
        if (!child->isVisible()) continue;
        if (exact_cast<CCSpriteBatchNode*>(child)) {
            m_opacityNodes.reserve(m_opacityNodes.size() + child->getChildrenCount());
            for (auto grandchild : CCArrayExt<CCNodeRGBA*>(child->getChildren())) {
                m_opacityNodes.push_back(grandchild);
            }
        }
        else if (auto opacityChild = typeinfo_cast<CCNodeRGBA*>(child)) {
            m_opacityNodes.push_back(opacityChild);
            for (auto grandchild : CCArrayExt<CCNode*>(child->getChildren())) {
                if (auto opacityGrandchild = typeinfo_cast<CCNodeRGBA*>(grandchild)) {
                    m_opacityNodes.push_back(opacityGrandchild);
                }
            }
        }
        else if (exact_cast<CCLabelBMFont*>(child) || exact_cast<CCMenu*>(child) || exact_cast<TextInput*>(child)) {
            child->setVisible(false);
        }
    }

    m_redLabel = CCLabelBMFont::create("R", "goldFont.fnt");
    m_redLabel->setPosition({ 35.0f, 145.0f });
    m_redLabel->setScale(0.6f);
    m_redLabel->setID("red-label");
    addChild(m_redLabel);

    m_greenLabel = CCLabelBMFont::create("G", "goldFont.fnt");
    m_greenLabel->setPosition({ 150.0f, 145.0f });
    m_greenLabel->setScale(0.6f);
    m_greenLabel->setID("green-label");
    addChild(m_greenLabel);

    m_blueLabel = CCLabelBMFont::create("B", "goldFont.fnt");
    m_blueLabel->setPosition({ 35.0f, 95.0f });
    m_blueLabel->setScale(0.6f);
    m_blueLabel->setID("blue-label");
    addChild(m_blueLabel);

    m_alphaLabel = CCLabelBMFont::create("A", "goldFont.fnt");
    m_alphaLabel->setPosition({ 150.0f, 95.0f });
    m_alphaLabel->setScale(0.6f);
    m_alphaLabel->setID("alpha-label");
    addChild(m_alphaLabel);

    m_widthLabel = CCLabelBMFont::create("Width", "goldFont.fnt");
    m_widthLabel->setPosition({ 90.0f, 45.0f });
    m_widthLabel->setScale(0.6f);
    m_widthLabel->setID("width-label");
    addChild(m_widthLabel);

    m_redInput = TextInput::create(60.0f, "Num");
    m_redInput->setPosition({ 65.0f, 145.0f });
    m_redInput->setScale(0.7f);
    m_redInput->setFilter("0123456789");
    m_redInput->setMaxCharCount(3);
    m_redInput->setString(fmt::to_string(m_color.r));
    m_redInput->setDelegate(this, 0);
    m_redInput->setID("red-input");
    addChild(m_redInput);

    m_greenInput = TextInput::create(60.0f, "Num");
    m_greenInput->setPosition({ 180.0f, 145.0f });
    m_greenInput->setScale(0.7f);
    m_greenInput->setFilter("0123456789");
    m_greenInput->setMaxCharCount(3);
    m_greenInput->setString(fmt::to_string(m_color.g));
    m_greenInput->setDelegate(this, 1);
    m_greenInput->setID("green-input");
    addChild(m_greenInput);

    m_blueInput = TextInput::create(60.0f, "Num");
    m_blueInput->setPosition({ 65.0f, 95.0f });
    m_blueInput->setScale(0.7f);
    m_blueInput->setFilter("0123456789");
    m_blueInput->setMaxCharCount(3);
    m_blueInput->setString(fmt::to_string(m_color.b));
    m_blueInput->setDelegate(this, 2);
    m_blueInput->setID("blue-input");
    addChild(m_blueInput);

    m_alphaInput = TextInput::create(60.0f, "Num");
    m_alphaInput->setPosition({ 180.0f, 95.0f });
    m_alphaInput->setScale(0.7f);
    m_alphaInput->setFilter("0123456789");
    m_alphaInput->setMaxCharCount(3);
    m_alphaInput->setString(fmt::to_string(m_color.a));
    m_alphaInput->setDelegate(this, 3);
    m_alphaInput->setID("alpha-input");
    addChild(m_alphaInput);

    m_widthInput = TextInput::create(60.0f, "Num");
    m_widthInput->setPosition({ 140.0f, 45.0f });
    m_widthInput->setScale(0.8f);
    m_widthInput->setFilter(".0123456789");
    m_widthInput->setMaxCharCount(4);
    m_widthInput->setString(fmt::format("{:.02f}", m_width));
    m_widthInput->setDelegate(this, 4);
    m_widthInput->setID("width-input");
    addChild(m_widthInput);

    m_redSlider = Slider::create(this, menu_selector(SBTColorWidget::sliderChanged), 0.5f);
    m_redSlider->m_touchLogic->m_thumb->setTag(0);
    m_redSlider->setPosition({ 57.5f, 125.0f });
    m_redSlider->setValue(m_color.r / 255.0f);
    m_redSlider->setBarVisibility(false);
    m_redSlider->setID("red-slider");
    addChild(m_redSlider);

    m_greenSlider = Slider::create(this, menu_selector(SBTColorWidget::sliderChanged), 0.5f);
    m_greenSlider->m_touchLogic->m_thumb->setTag(1);
    m_greenSlider->setPosition({ 172.5f, 125.0f });
    m_greenSlider->setValue(m_color.g / 255.0f);
    m_greenSlider->setBarVisibility(false);
    m_greenSlider->setID("green-slider");
    addChild(m_greenSlider);

    m_blueSlider = Slider::create(this, menu_selector(SBTColorWidget::sliderChanged), 0.5f);
    m_blueSlider->m_touchLogic->m_thumb->setTag(2);
    m_blueSlider->setPosition({ 57.5f, 75.0f });
    m_blueSlider->setValue(m_color.b / 255.0f);
    m_blueSlider->setBarVisibility(false);
    m_blueSlider->setID("blue-slider");
    addChild(m_blueSlider);

    m_alphaSlider = Slider::create(this, menu_selector(SBTColorWidget::sliderChanged), 0.5f);
    m_alphaSlider->m_touchLogic->m_thumb->setTag(3);
    m_alphaSlider->setPosition({ 172.5f, 75.0f });
    m_alphaSlider->setValue(m_color.a / 255.0f);
    m_alphaSlider->setBarVisibility(false);
    m_alphaSlider->setID("alpha-slider");
    addChild(m_alphaSlider);

    m_widthSlider = Slider::create(this, menu_selector(SBTColorWidget::sliderChanged), 0.8f);
    m_widthSlider->m_touchLogic->m_thumb->setTag(4);
    m_widthSlider->setPosition({ 115.0f, 20.0f });
    m_widthSlider->setValue(m_width / 5.0f);
    m_widthSlider->setBarVisibility(false);
    m_widthSlider->setID("width-slider");
    addChild(m_widthSlider);

    runAction(CCSequence::createWithTwoActions(CCDelayTime::create(0.0f),
        CCCallFunc::create(this, callfunc_selector(SBTColorWidget::hideSliderBackgrounds))));

    m_redNode = CCDrawNode::create();
    m_redNode->setPosition({ 2.0f, 4.0f });
    m_redNode->setID("red-node"_spr);
    m_redSlider->m_groove->addChild(m_redNode, -1);

    m_greenNode = CCDrawNode::create();
    m_greenNode->setPosition({ 2.0f, 4.0f });
    m_greenNode->setID("green-node"_spr);
    m_greenSlider->m_groove->addChild(m_greenNode, -1);

    m_blueNode = CCDrawNode::create();
    m_blueNode->setPosition({ 2.0f, 4.0f });
    m_blueNode->setID("blue-node"_spr);
    m_blueSlider->m_groove->addChild(m_blueNode, -1);

    m_alphaNode = CCDrawNode::create();
    m_alphaNode->setPosition({ 2.0f, 4.0f });
    m_alphaNode->setID("alpha-node"_spr);
    m_alphaSlider->m_groove->addChild(m_alphaNode, -1);

    m_widthSprite = CCSprite::create("sliderBar2.png");
    m_widthSprite->setPosition({ 2.0f, 4.0f });
    m_widthSprite->setAnchorPoint({ 0.0f, 0.0f });
    ccTexParams texParams = { GL_LINEAR, GL_LINEAR, GL_REPEAT, GL_REPEAT };
    m_widthSprite->getTexture()->setTexParameters(&texParams);
    m_widthSprite->setTextureRect({ 0.0f, 0.0f, m_widthSlider->m_width, m_widthSlider->m_height });
    m_widthSprite->setID("width-sprite"_spr);
    m_widthSlider->m_groove->addChild(m_widthSprite, -1);

    updateNodes(false, false, false, false);

    return true;
}

void SBTColorWidget::hideSliderBackgrounds() {
    m_redSlider->setBarVisibility(false);
    m_greenSlider->setBarVisibility(false);
    m_blueSlider->setBarVisibility(false);
    m_alphaSlider->setBarVisibility(false);
    m_widthSlider->setBarVisibility(false);
}

void SBTColorWidget::sliderChanged(CCObject* sender) {
    auto thumb = static_cast<SliderThumb*>(sender);
    switch (sender->getTag()) {
        case 0:
            m_color.r = roundf(thumb->getValue() * 255.0f);
            updateRed(false, false, true);
            break;
        case 1:
            m_color.g = roundf(thumb->getValue() * 255.0f);
            updateGreen(false, false, true);
            break;
        case 2:
            m_color.b = roundf(thumb->getValue() * 255.0f);
            updateBlue(false, false, true);
            break;
        case 3:
            m_color.a = roundf(thumb->getValue() * 255.0f);
            updateAlpha(false, false, true);
            break;
        case 4:
            m_width = roundf(thumb->getValue() * 500.0f) / 100.0f;
            updateWidth(false, false, true);
            break;
    }
}

void SBTColorWidget::textChanged(CCTextInputNode* input) {
    auto str = input->getString();
    switch (input->getTag()) {
        case 0: {
            if (auto num = numFromString<uint8_t>(str)) {
                m_color.r = num.unwrap();
            }
            updateRed(false, true, false);
            break;
        }
        case 1: {
            if (auto num = numFromString<uint8_t>(str)) {
                m_color.g = num.unwrap();
            }
            updateGreen(false, true, false);
            break;
        }
        case 2: {
            if (auto num = numFromString<uint8_t>(str)) {
                m_color.b = num.unwrap();
            }
            updateBlue(false, true, false);
            break;
        }
        case 3: {
            if (auto num = numFromString<uint8_t>(str)) {
                m_color.a = num.unwrap();
            }
            updateAlpha(false, true, false);
            break;
        }
        case 4: {
            if (auto num = numFromString<float>(str)) {
                m_width = num.unwrap();
            }
            m_width = std::clamp(m_width, 0.0f, 5.0f);
            updateWidth(false, true, false);
            break;
        }
    }
}

void SBTColorWidget::colorValueChanged(ccColor3B color) {
    if (m_disableDelegate) return;
    m_color.r = color.r;
    updateRed(true, false, false);
    m_color.g = color.g;
    updateGreen(true, false, false);
    m_color.b = color.b;
    updateBlue(true, false, false);
    if (m_callback) m_callback(m_color, m_width);
}

void SBTColorWidget::updatePicker() {
    m_disableDelegate = true;
    m_colorPicker->setColorValue({ m_color.r, m_color.g, m_color.b });
    m_disableDelegate = false;
}

void SBTColorWidget::updateRed(bool color, bool input, bool slider) {
    if (!color) updatePicker();
    if (!input) m_redInput->setString(fmt::to_string(m_color.r));
    if (!slider) m_redSlider->setValue(m_color.r / 255.0f);

    updateNodes(true, false, false, false);

    if (!color && m_callback) m_callback(m_color, m_width);
}

void SBTColorWidget::updateGreen(bool color, bool input, bool slider) {
    if (!color) updatePicker();
    if (!input) m_greenInput->setString(fmt::to_string(m_color.g));
    if (!slider) m_greenSlider->setValue(m_color.g / 255.0f);

    updateNodes(false, true, false, false);

    if (!color && m_callback) m_callback(m_color, m_width);
}

void SBTColorWidget::updateBlue(bool color, bool input, bool slider) {
    if (!color) updatePicker();
    if (!input) m_blueInput->setString(fmt::to_string(m_color.b));
    if (!slider) m_blueSlider->setValue(m_color.b / 255.0f);

    updateNodes(false, false, true, false);

    if (!color && m_callback) m_callback(m_color, m_width);
}

void SBTColorWidget::updateAlpha(bool values, bool input, bool slider) {
    if (!input) m_alphaInput->setString(fmt::to_string(m_color.a));
    if (!slider) m_alphaSlider->setValue(m_color.a / 255.0f);

    for (auto node : m_opacityNodes) {
        node->setOpacity(m_color.a);
    }

    updateNodes(false, false, false, true);

    if (!values && m_callback) m_callback(m_color, m_width);
}

void SBTColorWidget::updateWidth(bool values, bool input, bool slider) {
    if (!input) m_widthInput->setString(fmt::format("{:.02f}", m_width));
    if (!slider) m_widthSlider->setValue(m_width / 5.0f);

    if (!values && m_callback) m_callback(m_color, m_width);
}

void SBTColorWidget::updateNodes(bool red, bool green, bool blue, bool alpha) {
    auto r = m_color.r / 255.0f;
    auto g = m_color.g / 255.0f;
    auto b = m_color.b / 255.0f;
    auto a = m_color.a / 255.0f;

    if (!red) {
        m_redNode->clear();
        auto redWidth = m_redSlider->m_width;
        auto redHeight = m_redSlider->m_height - 0.5f;
        for (float i = 0.5f; i < redWidth; i++) {
            m_redNode->drawSegment({ i, 0.5f }, { i, redHeight }, 1.0f, { a * i / redWidth, a * g, a * b, a });
        }
    }

    if (!green) {
        m_greenNode->clear();
        auto greenWidth = m_greenSlider->m_width;
        auto greenHeight = m_greenSlider->m_height - 0.5f;
        for (float i = 0.5f; i < greenWidth; i++) {
            m_greenNode->drawSegment({ i, 0.5f }, { i, greenHeight }, 1.0f, { a * r, a * i / greenWidth, a * b, a });
        }
    }

    if (!blue) {
        m_blueNode->clear();
        auto blueWidth = m_blueSlider->m_width;
        auto blueHeight = m_blueSlider->m_height - 0.5f;
        for (float i = 0.5f; i < blueWidth; i++) {
            m_blueNode->drawSegment({ i, 0.5f }, { i, blueHeight }, 1.0f, { a * r, a * g, a * i / blueWidth, a });
        }
    }

    if (!alpha) {
        m_alphaNode->clear();
        auto alphaWidth = m_alphaSlider->m_width;
        auto alphaHeight = m_alphaSlider->m_height - 0.5f;
        for (float i = 0.5f; i < alphaWidth; i++) {
            auto alpha = i < 0.5f ? 0 : i / alphaWidth;
            m_alphaNode->drawSegment({ i, 0.5f }, { i, alphaHeight }, 1.0f, { alpha * r, alpha * g, alpha * b, alpha });
        }
    }

    m_widthSprite->setColor({ m_color.r, m_color.g, m_color.b });
    m_widthSprite->setOpacity(m_color.a);
}

void SBTColorWidget::setValues(const ccColor4B& color, float width) {
    m_color = color;
    m_width = width;

    updatePicker();
    updateRed(true, false, false);
    updateGreen(true, false, false);
    updateBlue(true, false, false);
    updateAlpha(true, false, false);
    updateWidth(true, false, false);

    if (m_callback) m_callback(m_color, m_width);
}

void prepareNode(CCNodeRGBA* node, uint8_t alpha, bool show) {
    if (show) node->setOpacity(0);
    node->stopAllActions();
    node->runAction(CCFadeTo::create(0.2f, alpha));
}

void prepareNode(CCLabelBMFont* node, uint8_t alpha, bool show) {
    if (show) node->setOpacity(0);
    node->stopAllActions();
    node->runAction(CCFadeTo::create(0.2f, alpha));
}

void prepareTextInput(TextInput* input, uint8_t alpha, bool show) {
    prepareNode(input->getBGSprite(), alpha * 0.353f, show);
    auto inputNode = input->getInputNode();
    prepareNode(inputNode->m_textLabel, alpha, show);
    prepareNode(inputNode->m_cursor, alpha, show);
    if (show) {
        inputNode->stopAllActions();
        inputNode->runAction(CCSequence::createWithTwoActions(CCDelayTime::create(0.2f), CallFuncExt::create([inputNode] {
            inputNode->setTouchEnabled(true);
        })));
    }
}

void prepareSlider(Slider* slider, uint8_t alpha, bool show) {
    prepareNode(slider->m_groove, alpha, show);
    auto touchLogic = slider->m_touchLogic;
    auto thumb = touchLogic->m_thumb;
    prepareNode(static_cast<CCNodeRGBA*>(thumb->getNormalImage()), alpha, show);
    prepareNode(static_cast<CCNodeRGBA*>(thumb->getSelectedImage()), alpha, show);
    if (show) {
        touchLogic->stopAllActions();
        touchLogic->runAction(CCSequence::createWithTwoActions(CCDelayTime::create(0.2f), CallFuncExt::create([touchLogic] {
            touchLogic->setTouchEnabled(true);
        })));
    }
}

void SBTColorWidget::prepareActions(bool show) {
    if (show) {
        m_colorPicker->stopAllActions();
        m_colorPicker->runAction(CCSequence::createWithTwoActions(CCDelayTime::create(0.2f), CallFuncExt::create([this, show] {
            m_colorPicker->setEnabled(true);
        })));
    }
    uint8_t alpha = show ? m_color.a : 0;
    for (auto node : m_opacityNodes) {
        prepareNode(node, alpha, show);
    }
    if (show) alpha = 255;
    prepareNode(m_redLabel, alpha, show);
    prepareNode(m_greenLabel, alpha, show);
    prepareNode(m_blueLabel, alpha, show);
    prepareNode(m_alphaLabel, alpha, show);
    prepareNode(m_widthLabel, alpha, show);
    prepareTextInput(m_redInput, alpha, show);
    prepareTextInput(m_greenInput, alpha, show);
    prepareTextInput(m_blueInput, alpha, show);
    prepareTextInput(m_alphaInput, alpha, show);
    prepareTextInput(m_widthInput, alpha, show);
    prepareNode(m_redNode, alpha, show);
    prepareNode(m_greenNode, alpha, show);
    prepareNode(m_blueNode, alpha, show);
    prepareNode(m_alphaNode, alpha, show);
    prepareNode(m_widthSprite, alpha, show);
    prepareSlider(m_redSlider, alpha, show);
    prepareSlider(m_greenSlider, alpha, show);
    prepareSlider(m_blueSlider, alpha, show);
    prepareSlider(m_alphaSlider, alpha, show);
    prepareSlider(m_widthSlider, alpha, show);
    if (!show) {
        m_colorPicker->setEnabled(false);
        m_redInput->defocus();
        m_redInput->getInputNode()->setTouchEnabled(false);
        m_redSlider->m_touchLogic->setTouchEnabled(false);
        m_greenInput->defocus();
        m_greenInput->getInputNode()->setTouchEnabled(false);
        m_greenSlider->m_touchLogic->setTouchEnabled(false);
        m_blueInput->defocus();
        m_blueInput->getInputNode()->setTouchEnabled(false);
        m_blueSlider->m_touchLogic->setTouchEnabled(false);
        m_alphaInput->defocus();
        m_alphaInput->getInputNode()->setTouchEnabled(false);
        m_alphaSlider->m_touchLogic->setTouchEnabled(false);
        m_widthInput->defocus();
        m_widthInput->getInputNode()->setTouchEnabled(false);
        m_widthSlider->m_touchLogic->setTouchEnabled(false);
        stopAllActions();
        runAction(CCSequence::createWithTwoActions(CCDelayTime::create(0.2f), CCRemoveSelf::create()));
    }
}

void SBTColorWidget::onEnter() {
    CCLayer::onEnter();
    jasmine::hook::toggle(SmartBPMTrigger::drawNodeHook, true);
}

void SBTColorWidget::onExit() {
    CCLayer::onExit();
    jasmine::hook::toggle(SmartBPMTrigger::drawNodeHook, false);
}
