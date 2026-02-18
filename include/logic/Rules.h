
#ifndef JUEGOUNO_RULES_H
#define JUEGOUNO_RULES_H
#include "../entity/Card.h"

class Rules
{
    private:
    public:
    Rules();
    ~Rules();
    bool isValidMove(Card played, Card tableTop, bool isDarkMode);
};
#endif //JUEGOUNO_RULES_H