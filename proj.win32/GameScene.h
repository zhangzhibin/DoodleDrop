#ifndef __GAME_SCENE_H__
#define __GAME_SCENE_H__

#include "cocos2d.h"
#include "SimpleAudioEngine.h"

class GameScene :
	public cocos2d::CCLayer
{
private:
	cocos2d::CCSprite * player;
	cocos2d::CCPoint playerVelocity;

	cocos2d::CCArray * spiders;
	float spiderMoveDuration;
	int numSpidersMoved;

public:
	GameScene();
	~GameScene();

    // Here's a difference. Method 'init' in cocos2d-x returns bool, instead of returning 'id' in cocos2d-iphone
    virtual bool init();

    // there's no 'id' in cpp, so we recommand to return the exactly class pointer
    static cocos2d::CCScene* scene();
    
    // a selector callback
    void menuCloseCallback(cocos2d::CCObject* pSender);

    // implement the "static node()" method manually
    CREATE_FUNC(GameScene);

	// player
	void movePlayer( cocos2d::CCPoint position );

	// spider 
	void initSpiders();
	void resetSpiders();
	void spidersUpdate( float delta );
	void runSpiderMoveSequence( cocos2d::CCSprite* spiderToMove );
	void spiderDidDrop( cocos2d::CCNode* pSender );

	// collision test
	void checkForCollision();

	// enable touch
	virtual bool ccTouchBegan(cocos2d::CCTouch *pTouch, cocos2d::CCEvent *pEvent);
    virtual void ccTouchMoved(cocos2d::CCTouch *pTouch, cocos2d::CCEvent *pEvent);
    virtual void ccTouchEnded(cocos2d::CCTouch *pTouch, cocos2d::CCEvent *pEvent);  
    virtual void ccTouchCancelled(cocos2d::CCTouch *pTouch, cocos2d::CCEvent *pEvent); 
};

#endif