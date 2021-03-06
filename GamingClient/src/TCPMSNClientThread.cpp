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
#include "MTCPListener.h"

using namespace std;
using namespace npl;

TCPMSNClientThread::TCPMSNClientThread(TCPSocket * socket) :
								MThread()
{
	this->socket = socket;
	string waitingSocketName = "NULL";
	bool currStateWaiting = false;
	listener = new MTCPListener();
	currStateWaiting = false;
}

//move later

vector<string> splitString(const string& inputString, const char* delim){
	vector<string> result;
	string copy = inputString;
	char * token = strtok((char *)copy.c_str(), delim);
	while(token != NULL){
		result.push_back(token);
		token= strtok(NULL, delim);
	}

	return result;
}



void TCPMSNClientThread::run()
{
	//cout << "client thread running" << endl;

	// TODO add stop boolean to stop when session disconnect
	while (true)
	{
		//cout << "client thread listening" << endl;
		listener->add(socket);
		listener->listen();

		char buffer[1024];
		int len = socket->recv(buffer, 1024);

		int msg = htonl(* ( (int *) buffer ) );
		//cout << "got message " << msg << " length " << len << endl;

		switch(msg){
		case SEND_MSG_TO_PEER:
		{
			int msgLength = htonl(* ( (int *) &buffer[4] ) );
			string msgText(&buffer[8],msgLength);
			cout << "received message : " << msgText << endl;
			break;
		}
		case GET_LIST_USERS:
		{
			int msgLength = htonl(* ( (int *) &buffer[4] ) );
			string msgText(&buffer[8],msgLength);
			cout << "online users : " << msgText << endl;
			vector<string> onlineUsers = splitString(msgText, USER_NAME_SEPERATOR);
			for(int index = 0 ; index < onlineUsers.size() ; index++) {
				cout << index << " : " << onlineUsers[ index ] << endl;
			}
			break;
		}
		case LOGIN_REGISTER_SUCCESFULL:{
			cout << "You are now logged in" << endl;
			break;
		}
		case LOGIN_REGISTER_FAILED:
		{
			int msgLength = htonl(* ( (int *) &buffer[4] ) );
			string msgText(&buffer[8],msgLength);
			cout << msgText << endl;
			break;
		}
		case MATCH_REJECTED:
		{
			break;
		}
		case REQUEST_START_MATCH:
		{
			int msgLength = htonl(* ( (int *) &buffer[4] ) );
			string userName(&buffer[8],msgLength);
			waitingUserName = userName;
			cout << "You got a new match request from " << userName << endl;
			cout << "would you like to accept the new match with " << userName << "? format: r  <yes/no>" << endl;

			break;
		}
		case DECLINE_START_MATCH:
		{
			int msgLength = htonl(* ( (int *) &buffer[4] ) );
			string otherUserName(&buffer[8],msgLength);
			cout << "User " +otherUserName + " declined your request." << endl;
			break;
		}
		case GAME_STARTED:
		{
			int msgLength = htonl(* ( (int *) &buffer[4] ) );
			string otherUserName(&buffer[8],msgLength);
			cout << "started game with " +otherUserName << endl;
			break;
		}
		case REQUEST_MATCH_BUSY:
		{
			int msgLength = htonl(* ( (int *) &buffer[4] ) );
			string userName(&buffer[8],msgLength);
			cout << "You got a new request from " << userName << " but it was ignored because you have a pending request" << endl;

			break;
		}
		case SHOW_SCORE:
		{


			int msgLength = htonl(* ( (int *) &buffer[4] ) );
			string scoreData(&buffer[8],msgLength);
			//just a random sign different from #
			cout << "This is the SCORE BOARD!" << endl;

			vector<string> scoreBoard = splitString(scoreData, ",");
			for(int index = 0 ; index < scoreBoard.size() ; index++) {
				cout << (index + 1) << " . " << scoreBoard[ index ] << endl;
			}

			break;
		}
		default:
		{
			cout <<"the message was not understood, please try again." << endl;
		}
		}
	}
	//cout << "client thread stopped" << endl;
}

string TCPMSNClientThread::getWaitingSocket(){
	return waitingUserName;
}

void TCPMSNClientThread::setWaitingSocket(string newName){
	waitingUserName = newName;
}





