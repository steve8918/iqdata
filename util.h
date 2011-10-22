#include <string>

#include "data.h"

struct Options;

void WriteLog(char *fmt, ... );
void GetOptionFlags(int argc, char *argv[], Options &o);
void GetSocket(SOCKET *s);
std::string GetSymbolTableName(std::string symbol, Options &o);
void ExtractIntData(char *input, DataFeedData *df);
void ExtractTickData(char *input, DataFeedData *df);
void ExtractTimestamp(char *input, char *timestamp);

