#ifndef DATAH
#define DATAH

#include "windows.h"
#include "sql.h"
#include "sqlext.h"

#include "odbc.h"

#include <vector>

#include "unicode\ucal.h"

#define BUFFER_SIZE 50000
#define OPTION_SYMBOL_LENGTH 32
#define SIZE 512 //SIZE used to be sizeof(tickId), etc, but because it's a pointer now, I can't use it

class DataException
{
public:
   std::string _function;
   int _lineNum;
   DataException(char *function, int lineNum):_function(function), _lineNum(lineNum){}
};

struct DataFeedData
{
   char symbol[SIZE];
   SQLINTEGER stringLength; //hacky, used for SQLBindParameter

   char timestamp[SIZE];
   char last[SIZE];
   char lastSize[SIZE];
   char totalVol[SIZE];
   char bid[SIZE];
   char ask[SIZE];
   char tickId[SIZE];
   char bidSize[SIZE];
   char askSize[SIZE];
   char basis[SIZE];

   //int data
   char highPrice[SIZE];
   char lowPrice[SIZE];
   char openPrice[SIZE];
   char closePrice[SIZE];
   char vol[SIZE];
};

struct link
{
	char *buffer;
        int numRows;
	link *next;
        char beginTimestamp[64];
        char endTimestamp[64];
};

struct dataPoint {
    int tickId;
    UDate date;
};


struct Options 
{
   bool isOption;
   bool isFutures;
   std::string gSymbol;
   bool useDB;
   char startDate[50]; //format 20090401
   char endDate[50];
   int interval;
   int numDays;

   bool useHashTable;
   bool useDates;
   bool useTicks;
   bool useNumDays;
   bool useInterval;
   bool onlyTables;
   bool useDaily;
   bool useHardcodedTable;
   bool useContract;
   bool specifyUsername;
   bool specifyPassword;

   char *gSqlCommand;
   char *gIndexSQL;
   char *gInsertSQL;

   char hardcodedTable[512];
   char username[32];
   char password[32];
   char serverName[32];

   Options() 
   {
      isOption = false;
      isFutures = false;
      useDB = true;
      interval = 60;
      numDays = 0;

      useHashTable = false;
      useDates = false;
      useTicks = false;
      useNumDays = false;
      useInterval = false;
      onlyTables = false;
      useDaily = false;
      useHardcodedTable = false;
      useContract = false;
      specifyUsername = false;
      specifyPassword = false;

      gSqlCommand = NULL;
      gIndexSQL = NULL;
      gInsertSQL = NULL;
   }
};

void extractTickData(char *input, char *timestamp, char *last, char *lastSize, 
             char *totalVol, char *bid, char* ask, char *tickId, char *bidSize,
             char *askSize, char *basis);
void extractIntData(char *input, char *timestamp, char *last, char *lastSize, char *totalVol, char *bid, char* ask, char *tickId);
link *getData(char *symbolName);
void writeData(DBHandles *d, link *head, char *symbolName, char *symbolTableName);
int writeTickToDB(DBHandles *d, char *tickId, char *timestamp, char *last, char *lastSize, char *totalVol, char *bid, char *ask, char* bidSize, char *askSize, char *symbol);
int writeIntToDB(DBHandles *d, char *tickId, char *timestamp, char *last, char *lastSize, char *totalVol, char *bid, char *ask, char *symbol);

std::vector<std::string> convertSymbolsToContracts(std::vector<std::string> &inputSymbols);

#endif