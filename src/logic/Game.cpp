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
    bool isWildLight = (!isDarkStep && Rules::isWildCard(card, false));
    bool isWildDark = (isDarkStep && Rules::isWildCard(card, true));

    if (isWildLight || isWildDark) {
        activeColor = GameUI::askForColor(isDarkStep);
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
            case Type::ROULETTE: executeRoulette(false); break;
            case Type::SNIPER:
            executeSniper();
            break;
            default: break;
        }
    } else {
        // LADO OSCURO -----------
        switch (card.darkType) {
            case Type::DRAW_TO: cardsToDraw += 3; break;
            case Type::DRAW_SIX: cardsToDraw += 6; break;
            case Type::REVERSE:
                clockwise = !clockwise;
                if (numPlayers == 2) skipTurn = true;
                break;
            case Type::JUMP_ALL: turnList.passTurn(!clockwise); break;
            case Type::FLIP: performFlip(); break;
            case Type::ROULETTE: executeRoulette(true); break;
            case Type::SNIPER: executeSniper(); break;
            case Type::DRAW_UNTIL_COLOR: executeDrawUntilColor(skipTurn); break;
            default: break;
        }
    }
}


void Game::renderUI(Player* player) {
    GameUI::printTable(discardPile.peek(), activeColor, isDarkStep, player, cardsToDraw, allowStacking, mainDeck.getSize());
}

void Game::handlePlayerTurn(Player* player, bool& skipTurn) {
    if (cardsToDraw > 0 && !allowStacking) {
        std::cout << ANSI_RED << "\n[!] CASTIGO OBLIGATORIO: Regla de acumular desactivada.\n" << ANSI_RESET;
        std::cout << ANSI_RED << "\n[!] CASTIGO APLICADO: " << player->getName()
              << " recibe " << cardsToDraw << " cartas.\n" << ANSI_RESET;
        drawCards(player, cardsToDraw);
        cardsToDraw = 0;
        return;
    }

    bool turnEnded = false;
    bool hasDrawn = false;

    while (!turnEnded) {
        Card topCard = discardPile.peek(); // Refrescamos la carta de la mesa

        int choice;
        std::cout << "\n>> Opcion (1-" << player->getHand().getSize() << " o 0 para robar): ";
        while (!(std::cin >> choice)) { std::cin.clear(); std::cin.ignore(1000, '\n'); }
        // ROBAR
        if (choice == 0) {
            // Si hay un castigo pendiente, roba y pierde el turno de inmediato
            if (cardsToDraw > 0) {
                std::cout << ANSI_RED << "\n[!] Aceptas el castigo. Robando " << cardsToDraw << " cartas...\n" << ANSI_RESET;

                drawCards(player, cardsToDraw);
                cardsToDraw = 0;

                turnEnded = true;
            }
            // Si es robo voluntario
            else {
                if (hasDrawn) {
                    // Si ya había robado en este turno...
                    if (drawUntilPlayable) {
                        std::cout << ANSI_RED << "¡Ya robaste y tienes una carta jugable! Elige su numero para jugarla." << ANSI_RESET << std::endl;
                    } else {
                        // En modo normal, presionar 0 por segunda vez significa "Pasar Turno"
                        std::cout << ANSI_YELLOW << ">> Pasas el turno sin jugar <<" << ANSI_RESET << std::endl;
                        turnEnded = true;
                    }
                } else {
                    if (drawUntilPlayable) {
                        infiniteDraw(player);
                    } else {
                        drawCards(player, 1);
                        std::cout << ANSI_CYAN << "Has robado una carta.\n" << ANSI_RESET;
                    }
                    hasDrawn = true;
                    std::cout << ANSI_CYAN << "\n--- MANO ACTUALIZADA ---" << ANSI_RESET << std::endl;
                    player->getHand().showHand(isDarkStep);

                    if (drawUntilPlayable) {
                        std::cout << ANSI_GREEN << "¡Juega la carta que encontraste!" << ANSI_RESET << std::endl;
                    } else {
                        std::cout << ANSI_YELLOW << "Si la carta te sirve, juegala. Si no, presiona '0' de nuevo para pasar turno." << ANSI_RESET << std::endl;
                    }
                }
            }
        }
        // JUGAR UNA CARTA
        else if (choice >= 1 && choice <= player->getHand().getSize()) {
            Card selected = player->getHand().getByIndex(choice);

            // VALIDACIÓN DEL MOVIMIENTO
            bool isValid = false;
            if (cardsToDraw > 0) {
                isValid = Rules::canStack(selected, topCard, isDarkStep); // Lógica delegada al motor
                if (!isValid) std::cout << ANSI_RED << "¡Debes jugar una carta de suma o robar!" << ANSI_RESET << std::endl;
            } else {
                isValid = Rules::validateMove(selected, topCard, isDarkStep, activeColor);
            }

            if (isValid) {
                if (!allowWinWithWild && player->getHand().getSize() == 1 && Rules::isWildCard(selected, isDarkStep)) {
                    std::cout << ANSI_RED << "¡REGLA VIOLADA! No puedes ganar con carta Negra.\nRobas 1 de castigo.\n" << ANSI_RESET;
                    drawCards(player, 1);
                    turnEnded = true;
                    continue;
                }

                // CAPTURAR VARIABLES PARA EL RETO +4 ANTES DE JUGARLA
                Color previousColor = isDarkStep ? ((topCard.darkColor == Color::BLACK) ? activeColor : topCard.darkColor)
                                                 : ((topCard.lightColor == Color::BLACK) ? activeColor : topCard.lightColor);
                int previousValue = isDarkStep ? topCard.darkValue : topCard.lightValue;

                // JUGAR LA CARTA
                Card playedCard = player->playCard(choice);
                discardPile.push(playedCard);

                // Actualizar Color Activo si la carta tiene color propio
                if (isDarkStep) activeColor = (playedCard.darkColor != Color::BLACK) ? playedCard.darkColor : activeColor;
                else activeColor = (playedCard.lightColor != Color::BLACK) ? playedCard.lightColor : activeColor;

                // LÓGICA DEL RETO DEL +4
                handlePlus4Challenge(player, playedCard, previousColor, previousValue);

                // APLICAR EFECTOS Y TERMINAR TURNO
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
            if (!GameUI::askForUno()) {
                std::cout << "¡Lento! +2 cartas.\n";
                drawCards(player, 2);
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

void Game::drawCards(Player* player, int amount) {
    for (int i = 0; i < amount; i++) {
        if (mainDeck.isEmpty()) GeneratorDeck::refillMainDeck(mainDeck, discardPile, maxDeckSize);
        if (!mainDeck.isEmpty()) player->addCard(mainDeck.pop());
    }
    // Ordenamos la mano una sola vez al terminar de robar
    player->getHand().insertionSort(isDarkStep);
}

void Game::infiniteDraw(Player* player)
{
    std::cout << ANSI_YELLOW << "Robando hasta encontrar carta jugable...\n" << ANSI_RESET;
    while (true) {
        if (mainDeck.isEmpty()) GeneratorDeck::refillMainDeck(mainDeck, discardPile, maxDeckSize);
        if (mainDeck.isEmpty()) break; // Seguro contra bucles infinitos si el mazo se rompe

        Card drawn = mainDeck.pop();
        player->addCard(drawn);

        if (Rules::validateMove(drawn, discardPile.peek(), isDarkStep, activeColor)) {
            std::cout << ANSI_GREEN << "¡Carta jugable encontrada!\n" << ANSI_RESET;
            break;
        }
    }
    player->getHand().insertionSort(isDarkStep);
}

void Game::handlePlus4Challenge(Player* player, Card& playedCard, Color previousColor, int previousValue) {
    if (!allowChallengePlus4 || isDarkStep || playedCard.lightType != Type::DRAW_FOUR) {
        return;
    }

    turnList.passTurn(clockwise);
    Player* victim = turnList.getCurrentPlayer();
    turnList.passTurn(!clockwise);

    if (GameUI::askToChallenge(victim->getName())) {
        bool challengeSuccess = Rules::checkBluff(victim, player, previousColor, previousValue, isDarkStep);

        if (challengeSuccess) {
            drawCards(player, 4); // jugador se lleva las 4 cartas
            playedCard.lightType = Type::WILD_CARD;
        } else {
            cardsToDraw += 2; // victima se equivoco se lleva 6 cartas
        }
    }
}

void Game::executeRoulette(bool isDark) {
    int castigo = (std::rand() % 6) + 1;
    cardsToDraw += castigo;

    if (!isDark) {
        std::cout << ANSI_BOLD << ANSI_RED << "\n¡RULETA! El siguiente roba " << castigo << " cartas.\n" << ANSI_RESET;
    } else {
        std::cout << ANSI_BOLD << ANSI_RED << "\n¡RULETA OSCURA! Castigo de " << castigo << " cartas.\n" << ANSI_RESET;
    }
}

void Game::executeDrawUntilColor(bool& skipTurn) {
    std::cout << ANSI_RED << ANSI_BOLD << "\n¡MALDICION MULTICOLOR! El siguiente roba hasta encontrar el color.\n" << ANSI_RESET;

    turnList.passTurn(clockwise);
    Player* victim = turnList.getCurrentPlayer();
    std::cout << "Victima: " << victim->getName() << "...\n";

    int count = 0;
    while (true) {
        if (mainDeck.isEmpty()) GeneratorDeck::refillMainDeck(mainDeck, discardPile, maxDeckSize);
        if (mainDeck.isEmpty()) break;

        Card drawn = mainDeck.pop();
        victim->addCard(drawn);
        count++;

        if (drawn.darkColor == activeColor) {
            std::cout << ANSI_GREEN << "¡Color encontrado tras robar " << count << " cartas!\n" << ANSI_RESET;
            break;
        }
    }

    victim->getHand().insertionSort(isDarkStep);

    // Regresar el turno al jugador actual y saltar a la víctima
    turnList.passTurn(!clockwise);
    skipTurn = true;
}

void Game::executeSniper() {
    if (numPlayers > 2) {
        std::cout << ANSI_BOLD << ANSI_GREEN << "\n-- FRANCOTIRADOR ACTIVADO --\n" << ANSI_RESET;
        for (int i = 1; i < numPlayers; i++) {
            turnList.passTurn(clockwise);
            std::cout << "[" << i << "] " << turnList.getCurrentPlayer()->getName() << "\n";
        }
        for (int i = 1; i < numPlayers; i++) turnList.passTurn(!clockwise);

        int t; std::cout << "Elige victima: "; std::cin >> t;

        for (int i = 0; i < t; i++) turnList.passTurn(clockwise);
        Player* v = turnList.getCurrentPlayer();

        std::cout << "¡PUM! " << v->getName() << " recibe 3 cartas.\n";
        drawCards(v, 3); // ¡Usamos nuestra nueva función refactorizada!

        for (int i = 0; i < t; i++) turnList.passTurn(!clockwise);
    } else {
        std::cout << "¡PUM! Tu oponente recibe 3 cartas.\n";
        cardsToDraw += 3;
    }
}