//
// Created by Hp on 10/02/2026.
//

#ifndef JUEGOUNO_STACK_H
#define JUEGOUNO_STACK_H
#include "../entity/Card.h"
class Stack
{
    private:
        struct Node
        {
            Card card;
            Node* next;
        };
        Node* head;
        int size;
    public:
        Stack();
        ~Stack();
        void push(Card card);
        Card pop();
        Card peek();
        bool isEmpty();
        int getSize();
};
#endif //JUEGOUNO_STACK_H