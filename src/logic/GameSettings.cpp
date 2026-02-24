#include "../../include/logic/GameSettings.h"
#include "../../include/view/GameUI.h"
#include "../../include/utils/ConsoleColors.h"
#include <iostream>
#include <limits>

GameSettings::GameSettings() {
    // Valores por defecto seguros
    numPlayers = 2;
    useFlip = false;
    allowStacking = false;
    allowChallengePlus4 = false;
    drawUntilPlayable = false;
    manualUnoRule = false;
    allowWinWithWild = false;
}

void GameSettings::promptUser(DCircularList& turnList) {
    GameUI::clearScreen();
    GameUI::printHeader("CONFIGURACION DE LA PARTIDA");

    auto getSafeInt = [](int min, int max) -> int {
        int choice;
        while (!(std::cin >> choice) || choice < min || choice > max) {
            std::cout << ANSI_RED << "Entrada invalida. Ingresa un numero entre "
                      << min << " y " << max << ": " << ANSI_RESET;
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        }
        return choice;
    };

    auto getSafeMinInt = [](int min) -> int {
        int choice;
        while (!(std::cin >> choice) || choice < min) {
            std::cout << ANSI_RED << "Entrada invalida. Ingresa un numero mayor o igual a "
                      << min << ": " << ANSI_RESET;
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        }
        return choice;
    };

    auto getSafeYesNo = []() -> bool {
        char resp;
        while (!(std::cin >> resp) || (resp != 'S' && resp != 's' && resp != 'N' && resp != 'n')) {
            std::cout << ANSI_RED << "Entrada invalida. Ingresa 'S' o 'N': " << ANSI_RESET;
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        }
        return (resp == 'S' || resp == 's');
    };

    // CANTIDAD DE JUGADORES
    std::cout << "Ingrese numero de jugadores (min 2): ";
    numPlayers = getSafeMinInt(2);

    // INSERCION DE NOMBRES
    GameUI::printHeader("REGISTRO DE JUGADORES");
    for (int i = 0; i < numPlayers; i++) {
        std::string name;
        std::cout << "Nombre del Jugador " << i + 1 << ": ";
        std::cin >> name;
        turnList.add(new Player(name));
    }

    // MODO DE JUEGO
    GameUI::printHeader("VARIANTE DE JUEGO");
    std::cout << "[1] UNO Clasico (Normal)\n[2] UNO Flip (Lado Oscuro)\nOpcion: ";
    useFlip = (getSafeInt(1, 2) == 2);

    // REGLAS DE LA CASA
    GameUI::printHeader("REGLAS DE LA CASA");

    std::cout << "¿Permitir acumular castigos (Stacking)? (S/N): ";
    allowStacking = getSafeYesNo();

    std::cout << "¿Activar regla de Reto/Desafio del +4? (S/N): ";
    allowChallengePlus4 = getSafeYesNo();

    std::cout << "Modo de Robo:\n[1] Estandar (Robas 1)\n[2] Infinito (Robas hasta encontrar)\nOpcion: ";
    drawUntilPlayable = (getSafeInt(1, 2) == 2);

    std::cout << "Regla de Grito UNO:\n[1] Manual (Debes escribir 'UNO')\n[2] Automatico\nOpcion: ";
    manualUnoRule = (getSafeInt(1, 2) == 1);

    std::cout << "¿Permitir ganar con carta Negra/Comodin? (S/N): ";
    allowWinWithWild = getSafeYesNo();

    std::cout << "\n" << ANSI_GREEN << "Configuracion completada exitosamente." << ANSI_RESET << std::endl;

    std::cin.clear();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    GameUI::pauseGame("Presione ENTER para barajar y comenzar...");
}