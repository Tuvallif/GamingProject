/*
 * Dispatcher.cpp
 *
 *  Created on: Jun 26, 2017
 *      Author: user
 */

#include "Dispatcher.h"
#include "MTCPListener.h"
#include <unistd.h>
#include <stdio.h>
#include <string>
#include <cstring>
#include "TCPMessangerProtocol.h"

using namespace std;


namespace npl {
Dispatcher::Dispatcher(DispatcherHandler* handler) : MThread() {
	cout << "Dispatcher created" << endl;
	userDB = new UsersDataBase(USERS_DB_FILE);
	this->dispatcherHandler = handler;
	listener = new MTCPListener();
	start();
}

Dispatcher::~Dispatcher() {
	delete listener;
}

void Dispatcher::addPeer(TCPSocket* peer){
	cout << "Adding peer " << peer->fromAddr() << ":" << peer->peerPort();
	cout << endl;
	listener->add(peer);
};

void Dispatcher::seek(TCPSocket* requestingSocket){

}

int Dispatcher::readFromSocket(TCPSocket * socket,  char * buffer , int bytesToRead)
{
	int length = socket->recv(buffer, bytesToRead );
	if ( length != bytesToRead )
	{
		cout << "problem reading length " << length;
		cout << endl;
		// TOOD this is error close the socket
	}
	return length;
}

void Dispatcher::run() {

	const vector<TCPSocket *> & sockets = listener->socketsList();
	cout << "Dispatcher running" << endl;
	while (true )
	{
		if ( sockets.empty() )
		{
			//cout << "Dispatcher waiting 1 second" << endl;
			usleep( 1000000 );
			continue;
		}
		//cout << "Dispatcher selecting " << sockets.size() << " sockets " << endl;
		TCPSocket * readySocket = listener->listen();
		if ( readySocket ) {
			cout << "Dispatcher socket ready" << endl;
			char commandType[4];
			int length = readFromSocket(readySocket, commandType, sizeof(commandType));

			if ( length == 0 )
			{
				cout << "socket error. disconnecting client" << endl;
				listener->remove(readySocket);
				continue;
			}
			int command = ntohl( *( (int * ) commandType ) );
			cout << "received command " << command << " from " << readySocket->fromAddr() << ":" << readySocket->peerPort();
			cout << endl;
			// TODO if command length is 0, close the peer and remove it from listener
			handleMessage(readySocket, command);
			// TODO : parse and handle the commands : disconnect, open session...
		}
	}
	cout << "Dispatcher terminated" << endl;
}
TCPSocket* Dispatcher::findSocketInMap(TCPSocket* socketKey){
	std::map<TCPSocket*, TCPSocket*>::iterator i = openRequests.find(socketKey);
	assert(i != openRequests.end());
	try{
		TCPSocket* sockToRtrn =  i->second;
		return sockToRtrn;
	}
	catch(exception e){
		cout << e.what() << endl;
		return NULL;
	}


}

int Dispatcher::sendCommandToClient(TCPSocket * socket, int cmd,const char* buff){
	int cmdNet = htonl(cmd);
	int res = socket->write((char*)&cmdNet,4);
	if (res < 4){
		return -1;
	}
	if (buff != NULL){
		int len = strlen(buff);
		int lenNet = htonl(len);
		res = socket->write((char*)&lenNet,4);
		if (res < 4){
			return -1;
		}
		res = socket->write(buff,len);
		if (res < len){
			return -1;
		}
	}
	return res;
}

void Dispatcher::handleMessage(TCPSocket * socket, int commandType )
{
	switch(commandType)
	{
	/*
	case OPEN_SESSION_WITH_PEER: {
		char messageLength[4];
		readFromSocket(socket, messageLength, sizeof(messageLength));
		int dataLength = ntohl( *( (int * ) ( messageLength ) ) );
		char dataBuffer[dataLength];
		readFromSocket(socket, dataBuffer, sizeof(dataBuffer));
		string data(dataBuffer, dataLength);
		cout << "got message length " << dataLength << " command " << data.c_str() << endl;
		const vector<TCPSocket *> & sockets = listener->socketsList();
		TCPSocket * foundPeer = NULL;
		for( vector<TCPSocket *>::const_iterator it = sockets.begin(); it != sockets.end() ; ++it)
		{
			TCPSocket * peer = * it;
			if ( socket == peer ) {
				// don't check against yourself
				continue;
			}
			char port[16];
			sprintf(port, "%d", peer->peerPort());
			string peerAddress = peer->fromAddr() + ":" + port;
			cout << "checking if requested peer " << data << " is " << peerAddress << endl;
			if ( data == peerAddress )
			{
				foundPeer = peer;
				break;
			}
		}
		if ( foundPeer == NULL )
		{
			// peer not found. send session refused to socket
			sendCommandToClient( socket, SESSION_REFUSED, NULL);
		} else{
			sendCommandToClient(socket, SESSION_ESTABLISHED, NULL);
			sendCommandToClient(foundPeer, SESSION_ESTABLISHED, NULL);
			listener->remove(socket);
			listener->remove(foundPeer);
			// create new broker and pass them to the broker
			dispatcherHandler->managePeerSession(socket, foundPeer);
		}
		break;
	}
	 */
	case SEEK:
	{
		User* secondUser = userDB->seek(socket);

		//Other user also seeking state - start a match
		if(secondUser != NULL){
			User * firstUser = userDB->findUserBySocket(socket);
			userDB->changeUserState(socket, User::STATE_BUSY);
			userDB->changeUserState(secondUser->socket, User::STATE_BUSY);
			sendCommandToClient(socket, GAME_STARTED, secondUser->username.c_str());
			sendCommandToClient(secondUser->socket, GAME_STARTED, firstUser->username.c_str());
			listener->remove(socket);
			listener->remove(secondUser->socket);
			// create new broker and pass them to the broker
			dispatcherHandler->managePeerSession(firstUser, secondUser);

		}
		break;
	}
	case START_MATCH:
	{
		char messageLength[4];
		readFromSocket(socket, messageLength, sizeof(messageLength));
		int dataLength = ntohl( *( (int * ) ( messageLength ) ) );
		char dataBuffer[dataLength];
		readFromSocket(socket, dataBuffer, sizeof(dataBuffer));
		string name(dataBuffer);
		int pos = name.find(":");
		string secondUserName = name.substr(0,pos);
		this->udpPort1 = name.substr(pos+1,5);
       cout<<"start match udp port: "<<this->udpPort1<<endl;
		int status = userDB->checkAvilability(secondUserName);

		if(status == -1) {
			sendCommandToClient(socket, MATCH_REJECTED, "The match could not be established, user not found or playing");
		}
		else if(status == User::STATE_SEEKING)
		{
			User * firstUser = userDB->findUserBySocket(socket);
			User * secondUser = userDB->findUserByName(secondUserName);
			userDB->changeUserState(socket, User::STATE_BUSY);
			userDB->changeUserState(secondUser->socket, User::STATE_BUSY);
			sendCommandToClient(socket, GAME_STARTED, secondUser->username.c_str());
			sendCommandToClient(secondUser->socket, GAME_STARTED, firstUser->username.c_str());
			listener->remove(socket);
			listener->remove(secondUser->socket);
			// create new broker and pass them to the broker
			dispatcherHandler->managePeerSession(firstUser, secondUser);
		}
		else if(status == User::STATE_DEFAULT)
		{

			TCPSocket * secondSocket = userDB->findUserByName(secondUserName)->socket;
			string requestingSocketName = userDB->findUserBySocket(socket)->username;
			openRequests[secondSocket] = socket;

			//change state for both users to be busy
			userDB->changeUserState(socket, User::STATE_BUSY);
			userDB->changeUserState(secondSocket, User::STATE_BUSY);

			//sending request to other user
			sendCommandToClient(secondSocket, REQUEST_START_MATCH, requestingSocketName.c_str());

		}
		else if(status == User::STATE_BUSY)
		{


			TCPSocket * secondSocket = userDB->findUserByName(secondUserName)->socket;
			string requestingSocketName = userDB->findUserBySocket(socket)->username;
			openRequests.insert(std::pair<TCPSocket*,TCPSocket*>(socket,secondSocket));


			sendCommandToClient(secondSocket, REQUEST_MATCH_BUSY, requestingSocketName.c_str());

			sendCommandToClient(socket, DECLINE_START_MATCH, secondUserName.c_str());
		}
		else{
			sendCommandToClient(socket, DECLINE_START_MATCH, secondUserName.c_str());
		}
		break;
	}


	case CLOSE_SESSION_WITH_PEER:
	{
		cout << "got message to close peer from " <<socket->getPort() << endl;
		cout << "can't close session because Broker handles this" << endl;
		break;
	}
	case EXIT:{
		sendCommandToClient(socket, EXIT, NULL);
		listener->remove(socket);
		break;
	}
	case SEND_MSG_TO_PEER:
	{
		cout << "got input to send message peer from " <<socket->getPort() << endl;
		cout << "can't send message because Broker handles this" << endl;
		break;
	}
	//when do i get those??
	case SESSION_REFUSED: break;
	case SESSION_ESTABLISHED: break;
	case LOGIN_REGISTER:
	{
		char usernameLengthBuff[4];
		readFromSocket(socket, usernameLengthBuff, sizeof(usernameLengthBuff));
		int usernameLength = ntohl( *( (int * ) ( usernameLengthBuff ) ) );
		char usernameBuffer[usernameLength];
		readFromSocket(socket, usernameBuffer, sizeof(usernameBuffer));
		string username(usernameBuffer, usernameLength);
		char passwordLengthBuff[4];
		readFromSocket(socket, passwordLengthBuff, sizeof(passwordLengthBuff));
		int passwordLength = ntohl( *( (int * ) ( passwordLengthBuff ) ) );
		char passwordBuffer[passwordLength];
		readFromSocket(socket, passwordBuffer, sizeof(passwordBuffer));
		string password(passwordBuffer, passwordLength);

		if(userDB->login(username, password, socket)){
			sendCommandToClient(socket, LOGIN_REGISTER_SUCCESFULL, NULL);
		}else{
			sendCommandToClient(socket, LOGIN_REGISTER_FAILED, NULL);
		}
		break;
	}
	case GET_LIST_USERS:
	{
		string onlineUsers = userDB->listUsersInString();
		sendCommandToClient(socket, GET_LIST_USERS, onlineUsers.c_str());
		break;
	}
	case ACCEPT_REQUEST_MATCH:
	{
		//TCP userName = userDB->findUserBySocket(socket)->username;

		TCPSocket* otherSocket = findSocketInMap(socket);
		if (otherSocket == NULL)
		{
			//remove from busy
			userDB->changeUserState(socket, User::STATE_DEFAULT);
			// peer not found. send session refused to socket
			sendCommandToClient(socket, SESSION_REFUSED, NULL);
		} else{
			char udpPort[4];
			readFromSocket(socket, udpPort, sizeof(udpPort));
			int udpPortLength = ntohl( *( (int * ) ( udpPort ) ) );
			char udpPortbuff[udpPortLength];
			readFromSocket(socket, udpPortbuff, sizeof(udpPortbuff));
			string uport(udpPortbuff, udpPortLength);
			cout<<"dispatcher accept uport: "<<uport<<endl;
			this->udpPort2 = uport;
			User * firstUser = userDB->findUserBySocket(socket);
			User * secondUser = userDB->findUserBySocket(otherSocket);
			userDB->changeUserState(firstUser->socket, User::STATE_PLAYING);
			userDB->changeUserState(secondUser->socket, User::STATE_PLAYING);
			sendCommandToClient(socket, GAME_STARTED, secondUser->username.c_str());
			sendCommandToClient(otherSocket, GAME_STARTED, firstUser->username.c_str());
			listener->remove(socket);
			listener->remove(otherSocket);
			// create new broker and pass them to the broker
			firstUser->udpPort = this->udpPort1;
			secondUser->udpPort = this->udpPort2;
			dispatcherHandler->managePeerSession(firstUser, secondUser);
		}
		break;
	}
	case DECLINE_START_MATCH:
	{
		TCPSocket* otherSocket = findSocketInMap(socket);
		if (otherSocket == NULL)
		{
			User * firstUser = userDB->findUserBySocket(socket);
			userDB->changeUserState(firstUser->socket, User::STATE_DEFAULT);

			// peer not found. send session refused to socket
			sendCommandToClient(socket, DECLINE_START_MATCH, NULL);

		} else{
			User * firstUser = userDB->findUserBySocket(socket);
			User * secondUser = userDB->findUserBySocket(otherSocket);

			userDB->changeUserState(firstUser->socket, User::STATE_DEFAULT);
			userDB->changeUserState(secondUser->socket, User::STATE_DEFAULT);
			sendCommandToClient(otherSocket, DECLINE_START_MATCH, firstUser->username.c_str());

		}
		break;
	}
	default:
	{
		sendCommandToClient(socket, SEND_MSG_TO_PEER, "Your request was not identified, Please try again");
		break;
	}
	}
	//int length = *( (int * ) ( commandStr.c_str() + 4 ) );
	//char * data = commandStr.c_str() + 8;
}


} /* namespace npl */
