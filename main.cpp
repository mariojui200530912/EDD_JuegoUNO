#include "include/logic/Game.h"
#include "include/view/GameUI.h"

// --- MAIN ---
int main() {
    GameUI::showWelcomeScreen();
    bool playAgain = true;
    while (playAgain)
    {
        Game unoGame;
        // 1. Fase de Configuración (Preguntas al usuario)
        unoGame.configureGame();
        // 2. Fase de Preparación (Creación interna de objetos)
        unoGame.setupDeck();
        // 3. Fase de Juego (Bucle principal)
        unoGame.loop();
        playAgain = GameUI::askToPlayAgain();
    }

    GameUI::clearScreen();
    std::cout << "\n==========================================================\n";
    std::cout << "       ¡Gracias por jugar UNO Flip Edicion C++!           \n";
    std::cout << "==========================================================\n";
    std::cout << " Sistema cerrado correctamente.\n\n";

    return 0;
}
