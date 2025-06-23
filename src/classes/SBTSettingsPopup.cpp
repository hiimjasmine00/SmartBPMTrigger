#include "SBTSettingsPopup.hpp"
#include "../SmartBPMTrigger.hpp"
#include <Geode/binding/ButtonSprite.hpp>
#include <Geode/binding/EditorUI.hpp>
#include <Geode/binding/LevelEditorLayer.hpp>
#include <Geode/binding/SpawnTriggerGameObject.hpp>
#include <Geode/binding/UndoObject.hpp>
#include <Geode/ui/GeodeUI.hpp>
#include <Geode/ui/TextInput.hpp>
#include <Geode/utils/ranges.hpp>

using namespace geode::prelude;

SBTSettingsPopup* SBTSettingsPopup::create(LevelEditorLayer* layer) {
    auto ret = new SBTSettingsPopup();
    if (ret->initAnchored(400.0f, 200.0f, layer)) {
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

    auto mod = Mod::get();

    auto snapLabel = CCLabelBMFont::create("Guideline Snap", "goldFont.fnt");
    snapLabel->setPosition({ 100.0f, 155.0f });
    snapLabel->setScale(0.6f);
    snapLabel->setID("snap-label");
    m_mainLayer->addChild(snapLabel);

    auto distributeToggle = CCMenuItemExt::createTogglerWithStandardSprites(0.75f, [](CCMenuItemToggler* sender) {
        SmartBPMTrigger::setSnapDistribute(!sender->m_toggled);
        sender->m_toggled = !sender->m_toggled;
    });
    distributeToggle->setPosition({ 30.0f, 125.0f });
    distributeToggle->toggle(SmartBPMTrigger::snapDistribute(mod));
    distributeToggle->setID("distribute-toggle");
    m_buttonMenu->addChild(distributeToggle);

    m_listeners.emplace(SmartBPMTrigger::settingListener<"snap-distribute", bool>([distributeToggle](bool value) {
        distributeToggle->toggle(value);
    }, mod));

    auto distributeLabel = CCLabelBMFont::create("Distribute", "bigFont.fnt");
    distributeLabel->setPosition({ 50.0f, 125.0f });
    distributeLabel->setScale(0.3f);
    distributeLabel->setAnchorPoint({ 0.0f, 0.5f });
    distributeLabel->setID("distribute-label");
    m_mainLayer->addChild(distributeLabel);

    auto orangeToggle = CCMenuItemExt::createTogglerWithStandardSprites(0.75f, [](CCMenuItemToggler* sender) {
        SmartBPMTrigger::setSnap(GuidelineType::Orange, !sender->m_toggled);
        sender->m_toggled = !sender->m_toggled;
    });
    orangeToggle->setPosition({ 120.0f, 125.0f });
    orangeToggle->toggle(SmartBPMTrigger::getSnap(GuidelineType::Orange, mod));
    orangeToggle->setID("orange-toggle");
    m_buttonMenu->addChild(orangeToggle);

    m_listeners.emplace(SmartBPMTrigger::settingListener<"snap-orange", bool>([orangeToggle](bool value) {
        orangeToggle->toggle(value);
    }, mod));

    auto orangeLabel = CCLabelBMFont::create("Orange", "bigFont.fnt");
    orangeLabel->setPosition({ 140.0f, 125.0f });
    orangeLabel->setScale(0.4f);
    orangeLabel->setAnchorPoint({ 0.0f, 0.5f });
    orangeLabel->setID("orange-label");
    m_mainLayer->addChild(orangeLabel);

    auto yellowToggle = CCMenuItemExt::createTogglerWithStandardSprites(0.75f, [](CCMenuItemToggler* sender) {
        SmartBPMTrigger::setSnap(GuidelineType::Yellow, !sender->m_toggled);
        sender->m_toggled = !sender->m_toggled;
    });
    yellowToggle->setPosition({ 30.0f, 95.0f });
    yellowToggle->toggle(SmartBPMTrigger::getSnap(GuidelineType::Yellow, mod));
    yellowToggle->setID("yellow-toggle");
    m_buttonMenu->addChild(yellowToggle);

    m_listeners.emplace(SmartBPMTrigger::settingListener<"snap-yellow", bool>([yellowToggle](bool value) {
        yellowToggle->toggle(value);
    }, mod));

    auto yellowLabel = CCLabelBMFont::create("Yellow", "bigFont.fnt");
    yellowLabel->setPosition({ 50.0f, 95.0f });
    yellowLabel->setScale(0.4f);
    yellowLabel->setAnchorPoint({ 0.0f, 0.5f });
    yellowLabel->setID("yellow-label");
    m_mainLayer->addChild(yellowLabel);

    auto greenToggle = CCMenuItemExt::createTogglerWithStandardSprites(0.75f, [](CCMenuItemToggler* sender) {
        SmartBPMTrigger::setSnap(GuidelineType::Green, !sender->m_toggled);
        sender->m_toggled = !sender->m_toggled;
    });
    greenToggle->setPosition({ 120.0f, 95.0f });
    greenToggle->toggle(SmartBPMTrigger::getSnap(GuidelineType::Green, mod));
    greenToggle->setID("green-toggle");
    m_buttonMenu->addChild(greenToggle);

    m_listeners.emplace(SmartBPMTrigger::settingListener<"snap-green", bool>([greenToggle](bool value) {
        greenToggle->toggle(value);
    }, mod));

    auto greenLabel = CCLabelBMFont::create("Green", "bigFont.fnt");
    greenLabel->setPosition({ 140.0f, 95.0f });
    greenLabel->setScale(0.4f);
    greenLabel->setAnchorPoint({ 0.0f, 0.5f });
    greenLabel->setID("green-label");
    m_mainLayer->addChild(greenLabel);

    auto bpmToggle = CCMenuItemExt::createTogglerWithStandardSprites(0.75f, [](CCMenuItemToggler* sender) {
        SmartBPMTrigger::setSnap(GuidelineType::BPM, !sender->m_toggled);
        sender->m_toggled = !sender->m_toggled;
    });
    bpmToggle->setPosition({ 30.0f, 65.0f });
    bpmToggle->toggle(SmartBPMTrigger::getSnap(GuidelineType::BPM, mod));
    bpmToggle->setID("bpm-toggle");
    m_buttonMenu->addChild(bpmToggle);

    m_listeners.emplace(SmartBPMTrigger::settingListener<"snap-bpm", bool>([bpmToggle](bool value) {
        bpmToggle->toggle(value);
    }, mod));

    auto bpmLabel = CCLabelBMFont::create("BPM", "bigFont.fnt");
    bpmLabel->setPosition({ 50.0f, 65.0f });
    bpmLabel->setScale(0.5f);
    bpmLabel->setAnchorPoint({ 0.0f, 0.5f });
    bpmLabel->setID("bpm-label");
    m_mainLayer->addChild(bpmLabel);

    auto bpbToggle = CCMenuItemExt::createTogglerWithStandardSprites(0.75f, [](CCMenuItemToggler* sender) {
        SmartBPMTrigger::setSnap(GuidelineType::BPB, !sender->m_toggled);
        sender->m_toggled = !sender->m_toggled;
    });
    bpbToggle->setPosition({ 120.0f, 65.0f });
    bpbToggle->toggle(SmartBPMTrigger::getSnap(GuidelineType::BPB, mod));
    bpbToggle->setID("bpb-toggle");
    m_buttonMenu->addChild(bpbToggle);

    m_listeners.emplace(SmartBPMTrigger::settingListener<"snap-bpb", bool>([bpbToggle](bool value) {
        bpbToggle->toggle(value);
    }, mod));

    auto bpbLabel = CCLabelBMFont::create("BPB", "bigFont.fnt");
    bpbLabel->setPosition({ 140.0f, 65.0f });
    bpbLabel->setScale(0.5f);
    bpbLabel->setAnchorPoint({ 0.0f, 0.5f });
    bpbLabel->setID("bpb-label");
    m_mainLayer->addChild(bpbLabel);

    auto loopLabel = CCLabelBMFont::create("BPM Spawn Loop", "goldFont.fnt");
    loopLabel->setPosition({ 300.0f, 155.0f });
    loopLabel->setScale(0.6f);
    loopLabel->setID("loop-label");
    m_mainLayer->addChild(loopLabel);

    auto spawnBPM = SmartBPMTrigger::spawnBPM(mod);

    auto loopBPMInput = TextInput::create(70.0f, "BPM");
    loopBPMInput->setPosition({ 300.0f, 100.0f });
    loopBPMInput->setString(fmt::to_string(spawnBPM));
    loopBPMInput->setCallback([loopBPMInput](const std::string& str) {
        if (auto value = numFromString<int>(str)) SmartBPMTrigger::setSpawnBPM(value.unwrap());
        loopBPMInput->setString(str);
    });
    loopBPMInput->setMaxCharCount(3);
    loopBPMInput->setCommonFilter(CommonFilter::Uint);
    loopBPMInput->setID("loop-bpm-input");
    m_mainLayer->addChild(loopBPMInput);

    auto loopBPMLabel = CCLabelBMFont::create(fmt::format("{} BPM", spawnBPM).c_str(), "bigFont.fnt");
    loopBPMLabel->setPosition({ 300.0f, 80.0f });
    loopBPMLabel->setScale(0.3f);
    loopBPMLabel->setID("loop-bpm-label");
    m_mainLayer->addChild(loopBPMLabel);

    m_listeners.emplace(SmartBPMTrigger::settingListener<"spawn-bpm", int>([loopBPMInput, loopBPMLabel](int value) {
        loopBPMInput->setString(fmt::to_string(value));
        loopBPMLabel->setString(fmt::format("{} BPM", value).c_str());
    }, mod));

    auto bpmSmallLeftButton = CCMenuItemExt::createSpriteExtraWithFrameName("edit_leftBtn_001.png", 1.0f, [](auto) {
        auto mod = Mod::get();
        auto spawnBPM = SmartBPMTrigger::spawnBPM(mod);
        if (spawnBPM > 1) SmartBPMTrigger::setSpawnBPM(spawnBPM - 1, mod);
    });
    bpmSmallLeftButton->setPosition({ 250.0f, 100.0f });
    bpmSmallLeftButton->setID("bpm-small-left-button");
    m_buttonMenu->addChild(bpmSmallLeftButton);

    auto bpmSmallRightButton = CCMenuItemExt::createSpriteExtraWithFrameName("edit_rightBtn_001.png", 1.0f, [](auto) {
        auto mod = Mod::get();
        auto spawnBPM = SmartBPMTrigger::spawnBPM(mod);
        if (spawnBPM < 999) SmartBPMTrigger::setSpawnBPM(spawnBPM + 1, mod);
    });
    bpmSmallRightButton->setPosition({ 350.0f, 100.0f });
    bpmSmallRightButton->setID("bpm-small-right-button");
    m_buttonMenu->addChild(bpmSmallRightButton);

    auto bpmBigLeftButton = CCMenuItemExt::createSpriteExtraWithFrameName("edit_leftBtn2_001.png", 0.8f, [](auto) {
        auto mod = Mod::get();
        auto spawnBPM = SmartBPMTrigger::spawnBPM(mod);
        if (spawnBPM > 1) SmartBPMTrigger::setSpawnBPM(std::max(spawnBPM - 10, 1), mod);
    });
    bpmBigLeftButton->setPosition({ 230.0f, 100.0f });
    bpmBigLeftButton->setID("bpm-big-left-button");
    m_buttonMenu->addChild(bpmBigLeftButton);

    auto bpmBigRightButton = CCMenuItemExt::createSpriteExtraWithFrameName("edit_rightBtn2_001.png", 0.8f, [](auto) {
        auto mod = Mod::get();
        auto spawnBPM = SmartBPMTrigger::spawnBPM(mod);
        if (spawnBPM < 999) SmartBPMTrigger::setSpawnBPM(std::min(spawnBPM + 10, 999), mod);
    });
    bpmBigRightButton->setPosition({ 370.0f, 100.0f });
    bpmBigRightButton->setID("bpm-big-right-button");
    m_buttonMenu->addChild(bpmBigRightButton);

    auto ui = layer->m_editorUI;
    auto objectSelected = ui->m_selectedObject || (ui->m_selectedObjects && ui->m_selectedObjects->count() > 0);
    auto opacity = 127 + (objectSelected * 128);

    auto snapSprite = ButtonSprite::create("Snap", 0.8f);
    snapSprite->m_BGSprite->setOpacity(opacity);
    snapSprite->m_label->setOpacity(opacity);
    auto snapButton = CCMenuItemExt::createSpriteExtra(snapSprite, [this, layer](auto) {
        guidelineSnap(layer);
        onClose(nullptr);
    });
    snapButton->setPosition({ 100.0f, 25.0f });
    snapButton->setEnabled(objectSelected);
    snapButton->setID("snap-button");
    m_buttonMenu->addChild(snapButton);

    auto settingsButton = CCMenuItemExt::createSpriteExtra(ButtonSprite::create("Settings", 0.8f), [](auto) {
        openSettingsPopup(Mod::get());
    });
    settingsButton->setPosition({ 195.0f, 25.0f });
    settingsButton->setID("settings-button");
    m_buttonMenu->addChild(settingsButton);

    auto createSprite = ButtonSprite::create("Create", 0.8f);
    createSprite->m_BGSprite->setOpacity(opacity);
    createSprite->m_label->setOpacity(opacity);
    auto createButton = CCMenuItemExt::createSpriteExtra(createSprite, [this, layer](auto) {
        createLoop(layer);
        onClose(nullptr);
    });
    createButton->setPosition({ 300.0f, 25.0f });
    createButton->setEnabled(objectSelected);
    createButton->setID("create-button");
    m_buttonMenu->addChild(createButton);

    return true;
}

void SBTSettingsPopup::guidelineSnap(LevelEditorLayer* layer) {
    auto mod = Mod::get();
    auto guidelines = SmartBPMTrigger::getGuidelines(layer->m_drawGridLayer, mod);
    if (guidelines.empty()) return;

    auto ui = layer->m_editorUI;
    if (ui->m_selectedObject) {
        layer->addToUndoList(UndoObject::create(ui->m_selectedObject, UndoCommand::Transform), false);
        auto x = ui->m_selectedObject->getPositionX();
        std::ranges::sort(guidelines, [x](float a, float b) { return std::abs(a - x) < std::abs(b - x); });
        ui->moveObject(ui->m_selectedObject, { guidelines[0] - x, 0.0f });
    }
    else if (ui->m_selectedObjects && ui->m_selectedObjects->count() > 0) {
        ui->createUndoObject(UndoCommand::Transform, false);
        if (SmartBPMTrigger::snapDistribute(mod)) {
            for (auto object : CCArrayExt<GameObject*>(ui->m_selectedObjects)) {
                auto x = object->getPositionX();
                std::ranges::sort(guidelines, [x](float a, float b) { return std::abs(a - x) < std::abs(b - x); });
                ui->moveObject(object, { guidelines[0] - x, 0.0f });
            }
        }
        else {
            auto x = ui->getGroupCenter(ui->m_selectedObjects, false).x;
            std::ranges::sort(guidelines, [x](float a, float b) { return std::abs(a - x) < std::abs(b - x); });
            auto dx = guidelines[0] - x;
            for (auto object : CCArrayExt<GameObject*>(ui->m_selectedObjects)) {
                ui->moveObject(object, { dx, 0.0f });
            }
        }
    }
}

void SBTSettingsPopup::createLoop(LevelEditorLayer* layer) {
    auto spawnObjects = CCArray::create();
    auto ui = layer->m_editorUI;
    if (ui->m_selectedObject) {
        if (ui->m_selectedObject->isSpawnableTrigger()) spawnObjects->addObject(ui->m_selectedObject);
    }
    else if (ui->m_selectedObjects && ui->m_selectedObjects->count() > 0) {
        for (auto object : CCArrayExt<GameObject*>(ui->m_selectedObjects)) {
            if (object->isSpawnableTrigger()) spawnObjects->addObject(object);
        }
    }

    auto spawnObjectsBegin = reinterpret_cast<GameObject**>(spawnObjects->data->arr);
    std::sort(spawnObjectsBegin, spawnObjectsBegin + spawnObjects->data->num, [](GameObject* a, GameObject* b) {
        return a->getRealPosition().x < b->getRealPosition().x;
    });

    auto spawnArray = CCArray::create();
    auto x = 0.0f;
    auto y = 0.0f;
    for (auto object : CCArrayExt<EffectGameObject*>(spawnObjects)) {
        object->m_isSpawnTriggered = true;
        object->m_isMultiTriggered = true;
        auto realPosition = object->getRealPosition();
        if (realPosition.x < x || x == 0.0f) x = realPosition.x;
        if (realPosition.y > y || y == 0.0f) y = realPosition.y;
        if (spawnArray->count() > 0) {
            auto lastArray = static_cast<CCArray*>(spawnArray->lastObject());
            auto lastRealPosition = static_cast<GameObject*>(lastArray->objectAtIndex(0))->getRealPosition();
            if (realPosition.x - lastRealPosition.x > 0.1f) {
                auto innerArray = CCArray::create();
                innerArray->addObject(object);
                spawnArray->addObject(innerArray);
            }
            else lastArray->addObject(object);
        }
        else {
            auto innerArray = CCArray::create();
            innerArray->addObject(object);
            spawnArray->addObject(innerArray);
        }
    }

    auto groupID = layer->getNextFreeGroupID(nullptr);
    auto firstID = groupID;
    auto mainTrigger = static_cast<EffectGameObject*>(ui->createObject(1268, { x, y + 90.0f }));
    mainTrigger->m_targetGroupID = std::clamp(groupID, 0, 9999);
    layer->removeSpecial(mainTrigger);
    layer->addSpecial(mainTrigger);

    auto time = layer->timeForPos({ x, 0.0f }, 0, 0, false, 0);
    auto delay = 0.0f;
    auto sum = 0.0f;
    auto oldID = groupID;
    auto lastIndex = spawnArray->count() - 1;
    for (int i = 0; i < spawnArray->count(); i++) {
        auto innerArray = static_cast<CCArray*>(spawnArray->objectAtIndex(i));
        auto diff = delay;
        if (i < lastIndex) {
            diff = layer->timeForPos({
                static_cast<GameObject*>(static_cast<CCArray*>(spawnArray->objectAtIndex(i + 1))->objectAtIndex(0))->getRealPosition().x,
                0.0f
            }, 0, 0, false, 0) - time;
        }
        groupID = oldID;
        for (int j = 0; j < innerArray->count(); j++) {
            auto object = static_cast<GameObject*>(innerArray->objectAtIndex(j));
            if (object->addToGroup(oldID) == 1) layer->addToGroup(object, oldID, false);
            if (j == 0) {
                delay = diff;
                groupID = firstID;
                if (i != lastIndex) {
                    groupID = layer->getNextFreeGroupID(nullptr);
                    delay = diff - sum;
                }
                else delay = std::max(60.0f / SmartBPMTrigger::spawnBPM() - sum, 0.0f);
                auto spawnTrigger = static_cast<SpawnTriggerGameObject*>(ui->createObject(1268, { object->getRealPosition().x, y + 60.0f }));
                if (spawnTrigger->addToGroup(oldID) == 1) layer->addToGroup(spawnTrigger, oldID, false);
                spawnTrigger->m_spawnDelay = delay;
                spawnTrigger->m_isSpawnTriggered = true;
                spawnTrigger->m_isMultiTriggered = true;
                spawnTrigger->m_targetGroupID = std::clamp(groupID, 0, 9999);
                layer->removeSpecial(spawnTrigger);
                layer->addSpecial(spawnTrigger);
                sum += delay;
            }
        }
        oldID = groupID;
    }

    layer->dirtifyTriggers();
}
