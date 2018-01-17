#include "Thunder.h"
#include <algorithm>

USING_NS_CC;

using namespace CocosDenshion;

Scene* Thunder::createScene() {
  // 'scene' is an autorelease object
  auto scene = Scene::create();

  // 'layer' is an autorelease object
  auto layer = Thunder::create();

  // add layer as a child to scene
  scene->addChild(layer);

  // return the scene
  return scene;
}

bool Thunder::init() {
  if (!Layer::init()) {
    return false;
  }

  isMove = false;  // �Ƿ����ɴ�
  visibleSize = Director::getInstance()->getVisibleSize();

  // ��������
  auto bgsprite = Sprite::create("bg.jpg");
  bgsprite->setPosition(visibleSize / 2);
  bgsprite->setScale(visibleSize.width / bgsprite->getContentSize().width,
                     visibleSize.height / bgsprite->getContentSize().height);
  this->addChild(bgsprite, 0);

  // �����ɴ�
  player = Sprite::create("player.png");
  player->setAnchorPoint(Vec2(0.5, 0.5));
  player->setPosition(visibleSize.width / 2, player->getContentSize().height);
  player->setName("player");
  this->addChild(player, 1);
  playerSpeed = 0;

  // ��ʾ��ʯ���ӵ�����
  enemysNum = Label::createWithTTF("enemys: 0", "fonts/arial.TTF", 20);
  enemysNum->setColor(Color3B(255, 255, 255));
  enemysNum->setPosition(50, 60);
  this->addChild(enemysNum, 3);
  bulletsNum = Label::createWithTTF("bullets: 0", "fonts/arial.TTF", 20);
  bulletsNum->setColor(Color3B(255, 255, 255));
  bulletsNum->setPosition(50, 30);
  this->addChild(bulletsNum, 3);

  addEnemy(5);        // ��ʼ����ʯ
  preloadMusic();     // Ԥ��������
  playBgm();          // ���ű�������
  explosion();        // ������ը֡����

  // ��Ӽ�����
  addTouchListener();
  addKeyboardListener();
  addCustomListener();

  // ������
  schedule(schedule_selector(Thunder::update), 0.04f, kRepeatForever, 0);
  return true;
}

//Ԥ���������ļ�
void Thunder::preloadMusic() {
  // Todo
}

//���ű�������
void Thunder::playBgm() {
	auto audio = SimpleAudioEngine::getInstance();
	audio->playBackgroundMusic("music/bgm.mp3", true);
}

//��ʼ����ʯ
void Thunder::addEnemy(int n) {
  enemys.clear();
  enemyWidth = visibleSize.width / (n + 1.0);
  enemyHeight = 50;
  for (unsigned i = 0; i < 3; ++i) {
    char enemyPath[20];
    sprintf(enemyPath, "stone%d.png", 3 - i);
    double width = enemyWidth,
           height = visibleSize.height - (enemyHeight * (i + 1));
    for (int j = 0; j < n; ++j) {
      auto enemy = Sprite::create(enemyPath);
      enemy->setAnchorPoint(Vec2(0.5, 0.5));
      enemy->setScale(0.5, 0.5);
      enemy->setPosition(width * (j + 1), height);
      enemys.push_back(enemy);
      addChild(enemy, 1);
    }
  }
  nextEnemyType = 1;
}

// ��ʯ�����ƶ��������µ�һ��(�ӷ���)
void Thunder::newEnemy() {
	int numPerRow = 5;

	// ���е���ʯ�����ƶ�
	for (auto enemy : enemys) {
		double height = enemy->getContentSize().height;
		enemy->setPosition(enemy->getPosition().x, enemy->getPosition().y - enemyHeight);
    }

	//�����µ�һ����ʯ
	const double width = enemyWidth,
		         height = visibleSize.height - enemyHeight;
	char enemyPath[20];
	sprintf(enemyPath, "stone%d.png", nextEnemyType);
	const int offsetX = 80;
	for (int i = 0; i < numPerRow; i++) {
		auto enemy = Sprite::create(enemyPath);
		enemy->setAnchorPoint(Vec2(0.5, 0.5));
		enemy->setScale(0.5, 0.5);
		enemy->setPosition(width * (i + 1) - offsetX, height);
		enemys.push_back(enemy);
		addChild(enemy, 1);
	}
	nextEnemyType = (nextEnemyType + 1) % 3 + 1; // ��������Ϊ1�� 2�� 3�е���һ�� 
}

// �ƶ��ɴ�
void Thunder::movePlane(char c) {
	int accelerate;
	if (isMove) {
		if (c == 'A') {
			accelerate = -2;
			if (-playerSpeed < maxSpeed)
				playerSpeed += accelerate;
		}
		else {
			accelerate = 2;
			if (playerSpeed < maxSpeed)
				playerSpeed += accelerate;
		}
	}
	else {
		accelerate = playerSpeed > 0? -2 : 2;
		int newSpeed = playerSpeed + accelerate;
		if (newSpeed * playerSpeed > 0)
			playerSpeed += accelerate;
		else
			playerSpeed = 0;
	}
	
	if (player != NULL && playerSpeed)
		setPlanePosition(player->getPosition() + Vec2(playerSpeed, 0));
}

void Thunder::setPlanePosition(Vect v)
{
	const int margin = 50;
	if (v.x > margin && v.x < visibleSize.width - margin
	 && v.y > margin && v.y < visibleSize.height - margin)
		player->setPosition(v);
}

//�����ӵ�
void Thunder::fire() {
  auto bullet = Sprite::create("bullet.png");
  bullet->setAnchorPoint(Vec2(0.5, 0.5));
  bullets.push_back(bullet);
  bullet->setPosition(player->getPosition());
  addChild(bullet, 1);
  SimpleAudioEngine::getInstance()->playEffect("music/fire.wav", false);

  // �Ƴ��ɳ���Ļ����ӵ�
  // Todo
  bullet->runAction(MoveBy::create(1.0f, Vec2(0, visibleSize.height)));

}

// �иը����֡
void Thunder::explosion() {
	explore.reserve(8);
	auto texture = Director::getInstance()->getTextureCache()->addImage("explosion.png");
	const int width = 188.8;
	const int height = 160;
	for (int i = 0; i < 8; i++) {
		int x = width * (i % 5);
		int y = height * (i / 5);
		auto frame = SpriteFrame::createWithTexture(texture, CC_RECT_PIXELS_TO_POINTS(Rect(x, y, width, height)));
		explore.pushBack(frame);
	}
    // 944 * 425
  
}

void Thunder::update(float f) {
  // ʵʱ����ҳ������ʯ���ӵ�����(����ɾ��)
  // Ҫ��������ʾ��ȷ(�ӷ���)
  char str[15];
  sprintf(str, "enemys: %d", enemys.size());
  enemysNum->setString(str);
  sprintf(str, "bullets: %d", bullets.size());
  bulletsNum->setString(str);

  // �ɴ��ƶ�
  this->movePlane(movekey);

  static int ct = 0;
  static int dir = 4;
  ++ct;
  if (ct == 120)
    ct = 40, dir = -dir;
  else if (ct == 80) {
    dir = -dir;
    newEnemy();  // ��ʯ�����ƶ��������µ�һ��(�ӷ���)
  }
  else if (ct == 20)
    ct = 40, dir = -dir;

  //��ʯ�����ƶ�
  for (Sprite* s : enemys) {
    if (s != NULL) {
      s->setPosition(s->getPosition() + Vec2(dir, 0));
    }
  }

  // �ַ��Զ����¼�
  EventCustom e("meet");
  EventCustom e2("removeBullet");
  _eventDispatcher->dispatchEvent(&e);
  _eventDispatcher->dispatchEvent(&e2);
}

// �Զ�����ײ�¼�
void Thunder::meet(EventCustom * event) {
  // �ж��ӵ��Ƿ������ʯ��ִ�ж�Ӧ����
	for (auto bullet : bullets) {
		bool hasCollision = false;
		for (auto enemy : enemys) {
			if (bullet->getPosition().getDistance(enemy->getPosition()) < 25) {
				auto explose = Animate::create(Animation::createWithSpriteFrames(explore, 0.05f));
				auto sequence = Sequence::create(explose, CallFunc::create([enemy]() {
					enemy->removeFromParentAndCleanup(true);
				}), nullptr);
				SimpleAudioEngine::getInstance()->playEffect("music/explore.wav");
				enemy->runAction(sequence);
				bullet->removeFromParentAndCleanup(true);
				bullets.remove(bullet);
				enemys.remove(enemy);
				hasCollision = true;
				break;
			}
		}
		if (hasCollision)
			break;
  }

	for (auto enemy : enemys) {
		if (enemy->getPosition().getDistance(player->getPosition()) < 50
			|| (enemy->getPosition().y < 50)) {
			//CCLog("aa");
			auto explose = Animate::create(Animation::createWithSpriteFrames(explore, 0.05f));
			auto sequence = Sequence::create(explose, CallFunc::create([this]() {
				player->removeFromParentAndCleanup(true);
			}), nullptr);
			SimpleAudioEngine::getInstance()->playEffect("music/explore.wav");
			player->runAction(sequence);
			unschedule(schedule_selector(Thunder::update));
			auto gameOver = Sprite::create("gameOver.png");
			gameOver->setPosition(Vect(visibleSize.width / 2, visibleSize.height / 2));
			addChild(gameOver);
			_eventDispatcher->removeAllEventListeners();
		}
	} 
  // �ж���Ϸ�Ƿ������ִ�ж�Ӧ����
  // Todo
}

void Thunder::removeBullet(EventCustom * event)
{
	for (auto bullet : bullets) {
		auto pos = bullet->getPosition();
		if (pos.x < 0 || pos.x > visibleSize.width
			|| pos.y < 0 || pos.y > visibleSize.height) {
			bullet->removeFromParentAndCleanup(true);
			bullets.remove(bullet);
			break;
		}
	}
}

// ����Զ��������
void Thunder::addCustomListener() {
	_eventDispatcher->addCustomEventListener("meet", CC_CALLBACK_1(Thunder::meet, this));
	_eventDispatcher->addCustomEventListener("removeBullet", CC_CALLBACK_1(Thunder::removeBullet, this));
}

// ��Ӽ����¼�������
void Thunder::addKeyboardListener() {
	auto kbListener = EventListenerKeyboard::create();
	kbListener->onKeyPressed = CC_CALLBACK_2(Thunder::onKeyPressed, this);
	kbListener->onKeyReleased = CC_CALLBACK_2(Thunder::onKeyReleased, this);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(kbListener, player);
}

void Thunder::onKeyPressed(EventKeyboard::KeyCode code, Event* event) {
  switch (code) {
  case EventKeyboard::KeyCode::KEY_LEFT_ARROW:
  case EventKeyboard::KeyCode::KEY_CAPITAL_A:
  case EventKeyboard::KeyCode::KEY_A:
    movekey = 'A';
    isMove = true;
    break;
  case EventKeyboard::KeyCode::KEY_RIGHT_ARROW:
  case EventKeyboard::KeyCode::KEY_CAPITAL_D:
  case EventKeyboard::KeyCode::KEY_D:
    movekey = 'D';
    isMove = true;
    break;
  case EventKeyboard::KeyCode::KEY_SPACE:
    fire();
    break;
  }
}

void Thunder::onKeyReleased(EventKeyboard::KeyCode code, Event* event) {
  switch (code) {
  case EventKeyboard::KeyCode::KEY_LEFT_ARROW:
  case EventKeyboard::KeyCode::KEY_A:
  case EventKeyboard::KeyCode::KEY_CAPITAL_A:
  case EventKeyboard::KeyCode::KEY_RIGHT_ARROW:
  case EventKeyboard::KeyCode::KEY_D:
  case EventKeyboard::KeyCode::KEY_CAPITAL_D:
    isMove = false;
    break;
  }
}

// ��Ӵ����¼�������
void Thunder::addTouchListener() {
	auto touchListener = EventListenerTouchOneByOne::create();
	touchListener->onTouchBegan = CC_CALLBACK_2(Thunder::onTouchBegan, this);
	touchListener->onTouchEnded = CC_CALLBACK_2(Thunder::onTouchEnded, this);
	touchListener->onTouchMoved = CC_CALLBACK_2(Thunder::onTouchMoved, this);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(touchListener, player);
}

// ����������ڵ�
bool Thunder::onTouchBegan(Touch *touch, Event *event) {
	auto loc = touch->getLocation();
	if (player->getBoundingBox().containsPoint(loc))
		isClick = true;
	fire();
    return true;
}

void Thunder::onTouchEnded(Touch *touch, Event *event) {
    isClick = false;
}

// ����갴ס�ɴ���ɿ��Ʒɴ��ƶ� (�ӷ���)
void Thunder::onTouchMoved(Touch *touch, Event *event) {
	if (isClick)
		setPlanePosition(Vect(touch->getLocation().x, player->getPosition().y));
  // Todo
}
