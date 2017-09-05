/*
 * Broker.cpp
 *
 *  Created on: Jun 26, 2017
 *      Author: user
 */

#include "Broker.h"
#include "BrokerMng.h"
#include<stdio.h>
#include<string.h>    //strlen
#include<sys/socket.h>
#include<arpa/inet.h> //inet_addr
#include<unistd.h>    //write
#include <netinet/in.h>
#include <sys/socket.h>

using namespace std;

namespace npl {

char* getCharFromString(string toConvert) {
	char * writable = new char[toConvert.size() + 1];
	std::copy(toConvert.begin(), toConvert.end(), writable);
	writable[toConvert.size()] = '\0';
	return writable;
}

string getWordAtIndexWithDlm(string sentance, int index,
		const char* delimiter) {
	char* sentanceInChar = getCharFromString(sentance);
	char* convertor = strtok(sentanceInChar, delimiter);
	for (int i = 0; i < index; i++) {
		convertor = strtok(NULL, delimiter);
		cout << convertor << endl;
	}
	string toReturn(convertor);
	if (toReturn.empty()) {
		toReturn = "p";
	}

	return toReturn;
}

Broker::Broker(User* peer1, User* peer2, BrokerMng* parent) {
	this->parent = parent;

	peerOne = peer1;
	peerTwo = peer2;
	//listener = new MTCPListener();
	//listener->add(peerOne->socket);
	//listener->add(peerTwo->socket);

	brokerUdpSocket = new UDPSocket(MSNGR_PORT);

	cout << "1 sockets peers address: " << peer1->udpPort << " "
			<< peer2->udpPort << endl;

	peer1IP = peer1->socket->fromAddr();
	peer1port = atoi(peer1->udpPort.c_str());

	peer2IP = peer2->socket->fromAddr();

	peer2port = atoi(peer2->udpPort.c_str());

	cout << "2 sockets peers address: " << peer1port << " " << peer2port
			<< endl;

	this->theGame = new TCGame(peer1->username, peer2->username);
	string openningMsg = this->theGame->startGame();

	brokerUdpSocket->sendTo(openningMsg, peer1IP, peer1port);
	brokerUdpSocket->sendTo(openningMsg, peer2IP, peer2port);
	this->start();
}

Broker::~Broker() {
	//delete listener;
	if (brokerUdpSocket != NULL)
		delete brokerUdpSocket;
	if (theGame != NULL)
		delete theGame;
}

void Broker::run() {
	while (!theGame->isGameEnded()) {


		char messageFromPeer[300];
		memset(messageFromPeer, 0, sizeof(messageFromPeer));

		brokerUdpSocket->recv(messageFromPeer, sizeof(messageFromPeer));

		cout << "received msg from " << brokerUdpSocket->fromAddr() << endl;

		string gameAnswer;
		if (strcmp(brokerUdpSocket->fromAddr().c_str(), "peer1") == 0)
			gameAnswer = theGame->getIncomingMsgFromPeer(FIRST_PLAYER,
					messageFromPeer);
		else
			gameAnswer = theGame->getIncomingMsgFromPeer(SECOND_PLAYER,
					messageFromPeer);

		//here we send the game answer o both of players
		brokerUdpSocket->sendTo(gameAnswer, peer1IP, peer1port);
		brokerUdpSocket->sendTo(gameAnswer, peer2IP, peer2port);

	}

	string winner = theGame->getTheGameWinner();

	cout << "Broker terminated" << endl;
	sendCommandToClient(peerOne->socket, CLOSE_SESSION_WITH_PEER, NULL);
	sendCommandToClient(peerTwo->socket, CLOSE_SESSION_WITH_PEER, NULL);

	//string winner = theGam
	if (strcmp(winner.c_str(),peerOne->username.c_str())==0){
	parent->releasePeer(peerOne, 20);
	parent->releasePeer(peerTwo, 10);
	} else if (strcmp(winner.c_str(),peerTwo->username.c_str())==0){
		parent->releasePeer(peerOne, 10);
		parent->releasePeer(peerTwo, 20);
	}
	else{
		parent->releasePeer(peerOne, 0);
		parent->releasePeer(peerTwo, 0);
	}

	parent->deleteBroker(this);
}

bool Broker::handleMessage(TCPSocket * socket, const char * commandStr) {
	cout << "WE ARE IN HANDLEMSG OF BROKER " << endl;
	int command = 0;
	command = ntohl(*((int *) commandStr));
	cout << "got command " << command << endl;
	string strngMsgToSnd;
	switch (command) {
	case SEND_MSG_TO_PEER: {
		char messageLength[4];
		readFromSocket(socket, messageLength, sizeof(messageLength));
		int dataLength = ntohl(*((int *) (messageLength)));
		char dataBuffer[dataLength];
		readFromSocket(socket, dataBuffer, sizeof(dataBuffer));
		string data(dataBuffer, dataLength);
		cout << "got message length " << dataLength << " command "
				<< data.c_str() << endl;

		//dont need this...
		TCPSocket * socketToSnd = getOtherPeer(socket);
		sendCommandToClient(socketToSnd, SEND_MSG_TO_PEER, data.c_str());
		cout << "We sent the command to client with " << data.c_str() << endl;
		break;
	}
	case CLOSE_SESSION_WITH_PEER: {
		/**BY RETURNING FALSE WE ACTUALLY DO THIS:
		 TCPSocket* otherPeer = getOtherPeer(socket);
		 sendCommandToClient(socket, CLOSE_SESSION_WITH_PEER, NULL);
		 sendCommandToClient(otherPeer, CLOSE_SESSION_WITH_PEER, NULL);
		 **/
		return false;
	}
	case EXIT: {
		sendCommandToClient(socket, EXIT, NULL);
		return false;
	}
	case SESSION_REFUSED:
		break;
	case SESSION_ESTABLISHED:
		break;
	case OPEN_SESSION_WITH_PEER: {
		sendCommandToClient(socket, SEND_MSG_TO_PEER,
				"Please end current conversation before starting a new one");
		break;
	}
	default: {
		sendCommandToClient(socket, SEND_MSG_TO_PEER,
				"Your request was not identified, Please try again");
		break;

	}
		return true;
	}
}

int Broker::readFromSocket(TCPSocket * socket, char * buffer, int bytesToRead) {
	int length = socket->recv(buffer, bytesToRead);
	if (length != bytesToRead) {
		cout << "problem reading length " << length << endl;
		// TOOD this is error close the socket
	}
	return length;
}

int Broker::sendCommandToClient(TCPSocket * socket, int cmd, const char* buff) {
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
	return res;
}

TCPSocket* Broker::getOtherPeer(TCPSocket* socket) {
	if (socket == peerOne->socket) {
		return peerTwo->socket;
	}
	return peerOne->socket;
}

} /* namespace npl */
