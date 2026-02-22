#ifndef JUEGOUNO_GAMESETTINGS_H
#define JUEGOUNO_GAMESETTINGS_H
#include <string>
#include "../structure/DCircularList.h"

class GameSettings {
public:
    int numPlayers;
    bool useFlip;
    bool allowStacking;
    bool allowChallengePlus4;
    bool drawUntilPlayable;
    bool manualUnoRule;
    bool allowWinWithWild;

    GameSettings();

    // este metodo ejecuta las pregustans en consola
    void promptUser(DCircularList& turnList);
};
#endif //JUEGOUNO_GAMESETTINGS_H