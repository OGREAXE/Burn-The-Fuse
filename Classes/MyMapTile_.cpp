#include "MyMapTile.h"
#include "math.h"

CCTubeGridTile::CCTubeGridTile(void)
{
	m_isErasing = FALSE;
	m_curveRadius = tileWidth/2;
	m_eraserRadius = 5.5;

	m_fire = CCFireSprite::create();
	m_fire->setVisible(FALSE);
	m_fire->retain();
}


CCTubeGridTile::~CCTubeGridTile(void)
{
}

static void doClip_unrotated_bottomLeft(CCSprite*podChild,float clipx,float clipy);
static void doClip_unrotated_topright(CCSprite*podChild,float clipx,float clipy);
CCTubeGridTile* CCTubeGridTile::createWithType(int type)
{
	CCRenderTexture* pRt = CCRenderTexture::create(tileWidth,tileHeight);
	pRt->begin();
	ccColor4F colorBackground = {0,0,0
		,1.0f};
	ccDrawSolidRect(ccp(0,0),ccp(tileWidth, tileHeight),colorBackground);
	ccColor4F colorBackground2 = {0.9f,0.9f,0.9f,1.0f};
	float flineWidth = 0.4;
	ccDrawSolidRect(ccp(flineWidth,flineWidth),ccp(tileWidth-flineWidth, tileHeight-flineWidth),colorBackground2);
	pRt->end();
	CCTubeGridTile * pobSprite = new CCTubeGridTile();
	if (pobSprite->initWithTexture(pRt->getSprite()->getTexture()))
	{
		pobSprite->autorelease();

	}

	//create the void tile render texture
	pobSprite->m_rtVoid = CCRenderTexture::create(tileWidth,tileHeight);
	pobSprite->m_rtVoid->begin();
	ccColor4F colorVoid = {0,0,0
		,1.0f};
	ccDrawSolidRect(ccp(0,0),ccp(tileWidth, tileHeight),colorVoid);
	pobSprite->m_rtVoid->end();

	//create the curve render texture, originally top-left
	pobSprite->m_rtCurve = CCRenderTexture::create(tileWidth,tileHeight);
	pobSprite->m_rtCurve->begin();
	CCSprite*pSpCurve = CCTubeGridTile::createSingleCurve();
	pSpCurve->setPosition(ccp(tileWidth/2,tileHeight/2));
	pSpCurve->visit();
	pobSprite->m_rtCurve->end();

	//create the line render texture, originally vertical
	pobSprite->m_rtLine = CCRenderTexture::create(tileWidth,tileHeight);
	pobSprite->m_rtLine->begin();
	CCSprite*pSpLine = CCTubeGridTile::createSingleLine();
	pSpLine->setPosition(ccp(tileWidth/2,tileHeight/2));
	pSpLine->visit();
	pobSprite->m_rtLine->end();

	//do image initialization with type
	switch (type)
	{
	case TILETYPE_VOID:
		{
			pobSprite->setTexture(pobSprite->m_rtVoid->getSprite()->getTexture());
			break;
		}
	case TILETYPE_NOTUBE:
		{
			break;
		}
	case TILETYPE_CURVE_ONE_BOTTONLEFT:
		{
			CCSprite* podChild1 = CCSprite::createWithTexture(pobSprite->m_rtCurve->getSprite()->getTexture());
			podChild1->setPosition(ccp(tileWidth/2,tileHeight/2));
			podChild1->setRotation(-90);
			podChild1->setTag(TAG_CURVE_BOTTONLEFT);
			pobSprite->addChild(podChild1,Z_CHILD1);
			break;
		}
	case TILETYPE_CURVE_ONE_BOTTONRIGHT:
		{
			CCSprite* podChild1 = CCSprite::createWithTexture(pobSprite->m_rtCurve->getSprite()->getTexture());
			podChild1->setPosition(ccp(tileWidth/2,tileHeight/2));
			podChild1->setRotation(-180);
			podChild1->setTag(TAG_CURVE_BOTTONRIGHT);
			pobSprite->addChild(podChild1,Z_CHILD1);
			break;
		}
	case TILETYPE_CURVE_ONE_TOPLEFT:
		{
			CCSprite* podChild1 = CCSprite::createWithTexture(pobSprite->m_rtCurve->getSprite()->getTexture());
			podChild1->setPosition(ccp(tileWidth/2,tileHeight/2));
			podChild1->setTag(TAG_CURVE_TOPLEFT);
			pobSprite->addChild(podChild1,Z_CHILD1);
			break;
		}
	case TILETYPE_CURVE_ONE_TOPRIGHT:
		{
			CCSprite* podChild1 = CCSprite::createWithTexture(pobSprite->m_rtCurve->getSprite()->getTexture());
			podChild1->setPosition(ccp(tileWidth/2,tileHeight/2));
			podChild1->setRotation(90);
			podChild1->setTag(TAG_CURVE_TOPRIGHT);
			pobSprite->addChild(podChild1,Z_CHILD1);
			break;
		}
	case TILETYPE_CURVE_TWO_TOPLEFT_BOTTONRIGHT:
		{
			CCSprite* podChild1 = CCSprite::createWithTexture(pobSprite->m_rtCurve->getSprite()->getTexture());
			podChild1->setPosition(ccp(tileWidth/2,tileHeight/2));

			podChild1->setTag(TAG_CURVE_TOPLEFT);
			pobSprite->addChild(podChild1,Z_CHILD1);

			CCSprite* podChild2 = CCSprite::createWithTexture(pobSprite->m_rtCurve->getSprite()->getTexture());
			podChild2->setPosition(ccp(tileWidth/2,tileHeight/2));
			podChild2->setRotation(180);
			podChild2->setTag(TAG_CURVE_BOTTONRIGHT);
			pobSprite->addChild(podChild2,Z_CHILD2);

			break;
		}
	case TILETYPE_CURVE_TWO_BOTTONLEFT_TOPRIGHT:
		{
			CCSprite* podChild1 = CCSprite::createWithTexture(pobSprite->m_rtCurve->getSprite()->getTexture());
			podChild1->setPosition(ccp(tileWidth/2,tileHeight/2));
			podChild1->setRotation(-90);
			podChild1->setTag(TAG_CURVE_BOTTONLEFT);
			pobSprite->addChild(podChild1,Z_CHILD1);

			CCSprite* podChild2 = CCSprite::createWithTexture(pobSprite->m_rtCurve->getSprite()->getTexture());
			podChild2->setPosition(ccp(tileWidth/2,tileHeight/2));
			podChild2->setRotation(90);
			podChild2->setTag(TAG_CURVE_TOPRIGHT);
				
			pobSprite->addChild(podChild2,Z_CHILD2);

			break;
		}
	case TILETYPE_LINE_ONE_HORIZONTAL:
		{
			CCSprite* podChild1 = CCSprite::createWithTexture(pobSprite->m_rtLine->getSprite()->getTexture());
			podChild1->setPosition(ccp(tileWidth/2,tileHeight/2));
			podChild1->setRotation(-90);
			podChild1->setTag(TAG_LINE_HORIZONTAL);
			pobSprite->addChild(podChild1,Z_CHILD1);
			break;
		}
	case TILETYPE_LINE_ONE_VERTICAL:
		{
			CCSprite* podChild1 = CCSprite::createWithTexture(pobSprite->m_rtLine->getSprite()->getTexture());
			podChild1->setPosition(ccp(tileWidth/2,tileHeight/2));
			podChild1->setTag(TAG_LINE_VERTICAL);
			pobSprite->addChild(podChild1,Z_CHILD1);
			break;
		}
	case TILETYPE_LINE_TWO_HORIZONTALONTOP:
		{
			CCSprite* podChild1 = CCSprite::createWithTexture(pobSprite->m_rtLine->getSprite()->getTexture());
			podChild1->setPosition(ccp(tileWidth/2,tileHeight/2));
			podChild1->setTag(TAG_LINE_VERTICAL);
			pobSprite->addChild(podChild1,Z_CHILD1);

			CCSprite* podChild2 = CCSprite::createWithTexture(pobSprite->m_rtLine->getSprite()->getTexture());
			podChild2->setPosition(ccp(tileWidth/2,tileHeight/2));
			podChild2->setRotation(-90);
			podChild2->setTag(TAG_LINE_HORIZONTAL);
			pobSprite->addChild(podChild2,Z_CHILD2);
			break;
		}
	case TILETYPE_LINE_TWO_VERTICALONTOP:
		{
			CCSprite* podChild1 = CCSprite::createWithTexture(pobSprite->m_rtLine->getSprite()->getTexture());
			podChild1->setPosition(ccp(tileWidth/2,tileHeight/2));
			podChild1->setRotation(-90);
			podChild1->setTag(TAG_LINE_HORIZONTAL);
			pobSprite->addChild(podChild1,Z_CHILD1);

			CCSprite* podChild2 = CCSprite::createWithTexture(pobSprite->m_rtLine->getSprite()->getTexture());
			podChild2->setPosition(ccp(tileWidth/2,tileHeight/2));
			podChild2->setTag(TAG_LINE_VERTICAL);
			pobSprite->addChild(podChild2,Z_CHILD2);
			break;
		}
	default:
		break;
	}
	pobSprite->type = type;
	if (TILETYPE_LINE_ONE_HORIZONTAL == type||
		TILETYPE_LINE_ONE_VERTICAL == type||
		TILETYPE_LINE_TWO_HORIZONTALONTOP == type||
		TILETYPE_LINE_TWO_VERTICALONTOP == type)
	{
		pobSprite->genericType = GENERIC_TYPE_LINE;
	}
	else
	{
		pobSprite->genericType = GENERIC_TYPE_CURVE;
	}
	return pobSprite;
}
//CCTubeGridTile* CCTubeGridTile::createWithType(int type,int x, int y)
//{
//	CCTubeGridTile* podTile = CCTubeGridTile::createWithType(type);
//	podTile->m_x = x;
//	podTile->m_y = y;
//	return podTile;
//}

CCSprite* CCTubeGridTile::createSingleLine()
{
	CCSprite*podSprite = CCSprite::create("tube_line.png");
	return podSprite;
}

CCSprite* CCTubeGridTile::createSingleCurve()
{
	CCSprite*podSprite = CCSprite::create("tube_curve.png");
	return podSprite;
}

void CCTubeGridTile::beginErasing(float eraserSpeed, int direction)
{
	m_erasingChild = getChildForStartErasingAt(direction);
	if (NULL == m_erasingChild)
	{
		return;
	}
	float originalRotation = m_erasingChild->getRotation();
	m_erasingRenderTexture = CCRenderTexture::create(tileWidth,tileHeight);
	m_erasingRenderTexture->begin();
	m_erasingChild->setPosition(ccp(tileWidth/2,tileHeight/2));
	m_erasingChild->setRotation(0);
	m_erasingChild->setFlipY(TRUE);
	m_erasingChild->visit();
	m_erasingRenderTexture->end();

	m_erasingChild->setRotation(originalRotation);
	m_erasingChild->setFlipY(FALSE);
	m_erasingChild->setTexture(m_erasingRenderTexture->getSprite()->getTexture());
	m_erasingRenderTexture->retain();

	m_eraser = CCDrawNode::create();
	m_eraser->drawDot(ccp(0,0),m_eraserRadius,ccc4f(0,0,0,0));
	m_eraser->retain();
	ccBlendFunc blendFunc = {GL_ONE,GL_ZERO};
	m_eraser->setBlendFunc(blendFunc);

	m_isErasing = TRUE;
	m_eraserSpeed = eraserSpeed;
	m_startErasingDirection = direction;

	//start pos and angle should relate to direction	
	
	m_isReversedErasing = getIsReversedErasing(m_startErasingDirection,m_erasingChild->getTag());
	if (m_isReversedErasing)
	{
		//m_erasingPoint = ccp(tileWidth/2,tileHeight);
		//m_erasingAngle = M_PI/2;
		m_eraserCurveAngleUpperBound = M_PI/2 + m_eraserRadius/(tileWidth/2);
		m_eraserCurveAngleLowerBound =  (m_eraserRadius-m_eraserSpeed)/(tileWidth/2);
		m_eraserLineUpperBound = tileHeight + m_eraserRadius;
		m_eraserLineLowerBound = -(m_eraserSpeed - m_eraserRadius);
		m_erasingPoint = ccp(tileWidth/2,m_eraserLineUpperBound);
		m_erasingAngle = m_eraserCurveAngleUpperBound;
	}
	else
	{
		m_eraserCurveAngleUpperBound = M_PI/2-m_eraserRadius/(tileWidth/2);
		m_eraserCurveAngleLowerBound = - (m_eraserRadius-m_eraserSpeed)/(tileWidth/2);
		m_eraserLineUpperBound = tileHeight - m_eraserRadius;
		m_eraserLineLowerBound = m_eraserSpeed - m_eraserRadius;
		m_erasingPoint = ccp(tileWidth/2,m_eraserLineLowerBound);
		m_erasingAngle = m_eraserCurveAngleLowerBound;	
	}

	m_endErasingDirection = getExitOfEraserForStartErasingAt(m_startErasingDirection);
	schedule(schedule_selector(CCTubeGridTile::updateErasing),0.05f);

	m_erasingChild->addChild((CCSprite*)m_fire);
	m_fire->setRotation(-m_erasingChild->getRotation());
	m_fire->setScale(0.6);
	CCAnimate*fireAnimate = CCAnimate::create(CCAnimationCache::sharedAnimationCache()->animationByName("fire_animation"));
	m_fire->runAction(CCRepeatForever::create(fireAnimate));
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
	if (!m_fire->isVisible())
	{
		//m_fire->setVisible(TRUE);
	}
	m_currentFireLoctionPoint = ccp(x,tileHeight-y);
	m_fire->setPosition(m_currentFireLoctionPoint);
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
		unschedule(schedule_selector(CCTubeGridTile::updateErasing));
		endErasing();
		m_fire->setVisible(FALSE);
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

BOOL CCTubeGridTile::getIsReversedErasing(int direction, int childTag)
{
	//originally, line is from top to bottom (0 to height), curve is (-90 to 0);
	BOOL isReversedErasing;
	if (DIRECTION_TOP == direction)
	{
		if ( childTag == TAG_CURVE_TOPLEFT)
		{
			isReversedErasing = TRUE;
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
			isReversedErasing = TRUE;
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
			isReversedErasing = TRUE;
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
			isReversedErasing = TRUE;
		}
		else 
		{
			isReversedErasing = FALSE;
		}
	}

	return isReversedErasing;
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




CCFireSprite* CCFireSprite::create()
{
	CCFireSprite*podSprite = new CCFireSprite();
	if (podSprite->initWithFile("fire2.png"))
	{
		podSprite->autorelease();
		return podSprite;
	}
	
	return NULL;
}

