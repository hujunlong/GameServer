#include "MySocket.h"
#include "common.h"
#include "db.h"
#include "global.h"
#include "account.h"

#include <stdio.h>
#include <string>
#include <thread>


MqHead getHead(ODSocket *socket_);

template<typename Msg>
int getBody(ODSocket *socket_,const Msg& msg);

void closeSocket();

void TalkToClient(ODSocket *socket_);

void connectSocket(ODSocket &mysocket,ODSocket &clientsocket);

void connectDb(Db &db);




 



