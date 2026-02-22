#ifndef JUEGOUNO_GAMEUI_H
#define JUEGOUNO_GAMEUI_H
#include "../entity/Card.h"
#include "../entity/Player.h"
#include <string>

class GameUI {
public:
    static void showWelcomeScreen();
    static bool askToPlayAgain();
    //LIMPIA LA CONSOLA
    static void clearScreen();

    // PAUSA EL JUEGO HASTA QUE JUGADOR DE ENTER
    static void pauseGame(std::string message);

    // IMPRIME TITULOS
    static void printHeader(std::string title);

    // DIBUJA LA CARTA
    static void drawCardASCII(Card card, Color activeColor, bool isDarkStep);

    // IMPRIME EL TABLERO
    static void printTable(Card topCard, Color activeColor, bool isDarkStep, Player* currentPlayer, int cardsToDraw, bool allowStacking, int deckSize);

    // OBTIENE EL TEXTO DE LA CARTA
    static std::string getCardText(Card card, bool isDarkStep);
    // METDOS INPUT
    static Color askForColor(bool isDarkStep);
    static bool askForUno();
    static bool askToChallenge(std::string victimName);
};
#endif //JUEGOUNO_GAMEUI_H