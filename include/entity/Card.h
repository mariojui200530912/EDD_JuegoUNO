//
// Created by Hp on 10/02/2026.
//

#ifndef JUEGOUNO_CARD_H
#define JUEGOUNO_CARD_H
#include <string>

enum class Color { RED, GREEN, BLUE, YELLOW, PINK, TURQUOISE, ORANGE, VIOLET, BLACK  };
enum class Type {NUMBER, JUMP, REVERSE, DRAW_TO, WILD_CARD, FLIP, CUSTOM};

struct Card
{
    Color lightColor;
    int lightValue;
    Type lightType;

    Color darkColor;
    int darkValue;
    Type darkType;

    Card* nextCard = nullptr;
    Card* previousCard = nullptr;

};

#endif //JUEGOUNO_CARD_H