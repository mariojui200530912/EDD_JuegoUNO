#ifndef JUEGOUNO_PLAYER_H
#define JUEGOUNO_PLAYER_H
#include <string>
#include "../structure/DLinkedList.h"

class Player
{
    private:
        std::string name;
        DLinkedList hand;
    public:
        Player(std::string name);
        void addCard(Card card);
        Card playCard(int index);
        DLinkedList& getHand();
        std::string getName();
};

#endif //JUEGOUNO_PLAYER_H