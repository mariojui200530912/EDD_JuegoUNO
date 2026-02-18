#include "../../include/structure/Stack.h"

Stack::Stack() : head(nullptr), size(0){};

void Stack::push(Card card)
{
    Node* newNode = new Node{card,head};
    head = newNode;
    size++;
};

Card Stack::pop()
{
    if (isEmpty()) return Card{};
    Node* aux = head;
    Card d = aux->card;
    head = head->next;
    delete aux;
    size--;
    return d;
}

Card Stack::peek()
{
    if (head != nullptr)
    {
        return head->card;
    }

    Card cardError;
    cardError.lightValue = -1;
    return cardError;
}

bool Stack::isEmpty()
{
    return head == nullptr;
}

int Stack::getSize()
{
    return size;
}

Stack::~Stack() {
    // Lógica para liberar los nodos de la pila
    while (head != nullptr) {
        Node* aux = head;
        head = head->next;
        delete aux;
    }
}