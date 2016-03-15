//
//  PuzzleData.cpp
//  FusePuzzle
//
//  Created by 梁志远 on 12/18/14.
//
//

#include "PuzzleData.h"
#include "PuzzleTool.h"
PuzzleDataCache * PuzzleDataCache::m_gameDataCache = NULL;
PuzzleDataCache * PuzzleDataCache::sharedCache()
{
    if(m_gameDataCache == NULL)
    {
        m_gameDataCache = new PuzzleDataCache();
    }
    return m_gameDataCache;
}

PuzzleDataCache::PuzzleDataCache(){
    const char* plistPath = "puzzledata.plist";
    CCDictionary* plistDic = CCDictionary::createWithContentsOfFile(plistPath);
    
    int count = plistDic->count();
    
    CCDictElement* pCCDictElement;
    
    m_dataList.clear();
    m_dataList.reserve(count);
    
    CCDICT_FOREACH(plistDic,pCCDictElement)
    {
        if (pCCDictElement != NULL)
        {
            const char* key = pCCDictElement->getStrKey();
            
            CCDictionary* pDicKey = dynamic_cast<CCDictionary*>(plistDic->objectForKey(key));
            
            CCString * startInfo = dynamic_cast<CCString *>(pDicKey->objectForKey("start"));
            
            vector<string> vTemp;
            string strDelim("%");
            string strStart = startInfo->getCString();
            split(strStart,strDelim,vTemp);
            
            int startDirection = CCString(vTemp[0]).intValue();
            int startx = CCString(vTemp[1]).intValue();
            int starty = CCString(vTemp[2]).intValue();
            CCArray * tilesArray = dynamic_cast<CCArray  *>(pDicKey->objectForKey("data"));
            
            
            PuzzleData puzzleData(startDirection,startx,starty,tilesArray);
            //m_dataList.insert(m_dataList.begin() + atoi(key) - 1, puzzleData);
            m_dataList.push_back(puzzleData);
        }
    }
    
    CCLog("finish loading plist");
}

PuzzleData* PuzzleDataCache::getData(int level)
{
    return &m_dataList[level - 1];
}
