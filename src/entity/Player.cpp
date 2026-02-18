#include "../../include/entity/Player.h"

Player::Player(std::string name) : name(name) {}

void Player::addCard(Card card)
{
    hand.insert(card);
}

Card Player::playCard(int index)
{
    return hand.deleteByIndex(index);
}

DLinkedList& Player::getHand()
{
    return hand;
}

std::string Player::getName() { return name; }