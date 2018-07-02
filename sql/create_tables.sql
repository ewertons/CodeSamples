CREATE TABLE SystemInfo
(
	Id varchar(44) PRIMARY KEY,
	OsName varchar(255),
	OsVersion varchar(255),
	Architecture varchar(32),
	Processors varchar(8000),
	Memory int,
	Packages varchar(max)
)

CREATE TABLE BuildInfo
(
	Id int NOT NULL IDENTITY(1,1) PRIMARY KEY,
	TimeStamp datetime,
	Command varchar(255),
	Result int,
	Files varchar(max),
	SystemId varchar(44)
)