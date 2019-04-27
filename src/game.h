#ifndef HW2_GAME_H
#define HW2_GAME_H

#include "main_aux.h"

/*Game module - responsible for managing the game flow and the commands' execution*/

Bool askUserForNextTurn(Input *input);

void executeCommand(Game **gameP, Input input);

Bool checkLegalInput(Game *game, Input input);

#endif
