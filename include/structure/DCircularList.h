
#ifndef JUEGOUNO_DCIRCULARLIST_H
#define JUEGOUNO_DCIRCULARLIST_H
#include "../entity/Player.h"
#include <iostream>

class DCircularList
{
    private:
        struct Node{
            Player* player;
            Node* next;
            Node* prev;
        };

        Node* head;
        Node* current;
        int size;
    public:
        DCircularList();
        ~DCircularList();
        void add(Player* player);
        // Para turnos
        Player* getCurrentPlayer();
        void passTurn(bool clockwise);
        // Utilidades
        void showPlayers();
        int getSize();
};
#endif //JUEGOUNO_DCIRCULARLIST_H