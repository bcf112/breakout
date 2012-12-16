//pixel�� box2d�� meter ������ �ٲٱ� ���� PTM_RATIO�� ũ�⸦ ������.
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

		// ���̾ �� sprite �߰�
		CCSize winSize=CCDirector::sharedDirector()->getWinSize();
		CCSprite *ball=CCSprite::create("ball.jpg");	
		ball->setTag(1);
		ball->setPosition(ccp(100, 100));
		this->addChild(ball);

		//
		// box2d world �����
		//

		// box2d���� ��ü��� ���� �ùķ��̼��� �����ϴ� ���� world object�̴�.
		// body definition�� body�� ��ġ�� �ӵ� ���� �Ӽ����� ����ϱ� ���� ���
		// world object�� ������� body definition�� ����ϸ鼭 body object�� ����� �ȴ�.
		// shape�� �ùķ���Ʈ �ϰ��� �ϴ� ���������� ���(?)�� ��Ÿ����.
		// fixture definition shape�� �е��� ������ ���� ������ ���Ѵ�.
		// fixture object�� fixture definition�� ����ϸ鼭 body object�� ���� �� �ִ�.
		// �ϳ��� body object�� ���� fixture object�� ���� �� �ִ�.������ ��ü�� ���� �� ���� ���̴�.
		// step �Լ��� ���� �ùķ��̼��� �����ϰ� �ϴ� ������ �Ѵ�.

		// �߷� ���� y������ -30�� ������ body object���� �ٴ����� ������ ���̴�. 
		b2Vec2 gravity=b2Vec2(0.0f, 0.0f);
		bool doSleep=true;
		_world=new b2World(gravity);

		//
		// ȭ�� �𼭸� �����
		//

		b2BodyDef groundBodyDef;
		groundBodyDef.position.Set(0, 0);
		_groundBody=_world->CreateBody(&groundBodyDef);
		//��ũ���� �𼭸��� �����. �� ������ setAsEdge�� �������� ��� Set�� ��� �Ѵ�.

		b2EdgeShape groundBox;
		b2FixtureDef groundBoxDef;
		groundBoxDef.shape=&groundBox;
		//b2Vec2(0, 0) -> �Ʒ��ʿ��� ����
		//b2Vec2(winSize.width/PTM_RATIO) -> �Ʒ��ʿ��� ������
		groundBox.Set(b2Vec2(0, 0), b2Vec2(winSize.width/PTM_RATIO, 0));
		_bottomFixture=_groundBody->CreateFixture(&groundBoxDef);
		groundBox.Set(b2Vec2(0, 0), b2Vec2(0, winSize.height/PTM_RATIO));
		_groundBody->CreateFixture(&groundBoxDef);
		groundBox.Set(b2Vec2(0, winSize.height/PTM_RATIO), b2Vec2(winSize.width/PTM_RATIO, winSize.height/PTM_RATIO));
		_groundBody->CreateFixture(&groundBoxDef);
		groundBox.Set(b2Vec2(winSize.width/PTM_RATIO, winSize.height/PTM_RATIO), b2Vec2(winSize.width/PTM_RATIO, 0));
		_groundBody->CreateFixture(&groundBoxDef);

		//
		// �� body�� shape �����
		//
		b2BodyDef ballBodyDef;
		//dynamic body�� �����ؾ� �ùķ���Ʈ�� �ȴ�. 
		//�̷��� ������ �����ָ� �⺻ ������ static body�� �ȴ�. 
		//static body�� ���������� �ʰ� �ùķ���Ʈ ������ �ʴ´�.
		ballBodyDef.type=b2_dynamicBody;
		ballBodyDef.position.Set(100/PTM_RATIO, 100/PTM_RATIO);
		//user data�� �����ڰ� ���� sprite�� ����ִ´�.
		ballBodyDef.userData=ball;
		b2Body *ballBody=_world->CreateBody(&ballBodyDef);

		//�� ��� �����
		b2CircleShape circle;
		circle.m_radius=26.0/PTM_RATIO;

		//Density : �е�(mass per unit volume), Density�� ������ mass�� �������� �����̱Ⱑ �������.
		//Friction : ������ 0~1 ���̷� �� �� �ִ�. 0�� �������� ���°���.
		//Restitution : ź����(�ۿ�, ���ۿ뿡��), 0~1 ������ ���� ����. 1�� ���� ź��
		b2FixtureDef ballShapeDef;
		ballShapeDef.shape=&circle;
		ballShapeDef.density=1.0f;
		ballShapeDef.friction=0.f;
		ballShapeDef.restitution=1.0f;
		_ballFixture=ballBody->CreateFixture(&ballShapeDef);

		//����� ���ϴ� �κ�. Ư�� �������� ���� �����̱� �����Ѵ�. 10 10�̴� ������ ���������� ������ ���̴�.
		b2Vec2 force=b2Vec2(10, 10);
		ballBody->ApplyLinearImpulse(force, ballBodyDef.position);

		//
		// ��(paddle) �߰�
		//
		CCSprite *paddle=CCSprite::create("Paddle.jpg");
		paddle->setPosition(ccp(winSize.width/2, 50));
		this->addChild(paddle);

		//paddle body �����
		b2BodyDef paddleBodyDef;
		paddleBodyDef.type=b2_dynamicBody;
		paddleBodyDef.position.Set(winSize.width/2/PTM_RATIO, 50/PTM_RATIO);
		paddleBodyDef.userData=paddle;
		_paddleBody=_world->CreateBody(&paddleBodyDef);

		//paddle shape �����
		b2PolygonShape paddleShape;
		paddleShape.SetAsBox(paddle->getContentSize().width/PTM_RATIO/2, paddle->getContentSize().height/2/PTM_RATIO);

		//shape definition�� body�� �߰��ϱ�
		b2FixtureDef paddleShapeDef;
		paddleShapeDef.shape=&paddleShape;
		paddleShapeDef.density=10.0f;
		paddleShapeDef.friction=0.4f;
		paddleShapeDef.restitution=0.1f;
		_paddleFixture=_paddleBody->CreateFixture(&paddleShapeDef);

		//paddle�� ������ �����ϱ�
		b2PrismaticJointDef jointDef;
		b2Vec2 worldAxis(1.0f, 0.0f);
		jointDef.collideConnected=true;
		jointDef.Initialize(_paddleBody, _groundBody, _paddleBody->GetWorldCenter(), worldAxis);
		_world->CreateJoint(&jointDef);


		_contactListener=new MyContactListener();
		_world->SetContactListener(_contactListener);

		//block�� �߰�����
		for(int i=0; i<4; i++){
			for(int j=0; j<2; j++){
			static int xPadding=20;
			static int yPadding=40;

			//layer�� block �����
			CCSprite *block=CCSprite::create("block.jpg");
			int xOffset=xPadding+block->getContentSize().width/2 + ((block->getContentSize().width+xPadding)*i);
			int yOffset=yPadding+block->getContentSize().height/2 + ((block->getContentSize().height+yPadding)*j);
			block->setPosition(ccp(xOffset, 250-yOffset));
			block->setTag(2);
			this->addChild(block);

			//block body �����
			b2BodyDef blockBodyDef;
			blockBodyDef.type=b2_dynamicBody;
			blockBodyDef.position.Set(xOffset/PTM_RATIO, 250/PTM_RATIO);
			blockBodyDef.userData=block;
			b2Body *blockBody=_world->CreateBody(&blockBodyDef);

			//block shape �����
			b2PolygonShape blockShape;
			blockShape.SetAsBox(block->getContentSize().width/PTM_RATIO/2, block->getContentSize().height/PTM_RATIO/2);

			//shape definition�� body�� �߰��ϱ�
			b2FixtureDef blockShapeDef;
			blockShapeDef.shape=&blockShape;
			blockShapeDef.density=10.0;
			blockShapeDef.friction=0.0;
			blockShapeDef.restitution=0.1f;
			blockBody->CreateFixture(&blockShapeDef);

			//paddle�� ������ �����ϱ�
			/*b2PrismaticJointDef jointDef;
			b2Vec2 worldAxis(1.0f, 0.0f);
			jointDef.collideConnected=false;
			jointDef.Initialize(blockBody, _groundBody, _groundBody->GetWorldCenter(), worldAxis);
			_world->CreateJoint(&jointDef);*/
			}
		}

		//�������
		CocosDenshion::SimpleAudioEngine::sharedEngine()->playBackgroundMusic("background-music-aac.caf");

		//pause ���� ����
		HelloWorld::isPaused=false;

		//ProgressTimer �����ϱ�
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

	//MouseJoint�� body�� Ư���� ������ �̵���Ű�� ���� ����Ѵ�.
	if(_paddleFixture->TestPoint(locationWorld)){
		b2MouseJointDef md;
		//bodyA�� bodyB�� box2d���� bodyA�� bodyB�� ������ body�� collision ó�� �϶�� �ǹ��̴�.
		//collision ó���� ���� ������ paddle�� �������� �� ȭ�� ������ ���������� ��찡 �����.
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

	//�¸� �Ǻ�
	bool blockFound=false;

	//step �Լ��� �ӵ��� ��ġ�� iteration�� ���� �� �ִ�. ���� 8-10 ���̷� 
	_world->Step(dt, 10, 10);

	//��� body object���� ���鼭 user data set�� ã�´�. �̷��� �ϸ鼭
	//�� �����Ӹ��� ������ ��ġ�� ������Ʈ ��Ų��.
	for(b2Body *b=_world->GetBodyList(); b; b=b->GetNext()){
		if(b->GetUserData()!=NULL){
			CCSprite *ballData=(CCSprite *)b->GetUserData();
			ballData->setPosition(ccp(b->GetPosition().x * PTM_RATIO,
				b->GetPosition().y * PTM_RATIO));
			ballData->setRotation(-1 * CC_RADIANS_TO_DEGREES(b->GetAngle()));
		
			if(ballData->getTag()==1){
				static int maxSpeed=10;

				//���� setLinearVelocity()�� ȣ���ؼ� �ӵ��� ������ ���� ������ damping���� �����ؾ� collision �ùķ��̼ǿ� ������ ���� �ʴ´�.
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


	// ���� �ٴڰ� ��Ҵ��� Ȯ���Ѵ�. ���� ������ �νñ�
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

		//fixture�κ��� body�� ���� �� �ִ�.
		b2Body *bodyA=contact.fixtureA->GetBody();
		b2Body *bodyB=contact.fixtureB->GetBody();
		if(bodyA->GetUserData()!=NULL && bodyB->GetUserData()!=NULL){
			CCSprite *spriteA=(CCSprite *)bodyA->GetUserData();
			CCSprite *spriteB=(CCSprite *)bodyB->GetUserData();

			//SpriteA�� ���̰�, SpriteB�� ��
			if(spriteA->getTag()==1 && spriteB->getTag()==2){
				if(std::find(toDestory.begin(), toDestory.end(), bodyB)==toDestory.end()){
					toDestory.push_back(bodyB);
				}
			}

			//SpriteA�� ���̰� SpriteB�� ���� ��
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

