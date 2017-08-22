/*
 * User.cpp
 *
 *  Created on: Aug 20, 2017
 *      Author: user
 */

#include "User.h"

using namespace std;

namespace npl {


User::User(string username, TCPSocket * socket){
	this->username = username;
	this->socket = socket;
	this->state = STATE_DEFAULT;
}

int User::getState(){
	return state;
}

bool User:: changeState(int newState){
	if(validInputState(newState))
	{
		state = newState;
		return true;
	}

	return false;
}

bool User::validInputState(int inputState){
	if(inputState >= User::STATE_DEFAULT &&  inputState <= User::STATE_BUSY){
		return true;
	}

	return false;
}



}
