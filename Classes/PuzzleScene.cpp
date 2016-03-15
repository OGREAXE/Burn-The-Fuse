#include "PuzzleScene.h"
#include "AppMacros.h"
#include "MyMapTile.h"
#include "PuzzleTool.h"
#include "LevelSelectScene.h"
#include "UserData.h"
#include "CLabel.h"
#include "SimpleAudioEngine.h"
#include "PuzzleData.h"

USING_NS_CC;

CCDictionary * PuzzleGame::m_gameData = NULL;

CCScene* PuzzleGame::scene(int level)
{
    CCLog("begin level %d",level);
	// 'scene' is an autorelease object
	CCScene *scene = CCScene::create();

	// 'layer' is an autorelease object
	PuzzleGame *layer = PuzzleGame::createWithLevel(level);

	// add layer as a child to scene
	scene->addChild(layer);

	// return the scene
	return scene;
}

PuzzleGame::~PuzzleGame()
{
    CCNotificationCenter::sharedNotificationCenter()->purgeNotificationCenter();
    deleteInfoGrid(m_infoGrid, n_tile_x);
    
    //m_vtilesArray->release();
}


PuzzleGame* PuzzleGame::createWithLevel(int level)
{
	PuzzleGame*pLayer = new PuzzleGame();
	if (pLayer->initWithLevel(level))
	{
		pLayer->autorelease();
		return pLayer;
	}
	CC_SAFE_DELETE(pLayer);
	return NULL;
}

bool PuzzleGame::initWithLevel(int level)
{
	//////////////////////////////
	// 1. super init first
	if ( !CCLayer::init() )
	{
		return false;
	}
	m_level = level;
    m_tileColor = (TileColor)(level%6);
	m_gameState = PUZZLE_READY;
	m_maxUnlockedLevel = UserData::sharedUserData()->getUnlockedLevel();

    long start = getCurrentTime();
	importGameData(level);
    CCLog("importGameData finish at %ld",getCurrentTime() - start);
	initBackground();
    CCLog("initBackground finish at %ld",getCurrentTime() - start);
	initTubeNet();
    CCLog("initTubeNet finish at %ld",getCurrentTime() - start);
	initUI();
	initAnim();
	initFire();
	initPseudoMask();
	initTeleportGateAll();
    initSound();
    
	m_pGOLayer = createResultWindow();
	CCSize visibleSize = CCDirector::sharedDirector()->getWinSize();
	
	m_pGOLayer->setPosition(visibleSize.width/2 - m_pGOLayer->getContentSize().width/2 ,visibleSize.height/2- m_pGOLayer->getContentSize().height/2);
	addChild(m_pGOLayer);
	m_pGOLayer->setVisible(false);
	setTouchEnabled(true);

	m_maxDistanceBetweenSteps = 0;
	m_stepCount = 0;
	return true;
}

void PuzzleGame::initPseudoMask()
{
	m_addPseudo = false;
	m_mskVerticalOnTop = CCTubeGridTile::createWithType(TILETYPE_LINE_ONE_VERTICAL,false, m_tileColor);
	m_mskVerticalOnTop->setAnchorPoint(CCPointZero);
	m_mskVerticalOnTop->setVisible(false);
	m_pBackground->addChild(m_mskVerticalOnTop,5);
	m_mskHorizontalOnTop = CCTubeGridTile::createWithType(TILETYPE_LINE_ONE_HORIZONTAL,false, m_tileColor);
	m_mskHorizontalOnTop->setAnchorPoint(CCPointZero);
	m_mskHorizontalOnTop->setVisible(false);
	m_pBackground->addChild(m_mskHorizontalOnTop,5);
}
void PuzzleGame::registerWithTouchDispatcher()           
{
	CCDirector::sharedDirector()->getTouchDispatcher()->addTargetedDelegate(this, 0, true);
}

void PuzzleGame::menuBackCallback(
	CCObject* pSender)
{
/*#if (CC_TARGET_PLATFORM == CC_PLATFORM_WINRT) || (CC_TARGET_PLATFORM == CC_PLATFORM_WP8)
	CCMessageBox("You pressed the close button. Windows Store Apps do not implement a close button.","Alert");
#else*/
    if (m_gameState != PUZZLE_MENU) {
        m_pIngameGearMenuLayer->setVisible(true);
        m_gameState = PUZZLE_MENU;
        
        m_startPointSprite->setVisible(false);
        m_zippo->setVisible(false);
    }
    else
    {
        m_pIngameGearMenuLayer->setVisible(false);
        m_gameState = PUZZLE_READY;
        
        m_startPointSprite->setVisible(true);
        m_zippo->setVisible(true);
    }
}

void PuzzleGame::menuTurboCallback(
	CCObject* pSender)
{
	CCTubeGridTile * currentErasingTile = dynamic_cast<CCTubeGridTile *>(m_infoGrid[m_currentTileX][m_currentTileY]);
	if (!currentErasingTile)
	{
		return;
	}
	if (!m_isTurbo)
	{
		m_pTurboTempBlue->setVisible(false);
		m_pTurboTempRed->setVisible(true);
		m_pTurboTempRed->runAction(CCRepeatForever::create(CCBlink::create(1,2)));
		m_isTurbo = true;
		m_fireSpeed = fireTurboSpeed;
		currentErasingTile->setEraserSpeed(m_fireSpeed);
	}
	else
	{
		m_pTurboTempBlue->setVisible(true);
		m_pTurboTempRed->setVisible(false);
		m_pTurboTempRed->stopAllActions();
		m_isTurbo = false;
		m_fireSpeed = fireNormalSpeed;
		currentErasingTile->setEraserSpeed(m_fireSpeed);
	}
	
}

void PuzzleGame::initBackground()
{
	//layer2
	CCSize visibleSize = CCDirector::sharedDirector()->getVisibleSize();
	CCPoint visibleOrigin = CCDirector::sharedDirector()->getVisibleOrigin();
    /*CCSprite*background_image1 = CCSprite::create("backImage.png");
	background_image1->setAnchorPoint(CCPointZero);
	background_image1->setPosition(CCPointZero);
	this->addChild(background_image1);
	CCSprite*background_image2 = CCSprite::create("backImage.png");
	background_image2->setAnchorPoint(CCPointZero);
	background_image2->setPosition(ccp(0,visibleSize.height-background_image2->getContentSize().height));
	this->addChild(background_image2);*/
    
    
    CCSprite*background_image1;
    
    if (m_level <= 40) {
        background_image1= getFixedSizeSprite("backImage1.png");
    }
    else if (m_level <= 70){
        background_image1= getFixedSizeSprite("backImage2.png");
    }
    else {
        background_image1= getFixedSizeSprite("backImage3.png");
    }
    
    //background_image1= getFixedSizeSprite("backImage1.png");
    
    background_image1->setScaleX(visibleSize.width/ background_image1->getContentSize().width);
    background_image1->setScaleY(visibleSize.height/ background_image1->getContentSize().height);
    background_image1->setAnchorPoint(CCPointZero);
    background_image1->setPosition(visibleOrigin);
    this->addChild(background_image1);
    
	//layer1
	int length1 = 270;
	CCRenderTexture*pTexL1 = CCRenderTexture::create(480,length1);
	pTexL1->begin();
	int h1=10,h2=4;
	ccColor4F c1 = {0.9,0.9,0.9,1.0};
	ccDrawSolidRect(ccp(0,h1),ccp(480,length1-h1),c1);
	ccColor4F c2 = {0.8,0.8,0.8,0.8};
	ccDrawSolidRect(ccp(0,h2),ccp(480,h1),c2);
	ccDrawSolidRect(ccp(0,length1-h2),ccp(480,length1-h1),c2);
	ccColor4F c3 = {0.7,0.7,0.7,0.4};
	ccDrawSolidRect(ccp(0,0),ccp(480,h2),c3);
	ccDrawSolidRect(ccp(0,length1),ccp(480,length1-h2),c3);
	pTexL1->end();
	CCSprite*bLayer1 = CCSprite::createWithTexture(pTexL1->getSprite()->getTexture());
	bLayer1->setPosition(ccp(240,165));
	this->addChild(bLayer1);
	

	int GridWidth = tileWidth*n_tile_x;
	int GridHeight = tileHeight*n_tile_y;
	//ccColor4F colorBackground = {0.7f,0.7f,0.7f,1.0f};
	ccColor4F colorBackground = {0.9f,0.9f,0.9f,1.0f};

	CCRenderTexture*pRt = CCRenderTexture::create(GridWidth,GridHeight);
	pRt->begin();
	ccDrawSolidRect(ccp(0,0),ccp(GridWidth,GridHeight),colorBackground);
	//grid should be 15 * 10   ()
	ccDrawColor4F(0,0,0,1);
	glLineWidth(3);
	ccDrawLine(ccp(1,0),ccp(1,GridHeight));
	for (int i = 0; i < n_tile_x; i++)
	{
		ccDrawLine(ccp(tileWidth*(1+i),0),ccp(tileWidth*(1+i),GridHeight));
	}
	ccDrawLine(ccp(GridWidth-1,0),ccp(GridWidth-1,GridHeight));

	ccDrawLine(ccp(0,1),ccp(GridWidth,1));
	for (int i = 0; i < n_tile_y-1; i++)
	{
		ccDrawLine(ccp(0,tileHeight*(1+i)),ccp(GridWidth,tileHeight*(1+i)));
	}
	ccDrawLine(ccp(0,GridHeight-1),ccp(GridWidth,GridHeight-1));
	//add black area under tiles
	CCObject*pObj = NULL;
	CCARRAY_FOREACH(m_vtilesArray,pObj)
	{
		CCTubeGridTile*pTile = dynamic_cast<CCTubeGridTile*>(pObj);
		if (pTile)
		{
			float drawPosX = tileWidth * pTile->grid_x ;
			float drawPosY = tileHeight * (n_tile_y - pTile->grid_y -1);
			ccDrawSolidRect(ccp(drawPosX,drawPosY),ccp(drawPosX + tileWidth,drawPosY + tileHeight),ccc4f(0,0,0,1));
		}	
	}

	pRt->end();
	m_pBackground = new CCSprite();
	if (m_pBackground->initWithTexture(pRt->getSprite()->getTexture()))
	{
		m_pBackground->setPosition(ccp(240,160));
        m_pBackground->autorelease();
		this->addChild(m_pBackground);
	}
    else
    {
        CC_SAFE_DELETE(m_pBackground);
        return;
    }
    
	m_backgroundOrigin = ccpAdd(m_pBackground->getPosition(),ccp(-m_pBackground->getContentSize().width/2,-m_pBackground->getContentSize().height/2));

	//add timer
	m_pTimer = PuzzleTimer::create();
    m_pTimer->setAnchorPoint(CCPointZero);
    m_pTimer->setPosition(ccp(33,10));
    CCSprite * pClock = getFixedSizeSprite("clock.png");
    pClock->addChild(m_pTimer);
	pClock->setPosition(ccpAdd(m_backgroundOrigin,ccp(18,m_pBackground->getContentSize().height+30)));
    this->addChild(pClock);
	//m_pTimer->setPosition(ccpAdd(m_backgroundOrigin,ccp(0,m_pBackground->getContentSize().height)));
	//this->addChild(m_pTimer);
}


void PuzzleGame::initTubeNet()
{
	/////////////
	//add tiles to background
	CCObject*pObject = NULL;
	CCARRAY_FOREACH(m_vtilesArray,pObject)
	{
		CCGridTileBase*pMaptile = dynamic_cast<CCGridTileBase*>(pObject);
		if (pMaptile)
		{
			CCPoint tPosition = ccp(tileWidth*pMaptile->grid_x,tileHeight*pMaptile->grid_y);
			pMaptile->setAnchorPoint(ccp(0,0));
			pMaptile->setPosition(tPosition);
			m_pBackground->addChild(pMaptile);
		}
		
	}
}

void PuzzleGame::addTubeTile(CCTubeGridTile *pTile)
{
	m_vtilesArray->addObject(pTile);
	m_infoGrid[pTile->grid_x][pTile->grid_y] = pTile;
}

CCTubeGridTile * PuzzleGame::addTubeTile(int nType, int x, int y)
{
	CCTubeGridTile * podTile = CCTubeGridTile::createWithType(nType);
	podTile->setGridCoordPosition(x,y);
	addTubeTile(podTile);
	return podTile;
}

bool PuzzleGame::ccTouchBegan(cocos2d::CCTouch *pTouch, cocos2d::CCEvent *pEvent)
{
    beginPoint = pTouch->getLocationInView();
	beginPoint = CCDirector::sharedDirector()->convertToGL(beginPoint);

	float fXOffset = beginPoint.x - m_backgroundOrigin.x;
	float fYOffset = beginPoint.y - m_backgroundOrigin.y;

	 int tileX = fXOffset/tileWidth;
	 int tileY = fYOffset/tileHeight;
    
    if (tileX< 0 || tileY < 0 || tileX >n_tile_x - 1 || tileY > n_tile_y - 1 ) {
        return false;
    }

	 switch (m_gameState)
	 {
	 case PUZZLE_READY:
		 {
			 if (abs((int)(tileX - m_startInfo.x)) <=2 && abs((int)(tileY - m_startInfo.y) <= 2))
			 {
				 start();
				 m_gameState = PUZZLE_RUNNING;
				 m_startPointSprite->removeFromParent();
			 }
		 }
		 break;
	 case PUZZLE_RUNNING:
#if (CONTROL_TYPE == GESTURE_USE_TAP_MOVETILE)
		 moveTile2VoidTile(tileX,tileY);
#endif
		 break;
	 case PUZZLE_OVER:
		 break;
	 default:
		 break;
	 }
	 
	return true;
}

void PuzzleGame::ccTouchMoved(CCTouch *pTouch, CCEvent *pEvent)
{

}
void PuzzleGame::ccTouchEnded(CCTouch *pTouch, CCEvent *pEvent)
{
#if (CONTROL_TYPE == GESTURE_USE_SWIPE_MOVETILE)
    
    if (m_gameState == PUZZLE_RUNNING) {
        CCPoint touchEndP=pTouch->getLocation();
        CCPoint touchStartP = beginPoint;
        double d=ccpDistance(touchStartP,touchEndP);
        
        if (d>20) {
            if (touchStartP.x-touchEndP.x<0&&touchStartP.x-touchEndP.x<touchStartP.y-touchEndP.y&&touchEndP.x-touchStartP.x>touchStartP.y-touchEndP.y) {
                touchRight();
                
            }
            if (touchStartP.x-touchEndP.x>0&&touchStartP.x-touchEndP.x>touchStartP.y-touchEndP.y&&touchEndP.x-touchStartP.x<touchStartP.y-touchEndP.y) {
                touchLeft();
            }
            
            if (touchStartP.y-touchEndP.y<0&&touchStartP.y-touchEndP.y<touchStartP.x-touchEndP.x&&touchEndP.y-touchStartP.y>touchStartP.x-touchEndP.x) {
                touchUp();
            }
            if (touchStartP.y-touchEndP.y>0&&touchStartP.y-touchEndP.y>touchStartP.x-touchEndP.x&&touchEndP.y-touchStartP.y<touchStartP.x-touchEndP.x) {
                touchDown();
            }
        }
    }
#endif
}

void PuzzleGame::touchLeft()
{
    int x = m_voidTile->grid_x + 1;
    int y = m_voidTile->grid_y ;
    
    if (x>=0 && x<=n_tile_x - 1) {
        moveTile2VoidTile(x,y);
    }
}
void PuzzleGame::touchRight()
{
    int x = m_voidTile->grid_x - 1;
    int y = m_voidTile->grid_y ;
    
    if (x>=0 && x<=n_tile_x - 1) {
        moveTile2VoidTile(x,y);
    }
}
void PuzzleGame::touchUp()
{
    int x = m_voidTile->grid_x;
    int y = m_voidTile->grid_y - 1 ;
    
    if (y>=0 && y<=n_tile_y - 1) {
        moveTile2VoidTile(x,y);
    }
}
void PuzzleGame::touchDown()
{
    int x = m_voidTile->grid_x;
    int y = m_voidTile->grid_y + 1;
    
    if (y>=0 && y<=n_tile_y - 1) {
        moveTile2VoidTile(x,y);
    }
}

void PuzzleGame::onNotificationNextTile(CCObject*pInfo)
{
	//
	if ((--m_remainingCountTubes)==0)
	{
		doGameOver();
		return;
	}
	hidePseudo();
	int nextTilex = 0;
	int nextTileY = 0;
	CCGridTileBase*pNextTile = NULL;
	int startErasingDirection;
	bool bShouldTeleport = false;

	switch (m_endErasingDirection)
	{
	case DIRECTION_LEFT:
		{
			nextTilex = m_currentTileX-1;
			nextTileY = m_currentTileY;
			if (nextTilex>=0 && m_infoGrid[nextTilex][nextTileY]!=NULL )
			{
				pNextTile = m_infoGrid[nextTilex][nextTileY];
				
			}
			else
			{
				if (m_canTeleport)
				{
					bShouldTeleport = true;
				}
			}
			startErasingDirection = DIRECTION_RIGHT;
			break;
		}
	case DIRECTION_TOP:
		{
			nextTilex = m_currentTileX;
			nextTileY = m_currentTileY + 1;
			if (nextTileY<n_tile_y && m_infoGrid[nextTilex][nextTileY]!=NULL)
			{
				pNextTile = m_infoGrid[nextTilex][nextTileY];
			}
			else
			{
				if (m_canTeleport)
				{
					bShouldTeleport = true;
				}
			}
			startErasingDirection = DIRECTION_BOTTON;
			break;
		}
	case DIRECTION_RIGHT:
		{
			nextTilex = m_currentTileX + 1;
			nextTileY = m_currentTileY;
			if (nextTilex<n_tile_x && m_infoGrid[nextTilex][nextTileY]!=NULL )
			{
				pNextTile = m_infoGrid[nextTilex][nextTileY];
			}
			else
			{
				if (m_canTeleport)
				{
					bShouldTeleport = true;
				}
			}
			startErasingDirection = DIRECTION_LEFT;
			break;
		}
	case DIRECTION_BOTTON:
		{
			nextTilex = m_currentTileX;
			nextTileY = m_currentTileY - 1;
			if (nextTileY>=0 && m_infoGrid[nextTilex][nextTileY]!=NULL )
			{
				pNextTile = m_infoGrid[nextTilex][nextTileY];
			}
			else
			{
				if (m_canTeleport)
				{
					bShouldTeleport = true;
				}
			}
			startErasingDirection = DIRECTION_TOP;
			break;
		}
	default:
		break;
	}

	CCTubeGridTile*pTubeTile = NULL;
	if (bShouldTeleport)
	{
		m_fire->stopSystem();
		switchFireAvator();
		pTubeTile = getNextTileTeleport(m_endErasingDirection,m_currentTileX,m_currentTileY);
		m_currentTileX = pTubeTile->grid_x;
		m_currentTileY = pTubeTile->grid_y;		
	}
	else if (NULL!=pNextTile)
	{
		m_currentTileX = nextTilex;
		m_currentTileY = nextTileY;

		pTubeTile = dynamic_cast<CCTubeGridTile*>(pNextTile);
	}
	if (pTubeTile)
	{
		//pTubeTile->retain();
		//pTubeTile->removeFromParent();
		//m_pBackground->addChild(pTubeTile);
		//pTubeTile->release();
		if (pTubeTile->getChildForStartErasingAt(startErasingDirection))
		{
			checkAddPseudo(pTubeTile,startErasingDirection);
			/*pTubeTile->beginErasing(m_fireSpeed,startErasingDirection);
			if (pTubeTile->grid_x == n_tile_x || pTubeTile->grid_x == 1 
				||  pTubeTile->grid_y == n_tile_y || pTubeTile->grid_y == 1)
			{
				m_teleportGateAll->setVisible(true);
			}
			else
			{
				m_teleportGateAll->setVisible(false);
			}*/
			beginErasingTile(pTubeTile,m_fireSpeed,startErasingDirection);

			m_endErasingDirection = pTubeTile->getEndErasingDirection();		
		}
		else
		{
			doGameOver();
		}
	}
	else
	{
		doGameOver();
	}
}

CCGridTileBase *** PuzzleGame::newInfoGrid(int n_x,int n_y)
{
	CCGridTileBase *** infoGrid = (CCGridTileBase***)malloc(sizeof(CCGridTileBase**) * n_x );
	for (int i=0;i<n_x;i++)
	{
		infoGrid[i]= (CCGridTileBase**)malloc(sizeof(CCGridTileBase*) * n_y );
	}

	for (int i=0;i<n_x;i++)
	{
		for (int j=0;j<n_y;j++)
		{
			infoGrid[i][j] = NULL;
		}
	}
	return infoGrid;
}

void PuzzleGame::deleteInfoGrid(CCGridTileBase ***pInfoGrip,int n_x)
{
	for (int i=0;i<n_x;i++)
	{
		free(pInfoGrip[i]);
	}
	free(pInfoGrip);
}

void PuzzleGame::moveTile2VoidTile(int x, int y)
{
	CCTubeGridTile* pTile2move = dynamic_cast<CCTubeGridTile*>(m_infoGrid[x][y]);
	if (NULL == m_voidTile || pTile2move == NULL)
	{
		return;
	}
	
	/*if( (m_voidTile->grid_x == pTile2move->grid_x - 1 && m_voidTile->grid_y == pTile2move->grid_y) ||
		(m_voidTile->grid_x == pTile2move->grid_x + 1 && m_voidTile->grid_y == pTile2move->grid_y) ||
		(m_voidTile->grid_x == pTile2move->grid_x  && m_voidTile->grid_y == pTile2move->grid_y -1) ||
		(m_voidTile->grid_x == pTile2move->grid_x  && m_voidTile->grid_y == pTile2move->grid_y +1))

	{
		setTouchEnabled(false);
		CCMoveTo*pMove2Void = CCMoveTo::create(0.2,m_voidTile->getPosition());
		CCCallFuncN*pCall = CCCallFuncN::create(this,callfuncN_selector(PuzzleGame::doAfterMoveTile2Void));
		CCSequence *pSeq = CCSequence::create(pMove2Void,pCall,NULL);
		pTile2move->runAction(pSeq);
		//m_infoGrid[x][y] = NULL;
		m_voidTile->setPosition(pTile2move->getPosition());
	}*/
 
    float moveTime = 0.2;
    m_vecBetweenTiles2Move.clear();
    if( canMoveTile2Void(x, y, m_vecBetweenTiles2Move,m_touch2moveBy))       
	{
        setTouchEnabled(false);
		for (int i=0; i<m_vecBetweenTiles2Move.size(); i++) {
            CCTubeGridTile * pTile = m_vecBetweenTiles2Move[i];
            if (pTile->getIsErasing()) {
                m_mskVerticalOnTop->runAction(CCMoveBy::create(moveTime, m_touch2moveBy));
                m_mskHorizontalOnTop->runAction(CCMoveBy::create(moveTime, m_touch2moveBy));
            }
            pTile->runAction(CCMoveBy::create(moveTime, m_touch2moveBy));
        }
        //move touched tile
        CCMoveBy*pMoveBy = CCMoveBy::create(moveTime,m_touch2moveBy);
		CCCallFuncN*pCall = CCCallFuncN::create(this,callfuncN_selector(PuzzleGame::doAfterMoveTile2Void));
		CCSequence *pSeq = CCSequence::create(pMoveBy,pCall,NULL);
		pTile2move->runAction(pSeq);
        if (pTile2move->getIsErasing()) {
            m_mskVerticalOnTop->runAction(CCMoveBy::create(moveTime, m_touch2moveBy));
            m_mskHorizontalOnTop->runAction(CCMoveBy::create(moveTime, m_touch2moveBy));
        }
		m_voidTile->setPosition(pTile2move->getPosition());

		m_stepCount ++;
	}
}

bool PuzzleGame::canMoveTile2Void(int x, int y, std::vector<CCTubeGridTile*>& vecTilesBetween, CCPoint & moveBy)
{
    int voidX = m_voidTile->grid_x;
    int voidY = m_voidTile->grid_y;
    
    if (x == voidX) {
        int upperY = max(y, voidY);
        int lowerY = min(y, voidY);
        
        if (y > voidY) {
            moveBy.x = 0;
            moveBy.y = -tileHeight;
        }
        else
        {
            moveBy.x = 0;
            moveBy.y = tileHeight;
        }
        
        for (int i = lowerY+1; i<upperY; i++) {
            if (m_infoGrid[x][i]!=NULL) {
                vecTilesBetween.push_back((CCTubeGridTile*)m_infoGrid[x][i]);
            }
            else
                return false;
        }
        return true;
    }
    else if (y == voidY) {
        int rightX = max(x, voidX);
        int leftX = min(x, voidX);
        
        if (x > voidX) {
            moveBy.y = 0;
            moveBy.x = -tileWidth;
        }
        else
        {
            moveBy.y = 0;
            moveBy.x = tileWidth;
        }
        
        for (int i = leftX+1; i<rightX; i++) {
            if (m_infoGrid[i][y]!=NULL) {
                vecTilesBetween.push_back((CCTubeGridTile*)m_infoGrid[i][y]);
            }
            else
                return false;
        }
        return true;
    }
    else
        return false;
    
}

void PuzzleGame::doAfterMoveTile2Void(CCNode*pNode)
{
	CCTubeGridTile*pTile = dynamic_cast<CCTubeGridTile*>(pNode);
	if (pTile == NULL)
	{
		return;
	}

    int dx = 0,dy = 0;
    if (m_touch2moveBy.x ==0) {
        if (m_touch2moveBy.y > 0) {
            dy = 1;
        }
        else
        {
            dy = -1;
        }
    }
    else
    {
        if (m_touch2moveBy.x > 0) {
            dx = 1;
        }
        else
        {
            dx = -1;
        }

    }
    
	int new_void_x = pTile->grid_x;
	int new_void_y = pTile->grid_y;

	updateTilePosition(pTile, dx, dy);
    
	m_voidTile->grid_x = new_void_x;
	m_voidTile->grid_y = new_void_y;
	m_infoGrid[m_voidTile->grid_x][m_voidTile->grid_y] = m_voidTile;
    
    for (int i=0; i<m_vecBetweenTiles2Move.size(); i++) {
        CCTubeGridTile * tileBetween = m_vecBetweenTiles2Move[i];
        updateTilePosition(tileBetween, dx, dy);
    }

	setTouchEnabled(true);
}
void PuzzleGame::updateTilePosition(CCTubeGridTile*pTile, int dx, int dy)
{
    pTile->grid_x += dx;
	pTile->grid_y += dy;
    m_infoGrid[pTile->grid_x][pTile->grid_y] = pTile;
    if (pTile->getIsErasing())
	{
		m_currentTileX = pTile->grid_x;
		m_currentTileY = pTile->grid_y;
	}
}

void PuzzleGame::importGameData(int level)
{
	//n_tile_x = 14;
    //n_tile_y = 8;
    
    n_tile_x = TILECOUNT_X;
    n_tile_y = TILECOUNT_Y;
	m_infoGrid = newInfoGrid(n_tile_x,n_tile_y);

    m_vtilesArray = CCArray::createWithCapacity(150);

	//load from puzzledata.plist
	char buf[4];
	sprintf(buf, "%d",level);

	//CCDictionary* pDictFile = CCDictionary::createWithContentsOfFile("puzzledata.plist");
    
    CCDictionary* pDictFile = getGameDataFromFile();
	CCDictionary*pDictPuzzleData = dynamic_cast<CCDictionary*>(pDictFile->objectForKey(buf));

    //always can teleport
    m_canTeleport  = true;

	CCString * startInfo = dynamic_cast<CCString *>(pDictPuzzleData->objectForKey("start"));
	parseStartInfo(m_startInfo,startInfo->getCString());

	CCArray * tilesArray = dynamic_cast<CCArray  *>(pDictPuzzleData->objectForKey("data"));
    
//    PuzzleData * pPuzzleData = PuzzleDataCache::sharedCache()->getData(level);
//    CCArray * tilesArray = pPuzzleData->DataCopy();
//    m_startInfo = pPuzzleData->getStartInfo();
    
	CCObject*pTempObj;
	m_countTubes = 0;

    CCLog("begin to create tiles");
    long start = getCurrentTime();
	CCARRAY_FOREACH(tilesArray,pTempObj)
	{
		CCString * tileInfoStr =  (CCString*)(pTempObj);
		CCTubeGridTile*pTile = getTileWithString(tileInfoStr->getCString());
		if (pTile->type == TILETYPE_VOID)
		{
			m_voidTile = pTile;
			m_voidTile->setVisible(false);
		}
		m_infoGrid[pTile->grid_x][pTile->grid_y] = pTile;
		m_vtilesArray->addObject(pTile);

		//sum tubes
		m_countTubes+=getCountTubeForTileType(pTile->type);
	}
	CCLog("finish, takes %ld", getCurrentTime()-start);
	m_remainingCountTubes = m_countTubes;
}

void PuzzleGame::initUI()
{
	CCSize visibleSize = CCDirector::sharedDirector()->getVisibleSize();
	CCPoint origin = CCDirector::sharedDirector()->getVisibleOrigin();
	/////////////////////////////
	// 2. add a menu item with "X" image, which is clicked to quit the program
	//    you may modify it.

	// add a "close" icon to exit the progress. it's an autorelease object
    CCSprite * pGear = getFixedSizeSprite("btngear.png");
    pGear->setAnchorPoint(CCPointZero);
    pGear->setPosition(CCPointZero);
	m_pBackMenuItem = PuzzleMenuItem::create(
		pGear,
		pGear,
		this,
		menu_selector(PuzzleGame::menuBackCallback));
    
	m_pBackMenuItem->setScale(2 * tileWidth /m_pBackMenuItem->getContentSize().width);
    CCMenu* pBackMenu = CCMenu::create(m_pBackMenuItem, NULL);
    pBackMenu->setAnchorPoint(CCPointZero);
    
    float gearposX = m_startInfo.x > 7?tileWidth:tileWidth *13;
    
    //CCPoint backMenuPos = ccpAdd(m_backgroundOrigin,ccp(tileWidth *13, tileHeight));
    CCPoint backMenuPos = ccpAdd(m_backgroundOrigin,ccp(gearposX, tileHeight));
	pBackMenu->setPosition(backMenuPos);
    this->addChild(pBackMenu);
    
    
	/*m_pTurboSprite = CCSprite::create("turbo.png");*/
	m_pTurboSprite = getFixedSizeSprite("turbo.png");
	m_pTurboTempRed = getFixedSizeSprite("temp_red.png");
	m_pTurboTempBlue = getFixedSizeSprite("temp_blue.png");
	m_pTurboTempRed->setAnchorPoint(CCPointZero);
	m_pTurboTempBlue->setAnchorPoint(CCPointZero);
	m_pTurboSprite->addChild(m_pTurboTempBlue);
	m_pTurboSprite->addChild(m_pTurboTempRed);
	m_pTurboTempRed->setVisible(false);

	m_pAturboMenuItem = PuzzleMenuItem::create(m_pTurboSprite,m_pTurboSprite,this,menu_selector(PuzzleGame::menuTurboCallback));
	m_isTurbo = false;
	m_pAturboMenuItem->setVisible(false);
	m_pAturboMenuItem->setPosition(ccp(14 * tileWidth + m_backgroundOrigin.x- m_pAturboMenuItem->getContentSize().width/2,m_pBackground->getPosition().y - m_pBackground->getContentSize().height/2 - m_pAturboMenuItem->getContentSize().height/2 -10));
	CCMenu* pMenu = CCMenu::create(m_pAturboMenuItem, NULL);
	pMenu->setPosition(CCPointZero);
	this->addChild(pMenu, 1);

	//start arrow 
	//start sprite
	m_startPointSprite =getFixedSizeSprite("direction.png");
	m_startPointSprite->setAnchorPoint(ccp(0.5,0.5));
	m_startPointSprite->runAction(CCRepeatForever::create(CCBlink::create(2,3)));
    
	if (m_startInfo.start_direction == DIRECTION_BOTTON)
	{
		m_startPointSprite->setRotation(90);

	}
	else if (m_startInfo.start_direction == DIRECTION_LEFT)
	{
		m_startPointSprite->setRotation(180);

	}
	else if (m_startInfo.start_direction == DIRECTION_TOP)
	{
		m_startPointSprite->setRotation(270);

	}
    m_startPointSprite->setPosition(ccp(tileWidth*m_startInfo.x+tileWidth/2,tileHeight*m_startInfo.y+tileHeight/2));
    //zippo
    m_zippo = getFixedSizeSprite("zippo.png");
    /*if (m_startInfo.x == 0) {
        m_zippo->setFlipX(true);
        m_zippo->setPosition(ccpAdd(m_startPointSprite->getPosition(), ccp(m_zippo->getContentSize().width/2, 10)));
    }
    else
    {
        m_zippo->setPosition(ccpAdd(m_startPointSprite->getPosition(), ccp(-m_zippo->getContentSize().width/2, 10)));
    }*/
    m_zippo->setPosition(ccpAdd(m_startPointSprite->getPosition(), ccp(-m_zippo->getContentSize().width/2, 10)));
    
	m_pBackground->addChild(m_startPointSprite,2);
    m_pBackground->addChild(m_zippo,2);
    
	m_pIngameGearMenuLayer = createIngameMenu();
	m_pIngameGearMenuLayer->setAnchorPoint(CCPointZero);
    m_pIngameGearMenuLayer->setPosition(ccpAdd(m_backgroundOrigin, ccp(1 * tileWidth, 3 * tileHeight)));
    m_pIngameGearMenuLayer->setVisible(false);
    this->addChild(m_pIngameGearMenuLayer,2);
    
    char strLvlTmp[16];
    sprintf(strLvlTmp, "LEVEL-%d",m_level);
    //CCLabelTTF *lvlLabel = CCLabelTTF::create(strLevel.c_str(), "Verdana-Bold", 20);
    CCLabelTTF *lvlLabel = CLabel::textAddOutlineAndShadow(strLvlTmp, "Verdana-Bold", 20, ccWHITE, 2, 4, 200);
    lvlLabel->setColor(ccc3(0, 0, 0));
    lvlLabel->setAnchorPoint(CCPointZero);
    lvlLabel->setPosition(ccp(35, 20));
    addChild(lvlLabel);
    //[CCLabelTTF labelWithString:@"Verdana-Bold"fontName:@"Verdana-Bold"fontSize:20];
}

void PuzzleGame::initSound()
{
    CocosDenshion::SimpleAudioEngine::sharedEngine()->preloadBackgroundMusic("FAIL.mp3");
    CocosDenshion::SimpleAudioEngine::sharedEngine()->preloadBackgroundMusic("MAIN_THEME.mp3");
    
    CocosDenshion::SimpleAudioEngine::sharedEngine()->playBackgroundMusic("MAIN_THEME.mp3", true);
}

void PuzzleGame::start()
{
	m_currentTileX = m_startInfo.x;
	m_currentTileY = m_startInfo.y;
	CCTubeGridTile*pStart = dynamic_cast<CCTubeGridTile*>(m_infoGrid[m_currentTileX][m_currentTileY]);
	if(pStart)
	{
		checkAddPseudo(pStart,m_startInfo.start_direction);
		//pStart->beginErasing(m_fireSpeed,m_startInfo.start_direction);
		beginErasingTile(pStart,m_fireSpeed,m_startInfo.start_direction);
		m_endErasingDirection = pStart->getEndErasingDirection();

		CCNotificationCenter::sharedNotificationCenter()->addObserver
			(this,callfuncO_selector(PuzzleGame::onNotificationNextTile),MY_NOTIFICATION_NEXT_TILE,NULL);
		schedule(schedule_selector(PuzzleGame::updateFirePos),0.02);
		//m_pBackMenuItem->setVisible(false);
        m_zippo->removeFromParent();
		m_pTimer->start();
        m_pBackMenuItem->setVisible(false);
	}
}

void PuzzleGame::initAnim()
{
	/*CCArray*pFrameNames = CCArray::create(CCSprite::create("fire2.png")->displayFrame(),
		CCSprite::create("fire3.png")->displayFrame(),CCSprite::create("fire4.png")->displayFrame());*/
	/*CCTexture2D*pFireTex = CCTextureCache::sharedTextureCache()->addImage("fire_whole.png");
	float wUnit = 23;
	float hUnit = pFireTex->getContentSize().height/3;
	CCSpriteFrame*pf1 = CCSpriteFrame::createWithTexture(pFireTex,CCRectMake(0,0,wUnit,hUnit));
	CCSpriteFrame*pf2 = CCSpriteFrame::createWithTexture(pFireTex,CCRectMake(0,wUnit,wUnit,hUnit));
	CCSpriteFrame*pf3 = CCSpriteFrame::createWithTexture(pFireTex,CCRectMake(0,wUnit*2+2,wUnit,hUnit));

	CCArray*pFrameNames = CCArray::create();
	pFrameNames->addObject(pf1);
	pFrameNames->addObject(pf2);
	pFrameNames->addObject(pf3);
	CCAnimation*pAnimFire = CCAnimation::createWithSpriteFrames(pFrameNames,0.1);
	CCAnimationCache::sharedAnimationCache()->addAnimation(pAnimFire,"fire_animation");*/
}

CCTubeGridTile * PuzzleGame::getNextTileTeleport(int enddirection,int now_x, int now_y)
{
	CCTubeGridTile * pRet;
	int tempx = now_x;
	int tempy = now_y;
	switch (enddirection)
	{
	case  DIRECTION_BOTTON:
		{
			tempy = now_y;
			while ( tempy+1 <=n_tile_y-1 && m_infoGrid[tempx][tempy+1]!=NULL)
			{
				tempy++;
			}
			break;
		}
	case  DIRECTION_TOP:
		{
			 tempy = now_y;
			while (tempy-1 >=0 && m_infoGrid[tempx][tempy-1]!=NULL)
			{
				tempy--;
			}
			break;
		}

	case  DIRECTION_LEFT:
		{
			tempx = now_x;
			while (tempx+1 <=n_tile_x-1 && m_infoGrid[tempx+1][tempy]!=NULL)
			{
				tempx++;
			}
			break;
		}
	case  DIRECTION_RIGHT:
		{
			tempx = now_x;
			while (tempx-1 >=0 && m_infoGrid[tempx-1][tempy]!=NULL )
			{
				tempx--;
			}
			break;
		}
	default:
		break;
	}
	return dynamic_cast<CCTubeGridTile *>(m_infoGrid[tempx][tempy]);
}


bool PuzzleGame::parseStartInfo(START_INFO&startInfo,string pStr)
{
	vector<string> vTemp;
	string strDelim("%");
	split(pStr,strDelim,vTemp);
	startInfo.start_direction = CCString(vTemp[0]).intValue();
	startInfo.x = CCString(vTemp[1]).intValue();
	startInfo.y = CCString(vTemp[2]).intValue();
	return true;
}

CCTubeGridTile* PuzzleGame::getTileWithString(string pStr)
{
	vector<string> vTemp;
	string strDelim("%");
	split(pStr,strDelim,vTemp);
	int type = atoi(vTemp[0].c_str());
    
	CCTubeGridTile*podSprite = CCTubeGridTile::createWithType(type, true, m_tileColor);
	podSprite->grid_x = atoi(vTemp[1].c_str());
	podSprite->grid_y = atoi(vTemp[2].c_str());

	return podSprite;
}

int PuzzleGame::getCountTubeForTileType(int type)
{
	if(type == TILETYPE_CURVE_ONE_BOTTONLEFT||type ==TILETYPE_CURVE_ONE_BOTTONRIGHT||
		type ==TILETYPE_CURVE_ONE_TOPLEFT||type ==TILETYPE_CURVE_ONE_TOPRIGHT||
		type ==TILETYPE_LINE_ONE_HORIZONTAL||type ==TILETYPE_LINE_ONE_VERTICAL)
		return 1;
	else if(type ==TILETYPE_CURVE_TWO_TOPLEFT_BOTTONRIGHT||type ==TILETYPE_CURVE_TWO_BOTTONLEFT_TOPRIGHT||	
			type ==TILETYPE_LINE_TWO_HORIZONTALONTOP||type ==TILETYPE_LINE_TWO_VERTICALONTOP)
			return 2;
		else
		{
			return 0;
		}
}

void PuzzleGame::updateFirePos(float dt)
{
	if (!m_infoGrid[m_currentTileX][m_currentTileY])
	{
		//doDo: fix it! this cause the fire static when move the current tile
		return;
	}
	CCTubeGridTile*pCurrentBurningTile = dynamic_cast<CCTubeGridTile*>(m_infoGrid[m_currentTileX][m_currentTileY]);
	if (!pCurrentBurningTile->isCanGetFireLocation())
	{
		return;
	}
	CCPoint nextPosition = pCurrentBurningTile->getFireLoctionWorld();
	if (nextPosition.x !=0 || nextPosition.y !=0 )
	{
		nextPosition = m_pBackground->convertToNodeSpace(nextPosition);

		float distanceBetweenSteps = ccpDistance(nextPosition,m_fire->getPosition());
		//normal distance should be 0.5;
		if (distanceBetweenSteps > 1 && distanceBetweenSteps <3)
		{
			nextPosition = ccpMidpoint(nextPosition,m_fire->getPosition());
		}

		m_fire->setPosition(nextPosition);
		if (!m_fire->isActive())
		{
			m_fire->resetSystem();
		}
	}	
}
void PuzzleGame::doGameOver()
{
	unschedule(schedule_selector(PuzzleGame::updateFirePos));
	m_fire->stopSystem();
	m_pTimer->stop();
	m_gameState = PUZZLE_OVER;
	m_pAturboMenuItem->removeFromParent();
	
    CocosDenshion::SimpleAudioEngine::sharedEngine()->stopBackgroundMusic(false);
    
	m_pGOLayer->runAction(CCSequence::create(CCDelayTime::create(2),
		CCCallFunc::create(this, callfunc_selector(PuzzleGame::callbackShowResult)),NULL));
	
}

void PuzzleGame::checkAddPseudo(CCTubeGridTile*target, int startDirection)
{
	if (target->getRemainTubes()==2)
	{
		if (target->type == TILETYPE_LINE_TWO_VERTICALONTOP)
		{
			if (startDirection == DIRECTION_LEFT || startDirection == DIRECTION_RIGHT)
			{
				m_mskVerticalOnTop->setPosition(target->getPosition());
				m_mskVerticalOnTop->setVisible(true);
			}
		}
		else if (target->type == TILETYPE_LINE_TWO_HORIZONTALONTOP)
		{
			if (startDirection == DIRECTION_BOTTON || startDirection == DIRECTION_TOP)
			{
				m_mskHorizontalOnTop->setPosition(target->getPosition());
				m_mskHorizontalOnTop->setVisible(true);
			}
		}
	}
}

void PuzzleGame::hidePseudo()
{
    m_mskVerticalOnTop->setVisible(false);
    m_mskHorizontalOnTop->setVisible(false);
}

void PuzzleGame::switchFireAvator()
{
	static int loop = 0;
	if (loop == 1)
	{
		m_fire = m_fireAvator1;
		loop = 0;
	}
	else
	{
		m_fire = m_fireAvator2;
		loop = 1;
	}
}

void PuzzleGame::initFire()
{
	m_fireAvator1 = CCParticleFire::create();
	m_fireAvator1->stopSystem();
	m_fireAvator1->setScale(0.18);
	m_pBackground->addChild(m_fireAvator1,3);

	m_fireAvator2 = CCParticleFire::create();
	m_fireAvator2->stopSystem();
	m_fireAvator2->setScale(0.18);
	m_pBackground->addChild(m_fireAvator2,3);

	m_fire = m_fireAvator1;

	m_fireSpeed = fireNormalSpeed;
}

CCLayer*PuzzleGame::createResultWindow()
{
	CCLayer*pRet = CCLayer::create();
	//pRet->setContentSize(CCSizeMake(winSize.width *3/4,winSize.height * 1/2));
	pRet->setContentSize(CCSizeMake(tileWidth * 12 ,tileHeight * 8));

	//win icon
	m_pResultbackgroundWin = CCSprite::create("resultBackground_win.png");
	m_pResultbackgroundWin->setScaleX(pRet->getContentSize().width/m_pResultbackgroundWin->getTexture()->getContentSize().width);
	m_pResultbackgroundWin->setScaleY(pRet->getContentSize().height/m_pResultbackgroundWin->getTexture()->getContentSize().height);
	m_pResultbackgroundWin->setPosition(ccp(pRet->getContentSize().width/2,pRet->getContentSize().height/2));
	pRet->addChild(m_pResultbackgroundWin);

	//lose icon
	m_pResultbackgroundLose = CCSprite::create("resultBackground_lose.png");
	m_pResultbackgroundLose->setScaleX(pRet->getContentSize().width/m_pResultbackgroundLose->getTexture()->getContentSize().width);
	m_pResultbackgroundLose->setScaleY(pRet->getContentSize().height/m_pResultbackgroundLose->getTexture()->getContentSize().height);
	m_pResultbackgroundLose->setPosition(ccp(pRet->getContentSize().width/2,pRet->getContentSize().height/2));
	pRet->addChild(m_pResultbackgroundLose);

#define MENU_SELECTED_RATIO 0.9
    CCSprite * pNext = getFixedSizeSprite("btnGameOverNext.png");
    CCSprite * pNextSelected = getResizedSrite_withOriginalRect(pNext, MENU_SELECTED_RATIO);
    //pNext->setScale(0.8);
    CCSprite * pMenu = getFixedSizeSprite("btnGameOverMenu.png");
    CCSprite * pMenuSelected = getResizedSrite_withOriginalRect(pMenu, MENU_SELECTED_RATIO);
    //pMenu->setScale(0.8);
    CCSprite * pRetry = getFixedSizeSprite("btnGameOverRetry.png");
    CCSprite * pRetrySelected = getResizedSrite_withOriginalRect(pRetry, MENU_SELECTED_RATIO);
    //pRetry->setScale(0.8);
    
    float itemHeight = pNext->getContentSize().height;
    
	m_menuItemGONext = PuzzleMenuItem::create(pNext,pNextSelected,
		this,menu_selector(PuzzleGame::menuGameOverNextCallback));
	m_menuItemGONext->setPosition(ccp(pRet->getContentSize().width,70));

	m_menuItemGORetry = PuzzleMenuItem::create(pRetry,pRetrySelected,
		this,menu_selector(PuzzleGame::menuGameOverRetryCallback));
	m_menuItemGORetry->setPosition(m_menuItemGONext->getPosition());

	m_menuItemGOMenu = PuzzleMenuItem::create(pMenu,pMenuSelected,
		this,menu_selector(PuzzleGame::menuGameOverMenuCallback));
	m_menuItemGOMenu->setPosition(ccpAdd(m_menuItemGONext->getPosition(),ccp(0,-itemHeight - 20)));

	m_pGameOverMenu = CCMenu::create(m_menuItemGONext,m_menuItemGORetry,m_menuItemGOMenu,NULL);
	m_pGameOverMenu->setPosition(ccp(0,-30));
	pRet->addChild(m_pGameOverMenu);
	return pRet;
}

CCLayer*PuzzleGame::createIngameMenu()
{
    CCLayer*pRet = CCLayer::create();
    pRet->setContentSize(CCSizeMake(12 * tileWidth, 3 * tileHeight));
    CCSprite *pMenuPrev = getFixedSizeSprite("menu_leftarrow.png");
    CCSprite *pMenuPrevSelected = getResizedSrite_withOriginalRect(pMenuPrev, MENU_SELECTED_RATIO);
    CCSprite *pMenuBackMenu = getFixedSizeSprite("menu_btnmenu.png");
    CCSprite *pMenuBackMenuSelected = getResizedSrite_withOriginalRect(pMenuBackMenu, MENU_SELECTED_RATIO);
	CCSprite *pMenuNext = getFixedSizeSprite("menu_rightarrow.png");
    CCSprite *pMenuNextSelected = getResizedSrite_withOriginalRect(pMenuNext, MENU_SELECTED_RATIO);
    CCSprite *pMenuNextDisabled = getFixedSizeSprite("menu_rightarrow_grey.png");

	//go to previous
    PuzzleMenuItem * pSpritePrev = PuzzleMenuItem::create(pMenuPrev, pMenuPrevSelected, this, menu_selector(PuzzleGame::menuInGamePrevCallback));
    pSpritePrev->setAnchorPoint(CCPointZero);
    pSpritePrev->setPosition(CCPointZero);
    
	//back to select-level menu
    PuzzleMenuItem * pSpriteMenu = PuzzleMenuItem::create(pMenuBackMenu, pMenuBackMenuSelected, this, menu_selector(PuzzleGame::menuInGameBack2MenuCallback));
    pSpriteMenu->setAnchorPoint(CCPointZero);
    pSpriteMenu->setPosition(ccpAdd(pSpritePrev->getPosition(),ccp(pSpritePrev->getContentSize().width,0)));
    
	//go to next
    PuzzleMenuItem * pSpriteNext = PuzzleMenuItem::create(pMenuNext, pMenuNextSelected, pMenuNextDisabled, this, menu_selector(PuzzleGame::menuInGameNextOkCallback));
	if (m_level >= m_maxUnlockedLevel)
	{
		pSpriteNext->setEnabled(false);
	}

    pSpriteNext->setAnchorPoint(CCPointZero);
    pSpriteNext->setPosition(ccpAdd(pSpriteMenu->getPosition(),ccp(pSpriteMenu->getContentSize().width,0)));
    
    CCSprite * pback = getFixedSizeSprite("menu_background.png");
    pback->setAnchorPoint(CCPointZero);
    pback->setPosition(CCPointZero);
    pback->setScale(tileWidth * 12 / pback->getContentSize().width);
    pRet->addChild(pback);
    
    CCMenu * pMenu = CCMenu::create(pSpritePrev,pSpriteMenu,pSpriteNext,NULL);
    pMenu->setAnchorPoint(CCPointZero);
    pMenu->setPosition(CCPointZero);
    pMenu->setScale(tileWidth * 12 / pback->getContentSize().width);
    pRet->addChild(pMenu);
    
    return pRet;
}

////////////////////////////////in game menu//////////////////////////////////////////////
void PuzzleGame::menuInGamePrevCallback(CCObject* pSender)
{
	if (m_level - 1 > 0)
	{
        CocosDenshion::SimpleAudioEngine::sharedEngine()->stopBackgroundMusic(false);
		CCDirector::sharedDirector()->replaceScene(CCTransitionFade::create(1, PuzzleGame::scene(m_level - 1)));
	}   
}

void PuzzleGame::menuInGameBack2MenuCallback(CCObject* pSender)
{
    CocosDenshion::SimpleAudioEngine::sharedEngine()->stopBackgroundMusic(false);
   CCDirector::sharedDirector()->replaceScene(CCTransitionFade::create(1, LevelSelector::scene()));
}

void PuzzleGame::menuInGameNextOkCallback(CCObject* pSender)
{
    CocosDenshion::SimpleAudioEngine::sharedEngine()->stopBackgroundMusic(false);
    CCDirector::sharedDirector()->replaceScene(CCTransitionFade::create(1, PuzzleGame::scene(m_level + 1)));
}

void PuzzleGame::menuInGameNextLockedCallback(CCObject* pSender)
{

}

////////////////////////////////game over menu////////////////////////////////
void PuzzleGame::menuGameOverRetryCallback(CCObject* pSender)
{
	CCDirector::sharedDirector()->replaceScene(CCTransitionFade::create(1, PuzzleGame::scene(m_level)));
}

void PuzzleGame::menuGameOverMenuCallback(CCObject* pSender)
{
	CCDirector::sharedDirector()->replaceScene(CCTransitionFade::create(1, LevelSelector::scene()));

}

void PuzzleGame::menuGameOverNextCallback(CCObject* pSender)
{
	CCDirector::sharedDirector()->replaceScene(CCTransitionFade::create(1, PuzzleGame::scene(m_level + 1)));
}

void PuzzleGame::callbackShowResult()
{
	m_pGOLayer->setVisible(true);
    m_pGameOverMenu->setVisible(false);
	if (m_remainingCountTubes == 0)
	{
		//win
		m_pResultbackgroundLose->setVisible(false);
        m_menuItemGONext->setVisible(true);
        m_menuItemGORetry->setVisible(false);

		if (m_level == m_maxUnlockedLevel)
		{
			UserData::sharedUserData()->setUnlockedLevel(m_level + 1);
		}
		
		int oldBestStepCount = UserData::sharedUserData()->getBestStepCount(m_level);
		int oldBestTime = UserData::sharedUserData()->getBestTime(m_level);
		if (oldBestStepCount > m_stepCount)
		{
			UserData::sharedUserData()->setBestStepCount(m_level,m_stepCount);
		}
		if (oldBestTime > m_pTimer->getTime())
		{
			UserData::sharedUserData()->setBestTime(m_level,m_pTimer->getTime());
		}
	}
	else
	{
		//lose
		m_pResultbackgroundWin->setVisible(false);
        m_menuItemGONext->setVisible(false);
        m_menuItemGORetry->setVisible(true);
        
        CocosDenshion::SimpleAudioEngine::sharedEngine()->playEffect("FAIL.mp3", false);
	}
    m_pGameOverMenu->runAction(CCSequence::create(CCDelayTime::create(1),
                       CCCallFunc::create(this, callfunc_selector(PuzzleGame::showGameoverMenuCallback)),NULL));
	//m_pGOLayer->runAction(CCFadeIn::create(0.4));
}

void PuzzleGame::showGameoverMenuCallback()
{
    m_pGameOverMenu->setVisible(true);
}

void PuzzleGame::initTeleportGateAll()
{
    float backgroundWidth =m_pBackground->getContentSize().width + 2 * tileWidth;
    float backgroundHeight = m_pBackground->getContentSize().height + 2 * tileHeight;
	CCRenderTexture *pRtTpAll = CCRenderTexture::create(backgroundWidth,backgroundHeight);
	
    CCSprite * pSptp = getFixedSizeSprite("teleport.png",tileWidth, tileHeight);
    
	float pSptpWidth = pSptp->getContentSize().width;
	float pSptpHeight= pSptp->getContentSize().height;
    
	pRtTpAll->begin();
    for (int i=0;i<n_tile_x;i++)
	{
        for (int j=0;j<n_tile_y;j++)
        {
            if (m_infoGrid[i][j]) {
                
                float anchorpointInMiddle_y = m_infoGrid[i][j]->getPosition().y + pSptpHeight/2 + tileWidth;
                float anchorpointInMiddle_x = m_infoGrid[i][j]->getPosition().x + pSptpWidth/2 + tileHeight;
                
                CCTubeGridTile * pSprite = (CCTubeGridTile *)m_infoGrid[i][j];
                
                if (j + 1 >n_tile_y -1 || NULL == m_infoGrid[i][j + 1])
                {
                    //add
                    pSptp->setRotation(180);
                    float pos_y = backgroundHeight - ( anchorpointInMiddle_y + pSptpHeight);
                    float pos_x = anchorpointInMiddle_x;
                    pSptp->setPosition(ccp(pos_x, pos_y));
                    pSptp->visit();
                    
                    TELEPORT_INFO teleportInfo = pSprite->getTeleportInfo();
                    teleportInfo.top = true;
                    pSprite->setTeleportInfo(teleportInfo);
                }
                if (j - 1 < 0 || NULL == m_infoGrid[i][j - 1])
                {
                    //add
                    pSptp->setRotation(0);
                    float pos_y = backgroundHeight - ( anchorpointInMiddle_y - pSptpHeight);
                    float pos_x = anchorpointInMiddle_x;
                    pSptp->setPosition(ccp(pos_x, pos_y));
                    pSptp->visit();
                    
                    TELEPORT_INFO teleportInfo = pSprite->getTeleportInfo();
                    teleportInfo.bottom = true;
                    pSprite->setTeleportInfo(teleportInfo);
                }
                if (i + 1 >n_tile_x -1 || NULL == m_infoGrid[i+1][j])
                {
                    //add
                    pSptp->setRotation(90);
                    float pos_y = backgroundHeight - anchorpointInMiddle_y;
                    float pos_x = anchorpointInMiddle_x + pSptpWidth;
                    pSptp->setPosition(ccp(pos_x, pos_y));
                    pSptp->visit();
                    
                    TELEPORT_INFO teleportInfo = pSprite->getTeleportInfo();
                    teleportInfo.right = true;
                    pSprite->setTeleportInfo(teleportInfo);
                }
                if (i - 1 < 0 || NULL == m_infoGrid[i-1][j])
                {
                    //add
                    pSptp->setRotation(270);
                    float pos_y = backgroundHeight - anchorpointInMiddle_y;
                    float pos_x = anchorpointInMiddle_x - pSptpWidth;
                    pSptp->setPosition(ccp(pos_x, pos_y));
                    pSptp->visit();
                    
                    TELEPORT_INFO teleportInfo = pSprite->getTeleportInfo();
                    teleportInfo.left = true;
                    pSprite->setTeleportInfo(teleportInfo);
                }
            }
        }
	}

	pRtTpAll->end();
	m_teleportGateAll = CCSprite::createWithTexture(pRtTpAll->getSprite()->getTexture());
	m_teleportGateAll->setPosition(ccp(m_pBackground->getContentSize().width/2,m_pBackground->getContentSize().height/2));
	m_teleportGateAll->setVisible(false);
	m_pBackground->addChild(m_teleportGateAll);
}

void PuzzleGame::beginErasingTile(CCTubeGridTile *pTile, float speed, int startdirection)
{
	pTile->beginErasing(speed,startdirection);
    
    toggleTeleport(pTile, startdirection);
    
    m_pAturboMenuItem->setVisible(true);
}

void PuzzleGame::toggleTeleport(CCTubeGridTile *pTile, int startdirection)
{
    bool showTeleport = false;
    bool tGateVisibleOld = m_teleportGateAll->isVisible();
    TELEPORT_INFO tInfo = pTile->getTeleportInfo();
    int endDirection = pTile->getEndErasingDirection();
    
    if (startdirection == DIRECTION_TOP && tInfo.top) {
        if (tGateVisibleOld) {
            showTeleport = true;
        }
    }
    else if (startdirection == DIRECTION_RIGHT && tInfo.right) {
        if (tGateVisibleOld) {
            showTeleport = true;
        }
    }
    else if (startdirection == DIRECTION_BOTTON && tInfo.bottom) {
        if (tGateVisibleOld) {
            showTeleport = true;
        }
    }
    else if (startdirection == DIRECTION_LEFT && tInfo.left) {
        if (tGateVisibleOld) {
            showTeleport = true;
        }
    }
    
    if (endDirection == DIRECTION_TOP && tInfo.top) {
        showTeleport = true;
    }
    else if (endDirection == DIRECTION_RIGHT && tInfo.right) {
        showTeleport = true;
    }
    else if (endDirection == DIRECTION_BOTTON && tInfo.bottom) {
        showTeleport = true;
    }
    else if (endDirection == DIRECTION_LEFT && tInfo.left) {
        showTeleport = true;
    }
    
    m_teleportGateAll->setVisible(showTeleport);
}

PuzzleTimer* PuzzleTimer::create(const char *string, const char *fontName, float fontSize)
{
	PuzzleTimer *pRet = new PuzzleTimer();
	if(pRet && pRet->initWithString(string, fontName, fontSize, CCSizeZero, kCCTextAlignmentCenter, kCCVerticalTextAlignmentTop))
	{
		pRet->m_mytime = 0;
		ccColor3B color = {0,0,0};
		pRet->setColor(color);
		pRet->autorelease();
		return pRet;
	}
	CC_SAFE_DELETE(pRet);
	return NULL;
}

PuzzleTimer* PuzzleTimer::create()
{
	return PuzzleTimer::create("00:00", "Arial", 18);
}

void PuzzleTimer::start()
{
	//every second
	schedule(schedule_selector(PuzzleTimer::updateTime),1.0f);
}
void PuzzleTimer::stop()
{
	unschedule(schedule_selector(PuzzleTimer::updateTime));
}
void PuzzleTimer::updateTime(float delta)
{
	m_mytime ++;
	sprintf(m_pstrTime,"%02d:%02d",m_mytime/60, m_mytime%60);
	setString(m_pstrTime);
}


