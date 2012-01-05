#include <stdio.h>
#include "windows.h"
#include "sql.h"
#include "sqlext.h"
#include "string.h"
#include "odbc.h"

#include <boost/unordered_set.hpp>
#include <boost/tokenizer.hpp>
#include <string>
#include <vector>
#include <map>

#include "iq32.h"

#include "data.h"
#include "timeutils.h"
#include "util.h"
#include "iqfeedutils.h"

//#define CHECK_MEMLEAK

#ifdef CHECK_MEMLEAK
#include "crtdbg.h"
#endif

extern char *checkTableExists;

//char *dow30="AA,AXP,BA,BAC,C,CAT,CVX,DD,DIS,GE,GM,HD,HPQ,IBM,INTC,JNJ,JPM,KFT,KO,MCD,MMM,MRK,MSFT,PFE,PG,T,UTX,VZ,WMT,XOM";
char *dow30="AA,AXP,BA,BAC,CAT,CSCO,CVX,DD,DIS,GE,HD,HPQ,IBM,INTC,JNJ,JPM,KFT,KO,MCD,MMM,MRK,MSFT,PFE,PG,T,TRV,UTX,VZ,WMT,XOM";

GregorianCalendar* dateGC;

bool operator==(dataPoint const& p1, dataPoint const& p2)
{
    return p1.tickId == p2.tickId && p1.date == p2.date;
}

std::size_t hash_value(dataPoint const& p) 
{
    std::size_t seed = 0;
    boost::hash_combine(seed, p.tickId);
    boost::hash_combine(seed, p.date);
    return seed;
}

//global required by IQFeed
extern bool goodToGo;

Options o;
DBHandles s;

FILE *gLogFile = NULL;

void BindTickVariables(DBHandles *d, DataFeedData *df) 
{
   SQLRETURN r;

   //tickId, timestamp, last, lastSize, totalVol, bid, ask, bidSize, askSize
   if (!SQL_SUCCEEDED(r = SQLBindParameter(d->hstmt, 1, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_INTEGER, 4, 0, df->tickId, SIZE, &df->stringLength)))
      throw DataException(__FILE__, __LINE__);

   if (!SQL_SUCCEEDED(r = SQLBindParameter(d->hstmt, 2, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_TIMESTAMP, 19, 0, df->timestamp, SIZE, &df->stringLength)))
      throw DataException(__FILE__, __LINE__);

   if (!SQL_SUCCEEDED(r = SQLBindParameter(d->hstmt, 3, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_DECIMAL, 20, 8, df->last, SIZE, &df->stringLength)))
      throw DataException(__FILE__, __LINE__);

   if (!SQL_SUCCEEDED(r = SQLBindParameter(d->hstmt, 4, SQL_PARAM_INPUT, SQL_C_CHAR, -5, 8, 0, df->lastSize, SIZE, &df->stringLength)))
      throw DataException(__FILE__, __LINE__);

   if (!SQL_SUCCEEDED(r = SQLBindParameter(d->hstmt, 5, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_INTEGER, 8, 0, df->totalVol, SIZE, &df->stringLength)))
      throw DataException(__FILE__, __LINE__);

   if (!SQL_SUCCEEDED(r = SQLBindParameter(d->hstmt, 6, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_DECIMAL, 20, 8, df->bid, SIZE, &df->stringLength)))
      throw DataException(__FILE__, __LINE__);

   if (!SQL_SUCCEEDED(r = SQLBindParameter(d->hstmt, 7, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_DECIMAL, 20, 8, df->ask, SIZE, &df->stringLength)))
      throw DataException(__FILE__, __LINE__);

   if (!SQL_SUCCEEDED(r = SQLBindParameter(d->hstmt, 8, SQL_PARAM_INPUT, SQL_C_CHAR, -5, 8, 0, df->bidSize, SIZE, &df->stringLength)))
      throw DataException(__FILE__, __LINE__);

   if (!SQL_SUCCEEDED(r = SQLBindParameter(d->hstmt, 9, SQL_PARAM_INPUT, SQL_C_CHAR, -5, 8, 0, df->askSize, SIZE, &df->stringLength)))
      throw DataException(__FILE__, __LINE__);

   if (o.isOption == true || o.isFutures == true || o.useContract == true) {
      if (!SQL_SUCCEEDED(r = SQLBindParameter(d->hstmt, 10, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_CHAR, OPTION_SYMBOL_LENGTH, 0, (SQLPOINTER) df->symbol, (SQLINTEGER) strlen(df->symbol), &df->stringLength)))
         throw DataException(__FILE__, __LINE__);
   }
}

void  BindIntVariables(DBHandles *d, DataFeedData *df) 
{
   SQLRETURN r;

   if (!SQL_SUCCEEDED(r = SQLBindParameter(d->hstmt, 1, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_TIMESTAMP, 19, 0, df->timestamp, SIZE, &df->stringLength)))
      throw DataException(__FILE__, __LINE__);

   if (!SQL_SUCCEEDED(r = SQLBindParameter(d->hstmt, 2, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_DECIMAL, 20, 8, df->highPrice, SIZE, &df->stringLength)))
      throw DataException(__FILE__, __LINE__);

   if (!SQL_SUCCEEDED(r = SQLBindParameter(d->hstmt, 3, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_DECIMAL, 20, 8, df->lowPrice, SIZE, &df->stringLength)))
      throw DataException(__FILE__, __LINE__);

   if (!SQL_SUCCEEDED(r = SQLBindParameter(d->hstmt, 4, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_DECIMAL, 20, 8, df->openPrice, SIZE, &df->stringLength)))
      throw DataException(__FILE__, __LINE__);

   if (!SQL_SUCCEEDED(r = SQLBindParameter(d->hstmt, 5, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_DECIMAL, 20, 8, df->closePrice, SIZE, &df->stringLength)))
      throw DataException(__FILE__, __LINE__);

   if (!SQL_SUCCEEDED(r = SQLBindParameter(d->hstmt, 6, SQL_PARAM_INPUT, SQL_C_CHAR, -5, 8, 0, df->totalVol, SIZE, &df->stringLength)))
      throw DataException(__FILE__, __LINE__);

   if (!SQL_SUCCEEDED(r = SQLBindParameter(d->hstmt, 7, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_INTEGER, 4, 0, df->vol, SIZE, &df->stringLength)))
      throw DataException(__FILE__, __LINE__);

   if (o.isOption == true || o.isFutures == true || o.useContract == true) {
      if (!SQL_SUCCEEDED(r = SQLBindParameter(d->hstmt, 8, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_CHAR, 10, 0, (SQLPOINTER) df->symbol, (SQLINTEGER) strlen(df->symbol), &df->stringLength)))
         throw DataException(__FILE__, __LINE__);
   }
}

int WriteDataToDB(DBHandles *d, DataFeedData *df) 
{
   SQLINTEGER stringLength = SQL_NTS;
   SQLRETURN r;

   //only daily data need this, because the times are jacked up so 
   //I just manually set it to midnight
   if (o.useDaily == true)
   {
      char newTransactionTime[50];
      char dummy[50];
      sscanf(df->timestamp, "%s %s", newTransactionTime, dummy);
      sprintf(df->timestamp, "%s 00:00:00", newTransactionTime);
   }

   r = SQLExecute(d->hstmt);

   if (!SQL_SUCCEEDED(r))
   {
      std::string errorString;
      int errorNum = ODBCError(d, errorString);
      //error code denoting that the row already exists
      if (errorNum == 2601 || errorNum == 2627) 
         return 0;
      else 
         throw DataException(__FILE__, __LINE__);
   }

   return 1;
}

int WriteOneLine(DBHandles *d, char *oneLine, boost::unordered_set<dataPoint> *hash, DataFeedData *df)
{
   if (o.useTicks == true)
   {
      ExtractTickData(oneLine, df);

      //first check the hashTable to see if it already has the data
      if (o.useHashTable == true && hash != NULL)
      {
         dataPoint dp;
         dp.date = GetUDate(df->timestamp, dateGC);
         dp.tickId = atoi(df->tickId);

         if (hash->find(dp) != hash->end())
            return 0;
      }
   }
   else if (o.useInterval == true || o.useDaily == true)
   {
      ExtractIntData(oneLine, df);
   }
   else 
   {
      WriteLog("Missing useInterval or useTicks\n");
      throw DataException(__FILE__, __LINE__);
   }

   return WriteDataToDB(d, df);
}

link *GetData(std::string symbol) 
{
   SOCKET s;
   char command[1000];

   WriteLog("Getting data for %s\n", symbol.c_str());

   GetSocket(&s);

   //get the historical data
   getCommand(command, symbol.c_str(), o);

   int rc = send(s, command, (int) strlen(command), 0);
   if (rc == SOCKET_ERROR)
   {
      WriteLog("error sending data to data feed, error Code %d\n", WSAGetLastError());
      throw DataException(__FILE__, __LINE__);
   }

   char *buffer = (char *) calloc(BUFFER_SIZE, sizeof(char));
   if (buffer == NULL)
   {
      WriteLog("calloc error for buffer\n");
      throw DataException(__FILE__, __LINE__);
   }

   char *bufferPtr = buffer;
   char *begin = buffer;
   char *end = NULL;

   link *head = NULL;

   unsigned long startTime = timeGetTime(); 
   unsigned long currentTime = 0;
   unsigned long totalTime = 0;
   link *currentLink = NULL;

   int numBlocks = 0;
   int totalRows = 0;

   bool exit = false;

   while (exit == false)
   {
      char oneLine[5120];
      char beginTimestamp[128];
      char endTimestamp[128];

      rc = recv(s, bufferPtr, (int) (BUFFER_SIZE - (bufferPtr - buffer)-1), 0);
      if (rc == 0 || rc == SOCKET_ERROR) {
         WriteLog("error receiving data from data feed, error code = %d\n", WSAGetLastError());
         throw DataException(__FILE__, __LINE__);
      }
      
      if (rc > BUFFER_SIZE - (bufferPtr - buffer)-1 || rc > BUFFER_SIZE) {
         WriteLog("Something f'ed with the data received, it's bigger than it should be\n");
         throw DataException(__FILE__, __LINE__);
      }

      begin = buffer;

      int numRows = 0;

      //validate each row, looking for the end of data message
      while ((end = strchr(begin, '\n')) != NULL)
      {
         //copy the line into oneLine so that we can check
         //for any errors.  I guess I could have just checked
         //it from the line itself but whatever, easier to debug
         memset(oneLine, 0, sizeof(oneLine));
         strncpy(oneLine, begin, end-begin + 1);
         //printf("%s\n", oneLine);

         //check for an error first
         if (isError(oneLine, symbol.c_str()) == true) 
         {
            //end of data
            exit = true;
            break;
         }

         //always extract the timestamp to endTimestamp since
         //we don't know when it will end, but if it's the first
         //row of the block, then copy it to beginTimestamp
         ExtractTimestamp(oneLine, endTimestamp);
         if (numRows == 0)
            strcpy(beginTimestamp, endTimestamp);

         begin = end + 1;

         numRows++;
      }

      //if the very first line of the buffer is an !ENDMSG!, 
      //then just break out of this loop
      if (buffer == begin)
      {
         break;
      }

      //create a new link
      link *newLink = (link *) calloc(1, sizeof(link));
      if (newLink == NULL)
      {
         WriteLog("Calloc error for newLink\n");
         throw DataException(__FILE__, __LINE__);
      }

      //create a new buffer for the new block that contains only the 
      //valid data for that block
      int blockSize = begin - buffer + 1;

      newLink->buffer = (char *) calloc(blockSize, sizeof(char));
      if (newLink->buffer == NULL)
      {
         WriteLog("Calloc error for newLink->buffer\n");
         throw DataException(__FILE__, __LINE__);
      }

      //copy everything up to the beginning of begin, because
      //that is likely an uncompleted line of data
      memcpy(newLink->buffer, buffer, begin - buffer);

      strcpy(newLink->beginTimestamp, beginTimestamp);
      strcpy(newLink->endTimestamp, endTimestamp);

      if (head == NULL)
      {
         head = newLink;
         currentLink = head;
      }
      else
      {
         currentLink->next = newLink;
      }
      currentLink->numRows = numRows;
      currentLink = newLink;

      //move any other stragglers to the front of the buffer
      memmove(buffer, begin, strlen(begin));
      bufferPtr = buffer + strlen(begin);
      //clear everything else after that to prevent errors
      memset(bufferPtr, 0, BUFFER_SIZE - (bufferPtr - buffer));

      //stats gathering shit
      totalRows += numRows;
      currentTime = timeGetTime();
      totalTime = currentTime - startTime;

      numBlocks++;

      if (numBlocks % 100 == 0)
      {
         WriteLog("%s: reading rows=%d, total time = %d, rows/s = %.2f\n", 
                symbol.c_str(), totalRows, totalTime/1000, 1000.0* totalRows / totalTime);
      }
   }

   WriteLog("%s: FINAL: reading rows=%d, total time = %d, rows/s = %.2f, numBlocks = %d\n", 
          symbol.c_str(), totalRows, totalTime/1000, 1000.0* totalRows / totalTime, numBlocks);

   free(buffer);
   closesocket(s);

   return head;
}

boost::unordered_set<dataPoint> *GetDBHash(DBHandles *d, std::string symbolTableName, 
                                           char *date1, char *date2, std::string contractName)
{
   //IQFeed will give the data backwards, so we need to order the dates properly
   char *firstDate = date2, *secondDate = date1;
   std::string errorString;

   UDate udate1 = GetUDate(date1, dateGC);
   UDate udate2 = GetUDate(date2, dateGC);

   if (udate1 < udate2)
   {
      firstDate = date1;
      secondDate = date2;
   }

   WriteLog("Getting data from %s to %s\n", firstDate, secondDate);

   char query[256];
   if (o.isOption || o.isFutures)
   {
      sprintf(query, "SELECT transactiontime, tickId FROM %s WHERE transactiontime >= '%s' " 
                     "AND transactiontime <= '%s' and contract = '%s'",
         symbolTableName.c_str(), firstDate, secondDate, contractName.c_str());
   }
   else
   {
      sprintf(query, "SELECT transactiontime, tickId FROM %s WHERE transactiontime >= '%s' "
                     "AND transactiontime <= '%s'",
              symbolTableName.c_str(), firstDate, secondDate);
   }

   CreateStatement(d);

   SQLRETURN ret;
   if (!SQL_SUCCEEDED(ret = SQLExecDirect(d->hstmt, (SQLCHAR *) query, SQL_NTS)))
      throw DataException(__FILE__, __LINE__);
   
   char transactionTime[64];
   int tickId;
   SQLLEN indicator;
   boost::unordered_set<dataPoint> *hash = new boost::unordered_set<dataPoint>();

   if (!SQL_SUCCEEDED(ret = SQLBindCol(d->hstmt, 1, SQL_C_CHAR, transactionTime, sizeof(transactionTime), &indicator)))
      throw DataException(__FILE__, __LINE__);

   if (!SQL_SUCCEEDED(ret = SQLBindCol(d->hstmt, 2, SQL_INTEGER, &tickId, sizeof(tickId), &indicator)))
      throw DataException(__FILE__, __LINE__);

   int numRows = 0;
   while (SQL_SUCCEEDED(ret = SQLFetch(d->hstmt))) 
   {
      dataPoint newData;
      
      newData.date = GetUDate(transactionTime, dateGC);
      newData.tickId = tickId;
      try 
      {
         hash->insert(newData);
      }
      catch (const std::bad_alloc &)
      {
         WriteLog("Fuck ran out of memory... Let's forget about getting more data from the database and keep going\n");
         break;
      }
      numRows++;
   }
   WriteLog("Total of %d rows retrieved\n", numRows);

   CloseStatement(d);

   return hash;
}

void WriteData(DBHandles *d, link *head, std::string symbol, std::string symbolTableName, 
               boost::unordered_set<dataPoint> *hash)
{
   char sql[512];
   SQLRETURN r;
   std::string errorString;

   CreateStatement(d);

   sprintf(sql, o.gInsertSQL, symbolTableName.c_str());
   //WriteLog("%s\n", sql);
   if (!SQL_SUCCEEDED(r = SQLPrepare(d->hstmt, (SQLTCHAR *) (sql), SQL_NTS)))
      throw DataException(__FILE__, __LINE__);

   link *currentLink = head;
   char oneLine[5120];

   unsigned long startTime = timeGetTime(); 
   int totalRows = 0;
   int numSkipped = 0;
   int numWritten = 0;
   
   unsigned long totalTime = 0;

   DataFeedData df;

   if (o.useTicks == true)
      BindTickVariables(d, &df);
   else
      BindIntVariables(d, &df);

   //externally set some df parameters, hacky
   strcpy(df.symbol, symbol.c_str());
   df.stringLength = SQL_NTS;

   //iterate through each chunk in the link list
   while (currentLink != NULL)
   {
      char *begin = currentLink->buffer;
      unsigned long blockTime = timeGetTime();
      int beginBlockCount = totalRows;
      char *end = NULL;

      //iterate through each line in the buffer and 
      //write it to the database
      while ((end = strchr(begin, '\n')) != NULL) 
      {
         memset(oneLine, 0, sizeof(oneLine));
         strncpy(oneLine, begin, end-begin + 1);
         char copyOfOneLine[512];
         strcpy(copyOfOneLine, oneLine);
         //printf("%s\n", copyOfOneLine);

         if (WriteOneLine(d, oneLine, hash, &df) == 1)
            numWritten++;
         else
            numSkipped++;

         totalRows++;

         begin = end + 1;

         if (numWritten % 100 == 0)
         {
            if (!SQL_SUCCEEDED(r = SQLEndTran(SQL_HANDLE_DBC, d->hdbc, SQL_COMMIT)))
               throw DataException(__FILE__, __LINE__);
         }

         if (totalRows % 50000 == 0)
         {
            //stats gathering shit
            unsigned long currentTime = timeGetTime();
            totalTime = currentTime - startTime;

            double blockDelta = currentTime - blockTime;
            double rowDelta = totalRows - beginBlockCount;
            double currentBlockTime = blockDelta == 0 ? 0 : 1000* rowDelta/blockDelta;

            WriteLog("%s: writing total rows=%d, total time = %d, rows/s = %.2f, numWritten = %d, "
                     "numSkipped = %d, timestamp = %s\n", 
               symbol.c_str(), totalRows, totalTime/1000, currentBlockTime, numWritten, numSkipped, oneLine);
         }
      }

      link *freeLink = currentLink;
      currentLink = currentLink->next;

      free(freeLink->buffer);
      free(freeLink);
   }

   totalTime = timeGetTime() - startTime;
   WriteLog("%s: FINAL: writing total rows=%d, total time = %d, rows/s = %.2f, numWritten = %d, numSkipped = %d\n", 
          symbol.c_str(), totalRows, totalTime/1000, 1000.0 * totalRows / totalTime, numWritten, numSkipped);

   CloseStatement(d);
}


int main(int argc, char* argv[])
{
#ifdef CHECK_MEMLEAK
   _CrtSetDbgFlag ( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
#endif

   //Initialize the ICU calendar
   UErrorCode status = U_ZERO_ERROR;
   dateGC = new GregorianCalendar(status);

   //get all the command line options
   GetOptionFlags(argc, argv, o);
   s.username = o.username;
   s.password = o.password;
   s.serverName = o.serverName;

   //set up the log file
   std::string dateString = GetDateString(dateGC->getNow(), dateGC);
   std::string fileName = dateString + o.gSymbol;
   fileName = GetSymbolTableName(fileName, o);
   fileName += ".log";
   gLogFile = fopen(fileName.c_str(), "w+");
   if (gLogFile == NULL)
   {
      printf("Could not open file %s, exiting.  Sucks that I can't log this output\n", fileName.c_str());
      exit(-1);
   }

   //Initialize IQ Feed
   InitDataFeed();

   //set the SQL pointers based on the data being downloaded
   SetSQLPointers(o);

   /////////////////////////////
   // Get the symbols
   /////////////////////////////
   if (o.gSymbol.compare("dow30") == 0)
      o.gSymbol = dow30;

   boost::char_separator<char> sep(",");
   boost::tokenizer< boost::char_separator<char> > tokens(o.gSymbol, sep);

   // Create a list from the tokens
   std::vector<std::string> symbolList(tokens.begin(), tokens.end());
   std::vector<std::string> contractList;
   std::map<std::string, std::string> contractMap;

   //get all the symbols
   if (o.isOption == true || o.isFutures == true)
   {
      contractList = convertSymbolsToContracts(symbolList, contractMap);
      symbolList = contractList;
   }

   int numSymbols = symbolList.size();
   WriteLog("There are %d symbols\n", numSymbols);

   //determine if we should use the hashTable, currently only
   //for ticks and non-contract-related data
   if (o.useTicks == true)// && (o.isOption == false && o.isFutures == false))
      o.useHashTable = true;

   DBHandles s;
   s.username = o.username;
   s.serverName = o.serverName;
   s.password = o.password;

   ConnectDB(&s);

   for (int i = 0; i < numSymbols; i++) 
   {
      std::string currentSymbol = symbolList[i];

      // Get the data from the data feed and stuff into link list
      link *head = NULL;
      try 
      {
         head = GetData(currentSymbol);
      }
      catch (const DataException &d)
      {
         std::string errorString;
         ODBCError(&s, errorString);
         WriteLog("GetData error: %s\n", errorString.c_str());
         WriteLog("problem on file %s, line %d\n", d._function.c_str(), d._lineNum);
         exit(-1);
      }

      //if there's no data then move to the next symbol
      if (head == NULL)
         continue;

      //get the symbolTableName and create table if necessary
      std::string symbolTableName;
      if (o.useHardcodedTable == true)
         symbolTableName = o.hardcodedTable;
      else if (o.isOption == true || o.isFutures == true)
         symbolTableName = GetSymbolTableName(contractMap[currentSymbol], o);
      else
         symbolTableName = GetSymbolTableName(currentSymbol, o);

      if (CheckTableExists(&s, checkTableExists, symbolTableName.c_str()) == false)
      {
         CreateTable(&s, o.gSqlCommand, o.gIndexSQL, (char *) symbolTableName.c_str());
      }

      //get the last chunk for its timestamp
      //lame could do this better but it's not too expensive for now
      link *lastChunk = head;
      while (lastChunk->next != NULL)
         lastChunk = lastChunk->next;

      //write the data
      try 
      {
         boost::unordered_set<dataPoint> *hash = NULL;
         if (o.useHashTable == true)
            hash = GetDBHash(&s, symbolTableName, head->beginTimestamp, lastChunk->endTimestamp, currentSymbol);

         WriteData(&s, head, currentSymbol, symbolTableName, hash);

         delete hash;
      }
      catch (const DataException &d)
      {
         std::string errorString;
         ODBCError(&s, errorString);
         WriteLog("WriteData error: %s\n", errorString.c_str());
         WriteLog("problem on file %s, line %d\n", d._function.c_str(), d._lineNum);
         exit(-1);
      }
   }

   DisconnectDB(&s);
   RemoveClientApp(NULL);
}