#include "GameScene.h"
#include "ui/CocosGUI.h"
#include <sstream>
#include <algorithm>
USING_NS_CC;

Scene* GameScene::createScene() {
    return GameScene::create();
}

bool GameScene::init() {
    if (!Scene::init()) return false;

    auto visibleSize = Director::getInstance()->getWinSize();
    Vec2 center = visibleSize / 2;

    world = Node::create();
    this->addChild(world);
  
    mousePos = center;
    
    auto bg = Sprite::create("background.png");
    bg->setPosition(1500, 1500);
    float scaleX = 3000.0f / bg->getContentSize().width;
    float scaleY = 3000.0f / bg->getContentSize().height;
    bg->setScale(MAX(scaleX, scaleY) * 1.2f);
    world->addChild(bg, -1);

    float worldR = 1200.0f;
    auto border = DrawNode::create();
    for (int r = 2400; r > 1200; r -= 10) {
    float alpha = (r > 1210) ? 0.85f : 1.0f;
        border->drawCircle(Vec2(1500, 1500), r, 0, 100, false,
            Color4F(0.25f, 0, 0, alpha));
    }
    border->drawCircle(Vec2(1500, 1500), 1200, 0, 100, false,
        Color4F(1, 0, 0, 1));
    world->addChild(border, 5);

    lbl = Label::createWithTTF(" ", "fonts/TitilliumWeb-Regular.ttf", 20);
    lbl->setColor(Color3B::WHITE);
    lbl->setAnchorPoint(Vec2(0, 0));
    lbl->setPosition(Vec2(10, 10));
    this->addChild(lbl, 10);

    for (int i = 0; i < 500; i++) randomFood();

    int color = UserDefault::getInstance()->getIntegerForKey("savedColor", 0);
    curBody = bodyImg[color];

    head = Sprite::create(headImg[color]);
    head->setScale(0.45f);
    head->setPosition(Vec2(1500, 1500));
    world->addChild(head, 3);

    positions.clear();
    for (int i = 0; i < 500; i++)
        positions.push_back(Vec2(1500 - i * 1.0f, 1500));

    for (int i = 1; i < length; i++) {
        auto seg = Sprite::create(curBody);
        seg->setScale(0.5f);
        seg->setPosition(Vec2(1500 - i * sgmntDist, 1500));
        world->addChild(seg, 2);
        segments.push_back(seg);
    }

    auto mouseListener = EventListenerMouse::create();
    mouseListener->onMouseMove = CC_CALLBACK_1(GameScene::onMouseMove, this);
    mouseListener->onMouseDown = CC_CALLBACK_1(GameScene::onMouseDown, this);
    mouseListener->onMouseUp = CC_CALLBACK_1(GameScene::onMouseUp, this);
    _eventDispatcher->addEventListenerWithSceneGraphPriority(mouseListener, this);

    showLeaderboard();
    updateLeaderboard();
    this->scheduleUpdate();
    return true;
}

void GameScene::onMouseMove(cocos2d::Event* event) {
    auto mouseEvent = dynamic_cast<EventMouse*>(event);
    mousePos = Vec2(mouseEvent->getCursorX(), mouseEvent->getCursorY());
}
void GameScene::onMouseDown(cocos2d::Event* event) {
    auto mouseEvent = dynamic_cast<EventMouse*>(event);
    if (mouseEvent->getMouseButton() == EventMouse::MouseButton::BUTTON_LEFT) {
        isBoosting = true;
    }
}
void GameScene::onMouseUp(cocos2d::Event* event) {
    auto mouseEvent = dynamic_cast<EventMouse*>(event);
    if (mouseEvent->getMouseButton() == EventMouse::MouseButton::BUTTON_LEFT) {
        isBoosting = false;
    }
};

void GameScene::randomFood() {
    int idx = random(0, 5);
    auto food = Sprite::create(foodImg[idx]);

    Vec2 worldCenter = Vec2(1500, 1500);
    float worldR = 1200.0f;
    Vec2 pos;
    do {
        pos = Vec2(random((int)(1500 - worldR + 50), (int)(1500 + worldR - 50)),
            random((int)(1500 - worldR + 50), (int)(1500 + worldR - 50)));
    } while (pos.distance(worldCenter) > 1188.0f - 50);

    food->setPosition(pos);
    food->setScale(0.3f);
    food->setOpacity(255);
    
    float delay = CCRANDOM_0_1() * 1.5f;
    auto seq = Sequence::create(
        DelayTime::create(delay), FadeTo::create(0.8f, 100), FadeTo::create(0.8f, 255),nullptr);
    food->runAction(RepeatForever::create(seq));
    world->addChild(food);
    foods.push_back(food);
}

void GameScene::addSegment() {
    auto seg = Sprite::create(curBody);
    seg->setScale(0.5f);
    seg->setPosition(segments.back()->getPosition());
    world->addChild(seg, 2);
    segments.push_back(seg);
}

void GameScene::update(float dt) {
    auto vs = Director::getInstance()->getWinSize();
    Vec2 center = vs / 2;

    if (isGameOver) return;
    lbl->setString("Length: " + std::to_string(length));
    updateLeaderboard();

    Vec2 dir = mousePos - center;
    if (dir.length() > 1.0f) {
        Vec2 targetDir = dir.getNormalized();
        direction = direction.lerp(targetDir, 15.0f * dt);
        direction.normalize();
    }

    float speed = 100.0f;
    if (isBoosting && length >= 10) {
        speed = 300.0f;
        static float boostTimer = 0;
        boostTimer += dt;
        if (boostTimer > 0.3f) {
            boostTimer = 0;
            if (segments.size() > 3) {
                int idx = segments.size() - 1;
                segments[idx]->removeFromParent();
                segments.erase(segments.begin() + idx);
                length--;
            }
        }
    }
    else {
        static float boostTimer = 0;
    }
    Vec2 newPos = (head->getPosition()) + direction * speed * dt;

    
    Vec2 worldCenter = Vec2(1500, 1500);
    float worldR = 1180.0f;
    if (newPos.distance(worldCenter) > worldR) {
        gameOver();
        return;
    }
    head->setPosition(newPos);
    Vec2 offset = head->getPosition() - center;
    world->setPosition(-offset);
    float angle = CC_RADIANS_TO_DEGREES(atan2(direction.y, direction.x)) - 90;
    head->setRotation(-angle);

    Vec2 prevPos = positions.empty() ? newPos : positions.front();
    float dist = prevPos.distance(newPos);
    int steps = std::max(1, (int)(dist / 1.0f)); 

    for (int i = 1; i <= steps; i++) {
        Vec2 interp = prevPos.lerp(newPos, (float)i / steps);
        positions.insert(positions.begin(), interp);
    }

    while (positions.size() > segments.size() * (int)sgmntDist + 50)
        positions.pop_back();

    for (int i = 0; i < segments.size(); i++) {
        int idx = (i + 1) * (int)sgmntDist;
        if (idx < positions.size()) {
            segments[i]->setPosition(positions[idx]);
            if (idx + 1 < positions.size()) {
                Vec2 d = positions[idx] - positions[idx + 1];
                float a = CC_RADIANS_TO_DEGREES(atan2(d.y, d.x)) - 90;
                segments[i]->setRotation(-a);
            }
        }
    }

    Vec2 newHeadPos = head->getPosition();
    for (int i = foods.size() - 1; i >= 0; i--) {
        if (newHeadPos.distance(foods[i]->getPosition()) < 5) {
            foods[i]->removeFromParent();
            foods.erase(foods.begin() + i);
            length++;
            addSegment();
            randomFood();
        }
    }
    drawMinimap();
}

void GameScene::showLeaderboard() {
    auto vs = Director::getInstance()->getWinSize();

    auto title = Label::createWithTTF("Leaderboard", "fonts/TitilliumWeb-Bold.ttf", 20);
    title->setColor(Color3B::WHITE);
    title->setAnchorPoint(Vec2(1, 1));
    title->setPosition(Vec2(vs.width * 0.93f, vs.height * 0.98f));
    this->addChild(title, 10);
}

void GameScene::updateLeaderboard() {
    auto visibleSize = Director::getInstance()->getWinSize();

    for (auto l : boardLabels) l->removeFromParent();
    boardLabels.clear();

    std::string nick = UserDefault::getInstance()->getStringForKey("nickname", "Player");

    std::vector<std::pair<std::string, int>> results;
    results.push_back({ nick, length });
    std::string item;
  
    float rightX = visibleSize.width * 0.98f;
    float startY = visibleSize.height * 0.93f;
    float rowH = 28.0f;

    for (int i = 0; i < std::min((int)results.size(), 10); i++) {
        bool isMe = results[i].first == nick;
        Color3B color = isMe ? Color3B::BLUE : Color3B(200, 200, 200);
        float y = startY - i * rowH;

        auto lblNum = Label::createWithTTF("#" + std::to_string(i + 1),"fonts/TitilliumWeb-Regular.ttf", 15);
        lblNum->setColor(Color3B(120, 120, 120));
        lblNum->setAnchorPoint(Vec2(1, 1));
        lblNum->setPosition(Vec2(rightX - 220, y));
        this->addChild(lblNum, 10);
        boardLabels.push_back(lblNum);

        auto lblNick = Label::createWithTTF(results[i].first, "fonts/TitilliumWeb-Regular.ttf", 16);
        lblNick->setColor(color);
        lblNick->setAnchorPoint(Vec2(0, 1));
        lblNick->setPosition(Vec2(rightX - 210, y));
        this->addChild(lblNick, 10);
        boardLabels.push_back(lblNick);

        auto lblScore = Label::createWithTTF(std::to_string(results[i].second),"fonts/TitilliumWeb-Regular.ttf", 16);
        lblScore->setColor(isMe ? Color3B::YELLOW : Color3B(100, 180, 255));
        lblScore->setAnchorPoint(Vec2(1, 1));
        lblScore->setPosition(Vec2(rightX, y));
        this->addChild(lblScore, 10);
        boardLabels.push_back(lblScore);
    }
}

Vec2 GameScene::worldToMinimap(Vec2 worldPos, Vec2 mapCenter, float R) {
    Vec2 worldCenter = Vec2(1500, 1500);
    float arenaR = 1200.0f;

    Vec2 dir = worldPos - worldCenter;
    Vec2 normalized = dir / 1250.0f;

    return mapCenter + normalized * R;
}

void GameScene::drawMinimap() {
    if (minimap) { minimap->clear(); }
    else {
        minimap = DrawNode::create();
        minimap->setPosition(Vec2::ZERO);
        this->addChild(minimap, 15);
    }
    auto vs = Director::getInstance()->getWinSize();
    float R = 70.0f;
    Vec2 center = Vec2(vs.width - R - 15, R + 15);
    minimap->drawSolidCircle(center, R, 0, 50, Color4F(0, 0, 0, 0.6f));
    minimap->drawCircle(center, R, 0, 50, false, Color4F(1, 0, 0, 0.8f));

    for (auto& f : foods) {
        Vec2 pos = worldToMinimap(f->getPosition(), center, R);
        if (pos.distance(center) < R) {
            minimap->drawSolidCircle(pos, 2.0f, 0, 8, Color4F(1, 0.8f, 0, 0.7f));
        }
    }
    for (int i = 0; i < segments.size(); i += 2) {
        Vec2 pos = worldToMinimap(segments[i]->getPosition(), center, R);
        if (pos.distance(center) < R) {
            minimap->drawSolidCircle(pos, 2.0f, 0, 8, Color4F(0, 0.8f, 0.4f, 0.8f));
        }
    }
    Vec2 headOnMap = worldToMinimap(head->getPosition(), center, R);
    minimap->drawSolidCircle(headOnMap, 3.5f, 0, 12, Color4F(0, 1, 0.5f, 1));
    minimap->drawCircle(center, R, 0, 50, false, Color4F(1, 1, 1, 0.25f));
}

void GameScene::gameOver() {
    isGameOver = true;
    this->unscheduleUpdate();
    UserDefault::getInstance()->setIntegerForKey("finalLength", length);
    UserDefault::getInstance()->flush();
    auto fadeLayer = LayerColor::create(Color4B(0, 0, 0, 0));
    this->addChild(fadeLayer, 1000);

    auto fade = FadeTo::create(0.8f, 255);
    auto goBack = CallFunc::create([]() { Director::getInstance()->popScene(); });

    auto seq = Sequence::create(fade, goBack, nullptr);
    fadeLayer->runAction(seq);
}