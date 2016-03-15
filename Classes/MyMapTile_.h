#pragma once
//#include "MyType.h"
#include "cocos2d.h"
using namespace cocos2d;
const static int tileWidth = 26;
const static int tileHeight = 26;
const static int tubeWidth = 0;
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

//direction
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

class CCFireSprite;

class CCGridTileBase:public CCSprite
{
public:
	int grid_x;
	int grid_y;
	void setGridCoordPosition(int x, int y){grid_x = x;grid_y = y;}
};

class CCTubeGridTile:public CCGridTileBase
{
public:
	CCTubeGridTile(void);
	~CCTubeGridTile(void);

public:
	//static CCTubeGridTile*createWithType(int type,int x, int y);
	static CCTubeGridTile*createWithType(int type);
	static CCSprite*createSingleCurve();
	static CCSprite*createSingleLine();

	int type;
	int genericType;


	void beginErasing(float eraserSpeed, int direction);
	void endErasing();
    void updateErasing(float delta);

	int getEndErasingDirection();
	BOOL getIsErasing(){return m_isErasing;}

	CCPoint getFireLoctionWorld();//
	//CCSprite*getErasingChild(){return m_erasingChild;}
private:
	//when a tile begin to erase, use this as the texture

	//if curve
	float m_curveRadius;
	float m_erasingAngle;
	BOOL m_isErasing;
	int m_startErasingDirection;
	int m_endErasingDirection;
	float m_eraserSpeed;
	CCPoint m_erasingPoint;
	float m_eraserRadius;
	BOOL m_isReversedErasing;
	float m_eraserCurveAngleUpperBound;
	float m_eraserCurveAngleLowerBound;
	float m_eraserLineUpperBound;
	float m_eraserLineLowerBound;

	CCRenderTexture*m_erasingRenderTexture;
	CCDrawNode*m_eraser;

	CCRenderTexture* m_rtCurve;
	CCRenderTexture* m_rtLine;
	CCRenderTexture* m_rtVoid;

	void erasePoint(float x, float y);
	void erasePointInCurveForAngle(float angle);
	void moveFire(float x, float y);
	void moveFire(float angle);

	CCSprite*getChildForStartErasingAt(int direction);
	int getExitOfEraserForStartErasingAt(int direction);
	BOOL getIsReversedErasing(int direction, int childTag);
	CCSprite*m_erasingChild;

	//for the concern of simplicity, every tile has a fire sprite to show fire animation
	CCFireSprite*m_fire;
	CCPoint m_currentFireLoctionPoint;
};

enum 
{
	TELEPORTTILE_GATE_BOTTON,
	TELEPORTTILE_GATE_TOP,
};


class CCFireSprite :public CCSprite
{
public:
	CCFireSprite()
	{
	}

	~CCFireSprite()
	{
	}
	int m_type;
	static CCFireSprite* create();
private:

};

