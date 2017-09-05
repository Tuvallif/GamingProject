/*
 * TCGame.cpp
 *
 *  Created on: Aug 23, 2017
 *      Author: dmanzur
 */

#include "TCGame.h"

TCGame::TCGame(string player1name, string player2name) {
	// TODO Auto-generated constructor stub
	this->firstPlayer = player1name;
	this->secondPlayer = player2name;
	this->scoreOfFirstPlayer = 0;
	this->scoreOfSecondPlayer = 0;
	this->ifGameEnded = false;
	this->numberOfQuestion = 1;
	this->isQuestionWasAnswered = false;
	this->currAnswer = 0;

}

string TCGame::gameWinner() {

	if (scoreOfFirstPlayer > scoreOfSecondPlayer)
		return this->firstPlayer;
	else if (scoreOfFirstPlayer < scoreOfSecondPlayer)
		return this->secondPlayer;
	else
		return "none";

}

bool TCGame::isGameEnded() {

	return this->ifGameEnded;
}

string TCGame::getTheGameWinner(){
	return this->theGameWinner;
}

string TCGame::getIncomingMsgFromPeer(int player, string msg) {


	char answerToPlayer[300];
	memset(answerToPlayer, 0, sizeof(answerToPlayer));


	strcpy(answerToPlayer, "\nPlayer ");
	char n[5];
	sprintf(n,"%d",player);
	strcat(answerToPlayer, n);
	strcat(answerToPlayer, " has answered first");

	bool isGoodAnswer = false;
	int num = atoi(msg.c_str());
	if (num == this->currAnswer)
		isGoodAnswer = true;

	if (isGoodAnswer) {
		strcat(answerToPlayer, " and correctly!\n");
		if (player == FIRST_PLAYER)
			scoreOfFirstPlayer++;
		else
			scoreOfSecondPlayer++;
	} else {
		strcat(answerToPlayer, " and wrong!\n");
		if (player == FIRST_PLAYER)
			scoreOfSecondPlayer++;
		else
			scoreOfFirstPlayer++;
	}

	strcat(answerToPlayer, "score status: ");
	strcat(answerToPlayer, this->firstPlayer.c_str());
	char  score1[4] ;
	sprintf(score1,"%d  " ,scoreOfFirstPlayer);
	strcat(answerToPlayer, score1);

	strcat(answerToPlayer, this->secondPlayer.c_str());
	char  score2[4] ;
	sprintf(score2,"%d%s" ,scoreOfSecondPlayer,"\n");
	strcat(answerToPlayer, score2);

	if (NUMBER_OF_QUESTIONS+1 == numberOfQuestion ) {
		ifGameEnded = true;
		this->theGameWinner = gameWinner();
		strcat(answerToPlayer, theGameWinner.c_str());
		strcat(answerToPlayer,"  is the winner !");
		strcat(answerToPlayer,"\nGood game! ");
		return answerToPlayer;
	}
	//return the right msg
	strcat(answerToPlayer, "Next question:\n");
	string nq = generateAtimeQuestion();
	strcat(answerToPlayer, nq.c_str());
	strcat(answerToPlayer, "?");


	return answerToPlayer;

}

string TCGame::startGame() {

	string msg = "Welcome to the TIME CALCULATE game !\n";
	msg.append(
			"You have to answer right to the following 5 questions as fast as possible\n");
	msg.append("You will get a point for each right answer\n");
	msg.append("Your component will get a point for each wrong answer\n\n");
	msg.append("Here we go !\n");

	string q = generateAtimeQuestion();
	msg.append(q);

	return msg;

}

string TCGame::endGame() {

	string endGameMsg =
			"The game is over since session was closed\nThere is no winner in this round\nGAME OVER";

	this->theGameWinner = "none";
	return endGameMsg;
}

void TCGame::calcAnswerToQuestion(string lowTd, int numOfHighTd,
		string highTd) {

	int answer = 0;
	if (lowTd == "seconds")
		answer = 60 * 60 * 24;
	else if (lowTd == "minutes")
		answer = 60 * 24;
	else if (lowTd == "hours")
		answer = 24;

	answer *= numOfHighTd;

	if (highTd == "days")
		answer *= 1;
	else if (highTd == "weeks")
		answer *= 7;
	else if (highTd == "years")
		answer *= 365;

	this->currAnswer = answer;
	cout << "the answer is: " << this->currAnswer << endl;
}

string TCGame::generateAtimeQuestion() {

	char ret[50];
	memset(ret, 0, sizeof(ret));
	srand(time(0));
	int num = rand() % 3 + 1;

	sprintf(ret,"%s%d%s%d\n","Question ",numberOfQuestion,"/",5);
	strcat(ret, "How many ");
	string lowTd = generateLowTimeDescriptions();
	strcat(ret, lowTd.c_str());
	strcat(ret, " has in ");
	char n[5];
	sprintf(n, "%d", num);
	strcat(ret, n);
	strcat(ret, " ");
	string highTd = generateHighTimeDescriptions();
	strcat(ret, highTd.c_str());

	calcAnswerToQuestion(lowTd, num, highTd);
	numberOfQuestion++;
	return string(ret);

}

string TCGame::generateLowTimeDescriptions() {
	srand(time(0));
	int num = rand() % 3;

	switch (num) {
	case SECONDS:
		return "seconds";
	case MINUTES:
		return "minutes";
	case HOURS:
		return "hours";
	default:
		return "seconds";
	}
	return "";
}
string TCGame::generateHighTimeDescriptions() {
	srand(time(0));
	int num = rand() % 3;

	switch (num) {
	case DAYS:
		return "days";
	case WEEKS:
		return "weeks";
	case YEARS:
		return "years";
	default:
		return "days";
	}
	return "";
}

TCGame::~TCGame() {
	// TODO Auto-generated destructor stub
}

