#include "GameSence.h"

USING_NS_CC;


Scene* GameSence::createScene()
{
	// 'scene' is an autorelease object
	auto scene = Scene::create();

	// 'layer' is an autorelease object
	auto layer = GameSence::create();

	// add layer as a child to scene
	scene->addChild(layer);

	// return the scene
	return scene;
}

// on "init" you need to initialize your instance
bool GameSence::init()
{

	if (!Layer::init())
	{
		return false;
	}


	Size visibleSize = Director::getInstance()->getVisibleSize();
	Vec2 origin = Director::getInstance()->getVisibleOrigin();

	auto bg = Sprite::create("level-background-0.jpg");
	bg->setPosition(Vec2(visibleSize.width / 2 + origin.x, visibleSize.height / 2 + origin.y));
	this->addChild(bg, 0, GameSceneTag::BG);

	auto stoneLayer = Layer::create();
	stoneLayer->setAnchorPoint(Vect(0, 0));
	stoneLayer->setPosition(0, 0);
	this->addChild(stoneLayer, 1, GameSceneTag::STONE);

	auto stone = Sprite::create("stone.png");
	stone->setPosition(560, 480);
	stoneLayer->addChild(stone, 1);

	auto mouseLayer = Layer::create();
	mouseLayer->setAnchorPoint(Vect(0, 0));
	mouseLayer->setPosition(0, visibleSize.height / 2);
	this->addChild(mouseLayer, 1, GameSceneTag::MOUSE);

	const std::string mouseFrameName = "mouse-0.png";
	auto mouse = Sprite::createWithSpriteFrameName("mouse-0.png");
	Animate* mouseAnimate = Animate::create(AnimationCache::getInstance()->getAnimation("mouseAnimation"));
	mouse->runAction(RepeatForever::create(mouseAnimate));
	mouse->setPosition(visibleSize.width / 2, 0);
	mouseLayer->addChild(mouse, 1);

	const char *shootName = "shoot";
	auto shootLabel = Label::createWithTTF(shootName, "fonts/Marker Felt.ttf", 72);
	shootLabel->setPosition(Vect(origin.x + visibleSize.width - shootLabel->getContentSize().width, 480));
	this->addChild(shootLabel, 1, GameSceneTag::SHOOTLABEL);


	//add touch listener
	EventListenerTouchOneByOne* listener = EventListenerTouchOneByOne::create();
	listener->setSwallowTouches(true);
	listener->onTouchBegan = CC_CALLBACK_2(GameSence::onTouchBegan, this);
	Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener, this);

	return true;
}

bool GameSence::onTouchBegan(Touch *touch, Event *unused_event) {
	Size visibleSize = Director::getInstance()->getVisibleSize();
	Vec2 origin = Director::getInstance()->getVisibleOrigin();

	auto location = touch->getLocation();
	auto stoneLayer = this->getChildByTag(GameSceneTag::STONE);
	auto mouseLayer = this->getChildByTag(GameSceneTag::MOUSE);
	auto shootLabel = this->getChildByTag(GameSceneTag::SHOOTLABEL);
	auto stone = stoneLayer->getChildren().at(0);
	auto mouse = mouseLayer->getChildren().at(0);
	
	auto mouselocalPosition = mouseLayer->convertToNodeSpace(location);
	auto stoneWorldPosition = mouseLayer->convertToWorldSpace(mouse->getPosition());
	auto stoneLocalPosition = stoneLayer->convertToNodeSpace(stoneWorldPosition);

	bool isInShootLabel = shootLabel->getBoundingBox().containsPoint(location);

	if (isInShootLabel) {
		//CCLog("worldX == %f, worldY == %f", stoneWorldPosition.x, stoneWorldPosition.y);
		auto stoneMoveTo = MoveTo::create(0.5, stoneLocalPosition);
		stone->runAction(stoneMoveTo);

		auto mouseNewLocalPosition = mouseLayer->convertToNodeSpace(Vect(CCRANDOM_0_1() * visibleSize.width, CCRANDOM_0_1() * visibleSize.height));
		auto mouseMoveTo = MoveTo::create(0.5, mouseNewLocalPosition);
		mouse->runAction(mouseMoveTo);

		auto diamond = Sprite::create("diamond.png");
		diamond->setPosition(stoneWorldPosition);
		this->addChild(diamond);

		return true;
	}
	else{

		//put cheese
		auto cheese = Sprite::create("cheese.png");
		cheese->setPosition(location);
		this->addChild(cheese);
	
		//mouse move to cheese
		auto moveTo = MoveTo::create(0.5, mouselocalPosition);
		mouse->runAction(moveTo);

		//cheese eaten by mouse
		auto fadeOut = FadeOut::create(0.5);
		auto callBackRemove = CallFunc::create([this, &cheese]() {
			this->removeChild(cheese);
		});
		auto cheeseSequence = Sequence::create(DelayTime::create(0.5), fadeOut, nullptr);
		cheese->runAction(cheeseSequence);

		return true;
	}
}
