#include "MySocket.h"
#include "common.h"
#include "db.h"
#include "account.h"

#include <stdio.h>
#include <string>
#include <thread>
#include <map>
#include <fstream>

namespace p{
	Db db;
	ODSocket mysocket,clientsocket;
	std::map<int32_t,UserRecord> g_user;
	Account account;
	uint32_t playerMaxId;
}


void cleanPlayerInfo(ODSocket *socket_){
	if (p::g_user.size() == 0)
	{
		return;
	}

	for (std::map<int32_t,UserRecord>::iterator iter = p::g_user.begin();iter != p::g_user.end();)
	{
		if (iter->second.odSocket.m_sock == socket_->m_sock)
		{
			p::g_user.erase(iter->first);
			return;
		}

		++iter;
		if (iter == p::g_user.end())
		{
			if (iter->second.odSocket.m_sock == socket_->m_sock)
			{
				p::g_user.erase(iter->first);
				return;
			}
			return;
		}
	}
}

MqHead getHead(ODSocket *socket_){
	MqHead head;
	int head_len = socket_->Recv((char*)&head,sizeof(head),0);
	
	if (head_len <= 0){
		head.type =0;
		cleanPlayerInfo(socket_);
	}
	return head;
}


template<typename Msg>
int getBody(ODSocket *socket_,const Msg& msg){
	int result = socket_->Recv((char*)&msg,sizeof(msg),0);
	if( result <= 0 ){
		cleanPlayerInfo(socket_);
	}
	return result;
}



void TalkToClient(ODSocket *socket_,char* clientIp){
	for (;;)
	{
		 
		MqHead head = getHead(socket_);
		if (head.type == 0)
		{
			return;
		}

		switch (head.type)
		{
		case Register::type:
			Register register_;
			if (getBody(socket_,register_) > 0){
				++p::playerMaxId;
				int result = p::account.DoUserRegister(register_,p::db,p::playerMaxId);
				if (result == kSucess)
				{
					UserRecord userRecord;
					userRecord.user_id = p::playerMaxId;
					userRecord.age = register_.age;
				 
					memcpy(userRecord.nick.name,register_.nick.name,register_.nick.len);
					userRecord.odSocket = *socket_;
					
					memcpy(userRecord.ip,clientIp,strlen(clientIp));

					memcpy(userRecord.address1,register_.address1,strlen(register_.address1));

					if (register_.address1_len > 0)
					{
						userRecord.address1_len = register_.address1_len;
						memcpy(userRecord.address1,register_.address1,register_.address1_len);
					}

					if (register_.address2_len > 0)
					{
						userRecord.address2_len = register_.address2_len;
						memcpy(userRecord.address2,register_.address2,register_.address2_len);
					}

					if (register_.address3_len > 0)
					{
						userRecord.address3_len = register_.address3_len;
						memcpy(userRecord.address3,register_.address3,register_.address3_len);
					}
					
					p::g_user[userRecord.user_id] = userRecord;

					
				}
				
				head.type = RegisterResult::type;
				head.aid = p::playerMaxId;

				RegisterResult registerResult;
				registerResult.result = result;
				
				socket_->SendMsg(head,(char*)&registerResult,sizeof(registerResult));
				break;
			}

		case Login::type:
			Login login_;
			if (getBody(socket_,login_))
			{
				UserRecord userRecord;
				int result = p::account.DoUserLogin(login_,p::db,userRecord);
				if (result == kSucess)
				{
					userRecord.odSocket = *socket_;
					memcpy(userRecord.ip,clientIp,strlen(clientIp));
					p::g_user[userRecord.user_id] = userRecord;
				}

				head.type = LoginResult::type;
				head.aid = userRecord.user_id;

				LoginResult loginResult;
				loginResult.result = result;
				socket_->SendMsg(head,(char*)&loginResult,sizeof(loginResult));
			}
			break;

		case GetPlayerInfo::type:
			GetPlayerInfo info_;
			if (getBody(socket_,info_)){
				PlayerInfoResult playerInfoResult;
				UserRecord userRecord;
				Login login_;
				login_.phone_len = info_.phone_len;
				memcpy(login_.phone,info_.phone,login_.phone_len);

				int result = p::account.DoUserLogin(login_,p::db,userRecord);
				if (result == kSucess)
				{
					userRecord.odSocket = *socket_;
					memcpy(userRecord.ip,clientIp,strlen(clientIp));
					p::g_user[userRecord.user_id] = userRecord;
				}

				head.type = PlayerInfoResult::type;
				head.aid = userRecord.user_id;
				playerInfoResult.result = result;

				if (playerInfoResult.result == kSucess){
					playerInfoResult.nick.len =userRecord.nick.len;
					memcpy(playerInfoResult.nick.name, userRecord.nick.name,userRecord.nick.len);

					playerInfoResult.address1_len = userRecord.address1_len;
					memcpy(playerInfoResult.address1, userRecord.address1,playerInfoResult.address1_len);

					playerInfoResult.address2_len = userRecord.address2_len;
					memcpy(playerInfoResult.address2, userRecord.address2,playerInfoResult.address2_len);

					playerInfoResult.address3_len = userRecord.address3_len;
					memcpy(playerInfoResult.address3, userRecord.address3,playerInfoResult.address3_len);
				}
				socket_->SendMsg(head,(char*)&playerInfoResult,sizeof(playerInfoResult));
			}
			
				break;
		case LoginOut::type:
			LoginOut loginOut_;
			if (getBody(socket_,loginOut_)){
				cleanPlayerInfo(socket_);
			}
			break;
		default:
			break;
		}

		std::this_thread::sleep_for(std::chrono::milliseconds(50));
	}
}

void connectSocket(ODSocket &mysocket){

	mysocket.Init();
	mysocket.Create(AF_INET,SOCK_STREAM,0);
	mysocket.Bind(7880);
	mysocket.Listen(500);
	//mysocket.setMode(1);
}


void connectDb(Db &db){
	db.OpenDb();

	bool result = db.CreateTable("Create Table playerMaxId(player_max_id int)");
	if (result)
	{
		db.WriteDb("insert into playerMaxId values(0)");
		p::playerMaxId = 0;
	}else{
		bool result = db.ReadDb("select player_max_id from playerMaxId");
		if (result)
		{
			std::string str = db.pazResult[db.nCol];
			p::playerMaxId = std::atoi(str.c_str());
			db.freeTableData();
		}
	}
	
	db.CreateTable("create table playerInfo(player_id int primary key,nick varchar(18),age int,register_time int,forbid_login_time int,phone varchar(12),address1 varchar(512),address2 varchar(512),address3 varchar(512))");
}

int main(){
	
	connectDb(p::db);

	connectSocket(p::mysocket);

	char ipClient[32] = "\0";
	ODSocket clientsocket;
	
	while(true){
		bool isAccpet = p::mysocket.Accept(clientsocket,ipClient);
		if (isAccpet)
		{
			printf("connect ip = %s",ipClient);
			std::thread new_thread(TalkToClient,&clientsocket,ipClient);
			new_thread.detach();
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(50));
	}
	
 
#ifdef _WIN32
	p::mysocket.Close();
	p::mysocket.Clean();
	 
#endif
}
