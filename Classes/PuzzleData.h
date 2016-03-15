//
//  PuzzleData.h
//  FusePuzzle
//
//  Created by 梁志远 on 12/18/14.
//
//

#ifndef __FusePuzzle__PuzzleData__
#define __FusePuzzle__PuzzleData__

#include "cocos2d.h"
#include "MyMapTile.h"
#include <vector>

using namespace cocos2d;
using namespace std;

class PuzzleData
{
public:
    PuzzleData(){}
    PuzzleData(int startDirection, int startX, int startY, CCArray*data)
    {
        m_start.start_direction = startDirection;
        m_start.x = startX;
        m_start.y = startY;
        
        m_data = data;
        m_data->retain();
    }
    //~PuzzleData(){m_data->release();}
    
    START_INFO getStartInfo(){return m_start;}
    CCArray*DataCopy(){
        //return CCArray::createWithArray(m_data);
        return m_data;
    }
private:
    START_INFO m_start;
    
    CCArray* m_data;
};

class PuzzleDataCache
{
public:
    PuzzleDataCache();
private:
    static PuzzleDataCache * m_gameDataCache;
    vector<PuzzleData> m_dataList;
public:
    static PuzzleDataCache * sharedCache();
    void init(){}
    
    PuzzleData* getData(int level);
};

#endif /* defined(__FusePuzzle__PuzzleData__) */
