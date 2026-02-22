#include "../../include/logic/Rules.h"
#include "../../include/utils/ConsoleColors.h"
#include <iostream>
#include <cstdlib>

bool Rules::validateMove(Card play, Card top, bool isDarkStep, Color activeColor) {
    if (!isDarkStep) {
        if (play.lightType == Type::WILD_CARD || play.lightType == Type::DRAW_FOUR ||
            play.lightType == Type::ROULETTE || play.lightType == Type::SNIPER) return true;

        if (top.lightColor == Color::BLACK) return (play.lightColor == activeColor);

        if (play.lightType != Type::NUMBER && top.lightType != Type::NUMBER) {
            if (play.lightType == top.lightType) return true;
        }

        return (play.lightColor == top.lightColor || play.lightValue == top.lightValue);
    } else {
        if (play.darkType == Type::WILD_CARD || play.darkType == Type::DRAW_UNTIL_COLOR ||
            play.darkType == Type::DRAW_SIX || play.darkType == Type::ROULETTE ||
            play.darkType == Type::SNIPER) return true;

        if (top.darkColor == Color::BLACK) return (play.darkColor == activeColor);

        if (play.darkType != Type::NUMBER && top.darkType != Type::NUMBER) {
            if (play.darkType == top.darkType) return true;
        }

        return (play.darkColor == top.darkColor || play.darkValue == top.darkValue);
    }
}

bool Rules::checkBluff(Player* challenger, Player* bluffer, Color neededColor, int neededValue, bool isDarkStep) {
    std::cout << ANSI_RED << ANSI_BOLD << "\n!!! " << challenger->getName() << " DESAFIA EL +4 DE "
              << bluffer->getName() << " !!!" << ANSI_RESET << std::endl;

    std::cout << "Revisando la mano de " << bluffer->getName() << "..." << std::endl;

    bool hasMatch = false;
    DLinkedList& hand = bluffer->getHand();

    for(int i=1; i <= hand.getSize(); i++) {
        Card c = hand.getByIndex(i);

        if (!isDarkStep) {
            bool matchColor = (c.lightColor == neededColor);
            bool matchValue = (c.lightValue == neededValue);

            if (matchColor || matchValue) {
                hasMatch = true;
                std::cout << "Carta culpable encontrada: " << getCardColorANSI(c.lightColor)
                          << "[" << (matchColor ? "Color" : "Numero") << " Coincide]" << ANSI_RESET << std::endl;
                break;
            }
        }
    }

    if (hasMatch) {
        std::cout << ANSI_GREEN << "¡DESAFIO GANADO! " << bluffer->getName() << " mintio. Tenia carta jugable." << ANSI_RESET << std::endl;
        return true;
    } else {
        std::cout << ANSI_RED << "¡DESAFIO FALLIDO! " << bluffer->getName() << " jugo legal. "
                  << challenger->getName() << " roba 6 cartas." << ANSI_RESET << std::endl;
        return false;
    }
}
