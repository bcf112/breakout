#include "GameOverScene.h"
#include "HelloWorldScene.h"

using namespace cocos2d;

bool GameOverLayer::init()
{
	if(!CCLayerColor::initWithColor(ccc4(0, 0, 0, 0))){
		return false;
	}

	CCSize winSize=CCDirector::sharedDirector()->getWinSize();
	this->_label=CCLabelTTF::create("", "Arial", 40);
	this->_label->retain();
	this->getLabel()->setColor(ccc3(255,255,255));
	this->getLabel()->setPosition(ccp(winSize.width/2, winSize.height/2));
	this->addChild(_label);

	CCMenuItemFont *restartItem=CCMenuItemFont::create("Restart", this, menu_selector(GameOverLayer::menuRestartCallback));
	restartItem->setPosition(0, 0);

	CCMenu *menu=CCMenu::create(restartItem, NULL);
	menu->setPosition(winSize.width/2, winSize.height/2 - 40);
	this->addChild(menu);

	//this->runAction(CCSequence::create(CCDelayTime::create(3), CCCallFunc::create(this, callfunc_selector(GameOverLayer::gameOverDone)), NULL));

	return true;
}

void GameOverLayer::menuRestartCallback(CCObject *pSender){
	gameOverDone();
}

void GameOverLayer::gameOverDone()
{
	CCDirector::sharedDirector()->replaceScene(HelloWorld::scene());
}

GameOverLayer::~GameOverLayer()
{
	_label->release();
}

bool GameOverScene::init()
{
	if(CCScene::init())
	{
		this->_layer=GameOverLayer::create();
		this->_layer->retain();
		this->addChild(_layer);
		return true;
	}else{
		return false;
	}
}

GameOverScene::~GameOverScene(){
	if(_layer){
		_layer->release();
		_layer=NULL;
	}
}