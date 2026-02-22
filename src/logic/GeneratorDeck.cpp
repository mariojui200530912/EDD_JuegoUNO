#include "../../include/logic/GeneratorDeck.h"

GeneratorDeck::GeneratorDeck(){};
GeneratorDeck::~GeneratorDeck(){};

void GeneratorDeck::swap(Card& first, Card& second)
{
    Card temp = first;
    first = second;
    second = temp;
}

void GeneratorDeck::initializeDeck(Stack& deckMain, int numPlayers, bool useFlip)
{
    // Calcular el numero de decks
    int numDecks = ((numPlayers - 1) / 6) + 1;

    // Si NO es flip, son 108 cartas. Si ES flip, son 120.
    int cardsInDeck = useFlip ? 120 : 108;
    int totalCards = numDecks * cardsInDeck;

    // Array temporal
    Card* lightSideDeck = new Card[totalCards];

    Card* darkSideDeck = nullptr;
    if (useFlip) {
        darkSideDeck = new Card[totalCards];
    }

    int idxLight = 0;
    int idxDark = 0;

    Color lightColors[] = {Color::RED, Color::YELLOW, Color::GREEN, Color::BLUE};

    //------ GENERAR LADO CLARO (Siempre se hace)
    for (int m = 0; m < numDecks; m++) {
        for (int i = 0; i < 4; i++) {
            Color c = lightColors[i];

            // 0 y 1-9
            if(idxLight < totalCards) lightSideDeck[idxLight++] = {c, 0, Type::NUMBER};
            for(int v=1; v<=9; v++) {
                if(idxLight < totalCards) lightSideDeck[idxLight++] = {c, v, Type::NUMBER};
                if(idxLight < totalCards) lightSideDeck[idxLight++] = {c, v, Type::NUMBER};
            }
            // Acciones
            for(int k=0; k<2; k++) {
                if(idxLight < totalCards) lightSideDeck[idxLight++] = {c, 20, Type::JUMP};
                if(idxLight < totalCards) lightSideDeck[idxLight++] = {c, 20, Type::REVERSE};
                if(idxLight < totalCards) lightSideDeck[idxLight++] = {c, 20, Type::DRAW_TO};

                // Solo agregamos la carta FLIP física si estamos en modo Flip
                if(useFlip && idxLight < totalCards) {
                    lightSideDeck[idxLight++] = {c, 20, Type::FLIP};
                }
            }
        }
        // Comodines Claros
        for(int j=0; j<4; j++) {
            if(idxLight < totalCards) lightSideDeck[idxLight++] = {Color::BLACK, 50, Type::WILD_CARD};
            if(idxLight < totalCards) lightSideDeck[idxLight++] = {Color::BLACK, 50, Type::DRAW_FOUR};
        }

        // --- CARTAS INVENTADAS (SOLO MODO FLIP) ---
        if (useFlip) {
            for(int j=0; j<2; j++) {
                // 2 Ruletas y 2 Francotiradores (Negras)
                if(idxLight < totalCards) lightSideDeck[idxLight++] = {Color::BLACK, 60, Type::ROULETTE};
                if(idxLight < totalCards) lightSideDeck[idxLight++] = {Color::BLACK, 60, Type::SNIPER};
            }
        }
    }

    // GENERAR LADO OSCURO (SOLO SI ES FLIP)
    if (useFlip) {
        Color darkColors[] = {Color::PINK, Color::TURQUOISE, Color::ORANGE, Color::PURPLE};

        for (int m = 0; m < numDecks; m++) {
            for (int i = 0; i < 4; i++) {
                Color c = darkColors[i];

                if(idxDark < totalCards) darkSideDeck[idxDark++] = {c, 1, Type::NUMBER};

                for(int v=1; v<=9; v++) {
                    if(idxDark < totalCards) darkSideDeck[idxDark++] = {c, v, Type::NUMBER};
                    if(idxDark < totalCards) darkSideDeck[idxDark++] = {c, v, Type::NUMBER};
                }

                for(int k=0; k<2; k++) {
                    if(idxDark < totalCards) darkSideDeck[idxDark++] = {c, 30, Type::JUMP_ALL};
                    if(idxDark < totalCards) darkSideDeck[idxDark++] = {c, 20, Type::REVERSE};
                    if(idxDark < totalCards) darkSideDeck[idxDark++] = {c, 50, Type::DRAW_SIX};

                    if(idxDark < totalCards) {
                        darkSideDeck[idxDark++] = {c, 20, Type::FLIP};
                    }
                }
            }
            for(int j=0; j<4; j++) {
                if(idxDark < totalCards) darkSideDeck[idxDark++] = {Color::BLACK, 100, Type::WILD_CARD};
                if(idxDark < totalCards) darkSideDeck[idxDark++] = {Color::BLACK, 100, Type::DRAW_UNTIL_COLOR};
            }

            // CARTAS INVENTADAS (SU REVERSO OSCURO) ---
            for(int j=0; j<2; j++) {
                if(idxDark < totalCards) darkSideDeck[idxDark++] = {Color::BLACK, 100, Type::ROULETTE};
                if(idxDark < totalCards) darkSideDeck[idxDark++] = {Color::BLACK, 100, Type::SNIPER};
            }
        }
    }

    // BARAJADO
    std::srand(std::time(0));

    if (idxLight > 0) {
        for (int i = idxLight - 1; i > 0; i--) {
            int j = std::rand() % (i + 1);
            Card temp = lightSideDeck[i];
            lightSideDeck[i] = lightSideDeck[j];
            lightSideDeck[j] = temp;
        }
    }

    if (useFlip && idxDark > 0) {
        for (int i = idxDark - 1; i > 0; i--) {
            int j = std::rand() % (i + 1);
            Card temp = darkSideDeck[i];
            darkSideDeck[i] = darkSideDeck[j];
            darkSideDeck[j] = temp;
        }
    }

    // Fusionar las cartas lado oscuro y lado claro
    for (int i = 0; i < totalCards; i++)
    {
        Card finalCard;

        // Copiamos siempre el lado claro
        if (i < idxLight) {
            finalCard.lightColor = lightSideDeck[i].lightColor;
            finalCard.lightValue = lightSideDeck[i].lightValue;
            finalCard.lightType = lightSideDeck[i].lightType;
        }

        if (useFlip && i < idxDark) {
            // Si es Flip, copiamos del mazo oscuro generado
            finalCard.darkColor = darkSideDeck[i].lightColor; // Ojo: darkSideDeck usa struct Card temporalmente
            finalCard.darkValue = darkSideDeck[i].lightValue;
            finalCard.darkType = darkSideDeck[i].lightType;
        } else {
            // Si es Normal, llenamos con valores "Nulos" o por defecto para evitar basura
            finalCard.darkColor = Color::BLACK;
            finalCard.darkValue = -1;
            finalCard.darkType = Type::NUMBER;
        }

        deckMain.push(finalCard);
    }

    delete[] lightSideDeck;
    if (useFlip) {
        delete[] darkSideDeck;
    }
}

void GeneratorDeck::refillMainDeck(Stack& mainDeck, Stack& discardPile, int maxDeckSize) {
    if (discardPile.isEmpty()) {
        std::cout << ANSI_RED << "[!] Error: No hay cartas en el descarte para recargar." << ANSI_RESET << std::endl;
        return;
    }

    std::cout << ANSI_YELLOW << "\n[!] El mazo se ha agotado. Rebarajando el descarte..." << ANSI_RESET << std::endl;

    // Guardamos la carta superior (la que está en juego)
    Card topCard = discardPile.pop();

    // Extraemos el resto de cartas a un array temporal para barajar
    Card* tempCards = new Card[maxDeckSize];
    int count = 0;

    while (!discardPile.isEmpty()) {
        if (count < maxDeckSize) {
            tempCards[count++] = discardPile.pop();
        } else {
            discardPile.pop();
        }
    }

    // Barajar (Fisher-Yates)
    for (int i = count - 1; i > 0; i--) {
        int j = std::rand() % (i + 1);
        Card temp = tempCards[i];
        tempCards[i] = tempCards[j];
        tempCards[j] = temp;
    }

    for (int i = 0; i < count; i++) {
        mainDeck.push(tempCards[i]);
    }

    discardPile.push(topCard);
    delete[] tempCards;

    std::cout << ANSI_GREEN << "[OK] Nuevo mazo listo con " << count << " cartas." << ANSI_RESET << std::endl;
}
