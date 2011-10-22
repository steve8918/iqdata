#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <windows.h>

#include <set>
#include <map>

#include "unicode\datefmt.h"
#include "unicode\gregocal.h"

char *getSymbolTableName(char *symbol);
#define BUFFER_SIZE 50000
#define FUTURES_SYMBOL_LENGTH 32
int getData(char *symbol, char *symbolTableName);

extern char *gSqlCommand;
extern char *gPrepareSQL;
extern char *gIndexSQL;
extern char *createTransIndex;
extern char *createIntIndex;
extern bool useDaily;

/*
'F', "January"
'G', "February"
'H', "March"
'J', "April"
'K', "May"
'M', "June"
'N', "July"
'Q', "August"
'U', "September"
'V', "October"
'X', "November"
'Z', "December"
*/

/*
C: H,K,N,U,Z
W: H,K,N,U,Z
O: H,K,N,U,Z
S: F,H,K,N,Q,U,X
GC: G,J,M,Q,V,Z
SI: H,K,N,U,Z
*/

char *monthCodes = "FGHJKMNQUVXZ";
char *cornContract = "HKNUZ";
char *soybeanContract = "FHKNQUX";
char *goldContract = "FGHJKMNQUVXZ";
char *crudeContract = "FGHJKMNQUVXZ";

std::map<std::string, char *> symbolContractMap;

char *createFuturesTickTable = 
"CREATE TABLE [dbo].[%s]("
"   [id] [bigint] IDENTITY(1,1) NOT NULL,"
"   [contract] [varchar](32) NOT NULL,"
"   [tickId] [int] NOT NULL,"
"   [transactionTime] [datetime] NOT NULL,"
"   [last] [decimal](18, 5) NOT NULL,"
"   [lastSize] [bigint] NOT NULL,"
"   [totalVol] [bigint] NOT NULL,"
"   [bid] [decimal](18, 5) NOT NULL,"
"   [ask] [decimal](18, 5) NOT NULL,"
"   [bidSize] [bigint] NOT NULL,"
"   [askSize] [bigint] NOT NULL,"
" CONSTRAINT [PK_%s_TRANS_TICK_ID] PRIMARY KEY CLUSTERED "
"("
"	[transactionTime] ASC,"
"	[contract] ASC,"
"	[tickId] ASC"
")WITH (IGNORE_DUP_KEY = OFF) ON [PRIMARY]"
") ON [PRIMARY]";

char *futuresTickPrepareSQL = "INSERT INTO %s (tickId, transactionTime, last, lastSize, totalVol, bid, ask, bidSize, askSize, contract)"
                       "values (?,?, ?, ?, ?, ?, ?, ?, ?, ?)";

char *createFuturesDailyTable = 
"CREATE TABLE [dbo].[%s]("
"   [id] [bigint] IDENTITY(1,1) NOT NULL,"
"   [contract] [varchar](32) NOT NULL,"
"   [transactionTime] [datetime] NOT NULL,"
"   [highPrice] [decimal](18, 5) NOT NULL,"
"   [lowPrice] [decimal](18, 5) NOT NULL,"
"   [openPrice] [decimal](18, 5) NOT NULL,"
"   [closePrice] [decimal](18, 5) NOT NULL,"
"   [totalVol] [bigint] NOT NULL,"
"   [openInterest] [int] NOT NULL,"
" CONSTRAINT [PK_%s_ID] PRIMARY KEY CLUSTERED "
"("
"   [id] ASC"
")WITH (IGNORE_DUP_KEY = OFF) ON [PRIMARY]"
") ON [PRIMARY]";

char *futuresDailyPrepareSQL = "INSERT INTO %s (transactionTime, highPrice, lowPrice, openPrice, closePrice, totalVol, openInterest, contract) "
                       "values (?, ?, ?, ?, ?, ?, ?, ?)";

char *createFuturesDailyIndex = 
"CREATE UNIQUE NONCLUSTERED INDEX [IDX_$s_TRANS] ON [dbo].[%s] "
"("
"	[contract] ASC,"
"	[transactionTime] ASC"
")WITH (SORT_IN_TEMPDB = OFF, DROP_EXISTING = OFF, IGNORE_DUP_KEY = OFF, ONLINE = OFF) ON [PRIMARY]";

void initFuturesContracts()
{
   symbolContractMap["@S"] = soybeanContract;
   symbolContractMap["@C"] = cornContract;
   symbolContractMap["@W"] = cornContract;
   symbolContractMap["@O"] = cornContract;
   symbolContractMap["QSI"] = cornContract;
   symbolContractMap["QGC"] = goldContract;
   symbolContractMap["QCL"] = goldContract;
}

void extractFutures(char *symbol, int year, int month, char **futuresList, int *count) 
{
}

/*
bool getFuturesData(char *symbol) 
{
   initFuturesContracts();
   char *contracts = symbolContractMap[symbol];
   printf("contracts = %s\n", contracts);

   char *symbolTableName = getSymbolTableName(symbol);

   UErrorCode status = U_ZERO_ERROR;
   GregorianCalendar* gc = new GregorianCalendar(status);

   UDate now = gc->getNow();
   int year = gc->get(UCAL_YEAR, status);
   year = year % 2000;
   int month = gc->get(UCAL_MONTH, status);
   char currentMonthCode = monthCodes[month];
   printf("current year = %d month = %c\n", year, currentMonthCode);

   printf("current code = %s%c%d\n", symbol, monthCodes[month],year);

   //for current year
   for (unsigned int i = 0; i < strlen(contracts); i++)
   {
      if (currentMonthCode > contracts[i])
         continue;
      printf("symbol = %s%c%d\n", symbol, contracts[i], year);
   }

   //for next year
   for (unsigned int i = 0; i < strlen(contracts); i++)
   {
      printf("symbol = %s%c%d\n", symbol, contracts[i], year + 1);
   }

   exit(0);


   //extractFutures(optionsBuffer, &optionsList, &count);
   //printf("Found %d options for %s\n", count, symbol);

   //char (*optionsPtr)[OPTION_SYMBOL_LENGTH] = (char (*)[OPTION_SYMBOL_LENGTH]) optionsList;

   if (useDaily == true)
   {
      gSqlCommand = createFuturesDailyTable;
      gPrepareSQL = futuresDailyPrepareSQL;
      gIndexSQL = createFuturesDailyIndex;
   }
   else
   {
      gSqlCommand = createFuturesTickTable;
      gPrepareSQL = futuresTickPrepareSQL;
      gIndexSQL = createTransIndex;
   }

   for (int i = 0; i < count; i++) {

      getData((char *)optionsPtr, symbolTableName);

      optionsPtr++;
   }
   free(optionsList);


   //get all the symbols for the option

   //for each option get the data and write it into the _OPT table
   return true;
}
*/

