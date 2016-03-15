//
//  TileManager.cpp
//  FusePuzzle
//
//  Created by 梁志远 on 12/20/14.
//
//

#include "TileManager.h"
TileManager * TileManager::m_tilemanager = NULL;
TileManager * TileManager::sharedManager()
{
    if (m_tilemanager == NULL) {
        m_tilemanager = new TileManager();
    }
    return m_tilemanager;
}