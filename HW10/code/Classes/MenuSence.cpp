#include "MenuSence.h"
#include "GameSence.h"
USING_NS_CC;

Scene* MenuSence::createScene()
{
    // 'scene' is an autorelease object
    auto scene = Scene::create();

    // 'layer' is an autorelease object
    auto layer = MenuSence::create();

    // add layer as a child to scene
    scene->addChild(layer);

    // return the scene
    return scene;
}

// on "init" you need to initialize your instance
bool MenuSence::init()
{

    if ( !Layer::init() )
    {
        return false;
    }

    Size visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

	auto bg_sky = Sprite::create("menu-background-sky.jpg");
	bg_sky->setPosition(Vec2(visibleSize.width / 2 + origin.x, visibleSize.height / 2 + origin.y + 150));
	this->addChild(bg_sky, 0);

	auto bg = Sprite::create("menu-background.png");
	bg->setPosition(Vec2(visibleSize.width / 2 + origin.x, visibleSize.height / 2 + origin.y - 60));
	this->addChild(bg, 0);

	auto miner = Sprite::create("menu-miner.png");
	miner->setPosition(Vec2(150 + origin.x, visibleSize.height / 2 + origin.y - 60));
	this->addChild(miner, 1);

	auto leg = Sprite::createWithSpriteFrameName("miner-leg-0.png");
	Animate* legAnimate = Animate::create(AnimationCache::getInstance()->getAnimation("legAnimation"));
	leg->runAction(RepeatForever::create(legAnimate));
	leg->setPosition(110 + origin.x, origin.y + 102);
	this->addChild(leg, 1);

	auto goldMinerText = Sprite::create("gold-miner-text.png");
	goldMinerText->setPosition(Vect(origin.x + visibleSize.width / 2,
		origin.y + visibleSize.height - goldMinerText->getContentSize().height));
	this->addChild(goldMinerText);

	auto sprite = Sprite::create("menu-start-gold.png");
	sprite->setPosition(Vec2(origin.x + visibleSize.width - sprite->getContentSize().width / 2,
								origin.y + sprite->getContentSize().width / 2));
	this->addChild(sprite, 0);

	auto startItem = MenuItemImage::create(
		"start-0.png",
		"start-1.png",
		CC_CALLBACK_1(MenuSence::menuStartCallback, this));
	startItem->setAnchorPoint(Vect(0.5, 0.5));
	startItem->setPosition(Vec2(origin.x + sprite->getContentSize().width / 2,
								origin.y + sprite->getContentSize().width * 8 / 13));
	auto menu = Menu::create(startItem, NULL);
	menu->setPosition(Vec2::ZERO);
	sprite->addChild(menu, 0);

    return true;
}

void MenuSence::menuStartCallback(Ref* pSender)
{
	auto gameScene = GameSence::createScene();
	gameScene->init();
	Director::getInstance()->replaceScene(gameScene);
}
