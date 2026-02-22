#include "../../include/view/GameUI.h"
#include "../../include/utils/ConsoleColors.h"
#include <iostream>
#include <limits>
#include <iomanip>

void GameUI::clearScreen() {
    // CODIGO ANSI LIMPIA CONSOLA
    std::cout << "\033[2J\033[1;1H";
}

void GameUI::pauseGame(std::string message) {
    std::cout << "\n" << ANSI_YELLOW << message << ANSI_RESET;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    std::cin.get(); // Espera ENTER
}

void GameUI::printHeader(std::string title) {
    std::cout << "\n" << ANSI_CYAN << "==========================================================" << ANSI_RESET << std::endl;
    std::cout << "      " << ANSI_BOLD << title << ANSI_RESET << std::endl;
    std::cout << ANSI_CYAN << "==========================================================" << ANSI_RESET << std::endl;
}

std::string GameUI::getCardText(Card card, bool isDarkStep) {
    Type t = isDarkStep ? card.darkType : card.lightType;
    int v = isDarkStep ? card.darkValue : card.lightValue;

    if (t == Type::NUMBER) return std::to_string(v);

    if (!isDarkStep) {
        if (t == Type::JUMP) return "SALTO";
        if (t == Type::REVERSE) return "REVERSA";
        if (t == Type::DRAW_TO) return "+2";
        if (t == Type::DRAW_FOUR) return "+4";
        if (t == Type::WILD_CARD) return "COLOR";
        if (t == Type::FLIP) return "FLIP";
        if (t == Type::ROULETTE) return "RULETA";
        if (t == Type::SNIPER) return "SNIPER";
    } else {
        if (t == Type::JUMP_ALL) return "S-TODO";
        if (t == Type::REVERSE) return "REVERSA";
        if (t == Type::DRAW_SIX) return "+3";
        if (t == Type::DRAW_UNTIL_COLOR) return "+6 COLOR";
        if (t == Type::WILD_CARD) return "COLOR";
        if (t == Type::FLIP) return "FLIP";
        if (t == Type::ROULETTE) return "RULETA";
        if (t == Type::SNIPER) return "SNIPER";
    }
    return "?";
}

void GameUI::drawCardASCII(Card card, Color activeColor, bool isDarkStep) {
    Color c = isDarkStep ? card.darkColor : card.lightColor;
    if (c == Color::BLACK) c = activeColor; // Si es comodin se muestra el color elegido

    std::string colorCode = getCardColorANSI(c);
    std::string text = getCardText(card, isDarkStep);

    // Nombres cortos de color para la esquina de la carta
    std::string colorName = "WILD";
    if (c == Color::RED) colorName = "ROJO";
    else if (c == Color::BLUE) colorName = "AZUL";
    else if (c == Color::GREEN) colorName = "VERD";
    else if (c == Color::YELLOW) colorName = "AMAR";
    else if (c == Color::PINK) colorName = "ROSA";
    else if (c == Color::TURQUOISE) colorName = "TURQ";
    else if (c == Color::ORANGE) colorName = "NARA";
    else if (c == Color::PURPLE) colorName = "MORA";

    // Arte ASCII de la carta
    std::cout << colorCode;
    std::cout << " .-----------.\n";
    std::cout << " | " << std::left << std::setw(9) << colorName << " |\n";
    std::cout << " |           |\n";
    // Centrar el texto en el medio de la carta
    int padding = (9 - text.length()) / 2;
    std::cout << " |" << std::string(padding, ' ') << text << std::string(9 - text.length() - padding, ' ') << "|\n";
    std::cout << " |           |\n";
    std::cout << " '-----------'\n" << ANSI_RESET;
}

void GameUI::printTable(Card topCard, Color activeColor, bool isDarkStep, Player* currentPlayer, int cardsToDraw, bool allowStacking) {
    clearScreen(); // Limpiamos la pantalla de lo que hizo el jugador anterior

    std::string title = "TURNO DE: " + currentPlayer->getName();
    printHeader(title);

    std::cout << "  MODO: " << (isDarkStep ? ANSI_MAGENTA "LADO OSCURO (FLIP)" : ANSI_YELLOW "LADO NORMAL") << ANSI_RESET << "\n\n";

    std::cout << "  MESA ACTUAL:\n";
    drawCardASCII(topCard, activeColor, isDarkStep);

    if (cardsToDraw > 0) {
        std::cout << ANSI_RED << ANSI_BOLD << "\n  ¡ALERTA! Castigo de +" << cardsToDraw;
        if (allowStacking) std::cout << ". Juega para acumular o '0' para robar.";
        std::cout << ANSI_RESET << "\n";
    }

    std::cout << "\n  TUS CARTAS:\n";
    currentPlayer->getHand().showHand(isDarkStep);
}