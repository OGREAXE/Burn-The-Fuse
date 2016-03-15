//
//  MenuItem.h
//  FusePuzzle
//
//  Created by 梁志远 on 12/7/14.
//
//

#ifndef __FusePuzzle__MenuItem__
#define __FusePuzzle__MenuItem__

#include "cocos2d.h"
using namespace cocos2d;
class PuzzleMenuItem : public CCMenuItemSprite
{
public:
	static PuzzleMenuItem*create(CCNode* normalSprite, CCNode* selectedSprite, CCNode* disabledSprite,CCObject* target, SEL_MenuHandler selector);
    static PuzzleMenuItem*create(CCNode* normalSprite, CCNode* selectedSprite,CCObject* target, SEL_MenuHandler selector);
    void unselected(){}
};

#endif /* defined(__FusePuzzle__MenuItem__) */
