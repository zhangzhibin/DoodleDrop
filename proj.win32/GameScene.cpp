#include "GameScene.h"


using namespace cocos2d;

GameScene::GameScene()
{
	player = NULL;
	spiders = NULL;
}
GameScene::~GameScene()
{
	CCLog("~GameScene called");

	if( spiders != NULL )
	{
		spiders->removeAllObjects();
		spiders->release();
		spiders = NULL;
	}

}
CCScene* GameScene::scene()
{
    CCScene * scene = NULL;
    do 
    {
        // 'scene' is an autorelease object
        scene = CCScene::create();
        CC_BREAK_IF(! scene);

        // 'layer' is an autorelease object
        GameScene *layer = GameScene::create();
        CC_BREAK_IF(! layer);

        // add layer as a child to scene
        scene->addChild(layer);
    } while (0);

    // return the scene
    return scene;
}

// on "init" you need to initialize your instance
bool GameScene::init()
{
    bool bRet = false;
    do 
    {
        //////////////////////////////////////////////////////////////////////////
        // super init first
        //////////////////////////////////////////////////////////////////////////

        CC_BREAK_IF(! CCLayer::init());

		//CCTouchDispatcher::sharedDispatcher()->addTargetedDelegate(this, 0, true);
		CCDirector::sharedDirector()->getTouchDispatcher()->addTargetedDelegate( this, 0, true );

//		setIsTouchEnabled(true);

        //////////////////////////////////////////////////////////////////////////
        // add your codes below...
        //////////////////////////////////////////////////////////////////////////

        // 1. Add a menu item with "X" image, which is clicked to quit the program.

        // Create a "close" menu item with close icon, it's an auto release object.
        CCMenuItemImage *pCloseItem = CCMenuItemImage::create(
            "CloseNormal.png",
            "CloseSelected.png",
            this,
            menu_selector(GameScene::menuCloseCallback));
        CC_BREAK_IF(! pCloseItem);

        // Place the menu item bottom-right conner.
        pCloseItem->setPosition(ccp(CCDirector::sharedDirector()->getWinSize().width - 20, 20));

        // Create a menu with the "close" menu item, it's an auto release object.
        CCMenu* pMenu = CCMenu::create(pCloseItem, NULL);
        pMenu->setPosition(CCPointZero);
        CC_BREAK_IF(! pMenu);

        // Add the menu to HelloWorld layer as a child layer.
        this->addChild(pMenu, 1);
/*
        // 2. Add a label shows "Hello World".

        // Create a label and initialize with string "Hello World".
        CCLabelTTF* pLabel = CCLabelTTF::create("Hello World", "Arial", 24);
        CC_BREAK_IF(! pLabel);

        // Get window size and place the label upper. 
        CCSize size = CCDirector::sharedDirector()->getWinSize();
        pLabel->setPosition(ccp(size.width / 2, size.height - 50));

        // Add the label to HelloWorld layer as a child layer.
        this->addChild(pLabel, 1);


        // 3. Add add a splash screen, show the cocos2d splash image.
        CCSprite* pSprite = CCSprite::create("HelloWorld.png");
        CC_BREAK_IF(! pSprite);

        // Place the sprite on the center of the screen
        pSprite->setPosition(ccp(size.width/2, size.height/2));

        // Add the sprite to HelloWorld layer as a child layer.
        this->addChild(pSprite, 0);
*/
		// add player
		player = CCSprite::create( "alien.png" );
		this->addChild( player );
		float screenWidth = CCDirector::sharedDirector()->getWinSize().width;
		float playerHeight = player->getTexture()->getContentSize().height;
		player->setPosition( CCPointMake( screenWidth/2, playerHeight/2 ) );

		// add spiders
		scheduleUpdate();

		initSpiders();

        bRet = true;
    } while (0);

    return bRet;
}

void GameScene::menuCloseCallback(CCObject* pSender)
{
    // "close" menu item clicked
    CCDirector::sharedDirector()->end();
}

void GameScene::initSpiders()
{
	CCSize screenSize = CCDirector::sharedDirector()->getWinSize();

	CCSprite* spider = CCSprite::create( "spider.png" );
	float spiderWidth = spider->getTexture()->getContentSize().width;
	int numSpiders = screenSize.width / spiderWidth;
	
	this->spiders = CCArray::create();
	spiders->retain();
	this->spiders->initWithCapacity( numSpiders );

	int i = 0;
	for( i=0;i<numSpiders;i++ )
	{
		spider = CCSprite::create( "spider.png" );
		spiders->addObject( spider );
		this->addChild( spider );
	}
	
	resetSpiders();
}

void GameScene::resetSpiders()
{
	CCSize screenSize = CCDirector::sharedDirector()->getWinSize();
	CCSprite* spider = CCSprite::create( "spider.png" );
	CCSize spiderSize = spider->getTexture()->getContentSize();
	int numSpiders = spiders->count();
	for( int i=0;i<numSpiders;i++ )
	{
		spider = (CCSprite*) spiders->objectAtIndex( i );
		spider->setPosition( CCPointMake( spiderSize.width * (i+0.5), screenSize.height + spiderSize.height/2) );
		spider->stopAllActions();
	}
	this->unschedule( schedule_selector(GameScene::spidersUpdate) );
	this->schedule( schedule_selector(GameScene::spidersUpdate), 0.7f );
	numSpidersMoved = 0;
	spiderMoveDuration = 5.0f;
}

void GameScene::spidersUpdate( float delta )
{

//	CCLog( "GameScene::spidersUpdate()=> spiders->count: %d", spiders->count() );
/*
	float player_pos_x = player->getPositionX();
	float x = player_pos_x + 16.0f;
	if( x > 480.0f )
	{
		x = 16.0f;
	}
	player->setPositionX (x );
*/
	for( int i=0;i<10;i++ )
	{
		int randomSpiderIndex = CCRANDOM_0_1() * (spiders->count());
//		int randomSpiderIndex = CCRANDOM_0_1() * 15;
//		CCLog( "GameScene::spidersUpdate()=> index=%d", randomSpiderIndex );
		CCSprite* spider = (CCSprite*) spiders->objectAtIndex( randomSpiderIndex );
		if( spider->numberOfRunningActions() == 0 )
		{
			this->runSpiderMoveSequence( spider );
			break;
		}
	}

	checkForCollision();
}

void GameScene::runSpiderMoveSequence( CCSprite* spiderToMove )
{
	numSpidersMoved ++;
	if( numSpidersMoved % 8 == 0 && spiderMoveDuration > 2.0f )
	{
		spiderMoveDuration -= 0.1f;
	}
	CCPoint belowScreenPosition = CCPointMake( spiderToMove->getPositionX(), -(spiderToMove->getTexture()->getContentSize().height) );
	CCMoveTo* move = CCMoveTo::create( spiderMoveDuration, belowScreenPosition );
	CCAction* callDidDrop = CCCallFuncN::create( this, 
		callfuncN_selector( GameScene::spiderDidDrop ) );
	CCSequence* seq = CCSequence::create( move, callDidDrop, NULL );
	spiderToMove->runAction( seq );
}

void GameScene::spiderDidDrop( CCNode* pSender )
{
	CCSprite* spider = (CCSprite*) pSender;
	CCPoint spider_pos = spider->getPosition();
	CCSize screenSize = CCDirector::sharedDirector()->getWinSize();
	spider_pos.y = screenSize.height + spider->getTexture()->getContentSize().height;
	spider->setPosition( spider_pos );
}

void GameScene::movePlayer( CCPoint position )
{
	player->stopAllActions();

//	CCBlink* blink = CCBlink::create(0.1f, 1);
	
	CCPoint current_pos = player->getPosition();
	float distance = ccpDistance( current_pos, position );
	float speed = 100.0f; // 100 pixel per second
	float duration = distance / speed;

	CCMoveTo* move = CCMoveTo::create( duration, position );
	CCSequence* seq = CCSequence::create( move, NULL );
	player->runAction( seq );
}
// touch support
bool GameScene::ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent)
{
	CCPoint pos = pTouch->getLocation( );
	movePlayer( pos );

	return false;
}
void GameScene::ccTouchMoved(CCTouch *pTouch, CCEvent *pEvent)
{
	CCPoint pos = CCDirector::sharedDirector()->convertToGL(pTouch->getLocationInView());
	CCLog("ccTouchMoved @ (%f, %f)", pos.x, pos.y);
}
void GameScene::ccTouchEnded(CCTouch *pTouch, CCEvent *pEvent)
{
	CCPoint pos = CCDirector::sharedDirector()->convertToGL(pTouch->getLocationInView());
	CCLog("ccTouchEnded @ (%f, %f)", pos.x, pos.y);
}
void GameScene::ccTouchCancelled(CCTouch *pTouch, CCEvent *pEvent)
{
	CCPoint pos = CCDirector::sharedDirector()->convertToGL(pTouch->getLocationInView());
	CCLog("ccTouchCancelled @ (%f, %f)", pos.x, pos.y);
}

void GameScene::checkForCollision()
{
	CCPoint player_pos = player->getPosition();
	float player_radius = player->getTexture()->getContentSize().width;
	float spider_radius = ((CCSprite*) spiders->lastObject())->getTexture()->getContentSize().width;

	float player_collision_radius = player_radius * 0.4f;
	float spider_collision_radius = spider_radius * 0.4f;

	float max_collision_radius = player_collision_radius + spider_collision_radius;

	int numSpiders = spiders->count();
//	CCLog("checkForCollision=> player @(%f, %f), max_collision=%f", player_pos.x, player_pos.y, max_collision_radius );
	
	bool collision = false;
	for(int i=0;i<numSpiders;i++)
	{
		CCSprite* spider = (CCSprite*) spiders->objectAtIndex(i);
//		if( spider->numberOfRunningActions() <= 0 )
//		{
//			CCLog("checkForCollision=> spider #%d is not moving yet", i+1 );
//			continue;
//		}
		CCPoint spider_pos = spider->getPosition();
		float distance = ccpDistance( player_pos, spider_pos );
//		CCLog("checkForCollision=> spider #%d, distance=%f", i+1, distance );
		if( distance < max_collision_radius )
		{
			CCLog("checkForCollision=> hit!!! max=%f, dist=%f", max_collision_radius, distance);
			collision = true;
			break;
		}
	}

	if( collision )
	{
		CCBlink* blink = CCBlink::create( 1.0f, 10 );
		CCSequence* seq = CCSequence::create( blink, NULL );
		player->stopAllActions();
		player->runAction(seq);

		resetSpiders();
	}
}