#include "account.h"

Account::Account(){

}
Account::~Account(){

}

int Account::DoUserRegister(Register &register_,Db &db,int playerId){
	char sql[256];
	time_t now_time;
	now_time = time(NULL);
	
	int forbid_login_time = 0;

	char nick[48] = "\0";
	//nick.assign(register_.nick.name,register_.nick.len); 
	int a = register_.nick.len;
	memcpy(nick,register_.nick.name,register_.nick.len); 
 
	char phone[12] = "\0";
	//phone.assign(register_.phone,register_.phone_len); 
	memcpy(phone,register_.phone,register_.phone_len); 

	char address_1[512] = "\0";
	memcpy(address_1,register_.address1,register_.address1_len);

	char address_2[512] = "\0";
	memcpy(address_2,register_.address2,register_.address2_len);

	char address_3[512] = "\0";
	memcpy(address_3,register_.address3,register_.address3_len);

	sprintf_s(sql, "insert into playerInfo values(%d, \'%s\', %d, %d, %d, \'%s\', \'%s\', \'%s\', \'%s\');", playerId,nick,register_.age,int(now_time),forbid_login_time,phone,address_1,address_2,address_3);
	
	
	string sql_find_nick = "select player_id from playerInfo where nick = \'" + (string)nick + "\'";
	
	char sql_updatePlayerId[256];
	sprintf_s(sql_updatePlayerId,"update playerMaxId set player_max_id = %d",playerId);

	db.ReadDb(sql_find_nick);
	if (db.nRow == 1)
	{
		db.freeTableData();
		return kSameNick;
	}else{
		db.freeTableData();
		db.WriteDb(sql);
		db.WriteDb(sql_updatePlayerId);
	}
	return kSucess;
}

int Account::DoUserLogin(Login& login,Db& db,UserRecord& userRecord){
	char sql[256];
	const int maxArray = 256;
	if ( login.phone_len > maxArray )
	{
		assert(false);
		return kError;
	}

	string phone = "";
	phone.assign(login.phone,login.phone_len);

	sprintf_s(sql,"select * from playerInfo where  phone = \"%s\"",phone.c_str());
	db.ReadDb(sql);
	if (db.nRow > 0)
	{
		time_t now_time;
		now_time = time(NULL);
		userRecord.login_time = (uint32_t)now_time;
		
		string str_player_id = db.pazResult[db.nCol];
		userRecord.user_id = atoi(str_player_id.c_str());

		string nick = db.pazResult[db.nCol+1];
		userRecord.nick.len = strlen(nick.c_str());
		memcpy(userRecord.nick.name,nick.c_str(),userRecord.nick.len);
		 
		string str_age = db.pazResult[db.nCol+2];
		userRecord.age = atoi(str_age.c_str());

		string str_forbid_login_time = db.pazResult[db.nCol + 3];
		userRecord.forbid_login_time = atoi(str_forbid_login_time.c_str());

		string str_phone = db.pazResult[db.nCol + 5];
		memcpy(userRecord.phone,str_phone.c_str(),strlen(str_phone.c_str()));

		string str_address_1 = db.pazResult[db.nCol + 6];
		userRecord.address1_len = str_address_1.length();
		memcpy(userRecord.address1,str_address_1.c_str(),userRecord.address1_len);

		string str_address_2 = db.pazResult[db.nCol + 7];
		userRecord.address2_len = str_address_2.length();
		memcpy(userRecord.address2,str_address_2.c_str(),userRecord.address2_len);

		string str_address_3 = db.pazResult[db.nCol + 8];
		userRecord.address3_len = str_address_3.length();
		memcpy(userRecord.address3,str_address_3.c_str(),userRecord.address3_len);

		db.freeTableData();
	}else{
		return kUserOrPasswdError;
	}
	
	return kSucess;
}

void Account::DoIsUidExist(){

}
