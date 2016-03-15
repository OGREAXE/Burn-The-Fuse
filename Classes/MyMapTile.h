#pragma once
//#include "MyType.h"
#include "cocos2d.h"
using namespace cocos2d;
const static int tileWidth = 26;
const static int tileHeight = 26;
const static int tubeWidth = 0;

const static float fireNormalSpeed = 0.7;
const static float fireTurboSpeed = 1.5;
const static char* MY_NOTIFICATION_NEXT_TILE = "NOTIFICATION_NEXT_TILE";
enum 
{
	GENERIC_TYPE_CURVE,
	GENERIC_TYPE_LINE,
};

enum 
{
	//out side tile is unmovable and no effective in game
	TILETYPE_OUTSIDE,
	//other tiles can move to void type
	TILETYPE_VOID,
	//a normal tile with tubes becomes this notube type when its tubes are erased out. Maybe it's unnecessary to define this type.
	TILETYPE_NOTUBE,
	//normal tile types
	TILETYPE_CURVE_ONE_BOTTONLEFT,
	TILETYPE_CURVE_ONE_BOTTONRIGHT,
	TILETYPE_CURVE_ONE_TOPLEFT,
	TILETYPE_CURVE_ONE_TOPRIGHT,
	TILETYPE_CURVE_TWO_TOPLEFT_BOTTONRIGHT,
	TILETYPE_CURVE_TWO_BOTTONLEFT_TOPRIGHT,
	TILETYPE_LINE_ONE_HORIZONTAL,
	TILETYPE_LINE_ONE_VERTICAL,
	TILETYPE_LINE_TWO_HORIZONTALONTOP,
	TILETYPE_LINE_TWO_VERTICALONTOP,
};

enum 
{
	TAG_INVALID,
	TAG_CURVE_TOPLEFT,
	TAG_CURVE_TOPRIGHT,
	TAG_CURVE_BOTTONLEFT,
	TAG_CURVE_BOTTONRIGHT,
	TAG_LINE_HORIZONTAL,
	TAG_LINE_VERTICAL,
};

//direction(from)
enum 
{
	DIRECTION_TOP,
	DIRECTION_LEFT,
	DIRECTION_BOTTON,
	DIRECTION_RIGHT,
};
struct START_INFO
{
	START_INFO(){start_direction = DIRECTION_LEFT;x = 0;y = 0;}
	int start_direction;
	int x;
	int y;
	inline void setPos(int px, int py){x=px,y=py;}
};
//position of Z axis of child1 and child2, the numbers between is for other sprites like fire
enum 
{
	Z_CHILD1 = 1,
	Z_CHILD2 = 10,

};

struct TELEPORT_INFO
{
    bool top = false;
    bool bottom = false;
    bool left = false;
    bool right = false;
};

enum TileColor
{
    TILECOLOR_RED = 0,
    TILECOLOR_BLUE,
    TILECOLOR_GREEN,
    TILECOLOR_YELLOW,
    TILECOLOR_PURPLE,
    TILECOLOR_BLACK,
};

class TileCache
{
public:
    static TileCache * sharedCache()
    {
        if (m_cache == NULL) {
            m_cache = new TileCache();
        }
        return m_cache;
    }
private:
    static TileCache * m_cache;
private:
    std::map<int, CCSprite*> m_linebuffer;
    std::map<int, CCSprite*> m_curvebuffer;
public:
    TileCache(){};
    CCSprite * getLineByColor(TileColor color);
    CCSprite * getCurveByColor(TileColor color);
};

class CCGridTileBase:public CCSprite
{
public:
	int grid_x;
	int grid_y;
	void setGridCoordPosition(int x, int y){grid_x = x;grid_y = y;}
};
//a magic var to solve the rendertexture too small causing white gap problem
const static float tile_rt_scale = 1.02;
static ccColor4F tileBackgroundColor = {0.6f,0.6f,0.6f,1.0f};

/*
 the tile puzzle consist of
 */
class CCTubeGridTile:public CCGridTileBase
{
public:
	CCTubeGridTile(void);
	~CCTubeGridTile(void);

public:
	int type;
	int genericType;
    
    CC_PROPERTY(TELEPORT_INFO, m_teleportInfo, TeleportInfo)
    
public:
    
	//static CCTubeGridTile*createWithType(int type,int x, int y);
	static CCTubeGridTile*createWithType(int type, bool backGround = true, TileColor color = TILECOLOR_BLUE);
    
	static CCSprite*createSingleCurve(TileColor color);
	static CCSprite*createSingleLine(TileColor color);
    
    bool initWithType(int type, bool backGround, TileColor color);

	void beginErasing(float eraserSpeed, int direction);
	void endErasing();
    void updateErasing(float delta);

	int getEndErasingDirection();
	bool getIsErasing(){return m_isErasing;}
	CCPoint getFireLoctionWorld();//

	bool isCanGetFireLocation(){return m_canGetFireLocation;}
	void setCanGetFireLocation(bool canGet){ m_canGetFireLocation = canGet;}

	float getRemainTubes(){return m_remainTubes;}
	CCSprite*getChildForStartErasingAt(int direction);

	void setEraserSpeed(float speed){m_eraserSpeed = speed;}
private:
	//when a tile begin to erase, use this as the texture

	//if curve
	float m_curveRadius;
	float m_erasingAngle;
	bool m_isErasing;
	int m_startErasingDirection;
	int m_endErasingDirection;
	float m_eraserSpeed;
	CCPoint m_erasingPoint;
	float m_eraserRadius;
	bool m_isReversedErasing;
	float m_eraserCurveAngleUpperBound;
	float m_eraserCurveAngleLowerBound;
	float m_eraserLineUpperBound;
	float m_eraserLineLowerBound;

	CCRenderTexture*m_erasingRenderTexture;
	CCDrawNode*m_eraser;

	CCRenderTexture* m_rtCurve;
	CCRenderTexture* m_rtLine;
    CCRenderTexture* m_rtCurve2;
	CCRenderTexture* m_rtLine2;
	CCRenderTexture* m_rtVoid;
	
	CCSprite*m_erasingChild;

	//for the concern of simplicity, every tile has a fire sprite to show fire animation
	CCPoint m_currentFireLoctionPoint;
	bool m_canGetFireLocation;
	float m_remainTubes;
    
    static CCSprite* m_singleCurve;
    static CCSprite* m_singleLine;
private:
    void erasePoint(float x, float y);
	void erasePointInCurveForAngle(float angle);
	void moveFire(float x, float y);
    int getExitOfEraserForStartErasingAt(int direction);
	bool getIsReversedErasing(int direction, int childTag);
    
    void initLine1(TileColor color);
    void initLine2(TileColor color);
    void initCurve1(TileColor color);
    void initCurve2(TileColor color);
};

enum 
{
	TELEPORTTILE_GATE_BOTTON,
	TELEPORTTILE_GATE_TOP,
};

class ChildTubeSprite:public CCSprite
{
public:
    static ChildTubeSprite* createWithTexture(CCTexture2D *pTexture);
    void setRenderTexture(CCRenderTexture*rt){m_rt = rt;}
    CCRenderTexture * getRenderTexture(){return  m_rt;}
protected:
    CCRenderTexture*m_rt;
};


