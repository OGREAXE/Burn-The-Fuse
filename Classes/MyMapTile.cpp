#include "MyMapTile.h"
#include "math.h"
#define FALSE false
#define true true
CCTubeGridTile::CCTubeGridTile(void)
{
	m_isErasing = FALSE;
	m_curveRadius = tileWidth/2;
	m_eraserRadius = 5.7;
    
    m_rtCurve = NULL;
    m_rtCurve2 = NULL;
    m_rtLine = NULL;
    m_rtLine2 = NULL;
}

CCSprite* CCTubeGridTile::m_singleCurve = NULL;
CCSprite* CCTubeGridTile::m_singleLine = NULL;

CCTubeGridTile::~CCTubeGridTile(void)
{
    if (m_rtCurve) {
        m_rtCurve->release();
    }
    if (m_rtCurve2) {
        m_rtCurve2->release();
    }
    if (m_rtLine) {
        m_rtLine->release();
    }
    if (m_rtLine2) {
        m_rtLine2->release();
    }
}

static void doClip_unrotated_bottomLeft(CCSprite*podChild,float clipx,float clipy);
static void doClip_unrotated_topright(CCSprite*podChild,float clipx,float clipy);

bool CCTubeGridTile::initWithType(int type, bool backGround,TileColor color)
{
    CCRenderTexture* pRt = CCRenderTexture::create(tileWidth,tileHeight);
	if (backGround)
	{
		pRt->begin();
        
		float flineWidth = 0.3;
		ccDrawSolidRect(ccp(flineWidth,flineWidth),ccp(tileWidth-flineWidth, tileHeight-flineWidth),tileBackgroundColor);
        
		pRt->end();
	}
    
	if(!initWithTexture(pRt->getSprite()->getTexture()))return false;
	
    
	//do image initialization with type
    CCPoint childPosWithOffset = ccp(tileWidth/2, tileHeight/2);
    ChildTubeSprite* podChild1 = NULL;
    ChildTubeSprite* podChild2 = NULL;
    
    this->type = type;
	m_canGetFireLocation = false;
	if (TILETYPE_LINE_ONE_HORIZONTAL == type||
		TILETYPE_LINE_ONE_VERTICAL == type||
		TILETYPE_LINE_TWO_HORIZONTALONTOP == type||
		TILETYPE_LINE_TWO_VERTICALONTOP == type)
	{
		genericType = GENERIC_TYPE_LINE;
        initLine1(color);
        if (TILETYPE_LINE_TWO_HORIZONTALONTOP == type||
            TILETYPE_LINE_TWO_VERTICALONTOP == type) {
            initLine2(color);
        }
	}
	else
	{
		genericType = GENERIC_TYPE_CURVE;
        initCurve1(color);
        if (TILETYPE_CURVE_TWO_BOTTONLEFT_TOPRIGHT == type||
            TILETYPE_CURVE_TWO_TOPLEFT_BOTTONRIGHT == type) {
            initCurve2(color);
        }
	}
    
	switch (type)
	{
        case TILETYPE_VOID:
		{
            //create the void tile render texture
            m_rtVoid = CCRenderTexture::create(tileWidth,tileHeight);
            m_rtVoid->begin();
            ccColor4F colorVoid = {0,0,0
                ,1.0f};
            ccDrawSolidRect(ccp(0,0),ccp(tileWidth, tileHeight),colorVoid);
            m_rtVoid->end();
			setTexture(m_rtVoid->getSprite()->getTexture());
			break;
		}
        case TILETYPE_NOTUBE:
		{
			break;
		}
        case TILETYPE_CURVE_ONE_BOTTONLEFT:
		{
            
            podChild1 = ChildTubeSprite::createWithTexture(m_rtCurve->getSprite()->getTexture());
			podChild1->setPosition(childPosWithOffset);
			podChild1->setRotation(-90);
			podChild1->setTag(TAG_CURVE_BOTTONLEFT);
			addChild(podChild1,Z_CHILD1);
			break;
		}
        case TILETYPE_CURVE_ONE_BOTTONRIGHT:
		{
            podChild1 = ChildTubeSprite::createWithTexture(m_rtCurve->getSprite()->getTexture());
			podChild1->setPosition(childPosWithOffset);
			podChild1->setRotation(-180);
			podChild1->setTag(TAG_CURVE_BOTTONRIGHT);
			addChild(podChild1,Z_CHILD1);
			break;
		}
        case TILETYPE_CURVE_ONE_TOPLEFT:
		{
			podChild1 = ChildTubeSprite::createWithTexture(m_rtCurve->getSprite()->getTexture());
			podChild1->setPosition(childPosWithOffset);
			podChild1->setTag(TAG_CURVE_TOPLEFT);
			addChild(podChild1,Z_CHILD1);
			break;
		}
        case TILETYPE_CURVE_ONE_TOPRIGHT:
		{
			podChild1 = ChildTubeSprite::createWithTexture(m_rtCurve->getSprite()->getTexture());
			podChild1->setPosition(childPosWithOffset);
			podChild1->setRotation(90);
			podChild1->setTag(TAG_CURVE_TOPRIGHT);
			addChild(podChild1,Z_CHILD1);
			break;
		}
        case TILETYPE_CURVE_TWO_TOPLEFT_BOTTONRIGHT:
		{
			podChild1 = ChildTubeSprite::createWithTexture(m_rtCurve->getSprite()->getTexture());
			podChild1->setPosition(childPosWithOffset);
            
			podChild1->setTag(TAG_CURVE_TOPLEFT);
            addChild(podChild1,Z_CHILD1);
            
			podChild2 = ChildTubeSprite::createWithTexture(m_rtCurve2->getSprite()->getTexture());
			podChild2->setPosition(childPosWithOffset);
			podChild2->setRotation(180);
			podChild2->setTag(TAG_CURVE_BOTTONRIGHT);
			addChild(podChild2,Z_CHILD2);
            
			break;
		}
        case TILETYPE_CURVE_TWO_BOTTONLEFT_TOPRIGHT:
		{
			podChild1 = ChildTubeSprite::createWithTexture(m_rtCurve->getSprite()->getTexture());
			podChild1->setPosition(childPosWithOffset);
			podChild1->setRotation(-90);
			podChild1->setTag(TAG_CURVE_BOTTONLEFT);
			addChild(podChild1,Z_CHILD1);
            
			podChild2 = ChildTubeSprite::createWithTexture(m_rtCurve2->getSprite()->getTexture());
			podChild2->setPosition(childPosWithOffset);
			podChild2->setRotation(90);
			podChild2->setTag(TAG_CURVE_TOPRIGHT);
            
			addChild(podChild2,Z_CHILD2);
            
			break;
		}
        case TILETYPE_LINE_ONE_HORIZONTAL:
		{
			podChild1 = ChildTubeSprite::createWithTexture(m_rtLine->getSprite()->getTexture());
			podChild1->setPosition(childPosWithOffset);
			podChild1->setRotation(-90);
			podChild1->setTag(TAG_LINE_HORIZONTAL);
			addChild(podChild1,Z_CHILD1);
			break;
		}
        case TILETYPE_LINE_ONE_VERTICAL:
		{
			podChild1 = ChildTubeSprite::createWithTexture(m_rtLine->getSprite()->getTexture());
			podChild1->setPosition(childPosWithOffset);
			podChild1->setTag(TAG_LINE_VERTICAL);
			addChild(podChild1,Z_CHILD1);
			break;
		}
        case TILETYPE_LINE_TWO_HORIZONTALONTOP:
		{
			podChild1 = ChildTubeSprite::createWithTexture(m_rtLine->getSprite()->getTexture());
			podChild1->setPosition(childPosWithOffset);
			podChild1->setTag(TAG_LINE_VERTICAL);
			addChild(podChild1,Z_CHILD1);
            
			podChild2 = ChildTubeSprite::createWithTexture(m_rtLine2->getSprite()->getTexture());
			podChild2->setPosition(childPosWithOffset);
			podChild2->setRotation(-90);
			podChild2->setTag(TAG_LINE_HORIZONTAL);
			addChild(podChild2,Z_CHILD2);
			break;
		}
        case TILETYPE_LINE_TWO_VERTICALONTOP:
		{
			podChild1 = ChildTubeSprite::createWithTexture(m_rtLine->getSprite()->getTexture());
			podChild1->setPosition(childPosWithOffset);
			podChild1->setRotation(-90);
			podChild1->setTag(TAG_LINE_HORIZONTAL);
			addChild(podChild1,Z_CHILD1);
            
			podChild2 = ChildTubeSprite::createWithTexture(m_rtLine2->getSprite()->getTexture());
			podChild2->setPosition(ccp(tileWidth/2,tileHeight/2));
			podChild2->setTag(TAG_LINE_VERTICAL);
			addChild(podChild2,Z_CHILD2);
			break;
		}
        default:
            break;
	}
    
    //////solve the not match glitch
    if (podChild1) {
        podChild1->setScale(tile_rt_scale);
    }
    if (podChild2) {
        podChild2->setScale(tile_rt_scale);
    }
    ///////////////////////
    
	
    
    //set the render texture
    if (genericType == GENERIC_TYPE_LINE) {
        if (podChild1) {
            podChild1->setRenderTexture(m_rtLine);
        }
        if (podChild2) {
            podChild2->setRenderTexture(m_rtLine2);
        }
    }
    else
    {
        if (podChild1) {
            podChild1->setRenderTexture(m_rtCurve);
        }
        if (podChild2) {
            podChild2->setRenderTexture(m_rtCurve2);
        }
    }
    
	//decide tubeCount
	if (TILETYPE_VOID == type||TILETYPE_NOTUBE == type)
	{
		m_remainTubes = 0;
	}
	else if (TILETYPE_LINE_TWO_VERTICALONTOP == type ||TILETYPE_LINE_TWO_HORIZONTALONTOP == type ||
             TILETYPE_CURVE_TWO_BOTTONLEFT_TOPRIGHT == type ||TILETYPE_CURVE_TWO_TOPLEFT_BOTTONRIGHT == type )
	{
		m_remainTubes = 2;
	}
	else
	{
		m_remainTubes = 1;
	}
    return true;
}

CCTubeGridTile* CCTubeGridTile::createWithType(int type, bool backGround,TileColor color)
{
    CCTubeGridTile * podTile = new CCTubeGridTile();
	if (podTile->initWithType(type,backGround,color)) {
        podTile->autorelease();
        return podTile;
    }
    CC_SAFE_DELETE(podTile);
	return NULL;
}
void CCTubeGridTile::initLine1(TileColor color)
{
    CCSprite*pSpLine = createSingleLine(color);
    
    m_rtLine = CCRenderTexture::create(tileWidth,tileHeight);
	m_rtLine->begin();
	pSpLine->setPosition(ccp(tileWidth/2,tileHeight/2));
	pSpLine->visit();
	m_rtLine->end();
    m_rtLine->retain();

}
void CCTubeGridTile::initLine2(TileColor color)
{
	CCSprite*pSpLine = CCTubeGridTile::createSingleLine(color);
	
    m_rtLine2 = CCRenderTexture::create(tileWidth,tileHeight);
	m_rtLine2->begin();
    pSpLine->setPosition(ccp(tileWidth/2,tileHeight/2));
	pSpLine->visit();
	m_rtLine2->end();
    m_rtLine2->retain();
}
void CCTubeGridTile::initCurve1(TileColor color)
{
    CCSprite*pSpCurve = CCTubeGridTile::createSingleCurve(color);
    
    m_rtCurve = CCRenderTexture::create(tileWidth,tileHeight);
	m_rtCurve->begin();
	pSpCurve->setPosition(ccp(tileWidth/2,tileHeight/2));
	pSpCurve->visit();
	m_rtCurve->end();
    m_rtCurve->retain();
    
    
}
void CCTubeGridTile::initCurve2(TileColor color)
{
    CCSprite*pSpCurve = CCTubeGridTile::createSingleCurve(color);
    
    m_rtCurve2 = CCRenderTexture::create(tileWidth,tileHeight);
	m_rtCurve2->begin();
    pSpCurve->setPosition(ccp(tileWidth/2,tileHeight/2));
	pSpCurve->visit();
	m_rtCurve2->end();
    m_rtCurve2->retain();
}

CCSprite* CCTubeGridTile::createSingleLine(TileColor color)
{
    /*std::string strColor;
    if (color == TILECOLOR_RED) {
        strColor = "tube_line_red.png";
    }
    else if (color == TILECOLOR_GREEN) {
        strColor = "tube_line_green.png";
    }
    else if (color == TILECOLOR_BLUE) {
        strColor = "tube_line_blue.png";
    }
    else if (color == TILECOLOR_PURPLE) {
        strColor = "tube_line_purple.png";
    }
    else if (color == TILECOLOR_YELLOW) {
        strColor = "tube_line_yellow.png";
    }
    else if (color == TILECOLOR_BLACK) {
        strColor = "tube_line_black.png";
    }
    
    m_singleLine = CCSprite::createWithTexture(CCTextureCache::sharedTextureCache()->textureForKey(strColor.c_str()));
    m_singleLine->retain();*/
    
    //fix at tilewidth
    //m_singleLine->setScale(tileWidth/m_singleLine->getContentSize().width);
    //return m_singleLine;
    
    CCSprite * ret = TileCache::sharedCache()->getLineByColor(color);
    ret->setScale(tileWidth/ret->getContentSize().width);
	return ret;
}

CCSprite* CCTubeGridTile::createSingleCurve(TileColor color)
{
    /*std::string strColor;
    if (color == TILECOLOR_RED) {
        strColor = "tube_curve_red.png";
    }
    else if (color == TILECOLOR_GREEN) {
        strColor = "tube_curve_green.png";
    }
    else if (color == TILECOLOR_BLUE) {
        strColor = "tube_curve_blue.png";
    }
    else if (color == TILECOLOR_PURPLE) {
        strColor = "tube_curve_purple.png";
    }
    else if (color == TILECOLOR_YELLOW) {
        strColor = "tube_curve_yellow.png";
    }
    else if (color == TILECOLOR_BLACK) {
        strColor = "tube_curve_black.png";
    }
    
    m_singleCurve = CCSprite::createWithTexture(CCTextureCache::sharedTextureCache()->textureForKey(strColor.c_str()));
    m_singleCurve->retain();
    
    m_singleCurve->setScale(tileWidth/m_singleCurve->getContentSize().width);
    return m_singleCurve;
     */
    
    CCSprite * ret = TileCache::sharedCache()->getCurveByColor(color);
    ret->setScale(tileWidth/ret->getContentSize().width);
    
	return ret;
}

void CCTubeGridTile::beginErasing(float eraserSpeed, int direction)
{
	m_erasingChild = getChildForStartErasingAt(direction);
	if (NULL == m_erasingChild)
	{
		return;
	}

    m_erasingRenderTexture = ((ChildTubeSprite*)m_erasingChild)->getRenderTexture();   

	m_eraser = CCDrawNode::create();
	m_eraser->drawDot(ccp(0,0),m_eraserRadius,ccc4f(0,0,0,0));
	m_eraser->retain();
	ccBlendFunc blendFunc = {GL_ONE,GL_ZERO};
	m_eraser->setBlendFunc(blendFunc);

	m_isErasing = true;
	m_eraserSpeed = eraserSpeed;
	m_startErasingDirection = direction;

	//start pos and angle should relate to direction	
	
	m_isReversedErasing = getIsReversedErasing(m_startErasingDirection,m_erasingChild->getTag());
	if (m_isReversedErasing)
	{
		m_eraserCurveAngleUpperBound = M_PI/2/tile_rt_scale + m_eraserRadius/(tileWidth/2);
		m_eraserCurveAngleLowerBound =  (m_eraserRadius-m_eraserSpeed)/(tileWidth/2);
		m_eraserLineUpperBound = tileHeight/tile_rt_scale + m_eraserRadius;
		m_eraserLineLowerBound = -(m_eraserSpeed - m_eraserRadius);

		m_erasingPoint = ccp(tileWidth/2,m_eraserLineUpperBound);
		m_erasingAngle = m_eraserCurveAngleUpperBound;
	}
	else
	{
		m_eraserCurveAngleUpperBound = M_PI/2-m_eraserRadius/(tileWidth/2);
		//m_eraserCurveAngleUpperBound /=rt_scale;
		m_eraserCurveAngleLowerBound = - (m_eraserRadius-m_eraserSpeed)/(tileWidth/2);
		m_eraserLineUpperBound = tileHeight - m_eraserRadius;
		m_eraserLineLowerBound = m_eraserSpeed - m_eraserRadius;
		
		m_erasingPoint = ccp(tileWidth/2,m_eraserLineLowerBound);
		m_erasingAngle = m_eraserCurveAngleLowerBound;	
	}

	m_endErasingDirection = getExitOfEraserForStartErasingAt(m_startErasingDirection);
	schedule(schedule_selector(CCTubeGridTile::updateErasing),0.02f);
}

void CCTubeGridTile::erasePoint(float x, float y)
{
	m_erasingRenderTexture->begin();
	m_eraser->setPosition(x,y);
	m_eraser->visit();
	m_erasingRenderTexture->end();
	moveFire(x,y);
}

void CCTubeGridTile::erasePointInCurveForAngle(float angle)
{
	float erasePointx = m_curveRadius*::sin(angle);
	float erasePointy = m_curveRadius*::cos(angle);
	erasePoint(erasePointx,erasePointy);
}

void CCTubeGridTile::moveFire(float x, float y)
{
	m_currentFireLoctionPoint = ccp(x,tileHeight-y);
}

CCSprite* CCTubeGridTile::getChildForStartErasingAt(int direction)
{
	CCSprite*prtSprite = NULL;
	if (DIRECTION_TOP == direction)
	{
		prtSprite = (CCSprite*)getChildByTag(TAG_CURVE_TOPLEFT);
		if (NULL == prtSprite)
		{
			prtSprite = (CCSprite*)getChildByTag(TAG_CURVE_TOPRIGHT);
		}
		if (NULL == prtSprite)
		{
			prtSprite = (CCSprite*)getChildByTag(TAG_LINE_VERTICAL);
		}
	}
	else if (DIRECTION_LEFT == direction)
	{
		prtSprite = (CCSprite*)getChildByTag(TAG_CURVE_TOPLEFT);
		if (NULL == prtSprite)
		{
			prtSprite = (CCSprite*)getChildByTag(TAG_CURVE_BOTTONLEFT);
		}
		if (NULL == prtSprite)
		{
			prtSprite = (CCSprite*)getChildByTag(TAG_LINE_HORIZONTAL);
		}
	}
	else if (DIRECTION_BOTTON == direction)
	{
		prtSprite = (CCSprite*)getChildByTag(TAG_CURVE_BOTTONLEFT);
		if (NULL == prtSprite)
		{
			prtSprite = (CCSprite*)getChildByTag(TAG_CURVE_BOTTONRIGHT);
		}
		if (NULL == prtSprite)
		{
			prtSprite = (CCSprite*)getChildByTag(TAG_LINE_VERTICAL);
		}
	}
	else if (DIRECTION_RIGHT == direction)
	{
		prtSprite = (CCSprite*)getChildByTag(TAG_CURVE_BOTTONRIGHT);
		if (NULL == prtSprite)
		{
			prtSprite = (CCSprite*)getChildByTag(TAG_CURVE_TOPRIGHT);
		}
		if (NULL == prtSprite)
		{
			prtSprite = (CCSprite*)getChildByTag(TAG_LINE_HORIZONTAL);
		}
	}

	return prtSprite;
}

void CCTubeGridTile::updateErasing(float delta)
{
	if (m_isErasing)
	{
		m_canGetFireLocation = true;
		if (GENERIC_TYPE_LINE == genericType)
		{
			if (!m_isReversedErasing)
			{
				erasePoint(m_erasingPoint.x,m_erasingPoint.y);
				m_erasingPoint = ccpAdd(m_erasingPoint,ccp(0,m_eraserSpeed));
				if (m_erasingPoint.y>m_eraserLineUpperBound)
				{
					m_isErasing = FALSE;
				}
			}
			else
			{
				erasePoint(m_erasingPoint.x,m_erasingPoint.y);
				m_erasingPoint = ccpAdd(m_erasingPoint,ccp(0,-m_eraserSpeed));
				if (m_erasingPoint.y<m_eraserLineLowerBound)
				{
					m_isErasing = FALSE;
				}
			}
			
		}
		else
		{
			if (!m_isReversedErasing)
			{
				erasePointInCurveForAngle(m_erasingAngle);
				m_erasingAngle +=m_eraserSpeed/m_curveRadius;			
				if (m_erasingAngle>m_eraserCurveAngleUpperBound)
				{
					m_isErasing = FALSE;
				}
			}
			else
			{
				erasePointInCurveForAngle(m_erasingAngle);
				m_erasingAngle -=m_eraserSpeed/m_curveRadius;
				
				if (m_erasingAngle<m_eraserCurveAngleLowerBound)
				{
					m_isErasing = FALSE;
				}
			}
		}

	}

	if (!m_isErasing)
	{
		m_canGetFireLocation = false;
		unschedule(schedule_selector(CCTubeGridTile::updateErasing));
		endErasing();
		
		m_remainTubes --;
	}
}


int CCTubeGridTile::getExitOfEraserForStartErasingAt(int direction)
{
	int exitDirection;
	if (DIRECTION_TOP == direction)
	{
		if ( TILETYPE_CURVE_ONE_TOPLEFT == type || TILETYPE_CURVE_TWO_TOPLEFT_BOTTONRIGHT == type)
		{
			exitDirection = DIRECTION_LEFT;
		}
		else if ( TILETYPE_CURVE_ONE_TOPRIGHT == type|| TILETYPE_CURVE_TWO_BOTTONLEFT_TOPRIGHT == type)
		{
			exitDirection = DIRECTION_RIGHT;
		}
		else if ( TILETYPE_LINE_ONE_VERTICAL  == type || TILETYPE_LINE_TWO_VERTICALONTOP  == type
			|| TILETYPE_LINE_TWO_HORIZONTALONTOP  == type)
		{
			exitDirection = DIRECTION_BOTTON;
		}
	}
	else if (DIRECTION_LEFT == direction)
	{
		if ( TILETYPE_CURVE_ONE_TOPLEFT == type || TILETYPE_CURVE_TWO_TOPLEFT_BOTTONRIGHT == type)
		{
			exitDirection = DIRECTION_TOP;
		}
		else if ( TILETYPE_CURVE_ONE_BOTTONLEFT == type|| TILETYPE_CURVE_TWO_BOTTONLEFT_TOPRIGHT == type)
		{
			exitDirection = DIRECTION_BOTTON;
		}
		else if ( TILETYPE_LINE_ONE_HORIZONTAL  == type || TILETYPE_LINE_TWO_VERTICALONTOP  == type
			|| TILETYPE_LINE_TWO_HORIZONTALONTOP  == type)
		{
			exitDirection = DIRECTION_RIGHT;
		}
	}
	else if (DIRECTION_BOTTON == direction)
	{
		if ( TILETYPE_CURVE_ONE_BOTTONLEFT == type || TILETYPE_CURVE_TWO_BOTTONLEFT_TOPRIGHT == type)
		{
			exitDirection = DIRECTION_LEFT;
		}
		else if ( TILETYPE_CURVE_ONE_BOTTONRIGHT == type|| TILETYPE_CURVE_TWO_TOPLEFT_BOTTONRIGHT == type)
		{
			exitDirection = DIRECTION_RIGHT;
		}
		else if ( TILETYPE_LINE_ONE_VERTICAL  == type || TILETYPE_LINE_TWO_VERTICALONTOP  == type
			|| TILETYPE_LINE_TWO_HORIZONTALONTOP  == type)
		{
			exitDirection = DIRECTION_TOP;
		}
	}
	else if (DIRECTION_RIGHT == direction)
	{
		if ( TILETYPE_CURVE_ONE_TOPRIGHT == type || TILETYPE_CURVE_TWO_BOTTONLEFT_TOPRIGHT == type)
		{
			exitDirection = DIRECTION_TOP;
		}
		else if ( TILETYPE_CURVE_ONE_BOTTONRIGHT == type|| TILETYPE_CURVE_TWO_TOPLEFT_BOTTONRIGHT == type)
		{
			exitDirection = DIRECTION_BOTTON;
		}
		else if ( TILETYPE_LINE_ONE_HORIZONTAL  == type || TILETYPE_LINE_TWO_VERTICALONTOP  == type
			|| TILETYPE_LINE_TWO_HORIZONTALONTOP  == type)
		{
			exitDirection = DIRECTION_LEFT;
		}
	}

	return exitDirection;
}

void CCTubeGridTile::endErasing()
{
	if (m_erasingChild)
	{
		removeChild(m_erasingChild, FALSE);
		
		CCNotificationCenter::sharedNotificationCenter()->postNotification(MY_NOTIFICATION_NEXT_TILE,NULL);
	}
}

int CCTubeGridTile::getEndErasingDirection()
{
	return m_endErasingDirection;
}

bool CCTubeGridTile::getIsReversedErasing(int direction, int childTag)
{
	//originally, line is from top to bottom (0 to height), curve is (-90 to 0);
	bool isReversedErasing;
	if (DIRECTION_TOP == direction)
	{
		if ( childTag == TAG_CURVE_TOPLEFT)
		{
			isReversedErasing = true;
		}
		else 
		{
			isReversedErasing = FALSE;
		}
	}
	else if (DIRECTION_LEFT == direction)
	{
		if ( childTag == TAG_CURVE_BOTTONLEFT)
		{
			isReversedErasing = true;
		}
		else 
		{
			isReversedErasing = FALSE;
		}
	}
	else if (DIRECTION_BOTTON == direction)
	{
		if ( childTag == TAG_CURVE_BOTTONRIGHT || childTag == TAG_LINE_VERTICAL)
		{
			isReversedErasing = true;
		}
		else 
		{
			isReversedErasing = FALSE;
		}
	}
	else if (DIRECTION_RIGHT == direction)
	{
		if ( childTag == TAG_CURVE_TOPRIGHT || childTag == TAG_LINE_HORIZONTAL)
		{
			isReversedErasing = true;
		}
		else 
		{
			isReversedErasing = FALSE;
		}
	}

	return isReversedErasing;
}


ChildTubeSprite* ChildTubeSprite::createWithTexture(CCTexture2D *pTexture)
{
    ChildTubeSprite *pobSprite = new ChildTubeSprite();
    if (pobSprite && pobSprite->initWithTexture(pTexture))
    {
        pobSprite->autorelease();
        return pobSprite;
    }
    CC_SAFE_DELETE(pobSprite);
    return NULL;
}

cocos2d::CCPoint CCTubeGridTile::getFireLoctionWorld()
{
	CCPoint firepos;
	if (m_currentFireLoctionPoint.x==0 && m_currentFireLoctionPoint.y==0)
	{
		firepos = CCPointZero;
	}
	else 
		firepos =  m_erasingChild->convertToWorldSpace(m_currentFireLoctionPoint);
	return firepos;
}

TELEPORT_INFO CCTubeGridTile::getTeleportInfo()
{
    return m_teleportInfo;
}
void CCTubeGridTile::setTeleportInfo(TELEPORT_INFO info)
{
    m_teleportInfo = info;
}


////////////////////////////
//TileCache
////////////////////////////
TileCache * TileCache::m_cache = NULL;

CCSprite * TileCache :: getLineByColor(TileColor color)
{
    std::map<int,CCSprite*>::iterator val = m_linebuffer.find(color);
    if (val != m_linebuffer.end())
    {
        return val->second;
    }
    else
    {
        CCSprite * pSprite;
        if (color == TILECOLOR_RED) {
            pSprite = CCSprite::createWithTexture(CCTextureCache::sharedTextureCache()->textureForKey("tube_line_red.png"));
        }
        else if (color == TILECOLOR_GREEN) {
             pSprite = CCSprite::createWithTexture(CCTextureCache::sharedTextureCache()->textureForKey("tube_line_green.png"));
        }
        else if (color == TILECOLOR_BLUE) {
             pSprite = CCSprite::createWithTexture(CCTextureCache::sharedTextureCache()->textureForKey("tube_line_blue.png"));
        }
        else if (color == TILECOLOR_BLACK) {
             pSprite = CCSprite::createWithTexture(CCTextureCache::sharedTextureCache()->textureForKey("tube_line_black.png"));
        }
        else if (color == TILECOLOR_YELLOW) {
             pSprite = CCSprite::createWithTexture(CCTextureCache::sharedTextureCache()->textureForKey("tube_line_yellow.png"));
        }
        else if (color == TILECOLOR_PURPLE) {
             pSprite = CCSprite::createWithTexture(CCTextureCache::sharedTextureCache()->textureForKey("tube_line_purple.png"));
        }
        pSprite->retain();
        m_linebuffer.insert(std::map<int,CCSprite*>::value_type(color, pSprite));
        return pSprite;
    }
}

CCSprite * TileCache :: getCurveByColor(TileColor color)
{
    std::map<int,CCSprite*>::iterator val = m_curvebuffer.find(color);
    if (val != m_curvebuffer.end())
    {
        return val->second;
    }
    else
    {
        CCSprite * pSprite;
        if (color == TILECOLOR_RED) {
            pSprite = CCSprite::createWithTexture(CCTextureCache::sharedTextureCache()->textureForKey("tube_curve_red.png"));
        }
        else if (color == TILECOLOR_GREEN) {
            pSprite = CCSprite::createWithTexture(CCTextureCache::sharedTextureCache()->textureForKey("tube_curve_green.png"));
        }
        else if (color == TILECOLOR_BLUE) {
            pSprite = CCSprite::createWithTexture(CCTextureCache::sharedTextureCache()->textureForKey("tube_curve_blue.png"));
        }
        else if (color == TILECOLOR_BLACK) {
            pSprite = CCSprite::createWithTexture(CCTextureCache::sharedTextureCache()->textureForKey("tube_curve_black.png"));
        }
        else if (color == TILECOLOR_YELLOW) {
            pSprite = CCSprite::createWithTexture(CCTextureCache::sharedTextureCache()->textureForKey("tube_curve_yellow.png"));
        }
        else if (color == TILECOLOR_PURPLE) {
            pSprite = CCSprite::createWithTexture(CCTextureCache::sharedTextureCache()->textureForKey("tube_curve_purple.png"));
        }
        pSprite->retain();
        m_curvebuffer.insert(std::map<int,CCSprite*>::value_type(color, pSprite));
        return pSprite;
    }
}


