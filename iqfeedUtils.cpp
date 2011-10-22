#include <stdio.h>
#include <windows.h>
#include <string>
#include <vector>
#include <map>

#include <boost/tokenizer.hpp>

#include "iq32.h"

#include "util.h"
#include "sqlcommands.h"
#include "data.h"
#include "key.h"


extern Options o;
//global required by IQFeed
bool goodToGo = false;

bool isError(const char *oneLine, const char *symbol)
{
   if (strstr(oneLine, "!NO_DATA!") != 0) {
      WriteLog("Error with getting the data, no data\n");
      return true;
   }

   if (strstr(oneLine, "Unknown Error") != 0) {
      WriteLog("Error with getting the data, unknown error\n");
      return true;
   }

   if (strstr(oneLine, "SYNTAX_ERROR") != 0) {
      WriteLog("Error with getting the data, syntax error... maybe symbol is not all caps?\n");
      return true;
   }

   if (strstr(oneLine, "!ENDMSG!") != 0) {
      return true;
   }

   if (strstr(oneLine, "Invalid symbol") != 0) {
      WriteLog("Invalid symbol %s\n", symbol);
      return true;
   }

   if (strstr(oneLine, "E,Could not connect to History socket") != 0) {
      WriteLog("Could not connect to history socket for %s\n", symbol);
      return true;
   }

   return false;
}

void getCommand(char *command, const char *symbolName, Options &options) 
{
   if (options.useTicks == true && options.useDates == true) 
   {
      //specified tick data between 2 dates
      sprintf(command, "HTT,%s,%s 000000, %s 235959\r\n", symbolName, options.startDate, options.endDate);
   } 
   else if (options.useInterval == true && options.useDates == true) 
   {
      //specified interval data between 2 dates
      sprintf(command, "HIT,%s,%d,%s 000000, %s 235959,,,,1\r\n", symbolName, options.interval, options.startDate, options.endDate);
   } 
   else if (options.useTicks == true && options.useNumDays == true) 
   {
      //specifies number of days of tick data
      sprintf(command, "HTD,%s,%d\r\n", symbolName, options.numDays);
   } 
   else if (options.useInterval == true && options.useNumDays == true) 
   {
      //specifies the number of days to retrieve interval data
      sprintf(command, "HID,%s,%d,%d,,,,1\r\n", symbolName, options.interval, options.numDays);
   }
   else if (options.useDaily == true)
   {
      sprintf(command, "HDX,%s,%d\r\n", symbolName, options.numDays);
   }
}

void SetSQLPointers(Options &o)
{
   if (o.isOption || o.isFutures)
   {
      if (o.useDaily == true)
      {
         o.gSqlCommand = createOptionDailyTable;
         o.gInsertSQL = optDailyInsertSQL;
         o.gIndexSQL = createOptDailyIndex;
      }
      else
      {
         o.gSqlCommand = createOptionTickTable;
         o.gInsertSQL = optTickInsertSQL;
         o.gIndexSQL = createOptTickIndex;
      }
   }
   else
   {
      if (o.useTicks == true) {
         o.gSqlCommand = createTickTable;
         if (o.useContract == true)
            o.gInsertSQL = optTickInsertSQL;
         else
            o.gInsertSQL = tickInsertSQL;
         o.gIndexSQL = createTransIndex;

      } 
      else if (o.useInterval == true) 
      {
         o.gSqlCommand = createIntTable;
         o.gInsertSQL = intInsertSQL;
         o.gIndexSQL = createTransIndex;
      }
      else if (o.useDaily == true)
      {
         o.gSqlCommand = createDailyTable;
         if (o.useContract == true)
            o.gInsertSQL = optDailyInsertSQL;
         else
            o.gInsertSQL = intInsertSQL;
         o.gIndexSQL = createTransIndex;
      }
   }
}

// Receives information from iqfeed.
void __stdcall callback(int x, int y)
{
   goodToGo = (x==0 && y==0);
   printf("x = %d y = %d\n", x, y);

   if (x && y) {
      WriteLog("IQFeed delivered close message\n");
      RemoveClientApp(NULL);
      exit(0);
   }
}

std::vector<std::string> convertSymbolsToContracts(std::vector<std::string> &inputSymbols, std::map<std::string, std::string> &contractMap)
{
   char optionsBuffer[BUFFER_SIZE];

   std::vector<std::string> contractList;

   std::vector<std::string>::iterator symbolIter;

   SOCKET s;
   GetSocket(&s);

   for (symbolIter = inputSymbols.begin(); symbolIter != inputSymbols.end(); ++symbolIter)
   {
      char command[1000];
      const char *symbol = (*symbolIter).c_str();
      if (o.isOption == true)
         sprintf(command, "OEA,%s;\r\n", symbol);
      else 
         sprintf(command, "CFU,%s,,9012,4\r\n", symbol);

      send(s, command, (int) strlen(command), 0);
      int rc = recv(s, optionsBuffer, BUFFER_SIZE-1, 0);

      std::string optionsString = optionsBuffer;

      boost::char_separator<char> sep(",");

      boost::tokenizer< boost::char_separator<char> > tokens(optionsString, sep);

      // Create a list from the tokens
      std::vector<std::string> currentContractList(tokens.begin(), tokens.end());

      std::vector<std::string>::iterator currIter;

      int numContracts = 0;
      for (currIter = currentContractList.begin(); currIter != currentContractList.end(); ++currIter)
      {
         if (isError((*currIter).c_str(), symbol) == true)
            break;

         contractList.push_back(*currIter);
         contractMap.insert(std::pair<std::string, std::string>(*currIter, *symbolIter));
         numContracts++;
      }

      printf("Found %d options for %s\n", numContracts, symbol);
   }

   closesocket(s);

   return contractList;
}

void InitDataFeed()
{
   SetCallbackFunction(callback);
   RegisterClientApp(NULL, iqkey, "1.0", "1.0");

   int maxAttempts = 100;
   int attempt = 0;
   while (!goodToGo && attempt++ <= maxAttempts)
   {
      WriteLog("not goodToGo yet, sleeping 1000...\n");
      Sleep(1000);
   }

}
