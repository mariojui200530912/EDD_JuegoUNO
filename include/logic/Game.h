
#ifndef JUEGOUNO_GAME_H
#define JUEGOUNO_GAME_H
#include "../structure/Stack.h"
#include "../structure/DCircularList.h"
#include "../entity/Card.h"
#include "GeneratorDeck.h"
#include <string>

class Game {
private:
    // --- COMPONENTES ---
    Stack mainDeck;
    Stack discardPile;
    DCircularList turnList;
    GeneratorDeck generator;

    // --- CONFIGURACIÓN (Variables nuevas) ---
    int numPlayers;
    bool useFlip;          // ¿Es UNO Flip?
    bool allowStacking;    // ¿Se puede tirar +2 sobre +2?
    bool showAnsiColors;   // ¿Mostrar colores en consola?

    // --- ESTADO DEL JUEGO ---
    bool isDarkStep;
    bool clockwise;
    bool gameFinished;
    int cardsToDraw;
    Color activeColor;

    // --- MÉTODOS PRIVADOS ---
    void printHeader(std::string title); // Para decorar el menú
    // ... tus otros métodos privados ...

public:
    Game();
    void configureGame(); // <--- EL NUEVO MÉTODO IMPORTANTE
    void setupDeck();     // Separamos la configuración del mazo
    void loop();
    bool validateMove(Card play, Card top);
    void applyCardEffect(Card card, bool& skipTurn);
    void performFlip();
};
#endif //JUEGOUNO_GAME_H