#ifndef HW2_GAME_H
#define HW2_GAME_H

#include "main_aux.h"

Bool isCoordinateFixed(Game *game, Coordinate coordinate);

Bool isCoordinateEmpty(Game *game, Coordinate coordinate);

Bool setCoordinate(Game *game, Input input);

void hint(Game *game, Input input);

void validate(Game *game);

void clearGame(Game *game);

#endif
