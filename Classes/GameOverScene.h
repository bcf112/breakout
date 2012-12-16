#include "cocos2d.h"

using namespace cocos2d;

class GameOverLayer : public CCLayerColor
{
public:
	CREATE_FUNC(GameOverLayer);
	CC_SYNTHESIZE_READONLY(CCLabelTTF*, _label, Label);

	virtual ~GameOverLayer();
	bool init();
	void gameOverDone();
};

class GameOverScene : public CCScene
{
public:
	CREATE_FUNC(GameOverScene);
	//SCENE_NODE_FUNC(GameOverScene);
	CC_SYNTHESIZE_READONLY(GameOverLayer*, _layer, Layer);

	~GameOverScene();
	bool init();
};