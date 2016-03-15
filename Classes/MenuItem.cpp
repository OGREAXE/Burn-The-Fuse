//
//  MenuItem.cpp
//  FusePuzzle
//
//  Created by 梁志远 on 12/7/14.
//
//

#include "MenuItem.h"

PuzzleMenuItem * PuzzleMenuItem::create(CCNode* normalSprite, CCNode* selectedSprite,CCObject* target, SEL_MenuHandler selector)
{
    return create(normalSprite, selectedSprite, NULL, target, selector);
}

PuzzleMenuItem * PuzzleMenuItem::create(CCNode* normalSprite, CCNode* selectedSprite, CCNode* disabledSprite,CCObject* target, SEL_MenuHandler selector)
{
    PuzzleMenuItem*pRet = new PuzzleMenuItem();
    if (pRet->initWithNormalSprite(normalSprite, selectedSprite,disabledSprite, target, selector))
    {
        pRet->autorelease();
    }
    else
    {
        CC_SAFE_DELETE(pRet);
    }
    
    return pRet;
    
}