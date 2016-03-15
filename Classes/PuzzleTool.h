#pragma  once
#include <string>
#include <vector>
#include "cocos2d.h"
using namespace std;
using namespace cocos2d;

void split(string&s ,string&delim, vector<string>&ret);

CCSprite * getSpriteBySize(CCSize size, string filename);

CCSprite * getFixedSizeSprite(string filename);

CCSprite * getFixedSizeSprite(string filename, float rate);

CCSprite * getFixedSizeSprite(string filename, float width, float height);

CCSprite * getResizedSrite_withOriginalRect(CCSprite * original, float ratio);

long  getCurrentTime();
//void itoa(int val, char*buf, int base);

