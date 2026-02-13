//
// Created by Hp on 10/02/2026.
//

#ifndef JUEGOUNO_STACK_H
#define JUEGOUNO_STACK_H
#include "../entity/Card.h"
class Stack
{
    private:
    struct Nodo
    {
        Card card;
        Nodo* next;
    };
    Nodo* head;
    int counter;
    public:
    Stack() : head(nullptr), counter(0){};
    void push(Card card)
    {
        Nodo* newNode = new Nodo{c,head};
        head = newNode;
        counter++;
    };

    Card pop()
    {
        if (isEmpty()) return Card{};
        Nodo* aux = head;
        Card d = aux->card;
        head = head->next;
        delete aux;
        counter--;
        return d;
    }

    bool isEmpty() {return head == nullptr;}
    int getSize() {return counter;}
    
};
#endif //JUEGOUNO_STACK_H