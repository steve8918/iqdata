#ifndef ODBCH
#define ODBCH

#include <string>
#include <windows.h>
#include <sql.h>

struct DBHandles
{
public:
   std::string username;
   std::string password;
   std::string serverName;
   SQLHANDLE henv;
   SQLHANDLE hdbc;
   SQLHANDLE hstmt;
};


int ODBCError(DBHandles *d, std::string &returnString);
void ConnectDB2(DBHandles *d, char *username, char *password, char *serverName);
void ConnectDB(DBHandles *d);
void DisconnectDB (DBHandles *d);
void CreateStatement (DBHandles *d);
void CloseStatement (DBHandles *d);
bool CheckTableExists(DBHandles *d, const char *sqlCommand, const char *tableName);
void CreateTable (DBHandles *d, char *sqlCommand, char *indexSQL, char *symbolTableName);
long GetNumRows(DBHandles *d, const char *symbolTableName, const char *beginDate, const char *endDate);

#endif