#include <stdio.h>
#include <string>
#include <algorithm>
#include "windows.h"

#include <boost/unordered_set.hpp>

#include "option.h"

#include "datautil.h"
#include "data.h"

extern Options o;
extern FILE *gLogFile;

std::string getSymbolTableName(std::string symbol)
{
   std::string newString(symbol);

   std::replace(newString.begin(), newString.end(), ' ', '_');
   std::replace(newString.begin(), newString.end(), '#', '_');
   std::replace(newString.begin(), newString.end(), '@', '_');
   std::replace(newString.begin(), newString.end(), '.', '_');
   std::replace(newString.begin(), newString.end(), ',', '_');

   std::string tableSuffix;
   if (o.useInterval == true) 
   {
      tableSuffix = "_INT";
   } 
   else if (o.isOption == true && o.useDaily == true)
   {
      tableSuffix = "_OPT_DAILY";
   }
   else if (o.isOption == true) 
   {
      tableSuffix = "_OPT";
   } 
   else if (o.isFutures == true)
   {
      tableSuffix = "_FUT";
   }
   else if (o.useDaily == true)
   {
      tableSuffix = "_DAILY";
   }

   if (tableSuffix.empty() == false) {
      newString += tableSuffix;
   }

   return newString;
}

void getSocket(SOCKET *s) 
{
   //set up TCPIP connection
   *s = socket(AF_INET,SOCK_STREAM,0);                  // Create a socket
   if (*s == INVALID_SOCKET) {
      WriteLog("Unable to create a socket\n");
   }
   struct sockaddr_in addr;
   addr.sin_family = AF_INET;                           // set the address to 127.0.0.1:5009
   addr.sin_port = htons(9100);
   addr.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
   memset(&addr.sin_zero, 0, sizeof(addr.sin_zero));
   int rc = connect(*s,(struct sockaddr*)&addr,sizeof(addr));
   if (rc != 0) {
      WriteLog("Fuck, I can't connect!");
      exit(-1);
   }
}

void getOptionFlags(int argc, char *argv[], Options &o) 
{
   //get all the options
   char currentSymbol[2048];
   scan_option(argc, argv, "-sym %s", currentSymbol);
   o.gSymbol = currentSymbol;

   o.isOption = scan_option(argc, argv, "-o") != 0;

   o.isFutures = scan_option(argc, argv, "-futures") != 0;
   o.useDB = scan_option(argc, argv, "-nodb") == 0;
   //3 options, either interval, ticks or numdays
   o.useInterval = scan_option(argc, argv, "-int %d", &o.interval) != 0;
   //if use interval, then specify something other than 1 second interval
   o.onlyTables = scan_option(argc, argv, "-onlyTables") != 0;

   //specify start and end dates
   memset(o.startDate, 0, sizeof(o.startDate));
   memset(o.endDate, 0, sizeof(o.endDate));

   char tempStartDate[64], tempEndDate[64];
   o.useDates = scan_option(argc, argv, "-d1 %s", tempStartDate) != 0;
   scan_option(argc, argv, "-d2 %s", tempEndDate);

   if (o.useDates == true) 
   {
      //convert from YYYY-mm-dd to YYYYmmdd
      int year, month, day;
      sscanf(tempStartDate, "%d-%d-%d", &year, &month, &day);
      sprintf(o.startDate, "%d%02d%02d", year, month, day);

      sscanf(tempEndDate, "%d-%d-%d", &year, &month, &day);
      sprintf(o.endDate, "%d%02d%02d", year, month, day);
   }

   o.useTicks = scan_option(argc, argv, "-tick") != 0;
   o.useDaily = scan_option(argc, argv, "-daily") != 0;

   if (o.useTicks == true && o.useDaily == true)
   {
      WriteLog("Cannot set both -tick and -daily");
      exit(-1);

   }
   else if (o.useTicks == true && o.useInterval == true)
   {
      WriteLog("Cannot set both -tick and -int");
      exit(-1);
   }
   else if (o.useInterval == true && o.useDaily == true)
   {
      WriteLog("Cannot set both -int and -daily");
      exit(-1);
   }

   o.useNumDays = scan_option(argc, argv, "-numDays %d", &o.numDays) != 0;

   o.useContract = scan_option(argc, argv, "-useContract") != 0;

   //set a single hardcoded table name so that I can dump it into something like correlate
   o.useHardcodedTable = scan_option(argc, argv, "-table %s", o.hardcodedTable) != 0;

   strcpy(o.username, "stocks");
   strcpy(o.password, "stocks");
   strcpy(o.serverName, "VM1");
   o.specifyUsername = (scan_option(argc, argv, "-username %s", o.username) != 0);
   o.specifyPassword = (scan_option(argc, argv, "-password %s", o.password) != 0);
   scan_option(argc, argv, "-serverName %s", o.serverName);
}

void WriteLog(char *fmt, ... )
{
   va_list argptr;
   va_start(argptr,fmt);
   vfprintf(stdout, fmt, argptr);
   vfprintf(gLogFile, fmt, argptr);
   va_end(argptr);
   fflush(gLogFile);
}


