#ifndef _LEVELSELECTSCENE_H_
#define  _LEVELSELECTSCENE_H_
#include "cocos2d.h"
#include "cocos-ext.h"
#include "PuzzleCreator.h"
#include "PuzzleScene.h"
USING_NS_CC;
USING_NS_CC_EXT;

class ScMenuItem : public CCMenuItemSprite
{
public:
	static ScMenuItem*create(CCNode* normalSprite, CCNode* selectedSprite, CCObject* target, SEL_MenuHandler selector);
	void setLevel(int lvl){m_nLevel = lvl;}
	int getLevel(){return m_nLevel;}
private:
	int m_nLevel;
};

class ScMenu :public CCMenu
{
public:
	virtual void registerWithTouchDispatcher()
	{
		CCDirector::sharedDirector()->getTouchDispatcher() ->addTargetedDelegate(this,1,true);
	}
	static ScMenu * create(CCMenuItem* item, CCMenuItem* item2);
	bool ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent);
	void ccTouchMoved(CCTouch *pTouch, CCEvent *pEvent);
	void ccTouchEnded(CCTouch *pTouch, CCEvent *pEvent);
    bool isMoving(){return move_;}
private:
	bool move_;
};

struct ViewPosition
{
    float value;
};

class LevelSelectorHeader : public cocos2d::CCLayer
{
public:
    static LevelSelectorHeader*create(int length);
    bool initWithLength(int length);
    
    void setScrollView(CCScrollView * view){m_scrollView = view;}
private:
    void menuScrollDownButtonCallback(CCObject*pSender);
    void menuScrollUpButtonCallback(CCObject*pSender);
    CCScrollView * m_scrollView;
};

class LevelSelector : public cocos2d::CCLayer
{
private:
    static ViewPosition * m_viewposition;
public:
	static cocos2d::CCScene* scene();
	virtual bool init();  
	CREATE_FUNC(LevelSelector);

	void menuLevelTapOkCallback(CCObject* pSender);
    void menuLevelTapOkCallback2(CCObject* pSender);
	void menuLevelTapLockedCallback(CCObject* pSender);
	void menuTapGetMoreCallback(CCObject* pSender);
	void menuCreateButtonback(CCObject*pSender);
private:
    ScMenuItem* createScMenuItem (int level, CCPoint position);
	int m_unlockedLevelMax;
};

#endif