#include "HelloWorldScene.h"
#pragma execution_character_set("utf-8")
USING_NS_CC;

Scene* HelloWorld::createScene()
{
    // 'scene' is an autorelease object
    auto scene = Scene::create();

    // 'layer' is an autorelease object
    auto layer = HelloWorld::create();

    // add layer as a child to scene
    scene->addChild(layer);

    // return the scene
    return scene;
}

// on "init" you need to initialize your instance
bool HelloWorld::init()
{
    //////////////////////////////
    // 1. super init first
    if ( !Layer::init() )
    {
        return false;
    }

    visibleSize = Director::getInstance()->getVisibleSize();
    origin = Director::getInstance()->getVisibleOrigin();

	//创建一张贴图
	auto texture = Director::getInstance()->getTextureCache()->addImage("$lucia_2.png");
	//从贴图中以像素单位切割，创建关键帧
	auto frame0 = SpriteFrame::createWithTexture(texture, CC_RECT_PIXELS_TO_POINTS(Rect(0, 0, 113, 113)));
	//使用第一帧创建精灵
	player = Sprite::createWithSpriteFrame(frame0);
	player->setPosition(Vec2(origin.x + visibleSize.width / 2,
							origin.y + visibleSize.height/2));
	addChild(player, 3);

	//hp条
	Sprite* sp0 = Sprite::create("hp.png", CC_RECT_PIXELS_TO_POINTS(Rect(0, 320, 420, 47)));
	Sprite* sp = Sprite::create("hp.png", CC_RECT_PIXELS_TO_POINTS(Rect(610, 362, 4, 16)));
	// sp->setPosition(Vec2(100, 100));
	// addChild(sp);
	//使用hp条设置progressBar
	pT = ProgressTimer::create(sp);
	pT->setScaleX(90);
	pT->setAnchorPoint(Vec2(0, 0));
	pT->setType(ProgressTimerType::BAR);
	pT->setBarChangeRate(Point(1, 0));
	pT->setMidpoint(Point(0, 1));
	pT->setPercentage(100);
	pT->setPosition(Vec2(origin.x+14*pT->getContentSize().width,origin.y + visibleSize.height - 2*pT->getContentSize().height));
	addChild(pT,1);
	sp0->setAnchorPoint(Vec2(0, 0));
	sp0->setPosition(Vec2(origin.x + pT->getContentSize().width, origin.y + visibleSize.height - sp0->getContentSize().height));
	addChild(sp0,0);

	
	// 静态动画
	idle.reserve(1);
	idle.pushBack(frame0);

	// 攻击动画
	attack.reserve(17);
	for (int i = 0; i < 17; i++) {
		auto frame = SpriteFrame::createWithTexture(texture, CC_RECT_PIXELS_TO_POINTS(Rect(113*i,0,113,113)));
		attack.pushBack(frame);
	}
	attack.pushBack(frame0);
	auto attackAnimation = Animation::createWithSpriteFrames(attack, 0.1f);
	AnimationCache::getInstance()->addAnimation(attackAnimation, "attack");

	// 可以仿照攻击动画
	// 死亡动画(帧数：22帧，高：90，宽：79）
	auto texture2 = Director::getInstance()->getTextureCache()->addImage("$lucia_dead.png");
	dead.reserve(22);
	for (int i = 0; i < 22; i++) {
		auto frame = SpriteFrame::createWithTexture(texture2, CC_RECT_PIXELS_TO_POINTS(Rect(79 * i, 0, 79, 90)));
		dead.pushBack(frame);
	}
	dead.pushBack(frame0);
	auto deadAnimation = Animation::createWithSpriteFrames(dead, 0.1f);
	AnimationCache::getInstance()->addAnimation(deadAnimation, "dead");
    // Todo

	// 运动动画(帧数：8帧，高：101，宽：68）
	auto texture3 = Director::getInstance()->getTextureCache()->addImage("$lucia_forward.png");
	run.reserve(8);
	for (int i = 0; i < 2; i++) {
		auto frame = SpriteFrame::createWithTexture(texture3, CC_RECT_PIXELS_TO_POINTS(Rect(68 * i, 0, 68, 101)));
		run.pushBack(frame);
	}
	run.pushBack(frame0);
	auto runAnimation = Animation::createWithSpriteFrames(run, 0.1f);
	AnimationCache::getInstance()->addAnimation(runAnimation, "run");
    // Todo

	
	// 四个按键

	auto menu = Menu::create();
	menu->setPosition(80, 50);
	addChild(menu);
	auto createDirectionLabel = [this, &menu](std::string c) {
		int x = 0, y = 0;
		auto label = Label::create(c, "arial", 26);
		auto menuItem = MenuItemLabel::create(label, CC_CALLBACK_1(HelloWorld::labelCallback, this, c));
		if (c == "W")
			y += 1.2 * label->getContentSize().height;
		else if (c == "A")
			x -= 1.5 * label->getContentSize().width;
		else if (c == "D")
			x += 1.5 * label->getContentSize().width;
		else if (c == "S")
			x = x; // do nothing;
		menuItem->setPosition(x, y);
		menu->addChild(menuItem);
	};


	createDirectionLabel("W");
	createDirectionLabel("S");
	createDirectionLabel("A");
	createDirectionLabel("D");


	auto label = Label::create("X", "arial", 36);
	auto menuItem = MenuItemLabel::create(label, CC_CALLBACK_1(HelloWorld::attackCallback, this));
	menuItem->setPosition(origin.x + visibleSize.width - 120, -10);
	menu->addChild(menuItem);


	label = Label::create("Y", "arial", 36);
	menuItem = MenuItemLabel::create(label, CC_CALLBACK_1(HelloWorld::deadCallback, this));
	menuItem->setPosition(origin.x + visibleSize.width - 100, 10);
	menu->addChild(menuItem);

	time = Label::createWithTTF("180", "fonts/arial.ttf", 26);
	time->setPosition(origin.x + visibleSize.width / 2, origin.y + visibleSize.height - label->getContentSize().height);
	addChild(time);

	schedule(schedule_selector(HelloWorld::update), 1.0f);

	return true;
}

void HelloWorld::labelCallback(Ref* pSender, std::string direction)
{
	auto position = player->getPosition();
	int bound = 50;
	if (playerIsAnimating == false 
		&& ((position.x > bound && direction == "A")
		|| (position.x < visibleSize.width - bound && direction == "D")
		|| (position.y > bound && direction == "S")
		|| (position.y < visibleSize.height - bound && direction == "W"))) {
		playerIsAnimating = true;
		int x, y;
		if (direction == "W") {
			x = 0;
			y = player->getContentSize().height * 2 / 7;
		}
		else if (direction == "A"){
			x = -player->getContentSize().width * 2 / 7;
			y = 0;
		}
		else if (direction == "S") {
			x = 0;
			y = -player->getContentSize().height * 2 / 7;
		}
		else if (direction == "D") {
			x = player->getContentSize().width * 2 / 7;
			y = 0;
		}
		auto spawn = Spawn::createWithTwoActions(Animate::create(AnimationCache::getInstance()->getAnimation("run")),
			MoveBy::create(0.2f, Vec2(x, y)));
		auto sequence = Sequence::create(spawn, CCCallFunc::create(([this]() {
			playerIsAnimating = false;
		})), nullptr);
		player->runAction(sequence);
	}
	return;
}

void HelloWorld::attackCallback(Ref * pSender)
{
	if (playerIsAnimating == false) {
		playerIsAnimating = true;
		auto sequence = Sequence::create(Animate::create(AnimationCache::getInstance()->getAnimation("attack")),
			CCCallFunc::create(([this]() {
				playerIsAnimating = false;
			})), nullptr);
		player->runAction(sequence);

		float percentage = pT->getPercentage();
		if (percentage < 100) {
			auto to = ProgressFromTo::create(1.7f, percentage, percentage + 20);
			pT->runAction(to);
		}
	}
	
}

void HelloWorld::deadCallback(Ref * pSender)
{
	if (playerIsAnimating == false) {
		playerIsAnimating = true;
		auto sequence = Sequence::create(Animate::create(AnimationCache::getInstance()->getAnimation("dead")),
			CCCallFunc::create(([this]() {
			playerIsAnimating = false;
		})), nullptr);
		player->runAction(sequence);

		float percentage = pT->getPercentage();
		if (percentage > 0) {
			auto to = ProgressFromTo::create(2.2f, percentage, percentage - 20);
			pT->runAction(to);
		}
	}
}

void HelloWorld::update(float dt)
{
	std::string str = time->getString();
	int number = atoi(str.c_str());
	if (number > 0) {
		number--;
		char result[10] = { 0 };
		sprintf(result, "%d", number);
		time->setString(result);
	}
	else
		unschedule(schedule_selector(HelloWorld::update));
}
