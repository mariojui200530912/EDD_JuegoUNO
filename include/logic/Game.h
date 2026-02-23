
#ifndef JUEGOUNO_GAME_H
#define JUEGOUNO_GAME_H
#include "../structure/Stack.h"
#include "../structure/DCircularList.h"
#include "../entity/Card.h"
#include "GeneratorDeck.h"
#include "../view/GameUI.h"
#include "Rules.h"

class Game {
private:
    // --- COMPONENTES ---
    Stack mainDeck;
    Stack discardPile;
    DCircularList turnList;
    GeneratorDeck generator;

    // --- CONFIGURACION ---
    int numPlayers;
    bool useFlip;          // ¿Es UNO Flip?
    bool allowStacking;    // ¿Se puede tirar +2 sobre +2?
    bool allowChallengePlus4; // ¿Se permite retar el +4?
    bool allowWinWithWild;    // ¿Se permite ganar con carta negra?
    bool drawUntilPlayable; // True: Robar hasta encontrar carta. False: Robar 1 y pasar.
    bool manualUnoRule;     // True: Escribir "UNO". False: Automático.

    // --- ESTADO DEL JUEGO ---
    int maxDeckSize;
    bool isDarkStep;
    bool clockwise;
    bool gameFinished;
    int cardsToDraw;
    Color activeColor;

    // --- METODOS PRIVADOS ---
    void renderUI(Player* player);
    void handlePlayerTurn(Player* player, bool& skipTurn);
    void checkUnoAndWin(Player* player);
    void advanceTurn(bool& skipTurn);
    void performFlip();
    void applyCardEffect(Card card, bool& skipTurn);
    void drawCards(Player* player, int amount);
    void infiniteDraw(Player* player);
    void handlePlus4Challenge(Player* player, Card& playedCard, Color previousColor, int previousValue);
    void executeRoulette(bool isDark);
    void executeDrawUntilColor(bool& skipTurn);
    void executeSniper();

public:
    Game();
    void configureGame(); // ---- METODO DE CONFIGURACION
    void setupDeck();     // ---- CONFIGURACION DEL MAZO
    void loop();

};
#endif //JUEGOUNO_GAME_H