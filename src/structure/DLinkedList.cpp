#include "../../include/structure/DLinkedList.h"
#include "../../include/utils/ConsoleColors.h"

#include <algorithm>

DLinkedList::DLinkedList() : head(nullptr), tail(nullptr), size(0) {}

DLinkedList::~DLinkedList()
{
    Node* current = head;
    while (current != nullptr)
    {
        Node* nextNode = current->next;
        delete current;
        current = nextNode;
    }
    head = nullptr;
    tail = nullptr;
}

void DLinkedList::insert(Card card)
{
    Node* newNode = new Node;
    newNode->card = card;
    newNode->next = nullptr;
    newNode->prev = nullptr;
    if (head == nullptr)
    {
        head = newNode;
        tail = newNode;
    }
    else
    {
        tail->next = newNode;
        newNode->prev = tail;
        tail = newNode;
    }
    size++;
}

Card DLinkedList::deleteByIndex(int index)
{
    if (index < 1 || index > size || head == nullptr)
    {
        return Card{Color::BLACK, -1, Type::NUMBER};
    }

    Node* current = head;
    int counter = 1;
    while (counter < index && current != nullptr)
    {
        current = current->next;
        counter++;
    }
    Card playedCard = current->card;
    if (current == head)
    {
        head = head->next;
        if (head) head->prev = nullptr;
    }
    else if (current == tail)
    {
        tail = tail->prev;
        if (tail) tail->next = nullptr;
    }
    else
    {
        current->prev->next = current->next;
        current->next->prev = current->prev;
    }
    if (head == nullptr) tail = nullptr;
    delete current;
    size--;
    return playedCard;
}

Card DLinkedList::getByIndex(int index)
{
    if (index < 1 || index > size) return Card{};
    Node* current = head;
    for (int i = 1; i < index; i++)
    {
        current = current->next;
    }
    return current->card;
}

void DLinkedList::showHand(bool isDarkStep) {
    Node* current = head;
    int i = 1;

    if (current == nullptr) {
        std::cout << "   (Mano vacia)" << std::endl;
        return;
    }

    while (current != nullptr) {
        Color c = isDarkStep ? current->card.darkColor : current->card.lightColor;
        int val = isDarkStep ? current->card.darkValue : current->card.lightValue;
        Type t  = isDarkStep ? current->card.darkType  : current->card.lightType;

        std::string colorCode = getCardColorANSI(c);

        std::cout << colorCode << i << ". [";

        if (t == Type::NUMBER) {
            std::cout << val;
        } else {
            // LADO CLARO
            if (!isDarkStep) {
                if (t == Type::JUMP) std::cout << "SALTO";
                else if (t == Type::REVERSE) std::cout << "REVERSA";
                else if (t == Type::DRAW_TO) std::cout << "+2";
                else if (t == Type::DRAW_FOUR) std::cout << "+4";
                else if (t == Type::WILD_CARD) std::cout << "COLOR";
                else if (t == Type::FLIP) std::cout << "FLIP";
                else if (t == Type::ROULETTE) std::cout << "RULETA";
                else if (t == Type::SNIPER) std::cout << "SNIPER";
                else std::cout << "ESPECIAL";
            }
            // LADO OSCURO
            else {
                if (t == Type::JUMP_ALL) std::cout << "S-TODO";
                else if (t == Type::REVERSE) std::cout << "REVERSA";
                else if (t == Type::DRAW_TO) std::cout << "+3";
                else if (t == Type::DRAW_SIX) std::cout << "+6";
                else if (t == Type::DRAW_UNTIL_COLOR) std::cout << "+COLOR";
                else if (t == Type::WILD_CARD) std::cout << "COLOR";
                else if (t == Type::FLIP) std::cout << "FLIP";
                else if (t == Type::ROULETTE) std::cout << "RULETA";
                else if (t == Type::SNIPER) std::cout << "SNIPER";
                else std::cout << "ESPECIAL";
            }
        }

        std::cout << "]" << ANSI_RESET << "  ";

        current = current->next;
        i++;
    }
    std::cout << std::endl;
}

int DLinkedList::getSize()
{
    return size;
}


bool DLinkedList::isEmpty()
{
    return size == 0;
}

bool compareCards(Card a, Card b, bool isDarkStep) {
    if (isDarkStep) {
        if (a.darkColor != b.darkColor) return a.darkColor < b.darkColor;
        return a.darkValue < b.darkValue;
    } else {
        if (a.lightColor != b.lightColor) return a.lightColor < b.lightColor;
        return a.lightValue < b.lightValue;
    }
}

//-----INSERTION SORT
void DLinkedList::insertionSort(bool isDarkStep) {
    if (!head || !head->next) return;

    Node* current = head->next;
    while (current != nullptr) {
        Node* keyNode = current;
        Node* prevNode = current->prev;
        Node* nextIter = current->next; // Guardamos el siguiente para no perdernos

        // Movemos hacia atrás intercambiando DATOS
        while (prevNode != nullptr && compareCards(keyNode->card, prevNode->card, isDarkStep)) {
            // Swap Data
            Card temp = keyNode->card;
            keyNode->card = prevNode->card;
            prevNode->card = temp;

            // Retroceder
            keyNode = prevNode;
            prevNode = prevNode->prev;
        }
        current = nextIter;
    }
}

// --- MERGE SORT (Para FLIP) ---

void DLinkedList::mergeSort(bool isDarkStep) {
    if (!head || !head->next) return;

    head = mergeSortRec(head, isDarkStep);
    Node* temp = head;
    while (temp->next != nullptr) {
        temp = temp->next;
    }
    tail = temp;
}
DLinkedList::Node* DLinkedList::split(Node* source) {
    Node* fast = source;
    Node* slow = source;

    while (fast->next && fast->next->next) {
        fast = fast->next->next;
        slow = slow->next;
    }
    Node* temp = slow->next;
    slow->next = nullptr;
    if (temp) temp->prev = nullptr; // Romper enlace hacia atrás
    return temp;
}

// Fusionar dos sub-listas ordenadas
DLinkedList::Node* DLinkedList::merge(Node* first, Node* second, bool isDarkStep) {
    if (!first) return second;
    if (!second) return first;

    Node* result = nullptr;

    // Usamos la función de comparación
    if (compareCards(first->card, second->card, isDarkStep)) {
        result = first;
        result->next = merge(first->next, second, isDarkStep);
        if (result->next) result->next->prev = result; // Arreglar prev
        result->prev = nullptr;
    } else {
        result = second;
        result->next = merge(first, second->next, isDarkStep);
        if (result->next) result->next->prev = result; // Arreglar prev
        result->prev = nullptr;
    }
    return result;
}

// Función recursiva principal
DLinkedList::Node* DLinkedList::mergeSortRec(Node* node, bool isDarkStep) {
    if (!node || !node->next) return node;

    Node* second = split(node);

    // Recurrencia
    node = mergeSortRec(node, isDarkStep);
    second = mergeSortRec(second, isDarkStep);

    // Fusión
    return merge(node, second, isDarkStep);
}