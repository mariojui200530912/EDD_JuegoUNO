#include "../../include/logic/Game.h"
#include "../../include/utils/ConsoleColors.h"
#include "../../include/logic/GameSettings.h"
#include <iostream>
#include <limits>

Game::Game() {
    isDarkStep = false;
    clockwise = true;
    gameFinished = false;
    cardsToDraw = 0;
    activeColor = Color::BLACK;
}

void Game::configureGame() {
    GameSettings settings;
    settings.promptUser(turnList);

    this->numPlayers = settings.numPlayers;
    this->useFlip = settings.useFlip;
    this->allowStacking = settings.allowStacking;
    this->allowChallengePlus4 = settings.allowChallengePlus4;
    this->drawUntilPlayable = settings.drawUntilPlayable;
    this->manualUnoRule = settings.manualUnoRule;
    this->allowWinWithWild = settings.allowWinWithWild;
}

// Metodo para configurar deck
void Game::setupDeck() {
    std::cout << "Generando mazo..." << std::endl;
    // Pasamos la configuración 'useFlip' al generador
    generator.initializeDeck(mainDeck, numPlayers, useFlip);
    maxDeckSize = mainDeck.getSize();
    std::cout << "[INFO] Mazo inicializado con " << maxDeckSize << " cartas." << std::endl;

    for (int i = 0; i < 7 * numPlayers; i++) {
        Player* p = turnList.getCurrentPlayer();
        p->addCard(mainDeck.pop());
        p->getHand().insertionSort(isDarkStep);
        turnList.passTurn(true);
    }

    // Poner primera carta en mesa
    Card first = mainDeck.pop();
    Stack tempStack;
    while (first.lightColor == Color::BLACK) {
        std::cout << "[INFO] La primera carta es Negra. Buscando otra..." << std::endl;
        tempStack.push(first);     // La apartamos temporalmente
        first = mainDeck.pop();    // Sacamos la siguiente del mazo
    }
    while (!tempStack.isEmpty()) {
        mainDeck.push(tempStack.pop());
    }
    discardPile.push(first);
    activeColor = first.lightColor;
}

void Game::loop() {
    bool skipTurn = false;

    while (!gameFinished) {
        Player* currentPlayer = turnList.getCurrentPlayer();

        // Verificar castigos (+2, +4, etc.)
        if (handlePendingPenalties(currentPlayer)) {
            continue; // Si recibió castigo y no pudo acumular, pierde el turno
        }

        // Mostrar la mesa y la mano
        renderUI(currentPlayer);

        // Dejar que el jugador elija y juegue
        handlePlayerTurn(currentPlayer, skipTurn);

        // Verificar SI GRITO UNO O GANO
        if (!gameFinished) { // Evita revisar UNO si ya ganó en este turno
            checkUnoAndWin(currentPlayer);
        }

        // Pasar al siguiente jugador
        if (!gameFinished) {
            advanceTurn(skipTurn);
        }
    }
}

void Game::applyCardEffect(Card card, bool& skipTurn) {
    bool isWildLight = (!isDarkStep && (card.lightType == Type::WILD_CARD || card.lightType == Type::DRAW_FOUR || card.lightType == Type::ROULETTE || card.lightType == Type::SNIPER));
    bool isWildDark = (isDarkStep && (card.darkType == Type::WILD_CARD || card.darkType == Type::DRAW_UNTIL_COLOR || card.darkType == Type::ROULETTE || card.darkType == Type::SNIPER));

    if (isWildLight || isWildDark) {
        int colorOpt;
        std::cout << "\n" << ANSI_BOLD << ">> SELECCIONA EL SIGUIENTE COLOR <<" << ANSI_RESET << std::endl;
        if (!isDarkStep) std::cout << "[0:Rojo, 1:Verde, 2:Azul, 3:Amarillo]: ";
        else std::cout << "[4:Rosa, 5:Turquesa, 6:Naranja, 7:Morado]: ";

        while(!(std::cin >> colorOpt)) { std::cin.clear(); std::cin.ignore(1000,'\n'); }
        activeColor = static_cast<Color>(colorOpt);
    }

    if (!isDarkStep) {
        // LADO CLARO-----------
        switch (card.lightType) {
            case Type::DRAW_TO: cardsToDraw += (useFlip ? 1 : 2); break;
            case Type::DRAW_FOUR: cardsToDraw += (useFlip ? 2 : 4); break;
            case Type::REVERSE:
                clockwise = !clockwise;
                if (numPlayers == 2) skipTurn = true;
                break;
            case Type::JUMP: skipTurn = true; break;
            case Type::FLIP: performFlip(); break;
            case Type::ROULETTE:
                cardsToDraw += ((std::rand() % 6) + 1);
                std::cout << ANSI_BOLD << ANSI_RED << "\n¡RULETA! El siguiente roba al azar.\n" << ANSI_RESET;
                break;
            case Type::SNIPER:
                if (numPlayers > 2) {
                    std::cout << ANSI_BOLD << ANSI_GREEN << "\n--🎯 FRANCOTIRADOR 🎯--\n" << ANSI_RESET;
                    Player* a = turnList.getCurrentPlayer();
                    for (int i = 1; i < numPlayers; i++) {
                        turnList.passTurn(clockwise);
                        std::cout << "[" << i << "] " << turnList.getCurrentPlayer()->getName() << "\n";
                    }
                    for (int i = 1; i < numPlayers; i++) turnList.passTurn(!clockwise);
                    int t; std::cout << "Elige victima: "; std::cin >> t;
                    for (int i = 0; i < t; i++) turnList.passTurn(clockwise);
                    Player* v = turnList.getCurrentPlayer();

                    for(int k=0; k<3; k++) {
                        if(mainDeck.isEmpty()) GeneratorDeck::refillMainDeck(mainDeck, discardPile, maxDeckSize);
                        if(!mainDeck.isEmpty()) v->addCard(mainDeck.pop());
                    }
                    v->getHand().insertionSort(isDarkStep);
                    for (int i = 0; i < t; i++) turnList.passTurn(!clockwise);
                } else cardsToDraw += 3;
                break;
            default: break;
        }
    } else {
        // LADO OSCURO -----------
        switch (card.darkType) {
            case Type::DRAW_SIX: cardsToDraw += 6; break;
            case Type::REVERSE:
                clockwise = !clockwise;
                if (numPlayers == 2) skipTurn = true;
                break;
            case Type::JUMP_ALL: turnList.passTurn(!clockwise); break;
            case Type::FLIP: performFlip(); break;
            case Type::ROULETTE:
                cardsToDraw += ((std::rand() % 6) + 1);
                std::cout << ANSI_BOLD << ANSI_RED << "\n¡RULETA OSCURA! Castigo al azar.\n" << ANSI_RESET;
                break;
            case Type::SNIPER:
                if (numPlayers > 2) {
                    std::cout << ANSI_BOLD << ANSI_GREEN << "\n--🎯 FRANCOTIRADOR OSCURO 🎯--\n" << ANSI_RESET;
                    for (int i = 1; i < numPlayers; i++) {
                        turnList.passTurn(clockwise);
                        std::cout << "[" << i << "] " << turnList.getCurrentPlayer()->getName() << "\n";
                    }
                    for (int i = 1; i < numPlayers; i++) turnList.passTurn(!clockwise);
                    int t; std::cout << "Elige victima: "; std::cin >> t;
                    for (int i = 0; i < t; i++) turnList.passTurn(clockwise);
                    Player* v = turnList.getCurrentPlayer();

                    for(int k=0; k<3; k++) {
                        if(mainDeck.isEmpty()) GeneratorDeck::refillMainDeck(mainDeck, discardPile, maxDeckSize);
                        if(!mainDeck.isEmpty()) v->addCard(mainDeck.pop());
                    }
                    v->getHand().insertionSort(isDarkStep);
                    for (int i = 0; i < t; i++) turnList.passTurn(!clockwise);
                } else cardsToDraw += 3;
                break;
            case Type::DRAW_UNTIL_COLOR: {
                std::cout << ANSI_RED << ANSI_BOLD << "\n¡MALDICION MULTICOLOR! El siguiente roba hasta encontrar el color.\n" << ANSI_RESET;

                turnList.passTurn(clockwise);
                Player* victim = turnList.getCurrentPlayer();
                std::cout << "Victima: " << victim->getName() << "...\n";

                bool found = false;
                int count = 0;

                // Robando del maso
                while (!found) {
                    if (mainDeck.isEmpty()) GeneratorDeck::refillMainDeck(mainDeck, discardPile, maxDeckSize);
                    if (mainDeck.isEmpty()) break;

                    Card drawn = mainDeck.pop();
                    victim->addCard(drawn);
                    count++;

                    if (drawn.darkColor == activeColor) {
                        found = true;
                        std::cout << ANSI_GREEN << "¡Color encontrado tras robar " << count << " cartas!\n" << ANSI_RESET;
                    }
                }

                victim->getHand().insertionSort(isDarkStep);

                // Regresar el turno al jugador actual y saltar a la víctima
                turnList.passTurn(!clockwise);
                skipTurn = true;
                break;
            }
            default: break;
        }
    }
}

bool Game::handlePendingPenalties(Player* player) {
    bool mustStackOrDraw = (cardsToDraw > 0);

    // Si hay castigo y NO se permite acumular, robar inmediatamente y perder turno
    if (mustStackOrDraw && !allowStacking) {
        std::cout << ANSI_RED << "\n[!] CASTIGO: " << player->getName()
                  << " roba " << cardsToDraw << " cartas." << ANSI_RESET << std::endl;
        for (int i = 0; i < cardsToDraw; i++) {
            if (mainDeck.isEmpty()) GeneratorDeck::refillMainDeck(mainDeck, discardPile, maxDeckSize);
            if (!mainDeck.isEmpty()) player->addCard(mainDeck.pop());
        }
        player->getHand().insertionSort(isDarkStep);
        cardsToDraw = 0;

        turnList.passTurn(clockwise);
        return true;
    }
    return false;
}

void Game::renderUI(Player* player) {
    GameUI::printTable(discardPile.peek(), activeColor, isDarkStep, player, cardsToDraw, allowStacking);
}

void Game::executeDraw(Player* player) {
    if (cardsToDraw > 0) { // Robar por castigo acumulado
        std::cout << "Aceptas el castigo. Robando " << cardsToDraw << " cartas.\n";
        for (int i = 0; i < cardsToDraw; i++) {
            if (mainDeck.isEmpty()) GeneratorDeck::refillMainDeck(mainDeck, discardPile, maxDeckSize);
            if (!mainDeck.isEmpty()) player->addCard(mainDeck.pop());
        }
        cardsToDraw = 0;
    }
    else { // Robo voluntario
        if (drawUntilPlayable) { // Modo infinito
            std::cout << "Robando hasta encontrar carta jugable...\n";
            while (true) {
                if (mainDeck.isEmpty()) GeneratorDeck::refillMainDeck(mainDeck, discardPile, maxDeckSize);
                if (mainDeck.isEmpty()) break;

                Card drawn = mainDeck.pop();
                player->addCard(drawn);
                if (Rules::validateMove(drawn, discardPile.peek(), isDarkStep, activeColor)) {
                    std::cout << "¡Carta jugable encontrada!\n";
                    break;
                }
            }
        } else { // Modo 1 carta
            if (mainDeck.isEmpty()) GeneratorDeck::refillMainDeck(mainDeck, discardPile, maxDeckSize);
            if (!mainDeck.isEmpty()) {
                player->addCard(mainDeck.pop());
                std::cout << "Has robado una carta.\n";
            }
        }
    }
    player->getHand().insertionSort(isDarkStep);
}

void Game::handlePlayerTurn(Player* player, bool& skipTurn) {
    bool turnEnded = false;

    while (!turnEnded) {
        Card topCard = discardPile.peek(); // Refrescamos la carta de la mesa

        int choice;
        std::cout << "\n>> Opcion (1-" << player->getHand().getSize() << " o 0 para robar): ";
        while (!(std::cin >> choice)) { std::cin.clear(); std::cin.ignore(1000, '\n'); }

        if (choice == 0) {
            executeDraw(player);
            // Si no estamos en modo robar hasta jugar, o si aceptó castigo, pierde turno
            if (!drawUntilPlayable || cardsToDraw == 0) turnEnded = true;
        }
        else if (choice >= 1 && choice <= player->getHand().getSize()) {
            Card selected = player->getHand().getByIndex(choice);

            // 1. VALIDACIÓN DEL MOVIMIENTO
            bool isValid = false;
            if (cardsToDraw > 0) {
                if (!isDarkStep && selected.lightType == Type::DRAW_TO && topCard.lightType == Type::DRAW_TO) isValid = true;
                else if (isDarkStep && selected.darkType == Type::DRAW_SIX && topCard.darkType == Type::DRAW_SIX) isValid = true;
                else std::cout << ANSI_RED << "¡Debes jugar una carta de suma o robar!" << ANSI_RESET << std::endl;
            } else {
                isValid = Rules::validateMove(selected, topCard, isDarkStep, activeColor);
            }

            if (isValid) {
                // 2. REGLA: GANAR CON CARTA NEGRA
                bool isWild = (!isDarkStep && (selected.lightType == Type::WILD_CARD || selected.lightType == Type::DRAW_FOUR || selected.lightType == Type::ROULETTE || selected.lightType == Type::SNIPER)) ||
                              (isDarkStep && (selected.darkType == Type::WILD_CARD || selected.darkType == Type::DRAW_UNTIL_COLOR));

                if (!allowWinWithWild && player->getHand().getSize() == 1 && isWild) {
                    std::cout << ANSI_RED << "¡REGLA VIOLADA! No puedes ganar con carta Negra.\nRobas 1 de castigo.\n" << ANSI_RESET;
                    if(mainDeck.isEmpty()) GeneratorDeck::refillMainDeck(mainDeck, discardPile, maxDeckSize);
                    if(!mainDeck.isEmpty()) player->addCard(mainDeck.pop());
                    player->getHand().insertionSort(isDarkStep);
                    turnEnded = true;
                    continue; // Salta al siguiente turno
                }

                // 3. CAPTURAR VARIABLES PARA EL RETO +4 ANTES DE JUGARLA
                Color previousColor = isDarkStep ? ((topCard.darkColor == Color::BLACK) ? activeColor : topCard.darkColor)
                                                 : ((topCard.lightColor == Color::BLACK) ? activeColor : topCard.lightColor);
                int previousValue = isDarkStep ? topCard.darkValue : topCard.lightValue;

                // 4. JUGAR LA CARTA
                Card playedCard = player->playCard(choice);
                discardPile.push(playedCard);

                // Actualizar Color Activo si la carta tiene color propio
                if (isDarkStep) activeColor = (playedCard.darkColor != Color::BLACK) ? playedCard.darkColor : activeColor;
                else activeColor = (playedCard.lightColor != Color::BLACK) ? playedCard.lightColor : activeColor;

                // 5. LÓGICA DEL RETO DEL +4
                bool challengeSuccess = false;

                if (allowChallengePlus4 && !isDarkStep && playedCard.lightType == Type::DRAW_FOUR) {

                    // Identificamos a la víctima
                    turnList.passTurn(clockwise);
                    Player* victim = turnList.getCurrentPlayer();
                    turnList.passTurn(!clockwise); // Regresamos al jugador actual

                    char response;
                    std::cout << ANSI_YELLOW << victim->getName() << ", ¿desafiar este +4? (S/N): " << ANSI_RESET;
                    std::cin >> response;

                    if (response == 'S' || response == 's') {
                        // Llamamos a checkBluff con los valores capturados en el paso 3
                        challengeSuccess = Rules::checkBluff(victim, player, previousColor, previousValue, isDarkStep);

                        if (challengeSuccess) {
                            // Castigo para el que mintió
                            for(int k=0; k<4; k++) {
                                if(mainDeck.isEmpty()) GeneratorDeck::refillMainDeck(mainDeck, discardPile, maxDeckSize);
                                if(!mainDeck.isEmpty()) player->addCard(mainDeck.pop());
                            }
                            player->getHand().insertionSort(isDarkStep);

                            // Anulamos el efecto del +4 para la víctima convirtiéndolo en un Wild normal en memoria
                            playedCard.lightType = Type::WILD_CARD;
                        } else {
                            // Castigo extra para la víctima si se equivocó
                            cardsToDraw += 2;
                        }
                    }
                }

                // 6. APLICAR EFECTOS Y TERMINAR TURNO
                applyCardEffect(playedCard, skipTurn);
                turnEnded = true;

            } else if (cardsToDraw == 0) {
                std::cout << ANSI_YELLOW << "Movimiento no valido." << ANSI_RESET << std::endl;
            }
        } else {
            std::cout << "Numero fuera de rango." << std::endl;
        }
    }
}

void Game::checkUnoAndWin(Player* player) {
    int handSize = player->getHand().getSize();

    if (handSize == 0) {
        std::cout << ANSI_GREEN << "\n¡¡¡ " << player->getName() << " GANA !!!" << ANSI_RESET << std::endl;
        gameFinished = true;
    }
    else if (handSize == 1) {
        if (manualUnoRule) {
            std::string grito;
            std::cout << ANSI_RED << "¡Te queda 1! Escribe 'UNO': " << ANSI_RESET;
            std::cin >> grito;
            if (grito != "UNO" && grito != "uno" && grito != "Uno") {
                std::cout << "¡Lento! +2 cartas.\n";
                for(int i=0; i<2; i++) {
                    if (mainDeck.isEmpty()) GeneratorDeck::refillMainDeck(mainDeck, discardPile, maxDeckSize);
                    if (!mainDeck.isEmpty()) player->addCard(mainDeck.pop());
                }
                player->getHand().insertionSort(isDarkStep);
            }
        } else {
            std::cout << ANSI_CYAN << ANSI_BOLD << "¡¡¡ UNO AUTOMATICO por " << player->getName() << " !!!" << ANSI_RESET << std::endl;
        }
    }
}

void Game::advanceTurn(bool& skipTurn) {
    GameUI::pauseGame("Presiona ENTER para terminar tu turno y pasar la pantalla...");
    turnList.passTurn(clockwise);
    if (skipTurn) {
        GameUI::clearScreen();
        std::cout << ANSI_MAGENTA << "\n\n>> ¡" << turnList.getCurrentPlayer()->getName()
                  << " HA SIDO SALTADO! <<\n" << ANSI_RESET << std::endl;
        GameUI::pauseGame("Presiona ENTER para continuar...");

        turnList.passTurn(clockwise);
        skipTurn = false;
    }
}

void Game::performFlip() {
    isDarkStep = !isDarkStep;
    std::cout << ANSI_BOLD << ANSI_MAGENTA << "\n¡¡¡ FLIP !!! EL MUNDO SE INVIERTE..." << ANSI_RESET << std::endl;

    for (int i = 0; i < numPlayers; i++)
    {
        Player* p = turnList.getCurrentPlayer();
        p->getHand().mergeSort(isDarkStep);
        turnList.passTurn(true);
    }
}