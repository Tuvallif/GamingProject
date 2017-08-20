/*
 * UsersDataBase.cpp
 *
 *  Created on: Jul 13, 2017
 *      Author: user
 */


#include <fstream>
#include <stdio.h>
#include <iostream>
#include <algorithm>
#include <string>
#include <stdlib.h>
#include <vector>
#include "TCPMessangerProtocol.h"
#include "UsersDataBase.h"


using namespace std;
namespace npl{


UsersDataBase::UsersDataBase(  const string  & databaseFileName  ) :
									fileName( databaseFileName ) {
	/*
	registerUser("tuval", "12345");
	registerUser("ran", "rrrrr");
	string test;
	getPassword("ran", test);
	cout << "ran password '" << test << "'" << endl;
	 */
}

UsersDataBase::~UsersDataBase() {
	// TODO Auto-generated destructor stub
}

/**
 * try to login the user.if the user does not exist register it
 * return true if the user logged in successfully
 */
bool UsersDataBase::login( const string & username, const string & password, TCPSocket * socket ){
	list<User>::iterator it = users.begin();
	while (it != users.end()) {
		if (it->username == username) {
			cout << "user " << username << " already logged in" << endl;
		}
		++ it;
	}

	string DBpassword;
	if(getPassword(username, DBpassword)){
		if(hashPasswordString(password) == DBpassword){
			User user(username, socket);
			cout<< "user " << username << " is now logged in" << endl;
			users.push_back(user);
			return true;
		}
		cout << "bad password from user "<< username << "please try again" <<endl;
		return false;
	} else {
		registerUser( username, password );
		cout<< "user " << username << " is now registered and logged in" << endl;
		User user(username, socket);
		users.push_back(user);
		return true;
	}
}

/**
 * return list of connected users
 */
list< User > UsersDataBase::listUsers()
{
	return users;
}

string UsersDataBase::listUsersInString()
{
	string toReturn = "";
	for(list<User>::iterator it = users.begin(); it != users.end(); it++){
		toReturn += it->username;
		toReturn += USER_NAME_SEPERATOR;
	}

	return toReturn;
}

User* UsersDataBase::seek(TCPSocket* seekingSocket){
	User * user = NULL;
	for(list<User>::iterator it = users.begin(); it != users.end(); it++){
		if ( it->socket == seekingSocket ) {
			user = & (* it );
		}
		if(it->getState() == User::STATE_SEEKING && it->socket == seekingSocket)
		{
			cout << "we found a seeker " + it->username << endl;
			return & ( * it );
		}
	}
	cout << "changing the state to seeking for " +user->username << endl;
	user->changeState(User::STATE_SEEKING);
	return NULL;
}

int UsersDataBase::checkAvilability(const string& userName)
{
	User* usr = findUserByName(userName);
	if(usr != NULL){
		return usr->getState();
	}
	return -1;

}

/**
 * log out a user.
 */
void UsersDataBase::logout( const string & username )
{
	list<User>::iterator it = users.begin();
	while (it != users.end()) {
		if (it->username == username) {
			users.erase(it);
			return;
		}
		++ it;
	}
}

/**
 * gets the password of the user from the database
 */
bool UsersDataBase::getPassword( const string & username, string & password ) {
	ifstream inputFile(fileName.c_str());
	string line;
	string DBUserName, DBPassword;
	while(getline(inputFile, line)){
		int position = line.find("=");
		DBUserName = line.substr(0, position);
		DBPassword = line.substr(position + 1);
		if(username == DBUserName){
			password = DBPassword;
			return true;
		}
	}

	return false;
}

/**
 * append the username and password to the end of the file
 * in the format username=password\n
 */
bool UsersDataBase::registerUser( const string & username, const string & password ) {
	ofstream outputFile(fileName.c_str(), ios_base::app);
	string line = username + "=" + hashPasswordString(password) + "\n";
	outputFile.write(line.c_str(), line.length());
	return true;

}

User* UsersDataBase::findUserBySocket(TCPSocket* socket)
{
	User* usr;
	for(list<User>::iterator it = users.begin(); it != users.end(); it++){
		if ( it->socket == socket ) {
			usr = & (* it );
			return usr;
		}
	}

	return NULL;
}

User* UsersDataBase::findUserByName(const string& userName)
{
	User* usr;
	for(list<User>::iterator it = users.begin(); it != users.end(); it++){
		if ( it->username == userName ) {
			usr = & (* it );
			return usr;
		}
	}

	return NULL;
}

bool UsersDataBase::changeUserState(TCPSocket* userSocket, int newState){
	User* toChange = findUserBySocket(userSocket);
	if(toChange != NULL){
		return toChange->changeState(newState);
	}

	return false;
}


User* UsersDataBase::getSeekingUser(User* seeker){
	User* usrToRtrn;
	for(list<User>::iterator it = users.begin(); it != users.end(); it++){
		if ( it->getState() == User::STATE_SEEKING ) {
			usrToRtrn = & (* it );
			return usrToRtrn;
		}
	}

	return NULL;
}

string UsersDataBase::hashPasswordString(const string & password ){
	char passwordBuffer[64];
	sprintf(passwordBuffer,"%u", hash_str(password.c_str()));
	return passwordBuffer;
}
#define A 54059 /* a prime */
#define B 76963 /* another prime */
#define C 86969 /* yet another prime */
#define FIRSTH 37 /* also prime */

unsigned UsersDataBase::hash_str(const char* s){
	unsigned h = FIRSTH;
	while (*s) {
		h = (h * A) ^ (s[0] * B);
		s++;
	}
	return h; // or return h % C;
}


} /* namespace npl */
