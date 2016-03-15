//
//  CLabel.h
//  FusePuzzle
//
//  Created by 梁志远 on 12/3/14.
//
//

#ifndef __FusePuzzle__CLabel__
#define __FusePuzzle__CLabel__


#include "cocos2d.h"

USING_NS_CC;

namespace CLabel{
    /*使用示例
     CCSize size = CCDirector::sharedDirector()->getWinSize();
     
     //创建个背景
     CCLayerColor* whiteLayer = CCLayerColor::create(ccc4(0, 205, 205, 255), size.width, size.height);
     this->addChild(whiteLayer);
     
     //创建描边
     CCLabelTTF* outline = CLabel::textAddOutline("描边 Outline", "Arial", 40, ccWHITE, 1);
     outline->setPosition(ccp(size.width*0.5, size.height*0.7));
     this->addChild(outline);
     
     //创建阴影
     CCLabelTTF* shadow = CLabel::textAddShadow("阴影 Shadow", "Arial", 40, ccWHITE, 2, 200);
     shadow->setPosition(ccp(size.width*0.5, size.height*0.5));
     this->addChild(shadow);
     
     //创建描边加阴影
     CCLabelTTF* outlineShadow = CLabel::textAddOutlineAndShadow("描边加阴影 OutlineShadow", "Arial", 40, ccWHITE, 1, 4, 200);
     outlineShadow->setPosition(ccp(size.width*0.5, size.height*0.3));
     this->addChild(outlineShadow);
     */
    
    //**************************************************************************
    // 给文字添加描边
    //**************************************************************************
    CCLabelTTF* textAddOutline(const char* string,          // 显示的文字
                               const char* fontName,        // 字体名称
                               float fontSize,              // 字体大小
                               const ccColor3B &color3,     // 描边颜色
                               float lineWidth);            // 描边宽度
    
    //**************************************************************************
    // 添加阴影
    CCLabelTTF* textAddShadow(const char* string,           // 显示的文字
                              const char* fontName,         // 字体名称
                              float fontSize,               // 字体大小
                              const ccColor3B &color3,      // 阴影颜色
                              float shadowSize,             // 阴影宽度
                              float shadowOpacity);         // 阴影透明度
    //**************************************************************************
    
    //**************************************************************************
    // 既添加描边又添加阴影
    //**************************************************************************
    CCLabelTTF* textAddOutlineAndShadow(const char* string,     // 显示的文字
                                        const char* fontName,   // 字体名称
                                        float fontSize,         // 字体大小
                                        const ccColor3B &color3,// 阴影颜色
                                        float lineWidth,        // 描边宽度
                                        float shadowSize,       // 阴影宽度
                                        float shadowOpacity);   // 阴影透明度
};


#endif /* defined(__FusePuzzle__CLabel__) */
