#include "cocos2d.h"

using namespace cocos2d;

class GameOverLayer : public CCLayerColor
{
public:
	CREATE_FUNC(GameOverLayer);
	CC_SYNTHESIZE_READONLY(CCLabelTTF*, _label, Label);

	~GameOverLayer();
	bool init();
	void gameOverDone();
	void menuRestartCallback(CCObject* pSender);
};

class GameOverScene : public CCScene
{
public:
	CREATE_FUNC(GameOverScene);
	CC_SYNTHESIZE_READONLY(GameOverLayer*, _layer, Layer);

	~GameOverScene();
	bool init();
};