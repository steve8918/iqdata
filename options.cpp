#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <windows.h>
#include "data.h"

#include <string>
#include <vector>
#include <map>
#include <boost/tokenizer.hpp>

extern char *gSqlCommand;
extern char *gPrepareSQL;
extern char *gIndexSQL;
extern char *createTransIndex;
extern char *createIntIndex;
extern bool useDaily;
extern bool isOption;
extern std::map<std::string, std::string> contractMap;

/*
'A', "January"
'B', "February"
'C', "March"
'D', "April"
'E', "May"
'F', "June"
'G', "July"
'H', "August"
'I', "September"
'J', "October"
'K', "November"
'L', "December"

'M', "January"
'N', "February"
'O', "March"
'P', "April"
'Q', "May"
'R', "June"
'S', "July"
'T', "August"
'U', "September"
'V', "October"
'W', "November"
'X', "December"

'A', {  "5", "105", "205", "305", "405", "505", "605", "705"}
'B', { "10", "110", "210", "310", "410", "510", "610", "710"}
'C', { "15", "115", "215", "315", "415", "515", "615", "715"}
'D', { "20", "120", "220", "320", "420", "520", "620", "720"}
'E', { "25", "125", "225", "325", "425", "525", "625", "725"}
'F', { "30", "130", "230", "330", "430", "530", "630", "730"}
'G', { "35", "135", "235", "335", "435", "535", "635", "735"}
'H', { "40", "140", "240", "340", "440", "540", "640", "740"}
'I', { "45", "145", "245", "345", "445", "545", "645", "745"}
'J', { "50", "150", "250", "350", "450", "550", "650", "750"}
'K', { "55", "155", "255", "355", "455", "555", "655", "755"}
'L', { "60", "160", "260", "360", "460", "560", "660", "760"}
'M', { "65", "165", "265", "365", "465", "565", "665", "765"}
'N', { "70", "170", "270", "370", "470", "570", "670", "770"}
'O', { "75", "175", "275", "375", "475", "575", "675", "775"}
'P', { "80", "180", "280", "380", "480", "580", "680", "780"}
'Q', { "85", "185", "285", "385", "485", "585", "685", "785"}
'R', { "90", "190", "290", "390", "490", "590", "690", "790"}
'S', { "95", "195", "295", "395", "495", "595", "695", "795"}
'T', {"100", "200", "300", "400", "500", "600", "700", "800"}
'U', { "7.5", "37.5", "67.5", "97.5",  "127.5", "157.5", "187.5", "217.5"}
'V', {"12.5", "42.5", "72.5", "102.5", "132.5", "162.5", "192.5", "222.5"}
'W', {"17.5", "47.5", "77.5", "107.5", "137.5", "167.5", "197.5", "227.5"}
'X', {"22.5", "52.5", "82.5", "112.5", "142.5", "172.5", "202.5", "232.5"}
'Y', {"27.5", "57.5", "87.5", "117.5", "147.5", "177.5", "207.5", "237.5"}
'Z', {"32.5", "62.5", "92.5", "122.5", "152.5", "182.5", "212.5", "242.5"}
*/
char *month[12] = {
   "January",
   "February",
   "March",
   "April",
   "May",
   "June",
   "July",
   "August",
   "September",
   "October",
   "November",
   "December",
};



bool isError(char *oneLine, const char *symbol);


/*
bool getOptionData(char *symbol) {

   char *symbolTableName = getSymbolTableName(symbol);


   for (int i = 0; i < count; i++) {

      link *head = getData((char *)optionsPtr);

      link *ptr = head;
      while (ptr->next != NULL)
         ptr = ptr->next;

      //hash = NULL;
      //if (useTicks == true)
      //   hash = getDBData(symbolTableName, head->beginTimestamp, ptr->endTimestamp);

      writeData(head, (char *)optionsPtr, symbolTableName);

      optionsPtr++;
   }
   free(optionsList);

   //get all the symbols for the option

   //for each option get the data and write it into the _OPT table
   return true;
}
*/
