#ifndef __APPMACROS_H__
#define __APPMACROS_H__

#include "cocos2d.h"

/* For demonstrating using one design resolution to match different resources,
   or one resource to match different design resolutions.

   [Situation 1] Using one design resolution to match different resources.
     Please look into Appdelegate::applicationDidFinishLaunching.
     We check current device frame size to decide which resource need to be selected.
     So if you want to test this situation which said in title '[Situation 1]',
     you should change ios simulator to different device(e.g. iphone, iphone-retina3.5, iphone-retina4.0, ipad, ipad-retina),
     or change the window size in "proj.XXX/main.cpp" by "CCEGLView::setFrameSize" if you are using win32 or linux plaform
     and modify "proj.mac/AppController.mm" by changing the window rectangle.

   [Situation 2] Using one resource to match different design resolutions.
     The coordinates in your codes is based on your current design resolution rather than resource size.
     Therefore, your design resolution could be very large and your resource size could be small.
     To test this, just define the marco 'TARGET_DESIGN_RESOLUTION_SIZE' to 'DESIGN_RESOLUTION_2048X1536'
     and open iphone simulator or create a window of 480x320 size.

   [Note] Normally, developer just need to define one design resolution(e.g. 960x640) with one or more resources.
 */

#define DESIGN_RESOLUTION_480X320    0
#define DESIGN_RESOLUTION_1024X768   1
#define DESIGN_RESOLUTION_2048X1536  2

/* If you want to switch design resolution, change next line */
#define TARGET_DESIGN_RESOLUTION_SIZE  DESIGN_RESOLUTION_480X320

typedef struct tagResource
{
    cocos2d::CCSize size;
    char directory[100];
}Resource;

static Resource Resource_960_640  =  { cocos2d::CCSizeMake(960, 640),   "960_640" };
static Resource Resource_1024_768 =  { cocos2d::CCSizeMake(1024, 768),  "1024_768"   };
static Resource Resource_1136_640  =  { cocos2d::CCSizeMake(1136, 640), "1136_640" };
static Resource Resource_1334_750  =  { cocos2d::CCSizeMake(1334, 750), "1334_750" };
static Resource Resource_2048_1536 =  { cocos2d::CCSizeMake(2048, 1536), "2048_1536" };
static Resource Resource_2208_1242 =  { cocos2d::CCSizeMake(2208, 1242), "2208_1242" };

#define DESIGN_PHYSICAL_RESOLUTION 1536

#if (TARGET_DESIGN_RESOLUTION_SIZE == DESIGN_RESOLUTION_480X320)
static cocos2d::CCSize designResolutionSize = cocos2d::CCSizeMake(480, 320);
#elif (TARGET_DESIGN_RESOLUTION_SIZE == DESIGN_RESOLUTION_1024X768)
static cocos2d::CCSize designResolutionSize = cocos2d::CCSizeMake(1024, 768);
#elif (TARGET_DESIGN_RESOLUTION_SIZE == DESIGN_RESOLUTION_2048X1536)
static cocos2d::CCSize designResolutionSize = cocos2d::CCSizeMake(2048, 1536);
#else
#error unknown target design resolution!
#endif

// The font size 24 is designed for small resolution, so we should change it to fit for current design resolution
#define TITLE_FONT_SIZE  (cocos2d::CCEGLView::sharedOpenGLView()->getDesignResolutionSize().width / smallResource.size.width * 24)

#define GESTURE_USE_SWIPE_MOVETILE 1
#define GESTURE_USE_TAP_MOVETILE 2
#define CONTROL_TYPE GESTURE_USE_TAP_MOVETILE

//the number of playable levels (unlocked) when player enter this game for the first time
#define  PLAYABLE_LEVEl_ON_FIRSTPLAY 4

//version lite or complete
#define VERSION_LITE 0
#define VERSION_COMPLETE 1
#define BUILD_VERSION VERSION_COMPLETE

//the number of levels
#if BUILD_VERSION == VERSION_LITE
#define  LEVELCOUNT 16
#else
#define  LEVELCOUNT 100
#endif

//comment this out in release
//developer version unlocks all levels
//#define DEVELOPER_VERSION

#define TILECOUNT_X 14
#define TILECOUNT_Y 8

#endif /* __APPMACROS_H__ */
