#include "cocos2d.h"

using namespace cocos2d;

class StartMenuLayer:public CCLayerColor
{
public:
	CREATE_FUNC(StartMenuLayer);
	bool init();
	~StartMenuLayer();

	void menuCloseCallback(CCObject* pSender);
	void menuStartCallback(CCObject* pSender);
};

class StartMenuScene:public CCScene
{
public:
	CREATE_FUNC(StartMenuScene);
	~StartMenuScene();
	bool init();

	CC_SYNTHESIZE_READONLY(StartMenuLayer*, _layer, Layer);
};