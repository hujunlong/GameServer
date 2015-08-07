#pragma once
#include "MySocket.h"
#include <cstdint>
enum //发送包
{
	kRegister = 200,
	kLogin,
	kLoginOut,
	kGetPlayerInfo,
};

enum //接收包
{
	kRegisterResult = 600,
	kLoginResult,
	kGetPlayerInfoResult,
};

enum //错误码
{
	kSucess = 0,
	kError = 100,//未知错误
	kSameNick,//相同Nick 玩家不能注册
	kUserOrPasswdError,//用户名或密码错误
	kForbidLogin,//禁止登陆
};


struct Nickname
{
	uint32_t len;
	char name[18];
};
typedef struct Nickname Nickname;

struct Register//c2s
{
	static const uint32_t type = kRegister;
	uint32_t age;
	Nickname nick;
	//uint32_t passworldLen;

	uint32_t address1_len;
	char address1[128];

	uint32_t address2_len;
	char address2[128];

	uint32_t address3_len;
	char address3[128];

	uint32_t phone_len;
	char phone[16];//电话号码
};
typedef struct Register Register;

struct RegisterResult
{
	static const uint32_t type = kRegisterResult;
	uint32_t result;//
};
typedef struct RegisterResult RegisterResult;//s2c


struct Login//c2s
{
	static const uint32_t type = kLogin;
	uint32_t phone_len;
	char phone[12];
};
typedef struct Login Login;

struct LoginResult
{
	static const uint32_t type = kLoginResult;
	uint32_t result;//
};
typedef struct LoginResult LoginResult;//s2c


struct LoginOut{
	static const uint32_t type = kLoginOut;
};
typedef struct LoginOut LoginOut;//c2s


struct UserRecord
{
	uint32_t user_id;			//用户id
	uint32_t login_time;		//登录时间
	int16_t  b_enable;			//是否允许登录
	int16_t  age;			    //性别
	uint32_t forbid_login_time; //禁止登陆时间
	Nickname nick;				//nick
	ODSocket odSocket;			//socket 
	char ip[32] ;				//ip 地址
	char phone[16];				//电话号码

	uint32_t address1_len;
	char address1[512];			//常用活动地址1

	uint32_t address2_len;
	char address2[512];			//常用活动地址2

	uint32_t address3_len;
	char address3[512];			//常用地址3

};
typedef UserRecord UserRecord;

struct GetPlayerInfo{
	static const uint32_t type = kGetPlayerInfo;
	int phone_len;
	char phone[12];
};
typedef struct GetPlayerInfo GetPlayerInfo;//c2s

struct PlayerInfoResult//s2c
{
	static const uint32_t type = kGetPlayerInfoResult;
	uint32_t result;//
	Nickname nick;
	uint32_t address1_len;
	char address1[128];

	uint32_t address2_len;
	char address2[128];

	uint32_t address3_len;
	char address3[128];
};
typedef struct PlayerInfoResult PlayerInfoResult;