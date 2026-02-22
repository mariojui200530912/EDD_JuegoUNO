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

    // CANTIDAD DE JUGADORES
    do {
        std::cout << "Ingrese numero de jugadores (Minimo 2): ";
        std::cin >> numPlayers;

        if (std::cin.fail() || numPlayers < 2) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout << ANSI_YELLOW << "Error: Debe ser un numero mayor o igual a 2." << ANSI_RESET << std::endl;
        } else break;
    } while (true);

    // INSERCION DE NOMBRES ---
    GameUI::printHeader("REGISTRO DE JUGADORES");
    for (int i = 0; i < numPlayers; i++) {
        std::string name;
        std::cout << "Nombre del Jugador " << i + 1 << ": ";
        std::cin >> name;
        // Creamos al jugador y lo agregamos a TU lista circular de una vez
        turnList.add(new Player(name));
    }

    // MODO DE JUEGO
    GameUI::printHeader("VARIANTE DE JUEGO");
    std::cout << "[1] UNO Clasico (Normal)\n[2] UNO Flip (Lado Oscuro)\nOpcion: ";
    int modeChoice; std::cin >> modeChoice;
    useFlip = (modeChoice == 2);

    GameUI::printHeader("REGLAS DE LA CASA");
    char resp;

    std::cout << "¿Permitir acumular castigos (Stacking)? (S/N): "; std::cin >> resp;
    allowStacking = (resp == 'S' || resp == 's');

    std::cout << "¿Activar regla de Reto/Desafio del +4? (S/N): "; std::cin >> resp;
    allowChallengePlus4 = (resp == 'S' || resp == 's');

    std::cout << "Modo de Robo:\n[1] Estandar (Robas 1)\n[2] Infinito (Robas hasta encontrar)\nOpcion: ";
    int drawOpt; std::cin >> drawOpt;
    drawUntilPlayable = (drawOpt == 2);

    std::cout << "Regla de Grito UNO:\n[1] Manual (Debes escribir 'UNO')\n[2] Automatico\nOpcion: ";
    int unoOpt; std::cin >> unoOpt;
    manualUnoRule = (unoOpt == 1);

    std::cout << "¿Permitir ganar con carta Negra/Comodin? (S/N): "; std::cin >> resp;
    allowWinWithWild = (resp == 'S' || resp == 's');

    std::cout << "\n" << ANSI_GREEN << "Configuracion completada exitosamente." << ANSI_RESET << std::endl;
    std::cin.ignore(1000, '\n');
    GameUI::pauseGame("Presione ENTER para barajar y comenzar...");
}