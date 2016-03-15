#include "UserData.h"
#include "cocos2d.h"
#include "AppMacros.h"

UserData * UserData::m_userData = NULL;
string const UserData::strMainTableName = "MAIN";
string const UserData::strDbFileName = "userData.db3";
int const UserData::nCountLevel = LEVELCOUNT;

UserData::UserData(void)
{
	string path = cocos2d::CCFileUtils::sharedFileUtils()->getWritablePath() + strDbFileName;
	int resut = sqlite3_open(path.c_str(),&m_pDb);
	if (resut == SQLITE_OK)
	{
		if (!isDbExist())
		{
			createDB();
		}
	}
}


UserData::~UserData(void)
{
	sqlite3_close(m_pDb);
}

UserData* UserData::sharedUserData()
{
	if (! m_userData)
	{
		m_userData = new UserData();
	}

	return m_userData;
}

int UserData::getBestStepCount(int level)
{
	int ret = 0;
	string sql = "select ";
	sql.append(COLUMNNAME_STEPCOUNT);
	sql.append(" from ");
	sql.append(strMainTableName);
	sql.append(" where level=");
	sql += level;
	sqlite3_stmt * pStmt = NULL;
	int result = sqlite3_prepare(m_pDb,sql.c_str(),sql.length(),&pStmt,0);
	if (result != SQLITE_OK)
	{
		return 0;
	}
	if (sqlite3_step(pStmt) == SQLITE_ROW)
	{
		ret = sqlite3_column_int(pStmt,1);
	}
	
	return ret;
}

void UserData::setBestStepCount(int level, int stepCount)
{
	string sql = "update ";
	sql.append(strMainTableName);
	sql.append(" set ");
	sql.append(COLUMNNAME_STEPCOUNT);
	sql.append("=");
	sql += stepCount;
	sql.append(" where level=");
	sql += level;
	int result = sqlite3_exec(m_pDb,sql.c_str(),NULL,NULL,NULL);
	if (result != SQLITE_OK)
	{
	}
}

int UserData::getBestTime(int level)
{
	int ret;
	string sql = "select ";
	sql.append(COLUMNNAME_BESTTIME);
	sql.append(" from ");
	sql.append(strMainTableName);
	sql.append(" where level=");
	sql += level;
	sqlite3_stmt * pStmt = NULL;
	int result = sqlite3_prepare(m_pDb,sql.c_str(),sql.length(),&pStmt,0);
	if (result != SQLITE_OK)
	{
		return 0;
	}
	if (sqlite3_step(pStmt) == SQLITE_ROW)
	{
		ret = sqlite3_column_int(pStmt,1);
		//ret.append((char*)sqlite3_column_text(pStmt,1));
	}

	return ret;
}

void UserData::setBestTime(int level,int bestTime)
{
	string sql = "update ";
	sql.append(strMainTableName);
	sql.append(" set ");
	sql.append(COLUMNNAME_BESTTIME);
	sql.append("=");
	sql += bestTime;
	sql.append(" where level=");
	sql += level;
	int result = sqlite3_exec(m_pDb,sql.c_str(),NULL,NULL,NULL);
	if (result != SQLITE_OK)
	{
	}
}

void UserData::createDB()
{
	string sql = "create table ";
	sql.append(strMainTableName);
	sql.append("(id integer, level integer, ");
	sql.append(COLUMNNAME_STEPCOUNT);
	sql.append(" integer, ");
	sql.append(COLUMNNAME_BESTTIME);
	sql.append(" integer)");

	char* errMsg = NULL;
	int result = sqlite3_exec(m_pDb,sql.c_str(),NULL,NULL,&errMsg);
	if (result != SQLITE_OK)
	{
		cocos2d::CCLog("create table fail, error :%d, message :%s", result,errMsg);
	}
	else
	{
		for (int i=1;i<=nCountLevel;i++)
		{
			string sql = "insert into ";
			sql.append(strMainTableName);
			sql.append("(id,level,");
			sql.append(COLUMNNAME_STEPCOUNT);
			sql.append(",");
			sql.append(COLUMNNAME_BESTTIME);
			sql.append(") values(");
			sql +=i;
			sql.append(",");
			sql +=i;
			sql.append(",0,0)");
			int result = sqlite3_exec(m_pDb,sql.c_str(),NULL,NULL,&errMsg);
			if (result!=SQLITE_OK)
			{
				cocos2d::CCLog("insert fail, error :%d, message :%s", result,errMsg);
			}
		}
	}
}

bool UserData::isDbExist()
{
	string sql = "select count(type) from sqlite_master where type='table' and name='";
	sql.append(strMainTableName);
	sql.append("'");
	int result = sqlite3_exec(m_pDb,sql.c_str(),NULL,NULL,NULL);
	if (result != SQLITE_OK)
	{
		return false;
	}
	else
	{
		return true;
	}
}

int UserData::getUnlockedLevel()
{
#ifdef DEVELOPER_VERSION
    return LEVELCOUNT;
#else
#define  ZERO_DEFAULT 0
	int unlockedLevel = cocos2d::CCUserDefault::sharedUserDefault()->getIntegerForKey(KEY_UNLOCKEDLEVEL, ZERO_DEFAULT);

	//if no level data found 
	if (ZERO_DEFAULT == unlockedLevel )
	{
		unlockedLevel = PLAYABLE_LEVEl_ON_FIRSTPLAY;
		cocos2d::CCUserDefault::sharedUserDefault()->setIntegerForKey(KEY_UNLOCKEDLEVEL, unlockedLevel);
	}
	return unlockedLevel;
#endif
}
	 

void UserData::setUnlockedLevel(int level)
{
	cocos2d::CCUserDefault::sharedUserDefault()->setIntegerForKey(KEY_UNLOCKEDLEVEL, level);
}
