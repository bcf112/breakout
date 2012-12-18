#include "StartMenuScene.h"
#include "HelloWorldScene.h"

using namespace cocos2d;

bool StartMenuLayer::init()
{
	if(!CCLayerColor::initWithColor(ccc4(0, 0, 0, 0))){
		return false;
	}

	CCSize winSize=CCDirector::sharedDirector()->getWinSize();

	CCMenuItemFont *startItem=CCMenuItemFont::create("Start", this, menu_selector(StartMenuLayer::menuStartCallback));
	CCMenuItemFont *closeItem=CCMenuItemFont::create("Exit", this, menu_selector(StartMenuLayer::menuCloseCallback));

	startItem->setPosition(0, 0);
	closeItem->setPosition(0, -30);

	CCMenu *menu=CCMenu::create(startItem, closeItem, NULL);
	menu->setPosition(winSize.width/2, winSize.height/2);
	this->addChild(menu);

	CCLabelTTF *label=CCLabelTTF::create("BreakOut", "Arial", 40);
	label->setColor(ccc3(255, 255, 255));
	label->setPosition(ccp(winSize.width/2, winSize.height/2 + 50));
	this->addChild(label);
	
	return true;
}

StartMenuLayer::~StartMenuLayer(){
}

void StartMenuLayer::menuStartCallback(CCObject *pSender){
	CCScene *helloWorldScene=HelloWorld::scene();
	CCDirector::sharedDirector()->replaceScene(helloWorldScene);
}

void StartMenuLayer::menuCloseCallback(CCObject *pSender){
	CCDirector::sharedDirector()->end();
}

bool StartMenuScene::init()
{
	if(CCScene::init())
	{
		this->_layer=StartMenuLayer::create();
		this->_layer->retain();
		this->addChild(_layer);
		return true;
	}else{
		return false;
	}
}

StartMenuScene::~StartMenuScene(){
	if(_layer){
		_layer->release();
		_layer=NULL;
	}
}

	
	
