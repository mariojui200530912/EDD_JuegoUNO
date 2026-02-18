#ifndef JUEGOUNO_DLINKEDLIST_H
#define JUEGOUNO_DLINKEDLIST_H

#include "../entity/Card.h"
#include <iostream>

class DLinkedList
{
    private:
        struct Node
        {
            Card card;
            Node* next;
            Node* prev;
        };

        Node* head;
        Node* tail;
        int size;
        Node* split(Node* head);
        Node* merge(Node* first, Node* second, bool isDarkStep);
        Node* mergeSortRec(Node* head, bool isDarkStep);

    public:
        DLinkedList();
        ~DLinkedList();

        void insert(Card card);
        void insertionSort(bool isDarkStep);
        void mergeSort(bool isDarkStep);
        void pop();
        Card deleteByIndex(int index);
        Card getByIndex(int index);
        void showHand(bool isDarkStep);
        int getSize();
        bool isEmpty();
};
#endif //JUEGOUNO_DLINKEDLIST_H