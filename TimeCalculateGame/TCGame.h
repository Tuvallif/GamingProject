/*
 * TCGame.h
 *
 *  Created on: Aug 23, 2017
 *      Author: dmanzur
 */

#ifndef TCGAME_H_
#define TCGAME_H_

#define NUMBER_OF_QUESTIONS 5

#include "ISpecificGame.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <string>
#include <iostream>

using namespace std;

enum LowTimeDescription { SECONDS,MINUTES,HOURS};
enum HighTimeDescription { DAYS,WEEKS,YEARS};

class TCGame : public ISpecificGame {

private:

	string firstPlayer,secondPlayer;
    unsigned int scoreOfFirstPlayer,scoreOfSecondPlayer;
    bool ifGameEnded;
    unsigned int numberOfQuestion;
    bool isQuestionWasAnswered;
    int currAnswer;

    string gameWinner();
	string generateLowTimeDescriptions();
    string generateHighTimeDescriptions();
    string generateAtimeQuestion();
    void calcAnswerToQuestion(string lowTd,int numOfHighTd ,string highTd);

public:
//ISpecific game functions
	virtual bool isGameEnded();
	virtual string getIncomingMsgFromPeer(int player,string msg);
	virtual string endGame();
	virtual ~TCGame();
    virtual string startGame();
	//game functions
	TCGame(string player1name, string user2name);

};



#endif /* TCGAME_H_ */
