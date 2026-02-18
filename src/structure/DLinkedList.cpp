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

void DLinkedList::showHand(bool isDarkStep)
{
    Node* current = head;
    int i = 1;

    if (current == nullptr) {
        std::cout << "   (Mano vacia)" << std::endl;
        return;
    }

    while (current != nullptr)
    {
        // 1. Determinar atributos según el lado
        Color c = isDarkStep ? current->card.darkColor : current->card.lightColor;
        int val = isDarkStep ? current->card.darkValue : current->card.lightValue;
        Type t  = isDarkStep ? current->card.darkType  : current->card.lightType;

        // 2. Obtener color usando la utilidad compartida
        std::string colorCode = getCardColorANSI(c);

        // 3. Imprimir
        std::cout << i << ". " << colorCode << "[";

        // Lógica de visualización de texto (Simplificada)
        if (t == Type::NUMBER) {
            std::cout << val;
        } else {
            // Puedes hacer otra función auxiliar para esto si quieres limpiar más
            if (t == Type::JUMP || t == Type::JUMP_ALL) std::cout << "S"; // Skip
            else if (t == Type::REVERSE) std::cout << "R";
            else if (t == Type::DRAW_TO) std::cout << "+2";
            else if (t == Type::DRAW_SIX) std::cout << "+5";
            else if (t == Type::WILD_CARD) std::cout << "W";
            else if (t == Type::CUSTOM || t == Type::DRAW_UNTIL_COLOR) std::cout << "+W";
            else if (t == Type::FLIP) std::cout << "FLIP";
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

// Función auxiliar global (o estática) para comparar
bool compareCards(Card a, Card b, bool isDarkStep) {
    if (isDarkStep) {
        if (a.darkColor != b.darkColor) return a.darkColor < b.darkColor;
        return a.darkValue < b.darkValue;
    } else {
        if (a.lightColor != b.lightColor) return a.lightColor < b.lightColor;
        return a.lightValue < b.lightValue;
    }
}

// --- ESTRATEGIA 1: INSERTION SORT (Para Robar) ---
// Intercambia DATOS, no punteros (más seguro para ajustes pequeños)
void DLinkedList::insertionSort(bool isDarkStep) {
    if (!head || !head->next) return;

    Node* current = head->next;
    while (current != nullptr) {
        Node* keyNode = current;
        Node* prevNode = current->prev;
        Node* nextIter = current->next; // Guardamos el siguiente para no perdernos

        // Movemos hacia atrás intercambiando DATOS (más seguro que mover punteros)
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

// --- ESTRATEGIA 2: MERGE SORT (Para FLIP) ---

// Función pública que llama al proceso recursivo y arregla head/tail
void DLinkedList::mergeSort(bool isDarkStep) {
    if (!head || !head->next) return;

    head = mergeSortRec(head, isDarkStep);

    // Reconstruir el puntero TAIL (necesario porque los punteros cambiaron)
    Node* temp = head;
    while (temp->next != nullptr) {
        temp = temp->next;
    }
    tail = temp;
}

// 1. Dividir la lista en dos mitades
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

// 2. Fusionar dos sub-listas ordenadas
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

// 3. Función recursiva principal
DLinkedList::Node* DLinkedList::mergeSortRec(Node* node, bool isDarkStep) {
    if (!node || !node->next) return node;

    Node* second = split(node);

    // Recurrencia
    node = mergeSortRec(node, isDarkStep);
    second = mergeSortRec(second, isDarkStep);

    // Fusión
    return merge(node, second, isDarkStep);
}