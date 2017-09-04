/*
 * Broker.h
 *
 *  Created on: Jun 26, 2017
 *      Author: user
 */

#ifndef SRC_BROKER_H_
#define SRC_BROKER_H_
#include "TCPSocket.h"
#include "MThread.h"
#include "BrokerMng.h"
#include "MTCPListener.h"
#include "TCPMessangerProtocol.h"
#include "UsersDataBase.h"
#include "User.h"
#include <stdlib.h>
#include "TCGame.h"
#include "UDPSocket.h"
#include "ISpecificGame.h"

namespace npl {

class Broker:public MThread {


public:
	Broker(User* peer1, User* peer2, BrokerMng* parent);
	~Broker();
	virtual void run();
	int sendCommandToClient(TCPSocket * socket, int cmd,const char* buff);

private:
	bool  handleMessage(TCPSocket * socket, const char * commandStr);
	int readFromSocket(TCPSocket * socket,  char * buffer , int bytesToRead);
	TCPSocket* getOtherPeer(TCPSocket* socket);
	BrokerMng* parent;
	User * peerOne;
	User * peerTwo;
	//MTCPListener* listener;

	UDPSocket* brokerUdpSocket;

	string peer1IP;
	string peer2IP;

	int peer1port;
	int peer2port;

	ISpecificGame* theGame;

};

}/* namespace npl */

#endif /* SRC_BROKER_H_ */
