/*
 * UDPClientThread.cpp
 *
 *  Created on: Sep 4, 2017
 *      Author: dmanzur
 */

#include <src/UDPClientThread.h>
#include <string.h>
#include <string>
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include "MThread.h"


using namespace std;
using namespace npl;

UDPClientThread::UDPClientThread(int port) {

	this->socket = new UDPSocket(port);
	// TODO Auto-generated constructor stub

}


 void UDPClientThread::run(){

		while (true) {
			char messageFromGame[300];
			memset(messageFromGame, 0, sizeof(messageFromGame));
			socket->recv(messageFromGame, sizeof(messageFromGame));
			cout<<"received msg from "<<socket->fromAddr()<<endl;
			cout<<messageFromGame<<endl;
		}
 }
UDPClientThread::~UDPClientThread() {
	// TODO Auto-generated destructor stub
}


