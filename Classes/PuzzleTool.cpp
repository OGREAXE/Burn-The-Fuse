#include "PuzzleTool.h"
#include "AppDelegate.h"

void split(string&s ,string&delim, vector<string>&ret)
{
	size_t last = 0;
	size_t index = s.find_first_of(delim,last);
	while (index!=std::string::npos)
	{
		ret.push_back(s.substr(last,index-last));
		last = index+1;
		index=s.find_first_of(delim,last);
	}

	if (index - last > 0)
	{
		ret.push_back(s.substr(last,index-last));
	}
}

CCSprite * getSpriteBySize(CCSize size, string filename)
{
	CCSprite * pRet = CCSprite::create(filename.c_str());
	pRet->setScaleX(size.width/pRet->getTexture()->getContentSize().width);
	pRet->setScaleY(size.height/pRet->getTexture()->getContentSize().height);
	return pRet;
}

//scale sprite related to 2048 resolution
extern float scale_rate;
CCSprite * getFixedSizeSprite(string filename)
{
    return getFixedSizeSprite(filename,scale_rate);
}
#define AUTO_SCALE
CCSprite * getFixedSizeSprite(string filename, float rate)
{
	CCSprite * pSprite = CCSprite::create(filename.c_str());
    
#ifdef AUTO_SCALE
	if (pSprite)
	{
        float scaledWidth = pSprite->getContentSize().width * rate;
        float scaledHeight = pSprite->getContentSize().height * rate;
        
        CCRenderTexture * pRt = CCRenderTexture::create(scaledWidth, scaledHeight);
        pRt->begin();
        pSprite->setScale(rate);
		//pSprite->setScaleY(rate);
        pSprite->setPosition(ccp(scaledWidth/2,scaledHeight/2));
        pSprite->setFlipY(true);
        pSprite->visit();
        pRt->end();
        return CCSprite::createWithTexture(pRt->getSprite()->getTexture());
        
	}
	
	return NULL;
#else
    return pSprite;
#endif
}

CCSprite * getFixedSizeSprite(string filename, float width, float height)
{
	CCSprite * pSprite = CCSprite::create(filename.c_str());
    
#ifdef AUTO_SCALE
	if (pSprite)
	{
        CCRenderTexture * pRt = CCRenderTexture::create(width, height);
        pRt->begin();
        pSprite->setScaleX(width / pSprite->getContentSize().width);
        pSprite->setScaleY(height / pSprite->getContentSize().height);
		//pSprite->setScaleY(rate);
        pSprite->setPosition(ccp(width/2,height/2));
        pSprite->setFlipY(true);
        pSprite->visit();
        pRt->end();
        return CCSprite::createWithTexture(pRt->getSprite()->getTexture());
        
	}
	
	return NULL;
#else
    return pSprite;
#endif
}

CCSprite * getResizedSrite_withOriginalRect(CCSprite * original, float ratio)
{
    CCPoint oldPosition = original->getPosition();
    CCPoint oldAp = original->getAnchorPoint();
    float oldscale = original->getScale();
    
    CCRenderTexture * pRt = CCRenderTexture::create(original->getContentSize().width, original->getContentSize().height);
    
    original->setAnchorPoint(ccp(0.5, 0.5));
    original->setPosition(ccp(original->getContentSize().width/2,original->getContentSize().height/2));
    original->setFlipY(true);
    original->setScale(ratio);
    
    pRt->begin();
    original->visit();
    pRt->end();
    
    original->setFlipY(false);
    original->setPosition(oldPosition);
    original->setAnchorPoint(oldAp);
    original->setScale(oldscale);
    
    return CCSprite::createWithTexture(pRt->getSprite()->getTexture());
}

long  getCurrentTime()
{
    struct  timeval tv;
    gettimeofday(&tv,NULL);
    return  tv.tv_sec * 1000 + tv.tv_usec / 1000;
}