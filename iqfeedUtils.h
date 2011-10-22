#include <vector>
#include <string>

void __stdcall callback(int x, int y);
void SetSQLPointers(Options &options);
void getCommand(char *command, const char *symbolName, Options &options);
bool isError(const char *oneLine, const char *symbol);
std::vector<std::string> convertSymbolsToContracts(std::vector<std::string> &inputSymbols, std::map<std::string, std::string> &contractMap);
void InitDataFeed();
