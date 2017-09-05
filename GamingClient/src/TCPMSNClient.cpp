/*
 * TCPMSNClient.cpp
 *
 *  Created on: May 29, 2017
 *      Author: user
 */

#include <string.h>
#include <string>
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include "TCPMessangerProtocol.h"
#include "MThread.h"
#include "TCPMSNClient.h"

using namespace std;
using namespace npl;

bool sessionIsActive = false;

TCPMSNClient::TCPMSNClient() {
	socket = NULL;
	udpPort = 0;
}

TCPMSNClient::TCPMSNClient(TCPMessangerClientHandler* handler) {
	this->handler = handler;
	socket = NULL;
	udpPort = 0;
}

TCPMSNClient::~TCPMSNClient() {
	if (socket != NULL) {
		delete socket;
	}
	delete handler;
	if (udpThread != NULL)
		delete udpThread;
}

bool TCPMSNClient::connect(string ip) {
	socket = new TCPSocket(ip, MSNGR_PORT);
	int error = 0;
	socklen_t len = sizeof(error);
	int retval = getsockopt(socket->socket(), SOL_SOCKET, SO_ERROR, &error,
			&len);
	if (retval != 0) {
		cout << "there was an error opening the socket!" << endl;
		return false;
	}

	thread = new TCPMSNClientThread(socket);
	thread->start();

	time_t timev;
	time(&timev);

	udpPort = timev % 100000;
	udpThread = new UDPClientThread(udpPort);
	cout << "client udp port: " << udpPort << endl;
	udpThread->start();

	return true;
}

bool TCPMSNClient::openSession(string ip, int port) {
	char buff[100];
	sprintf(buff, "%s:%d", ip.c_str(), port);
	cout << "data to send:" << buff << endl;
	int res = sendCommand(OPEN_SESSION_WITH_PEER, buff);
	sessionIsActive = (res > -1);
	return sessionIsActive;
}

bool TCPMSNClient::loginRegister(string username, string password) {

	char buff[20];
	sprintf(buff, "%d", this->udpPort);
	cout << "client udp port is " << this->udpPort << endl;
	int res = sendCommand(LOGIN_REGISTER, username.c_str(), password.c_str(), buff);
	sessionIsActive = (res > -1);
	return sessionIsActive;
}

bool TCPMSNClient::getListOfUsers() {
	int res = sendCommand(GET_LIST_USERS, NULL);
	sessionIsActive = (res > -1);
	return sessionIsActive;
}

void TCPMSNClient::startMatchWithUser(string userName) {
	cout << "sending request to server to start match with user " + userName
			<< endl;

	char buff[20];
	sprintf(buff, ":%d", this->udpPort);
	userName.append(buff);
	sendCommand(START_MATCH, userName.c_str());

}

void TCPMSNClient::seek() {
	cout << "sending request to server to start match with random user "
			<< endl;
	sendCommand(SEEK, NULL);

}

void TCPMSNClient::answerRequest(string answer) {
	string userName = this->thread->getWaitingSocket();
	this->thread->setWaitingSocket("");
	if (answer == "YES" || answer == "yes") {
		char buff[4];
		sprintf(buff, "%d", this->udpPort);
		sendCommand(ACCEPT_REQUEST_MATCH, buff);
	} else {
		sendCommand(DECLINE_START_MATCH, NULL);
	}
}

void TCPMSNClient::sendMessage(string msg) {
	cout << "this message will be sent to peer " << msg << endl << flush; //why flush?;
	if (!sessionIsActive) {
		sendCommand(SESSION_REFUSED, SESSION_REFUSED_MSG);
	} else {
		sendCommand(SEND_MSG_TO_PEER, msg.c_str());
	}
}

int TCPMSNClient::getPort() {
	return socket->getPort();
}

void TCPMSNClient::closeSession() {
	if (sessionIsActive) {
		sendCommand(CLOSE_SESSION_WITH_PEER, " ");
		cout << "Session was closed by other side" << endl << flush;
	} else {
		sendCommand(SESSION_REFUSED, SESSION_REFUSED_MSG);
	}
}

void TCPMSNClient::disconnect() {

	sendCommand(SESSION_REFUSED, "other user asked to disconnect");

	if (socket != NULL) {
		cout << "disconnecting from socket" << endl << flush;
		closeSession();
		socket->close();
		//waitForThread();
		sendCommand(EXIT, " ");
		cout << "communication with server was interrupted - connection closed"
				<< endl;
		cout << "closing socket" << endl << flush;
	} else
		cout << "you're not connected" << endl << flush;
}

void TCPMSNClient::exit() {
	sendCommand(EXIT, "other user asked to exit");
}

void TCPMSNClient::showScore(){
	sendCommand(SHOW_SCORE,NULL);
}

int TCPMSNClient::sendCommand(int cmd, const char* buff, const char * buff2, const char * buff3) {
	int cmdNet = htonl(cmd);
	int res = socket->write((char*) &cmdNet, 4);
	if (res < 4) {
		return -1;
	}
	if (buff != NULL) {
		int len = strlen(buff);
		int lenNet = htonl(len);
		res = socket->write((char*) &lenNet, 4);
		if (res < 4) {
			return -1;
		}
		res = socket->write(buff, len);
		if (res < len) {
			return -1;
		}
	}
	if (buff2 != NULL) {
		int len = strlen(buff2);
		int lenNet = htonl(len);
		res = socket->write((char*) &lenNet, 4);
		if (res < 4) {
			return -1;
		}
		res = socket->write(buff2, len);
		if (res < len) {
			return -1;
		}
	}
	if (buff3 != NULL) {
		int len = strlen(buff3);
		int lenNet = htonl(len);
		res = socket->write((char*) &lenNet, 4);
		if (res < 4) {
			return -1;
		}
		res = socket->write(buff3, len);
		if (res < len) {
			return -1;
		}
	}
	return res;
}

void TCPMSNClient::sendAnswerToGame(string answer) {

	string ip = "127.0.0.1";

	cout << "sendAnswerToGame" << endl;
	this->udpThread->socket->reply(answer);
}

