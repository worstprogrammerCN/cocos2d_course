#include "GameScene.h"
#include "json/rapidjson.h"
#include "json/document.h"
#include "json/writer.h"
#include "json/stringbuffer.h"
#include <regex>
#include <sstream>
using std::regex;
using std::match_results;
using std::regex_match;
using std::cmatch;
using namespace rapidjson;

USING_NS_CC;

cocos2d::Scene* GameScene::createScene() {
    // 'scene' is an autorelease object
    auto scene = Scene::create();

    // 'layer' is an autorelease object
    auto layer = GameScene::create();

    // add layer as a child to scene
    scene->addChild(layer);

    // return the scene
    return scene;
}

bool GameScene::init() {
    if (!Layer::init())
    {
        return false;
    }

    Size size = Director::getInstance()->getVisibleSize();
    visibleHeight = size.height;
    visibleWidth = size.width;

    score_field = TextField::create("input score / top n rank", "Arial", 30);
    score_field->setPosition(Size(visibleWidth / 2, visibleHeight / 4 * 3));
    this->addChild(score_field, 2);

	highest_score = Text::create();
	highest_score->setFontSize(30);
	highest_score->setPosition(Size(visibleWidth / 2, visibleHeight / 2));
	highest_score->setString("highest score / top n scores");
	this->addChild(highest_score, 2);

    submit_button = Button::create();
    submit_button->setTitleText("Submit");
    submit_button->setTitleFontSize(30);
    submit_button->setPosition(Size(visibleWidth / 4, visibleHeight / 4));
	submit_button->addTouchEventListener(CC_CALLBACK_2(GameScene::submit, this));
    this->addChild(submit_button, 2);

    rank_field = TextField::create("", "Arial", 30);
    rank_field->setPosition(Size(visibleWidth / 4 * 3, visibleHeight / 4 * 3));
    this->addChild(rank_field, 2);

    rank_button = Button::create();
    rank_button->setTitleText("Rank");
    rank_button->setTitleFontSize(30);
    rank_button->setPosition(Size(visibleWidth / 4 * 3, visibleHeight / 4));
	rank_button->addTouchEventListener(CC_CALLBACK_2(GameScene::rank, this));
	
    this->addChild(rank_button, 2);

    return true;
}

void GameScene::submit(Ref * pSender, Widget::TouchEventType type)
{
	if (type != Widget::TouchEventType::BEGAN)
		return;
	auto score = score_field->getStringValue().c_str();
	char scoreData[40];
	sprintf(scoreData, "score=%s", score);

	HttpRequest* request = new HttpRequest();
	request->setUrl("http://localhost:8080/submit");
	request->setRequestType(HttpRequest::Type::POST);

	request->setRequestData(scoreData, strlen(scoreData));
	std::vector<std::string> headers;
	headers.push_back("Content-Type: application/x-www-form-urlencoded; charset=UTF-8");
	headers.push_back("Cookies: GAMESESSIONID=" + Global::gameSessionId);
	request->setHeaders(headers);

	request->setResponseCallback(CC_CALLBACK_2(GameScene::onHttpRequestCompleted, this));

	cocos2d::network::HttpClient::getInstance()->send(request);
	request->release();
}

void GameScene::rank(Ref * pSender, Widget::TouchEventType type)
{
	if (type != Widget::TouchEventType::BEGAN)
		return;
	string rank = score_field->getString();
	string rankData;
	std::stringstream ss;
	string random;
	ss << rand();
	ss >> random;
	rankData += "top=" + rank + "&rand=" + random;

	HttpRequest* request = new HttpRequest();
	request->setUrl("http://localhost:8080/rank?" + rankData);
	request->setRequestType(HttpRequest::Type::GET);

	std::vector<std::string> headers;
	headers.push_back("Content-Type: application/x-www-form-urlencoded; charset=UTF-8");
	headers.push_back("Cookies: GAMESESSIONID=" + Global::gameSessionId);
	request->setHeaders(headers);

	request->setResponseCallback(CC_CALLBACK_2(GameScene::onHttpRequestCompleted1, this));

	cocos2d::network::HttpClient::getInstance()->send(request);
	request->release();
	
}

void GameScene::onHttpRequestCompleted(HttpClient *sender, HttpResponse *response) {
	auto responseData = response->getResponseData();
	string str = Global::toString(responseData);
	rapidjson::Document d;
	d.Parse<0>(str.c_str());
	if (d.IsObject() && d["result"].GetBool() == true) {
		auto score = d["info"].GetString();
		auto scoreText = std::string("highest score : ");
		scoreText += score;
		highest_score->setString(scoreText.c_str());
		CCLOG(d["info"].GetString());
	}
}

void GameScene::onHttpRequestCompleted1(HttpClient * sender, HttpResponse * response)
{
	auto responseData = response->getResponseData();
	string str = Global::toString(responseData);
	rapidjson::Document d;
	d.Parse<0>(str.c_str());
	CCLOG(str.c_str());
	if (d.IsObject() && d["result"].GetBool() == true) {
		auto ranks = d["info"].GetString();
		auto scoreText = std::string("top students : ");
		scoreText += ranks;
		highest_score->setString(scoreText.c_str());
		CCLOG(ranks);
	}
}

