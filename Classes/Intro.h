#include "cocos2d.h"
using namespace cocos2d;

class IntroLayer : public CCLayer
{
public:
	static cocos2d::CCScene* scene();
	virtual bool init();  
	CREATE_FUNC(IntroLayer);
	virtual void onEnterTransitionDidFinish();
    void registerWithTouchDispatcher();
	bool ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent);
    
    void updateProgress(float dt);
    void draw();
    
    int getProgress(){return m_loadIndex;}
private:
    void delayCallback(CCObject* pSender);
    void delayShowFinishProgress(CCObject* pSender);
private:
	CCScene *m_pSelScene;
    //CCLabelTTF* m_pLabelLoading;
    CCSprite * m_loading;
    CCSprite * m_pBack;
    int m_loadIndex;
    
    bool m_finishLoading;
};