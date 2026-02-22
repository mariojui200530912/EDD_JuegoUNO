#ifndef JUEGOUNO_GENERATORDECK_H
#define JUEGOUNO_GENERATORDECK_H
#include "../../include/structure/Stack.h"
#include "../../include/entity/Card.h"
#include "../../include/utils/ConsoleColors.h"
#include <iostream>
#include <cstdlib>
#include <ctime>

class GeneratorDeck
{
    private:

    public:
    GeneratorDeck();
    ~GeneratorDeck();
    void swap(Card& first, Card& second);
    void initializeDeck(Stack& deckMain, int numPlayers, bool useFlip);
    static void refillMainDeck(Stack& mainDeck, Stack& discardPile, int maxDeckSize);
};
#endif //JUEGOUNO_GENERATORDECK_H