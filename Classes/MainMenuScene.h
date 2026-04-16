#ifndef __MAIN_MENU_SCENE_H__
#define __MAIN_MENU_SCENE_H__
#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "ui/UIEditBox/UIEditBox.h"

class MainMenuScene : public cocos2d::Layer
{
public:
	static cocos2d::Scene* createScene();
	virtual bool init() override;
	CREATE_FUNC(MainMenuScene);
	cocos2d::ui::TextField* nickField = nullptr;
	cocos2d::Label* hint;
	cocos2d::Label* hint2;
	cocos2d::Label* finalLbl = nullptr;
	void onEnter() override;
	
private:
	void btn_Play();
	void btn_Change();
	void lineEdit();
	void btn_Exit();
};
#endif // __MAIN_MENU_SCENE_H__