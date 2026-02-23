
#ifndef JUEGOUNO_RULES_H
#define JUEGOUNO_RULES_H
#include "../entity/Card.h"
#include "../entity/Player.h"
#include "../structure/DCircularList.h"

class Rules {
public:
    // Metodo para validar si la jugada es buena
    static bool validateMove(Card play, Card top, bool isDarkStep, Color activeColor);
    static bool canStack(Card play, Card top, bool isDarkStep); // verifica si se puede acumular
    static bool isWildCard(Card card, bool isDarkStep); // verifica si carta es negra/comodin

    // Verifica si el desafío del +4 fue exitoso (Retorna true si el jugador mintió)
    static bool checkBluff(Player* challenger, Player* bluffer, Color neededColor, int neededValue, bool isDarkStep);
};
#endif //JUEGOUNO_RULES_H