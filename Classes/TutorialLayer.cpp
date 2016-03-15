#include "TutorialLayer.h"
#include "MyMapTile.h"
#include "LevelSelectScene.h"
#include "UserData.h"

#define ACTIONTAG_FLASH 5

CCScene* TutorialLayer::scene()
{
    CCScene *scene = CCScene::create();
	TutorialLayer *layer = TutorialLayer::create();
	scene->addChild(layer);
    
	return scene;
}

const static int TILE_COUNT = 15;
const static int TUTORIAL_TILES[TILE_COUNT][3] = {
	{TILETYPE_CURVE_ONE_TOPRIGHT, 0, 0},{TILETYPE_LINE_ONE_VERTICAL, 0, 1},{TILETYPE_LINE_ONE_VERTICAL, 0, 2},{TILETYPE_CURVE_ONE_BOTTONRIGHT, 0, 3},
	{TILETYPE_LINE_ONE_HORIZONTAL, 1, 0},{TILETYPE_NOTUBE, 1, 1},{TILETYPE_LINE_ONE_HORIZONTAL, 1, 2},
	{TILETYPE_LINE_ONE_HORIZONTAL, 2, 0},{TILETYPE_NOTUBE, 2, 1},{TILETYPE_NOTUBE, 2, 2},{TILETYPE_LINE_ONE_HORIZONTAL, 2, 3},
    {TILETYPE_CURVE_ONE_TOPLEFT, 3, 0},{TILETYPE_LINE_ONE_VERTICAL, 3, 1},{TILETYPE_LINE_ONE_VERTICAL, 3, 2},{TILETYPE_CURVE_ONE_BOTTONLEFT, 3, 3}
};

TutorialLayer::TutorialLayer()
{
	m_currentStep = 0;
}

bool TutorialLayer::init()
{
	//draw background
	CCSize size = CCDirector::sharedDirector()->getWinSize();

	m_zeroPoint = ccp(size.width/2 - tileWidth - tileWidth/2, size.height/2 - tileHeight - tileHeight/2);

	CCRenderTexture * pBack = CCRenderTexture::create(size.width,size.height);
	pBack->begin();
	//background is all white
	ccDrawSolidRect(ccp(0,0),ccp(size.width,size.height),ccc4f(255,255,255,1));
	//background beneath tiles is all black
	ccDrawSolidRect(ccp(size.width/2 - tileWidth * 2,size.height/2 - tileHeight * 2),ccp(size.width/2 + tileWidth * 2,size.height/2 + tileHeight * 2),ccc4f(0,0,0,1));
	pBack->end();
	CCSprite * pSpriteBack = CCSprite::createWithTexture(pBack->getSprite()->getTexture());
	pSpriteBack->setPosition(ccp(size.width/2, size.height/2));
	addChild(pSpriteBack);

	for (int i=0;i<TILE_COUNT;i++)
	{
		CCTubeGridTile * tile = CCTubeGridTile::createWithType(TUTORIAL_TILES[i][0]);
		if (i == 6)
		{
			m_tutorTile1 = tile;
		}
		if (i == 5)
		{
			m_tutorTile2 = tile;
		}
		putTile(tile,TUTORIAL_TILES[i][1],TUTORIAL_TILES[i][2]);
	}

    m_nextLabel = CCLabelTTF::create("Tutorial", "Arial", 25);
    m_nextLabel->setColor(ccBLACK);
    m_nextLabel->setPosition(ccp(size.width/2, 50));
    addChild(m_nextLabel);
    
    //m_tutorTile1->runAction(CCRepeatForever::create(CCSequence::create(CCFadeTo::create(0.5, 155),
                                                                      //CCFadeTo::create(0.5, 255),NULL)));
    
    m_tutorTile1->runAction(flashAction());
    
    m_tips = CCLabelTTF::create("Tap the flashing tile to move", "Arial", 25);
    m_tips->setColor(ccBLACK);
    m_tips->setPosition(ccp(size.width/2, 280));
    addChild(m_tips);
    
	setTouchEnabled(true);
    
    return true;
}

void TutorialLayer::registerWithTouchDispatcher()
{
	CCDirector::sharedDirector()->getTouchDispatcher()->addTargetedDelegate(this, 0, true);
}

bool TutorialLayer::ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent)
{
	CCPoint touchpoint = pTouch->getLocationInView();
	touchpoint = CCDirector::sharedDirector()->convertToGL(touchpoint);
	switch (m_currentStep)
	{
	case 0:
		{
			//display 1 tile run together

			CCRect tutorTileRect = CCRectMake(m_tutorTile1->getPosition().x - tileWidth/2, m_tutorTile1->getPosition().y - tileHeight/2, tileWidth,tileHeight);
			if (tutorTileRect.containsPoint(touchpoint))
			{
				setTouchEnabled(false);
                m_tutorTile1->stopActionByTag(ACTIONTAG_FLASH);
                m_tutorTile1->setOpacity(255);
				m_tutorTile1->runAction(CCSequence::create(CCMoveBy::create(0.6,ccp(0,tileHeight)),
                                                           CCDelayTime::create(0.5),
					CCCallFunc::create(this, callfunc_selector(TutorialLayer::tutor1Callback)),NULL));
					//CCMoveBy::create(0.6,ccp(0,tileHeight)));
                m_currentStep++;
			}

			
			break;
		}
	case 1:
		{
			//display 2 run
            CCRect tutorTileRect = CCRectMake(m_tutorTile2->getPosition().x - tileWidth/2, m_tutorTile2->getPosition().y - tileHeight/2, tileWidth,tileHeight);
			if (tutorTileRect.containsPoint(touchpoint))
			{
				setTouchEnabled(false);
                m_tutorTile1->runAction(CCMoveBy::create(0.6,ccp(0,tileHeight)));
                
                m_tutorTile2->stopActionByTag(ACTIONTAG_FLASH);
                m_tutorTile2->setOpacity(255);
				m_tutorTile2->runAction(CCSequence::create(CCMoveBy::create(0.6,ccp(0,tileHeight)),
                                                           CCDelayTime::create(0.5),
                                                           CCCallFunc::create(this, callfunc_selector(TutorialLayer::tutor2Callback)),NULL));
                
                m_currentStep++;
			}
			//change tip
			break;
		}
	case 2:
		{
			//run with fire
            CCRect tutorTileRect = CCRectMake(m_tutorTile1->getPosition().x - tileWidth/2, m_tutorTile1->getPosition().y - tileHeight/2, tileWidth,tileHeight);
			if (tutorTileRect.containsPoint(touchpoint))
			{
				setTouchEnabled(false);
                m_pFire->runAction(CCMoveBy::create(0.6,ccp(0,tileHeight)));
				m_tutorTile1->runAction(CCSequence::create(CCMoveBy::create(0.6,ccp(0,tileHeight)),
                                                           CCCallFunc::create(this, callfunc_selector(TutorialLayer::tutor3Callback)),NULL));
                m_currentStep++;
			}
            break;
		}
        case 3:
        {
            CCUserDefault::sharedUserDefault()->setBoolForKey(KEY_HASSEEMTUTORIALS, true);
            CCDirector::sharedDirector()->replaceScene(CCTransitionFade::create(0.5, LevelSelector::scene()));
            break;
        }
	default:
		break;
	}
}

void TutorialLayer::tutor1Callback()
{
	m_tips->setString("Good. now tap the flashing tile \n to move 2 tiles together");
    m_tutorTile1->setPosition(ccpAdd(m_tutorTile1->getPosition(), ccp(0, -tileHeight)));
    m_tutorTile2->runAction(flashAction());
    setTouchEnabled(true);
}

void TutorialLayer::tutor2Callback()
{
	m_tips->setString("Very good,\nnow tap the tile with fire to move");
    
    m_tutorTile1->setPosition(ccpAdd(m_tutorTile1->getPosition(), ccp(0, -tileHeight)));
    m_pFire= CCParticleFire::create();
    m_pFire->setPosition(ccpAdd(m_tutorTile1->getPosition(), ccp(tileWidth/2,0)));
    m_pFire->setScale(0.18);
    addChild(m_pFire,1);
    
    m_tutorTile2->setPosition(ccpAdd(m_tutorTile2->getPosition(), ccp(0, -tileHeight)));
    
    setTouchEnabled(true);
}

void TutorialLayer::tutor3Callback()
{
	m_tips->setString("Excellent. the tutorial is finished, \ntap to start game");
    setTouchEnabled(true);
}

void TutorialLayer::putTile(CCTubeGridTile * pTile, int x, int y)
{
	CCPoint pos = ccp(m_zeroPoint.x + tileWidth * x, m_zeroPoint.y + tileHeight * y);
	pTile->setPosition(pos);
	addChild(pTile);
}

CCAction * TutorialLayer::flashAction()
{
    CCAction * ret = CCRepeatForever::create(CCSequence::create(CCFadeTo::create(0.5, 155),
                                                                CCFadeTo::create(0.5, 255),NULL));
    ret->setTag(ACTIONTAG_FLASH);
    return ret;
}
