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
    // 1. Calcular tamaño total
    int numDecks = ((numPlayers - 1) / 6) + 1;

    // Si NO es flip, son 108 cartas. Si ES flip, son 116.
    int cardsInDeck = useFlip ? 116 : 108;
    int totalCards = numDecks * cardsInDeck;

    // 2. Arrays temporales
    // Siempre necesitamos el lado claro
    Card* lightSideDeck = new Card[totalCards];

    // Solo creamos el array oscuro si vamos a jugar FLIP
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
            if(idxLight < totalCards) lightSideDeck[idxLight++] = {Color::BLACK, 50, Type::CUSTOM};
        }
    }

    // ==========================================
    // B. GENERAR LADO OSCURO (SOLO SI ES FLIP)
    // ==========================================
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

                    if(idxDark < totalCards) { // Ya validamos useFlip arriba
                        darkSideDeck[idxDark++] = {c, 20, Type::FLIP};
                    }
                }
            }
            for(int j=0; j<4; j++) {
                if(idxDark < totalCards) darkSideDeck[idxDark++] = {Color::BLACK, 100, Type::WILD_CARD};
                if(idxDark < totalCards) darkSideDeck[idxDark++] = {Color::BLACK, 100, Type::DRAW_UNTIL_COLOR};
            }
        }
    }

    // 3. BARAJADO
    std::srand(std::time(0));

    // Barajar Lado Claro (Siempre)
    if (idxLight > 0) {
        for (int i = idxLight - 1; i > 0; i--) {
            int j = std::rand() % (i + 1);
            Card temp = lightSideDeck[i];
            lightSideDeck[i] = lightSideDeck[j];
            lightSideDeck[j] = temp;
        }
    }

    // OPTIMIZACIÓN: Barajar Lado Oscuro (SOLO SI ES FLIP)
    if (useFlip && idxDark > 0) {
        for (int i = idxDark - 1; i > 0; i--) {
            int j = std::rand() % (i + 1);
            Card temp = darkSideDeck[i];
            darkSideDeck[i] = darkSideDeck[j];
            darkSideDeck[j] = temp;
        }
    }

    // 4. FUSIÓN Y LLENADO DE PILA
    for (int i = 0; i < totalCards; i++)
    {
        Card finalCard;

        // Copiamos siempre el lado claro
        if (i < idxLight) {
            finalCard.lightColor = lightSideDeck[i].lightColor;
            finalCard.lightValue = lightSideDeck[i].lightValue;
            finalCard.lightType = lightSideDeck[i].lightType;
        }

        // LÓGICA CONDICIONAL PARA EL LADO OSCURO
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

    // 5. LIMPIEZA
    delete[] lightSideDeck;

    // OPTIMIZACIÓN: Solo borramos si lo creamos
    if (useFlip) {
        delete[] darkSideDeck;
    }
}
