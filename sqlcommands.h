#ifndef SQLPTRS
#define SQLPTRS

char *createTickTable = 
"CREATE TABLE [dbo].[%s]("
"   [id] [bigint] IDENTITY(1,1) NOT NULL,"
"   [tickId] [int] NOT NULL,"
"   [transactionTime] [datetime] NOT NULL,"
"   [last] [decimal](20, 8) NOT NULL,"
"   [lastSize] [bigint] NOT NULL,"
"   [totalVol] [bigint] NOT NULL,"
"   [bid] [decimal](20, 8) NOT NULL,"
"   [ask] [decimal](20, 8) NOT NULL,"
"   [bidSize] [bigint] NOT NULL,"
"   [askSize] [bigint] NOT NULL,"
" CONSTRAINT [PK_%s_TRANS_TICK_ID] PRIMARY KEY CLUSTERED "
"("
"	[transactionTime] ASC,"
"	[tickId] ASC"
")WITH (IGNORE_DUP_KEY = OFF) ON [PRIMARY]"
") ON [PRIMARY]";

char *createIntTable = 
"CREATE TABLE [dbo].[%s]("
"   [id] [bigint] IDENTITY(1,1) NOT NULL,"
"   [transactionTime] [datetime] NOT NULL,"
"   [openPrice] [decimal](20, 8) NOT NULL,"
"   [highPrice] [decimal](20, 8) NOT NULL,"
"   [lowPrice] [decimal](20, 8) NOT NULL,"
"   [closePrice] [decimal](20, 8) NOT NULL,"
"   [totalVol] [bigint] NOT NULL,"
"   [vol] [int] NOT NULL,"
" CONSTRAINT [PK_%s_ID] PRIMARY KEY CLUSTERED "
"("
"   [transactionTime] ASC"
")WITH (IGNORE_DUP_KEY = OFF) ON [PRIMARY]"
") ON [PRIMARY]";

char *createDailyTable = 
"CREATE TABLE [dbo].[%s]("
"   [id] [bigint] IDENTITY(1,1) NOT NULL,"
"   [transactionTime] [datetime] NOT NULL,"
"   [openPrice] [decimal](20, 8) NOT NULL,"
"   [highPrice] [decimal](20, 8) NOT NULL,"
"   [lowPrice] [decimal](20, 8) NOT NULL,"
"   [closePrice] [decimal](20, 8) NOT NULL,"
"   [totalVol] [bigint] NOT NULL,"
"   [vol] [int] NOT NULL,"
"   [PP] [decimal](20, 8),"
"   [R1] [decimal](20, 8),"
"   [R2] [decimal](20, 8),"
"   [R3] [decimal](20, 8),"
"   [S1] [decimal](20, 8),"
"   [S2] [decimal](20, 8),"
"   [S3] [decimal](20, 8),"
" CONSTRAINT [PK_%s_ID] PRIMARY KEY CLUSTERED "
"("
"   [transactionTime] ASC"
")WITH (IGNORE_DUP_KEY = OFF) ON [PRIMARY]"
") ON [PRIMARY]";

char *createOptionTickTable = 
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
"	[tickId] ASC"
")WITH (IGNORE_DUP_KEY = OFF) ON [PRIMARY]"
") ON [PRIMARY]";

char *createOptionDailyTable = 
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
"   [transactionTime] ASC"
")WITH (IGNORE_DUP_KEY = OFF) ON [PRIMARY]"
") ON [PRIMARY]";

char *createTransIndex = 
"CREATE NONCLUSTERED INDEX [IX_%s_TRANS] ON [dbo].[%s] "
"("
"	[transactionTime] DESC"
")WITH (SORT_IN_TEMPDB = OFF, DROP_EXISTING = OFF, IGNORE_DUP_KEY = OFF, ONLINE = OFF) ON [PRIMARY]";

char *createOptTickIndex = 
"CREATE NONCLUSTERED INDEX [IDX_%s_TRANS_CONTRACT] ON [dbo].[%s] "
"("
"	[transactionTime] ASC,"
"	[contract] ASC"
")INCLUDE ( [tickId]) WITH (SORT_IN_TEMPDB = OFF, DROP_EXISTING = OFF, IGNORE_DUP_KEY = OFF, ONLINE = OFF) ON [PRIMARY]";

char *createOptDailyIndex = 
"CREATE UNIQUE NONCLUSTERED INDEX [IDX_%s_TRANS_CONTRACT] ON [dbo].[%s] "
"("
"	[transactionTime] ASC,"
"	[contract] ASC"
")WITH (SORT_IN_TEMPDB = OFF, DROP_EXISTING = OFF, IGNORE_DUP_KEY = OFF, ONLINE = OFF) ON [PRIMARY]";

char *tickInsertSQL = "INSERT INTO %s (tickId, transactionTime, last, lastSize, totalVol, bid, ask, bidSize, askSize) "
                       "values (?, ?, ?, ?, ?, ?, ?, ?, ?)";

char *intInsertSQL = "INSERT INTO %s (transactionTime, highPrice, lowPrice, openPrice, closePrice, totalVol, vol) "
                       "values (?, ?, ?, ?, ?, ?, ?)";

char *intContractInsertSQL = "INSERT INTO %s (transactionTime, highPrice, lowPrice, openPrice, closePrice, totalVol, vol, contract) "
                       "values (?, ?, ?, ?, ?, ?, ?, ?)";

char *optTickInsertSQL = "INSERT INTO %s (tickId, transactionTime, last, lastSize, totalVol, bid, ask, bidSize, askSize, contract)"
                       "values (?,?, ?, ?, ?, ?, ?, ?, ?, ?)";

char *optDailyInsertSQL = "INSERT INTO %s (transactionTime, highPrice, lowPrice, openPrice, closePrice, totalVol, openInterest, contract) "
                       "values (?, ?, ?, ?, ?, ?, ?, ?)";

char *checkTableExists = 
"SELECT count(*) FROM sys.tables WHERE name = '%s'";

#endif