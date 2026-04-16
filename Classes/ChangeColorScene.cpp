#include "ChangeColorScene.h"
#include "ui/CocosGUI.h"
USING_NS_CC;

Scene* ChangeColorScene::createScene()
{
    return ChangeColorScene::create();
}

bool ChangeColorScene::init()
{
    if (!Scene::init())
    {
        return false;
    }
    auto vs = Director::getInstance()->getWinSize();
    Vec2 center = vs / 2;

    world = Node::create();
    this->addChild(world);
    auto border = DrawNode::create();

    auto bg = Sprite::create("background.png");
    bg->setAnchorPoint(Vec2(0.5f, 0.5f));
    bg->setPosition(center);
    float scaleX = 3000.0f / bg->getContentSize().width;
    float scaleY = 3000.0f / bg->getContentSize().height;
    bg->setScale(MAX(scaleX, scaleY));
    world->addChild(bg, -1);
    float w = vs.width;
    this->schedule([=](float dt) mutable {
        bg->setPositionX(bg->getPositionX() - 100 * dt);
        if (bg->getPositionX() <= -w / 2) {
            bg->setPositionX(bg->getPositionX() + w);
        }}, "bg_loop");

    snake = Sprite::create(snakeImg[currentColor]);
    snake->setPosition(Vec2(vs.width / 2, vs.height / 2));

    auto up = EaseSineInOut::create(MoveBy::create(0.8f, Vec2(0, 6)));
    auto down = EaseSineInOut::create(MoveBy::create(0.8f, Vec2(0, -6)));
    auto wave = Sequence::create(up, down, nullptr);
    snake->runAction(RepeatForever::create(wave));
    this->addChild(snake);

    arrow();
    save();
    return true;
}

void ChangeColorScene::arrow() {
    auto vs = Director::getInstance()->getVisibleSize();
    Vec2 center = vs / 2;

    auto btn1 = ui::Button::create("arrow_left.png", "arrow_left_pressed.png");
    btn1->setScale(0.5f);
    btn1->setPosition(Vec2(vs.width * 0.23f, vs.height * 0.5f));
    btn1->addClickEventListener([=](Ref* sender) {
        currentColor = (currentColor - 1 + 5) % 5;
        snake->setTexture(snakeImg[currentColor]);
        });
    this->addChild(btn1);

    auto btn2 = ui::Button::create("arrow_right.png", "arrow_right_pressed.png");
    btn2->setScale(0.5f);
    btn2->setPosition(Vec2(vs.width * 0.77f, vs.height * 0.5f));
    btn2->addClickEventListener([=](Ref* sender) {
        currentColor = (currentColor + 1) % 5;
        snake->setPosition(center);
        snake->setTexture(snakeImg[currentColor]);
        });
    this->addChild(btn2);
}

void ChangeColorScene::save() {
    auto vs = Director::getInstance()->getVisibleSize();
    Vec2 center = vs / 2;
    auto btn = ui::Button::create("save_normal.png","save_pressed.png");
    btn->setScale(0.5f);
    btn->setPosition(Vec2(vs.width * 0.5f, vs.height * 0.25f));
    btn->addClickEventListener([=](Ref* sender) {
        UserDefault::getInstance()->setIntegerForKey("savedColor", currentColor);
        UserDefault::getInstance()->flush();
        Director::getInstance()->popScene();
        });
    this->addChild(btn);
}