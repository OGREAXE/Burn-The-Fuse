#include "PuzzleCreator.h"
#include "AppMacros.h"
#include "MyMapTile.h"
#include "LevelSelectScene.h"
#include <string>
#include "PuzzleTool.h"

PuzzleCreator::~PuzzleCreator()
{
	m_vtilesArray->release();
}

CCScene* PuzzleCreator::scene()
{
    // 'scene' is an autorelease object
    CCScene *scene = CCScene::create();
    
    // 'layer' is an autorelease object
    PuzzleCreator *layer = PuzzleCreator::create();

    // add layer as a child to scene
    scene->addChild(layer);

    // return the scene
    return scene;
}

// on "init" you need to initialize your instance
bool PuzzleCreator::init()
{
    //////////////////////////////
    // 1. super init first
    if ( !CCLayer::init() )
    {
        return false;
    }
	setTouchEnabled(true);
	initBaseData();
	initBackground();
//	initTubeNet();
	initUI();
    /////////////////////////////
    // 3. add your codes below...
    return true;
}
void PuzzleCreator::registerWithTouchDispatcher()           
{
	CCDirector::sharedDirector()->getTouchDispatcher()->addTargetedDelegate(this, 0, true);
}

void PuzzleCreator::menuCloseCallback(CCObject* pSender)
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WINRT) || (CC_TARGET_PLATFORM == CC_PLATFORM_WP8)
	CCMessageBox("You pressed the close button. Windows Store Apps do not implement a close button.","Alert");
#else
    CCDirector::sharedDirector()->end();
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    exit(0);
#endif
#endif
}

void PuzzleCreator::initBackground()
{
	int GridWidth = tileWidth*n_tile_x;
	int GridHeight = tileHeight*n_tile_y;
	//ccColor4F colorBackground = {0.7f,0.7f,0.7f,1.0f};
	ccColor4F colorBackground = {0.8f,0.8f,0.8f,1.0f};

	CCRenderTexture*pRt = CCRenderTexture::create(GridWidth,GridHeight);
	pRt->begin();
	ccDrawSolidRect(ccp(0,0),ccp(GridWidth,GridHeight),colorBackground);
	//grid should be 15 * 10   ()
	ccDrawColor4F(0,0,0,1);
	glLineWidth(2);
	for (int i = 0; i < n_tile_x; i++)
	{
		ccDrawLine(ccp(tileWidth*(1+i),0),ccp(tileWidth*(1+i),GridHeight));
	}
	for (int i = 0; i < n_tile_y-1; i++)
	{
		ccDrawLine(ccp(0,tileHeight*(1+i)),ccp(GridWidth,tileHeight*(1+i)));
	}
	
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
		m_pBackground->setPosition(ccp(240,140));
		this->addChild(m_pBackground);
	}

	m_backgroundOrigin = ccpAdd(m_pBackground->getPosition(),ccp(-m_pBackground->getContentSize().width/2,-m_pBackground->getContentSize().height/2));

	//start sprite
	m_startPointSprite =CCSprite::create("direction.png");
	m_pBackground->addChild(m_startPointSprite,2);
	m_startPointSprite->setAnchorPoint(ccp(0.5,0.5));
	m_startPointSprite->setVisible(FALSE);
}


bool PuzzleCreator::ccTouchBegan(cocos2d::CCTouch *pTouch, cocos2d::CCEvent *pEvent)
{
	CCPoint beginPoint = pTouch->getLocationInView();
	beginPoint = CCDirector::sharedDirector()->convertToGL(beginPoint);

	float fXOffset = beginPoint.x - m_backgroundOrigin.x;
	float fYOffset = beginPoint.y - m_backgroundOrigin.y;

	int tileX = fXOffset/tileWidth;
	int tileY = fYOffset/tileHeight;

	if (tileX< 0 || tileY <0 || tileX >n_tile_x - 1 || tileY >n_tile_y - 1)
	{
		return true;
	}

	switch (m_currentOperationType)
	{
	case OPERATION_TYPE_TILE:
		{
			createType(tileX,tileY);
		}
		break;
	case OPERATION_TYPE_START:
		{
			m_startPointSprite->setVisible(true);		
			m_startPointSprite->setPosition(ccp(tileWidth*tileX+tileWidth/2,tileHeight*tileY+tileHeight/2));
			m_startInfo.setPos(tileX,tileY);
		}
		break;
	case OPERATION_TYPE_DESTROY:
		{
			destroyType(tileX,tileY);
		}
		break;
	default:
		break;
	}
	

	return true;
}

void PuzzleCreator::ccTouchMoved(CCTouch *pTouch, CCEvent *pEvent)
{

}
void PuzzleCreator::ccTouchEnded(CCTouch *pTouch, CCEvent *pEvent)
{

}

CCGridTileBase *** PuzzleCreator::newInfoGrid(int n_x,int n_y)
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

void PuzzleCreator::deleteInfoGrid(CCGridTileBase ***pInfoGrip,int n_x)
{
	for (int i=0;i<n_x;i++)
	{
		free(pInfoGrip[i]);
	}
	free(pInfoGrip);
}

void PuzzleCreator::createType(int x, int y)
{
	if (m_currentOperationType ==  OPERATION_TYPE_TILE)
	{	
		CCGridTileBase*pMaptile = CCTubeGridTile::createWithType(m_subtype);
		pMaptile->setGridCoordPosition(x,y);
		CCPoint tPosition = ccp(tileWidth*pMaptile->grid_x,tileHeight*pMaptile->grid_y);
		pMaptile->setAnchorPoint(ccp(0,0));
		pMaptile->setPosition(tPosition);
		m_pBackground->addChild(pMaptile);		
		m_infoGrid[x][y] = pMaptile;
		m_vtilesArray->addObject(pMaptile);
	}
}

void PuzzleCreator::destroyType(int x, int y)
{
	CCGridTileBase*pMaptile = m_infoGrid[x][y];
	if (pMaptile)
	{
		m_infoGrid[x][y] = NULL;
		m_vtilesArray->removeObject(pMaptile);
		m_pBackground->removeChild(pMaptile,true);
	}
}

void PuzzleCreator::initBaseData()
{
	n_tile_x = 14;
	n_tile_y = 8;
	m_infoGrid = newInfoGrid(n_tile_x,n_tile_y);

	m_vtilesArray = CCArray::createWithCapacity(n_tile_x * n_tile_y);
	m_vtilesArray->retain();

	m_strLevel = "0";

}

void PuzzleCreator::initUI()
{
	CCSize visibleSize = CCDirector::sharedDirector()->getVisibleSize();
	CCPoint origin = CCDirector::sharedDirector()->getVisibleOrigin();
	
	//shows when tap on button
	m_selectedFrame = CCSprite::create("selected_frame.png");
	addChild(m_selectedFrame,1);
	m_selectedFrame->setPosition(ccp(-100,-100));
	/////////////////////////////
	// create tile button
	CCMenuItemImage *pLine1hItem = CCMenuItemImage::create("line_1_h.png","line_1_h.png",
		this,menu_selector(PuzzleCreator::menuLine1hCallback));
	pLine1hItem->setPosition(ccp(origin.x + pLine1hItem->getContentSize().width ,
		origin.y + visibleSize.height - pLine1hItem->getContentSize().height));

	CCMenuItemImage *pLine1vItem = CCMenuItemImage::create("line_1_v.png","line_1_v.png",
		this,menu_selector(PuzzleCreator::menuLine1vCallback));
	pLine1vItem->setPosition(ccpAdd(pLine1hItem->getPosition(),ccp(tileWidth,0)));

	CCMenuItemImage *pLine2hItem = CCMenuItemImage::create("line_2_h.png","line_2_h.png",
		this,menu_selector(PuzzleCreator::menuLine2hCallback));
	pLine2hItem->setPosition(ccpAdd(pLine1hItem->getPosition(),ccp(2*tileWidth,0)));

	CCMenuItemImage *pLine2vItem = CCMenuItemImage::create("line_2_v.png","line_2_v.png",
		this,menu_selector(PuzzleCreator::menuLine2vCallback));
	pLine2vItem->setPosition(ccpAdd(pLine1hItem->getPosition(),ccp(3*tileWidth,0)));

	CCMenuItemImage *pTube1ulItem = CCMenuItemImage::create("tube_1_ul.png","tube_1_ul.png",
		this,menu_selector(PuzzleCreator::menuTube1ulCallback));
	pTube1ulItem->setPosition(ccpAdd(pLine1hItem->getPosition(),ccp(4*tileWidth,0)));

	CCMenuItemImage *pTube1urItem = CCMenuItemImage::create("tube_1_ur.png","tube_1_ur.png",
		this,menu_selector(PuzzleCreator::menuTube1urCallback));
	pTube1urItem->setPosition(ccpAdd(pLine1hItem->getPosition(),ccp(5*tileWidth,0)));

	CCMenuItemImage *pTube1blItem = CCMenuItemImage::create("tube_1_bl.png","tube_1_bl.png",
		this,menu_selector(PuzzleCreator::menuTube1blCallback));
	pTube1blItem->setPosition(ccpAdd(pLine1hItem->getPosition(),ccp(6*tileWidth,0)));

	CCMenuItemImage *pTube1brItem = CCMenuItemImage::create("tube_1_br.png","tube_1_br.png",
		this,menu_selector(PuzzleCreator::menuTube1brCallback));
	pTube1brItem->setPosition(ccpAdd(pLine1hItem->getPosition(),ccp(7*tileWidth,0)));

	CCMenuItemImage *pTube2upItem = CCMenuItemImage::create("tube_2_up.png","tube_2_up.png",
		this,menu_selector(PuzzleCreator::menuTube2upCallback));
	pTube2upItem->setPosition(ccpAdd(pLine1hItem->getPosition(),ccp(8*tileWidth,0)));

	CCMenuItemImage *pTube2downItem = CCMenuItemImage::create("tube_2_down.png","tube_2_down.png",
		this,menu_selector(PuzzleCreator::menuTube2downCallback));
	pTube2downItem->setPosition(ccpAdd(pLine1hItem->getPosition(),ccp(9*tileWidth,0)));

	CCMenuItemImage *pEmptyItem = CCMenuItemImage::create("empty.png","empty.png",
		this,menu_selector(PuzzleCreator::menuEmptyCallback));
	pEmptyItem->setPosition(ccpAdd(pLine1hItem->getPosition(),ccp(10*tileWidth,0)));

	CCMenuItemImage *pVoidItem = CCMenuItemImage::create("void.png","void.png",
		this,menu_selector(PuzzleCreator::menuVoidCallback));
	pVoidItem->setPosition(ccpAdd(pLine1hItem->getPosition(),ccp(11*tileWidth,0)));

	//direction of starting fire
	CCMenuItemImage *pDirectionLeftItem = CCMenuItemImage::create("direction.png","direction.png",
		this,menu_selector(PuzzleCreator::menuDirectionLeftCallback));
	pDirectionLeftItem->setPosition(ccpAdd(pLine1hItem->getPosition(),ccp(0,-tileHeight)));

	CCMenuItemImage *pDirectionRightItem = CCMenuItemImage::create("direction.png","direction.png",
		this,menu_selector(PuzzleCreator::menuDirectionRightCallback));
	pDirectionRightItem->setRotation(180);
	pDirectionRightItem->setPosition(ccpAdd(pDirectionLeftItem->getPosition(),ccp(tileWidth,0)));

	CCMenuItemImage *pDirectionDownItem = CCMenuItemImage::create("direction.png","direction.png",
		this,menu_selector(PuzzleCreator::menuDirectionDownCallback));
	pDirectionDownItem->setRotation(270);
	pDirectionDownItem->setPosition(ccpAdd(pDirectionLeftItem->getPosition(),ccp(2*tileWidth,0)));

	CCMenuItemImage *pDirectionUpItem = CCMenuItemImage::create("direction.png","direction.png",
		this,menu_selector(PuzzleCreator::menuDirectionUpCallback));
	pDirectionUpItem->setRotation(90);
	pDirectionUpItem->setPosition(ccpAdd(pDirectionLeftItem->getPosition(),ccp(3*tileWidth,0)));

	//destroy button
	CCMenuItemImage *pDestroyItem = CCMenuItemImage::create("destroy.png","destroy.png",
		this,menu_selector(PuzzleCreator::menuDestroyCallback));
	pDestroyItem->setPosition(ccpAdd(pVoidItem->getPosition(),ccp(2*tileWidth,0)));

	CCMenuItemImage *pGenItem = CCMenuItemImage::create("save.png","save.png",
		this,menu_selector(PuzzleCreator::menuGenCallback));
	pGenItem->setPosition(ccpAdd(pLine1hItem->getPosition(),ccp(11*tileWidth,-tileHeight)));
	CCMenuItemImage *pExitItem = CCMenuItemImage::create("exit.png","exit.png",
		this,menu_selector(PuzzleCreator::menuExitCallback));
	pExitItem->setPosition(ccpAdd(pGenItem->getPosition(),ccp(pGenItem->getContentSize().width+3,0)));



	//create teleport gate botton

	// create menu, it's an autorelease object
	CCMenu* pMenu = CCMenu::create
		(pLine1hItem,pLine1vItem,pLine2hItem,pLine2vItem,pTube1ulItem,pTube1urItem,pTube1blItem,pTube1brItem, pTube2upItem,pTube2downItem,pEmptyItem, pVoidItem,
		pDestroyItem,pGenItem,pExitItem,
		pDirectionLeftItem,pDirectionRightItem,pDirectionDownItem,pDirectionUpItem,NULL);

	pMenu->setPosition(CCPointZero);
	this->addChild(pMenu, 1);

	//set the editbox of level
	
	CCEditBox*pEdit = CCEditBox::create(CCSizeMake(50,15),CCScale9Sprite::create("empty.png"));
	pEdit->setPosition(ccpAdd(pLine1hItem->getPosition(),ccp(9*tileWidth,-tileHeight)));
	pEdit->setFontSize(35);
	pEdit->setFontColor(ccRED);
	pEdit->setInputMode(kEditBoxInputModePhoneNumber);
	pEdit->setDelegate(this);
	pEdit->setTouchPriority(0);
	this->addChild(pEdit,1);
}


void PuzzleCreator::setSelectedAndType(CCObject*pItmNode,OPERATION_TYPE optype,int subtype)
{
	CCNode * pNodeItem = dynamic_cast<CCNode*>(pItmNode);
	if (pNodeItem)
	{
		m_selectedFrame->setPosition(pNodeItem->getPosition());
	}

	m_currentOperationType = optype;
	m_subtype = subtype;
}
void PuzzleCreator::menuLine1hCallback(CCObject* pSender)
{
	setSelectedAndType(pSender, OPERATION_TYPE_TILE,TILETYPE_LINE_ONE_HORIZONTAL);
}

void PuzzleCreator::menuLine1vCallback(CCObject* pSender)
{
	setSelectedAndType(pSender, OPERATION_TYPE_TILE,TILETYPE_LINE_ONE_VERTICAL);
}

void PuzzleCreator::menuLine2hCallback(CCObject* pSender)
{
	setSelectedAndType(pSender, OPERATION_TYPE_TILE,TILETYPE_LINE_TWO_HORIZONTALONTOP);
}

void PuzzleCreator::menuLine2vCallback(CCObject* pSender)
{
	setSelectedAndType(pSender, OPERATION_TYPE_TILE,TILETYPE_LINE_TWO_VERTICALONTOP);
}

void PuzzleCreator::menuTube1ulCallback(CCObject* pSender)
{
	setSelectedAndType(pSender,OPERATION_TYPE_TILE, TILETYPE_CURVE_ONE_TOPLEFT);
}

void PuzzleCreator::menuTube1urCallback(CCObject* pSender)
{
	setSelectedAndType(pSender,OPERATION_TYPE_TILE, TILETYPE_CURVE_ONE_TOPRIGHT);
}

void PuzzleCreator::menuTube1blCallback(CCObject* pSender)
{
	setSelectedAndType(pSender,OPERATION_TYPE_TILE, TILETYPE_CURVE_ONE_BOTTONLEFT);
}

void PuzzleCreator::menuTube1brCallback(CCObject* pSender)
{
	setSelectedAndType(pSender,OPERATION_TYPE_TILE, TILETYPE_CURVE_ONE_BOTTONRIGHT);
}

void PuzzleCreator::menuTube2upCallback(CCObject* pSender)
{
	setSelectedAndType(pSender,OPERATION_TYPE_TILE, TILETYPE_CURVE_TWO_TOPLEFT_BOTTONRIGHT);
}

void PuzzleCreator::menuTube2downCallback(CCObject* pSender)
{
	setSelectedAndType(pSender,OPERATION_TYPE_TILE, TILETYPE_CURVE_TWO_BOTTONLEFT_TOPRIGHT);
}

void PuzzleCreator::menuEmptyCallback(CCObject* pSender)
{
	setSelectedAndType(pSender,OPERATION_TYPE_TILE, TILETYPE_NOTUBE);
}

void PuzzleCreator::menuVoidCallback(CCObject* pSender)
{
	setSelectedAndType(pSender,OPERATION_TYPE_TILE, TILETYPE_VOID);
}

void PuzzleCreator::menuDestroyCallback(CCObject* pSender)
{
	setSelectedAndType(pSender,OPERATION_TYPE_DESTROY);
}

bool PuzzleCreator::writeTilesDataToFile()
{
	char strBuf[8];
	std::string strObject;
	//CCString startInfo("left%2%5");
	std::string startInfo;
	char *VAL_DIV = "%";
	CCString m_canTeleport("true");
	CCArray*pTilesArray = CCArray::createWithCapacity(m_vtilesArray->count());
	const static char*dataFilename = "puzzledata.plist";
	const char*levelname = m_strLevel.c_str();
	
	CCDictionary* pDictTop = CCDictionary::createWithContentsOfFile(dataFilename);
	CCDictionary* pDictSub = CCDictionary::create();

	//parse startinfo
	//itoa(m_startInfo.start_direction,strBuf,10);
	sprintf(strBuf, "%d",m_startInfo.start_direction);
	startInfo = strBuf;
	startInfo.append(VAL_DIV);
	//itoa(m_startInfo.x,strBuf,10);
	sprintf(strBuf, "%d",m_startInfo.x);
	startInfo.append(strBuf);
	startInfo.append(VAL_DIV);
	//itoa(m_startInfo.y,strBuf,10);
	sprintf(strBuf, "%d",m_startInfo.y);
	startInfo.append(strBuf);

	CCObject *pDataObj;
	CCARRAY_FOREACH(m_vtilesArray,pDataObj)
	{
		CCTubeGridTile*pTile = dynamic_cast<CCTubeGridTile*>(pDataObj);
		if (pTile)
		{
			//itoa(pTile->type,strBuf,10);
			sprintf(strBuf, "%d",pTile->type);
			strObject = strBuf;
			strObject.append(VAL_DIV);

			//itoa(pTile->grid_x,strBuf,10);
			sprintf(strBuf, "%d",pTile->grid_x);
			strObject.append(strBuf);
			strObject.append(VAL_DIV);

			//itoa(pTile->grid_y,strBuf,10);
			sprintf(strBuf, "%d",pTile->grid_y);
			strObject.append(strBuf);

			pTilesArray->addObject(CCString::create(strObject));
		}
	}
	pDictSub->setObject(pTilesArray,"data");
	pDictSub->setObject(&m_canTeleport,"teleport");
	CCString*pCCstrSi = CCString::create(startInfo);
	pDictSub->setObject(pCCstrSi,"start");
	pDictSub->removeObjectForKey(levelname);
	pDictTop->setObject(pDictSub,levelname);
	
	return pDictTop->writeToFile(dataFilename);;
}

void PuzzleCreator::menuGenCallback(CCObject* pSender)
{
	if(writeTilesDataToFile())
		if (createThumbnail())
		{
			CCMessageBox("Create Success!","ok");
		}
}

void PuzzleCreator::menuDirectionUpCallback(CCObject* pSender)
{
	//start direction is opposed to point of sign
	m_startInfo.start_direction = DIRECTION_BOTTON;
	m_startPointSprite->setRotation(90);
	setSelectedAndType(pSender,OPERATION_TYPE_START);
}

void PuzzleCreator::menuDirectionDownCallback(CCObject* pSender)
{
	m_startInfo.start_direction = DIRECTION_TOP;
	m_startPointSprite->setRotation(270);
	setSelectedAndType(pSender,OPERATION_TYPE_START);
}

void PuzzleCreator::menuDirectionLeftCallback(CCObject* pSender)
{
	m_startInfo.start_direction = DIRECTION_RIGHT;
	m_startPointSprite->setRotation(0);
	setSelectedAndType(pSender,OPERATION_TYPE_START);
}

void PuzzleCreator::menuDirectionRightCallback(CCObject* pSender)
{
	m_startInfo.start_direction = DIRECTION_LEFT;
	m_startPointSprite->setRotation(180);
	setSelectedAndType(pSender,OPERATION_TYPE_START);
}

void PuzzleCreator::editBoxReturn(CCEditBox* editBox)
{
	m_strLevel=editBox->getText();
}

void PuzzleCreator::menuExitCallback(CCObject* pSender)
{
	CCScene *pScene = LevelSelector::scene();
	CCDirector::sharedDirector()->replaceScene(pScene);
}

bool PuzzleCreator::createThumbnail()
{
	bool bOk = false;
	int GridWidth = tileWidth*n_tile_x;
	int GridHeight = tileHeight*n_tile_y;
	CCRenderTexture*pRtThumbnail = CCRenderTexture::create(GridWidth,GridHeight);
	pRtThumbnail->begin();

	ccDrawSolidRect(ccp(0,0),ccp(GridWidth,GridHeight),ccc4FFromccc3B(ccWHITE));

	CCObject*pObject = NULL;
	CCARRAY_FOREACH(m_vtilesArray,pObject)
	{
		CCGridTileBase*pMaptile = dynamic_cast<CCGridTileBase*>(pObject);
		if (pMaptile)
		{
			//set thumbnail background color under tile to same with tile background
			//thus erase the black line
			float drawPosX = tileWidth * pMaptile->grid_x ;
			float drawPosY = tileHeight * pMaptile->grid_y;
			ccDrawSolidRect(ccp(drawPosX,drawPosY),ccp(drawPosX + tileWidth,drawPosY + tileHeight),tileBackgroundColor);


			CCPoint tPosition = ccp(tileWidth*pMaptile->grid_x,tileHeight*pMaptile->grid_y);
			pMaptile->setAnchorPoint(ccp(0,0));
			pMaptile->setPosition(tPosition);
			pMaptile->visit();			
		}		
	}
	pRtThumbnail->end();
	char filename[20];
	sprintf(filename,"thumb%s.png",m_strLevel.c_str());
	bOk = pRtThumbnail->saveToFile(filename,kCCImageFormatPNG);
	return bOk;
}



