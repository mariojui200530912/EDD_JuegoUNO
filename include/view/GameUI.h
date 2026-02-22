#ifndef JUEGOUNO_GAMEUI_H
#define JUEGOUNO_GAMEUI_H
#include "../entity/Card.h"
#include "../entity/Player.h"
#include <string>

class GameUI {
public:
    //LIMPIA LA CONSOLA
    static void clearScreen();

    // PAUSA EL JUEGO HASTA QUE JUGADOR DE ENTER
    static void pauseGame(std::string message);

    // IMPRIME TITULOS
    static void printHeader(std::string title);

    // DIBUJA LA CARTA
    static void drawCardASCII(Card card, Color activeColor, bool isDarkStep);

    // IMPRIME EL TABLERO
    static void printTable(Card topCard, Color activeColor, bool isDarkStep, Player* currentPlayer, int cardsToDraw, bool allowStacking);

    // OBTIENE EL TEXTO DE LA CARTA
    static std::string getCardText(Card card, bool isDarkStep);
};
#endif //JUEGOUNO_GAMEUI_H