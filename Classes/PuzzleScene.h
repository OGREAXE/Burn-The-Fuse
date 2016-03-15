#ifndef __PUZZLE_SCENE_H__
#define __PUZZLE_SCENE_H__

#include "cocos2d.h"

#include "MyMapTile.h"
#include <vector>
#include "MenuItem.h"

class PuzzleTimer;
enum PUZZLE_STATE
{
	PUZZLE_READY,
    PUZZLE_MENU,
	PUZZLE_RUNNING,
	PUZZLE_OVER,
};

class PuzzleGame : public cocos2d::CCLayer
{
private:
    
    static CCDictionary * m_gameData;
public:
    static CCDictionary * getGameDataFromFile()
    {
        if(m_gameData == NULL)
        {
            m_gameData = CCDictionary::createWithContentsOfFile("puzzledata.plist");
            m_gameData->retain();
        }
        return m_gameData;
    }
    
public:
    // Here's a difference. Method 'init' in cocos2d-x returns bool, instead of returning 'id' in cocos2d-iphone
    ~PuzzleGame();
	virtual bool initWithLevel(int level);
	void importGameData(int level);
	void initBackground();
	void initTubeNet();
	void initUI();
	void initAnim();
	void initFire();
    void initSound();
	
    // there's no 'id' in cpp, so we recommend returning the class instance pointer
	static cocos2d::CCScene* scene(int level);
	void registerWithTouchDispatcher();
    // a selector callback
    void menuBackCallback(CCObject* pSender);
	void menuTurboCallback( CCObject* pSender);
	void callbackShowResult();
	void start();
    
    // implement the "static node()" method manually
    CREATE_FUNC(PuzzleGame);
	static PuzzleGame* createWithLevel(int level);
	
	bool ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent);
	void ccTouchMoved(CCTouch *pTouch, CCEvent *pEvent);
	void ccTouchEnded(CCTouch *pTouch, CCEvent *pEvent);
    
    void touchLeft();
    void touchRight();
    void touchUp();
    void touchDown();

	void beginErasingTile(CCTubeGridTile *pTile, float speed, int direction);
    void toggleTeleport(CCTubeGridTile *pTile, int direction);
    
	void onNotificationNextTile(CCObject*);

	int getTubeCount(){return m_countTubes;}

	void moveTile2VoidTile(int x, int y);
    bool canMoveTile2Void(int x, int y, std::vector<CCTubeGridTile*>& vecTilesBetween, CCPoint & moveBy);
	void doAfterMoveTile2Void(CCNode*pNode);

	void doGameOver();
	CCLayer*createResultWindow();
    CCLayer*createIngameMenu();
	void menuGameOverNextCallback(CCObject* pSender);
	void menuGameOverRetryCallback(CCObject* pSender);
	void menuGameOverMenuCallback(CCObject* pSender);
    
    void menuInGamePrevCallback(CCObject* pSender);
	void menuInGameBack2MenuCallback(CCObject* pSender);
	void menuInGameNextOkCallback(CCObject* pSender);
	void menuInGameNextLockedCallback(CCObject* pSender);

	void updateFirePos(float dt);
private:
    //touch
    CCPoint beginPoint;
    
    cocos2d::CCArray * m_vtilesArray;
    
	cocos2d::CCSprite*m_pBackground;
	CCPoint m_backgroundOrigin;

	int m_currentTileX;
	int m_currentTileY;
	int m_endErasingDirection;
	int n_tile_x;
	int n_tile_y;

	//number of tubes 
	int m_countTubes;
	int m_remainingCountTubes;
	//MyMapTile * m_infoGrid[15][10];
	CCGridTileBase *** m_infoGrid;

	CCTubeGridTile*m_voidTile;
	bool m_canTeleport;

	START_INFO m_startInfo;

	cocos2d::CCParticleSystem * m_fire;
	cocos2d::CCParticleSystem * m_fireAvator1;
	cocos2d::CCParticleSystem * m_fireAvator2;
	
	float m_fireSpeed;

	//pseudo masking sprite
	CCTubeGridTile * m_mskVerticalOnTop;
	bool m_addPseudo;
	CCTubeGridTile * m_mskHorizontalOnTop;
    
	int m_level;
    TileColor m_tileColor;
	PuzzleTimer*m_pTimer;

	CCSprite*m_startPointSprite;

	PUZZLE_STATE m_gameState;

	float m_maxDistanceBetweenSteps;
	//count of star
	int m_nStar;
	//step
	int m_stepCount;

    //menu button before game stop
	CCMenuItemSprite *m_pBackMenuItem;

	//the turbo button
	CCMenuItemSprite * m_pAturboMenuItem;
    //////////////////////
    
	CCSprite *m_pTurboSprite;
	CCSprite *m_pTurboTempRed;
	CCSprite *m_pTurboTempBlue;
	bool m_isTurbo;

	CCLayer * m_pGOLayer;
    
    //prompt when menu button is tapped
    CCLayer * m_pIngameGearMenuLayer;
    CCMenuItemSprite * m_menuItemGONext;
	CCMenuItemSprite * m_menuItemGORetry;
	CCMenuItemSprite * m_menuItemGOMenu;
    
	CCSprite * m_pResultbackgroundWin;
	CCSprite * m_pResultbackgroundLose;

	CCSprite * m_teleportGateAll;
    CCSprite * m_zippo;
    
    CCMenu* m_pGameOverMenu;
    
    std::vector<CCTubeGridTile*> m_vecBetweenTiles2Move;
    CCPoint m_touch2moveBy;

	int m_maxUnlockedLevel;
private:
    void addTubeTile(CCTubeGridTile *pTile);
	CCTubeGridTile * addTubeTile(int nType, int x, int y);
	CCGridTileBase ***newInfoGrid(int n_x,int n_y);
	void deleteInfoGrid(CCGridTileBase ***,int n_x);
    void updateTilePosition(CCTubeGridTile*ptile, int dx, int dy);   
    void showGameoverMenuCallback();
    CCTubeGridTile *getPseudoMskSprite();
	void checkAddPseudo(CCTubeGridTile*target, int startDirection);
	void hidePseudo();
	void initPseudoMask();
	void initTeleportGateAll();
    CCTubeGridTile *getNextTileTeleport(int enddirection,int now_x, int now_y);
    bool parseStartInfo( START_INFO&startInfo,std::string pStr);
	CCTubeGridTile* getTileWithString(std::string pStr);
	int getCountTubeForTileType(int type);
    void switchFireAvator();
};

class PuzzleTimer:public cocos2d::CCLabelTTF
{
public:
	static PuzzleTimer*create(const char *string, const char *fontName, float fontSize);
	static PuzzleTimer*create();
	void start();
	void stop();
	virtual void updateTime(float delta);
	int getTime(){return m_mytime;}
private:
	int m_mytime;
	char m_pstrTime[6];
};


#endif // __HELLOWORLD_SCENE_H__
