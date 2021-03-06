#ifndef __HELLOWORLD_SCENE_H__
#define __HELLOWORLD_SCENE_H__

#include "cocos2d.h"
using namespace cocos2d;
class HelloWorld : public cocos2d::Layer
{
public:
    static cocos2d::Scene* createScene();

    virtual bool init();
    
    // implement the "static create()" method manually
    CREATE_FUNC(HelloWorld);
	//void update(float dt) override;
private:
	cocos2d::Sprite* player;
	cocos2d::Vector<SpriteFrame*> attack;
	cocos2d::Vector<SpriteFrame*> dead;
	cocos2d::Vector<SpriteFrame*> run;
	cocos2d::Vector<SpriteFrame*> idle;
	cocos2d::Size visibleSize;
	cocos2d::Vec2 origin;
	cocos2d::Label* time;
	int dtime;
	int score;
	cocos2d::ProgressTimer* pT;
	bool playerIsAnimating;
	bool dying;
	bool attacking;
	void labelCallback(Ref* pSender, std::string direction);
	void attackCallback(Ref* pSender);
	void deadCallback(Ref* pSender);
	void update(float dt)override;
	void createAndMoveMonster(float dt);
	void hitByMonster(float dt);
	char lastCid;
	SpriteFrame *frame0;
};

#endif // __HELLOWORLD_SCENE_H__
