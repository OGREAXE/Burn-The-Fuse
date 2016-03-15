#pragma once
#include <string>
#include "sqlite3/sqlite3.h"
//int loadRecord(void * para, int n_column, char ** column_value, char**column_name);
using namespace std;

#define  KEY_UNLOCKEDLEVEL "unlocke_level"
#define  COLUMNNAME_STEPCOUNT "stepcount"
#define  COLUMNNAME_BESTTIME "besttime"
#define  KEY_HASSEEMTUTORIALS "hasSeemTutorial"
class UserData
{
public:
	UserData(void);
	~UserData(void);
	static UserData*sharedUserData();

	int getBestStepCount(int level);
	void setBestStepCount(int level, int stepCount);

	int getBestTime(int level);
	void setBestTime(int level,int bestTime);
	
	//from 1-100
	//levels beyond user level is locked
	int getUnlockedLevel();
	void setUnlockedLevel(int level);
	//friend int loadRecord(void * para, int n_column, char ** column_value, char**column_name);

	const static std::string strMainTableName;
	const static std::string strDbFileName;
	const static int nCountLevel;
private:
	static UserData*m_userData;
	sqlite3 *m_pDb;
	void createDB();
	bool isDbExist();
};

