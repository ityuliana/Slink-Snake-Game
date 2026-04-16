#include "MainMenuScene.h"
#include "ui/CocosGUI.h"
#include "ui/UIEditBox/UIEditBox.h"
#include "ChangeColorScene.h"
#include "GameScene.h"
USING_NS_CC;
Scene* MainMenuScene::createScene()
{
	auto scene = Scene::create();
	auto layer = MainMenuScene::create();
	scene->addChild(layer);
	return scene;
}

bool MainMenuScene::init()
{
	if (!Layer::init()) return false;
	auto vs = Director::getInstance()->getWinSize();
	Vec2 center = vs / 2;
	
	auto bg = LayerColor::create(Color4B(23, 28, 34, 255));
	this->addChild(bg, -1);

	auto logo = Sprite::create("logo.png");
	logo->setScale(0.8f);
	logo->setPosition(Vec2(vs.width * 0.5f, vs.height * 0.8f));
	this->addChild(logo);


	finalLbl = Label::createWithTTF("", "fonts/TitilliumWeb-Bold.ttf", 20);
	finalLbl->setColor(Color3B::WHITE);
	finalLbl->setPosition(Vec2(vs.width * 0.5f, vs.height * 0.62f));
	this->addChild(finalLbl, 20);

	hint = Label::createWithTTF("Eat to grow longer!", "fonts/TitilliumWeb-Regular.ttf", 20);
	hint->setColor(Color3B(147, 112, 219));
	hint->setOpacity(0);
	hint->setPosition(Vec2(vs.width * 0.5f, vs.height * 0.67f));
	this->addChild(hint, 20);

	hint2 = Label::createWithTTF("When longer, hold mouse for a speed boost!",
		"fonts/Exo2-VariableFont_wght.ttf", 20);
	hint2->setColor(Color3B(147, 112, 219));
	hint2->setOpacity(0);
	hint2->setPosition(Vec2(vs.width * 0.5f, vs.height * 0.63f));
	this->addChild(hint2, 20);
	
	UserDefault::getInstance()->flush();

	btn_Play();
	btn_Change();
	lineEdit();
	btn_Exit();
	return true;
}

void MainMenuScene::btn_Play() {
	auto vs = Director::getInstance()->getWinSize();
	Vec2 center = vs / 2;

	UserDefault::getInstance()->setIntegerForKey("finalLength", 0);

	auto btn = ui::Button::create("btn_normal.png", "btn_pressed.png");
	btn->setScale(0.55f);
	btn->setPosition(Vec2(vs.width * 0.5f, vs.height * 0.3f));
	btn->addClickEventListener([=](Ref* sender) {
		if (nickField && !nickField->getString().empty()) {
			UserDefault::getInstance()->setIntegerForKey("finalLength", 0);
			UserDefault::getInstance()->setStringForKey("nickname",
				nickField->getString());
		}
		else {
			UserDefault::getInstance()->setStringForKey("nickname", "Player");
		}
		UserDefault::getInstance()->flush();
		auto scene = GameScene::createScene();
		Director::getInstance()->pushScene(scene);
		});
	this->addChild(btn);
}

void MainMenuScene::btn_Change() {
	auto vs = Director::getInstance()->getWinSize();
	Vec2 center = vs / 2;
	auto btn = ui::Button::create();
	btn->setTitleText("Change Skin");
	btn->setTitleFontName("fonts/Exo2-VariableFont_wght.ttf");
	btn->setTitleFontSize(20);
	btn->setColor(Color3B(60, 179, 113));
	btn->setPosition(Vec2(vs.width * 0.05f, vs.height * 0.03f));
	btn->addClickEventListener([=](Ref* sender) {
		auto scene = ChangeColorScene::createScene();
		Director::getInstance()->pushScene(scene); });
	this->addChild(btn);
}

void MainMenuScene::lineEdit() {
	auto visibleSize = Director::getInstance()->getWinSize();
	auto bgSize = Size(300, 60);

	auto bg = Sprite::create("input_normal.png");
	bg->setScale(0.8f);
	bg->setPosition(Vec2(visibleSize.width * 0.5f, visibleSize.height * 0.5f));
	this->addChild(bg);

	auto layout = ui::Layout::create();
	layout->setContentSize(bgSize);
	layout->setClippingEnabled(true);
	layout->setPosition(Vec2(visibleSize.width * 0.5f - bgSize.width / 2,
		visibleSize.height * 0.5f - bgSize.height / 2));
	this->addChild(layout, 1);

	nickField = ui::TextField::create("Nickname", "fonts/Nunito-Regular.ttf", 22);
	nickField->setPlaceHolderColor(Color3B(180, 170, 220));
	nickField->setTextColor(Color4B::WHITE);
	nickField->setMaxLength(24);
	nickField->setMaxLengthEnabled(true);
	nickField->setPosition(Vec2(bgSize.width / 2, bgSize.height / 2));
	layout->addChild(nickField);

	auto mouseListener = EventListenerMouse::create();
	mouseListener->onMouseDown = [=](Event* event) {
		auto mouseEvent = dynamic_cast<EventMouse*>(event);
		Vec2 clickPos = Vec2(
			mouseEvent->getCursorX(),
			visibleSize.height - mouseEvent->getCursorY()
		);
		if (bg->getBoundingBox().containsPoint(clickPos)) {
			nickField->attachWithIME();
			nickField->setPlaceHolder("");
			bg->setTexture("input_focus.png");
		}
		};
	_eventDispatcher->addEventListenerWithSceneGraphPriority(mouseListener, this);
}

void MainMenuScene::onEnter() {
	Layer::onEnter();
	int finalLength = UserDefault::getInstance()->getIntegerForKey("finalLength", 0);
	if (finalLength > 0 && finalLbl) {
		finalLbl->stopAllActions();
			finalLbl->setString("Your final length was " +
				std::to_string(finalLength));
			finalLbl->setOpacity(255);
			hint->stopAllActions();
			hint2->stopAllActions();
			hint->setOpacity(0);
			hint2->setOpacity(0);
	}
	else if (finalLbl) {
		finalLbl->setString("");
		finalLbl->setOpacity(0);
		hint->stopAllActions();
		hint2->stopAllActions();
		hint->setOpacity(0);
		hint2->setOpacity(0);

		auto repeat = RepeatForever::create(Sequence::create(FadeIn::create(0.5f),
			DelayTime::create(2.5f),
			FadeOut::create(0.5f), DelayTime::create(3.5f), nullptr));
		hint->runAction(repeat);

		auto repeat2 = RepeatForever::create(Sequence::create(DelayTime::create(3.5f), FadeIn::create(0.5f),
			DelayTime::create(2.5f), FadeOut::create(0.5f), nullptr));
		hint2->runAction(repeat2);
	}
}

void MainMenuScene::btn_Exit() {
	auto vs = Director::getInstance()->getWinSize();
	Vec2 center = vs / 2;
	auto btn = ui::Button::create("exit_normal.png", "exit_pressed.png");
	btn->setScale(0.5f);
	btn->setPosition(Vec2(vs.width * 0.95f, vs.height * 0.035f));
	btn->addClickEventListener([=](Ref* sender) {
		CC_CALLBACK_1(MainMenuScene::onExit, this);
		Director::getInstance()->end(); });
	this->addChild(btn);
}