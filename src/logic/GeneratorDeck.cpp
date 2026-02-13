#include "../../include/structure/Stack.h"
#include "../../include/entity/Card.h"
#include <cstdlib>
#include <ctime>

void swap(Card& first, Card& second)
{
    Card temp = first;
    first = second;
    second = temp;
}

void initializeDeck(Stack& deckMain, int numPlayers, bool useFlip)
{
    int numDecks = ((numPlayers - 1)/6)+1;
    int cardsInDeck = useFlip ? 116 : 108;
    int totalCards = numDecks * cardsInDeck;

    Card* deckTemporal = new Card[totalCards];
    int indice = 0;

    Color colors[] = {Color::RED, Color::YELLOW, Color::GREEN, Color::BLUE};
    for (int m = 0; m < numDecks; m++)
    {
        for (int i = 0; i < 4; i++)
        {
            Color colorCurrent = colors[i];
            deckTemporal[indice++] = {colorCurrent, Type::NUMBER};

            for (int val=1; val <=9; val++)
            {
                deckTemporal[indice++] = {colorCurrent, val, Type::NUMBER};
                deckTemporal[indice++] = {colorCurrent, val, Type::NUMBER};
            }

            for (int k=0; k<2; k++)
            {
                deckTemporal[indice++] = {colorCurrent, 20, Type::JUMP};
                deckTemporal[indice++] = {colorCurrent, 20, Type::REVERSE};
                deckTemporal[indice++] = {colorCurrent, 20, Type::DRAW_TO};
            }

            if (useFlip)
            {
                deckTemporal[indice++] = {colorCurrent, 20, Type::FLIP};
                deckTemporal[indice++] = {colorCurrent, 20, Type::FLIP};
            }
        }

        for (int j=0; j<4; j++)
        {
            deckTemporal[indice++] = {Color::BLACK, 50, Type::WILD_CARD};
            deckTemporal[indice++] = {Color::BLACK, 50, Type::CUSTOM};
        }

        std::srand(std::time(0));

        for (int i = totalCards-1; i >= 0; i--)
        {
            int j = std::rand() % (i + 1);
            swap(deckTemporal[i], deckTemporal[j]);
        }

        for (int i=0; i<totalCards; i++)
        {
            deckMain.push(deckTemporal[i]);
        }

        delete[] deckTemporal;
    }
}