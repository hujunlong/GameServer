#pragma once
#include "common.h"
#include "db.h"
#include <time.h>
#include <assert.h>

class Account
{
public:
	Account();
	~Account();

	int DoUserRegister(Register &register_ ,Db &db,int playerId);
	
	int DoUserLogin(Login& login,Db& db,UserRecord& userRecord);

	void DoIsUidExist();

private:

};

 