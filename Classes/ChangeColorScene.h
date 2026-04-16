#pragma once
#include "cocos2d.h"
#include <vector>
#include <string>
#include "ui/CocosGUI.h"
class ChangeColorScene : public cocos2d::Scene
{
public:
    static cocos2d::Scene* createScene();
    virtual bool init();
    CREATE_FUNC(ChangeColorScene);
private:
    cocos2d::Sprite* snake = nullptr;
    cocos2d::Node* world = nullptr;
    int currentColor = 0;
    std::vector<std::string> snakeImg = {
        "snake_red.png", "snake_green.png",
        "snake_yellow.png","snake_blue.png", "snake_purple.png"
    };
    void arrow();
    void save();
};
