#pragma once
#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include <vector>
#include <string>

class GameScene : public cocos2d::Scene
{
public:
    static cocos2d::Scene* createScene();
    virtual bool init();
    void update(float dt) override;
    CREATE_FUNC(GameScene);

private:
    cocos2d::Sprite* head = nullptr;
    cocos2d::Node* world = nullptr;
    cocos2d::Label* lbl = nullptr;
    std::vector<cocos2d::Label*> boardLabels;
    std::vector<cocos2d::Sprite*> segments;
    std::vector<cocos2d::Sprite*> foods;
    std::vector<cocos2d::Vec2> positions;

    cocos2d::DrawNode* minimap = nullptr;
    cocos2d::Vec2 worldToMinimap(cocos2d::Vec2 worldPos, cocos2d::Vec2 mapCenter, float R);
    void drawMinimap();


    std::vector<std::string> foodImg = {
        "food_green.png", "food_pink.png",
        "food_blue.png", "food_purple.png", "food_yellow.png", "food_orange.png"
    };
    std::vector<std::string> headImg = {
        "red_head.png", "green_head.png", "yellow_head.png",
        "blue_head.png", "purple_head.png"
    };
    std::vector<std::string> bodyImg = {
        "red_body.png", "green_body.png", "yellow_body.png",
        "blue_body.png", "purple_body.png"
    };

    std::string curBody;
    std::string curTail;

    cocos2d::Vec2 mousePos;
    cocos2d::Vec2 direction = cocos2d::Vec2(1, 0);

    void randomFood();
    void addSegment();
    void onMouseMove(cocos2d::Event* event);
    void onMouseDown(cocos2d::Event* event);
    void onMouseUp(cocos2d::Event* event);
    void showLeaderboard();
    void updateLeaderboard();
    void gameOver();

    int length = 10;
    float sgmntDist = 12.0f;
    bool isGameOver = false;
    bool isBoosting = false;
};