#include "parser.h"

int isFixed(BoolBoard fixed_matrix, int i, int j) {
    return fixed_matrix[i][j];
}

int getType() {
    return 0;
}


/*return 0 only if finished successfully */
FinishCode parseDimension() {
    n = 3;
    m = 3;

    return FC_SUCCESS;
}

/*return 0 only if finished successfully */
FinishCode parseHintsAmount(int *hintsAmount) {
    printPrompt(PEnterFixedAmount, 0);

    /*
     * read input
     * */
    if (scanf("%d", hintsAmount) != 1) {
        char isEOF;
        if (scanf("%c", &isEOF) != EOF) {
            /* This part is just that the programs works as the example program*/
            printf("Error: not a number\n");
            return FC_EOF;
        }
        return FC_INVALID_TERMINATE;
    }

    /*
     * Validate input
     * */
    if (!(0 <= *hintsAmount && *hintsAmount <= n * n * m * m - 1)) {
        printError(EInvalidNumberOfCells, 0);
        *hintsAmount = -1;
        return FC_INVALID_RECOVERABLE;
    }

    return FC_SUCCESS;
}

void printSepRow(int len) {
    int i = 0;
    for (i = 0; i < len; i++) {
        printf("-");
    }

    printf("\n");

}


void printBoard(Board matrix, BoolBoard fixed_matrix) {
    const char SPACE = ' ', PIPE = '|', ASTERISK = '*', DOT = '.', NEWLINE = '\n';
    int N = n * m;
    int len = 4 * N + m + 1;
    int i = 0, j = 0, k = 0;
    int type;

    printSepRow(len);
    for (i = 0; i < n; i++) {
        for (j = 0; j < m; j++) {
            printf("%c", PIPE);

            for (k = 0; k < len; k++) {

                printf("%c", SPACE);
                printf("%2d", matrix[i][j]);
                type = getType();
                switch (type) {
                    case 0:
                        printf("%c", SPACE);
                        break;
                    case 1:
                        printf("%c", DOT);
                        break;
                    case 2:
                        printf("%c", ASTERISK);
                        break;

                }

            }

            printf("%c", PIPE);
        }
        printf("%c", NEWLINE);
        printSepRow(len);
    }


    printf("%c", SPACE);
    printf("%s", "sep");
}

/*
   * Categorize token to commands
   * */
int ClassifyCommand(char *token, Input *returnedInputP) {
    int numOfVars = 0;

    if (!strcmp(token, "solve")) {
        numOfVars = 1;
        returnedInputP->command = SOLVE;
    }
    if (!strcmp(token, "edit")) {
        numOfVars = 1;
        returnedInputP->command = EDIT;
    }
    if (!strcmp(token, "mark_errors")) {
        numOfVars = 1;
        returnedInputP->command = MARK_ERRORS;
    }
    if (!strcmp(token, "print_board")) {
        numOfVars = 0;
        returnedInputP->command = PRINT_BOARD;
    }
    if (!strcmp(token, "set")) {
        numOfVars = 3;
        returnedInputP->command = SET;
    }
    if (!strcmp(token, "validate")) {
        numOfVars = 0;
        returnedInputP->command = VALIDATE;
    }
    if (!strcmp(token, "guess")) {
        numOfVars = 1;
        returnedInputP->command = GUESS;
    }
    if (!strcmp(token, "generate")) {
        numOfVars = 2;
        returnedInputP->command = GENERATE;
    }
    if (!strcmp(token, "undo")) {
        numOfVars = 0;
        returnedInputP->command = UNDO;
    }
    if (!strcmp(token, "redo")) {
        numOfVars = 0;
        returnedInputP->command = REDO;
    }
    if (!strcmp(token, "save")) {
        numOfVars = 1;
        returnedInputP->command = SAVE;
    }
    if (!strcmp(token, "hint")) {
        numOfVars = 2;
        returnedInputP->command = HINT;
    }
    if (!strcmp(token, "guess_hint")) {
        numOfVars = 2;
        returnedInputP->command = GUESS_HINT;
    }
    if (!strcmp(token, "num_solutions")) {
        numOfVars = 0;
        returnedInputP->command = NUM_SOLUTIONS;
    }
    if (!strcmp(token, "autofill")) {
        numOfVars = 0;
        returnedInputP->command = AUTOFILL;
    }
    if (!strcmp(token, "reset")) {
        numOfVars = 0;
        returnedInputP->command = RESET;
    }
    if (!strcmp(token, "exit")) {
        numOfVars = 0;
        returnedInputP->command = EXIT;
    }

    return numOfVars;
}


FinishCode parseCommand(Input *returnedInput) {
    char str[1024];
    /* TODO: make constant */
    char *token, command[15];
    int numOfVars = -1;
    int x = 0, y = 0, value = -1;
    int index = 0;
    returnedInput->command = COMMAND_INVALID;

    /*
     * Do until a non empty line received
     * */
    do {
        if (fgets(str, 1024, stdin) == NULL) {
            return FC_EOF;
        }
        token = strtok(str, " \t\r\n");
    } while (token == NULL);

    numOfVars = ClassifyCommand(token, &returnedInput);

    if (numOfVars == -1) {
        printError(EInvalidCommand, COMMAND_INVALID);
        return FC_INVALID_RECOVERABLE;
    }

    /*
     *
     * */
    strcpy(command, token);

    token = strtok(NULL, " \t\r\n");
    index = 1;

    /*
     * while not all expected parameters has been interpreted
     * */
    while (token != NULL && index <= numOfVars) {

        switch (index) {
            case 1:
                x = token[0] - '0';
                break;
            case 2:
                y = token[0] - '0';
                break;
            case 3:
                value = token[0] - '0';
                break;
            default:
                printf("Unreachable Code Error");
                return FC_UNEXPECTED_ERROR;
        }

        index++;
        token = strtok(NULL, " \t\r\n");
    }

    /*
     * if X,Y in needed for command make the values changed
     * if VALUE in needed for command make the values changed
     * */
    if (
            (numOfVars >= 2 && (x == 0 || y == 0))
            || (numOfVars == 3 && value == -1)
            ) {
        returnedInput->command = COMMAND_INVALID;
        printError(EInvalidCommand, COMMAND_INVALID);
        return FC_INVALID_RECOVERABLE;
    }

    returnedInput->coordinate = createCoordinate(y - 1, x - 1);
    returnedInput->value = value;

    return FC_SUCCESS;
}

