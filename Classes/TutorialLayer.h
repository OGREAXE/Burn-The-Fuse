#pragma once
#include "cocos2d.h"
#include "MyMapTile.h"

using namespace cocos2d;

class TutorialLayer: public CCLayer
{
public:
	TutorialLayer();
public:
	static CCScene* scene();
	virtual bool init();  
	CREATE_FUNC(TutorialLayer);

    void registerWithTouchDispatcher();
	bool ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent);
private:
	CCTubeGridTile * m_tutorTile1;
	CCTubeGridTile * m_tutorTile2;
    CCParticleFire * m_pFire;
	CCLabelTTF * m_tips;
	CCLabelTTF * m_nextLabel;
	int m_currentStep;

	CCPoint m_zeroPoint;
private:
	void putTile(CCTubeGridTile * pTile, int x, int y);
	void tutor1Callback();
	void tutor2Callback();
	void tutor3Callback();
    
    CCAction * flashAction();
};

