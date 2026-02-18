#include "../../include/structure/DCircularList.h"

DCircularList::DCircularList() : head(nullptr), current(nullptr), size(0){}

void DCircularList::add(Player* player)
{
    Node* turnNew = new Node{player, nullptr, nullptr};
    if (head == nullptr)
    {
        head = turnNew;
        head->next = head;
        head->prev = head;
        current = head;
    } else
    {
        Node* last = head->prev;
        turnNew->next = head;
        turnNew->prev = last;
        last->next = turnNew;
        head->prev = turnNew;
    }
    size++;
}

Player* DCircularList::getCurrentPlayer()
{
    return current->player;
}

void DCircularList::passTurn(bool clockwise)
{
    if (size == 0) return;
    if (clockwise)
    {
        current = current->next;
    }else
    {
        current = current->prev;
    }
}

void DCircularList::showPlayers()
{
    if (head == nullptr) return;
    Node* temp = head;
    do
    {
        if (temp == current) std::cout << "->";
        else std::cout << " ";

        std::cout << temp->player->getName() << std::endl;
        temp = temp->next;
    }while (temp != head);
}

int DCircularList::getSize(){ return size; }


DCircularList::~DCircularList() {
    if (head != nullptr) {
        Node* actual = head;
        head->prev->next = nullptr;

        while (current != nullptr) {
            Node* next = current->next;
            delete current;
            current = next;
        }
    }
}