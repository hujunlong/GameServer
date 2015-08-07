#pragma once

#include <iostream>
#include <string>
#include <mutex>
using namespace std;

extern "C"
{
	#include "sqlite3.h"
};

class Db
{
public:
	
	Db();

	~Db();

	bool OpenDb();
	
	bool CloseDb();
	
	bool CreateTable(std::string sql);

	bool WriteDb(std::string sql);

	bool ReadDb(std::string sql);

	void freeTableData();
	
public:
	string dbname;
	sqlite3* db;
	char **pazResult;
	int nRow;
	int nCol;
	std::mutex g_mutex;  
};