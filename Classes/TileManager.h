//
//  TileManager.h
//  FusePuzzle
//
//  Created by 梁志远 on 12/20/14.
//
//

#ifndef __FusePuzzle__TileManager__
#define __FusePuzzle__TileManager__

#include <iostream>
#include "cocos2d.h"
#include "MyMapTile.h"

using namespace cocos2d;

class TileManager
{
private:
    static TileManager * m_tilemanager;
    
public:
    static TileManager * sharedManager();
};

#endif /* defined(__FusePuzzle__TileManager__) */
