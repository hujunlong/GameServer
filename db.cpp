#include "db.h"

 Db::Db(){
	 dbname = "data.db";
	 db = 0;
	 pazResult = nullptr;
 }


  Db::~Db(){
	  CloseDb();
  }


  bool Db::OpenDb()
  {
	  int result = sqlite3_open(dbname.c_str(),&db);
	  if (0 != result)
	  {
		  return false; 
	  }
	  return true;
  }


  bool Db::CloseDb()
  {
	  int result = sqlite3_close(db); 
	  if (0 != result)
	  {
		  return false;
	  }
	   return true;
  }


  bool Db::CreateTable(std::string sql){
	  char *errmsg;
	  int result = sqlite3_exec(db,sql.c_str(),nullptr,nullptr,&errmsg);
	  if (result != 0)
	  {
		  std::cout<<"error sql:"<<sql<<endl<<"exec error Message:"<<errmsg<<endl;
		  return false;
	  }
	  return true;
  }

  bool Db::WriteDb(std::string sql){
	  g_mutex.lock();
	  char *errmsg;
	  int result = sqlite3_exec(db,sql.c_str(),nullptr,nullptr,&errmsg);
	  g_mutex.unlock();
	  if (result != 0)
	  {
		  cout<<"error sql:"<<sql<<endl<<"exec error Message:"<<errmsg<<endl;
		  return false;
	  } 
	  return true;
  }

  
  bool Db::ReadDb(std::string sql){
	 char *errmsg;
	 g_mutex.lock();
	 int result = sqlite3_get_table(db, sql.c_str(), &pazResult, &nRow, &nCol, &errmsg);
	 g_mutex.unlock();
	 if (result != 0)
	 {
		 cout<<"error sql:"<<sql<<endl<<"exec error Message:"<<errmsg<<endl;
		 return false;
	 }

//	 int nIndex = nCol;
//	 string strOut;
// 	 for(int i=0;i<nRow;i++)
// 	 {
// 		 for(int j=0;j<nCol;j++)
// 		 {
// 			 strOut+=pazResult[j];
// 			 strOut+=":";
// 			 strOut+=pazResult[nIndex];
// 			 strOut+="\n";
// 			 ++nIndex;
// 		 }
// 	 }
//	 cout<<"str:"<<strOut<<endl;

	 return true;
  }

  void Db::freeTableData(){
	  g_mutex.lock();
	  sqlite3_free_table(pazResult);
	  g_mutex.unlock();
  }


