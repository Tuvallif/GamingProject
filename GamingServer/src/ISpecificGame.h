#ifndef ISPECIFICGAME_H_
#define ISPECIFICGAME_H_

#define BOTH_PLAYERS 0
#define FIRST_PLAYER 1
#define SECOND_PLAYER 2

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <iostream>

using namespace std;

class ISpecificGame {
 public:
        virtual ~ISpecificGame(){}
        virtual bool isGameEnded() = 0;
        virtual string endGame() = 0;
        virtual string getIncomingMsgFromPeer(int player,string msg) = 0;
        virtual string startGame()=0;
};
#endif /* ISPECIFICGAME_H_ */
