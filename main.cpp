#include <iostream>
#include <string>
#include "include/entity/Card.h"
#include "include/logic/Game.h"

// --- MAIN ---
int main() {
    Game unoGame;

    // 1. Fase de Configuración (Preguntas al usuario)
    unoGame.configureGame();

    // 2. Fase de Preparación (Creación interna de objetos)
    unoGame.setupDeck();

    // 3. Fase de Juego (Bucle principal)
    unoGame.loop();

    return 0;
}
