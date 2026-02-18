
#ifndef JUEGOUNO_CONSOLECOLORS_H
#define JUEGOUNO_CONSOLECOLORS_H
#include <string>
#include "../entity/Card.h" // Necesario para el enum Color

// Definiciones de Colores ANSI
#define ANSI_RESET   "\033[0m"
#define ANSI_RED     "\033[31m"
#define ANSI_YELLOW  "\033[33m"
#define ANSI_BLUE    "\033[34m"
#define ANSI_GREEN   "\033[32m"
#define ANSI_MAGENTA "\033[35m"
#define ANSI_CYAN    "\033[36m"
#define ANSI_WHITE   "\033[37m"
#define ANSI_BOLD    "\033[1m"
#define ANSI_CYAN    "\033[36m"

// Función auxiliar compartida (inline para evitar errores de duplicación)
inline std::string getCardColorANSI(Color c) {
    switch (c) {
        // Lado Claro
    case Color::RED:    return ANSI_RED;
    case Color::YELLOW: return ANSI_YELLOW;
    case Color::BLUE:   return ANSI_BLUE;
    case Color::GREEN:  return ANSI_GREEN;
    case Color::BLACK:  return ANSI_WHITE;

        // Lado Oscuro
    case Color::PINK:      return ANSI_MAGENTA;
    case Color::TURQUOISE: return ANSI_CYAN;
    case Color::ORANGE:    return ANSI_YELLOW;
    case Color::PURPLE:    return ANSI_BLUE;

    default: return ANSI_RESET;
    }
}
#endif //JUEGOUNO_CONSOLECOLORS_H