#include "Intro.h"
#include "LevelSelectScene.h"
#include "CLabel.h"
#include "PuzzleTool.h"
#include "TutorialLayer.h"
#include "UserData.h"

using namespace cocos2d;

#define STRATEGY_TAPTOSTART 1
#define STRATEGY_STARTAFTERLOADFINISHED 2
#define START_STRATEGY STRATEGY_STARTAFTERLOADFINISHED

cocos2d::CCScene* IntroLayer::scene()
{
	CCScene *scene = CCScene::create();
	IntroLayer *layer = IntroLayer::create();
	scene->addChild(layer);

	return scene;
}
void IntroLayer::registerWithTouchDispatcher()
{
	CCDirector::sharedDirector()->getTouchDispatcher()->addTargetedDelegate(this, 0, true);
}

bool IntroLayer::init()
{
	//CCSprite * pBack = getFixedSizeSprite("intro.png");//CCSprite::create("intro.png");
    m_pBack = getFixedSizeSprite("intro.png", 426, 320);
	CCPoint pos = CCDirector::sharedDirector()->getWinSize();
	m_pBack->setPosition(ccp(pos.x/2,pos.y/2));
	this->addChild(m_pBack, -1);
	this->setTouchEnabled(false);
    m_loadIndex = 0;
    return true;
}

void IntroLayer::onEnterTransitionDidFinish()
{
    m_loading = getFixedSizeSprite("loading.png");
    m_loading->setPosition(ccp(CCDirector::sharedDirector()->getWinSize().width/2,30));
    this->addChild(m_loading,-1);

    CCCallFunc* act2 = CCCallFunc::create(this, callfunc_selector(IntroLayer::delayCallback)) ;
    CCDelayTime * pDelay = CCDelayTime::create(0.1);
    CCSequence*seq = CCSequence::create(pDelay,act2,NULL);
    this->runAction(seq);
}

void IntroLayer::delayCallback(CCObject* pSender)
{
    schedule(schedule_selector(IntroLayer::updateProgress),0.05f);
}

//after progress bar run to 100%, delay some time to show next screen
void IntroLayer::delayShowFinishProgress(CCObject* pSender)
{
#if START_STRATEGY == STRATEGY_STARTAFTERLOADFINISHED
    bool hasSeemTutorial = CCUserDefault::sharedUserDefault()->getBoolForKey(KEY_HASSEEMTUTORIALS, false);
    if (hasSeemTutorial) {
        CCDirector::sharedDirector()->replaceScene(CCTransitionFade::create(0.5, LevelSelector::scene()));
    }
    else
    {
        CCDirector::sharedDirector()->replaceScene(CCTransitionFade::create(0.5, TutorialLayer::scene()));
    }
#else
    m_pBack = getFixedSizeSprite("intro.png", 426, 320);
    CCPoint pos = CCDirector::sharedDirector()->getWinSize();
    m_pBack->setPosition(ccp(pos.x/2,pos.y/2));
    this->addChild(m_pBack);
    
    CCSprite * pStart = getFixedSizeSprite("play.png");
    pStart->runAction(CCRepeatForever::create(CCBlink::create(1,1)));
    pStart->setPosition(ccp(CCDirector::sharedDirector()->getWinSize().width/2,30));
    m_loading->removeFromParent();
    this->addChild(pStart);
    
    this->setTouchEnabled(true);
#endif
}

bool IntroLayer::ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent)
{
	CCDirector::sharedDirector()->replaceScene(CCTransitionFade::create(0.5, m_pSelScene));
    return 1;
}
const int BufferdImageCount = 40;
const char * BufferedImageName[] = {
    "back.png","btnGameOverMenu.png","btnGameOverNext.png",
    "btnGameOverRetry.png","btngear.png","clock.png",
    "direction.png","header.png","header_withString.png",
    "menu_background.png","menu_btnmenu.png","menu_leftarrow.png",
    "menu_rightarrow.png","menu_rightarrow_grey.png","play.png",
    "resultBackground.jpg","resultBackground_lose.png","resultBackground_win.png",
    "scrolldown.png","scrollup.png","teleport.png",
    "temp_blue.png","temp_red.png","turbo.png",
    "zippo.png",
    
    "tube_curve_blue.png","tube_line_blue.png","tube_curve_black.png",
    "tube_line_black.png","tube_curve_red.png","tube_line_red.png",
    "tube_curve_green.png","tube_line_green.png","tube_curve_yellow.png",
    "tube_line_yellow.png","tube_curve_purple.png","tube_line_purple.png",
    
    "backImage1.png","backImage2.png","backImage3.png"};

/*void IntroLayer::loadResource()
{
    CCTextureCache::sharedTextureCache()->addImage("back.png");
    CCTextureCache::sharedTextureCache()->addImage("btnGameOverMenu.png");
    CCTextureCache::sharedTextureCache()->addImage("btnGameOverNext.png");
    CCTextureCache::sharedTextureCache()->addImage("btnGameOverRetry.png");
    CCTextureCache::sharedTextureCache()->addImage("btngear.png");
    CCTextureCache::sharedTextureCache()->addImage("clock.png");
    CCTextureCache::sharedTextureCache()->addImage("direction.png");
    CCTextureCache::sharedTextureCache()->addImage("header.png");
    CCTextureCache::sharedTextureCache()->addImage("header_withString.png");
    CCTextureCache::sharedTextureCache()->addImage("menu_background.png");
    CCTextureCache::sharedTextureCache()->addImage("menu_btnmenu.png");
    CCTextureCache::sharedTextureCache()->addImage("menu_leftarrow.png");
    CCTextureCache::sharedTextureCache()->addImage("menu_rightarrow.png");
    CCTextureCache::sharedTextureCache()->addImage("menu_rightarrow_grey.png");
    CCTextureCache::sharedTextureCache()->addImage("play.png");
    CCTextureCache::sharedTextureCache()->addImage("resultBackground.jpg");
    CCTextureCache::sharedTextureCache()->addImage("resultBackground_lose.png");
    CCTextureCache::sharedTextureCache()->addImage("resultBackground_win.png");
    CCTextureCache::sharedTextureCache()->addImage("scrolldown.png");
    CCTextureCache::sharedTextureCache()->addImage("scrollup.png");
    CCTextureCache::sharedTextureCache()->addImage("teleport.png");
    CCTextureCache::sharedTextureCache()->addImage("temp_blue.png");
    CCTextureCache::sharedTextureCache()->addImage("temp_red.png");
    CCTextureCache::sharedTextureCache()->addImage("turbo.png");
    CCTextureCache::sharedTextureCache()->addImage("zippo.png");

    
    CCTextureCache::sharedTextureCache()->addImage("tube_curve_blue.png");
    CCTextureCache::sharedTextureCache()->addImage("tube_line_blue.png");
    CCTextureCache::sharedTextureCache()->addImage("tube_curve_black.png");
    CCTextureCache::sharedTextureCache()->addImage("tube_line_black.png");
    CCTextureCache::sharedTextureCache()->addImage("tube_curve_red.png");
    CCTextureCache::sharedTextureCache()->addImage("tube_line_red.png");
    CCTextureCache::sharedTextureCache()->addImage("tube_curve_green.png");
    CCTextureCache::sharedTextureCache()->addImage("tube_line_green.png");
    CCTextureCache::sharedTextureCache()->addImage("tube_curve_yellow.png");
    CCTextureCache::sharedTextureCache()->addImage("tube_line_yellow.png");
    CCTextureCache::sharedTextureCache()->addImage("tube_curve_purple.png");
    CCTextureCache::sharedTextureCache()->addImage("tube_line_purple.png");
    
    
    CCTextureCache::sharedTextureCache()->addImage("backImage1.png");
    CCTextureCache::sharedTextureCache()->addImage("backImage2.png");
    CCTextureCache::sharedTextureCache()->addImage("backImage3.png");
    
}*/

void IntroLayer::draw()
{
    
    float progressTotalLength = tileWidth * 11;//progressDt * BufferdImageCount;
    
    float progressDt = progressTotalLength/BufferdImageCount;
    float progressHeight = 5;
    
    //CCLayer::draw();
    float x0 = m_pBack->getPosition().x - progressTotalLength/2;
    float y0 = 12;

    //draw the whole bar length with black
    ::ccDrawSolidRect(ccp(x0,y0), ccp(x0 + progressTotalLength, y0 + progressHeight),
                      ccc4f(0, 0, 0.1, 0.7));
    
    //draw progress
    ::ccDrawSolidRect(ccp(x0,y0), ccp(x0 + progressDt * m_loadIndex, y0 + progressHeight),
                      ccc4f(1, 1, 0.9, 0.9));
    //draw some shadow on bar
    ::ccDrawSolidRect(ccp(x0,y0), ccp(x0 + progressDt * m_loadIndex, y0 + 2),
                      ccc4f(0.8, 0.8, 0.8, 0.9));
}

void IntroLayer::updateProgress(float dt)
{
    CCTextureCache::sharedTextureCache()->addImage(BufferedImageName[m_loadIndex++]);
    CCTextureCache::sharedTextureCache()->addImage(BufferedImageName[m_loadIndex++]);
    CCTextureCache::sharedTextureCache()->addImage(BufferedImageName[m_loadIndex++]);
    CCTextureCache::sharedTextureCache()->addImage(BufferedImageName[m_loadIndex++]);
    
    if (m_loadIndex == BufferdImageCount) {
        
        m_finishLoading = true;
        
        unschedule(schedule_selector(IntroLayer::updateProgress));
        
//        m_pSelScene = LevelSelector::scene();
//        m_pSelScene->retain();
        
        CCCallFunc* delayShowFinished = CCCallFunc::create(this, callfunc_selector(IntroLayer::delayShowFinishProgress)) ;
        this->runAction(CCSequence::create(CCDelayTime::create(0.4),delayShowFinished,NULL));
    }
}

