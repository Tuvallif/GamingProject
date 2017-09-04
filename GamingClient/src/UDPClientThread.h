/*
 * UDPClientThread.h
 *
 *  Created on: Sep 4, 2017
 *      Author: dmanzur
 */

#ifndef UDPCLIENTTHREAD_H_
#define UDPCLIENTTHREAD_H_

#include <iostream>
#include "UDPSocket.h"
#include <sstream>
#include <string>
#include <string.h>
#include <pthread.h>
#include "MThread.h"

using namespace std;
namespace npl {

class UDPClientThread :public MThread {
public:
	UDPClientThread(int port);
	virtual ~UDPClientThread();
	virtual void run();
	UDPSocket* socket;

};

} /* namespace npl */

#endif /* UDPCLIENTTHREAD_H_ */
