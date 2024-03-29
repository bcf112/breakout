#ifndef __HELLOWORLD_SCENE_H__
#define __HELLOWORLD_SCENE_H__

#include "cocos2d.h"
#include "Box2D/Box2D.h"
#include "SimpleAudioEngine.h"
#include "MyContactListener.h"

using namespace cocos2d;

class HelloWorld : public cocos2d::CCLayer
{
public:
    // Here's a difference. Method 'init' in cocos2d-x returns bool, instead of returning 'id' in cocos2d-iphone
    virtual bool init();  

    // there's no 'id' in cpp, so we recommand to return the exactly class pointer
    static cocos2d::CCScene* scene();
    
    // a selector callback
    void menuCloseCallback(CCObject* pSender);
	void menuPauseCallback(CCObject* pSender);
	void gameOverCallback();

    // implement the "static node()" method manually
    CREATE_FUNC(HelloWorld);

	void tick(float dt);

	~HelloWorld();
	virtual void ccTouchesBegan(CCSet *touches, CCEvent *pEvent);
	virtual void ccTouchesMoved(CCSet *touches, CCEvent *pEvent);
	virtual void ccTouchesCancelled(CCSet *touches, CCEvent *pEvent);
	virtual void ccTouchesEnded(CCSet *touches, CCEvent *pEvent);

private:
	b2World *_world;
	b2Body *_groundBody;
	b2Fixture *_bottomFixture;
	b2Fixture *_ballFixture;
	b2Body *_paddleBody;
	b2Fixture *_paddleFixture;
	b2MouseJoint *_mouseJoint;

	MyContactListener *_contactListener;
	bool isPaused;

};

#endif  // __HELLOWORLD_SCENE_H__