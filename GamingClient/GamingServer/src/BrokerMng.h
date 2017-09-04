/*
 * BrokerMng.h
 *
 *  Created on: Jun 26, 2017
 *      Author: user
 */

#ifndef SRC_BROKERMNG_H_
#define SRC_BROKERMNG_H_

#include "TCPSocket.h"
#include "UsersDataBase.h"
#include <vector>
#include <algorithm>

namespace npl {

// forward declaration since the header file does not use methods
// of broker
class Broker;

class BrokerMngHandler{
public:
	virtual void handleReturnedPeer(User* peer,  int scoreAdj)=0;
	virtual ~BrokerMngHandler(){}
};


class BrokerMng {
private:
	BrokerMngHandler* handler;
	vector<Broker *> * brokers;

public:
	BrokerMng(BrokerMngHandler* handler);
	~BrokerMng();
	void createBroker(User* peer1, User* peer2);

	void releasePeer(User* peer, int scoreAdj);
	void deleteBroker(int i);
	void deleteBroker(Broker* broker);

};

} /* namespace npl */

#endif /* SRC_BROKERMNG_H_ */

