#include "LoginScene.h"
#include "cocostudio/CocoStudio.h"
#include "json/rapidjson.h"
#include "json/document.h"
#include "json/writer.h"
#include "json/stringbuffer.h"
#include "Global.h"
#include "GameScene.h"
#include "network/HttpClient.h"
#include <regex>
using std::to_string;
using std::regex;
using std::match_results;
using std::regex_match;
using std::cmatch;

using namespace cocos2d::network;
using namespace rapidjson;
USING_NS_CC;

using namespace cocostudio::timeline;

#include "json/document.h"
#include "json/writer.h"
#include "json/stringbuffer.h"
using namespace  rapidjson;

Scene* LoginScene::createScene()
{
    // 'scene' is an autorelease object
    auto scene = Scene::create();
    
    // 'layer' is an autorelease object
    auto layer = LoginScene::create();

    // add layer as a child to scene
    scene->addChild(layer);

    // return the scene
    return scene;
}





// on "init" you need to initialize your instance
bool LoginScene::init()
{
    // 1. super init first
    if ( !Layer::init() )
    {
        return false;
    }

    Size size = Director::getInstance()->getVisibleSize();
    visibleHeight = size.height;
    visibleWidth = size.width;

    textField = TextField::create("Player Name", "Arial", 30);
    textField->setPosition(Size(visibleWidth / 2, visibleHeight / 4 * 3));
    this->addChild(textField, 2);

    auto button = Button::create();
    button->setTitleText("Login");
    button->setTitleFontSize(30);
    button->setPosition(Size(visibleWidth / 4, visibleHeight / 2));
	button->addTouchEventListener(CC_CALLBACK_2(LoginScene::login, this));
    this->addChild(button, 2);

	auto button1 = Button::create();
	button1->setTitleText("changeScene");
	button1->setTitleFontSize(30);
	button1->setPosition(Size(visibleWidth / 4 * 3, visibleHeight / 2));
	button1->addTouchEventListener(CC_CALLBACK_2(LoginScene::changeScene, this));
	this->addChild(button1);

	text = Text::create();
	text->setFontSize(30);
	text->setPosition(Size(visibleWidth / 2, visibleHeight / 4));
	text->setString("header & body");
	this->addChild(text, 2);

    return true;
}

void LoginScene::changeScene(Ref * pSender, Widget::TouchEventType type) {
	Director::getInstance()->replaceScene(GameScene::createScene());
}

void LoginScene::login(Ref * pSender, Widget::TouchEventType type)
{
	if (type != Widget::TouchEventType::BEGAN)
		return;
	const char * username = textField->getStringValue().c_str();
	char postData[40];
	sprintf(postData, "username=%s", username);

	HttpRequest* request = new HttpRequest();
	request->setUrl("http://localhost:8080/login");
	request->setRequestType(HttpRequest::Type::POST);
	request->setResponseCallback(CC_CALLBACK_2(LoginScene::onHttpRequestCompleted, this));
	request->setRequestData(postData, strlen(postData));

	std::vector<std::string> headers;
	headers.push_back("Content-Type: application/x-www-form-urlencoded; charset=UTF-8");
	request->setHeaders(headers);

	cocos2d::network::HttpClient::getInstance()->send(request);
	request->release();
}

void LoginScene::onHttpRequestCompleted(HttpClient * sender, HttpResponse * response)
{
	if (!response) {
		textField->setString("not 200");
		return;
	}
	auto responseData = response->getResponseData();
	string str = Global::toString(responseData);
	rapidjson::Document d;
	d.Parse<0>(str.c_str());
	if (d.IsObject() && d["result"].GetBool() == true) {
		auto responseHeader = response->getResponseHeader();
		string header = Global::toString(responseHeader);
		Global::gameSessionId = Global::getSessionIdFromHeader(header);
		text->setString("header:" + header + "\n" + "body:" + str);
	}

	CCLOG(str.c_str());
	// Director::getInstance()->replaceScene(GameScene::createScene());
}

