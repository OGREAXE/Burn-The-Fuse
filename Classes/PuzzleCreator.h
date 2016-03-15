#ifndef _PUZZLECREATOR_H_
#define  _PUZZLECREATOR_H_
#include "cocos2d.h"
#include "cocos-ext.h"
#include "MyMapTile.h"
USING_NS_CC;
USING_NS_CC_EXT;
enum OPERATION_TYPE
{
	OPERATION_TYPE_TILE,
	OPERATION_TYPE_START,
	OPERATION_TYPE_DESTROY,
};

class PuzzleCreator : public cocos2d::CCLayer, public cocos2d::extension::CCEditBoxDelegate
{
public:
	// Here's a difference. Method 'init' in cocos2d-x returns bool, instead of returning 'id' in cocos2d-iphone
	virtual bool init();  
	~PuzzleCreator();
	void initBaseData();
	void initBackground();
	void initUI();

	// there's no 'id' in cpp, so we recommend returning the class instance pointer
	static cocos2d::CCScene* scene();
	void registerWithTouchDispatcher();
	// a selector callback
	void menuCloseCallback(CCObject* pSender);
	
	void menuLine1hCallback(CCObject* pSender);
	void menuLine1vCallback(CCObject* pSender);
	void menuLine2hCallback(CCObject* pSender);
	void menuLine2vCallback(CCObject* pSender);
	void menuTube1ulCallback(CCObject* pSender);
	void menuTube1urCallback(CCObject* pSender);
	void menuTube1blCallback(CCObject* pSender);
	void menuTube1brCallback(CCObject* pSender);
	void menuTube2upCallback(CCObject* pSender);
	void menuTube2downCallback(CCObject* pSender);
	void menuEmptyCallback(CCObject* pSender);
	void menuVoidCallback(CCObject* pSender);

	void menuDirectionUpCallback(CCObject* pSender);
	void menuDirectionDownCallback(CCObject* pSender);
	void menuDirectionLeftCallback(CCObject* pSender);
	void menuDirectionRightCallback(CCObject* pSender);

	void menuDestroyCallback(CCObject* pSender);
	void menuGenCallback(CCObject* pSender);
	void menuExitCallback(CCObject* pSender);

	// implement the "static node()" method manually
	CREATE_FUNC(PuzzleCreator);

	//touch
	bool ccTouchBegan(cocos2d::CCTouch *pTouch, cocos2d::CCEvent *pEvent);
	void ccTouchMoved(CCTouch *pTouch, CCEvent *pEvent);
	void ccTouchEnded(CCTouch *pTouch, CCEvent *pEvent);

	int m_currentTileX;
	int m_currentTileY;
	int m_endErasingDirection;

	void createType(int x, int y);
	void destroyType(int x, int y);

	bool writeTilesDataToFile();
	bool createThumbnail();
	//cceditboxdelegate
	virtual void editBoxReturn(CCEditBox* editBox);//

private:
	std::string m_strLevel;
	cocos2d::CCArray * m_vtilesArray;
	cocos2d::CCSprite*m_pBackground;
	CCPoint m_backgroundOrigin;

	int n_tile_x;
	int n_tile_y;
	//MyMapTile * m_infoGrid[15][10];
	CCGridTileBase *** m_infoGrid;

	CCGridTileBase ***newInfoGrid(int n_x,int n_y);
	void deleteInfoGrid(CCGridTileBase ***,int n_x);

	CCTubeGridTile*m_voidTile;

	OPERATION_TYPE m_currentOperationType;
	//detailed type define in mymaptile.h
	int m_subtype;
	CCSprite * m_selectedFrame;
	void setSelectedAndType(CCObject*pItmNode,OPERATION_TYPE optype,int subtype = 0);

	START_INFO m_startInfo;
	CCSprite*m_startPointSprite;
};

#endif