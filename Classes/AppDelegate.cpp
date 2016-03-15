/****************************************************************************
Copyright (c) 2010-2013 cocos2d-x.org
Copyright (c) Microsoft Open Technologies, Inc.

http://www.cocos2d-x.org

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
****************************************************************************/
#include "AppDelegate.h"

#include <vector>
#include <string>

#include "PuzzleScene.h"
#include "PuzzleCreator.h"
#include "LevelSelectScene.h"
#include "AppMacros.h"
#include "Intro.h"
USING_NS_CC;
using namespace std;

float scale_rate = 0;
AppDelegate::AppDelegate() {

}

AppDelegate::~AppDelegate() 
{
}

bool AppDelegate::applicationDidFinishLaunching() {
    // initialize director
    CCDirector* pDirector = CCDirector::sharedDirector();
    CCEGLView* pEGLView = CCEGLView::sharedOpenGLView();

    pDirector->setOpenGLView(pEGLView);
	CCSize frameSize = pEGLView->getFrameSize();

    // Set the design resolution
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WINRT) || (CC_TARGET_PLATFORM == CC_PLATFORM_WP8)
    pEGLView->setDesignResolutionSize(designResolutionSize.width, designResolutionSize.height, kResolutionShowAll);
#else
//    pEGLView->setDesignResolutionSize(designResolutionSize.width, designResolutionSize.height, kResolutionNoBorder);
    //pEGLView->setDesignResolutionSize(designResolutionSize.width, designResolutionSize.height, kResolutionNoBorder);
    float whRatio =frameSize.width/frameSize.height;
    if (whRatio > 1.5) {
        pEGLView->setDesignResolutionSize(designResolutionSize.width, designResolutionSize.height, kResolutionShowAll);
    }
    else
    {
        pEGLView->setDesignResolutionSize(designResolutionSize.width, designResolutionSize.height, kResolutionNoBorder);
    }
    
#endif

    
    vector<string> searchPath;

    // In this demo, we select resource according to the frame's height.
    // If the resource size is different from design resolution size, you need to set contentScaleFactor.
    // We use the ratio of resource's height to the height of design resolution,
    // this can make sure that the resource's height could fit for the height of design resolution.

	// if the frame's height is larger than the height of medium resource size, select large resource.
	if (frameSize.width > Resource_2048_1536.size.width)
	{
		//searchPath.push_back(Resource_2208_1242.directory);

		pDirector->setContentScaleFactor(MIN(Resource_2208_1242.size.height/designResolutionSize.height, Resource_2208_1242.size.width/designResolutionSize.width));
	}
	// if the frame's height is larger than the height of medium resource size, select large resource.
	else if (frameSize.width > Resource_1334_750.size.width)
	{
		//searchPath.push_back(Resource_2048_1536.directory);
		pDirector->setContentScaleFactor(MIN(Resource_2048_1536.size.height/designResolutionSize.height, Resource_2048_1536.size.width/designResolutionSize.width));
        //pDirector->setContentScaleFactor(Resource_2048_1536.size.height/designResolutionSize.height);
	}
    // if the frame's height is larger than the height of medium resource size, select large resource.
	else if (frameSize.width > Resource_1136_640.size.width)
	{
		//searchPath.push_back(Resource_1334_750.directory);

		pDirector->setContentScaleFactor(MIN(Resource_1334_750.size.height/designResolutionSize.height, Resource_1334_750.size.width/designResolutionSize.width));
	}
	// if the frame's height is larger than the height of small resource size, select medium resource.
	else if (frameSize.width > Resource_1024_768.size.width)
	{
        //searchPath.push_back(Resource_1136_640.directory);

        pDirector->setContentScaleFactor(MIN(Resource_1136_640.size.height/designResolutionSize.height, Resource_1136_640.size.width/designResolutionSize.width));
	}
    // if the frame's height is larger than the height of small resource size, select medium resource.
    else if (frameSize.width > Resource_960_640.size.width)
    {
        //searchPath.push_back(Resource_1024_768.directory);
        
        pDirector->setContentScaleFactor(MIN(Resource_1024_768.size.height/designResolutionSize.height, Resource_1024_768.size.width/designResolutionSize.width));
    }
    // if the frame's height is smaller than the height of medium resource size, select small resource.
	else 
    {
        //searchPath.push_back(Resource_960_640.directory);

        pDirector->setContentScaleFactor(MIN(Resource_960_640.size.height/designResolutionSize.height, Resource_960_640.size.width/designResolutionSize.width));
    }
    
    //set the 1536 property of tiles
    //use scale to fit device screen size
    searchPath.push_back(Resource_2048_1536.directory);
	//set scale rate
	scale_rate = frameSize.height/DESIGN_PHYSICAL_RESOLUTION;
	searchPath.push_back("common");
    searchPath.push_back("thumbs");
    searchPath.push_back("sound");
    // set searching path
    CCFileUtils::sharedFileUtils()->setSearchPaths(searchPath);
	
    // turn on display FPS
    pDirector->setDisplayStats(false);

    // set FPS. the default value is 1.0/60 if you don't call this
    pDirector->setAnimationInterval(1.0 / 60);

//#define _CREATE_PUZZLE_
	// create a scene. it's an autorelease object
#ifdef _CREATE_PUZZLE_
	CCScene *pScene = PuzzleCreator::scene();
	pDirector->runWithScene(pScene);
#else
	//CCScene *pScene = LevelSelector::scene();
    CCScene *pScene = IntroLayer::scene();
	// run
	pDirector->runWithScene(pScene);
#endif  

    return true;
}

// This function will be called when the app is inactive. When comes a phone call,it's be invoked too
void AppDelegate::applicationDidEnterBackground() {
    CCDirector::sharedDirector()->stopAnimation();

    // if you use SimpleAudioEngine, it must be pause
    // SimpleAudioEngine::sharedEngine()->pauseBackgroundMusic();
}

// this function will be called when the app is active again
void AppDelegate::applicationWillEnterForeground() {
    CCDirector::sharedDirector()->startAnimation();

    // if you use SimpleAudioEngine, it must resume here
    // SimpleAudioEngine::sharedEngine()->resumeBackgroundMusic();
}
