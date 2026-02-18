#include "../../include/logic/Game.h"
#include "../../include/utils/ConsoleColors.h"
#include <iostream>
#include <limits> // Para limpiar el buffer de entrada

void Game::printHeader(std::string title) {
    std::cout << "\n" << ANSI_CYAN << "========================================" << ANSI_RESET << std::endl;
    std::cout << "      " << ANSI_BOLD << title << ANSI_RESET << std::endl;
    std::cout << ANSI_CYAN << "========================================" << ANSI_RESET << std::endl;
}

Game::Game() {
    isDarkStep = false;
    clockwise = true;
    gameFinished = false;
    cardsToDraw = 0;
    activeColor = Color::BLACK;
}

void Game::configureGame() {
    printHeader("CONFIGURACION DE LA PARTIDA");

    // 1. CANTIDAD DE JUGADORES
    do {
        std::cout << "Ingrese numero de jugadores (2-10): ";
        std::cin >> numPlayers;
        if (std::cin.fail() || numPlayers < 2 || numPlayers > 10) {
            std::cin.clear(); // Limpiar estado de error
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Limpiar buffer
            std::cout << ANSI_YELLOW << "Error: Debe ser un numero entre 2 y 10." << ANSI_RESET << std::endl;
        } else {
            break;
        }
    } while (true);

    // 2. REGISTRO DE NOMBRES
    printHeader("REGISTRO DE JUGADORES");
    for (int i = 0; i < numPlayers; i++) {
        std::string name;
        std::cout << "Nombre del Jugador " << i + 1 << ": ";
        std::cin >> name;
        turnList.add(new Player(name));
    }

    // 3. MODO DE JUEGO
    printHeader("VARIANTE DE JUEGO");
    std::cout << "Seleccione el tipo de mazo y reglas:\n";
    std::cout << "[1] UNO Clasico (Normal)\n";
    std::cout << "[2] UNO Flip (Lado Oscuro)\n";
    std::cout << "Opcion: ";

    int modeChoice;
    std::cin >> modeChoice;
    useFlip = (modeChoice == 2);

    if (useFlip) {
        std::cout << "-> Modo seleccionado: " << ANSI_BOLD << "UNO FLIP" << ANSI_RESET << std::endl;
    } else {
        std::cout << "-> Modo seleccionado: " << ANSI_BOLD << "UNO CLASICO" << ANSI_RESET << std::endl;
    }

    // 4. REGLAS DE LA CASA (Configuraciones Avanzadas)
    printHeader("REGLAS DE LA CASA");

    char resp;
    std::cout << "¿Permitir acumular castigos (Stacking)?\n";
    std::cout << "(Ejemplo: Responder a un +2 con otro +2 para que el siguiente robe 4)\n";
    std::cout << "S/N: ";
    std::cin >> resp;
    allowStacking = (resp == 'S' || resp == 's');

    std::cout << "¿Activar colores ANSI en consola? (S/N): ";
    std::cin >> resp;
    showAnsiColors = (resp == 'S' || resp == 's');

    std::cout << "\n" << ANSI_GREEN << "Configuracion completada exitosamente." << ANSI_RESET << std::endl;
    std::cout << "Presione ENTER para barajar y comenzar...";
    std::cin.ignore();
    std::cin.get();
}

// Metodo para configurar deck
void Game::setupDeck() {
    std::cout << "Generando mazo..." << std::endl;

    // Pasamos la configuración 'useFlip' al generador
    generator.initializeDeck(mainDeck, numPlayers, useFlip);

    // Resto del reparto...
    for (int i = 0; i < 7 * numPlayers; i++) {
        Player* p = turnList.getCurrentPlayer();
        p->addCard(mainDeck.pop());
        p->getHand().insertionSort(isDarkStep);
        turnList.passTurn(true);
    }

    // Poner primera carta en mesa
    Card first = mainDeck.pop();
    while(first.lightType == Type::WILD_CARD || first.lightType == Type::CUSTOM) {
        mainDeck.push(first);
        first = mainDeck.pop();
    }
    discardPile.push(first);
    activeColor = first.lightColor;
}

void Game::loop() {
    bool skipTurn = false; // Bandera para controlar saltos de turno (Skip)

    while (!gameFinished) {
        Player* currentPlayer = turnList.getCurrentPlayer();

        // Determinar carta visual y color en mesa
        Card topCard = discardPile.peek();
        Color visualColor;
        int visualValue;
        if (isDarkStep) {
            visualColor = (topCard.darkColor == Color::BLACK) ? activeColor : topCard.darkColor;
            visualValue = topCard.darkValue;
        } else {
            visualColor = (topCard.lightColor == Color::BLACK) ? activeColor : topCard.lightColor;
            visualValue = topCard.lightValue;
        }

        // ---------------------------------------------------------
        // 1. GESTIÓN DE CASTIGOS (Lógica Stacking Corregida)
        // ---------------------------------------------------------
        bool mustStackOrDraw = (cardsToDraw > 0);

        // Si hay castigo y NO se permite acumular, robar inmediatamente
        if (mustStackOrDraw && !allowStacking) {
            std::cout << ANSI_RED << "\n[!] CASTIGO: " << currentPlayer->getName()
                      << " roba " << cardsToDraw << " cartas." << ANSI_RESET << std::endl;
            for (int i = 0; i < cardsToDraw; i++) {
                if(!mainDeck.isEmpty()) currentPlayer->addCard(mainDeck.pop());
            }
            currentPlayer->getHand().insertionSort(isDarkStep);
            cardsToDraw = 0;
            turnList.passTurn(clockwise);
            continue; // Pasa al siguiente jugador
        }

        // ---------------------------------------------------------
        // 2. INTERFAZ VISUAL
        // ---------------------------------------------------------
        std::cout << "\n" << ANSI_BOLD << "==========================================================" << ANSI_RESET << std::endl;
        std::cout << "TURNO DE: " << ANSI_BOLD << ANSI_CYAN << currentPlayer->getName() << ANSI_RESET << std::endl;
        std::cout << "MODO: " << (isDarkStep ? ANSI_MAGENTA "LADO OSCURO" : ANSI_YELLOW "LADO NORMAL") << ANSI_RESET << std::endl;
        std::cout << "MESA: " << getCardColorANSI(visualColor) << "[ CARTA ACTIVA ]" << ANSI_RESET
                  << " Valor: " << visualValue << std::endl;

        if (mustStackOrDraw && allowStacking) {
            std::cout << ANSI_RED << ANSI_BOLD << "¡ALERTA! Tienes un castigo de +" << cardsToDraw
                      << ". Juega un +" << (isDarkStep ? "5" : "2/4") << " para acumular o '0' para robar." << ANSI_RESET << std::endl;
        }
        std::cout << ANSI_BOLD << "==========================================================" << ANSI_RESET << std::endl;

        currentPlayer->getHand().showHand(isDarkStep);

        // ---------------------------------------------------------
        // 3. SELECCIÓN DEL JUGADOR
        // ---------------------------------------------------------
        bool turnEnded = false;

        while (!turnEnded) {
            int choice;
            std::cout << "\n>> Opcion: ";
            while (!(std::cin >> choice)) {
                std::cin.clear(); std::cin.ignore(1000, '\n');
            }

            // OPCIÓN 0: ROBAR (O ACEPTAR CASTIGO ACUMULADO)
            if (choice == 0) {
                if (mustStackOrDraw) {
                    // Acepta el castigo acumulado
                    std::cout << "Aceptas el castigo. Robando " << cardsToDraw << " cartas.\n";
                    for (int i = 0; i < cardsToDraw; i++) {
                        if(!mainDeck.isEmpty()) currentPlayer->addCard(mainDeck.pop());
                    }
                    cardsToDraw = 0; // Castigo pagado
                } else {
                    // Robo normal voluntario
                    if (!mainDeck.isEmpty()) {
                        currentPlayer->addCard(mainDeck.pop());
                        std::cout << "Has robado una carta.\n";
                    }
                }
                currentPlayer->getHand().insertionSort(isDarkStep);
                turnEnded = true;
            }
            // OPCIÓN > 0: JUGAR CARTA
            else if (choice >= 1 && choice <= currentPlayer->getHand().getSize()) {
                Card selected = currentPlayer->getHand().getByIndex(choice);

                // VALIDACIÓN ESPECÍFICA SI ESTAMOS EN "GUERRA DE STACKING"
                bool isValid = false;
                if (mustStackOrDraw) {
                    // Solo vale si es el mismo tipo de carta de castigo (ej: +2 sobre +2)
                    if (!isDarkStep && selected.lightType == Type::DRAW_TO && topCard.lightType == Type::DRAW_TO) isValid = true;
                    // Aquí podrías agregar lógica para +4 sobre +2 si quieres reglas muy locas
                    else if (isDarkStep && selected.darkType == Type::DRAW_SIX && topCard.darkType == Type::DRAW_SIX) isValid = true;
                    else {
                        std::cout << ANSI_RED << "¡Movimiento invalido! Debes jugar una carta de suma o robar (0)." << ANSI_RESET << std::endl;
                    }
                } else {
                    // Validación normal
                    isValid = validateMove(selected, topCard);
                }

                if (isValid) {
                    Card playedCard = currentPlayer->playCard(choice);
                    discardPile.push(playedCard);

                    // 1. Actualizar Color Activo INMEDIATAMENTE (Corrige error de colores)
                    if (isDarkStep) {
                        if (playedCard.darkColor != Color::BLACK) activeColor = playedCard.darkColor;
                    } else {
                        if (playedCard.lightColor != Color::BLACK) activeColor = playedCard.lightColor;
                    }

                    // 2. Aplicar Efectos (Calcula saltos y nuevos castigos)
                    applyCardEffect(playedCard, skipTurn); // Pasamos skipTurn por referencia

                    // 3. Verificar Victoria
                    if (currentPlayer->getHand().getSize() == 0) {
                        std::cout << ANSI_GREEN << "\n¡¡¡ " << currentPlayer->getName() << " GANA !!!" << ANSI_RESET << std::endl;
                        gameFinished = true;
                    }
                    // 4. Grito de UNO
                    else if (currentPlayer->getHand().getSize() == 1) {
                        std::string grito;
                        std::cout << ANSI_RED << "¡Te queda 1! Escribe 'UNO': " << ANSI_RESET;
                        std::cin >> grito;
                        // Convertir a mayusculas (simple check)
                        if (grito != "UNO" && grito != "uno") {
                            std::cout << "¡Lento! +2 cartas.\n";
                            if(!mainDeck.isEmpty()) currentPlayer->addCard(mainDeck.pop());
                            if(!mainDeck.isEmpty()) currentPlayer->addCard(mainDeck.pop());
                            currentPlayer->getHand().insertionSort(isDarkStep);
                        }
                    }
                    turnEnded = true;
                } else if (!mustStackOrDraw) {
                    std::cout << ANSI_YELLOW << "Carta no coincide con la mesa." << ANSI_RESET << std::endl;
                }
            } else {
                std::cout << "Numero invalido." << std::endl;
            }
        } // Fin while selección

        // ---------------------------------------------------------
        // 4. PASO DE TURNO (CONTROL CENTRALIZADO)
        // ---------------------------------------------------------
        if (!gameFinished) {
            turnList.passTurn(clockwise);

            // Si hubo una carta de salto (Skip) o Reversa en 2 jugadores
            if (skipTurn) {
                std::cout << ANSI_MAGENTA << ">> Turno Saltado <<" << ANSI_RESET << std::endl;
                turnList.passTurn(clockwise); // Avanzamos uno extra
                skipTurn = false; // Resetear bandera
            }
        }
    }
}

// ----------------------------------------------------------------------
// CORRECCIÓN 2: VALIDACIÓN ROBUSTA (Tipo vs Valor)
// ----------------------------------------------------------------------
bool Game::validateMove(Card play, Card top) {
    if (!isDarkStep) {
        // LADO CLARO
        if (play.lightType == Type::WILD_CARD || play.lightType == Type::CUSTOM) return true;

        // Si la mesa es comodín, validamos contra activeColor
        if (top.lightColor == Color::BLACK) {
            return (play.lightColor == activeColor);
        }

        // CORRECCIÓN: Validar Tipos de Acción explícitamente
        // Evita que un SKIP (val 20) mate a un REVERSE (val 20) si tienen distinto color
        if (play.lightType != Type::NUMBER && top.lightType != Type::NUMBER) {
            if (play.lightType == top.lightType) return true; // Tipo igual mata tipo igual
        }

        // Validación estándar: Color igual o Valor numérico igual
        return (play.lightColor == top.lightColor || play.lightValue == top.lightValue);

    } else {
        // LADO OSCURO
        if (play.darkType == Type::WILD_CARD || play.darkType == Type::DRAW_UNTIL_COLOR) return true;

        if (top.darkColor == Color::BLACK) {
            return (play.darkColor == activeColor);
        }

        // Validación Tipos Acción Oscuros
        if (play.darkType != Type::NUMBER && top.darkType != Type::NUMBER) {
            if (play.darkType == top.darkType) return true;
        }

        return (play.darkColor == top.darkColor || play.darkValue == top.darkValue);
    }
}

// ----------------------------------------------------------------------
// CORRECCIÓN 3 Y 4: EFECTOS Y FLUJO DE TURNO
// ----------------------------------------------------------------------
// Nota: Agregué 'bool& skipTurn' como parámetro para controlar el salto desde el loop principal
void Game::applyCardEffect(Card card, bool& skipTurn) {
    bool isWildLight = (!isDarkStep && (card.lightType == Type::WILD_CARD || card.lightType == Type::CUSTOM));
    bool isWildDark = (isDarkStep && (card.darkType == Type::WILD_CARD || card.darkType == Type::DRAW_UNTIL_COLOR));

    // Pedir color para comodines
    if (isWildLight || isWildDark) {
        int colorOpt;
        std::cout << "Elige color: ";
        if (!isDarkStep) std::cout << "[0:Rojo, 1:Amarillo, 2:Verde, 3:Azul]: ";
        else std::cout << "[4:Rosa, 5:Turquesa, 6:Naranja, 7:Morado]: ";

        while(!(std::cin >> colorOpt)) { std::cin.clear(); std::cin.ignore(1000,'\n'); }

        activeColor = static_cast<Color>(colorOpt);
        // IMPORTANTE: activeColor ya queda seteado para la validación del siguiente turno
    }

    if (!isDarkStep) {
        switch (card.lightType) {
            case Type::DRAW_TO:
                cardsToDraw += 2; // Acumulamos
                break;
            case Type::CUSTOM: // +4
                cardsToDraw += 4; // Acumulamos
                break;
            case Type::REVERSE:
                clockwise = !clockwise;
                std::cout << ">> SENTIDO CAMBIADO <<\n";
                // En 2 jugadores, Reversa = Salto
                if (numPlayers == 2) skipTurn = true;
                break;
            case Type::JUMP:
                skipTurn = true; // Marcamos para saltar al final del loop
                break;
            case Type::FLIP:
                performFlip();
                break;
            default: break;
        }
    } else {
        switch (card.darkType) {
            case Type::DRAW_SIX: // Ojo, en tu enum puede ser DRAW_FIVE
                cardsToDraw += 5;
                break;
            case Type::REVERSE:
                clockwise = !clockwise;
                std::cout << ">> SENTIDO CAMBIADO (DARK) <<\n";
                if (numPlayers == 2) skipTurn = true;
                break;
            case Type::JUMP_ALL:
                // Salta a todos = Juega otra vez.
                // Truco: Saltamos N-1 turnos o simplemente marcamos skipTurn especial.
                // Simplificación: Saltamos un turno pero invertimos dirección temporalmente
                // Para efectos prácticos, Jump All suele significar "Tiro otra vez"
                std::cout << ">> SALTO A TODOS (Tiras de nuevo) <<\n";
                // Lógica rápida: retroceder el iterador para que al avanzar caiga en mí mismo
                turnList.passTurn(!clockwise);
                break;
            case Type::FLIP:
                performFlip();
                break;
            default: break;
        }
    }
}

void Game::performFlip() {
    isDarkStep = !isDarkStep;
    std::cout << ANSI_BOLD << ANSI_MAGENTA << "\n¡¡¡ FLIP !!! EL MUNDO SE INVIERTE..." << ANSI_RESET << std::endl;

    // ESTRATEGIA MERGE SORT MASIVO
    // Recorremos a todos los jugadores para reordenar sus manos
    // Nota: Iteramos numPlayers veces
    for (int i = 0; i < numPlayers; i++) {
        Player* p = turnList.getCurrentPlayer();
        p->getHand().mergeSort(isDarkStep);
        turnList.passTurn(true); // Rotación temporal solo para acceder a la lista
    }
    // Al terminar el bucle, volvemos al jugador que tiró la carta (o al siguiente según lógica de turno)
}

// Función de Validación de Jugada
bool isMoveValid(Card play, Card top, bool isDarkStep, Color chosenColor) {
    if (!isDarkStep) {
        // LADO CLARO
        if (play.lightType == Type::WILD_CARD || play.lightType == Type::CUSTOM) return true;
        if (top.lightColor == Color::BLACK) return (play.lightColor == chosenColor);
        return (play.lightColor == top.lightColor || play.lightValue == top.lightValue);
    } else {
        // LADO OSCURO
        if (play.darkType == Type::WILD_CARD || play.darkType == Type::DRAW_UNTIL_COLOR) return true;
        if (top.darkColor == Color::BLACK) return (play.darkColor == chosenColor); // O el color dark elegido
        return (play.darkColor == top.darkColor || play.darkValue == top.darkValue);
    }
}