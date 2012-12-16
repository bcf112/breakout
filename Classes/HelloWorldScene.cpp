//pixel을 box2d의 meter 단위로 바꾸기 위해 PTM_RATIO로 크기를 나눈다.
#define PTM_RATIO 32.0

#include "HelloWorldScene.h"
#include "GameOverScene.h"
#include <SimpleAudioEngine.h>

using namespace cocos2d;

CCScene* HelloWorld::scene()
{
    CCScene * scene = NULL;
    do 
    {
        // 'scene' is an autorelease object
        scene = CCScene::create();
        CC_BREAK_IF(! scene);

        // 'layer' is an autorelease object
        HelloWorld *layer = HelloWorld::create();
        CC_BREAK_IF(! layer);

        // add layer as a child to scene
        scene->addChild(layer);
    } while (0);

    // return the scene
    return scene;
}

// on "init" you need to initialize your instance
bool HelloWorld::init()
{
    bool bRet = false;
    do 
    {
        //////////////////////////////////////////////////////////////////////////
        // super init first
        //////////////////////////////////////////////////////////////////////////

        CC_BREAK_IF(! CCLayer::init());

        //////////////////////////////////////////////////////////////////////////
        // add your codes below...
        //////////////////////////////////////////////////////////////////////////

        // 1. Add a menu item with "X" image, which is clicked to quit the program.

        // Create a "close" menu item with close icon, it's an auto release object.
        CCMenuItemImage *pCloseItem = CCMenuItemImage::create(
            "CloseNormal.png",
            "CloseSelected.png",
            this,
            menu_selector(HelloWorld::menuCloseCallback));
        CC_BREAK_IF(! pCloseItem);
		CCMenuItemImage *pPauseItem=CCMenuItemImage::create("Pause.png", NULL, this, menu_selector(HelloWorld::menuPauseCallback));

        // Place the menu item bottom-right conner.
        pCloseItem->setPosition(ccp(CCDirector::sharedDirector()->getWinSize().width - 20, 20));
		pPauseItem->setPosition(ccp(CCDirector::sharedDirector()->getWinSize().width - 70, 20));

        // Create a menu with the "close" menu item, it's an auto release object.
		CCMenu* pMenu = CCMenu::create(pCloseItem, pPauseItem, NULL);
        pMenu->setPosition(CCPointZero);
        CC_BREAK_IF(! pMenu);

        // Add the menu to HelloWorld layer as a child layer.
        this->addChild(pMenu, 1);
	
		_mouseJoint=NULL;

		// 레이어에 공 sprite 추가
		CCSize winSize=CCDirector::sharedDirector()->getWinSize();
		CCSprite *ball=CCSprite::create("ball.jpg");	
		ball->setTag(1);
		ball->setPosition(ccp(100, 100));
		this->addChild(ball);

		//
		// box2d world 만들기
		//

		// box2d에서 객체들과 물리 시뮬레이션을 조절하는 것이 world object이다.
		// body definition은 body의 위치나 속도 같은 속성값을 명시하기 위해 사용
		// world object를 만드려면 body definition을 명시하면서 body object를 만들면 된다.
		// shape은 시뮬레이트 하고자 하는 기하학적인 모양(?)을 나타낸다.
		// fixture definition shape의 밀도나 마찰과 같은 성질을 정한다.
		// fixture object는 fixture definition을 명시하면서 body object에 만들 수 있다.
		// 하나의 body object에 많은 fixture object를 넣을 수 있다.복잡한 객체를 만들 때 편리할 것이다.
		// step 함수는 물리 시뮬레이션이 동작하게 하는 역할을 한다.

		// 중력 생성 y축으로 -30을 줬으니 body object들은 바닥으로 떨어질 것이다. 
		b2Vec2 gravity=b2Vec2(0.0f, 0.0f);
		bool doSleep=true;
		_world=new b2World(gravity);

		//
		// 화면 모서리 만들기
		//

		b2BodyDef groundBodyDef;
		groundBodyDef.position.Set(0, 0);
		_groundBody=_world->CreateBody(&groundBodyDef);
		//스크린의 모서리를 만든다. 구 버전의 setAsEdge는 없어지고 대신 Set을 써야 한다.

		b2EdgeShape groundBox;
		b2FixtureDef groundBoxDef;
		groundBoxDef.shape=&groundBox;
		//b2Vec2(0, 0) -> 아랫쪽에서 왼쪽
		//b2Vec2(winSize.width/PTM_RATIO) -> 아랫쪽에서 오른쪽
		groundBox.Set(b2Vec2(0, 0), b2Vec2(winSize.width/PTM_RATIO, 0));
		_bottomFixture=_groundBody->CreateFixture(&groundBoxDef);
		groundBox.Set(b2Vec2(0, 0), b2Vec2(0, winSize.height/PTM_RATIO));
		_groundBody->CreateFixture(&groundBoxDef);
		groundBox.Set(b2Vec2(0, winSize.height/PTM_RATIO), b2Vec2(winSize.width/PTM_RATIO, winSize.height/PTM_RATIO));
		_groundBody->CreateFixture(&groundBoxDef);
		groundBox.Set(b2Vec2(winSize.width/PTM_RATIO, winSize.height/PTM_RATIO), b2Vec2(winSize.width/PTM_RATIO, 0));
		_groundBody->CreateFixture(&groundBoxDef);

		//
		// 공 body와 shape 만들기
		//
		b2BodyDef ballBodyDef;
		//dynamic body로 설정해야 시뮬레이트가 된다. 
		//이렇게 설정을 안해주면 기본 값으로 static body가 된다. 
		//static body는 움직이지도 않고 시뮬레이트 되지도 않는다.
		ballBodyDef.type=b2_dynamicBody;
		ballBodyDef.position.Set(100/PTM_RATIO, 100/PTM_RATIO);
		//user data에 개발자가 만든 sprite를 집어넣는다.
		ballBodyDef.userData=ball;
		b2Body *ballBody=_world->CreateBody(&ballBodyDef);

		//원 모양 만들기
		b2CircleShape circle;
		circle.m_radius=26.0/PTM_RATIO;

		//Density : 밀도(mass per unit volume), Density가 높으면 mass가 높아져서 움직이기가 어려워짐.
		//Friction : 마찰력 0~1 사이로 줄 수 있다. 0은 마찰력이 없는것임.
		//Restitution : 탄성력(작용, 반작용에서), 0~1 사이의 값을 가짐. 1이 완전 탄성
		b2FixtureDef ballShapeDef;
		ballShapeDef.shape=&circle;
		ballShapeDef.density=1.0f;
		ballShapeDef.friction=0.f;
		ballShapeDef.restitution=1.0f;
		_ballFixture=ballBody->CreateFixture(&ballShapeDef);

		//충격을 가하는 부분. 특정 방향으로 공이 움직이기 시작한다. 10 10이니 오른쪽 윗방향으로 움직일 것이다.
		b2Vec2 force=b2Vec2(10, 10);
		ballBody->ApplyLinearImpulse(force, ballBodyDef.position);

		//
		// 바(paddle) 추가
		//
		CCSprite *paddle=CCSprite::create("Paddle.jpg");
		paddle->setPosition(ccp(winSize.width/2, 50));
		this->addChild(paddle);

		//paddle body 만들기
		b2BodyDef paddleBodyDef;
		paddleBodyDef.type=b2_dynamicBody;
		paddleBodyDef.position.Set(winSize.width/2/PTM_RATIO, 50/PTM_RATIO);
		paddleBodyDef.userData=paddle;
		_paddleBody=_world->CreateBody(&paddleBodyDef);

		//paddle shape 만들기
		b2PolygonShape paddleShape;
		paddleShape.SetAsBox(paddle->getContentSize().width/PTM_RATIO/2, paddle->getContentSize().height/2/PTM_RATIO);

		//shape definition과 body에 추가하기
		b2FixtureDef paddleShapeDef;
		paddleShapeDef.shape=&paddleShape;
		paddleShapeDef.density=10.0f;
		paddleShapeDef.friction=0.4f;
		paddleShapeDef.restitution=0.1f;
		_paddleFixture=_paddleBody->CreateFixture(&paddleShapeDef);

		//paddle의 움직임 제한하기
		b2PrismaticJointDef jointDef;
		b2Vec2 worldAxis(1.0f, 0.0f);
		jointDef.collideConnected=true;
		jointDef.Initialize(_paddleBody, _groundBody, _paddleBody->GetWorldCenter(), worldAxis);
		_world->CreateJoint(&jointDef);


		_contactListener=new MyContactListener();
		_world->SetContactListener(_contactListener);

		//block을 추가하자
		for(int i=0; i<4; i++){
			for(int j=0; j<2; j++){
			static int xPadding=20;
			static int yPadding=40;

			//layer에 block 만들기
			CCSprite *block=CCSprite::create("block.jpg");
			int xOffset=xPadding+block->getContentSize().width/2 + ((block->getContentSize().width+xPadding)*i);
			int yOffset=yPadding+block->getContentSize().height/2 + ((block->getContentSize().height+yPadding)*j);
			block->setPosition(ccp(xOffset, 250-yOffset));
			block->setTag(2);
			this->addChild(block);

			//block body 만들기
			b2BodyDef blockBodyDef;
			blockBodyDef.type=b2_dynamicBody;
			blockBodyDef.position.Set(xOffset/PTM_RATIO, 250/PTM_RATIO);
			blockBodyDef.userData=block;
			b2Body *blockBody=_world->CreateBody(&blockBodyDef);

			//block shape 만들기
			b2PolygonShape blockShape;
			blockShape.SetAsBox(block->getContentSize().width/PTM_RATIO/2, block->getContentSize().height/PTM_RATIO/2);

			//shape definition과 body에 추가하기
			b2FixtureDef blockShapeDef;
			blockShapeDef.shape=&blockShape;
			blockShapeDef.density=10.0;
			blockShapeDef.friction=0.0;
			blockShapeDef.restitution=0.1f;
			blockBody->CreateFixture(&blockShapeDef);

			//paddle의 움직임 제한하기
			/*b2PrismaticJointDef jointDef;
			b2Vec2 worldAxis(1.0f, 0.0f);
			jointDef.collideConnected=false;
			jointDef.Initialize(blockBody, _groundBody, _groundBody->GetWorldCenter(), worldAxis);
			_world->CreateJoint(&jointDef);*/
			}
		}

		//배경음악
		CocosDenshion::SimpleAudioEngine::sharedEngine()->playBackgroundMusic("background-music-aac.caf");

		//pause 상태 감시
		HelloWorld::isPaused=false;

		//ProgressTimer 생성하기
		CCSprite *progressImage=CCSprite::create("paddle.jpg");
		CCProgressTimer *progressTimeBar=CCProgressTimer::create(progressImage);
		progressTimeBar->setType(kCCProgressTimerTypeBar);
		progressTimeBar->setPosition(ccp(40, 10));
		progressTimeBar->setPercentage(100.0f);
		this->addChild(progressTimeBar);

		CCCallFunc *cbTimerFinish=CCCallFunc::create(this, callfunc_selector(HelloWorld::gameOverCallback));
		CCProgressFromTo *progressToZero=CCProgressFromTo::create(30.0f, 100.0f, 0.0f);
		CCFiniteTimeAction *sequence=CCSequence::create(progressToZero, cbTimerFinish, NULL);
		
		progressTimeBar->runAction(sequence);

		this->schedule(schedule_selector(HelloWorld::tick));
		this->setTouchEnabled(true);

        bRet = true;
    } while (0);

    return bRet;
}

void HelloWorld::ccTouchesBegan(CCSet *touches, CCEvent *pEvent){
	if(_mouseJoint!=NULL){
		return;
	}
	
	CCTouch *myTouch=(CCTouch *)touches->anyObject();
	CCPoint location=myTouch->getLocation();
	//location=CCDirector::sharedDirector()->convertToGL(location);
	b2Vec2 locationWorld=b2Vec2(location.x/PTM_RATIO, location.y/PTM_RATIO);

	//MouseJoint는 body를 특정한 점으로 이동시키기 위해 사용한다.
	if(_paddleFixture->TestPoint(locationWorld)){
		b2MouseJointDef md;
		//bodyA와 bodyB는 box2d에게 bodyA와 bodyB로 지정한 body를 collision 처리 하라는 의미이다.
		//collision 처리를 하지 않으면 paddle을 움직였을 때 화면 밖으로 나가버리는 경우가 생긴다.
		md.bodyA=_groundBody;
		md.bodyB=_paddleBody;
		md.target=locationWorld;
		md.collideConnected=true;
		md.maxForce=1000.0f * _paddleBody->GetMass();

		_mouseJoint=(b2MouseJoint *)_world->CreateJoint(&md);
		_paddleBody->SetAwake(true);
	}
}

void HelloWorld::ccTouchesMoved(CCSet *touches, CCEvent *pEvent){
	if(_mouseJoint==NULL){
		return;
	}

	CCTouch *myTouch=(CCTouch *)touches->anyObject();
	CCPoint location=myTouch->getLocation();
	//location=CCDirector::sharedDirector()->convertToGL(location);
	b2Vec2 locationWorld=b2Vec2(location.x/PTM_RATIO, location.y/PTM_RATIO);

	_mouseJoint->SetTarget(locationWorld);
}

void HelloWorld::ccTouchesCancelled(CCSet *touches, CCEvent *pEvent){
	if(_mouseJoint){
		_world->DestroyJoint(_mouseJoint);
		_mouseJoint=NULL;
	}
}

void HelloWorld::ccTouchesEnded(CCSet *touches, CCEvent *pEvent){
	if(_mouseJoint){
		_world->DestroyJoint(_mouseJoint);
		_mouseJoint=NULL;
	}
}

void HelloWorld::menuPauseCallback(CCObject* pSender){
	if(isPaused){
		CCDirector::sharedDirector()->resume();
		isPaused=false;
	}else{
		CCDirector::sharedDirector()->pause();
		isPaused=true;
	}
}

void HelloWorld::menuCloseCallback(CCObject* pSender)
{
    // "close" menu item clicked
    CCDirector::sharedDirector()->end();
}

void HelloWorld::tick(float dt){

	//승리 판별
	bool blockFound=false;

	//step 함수로 속도와 위치의 iteration을 정할 수 있다. 값은 8-10 사이로 
	_world->Step(dt, 10, 10);

	//모든 body object들을 돌면서 user data set을 찾는다. 이렇게 하면서
	//매 프레임마다 각도와 위치를 업데이트 시킨다.
	for(b2Body *b=_world->GetBodyList(); b; b=b->GetNext()){
		if(b->GetUserData()!=NULL){
			CCSprite *ballData=(CCSprite *)b->GetUserData();
			ballData->setPosition(ccp(b->GetPosition().x * PTM_RATIO,
				b->GetPosition().y * PTM_RATIO));
			ballData->setRotation(-1 * CC_RADIANS_TO_DEGREES(b->GetAngle()));
		
			if(ballData->getTag()==1){
				static int maxSpeed=10;

				//직접 setLinearVelocity()를 호출해서 속도를 조절할 수도 있지만 damping으로 조절해야 collision 시뮬레이션에 영향을 주지 않는다.
				b2Vec2 velocity=b->GetLinearVelocity();
				float speed=velocity.Length();

				if(speed > maxSpeed){
					b->SetLinearDamping(0.5);
				}else if(speed < maxSpeed){
					b->SetLinearDamping(0.0);
				}
			}
			if(ballData->getTag()==2){
				blockFound=true;
			}
		}
	}


	// 공이 바닥과 닿았는지 확인한다. 블럭에 닿으면 부시기
	std::vector<b2Body *>toDestory;
	std::vector<MyContact>::iterator pos;
	for(pos=_contactListener->_contacts.begin(); pos!=_contactListener->_contacts.end(); ++pos){
		MyContact contact=*pos;

		if((contact.fixtureA==_bottomFixture && contact.fixtureB==_ballFixture) ||
			(contact.fixtureA==_ballFixture && contact.fixtureB==_bottomFixture)){
				GameOverScene *gameOverScene=GameOverScene::create();
				gameOverScene->getLayer()->getLabel()->setString("You Lose!");
				CCDirector::sharedDirector()->replaceScene(gameOverScene);
		}

		//fixture로부터 body를 얻어올 수 있다.
		b2Body *bodyA=contact.fixtureA->GetBody();
		b2Body *bodyB=contact.fixtureB->GetBody();
		if(bodyA->GetUserData()!=NULL && bodyB->GetUserData()!=NULL){
			CCSprite *spriteA=(CCSprite *)bodyA->GetUserData();
			CCSprite *spriteB=(CCSprite *)bodyB->GetUserData();

			//SpriteA는 공이고, SpriteB는 블럭
			if(spriteA->getTag()==1 && spriteB->getTag()==2){
				if(std::find(toDestory.begin(), toDestory.end(), bodyB)==toDestory.end()){
					toDestory.push_back(bodyB);
				}
			}

			//SpriteA가 블럭이고 SpriteB가 공일 때
			if(spriteA->getTag()==2 && spriteB->getTag()==1){
				if(std::find(toDestory.begin(), toDestory.end(), bodyA)==toDestory.end()){
					toDestory.push_back(bodyA);
				}
			}
		}
	}

	std::vector<b2Body *>::iterator pos2;
	for(pos2=toDestory.begin(); pos2!=toDestory.end(); ++pos2){
		b2Body *body=*pos2;
		if(body->GetUserData()!=NULL){
			CCSprite *sprite=(CCSprite *)body->GetUserData();
			this->removeChild(sprite, true);
		}
		_world->DestroyBody(body);
	}

	if(toDestory.size()>0){
		CocosDenshion::SimpleAudioEngine::sharedEngine()->playEffect("blip.caf");
	}

	if(!blockFound){
		GameOverScene *gameOverScene=GameOverScene::create();
		gameOverScene->getLayer()->getLabel()->setString("You Win!");
		CCDirector::sharedDirector()->replaceScene(gameOverScene);
	}
}

void HelloWorld::gameOverCallback(){
	GameOverScene *gameOverScene=GameOverScene::create();
	gameOverScene->getLayer()->getLabel()->setString("You Lose!");
	CCDirector::sharedDirector()->replaceScene(gameOverScene);
}

HelloWorld::~HelloWorld()
{
	CC_SAFE_DELETE(_world);
	_groundBody=NULL;
	delete _contactListener;
}

