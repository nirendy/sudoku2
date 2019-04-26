#include "game_aux.h"

/* TODO: module explanation */
/* TODO: copy classify from h*/


void printChange(int i, int j, int value) {
    printf("The value of the cell <%d,%d> set back to %d\n", i + 1, j + 1, value);
}

void setCoordinate(Game *game, Input input) {
    game->user_matrix[input.coordinate.i][input.coordinate.j] = 0;
    updateAfterClearErrorMatrix(game, input);
    if (input.value != 0) {
        updateAfterSetErrorMatrix(game, input);
        game->user_matrix[input.coordinate.i][input.coordinate.j] = input.value;
    }
}

void updateRedoUndoInputsAfterSingleSet(Game *game, Input in, Input *redo, Input *undo) {

    redo->coordinate = in.coordinate;
    undo->coordinate = in.coordinate;

    redo->value = in.value;
    undo->value = game->user_matrix[in.coordinate.i][in.coordinate.j]; /*only good for a single set*/

}

void insertInputsToList(Input *redoInputs, Input *undoInputs, int numOfInputs) {
    int k;

    g_curNode = insertAfterNode(g_curNode);
    for (k = 0; k < numOfInputs; k++) {
        g_curNode->currDataNode = insertAfterDataNode(g_curNode->currDataNode, redoInputs[k], undoInputs[k]);
    }

}

void chooseRandCords(Coordinate *source, int sourceSize, Coordinate *target, int targetSize) {
    int i, r, limit;
    int *randIndexes = (int *) smartMalloc(sourceSize * sizeof(int));

    for (i = 0; i < sourceSize; i++) { randIndexes[i] = i; }
    limit = sourceSize;
    for (i = 0; i < targetSize; i++) {
        r = randLimit(limit);
        target[i] = source[randIndexes[r]];
        randIndexes[r] = randIndexes[limit - 1];
        limit--;
    }

    free(randIndexes);
}

int diffToRedoUndoLists(Board original, Board final, Input *redoList, Input *undoList) {

    int i, j, listIndex = 0;
    for (i = 0; i < g_gameDim.N; ++i) {
        for (j = 0; j < g_gameDim.N; ++j) {
            if (original[i][j] != final[i][j]) {
                redoList[listIndex].coordinate.i = i;
                redoList[listIndex].coordinate.j = j;
                undoList[listIndex].coordinate.i = i;
                undoList[listIndex].coordinate.j = j;
                redoList[listIndex].value = final[i][j];
                undoList[listIndex].value = original[i][j];
                listIndex++;
            }
        }
    }

    return listIndex;
}

int numOfDiffs(Board original, Board final) {
    int i, j, diffs = 0;
    for (i = 0; i < g_gameDim.N; ++i) {
        for (j = 0; j < g_gameDim.N; ++j) {
            if (original[i][j] != final[i][j]) {
                diffs++;
            }
        }
    }
    return diffs;
}

void performSetsFromRedoList(Game *game, Input *sets, int len) {
    int k;
    for (k = 0; k < len; k++) {
        setCoordinate(game, sets[k]);
    }
}

void updateHistoryList(Game *game, Board final) {

    int numOfSets;
    Input *redoInputs;
    Input *undoInputs;

    Board original = game->user_matrix;
    numOfSets = numOfDiffs(original, final);
    if (numOfSets == 0) {
        destroyBoard(final, g_gameDim);
        return;
    }
    redoInputs = (Input *) smartMalloc(numOfSets * sizeof(Input));
    undoInputs = (Input *) smartMalloc(numOfSets * sizeof(Input));

    diffToRedoUndoLists(original, final, redoInputs, undoInputs);
    insertInputsToList(redoInputs, undoInputs, numOfSets);
    performSetsFromRedoList(game, redoInputs, numOfSets); /*update the user board*/

    destroyBoard(final, g_gameDim);
    free(redoInputs);
    free(undoInputs);
}

Bool fillXRandomCells(Board board, Coordinate *cellsToFill, int numToFill) {

    int k, numOfPossibleValues;
    int *possibleValues = (int *) smartMalloc(g_gameDim.N * sizeof(int *));

    for (k = 0; k < numToFill; k++) {
        numOfPossibleValues = getPossibleValues(board, cellsToFill[k], possibleValues);
        if (numOfPossibleValues == 0) { return false; }
        board[cellsToFill[k].i][cellsToFill[k].j] = possibleValues[randLimit(numOfPossibleValues)];
    }
    return true;
}

void clearRandomCells(Board board, Coordinate *cellsToClear, int numToClear) {

    int k;
    for (k = 0; k < numToClear; k++) {
        board[cellsToClear[k].i][cellsToClear[k].j] = 0;
    }

}

int chooseCellsToFill(Board board, Coordinate *cellsToFill, int sizeToFill) {

    Coordinate *emptyCells = (Coordinate *) smartMalloc(g_gameDim.cellsCount * sizeof(Coordinate));
    int numOfEmpty = getEmptyCells(board, emptyCells);
    chooseRandCords(emptyCells, numOfEmpty, cellsToFill, sizeToFill);
    free(emptyCells);
    return numOfEmpty;
}

void chooseCellsToClear(Board board, Coordinate *cellsToClear, int numToClear) {

    Coordinate *filledCells = (Coordinate *) smartMalloc(g_gameDim.cellsCount * sizeof(Coordinate));
    getFilledCells(board, filledCells); /* TODO: before submitting, use the returned value*/
    chooseRandCords(filledCells, g_gameDim.cellsCount, cellsToClear, numToClear);

    free(filledCells);
}

void fillObviousValues(Board board) {

    Coordinate *emptyCells = (Coordinate *) smartMalloc(g_gameDim.cellsCount * sizeof(Coordinate));
    int *possibleValues = (int *) smartMalloc(g_gameDim.cellsCount * sizeof(int));

    int numOfEmpty = getEmptyCells(board, emptyCells);
    int numOfPossibleValues;
    int numOfCellsToFill = 0;

    int k;
    for (k = 0; k < numOfEmpty; k++) {
        numOfPossibleValues = getPossibleValues(board, emptyCells[k], possibleValues);
        if (numOfPossibleValues == 1) {
            board[emptyCells[k].i][emptyCells[k].j] = possibleValues[0];
            numOfCellsToFill++;
        }
    }

    free(emptyCells);
    free(possibleValues);
}

Bool isGameErroneous(Game *game) {
    int i, j;
    for (i = 0; i < g_gameDim.N; ++i) {
        for (j = 0; j < g_gameDim.N; ++j) {
            if (game->error_matrix[i][j] == 1) {
                return true;
            }
        }
    }
    return false;
}

Bool isSolvableBoard(Board board) {
    Bool isValid;
    Board solutionBoard = createBoard();
    isValid = fillSolutionMatrix(board, solutionBoard);
    destroyBoard(solutionBoard, g_gameDim);
    return isValid;
}

void clearGame(Game *game) {
    clearBoard(game->user_matrix);
    clearBoolBoard(game->fixed_matrix);
    clearBoolBoard(game->error_matrix);
}