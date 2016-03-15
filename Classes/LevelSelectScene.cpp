#include "LevelSelectScene.h"
#include "VisibleRect.h"
#include "UserData.h"
#include "PuzzleTool.h"
#include "CLabel.h"
#include "AppMacros.h"

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
#include "InlineAppstore.h"
#endif
#include <string>

 ViewPosition * LevelSelector::m_viewposition = NULL;
cocos2d::CCScene* LevelSelector::scene()
{
	// 'scene' is an autorelease object
	CCScene *scene = CCScene::create();

	// 'layer' is an autorelease object
	LevelSelector *layer = LevelSelector::create();
    
    CCSize winsize = CCDirector::sharedDirector()->getVisibleSize();
    //header
    LevelSelectorHeader*pHeader = LevelSelectorHeader::create(0);
	pHeader->setAnchorPoint(CCPointZero);
	pHeader->setPosition(ccp(0,winsize.height-pHeader->getContentSize().height));
    
	CCScrollView*scrollView = CCScrollView::create();
	scrollView->setViewSize(CCSizeMake(winsize.width,winsize.height-pHeader->getContentSize().height));
	//scrollView->setContentOffset(ccp(10,200),true);
	scrollView->setContainer(layer);
	scrollView->setDirection(kCCScrollViewDirectionVertical);
    
    pHeader->setScrollView(scrollView);
    
	// add layer as a child to scene
    scene->addChild(pHeader);
	scene->addChild(scrollView);
	
    //some initialization of resource
    PuzzleGame::getGameDataFromFile();
    UserData::sharedUserData();
    
	// return the scene
	return scene;
}
LevelSelectorHeader*LevelSelectorHeader::create(int length)
{
    LevelSelectorHeader * pRet = new LevelSelectorHeader();
    if (pRet->initWithLength(length)) {
        pRet->autorelease();
        return pRet;
    }
    
    CC_SAFE_DELETE(pRet);
    return NULL;
}

bool LevelSelectorHeader::initWithLength(int length)
{
    if ( !CCLayer::init() )
	{
		return false;
	}
    CCSprite*background = getFixedSizeSprite("header_withString.png");
	background->setAnchorPoint(CCPointZero);
	background->setPosition(CCDirector::sharedDirector()->getVisibleOrigin());
    background->setScaleX(CCDirector::sharedDirector()->getVisibleSize().width/background->getContentSize().width);
    
    setContentSize(CCSizeMake(CCDirector::sharedDirector()->getWinSize().width,background->getContentSize().height));
	addChild(background);
    
    /*CCSprite * pCreate = getFixedSizeSprite("create.png");
     CCMenuItemSprite*menuItemCreate = CCMenuItemSprite::create(pCreate,pCreate,
     pRet,menu_selector(LevelSelector::menuCreateButtonback));
     menuItemCreate->setAnchorPoint(CCPointZero);
     CCMenu* pMenu = CCMenu::create(menuItemCreate,NULL);
     pMenu->setAnchorPoint(CCPointZero);
     pMenu->setPosition(ccp(pRet->getContentSize().width *3/4,2));
     pRet->addChild(pMenu);*/
    
    //scroll button
    CCSprite * pScrollDown = getFixedSizeSprite("scrolldown.png");
	CCMenuItemSprite*menuItemScrollDown = CCMenuItemSprite::create(pScrollDown,pScrollDown,
                                                                   this,menu_selector(LevelSelectorHeader::menuScrollDownButtonCallback));
    
    CCSprite * pScrollUp = getFixedSizeSprite("scrollup.png");
    CCMenuItemSprite*menuItemScrollUp = CCMenuItemSprite::create(pScrollUp,pScrollUp,this,menu_selector(LevelSelectorHeader::menuScrollUpButtonCallback));
    menuItemScrollUp->setPosition(ccpAdd(menuItemScrollDown->getPosition(), ccp(menuItemScrollDown->getContentSize().width + 20, 0)));
    
	CCMenu* pMenu = CCMenu::create(menuItemScrollDown,menuItemScrollUp,NULL);
	pMenu->setAnchorPoint(CCPointZero);
	pMenu->setPosition(ccp(getContentSize().width *3/4 - 10,menuItemScrollDown->getContentSize().height/2 + 5));
	addChild(pMenu);
    
    return true;
}

#define kScrollSpedd 2000

void LevelSelectorHeader::menuScrollDownButtonCallback(CCObject*pSender)
{
    CCPoint currentOffset =  m_scrollView->getContentOffset();
    CCPoint newOffset = ccpAdd(currentOffset, ccp(0, 1000));
    CCPoint maxOffset = m_scrollView->maxContainerOffset();
    
    if (newOffset.y > maxOffset.y) {
        newOffset.y = maxOffset.y;
    }
    
    float time = (newOffset.y - currentOffset.y)/kScrollSpedd;
    m_scrollView->setContentOffsetInDuration(newOffset, time);
}

void LevelSelectorHeader::menuScrollUpButtonCallback(CCObject*pSender)
{
    CCPoint currentOffset =  m_scrollView->getContentOffset();
    CCPoint newOffset = ccpAdd(currentOffset, ccp(0, -1000));
    CCPoint minOffset = m_scrollView->minContainerOffset();
    
    if (newOffset.y < minOffset.y) {
        newOffset.y = minOffset.y;
    }
    
    float time = (currentOffset.y - newOffset.y)/kScrollSpedd;
    m_scrollView->setContentOffsetInDuration(newOffset, time);
}

////////////////////////////////
//////LevelSelector class///////
////////////////////////////////

ScMenuItem* LevelSelector::createScMenuItem (int level, CCPoint position)
{
    char filename[32];
    char strlevel[16];
    
    sprintf(strlevel, "%d",level);
    sprintf(filename,"thumb%d.png",level);
    
	CCSprite*menuitemSprite;
	ScMenuItem*menuItemLevel;
    
    float width = 145;
    float height = 83;
   if (level <= m_unlockedLevelMax)
   {
	   menuitemSprite = getFixedSizeSprite(filename,width,height);
	   menuItemLevel = ScMenuItem::create(menuitemSprite,menuitemSprite,this,menu_selector(LevelSelector::menuLevelTapOkCallback));
   }
   else
   {
	   menuitemSprite = getFixedSizeSprite("level_locked.png",width,height);
	   menuItemLevel = ScMenuItem::create(menuitemSprite,menuitemSprite,this,menu_selector(LevelSelector::menuLevelTapLockedCallback));
   }
    
    menuItemLevel->setLevel(level);
    menuItemLevel->setAnchorPoint(CCPointZero);
    menuItemLevel->setPosition(position);

    CCLabelTTF * pLabelLevel = CCLabelTTF::create(strlevel, "Arial", 20);
    pLabelLevel->setPosition(ccp(menuItemLevel->getContentSize().width/2, -12));
    menuItemLevel->addChild(pLabelLevel);
    
    return menuItemLevel;
}

bool LevelSelector::init()
{	
	m_unlockedLevelMax = UserData::sharedUserData()->getUnlockedLevel();
	float heightPerLine = 130;
	
	int nItemPerLine = 2;
    int nLineCount = LEVELCOUNT/nItemPerLine;

	int tempLvl = nLineCount*nItemPerLine;
	CCPoint menuPoint = CCPointZero;
    
#define ITEMPOS_LEFT_X 70
#define ITEMPOS_RIGHT_X 270
#define ITEMPOS_Y 50

#if BUILD_VERSION == VERSION_LITE
	CCSprite*pSpGetMore = getFixedSizeSprite("getMore.png");
	float levelItemWidth = getFixedSizeSprite("thumb_temp.png")->getContentSize().width;
	float position_x = (ITEMPOS_RIGHT_X + levelItemWidth + ITEMPOS_LEFT_X)/2 - pSpGetMore->getContentSize().width/2;
	
	CCMenuItemSprite * pItemGetMore = CCMenuItemSprite::create(pSpGetMore,pSpGetMore,this,menu_selector(LevelSelector::menuTapGetMoreCallback));
	pItemGetMore->setAnchorPoint(CCPointZero);
	pItemGetMore->setPosition(ccp(position_x,ITEMPOS_Y));
	ScMenu*pGetmoreMenu = ScMenu::create(pItemGetMore,NULL);
	pGetmoreMenu->setPosition(menuPoint);
	addChild(pGetmoreMenu);
	menuPoint = ccpAdd(menuPoint,ccp(0,heightPerLine));
#endif 

	for (int i=0;i<nLineCount;i++)
	{
        ScMenuItem*menuItemLevel = createScMenuItem(tempLvl - 1, ccp(ITEMPOS_LEFT_X,ITEMPOS_Y));
        ScMenuItem*menuItemLevel2 = createScMenuItem(tempLvl, ccp(ITEMPOS_RIGHT_X,ITEMPOS_Y));
		tempLvl -= 2;

		ScMenu*levelMenu = ScMenu::create(menuItemLevel,menuItemLevel2);

		levelMenu->setPosition(menuPoint);
		menuPoint = ccpAdd(menuPoint,ccp(0,heightPerLine));
		addChild(levelMenu);
	}
	
#if BUILD_VERSION == VERSION_LITE
    float listContentHeight = heightPerLine*(nLineCount+2);
#else
    float listContentHeight = heightPerLine*(nLineCount+1);
#endif

	setContentSize(CCSizeMake(CCDirector::sharedDirector()->getWinSize().width,listContentHeight));
	setAnchorPoint(CCPointZero);
    
    //set the line position
    if (m_viewposition == NULL) {
        m_viewposition = new ViewPosition();
        m_viewposition->value = heightPerLine/2-heightPerLine*(nLineCount-1);
    }
	setPosition(ccp(0,m_viewposition->value));
	return true;
}

void LevelSelector::menuLevelTapOkCallback(CCObject* pSender)
{
	ScMenuItem*item = dynamic_cast<ScMenuItem*>(pSender);
	if (item)
	{
        m_viewposition->value = getPosition().y;
        
        CCCallFuncO* act2 = CCCallFuncO::create(this, callfuncO_selector(LevelSelector::menuLevelTapOkCallback2),item) ;
        
        //action to perform before replace scene
        CCDelayTime * pDelay = CCDelayTime::create(0.02);
//        CCSprite * image =  (CCSprite*)item->getNormalImage();
//        item->setNormalImage(getResizedSrite_withOriginalRect(image, 0.95));
        item->setOpacity(180);
        
        CCSequence*seq = CCSequence::create(pDelay,act2,NULL);
		item->runAction(seq);
	}
}
void LevelSelector::menuLevelTapOkCallback2(CCObject* pSender)
{
    ScMenuItem*item = dynamic_cast<ScMenuItem*>(pSender);
    CCScene *pScene = PuzzleGame::scene(item->getLevel());
    CCDirector::sharedDirector()->replaceScene(CCTransitionFade::create(0.3, pScene));
}

void LevelSelector::menuLevelTapLockedCallback(CCObject* pSender)
{
	//CCMessageBox("this level is locked, please complete lower level to unlock this level","Level Locked");
}

void LevelSelector::menuTapGetMoreCallback(CCObject* pSender)
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    CCAppStore::sharedInstance()->ShowInAppstore("???");
#endif
}

void LevelSelector::menuCreateButtonback(CCObject*pSender)
{
	CCScene *pScene = PuzzleCreator::scene();
	CCDirector::sharedDirector()->replaceScene(pScene);
}

ScMenu * ScMenu::create(CCMenuItem* item, CCMenuItem* item2)
{
	ScMenu *pRet = new ScMenu();
	CCArray*pArrayOfItems = CCArray::create(item,item2,NULL);
	if (pRet && pRet->initWithArray(pArrayOfItems))
	{
		pRet->autorelease();
	}
	else
	{
		CC_SAFE_DELETE(pRet);
	}

	return pRet;
}

bool ScMenu::ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent)
{
	CCRect rect = VisibleRect::getVisibleRect();
	if (rect.size.width && rect.size.height)
	{
		if (!VisibleRect::getVisibleRect().containsPoint(pTouch->getLocation()))
		{
			return false;
		}
	}
	move_ = false;
	return CCMenu::ccTouchBegan(pTouch,pEvent);
}

void ScMenu::ccTouchMoved(CCTouch *pTouch, CCEvent *pEvent)
{
	move_ = true;
	return CCMenu::ccTouchMoved(pTouch,pEvent);
}

void ScMenu::ccTouchEnded(CCTouch *pTouch, CCEvent *pEvent)
{
	if (!move_)
	{
		return CCMenu::ccTouchEnded(pTouch,pEvent);
	}
	else
	{
		m_eState = kCCMenuStateWaiting;
	}
}

ScMenuItem* ScMenuItem::create(CCNode* normalSprite, CCNode* selectedSprite, CCObject* target, SEL_MenuHandler selector)
{
	ScMenuItem*pRet = new ScMenuItem();
	if (pRet->initWithNormalSprite(normalSprite, selectedSprite,NULL, target, selector))
	{
		pRet->autorelease();
	}
	else
	{
		CC_SAFE_DELETE(pRet);
	}

	return pRet;
}

