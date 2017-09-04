/*
 * User.h
 *
 *  Created on: Aug 20, 2017
 *      Author: user
 */

#ifndef USER_H_
#define USER_H_

#include <map>
#include <list>
#include <string>
#include "TCPSocket.h"

using namespace std;

namespace npl {

class User {
public:

	static const int STATE_DEFAULT = 0;
	static const int STATE_SEEKING = 1;
	static const int STATE_PLAYING = 2;
	static const int STATE_BUSY = 3;
	string username;
	TCPSocket * socket;
    string udpPort;

	User(string username, TCPSocket * socket);

	bool changeState(int newState);

	int getState();

private:

	User(const User& other);
	int state;
	/**
	 * this method checks that the input state is valid
	 */
	bool validInputState(int inputState);


};
}
#endif /* USER_H_ */
