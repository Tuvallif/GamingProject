/*
 * User.cpp
 *
 *  Created on: Aug 20, 2017
 *      Author: user
 */

#include <src/User.h>

using namespace std;

namespace npl {


	User::User(string username, TCPSocket * socket){
		this->username = username;
		this->socket = socket;
		this->state = STATE_DEFAULT;
	}



}
