#include "../../include/structure/Stack.h"
#include "../../include/entity/Card.h"
#include <cstdlib>
#include <ctime>

#ifndef JUEGOUNO_GENERATORDECK_H
#define JUEGOUNO_GENERATORDECK_H

class GeneratorDeck
{
    private:

    public:
    GeneratorDeck();
    ~GeneratorDeck();
    void swap(Card& first, Card& second);
    void initializeDeck(Stack& deckMain, int numPlayers, bool useFlip);
};
#endif //JUEGOUNO_GENERATORDECK_H