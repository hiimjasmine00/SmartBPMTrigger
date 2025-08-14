#include "SBTSettingsPopup.hpp"
#include "../SmartBPMTrigger.hpp"
#include <Geode/binding/ButtonSprite.hpp>
#include <Geode/binding/EditorUI.hpp>
#include <Geode/binding/LevelEditorLayer.hpp>
#include <Geode/binding/SpawnTriggerGameObject.hpp>
#include <Geode/binding/Slider.hpp>
#include <Geode/binding/UndoObject.hpp>
#include <Geode/ui/TextInput.hpp>

using namespace geode::prelude;

SBTSettingsPopup* SBTSettingsPopup::create(LevelEditorLayer* layer) {
    auto ret = new SBTSettingsPopup();
    if (ret->initAnchored(400.0f, 290.0f, layer)) {
        ret->autorelease();
        return ret;
    }
    delete ret;
    return nullptr;
}

bool SBTSettingsPopup::setup(LevelEditorLayer* layer) {
    setID("SBTSettingsPopup");
    setTitle("Smart BPM Trigger");
    m_title->setID("smart-bpm-trigger-title");
    m_mainLayer->setID("main-layer");
    m_buttonMenu->setID("button-menu");
    m_bgSprite->setID("background");
    m_closeBtn->setID("close-button");
    m_noElasticity = true;

    m_colorWidget = SBTColorWidget::create({ 0, 0, 0, 0 }, 0.0f, [this](const ccColor4B& color, float width) {
        m_barSprite->setColor({ color.r, color.g, color.b });
        m_barSprite->setOpacity(color.a);
        m_barSprite->setScaleX(width / 5.0f);
        m_barButtonSprite->setColor({ color.r, color.g, color.b });
        m_barButtonSprite->setOpacity(color.a);
        m_barButtonSprite->setScaleX(width / 5.0f);
        m_colorSetting->setValue(color);
        m_widthSetting->setValue(width);
    });
    m_colorWidget->setPosition({ 290.0f, 130.0f });
    m_colorWidget->setScale(0.9f);
    m_colorWidget->setID("color-widget");

    auto squareTexture = SmartBPMTrigger::getTextureCache()->addImage(GEODE_MOD_ID "/square.png", false);
    auto squareSize = squareTexture ? squareTexture->m_tContentSize : CCSize { 2.0f, 2.0f };
    squareSize /= SmartBPMTrigger::getDirector()->getContentScaleFactor();
    CCRect squareRect = { { 0.0f, 0.0f }, squareSize };

    m_barSprite = CCSprite::createWithTexture(squareTexture, squareRect);
    m_barSprite->setPosition({ 90.0f, 135.0f });
    m_barSprite->setScaleY(250.0f / squareSize.height);
    m_barSprite->setID("bar-sprite");

    auto settingsLabel = CCLabelBMFont::create("Guideline Settings", "goldFont.fnt");
    settingsLabel->setPosition({ 200.0f, 250.0f });
    settingsLabel->setScale(0.6f);
    settingsLabel->setID("settings-label");
    m_mainLayer->addChild(settingsLabel);

    auto settingsMenu = CCMenu::create();
    settingsMenu->setPosition({ 200.0f, 215.0f });
    settingsMenu->setContentSize({ 400.0f, 50.0f });
    settingsMenu->setLayout(RowLayout::create()->setGap(45.0f)->setAutoScale(false));
    settingsMenu->setID("settings-menu");
    m_mainLayer->addChild(settingsMenu);

    constexpr std::array guidelineSettings = {
        std::make_tuple("orange-color", "orange-width", "Orange", "orange"),
        std::make_tuple("yellow-color", "yellow-width", "Yellow", "yellow"),
        std::make_tuple("green-color", "green-width", "Green", "green"),
        std::make_tuple("beats-per-minute-color", "beats-per-minute-width", "BPM", "bpm"),
        std::make_tuple("beats-per-bar-color", "beats-per-bar-width", "BPB", "bpb")
    };

    for (auto& [colorKey, widthKey, label, shortName] : guidelineSettings) {
        auto guidelineMenu = CCMenu::create();
        guidelineMenu->setContentSize({ squareSize.width, 50.0f });
        guidelineMenu->setID(fmt::format("{}-menu", shortName));
        settingsMenu->addChild(guidelineMenu);

        auto colorSetting = SmartBPMTrigger::getSetting<ccColor4B>(colorKey);
        auto widthSetting = SmartBPMTrigger::getSetting<float>(widthKey);

        auto barSprite = CCSprite::createWithTexture(squareTexture, squareRect);
        barSprite->setScaleX(widthSetting->getValue() / 5.0f);
        barSprite->setScaleY(40.0f / squareSize.height);
        auto barColor = colorSetting->getValue();
        barSprite->setColor({ barColor.r, barColor.g, barColor.b });
        barSprite->setOpacity(barColor.a);

        auto barButton = CCMenuItemExt::createSpriteExtra(barSprite, [this, barSprite, colorSetting, widthSetting, label](auto) {
            setTitle(fmt::format("{} Guidelines", label));
            showPicker(barSprite, colorSetting, widthSetting);
        });
        barButton->setPosition({ squareSize.width / 2.0f, 20.0f });
        barButton->setContentSize({ squareSize.width, 40.0f });
        barSprite->setPosition(barButton->getPosition());
        barButton->setID(fmt::format("{}-button", shortName));
        guidelineMenu->addChild(barButton);

        auto guidelineLabel = CCLabelBMFont::create(label, "bigFont.fnt");
        guidelineLabel->setPosition(barSprite->getPosition() + CCPoint { 0.0f, 27.0f });
        guidelineLabel->setScale(0.3f);
        guidelineLabel->setID(fmt::format("{}-label", shortName));
        guidelineMenu->addChild(guidelineLabel);
    }

    settingsMenu->updateLayout();

    auto snapLabel = CCLabelBMFont::create("Guideline Snap", "goldFont.fnt");
    snapLabel->setPosition({ 200.0f, 180.0f });
    snapLabel->setScale(0.6f);
    snapLabel->setID("snap-label");
    m_mainLayer->addChild(snapLabel);

    auto snapMenu = CCMenu::create();
    snapMenu->setPosition({ 200.0f, 145.0f });
    snapMenu->setContentSize({ 400.0f, 35.0f });
    snapMenu->setLayout(RowLayout::create()->setGap(20.0f));
    snapMenu->setID("snap-toggles-menu");
    m_mainLayer->addChild(snapMenu);

    constexpr std::array snapToggles = {
        std::make_pair("snap-distribute", "Distribute"),
        std::make_pair("snap-orange", "Orange"),
        std::make_pair("snap-yellow", "Yellow"),
        std::make_pair("snap-green", "Green"),
        std::make_pair("snap-bpm", "BPM"),
        std::make_pair("snap-bpb", "BPB")
    };

    auto& snapSize = SmartBPMTrigger::getSpriteFrameCache()->spriteFrameByName("GJ_checkOn_001.png")->getOriginalSize();
    for (auto& [settingName, label] : snapToggles) {
        auto snapNode = CCMenu::create();
        snapNode->setContentSize({ snapSize.width, 35.0f });
        snapNode->setID(fmt::format("{}-menu", settingName));
        snapMenu->addChild(snapNode);

        auto snapSetting = SmartBPMTrigger::getSetting<bool>(settingName);
        auto snapToggle = CCMenuItemExt::createTogglerWithStandardSprites(0.75f, [snapSetting](CCMenuItemToggler* sender) {
            snapSetting->setValue(!sender->m_toggled);
            sender->m_toggled = !sender->m_toggled;
        });
        snapToggle->setPosition(snapSize / 2.0f);
        snapToggle->toggle(snapSetting->getValue());
        snapToggle->setID(fmt::format("{}-toggle", settingName));
        snapNode->addChild(snapToggle);

        auto snapLabel = CCLabelBMFont::create(label, "bigFont.fnt");
        snapLabel->setPosition(snapToggle->getPosition() + CCPoint { 0.0f, snapSize.height / 2.0f + 2.0f });
        snapLabel->setScale(0.3f);
        snapLabel->setID(fmt::format("{}-label", settingName));
        snapNode->addChild(snapLabel);
    }

    snapMenu->updateLayout();

    auto loopLabel = CCLabelBMFont::create("BPM Spawn Loop", "goldFont.fnt");
    loopLabel->setPosition({ 200.0f, 120.0f });
    loopLabel->setScale(0.6f);
    loopLabel->setID("loop-label");
    m_mainLayer->addChild(loopLabel);

    auto spawnBPM = SmartBPMTrigger::getSetting<double>("spawn-bpm");

    auto loopBPMSlider = Slider::create(nullptr, nullptr, 0.8f);
    loopBPMSlider->setPosition({ 200.0f, 60.0f });
    loopBPMSlider->setValue(spawnBPM->getValue() / 1000.0);
    loopBPMSlider->setID("loop-bpm-slider");
    m_mainLayer->addChild(loopBPMSlider);

    auto loopBPMInput = TextInput::create(70.0f, "BPM");
    loopBPMInput->setPosition({ 200.0f, 90.0f });
    loopBPMInput->setString(fmt::format("{:.3f}", spawnBPM->getValue()));
    loopBPMInput->setCallback([loopBPMInput, loopBPMSlider, spawnBPM](const std::string& str) {
        if (auto value = numFromString<double>(str)) {
            spawnBPM->setValue(round(value.unwrap() * 1000.0) / 1000.0);
            loopBPMSlider->setValue(spawnBPM->getValue() / 1000.0);
        }
    });
    loopBPMInput->setMaxCharCount(7);
    loopBPMInput->setFilter(".0123456789");
    loopBPMInput->setID("loop-bpm-input");
    m_mainLayer->addChild(loopBPMInput);

    CCMenuItemExt::assignCallback<SliderThumb>(loopBPMSlider->m_touchLogic->m_thumb, [loopBPMInput, spawnBPM](SliderThumb* thumb) {
        auto value = round(thumb->getValue() * 1000000.0) / 1000.0;
        spawnBPM->setValue(value);
        loopBPMInput->setString(value == 1000.0 ? "1000.00" : fmt::format("{:.3f}", value));
    });

    auto ui = layer->m_editorUI;
    auto objectSelected = ui->m_selectedObject || (ui->m_selectedObjects && ui->m_selectedObjects->count() > 0);

    auto bottomMenu = CCMenu::create();
    bottomMenu->setPosition({ 200.0f, 25.0f });
    bottomMenu->setContentSize({ 400.0f, 30.0f });
    bottomMenu->setEnabled(objectSelected);
    bottomMenu->setLayout(RowLayout::create()->setGap(20.0f));
    bottomMenu->setID("bottom-menu");
    m_mainLayer->addChild(bottomMenu);

    auto snapSprite = ButtonSprite::create("Snap Object", 0.8f);
    snapSprite->setCascadeOpacityEnabled(true);
    auto snapButton = CCMenuItemExt::createSpriteExtra(snapSprite, [this, layer](auto) {
        guidelineSnap(layer);
        onClose(nullptr);
    });
    snapButton->setID("snap-button");
    bottomMenu->addChild(snapButton);

    auto createSprite = ButtonSprite::create("Create Loop", 0.8f);
    createSprite->setCascadeOpacityEnabled(true);
    auto createButton = CCMenuItemExt::createSpriteExtra(createSprite, [this, layer](auto) {
        createLoop(layer);
        onClose(nullptr);
    });
    createButton->setID("create-button");
    bottomMenu->addChild(createButton);

    bottomMenu->setOpacity(127 + (objectSelected * 128));
    bottomMenu->updateLayout();

    m_otherNodes = CCArray::create();
    m_otherNodes->addObjectsFromArray(m_mainLayer->getChildren());
    m_otherNodes->removeObject(m_buttonMenu);
    m_otherNodes->removeObject(m_title);
    m_otherNodes->removeObject(m_bgSprite);;

    return true;
}

CCPoint getDifference(float x) {
    auto closest = SmartBPMTrigger::guidelines[0];
    for (auto guideline : SmartBPMTrigger::guidelines) {
        if (abs(guideline - x) < abs(closest - x)) closest = guideline;
    }
    return { closest - x, 0.0f };
}

void SBTSettingsPopup::guidelineSnap(LevelEditorLayer* layer) {
    if (SmartBPMTrigger::guidelines.empty()) return;

    auto ui = layer->m_editorUI;
    if (ui->m_selectedObject) {
        layer->addToUndoList(UndoObject::create(ui->m_selectedObject, UndoCommand::Transform), false);
        ui->moveObject(ui->m_selectedObject, getDifference(ui->m_selectedObject->getPositionX()));
    }
    else if (ui->m_selectedObjects && ui->m_selectedObjects->count() > 0) {
        ui->createUndoObject(UndoCommand::Transform, false);
        if (SmartBPMTrigger::get<bool>("snap-distribute")) {
            for (auto object : CCArrayExt<GameObject*>(ui->m_selectedObjects)) {
                ui->moveObject(object, getDifference(object->getPositionX()));
            }
        }
        else {
            auto diff = getDifference(ui->getGroupCenter(ui->m_selectedObjects, false).x);
            for (auto object : CCArrayExt<GameObject*>(ui->m_selectedObjects)) {
                ui->moveObject(object, diff);
            }
        }
    }
}

void SBTSettingsPopup::createLoop(LevelEditorLayer* layer) {
    std::vector<EffectGameObject*> spawnObjects;
    auto ui = layer->m_editorUI;
    if (ui->m_selectedObject) {
        if (ui->m_selectedObject->isSpawnableTrigger()) spawnObjects.push_back(static_cast<EffectGameObject*>(ui->m_selectedObject));
    }
    else if (ui->m_selectedObjects && ui->m_selectedObjects->count() > 0) {
        for (auto object : CCArrayExt<EffectGameObject*>(ui->m_selectedObjects)) {
            if (object->isSpawnableTrigger()) spawnObjects.push_back(object);
        }
    }

    std::ranges::sort(spawnObjects, [](EffectGameObject* a, EffectGameObject* b) {
        return a->getRealPosition().x < b->getRealPosition().x;
    });

    std::vector<std::vector<EffectGameObject*>> spawnArray;
    auto x = 0.0f;
    auto y = 0.0f;
    for (auto object : spawnObjects) {
        object->m_isSpawnTriggered = true;
        object->m_isMultiTriggered = true;
        auto realPosition = object->getRealPosition();
        if (realPosition.x < x || x == 0.0f) x = realPosition.x;
        if (realPosition.y > y || y == 0.0f) y = realPosition.y;
        if (spawnArray.empty()) spawnArray.push_back({ object });
        else {
            auto& lastArray = spawnArray.back();
            if (realPosition.x - lastArray.front()->getRealPosition().x > 0.1f) spawnArray.push_back({ object });
            else lastArray.push_back(object);
        }
    }

    auto groupID = layer->getNextFreeGroupID(nullptr);
    auto firstID = groupID;
    auto mainTrigger = static_cast<EffectGameObject*>(ui->createObject(1268, { x, y + 90.0f }));
    mainTrigger->setTargetID(groupID);
    layer->refreshSpecial(mainTrigger);

    auto time = layer->timeForPos({ x, 0.0f }, 0, 0, false, 0);
    auto delay = 0.0f;
    auto sum = 0.0f;
    auto oldID = groupID;
    auto lastIndex = spawnArray.size() - 1;
    auto spawnBPM = SmartBPMTrigger::get<float>("spawn-bpm");
    for (int i = 0; i < spawnArray.size(); i++) {
        auto& innerArray = spawnArray[i];
        auto diff = i < lastIndex ? layer->timeForPos({ spawnArray[i + 1].front()->getRealPosition().x, 0.0f }, 0, 0, false, 0) - time : delay;
        groupID = oldID;
        for (int j = 0; j < innerArray.size(); j++) {
            auto object = innerArray[j];
            layer->addObjectToGroup(object, oldID);
            if (j == 0) {
                delay = i != lastIndex ? diff - sum : std::max(60.0f / spawnBPM - sum, 0.0f);
                groupID = i != lastIndex ? layer->getNextFreeGroupID(nullptr) : firstID;
                auto spawnTrigger = static_cast<SpawnTriggerGameObject*>(ui->createObject(1268, { object->getRealPosition().x, y + 60.0f }));
                layer->addObjectToGroup(spawnTrigger, oldID);
                spawnTrigger->m_spawnDelay = delay;
                spawnTrigger->m_isSpawnTriggered = true;
                spawnTrigger->m_isMultiTriggered = true;
                spawnTrigger->setTargetID(groupID);
                layer->refreshSpecial(spawnTrigger);
                sum += delay;
            }
        }
        oldID = groupID;
    }

    layer->dirtifyTriggers();
}

void SBTSettingsPopup::showPicker(CCSprite* barSprite, Color4BSettingV3* colorSetting, FloatSettingV3* widthSetting) {
    m_barButtonSprite = barSprite;
    m_colorSetting = colorSetting;
    m_widthSetting = widthSetting;
    m_colorWidget->setValues(colorSetting->getValue(), widthSetting->getValue());
    m_mainLayer->addChild(m_colorWidget);
    m_mainLayer->addChild(m_barSprite);
    for (auto node : CCArrayExt<CCNode*>(m_otherNodes)) {
        node->setVisible(false);
    }
    auto normalImage = static_cast<CCSprite*>(m_closeBtn->getNormalImage());
    normalImage->setDisplayFrame(SmartBPMTrigger::getSpriteFrameCache()->spriteFrameByName("GJ_backBtn_001.png"));
    normalImage->setScale(0.74f);
}

void SBTSettingsPopup::onClose(CCObject* sender) {
    if (m_barButtonSprite && m_colorSetting && m_widthSetting) {
        m_barButtonSprite = nullptr;
        m_colorSetting = nullptr;
        m_widthSetting = nullptr;
        m_colorWidget->removeFromParent();
        m_barSprite->removeFromParent();
        for (auto node : CCArrayExt<CCNode*>(m_otherNodes)) {
            node->setVisible(true);
        }
        auto normalImage = static_cast<CCSprite*>(m_closeBtn->getNormalImage());
        normalImage->setDisplayFrame(SmartBPMTrigger::getSpriteFrameCache()->spriteFrameByName("GJ_closeBtn_001.png"));
        normalImage->setScale(0.8f);
        setTitle("Smart BPM Trigger");
    }
    else Popup::onClose(sender);
}
