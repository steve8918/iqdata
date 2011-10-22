#include <stdio.h>
#include "windows.h"
#include "sql.h"
#include "sqlext.h"

#include <string>

#include "data.h"
#include "util.h"
#include "odbc.h"

int ODBCError(DBHandles *d, std::string &returnString)
{
   SQLRETURN r;
   SQLCHAR szErrorMsg[1024];
   SQLTCHAR szSqlState[6];
   SQLINTEGER fNativeError;
   SQLSMALLINT cbErrorMsgMax = sizeof(szErrorMsg);
   SQLSMALLINT cbErrorMsg;

   if (d->hstmt != 0) 
   {
      r = SQLGetDiagRec(SQL_HANDLE_STMT, d->hstmt, 1, szSqlState, &fNativeError, szErrorMsg, cbErrorMsgMax, &cbErrorMsg);
   }
   else if (d->hdbc != 0) 
   {
      r = SQLGetDiagRec(SQL_HANDLE_DBC, d->hdbc, 1, szSqlState, &fNativeError, szErrorMsg, cbErrorMsgMax, &cbErrorMsg);
   }
   else 
   {
      r = SQLGetDiagRec(SQL_HANDLE_ENV, d->henv, 1, szSqlState, &fNativeError, szErrorMsg, cbErrorMsgMax, &cbErrorMsg);
   }

   returnString = (char *) szErrorMsg;

   return fNativeError;
}

void ConnectDB2(DBHandles *d, char *username, char *password, char *serverName) 
{
   SQLRETURN r;

   if (!SQL_SUCCEEDED(r = SQLAllocHandle(SQL_HANDLE_ENV, NULL, &d->henv)))
      throw DataException(__FILE__, __LINE__);

   // This is an ODBC v3 application
   if (!SQL_SUCCEEDED(r = SQLSetEnvAttr(d->henv, SQL_ATTR_ODBC_VERSION, (SQLPOINTER) SQL_OV_ODBC3, 0)))
      throw DataException(__FILE__, __LINE__);

   if (!SQL_SUCCEEDED(r = SQLAllocHandle(SQL_HANDLE_DBC, d->henv, &d->hdbc)))
      throw DataException(__FILE__, __LINE__);

   if (!SQL_SUCCEEDED(r = SQLSetConnectAttr(d->hdbc, SQL_ATTR_AUTOCOMMIT, (SQLPOINTER) SQL_AUTOCOMMIT_OFF, SQL_IS_INTEGER)))
      throw DataException(__FILE__, __LINE__);

/*
   r = SQLSetConnectAttr(hdbc, SQL_ATTR_TXN_ISOLATION, (SQLPOINTER) SQL_TXN_READ_UNCOMMITTED, SQL_IS_UINTEGER);
   if (r != SQL_SUCCESS && r!=SQL_SUCCESS_WITH_INFO) {
      ODBCError(henv, NULL, NULL, NULL);
      goto gotoError;
   }
*/
   
   char szConnStrIn[256];
   sprintf(szConnStrIn, "Driver={SQL Server};SERVER=%s;UID=%s;PWD=%s", serverName, username, password);

   if (!SQL_SUCCEEDED(r = SQLDriverConnect(d->hdbc, NULL, (SQLTCHAR *) szConnStrIn, SQL_NTS, NULL, 0, NULL, SQL_DRIVER_NOPROMPT)))
      throw DataException(__FILE__, __LINE__);
   
   return;
}

void ConnectDB(DBHandles *d)
{
   SQLRETURN r;

   if (!SQL_SUCCEEDED(r = SQLAllocHandle(SQL_HANDLE_ENV, NULL, &d->henv)))
      throw DataException(__FILE__, __LINE__);

   // This is an ODBC v3 application
   if (!SQL_SUCCEEDED(r = SQLSetEnvAttr(d->henv, SQL_ATTR_ODBC_VERSION, (SQLPOINTER) SQL_OV_ODBC3, 0)))
      throw DataException(__FILE__, __LINE__);

   if (!SQL_SUCCEEDED(r = SQLAllocHandle(SQL_HANDLE_DBC, d->henv, &d->hdbc)))
      throw DataException(__FILE__, __LINE__);

   if (!SQL_SUCCEEDED(r = SQLSetConnectAttr(d->hdbc, SQL_ATTR_AUTOCOMMIT, (SQLPOINTER) SQL_AUTOCOMMIT_OFF, SQL_IS_INTEGER)))
      throw DataException(__FILE__, __LINE__);

   char szConnStrIn[256];
   sprintf(szConnStrIn, "Driver={SQL Server};SERVER=%s;UID=%s;PWD=%s", d->serverName.c_str(), d->username.c_str(), d->password.c_str());

   if (!SQL_SUCCEEDED(r = SQLDriverConnect(d->hdbc, NULL, (SQLTCHAR *) szConnStrIn, SQL_NTS, NULL, 0, NULL, SQL_DRIVER_NOPROMPT)))
      throw DataException(__FILE__, __LINE__);
   
   return;
}

void DisconnectDB (DBHandles *d)
{
   SQLRETURN r;

   if (!SQL_SUCCEEDED(r = SQLDisconnect(d->hdbc)))
      throw DataException(__FILE__, __LINE__);

   if (!SQL_SUCCEEDED(r = SQLFreeHandle(SQL_HANDLE_DBC, d->hdbc)))
      throw DataException(__FILE__, __LINE__);

   if (!SQL_SUCCEEDED(r = SQLFreeHandle(SQL_HANDLE_ENV, d->henv)))
      throw DataException(__FILE__, __LINE__);
   
   return;
}


void CreateStatement (DBHandles *d) 
{
   SQLRETURN r;

   if (!SQL_SUCCEEDED(r = SQLAllocHandle(SQL_HANDLE_STMT, d->hdbc, &d->hstmt)))
      throw DataException(__FILE__, __LINE__);

   return;
}


void CloseStatement (DBHandles *d) 
{
   SQLEndTran(SQL_HANDLE_DBC, d->hdbc, SQL_COMMIT);
   SQLFreeHandle(SQL_HANDLE_STMT, d->hstmt);
}


bool CheckTableExists(DBHandles *d, const char *sqlCommand, const char *tableName)
{
   char checkSQL[2048];
   int count = 0;
   SQLLEN indicator;
   SQLRETURN ret;
   std::string errorString;

   sprintf(checkSQL, sqlCommand, tableName);

   CreateStatement(d);

   if (!SQL_SUCCEEDED(ret = SQLExecDirect(d->hstmt, (SQLTCHAR *) checkSQL, SQL_NTS)))
      throw DataException(__FILE__, __LINE__);

   if (!SQL_SUCCEEDED(ret = SQLBindCol(d->hstmt, 1, SQL_INTEGER, &count, sizeof(count), &indicator)))
      throw DataException(__FILE__, __LINE__);

   if (!SQL_SUCCEEDED(ret = SQLFetch(d->hstmt)))
      throw DataException(__FILE__, __LINE__);

   CloseStatement(d);

   if (count == 1)
      return true;
   else 
      return false;
}


void CreateTable (DBHandles *d, char *sqlCommand, char *indexSQL, char *symbolTableName) 
{
   char createSQL[2048];
   SQLRETURN r;

   CreateStatement(d);

   //create the table
   sprintf(createSQL, sqlCommand, symbolTableName, symbolTableName);

   if (!SQL_SUCCEEDED(r = SQLExecDirect(d->hstmt, (SQLTCHAR *) (createSQL), SQL_NTS)))
      throw DataException(__FILE__, __LINE__);

   //create the index
   sprintf(createSQL, indexSQL, symbolTableName, symbolTableName);

   if (!SQL_SUCCEEDED(r = SQLExecDirect(d->hstmt, (SQLTCHAR *) (createSQL), SQL_NTS)))
      throw DataException(__FILE__, __LINE__);

   CloseStatement(d);
}

long GetNumRows(DBHandles *d, const char *symbolTableName, const char *beginDate, const char *endDate)
{
   char query[512];
   SQLRETURN ret = 0;
   SQLINTEGER indicator = 0;
   long numRows = 0;

   CreateStatement(d);

   sprintf(query, "SELECT sum(numTrades) from futures.dbo.DailyTrades where symbol = '%s' and transactionTime >= '%s' and "
                  "transactionTime <= '%s'", symbolTableName, beginDate, endDate);

   if (!SQL_SUCCEEDED(ret = SQLExecDirect(d->hstmt, (SQLCHAR *) query, SQL_NTS)))
      throw DataException(__FILE__, __LINE__);

   if (!SQL_SUCCEEDED(ret = SQLFetch(d->hstmt))) 
      throw DataException(__FILE__, __LINE__);

   if (!SQL_SUCCEEDED(ret = SQLGetData(d->hstmt, 1, SQL_C_LONG, &numRows, sizeof(numRows), &indicator)))
      throw DataException(__FILE__, __LINE__);

   CloseStatement (d);

   return numRows;
}

