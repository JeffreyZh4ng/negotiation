#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>

#define CARD_COUNT 104

typedef struct card {
    int suit;
    int value;
} card;

typedef struct player {
    char *name;
    int score;
    card **hand;
    size_t handSize;
} player;

// Forward declare functions
void createDeck();
void printScores(player *dealer, player *reciever);
void shuffle(card **array, size_t n);
void initializePlayer(player* p, char *name);
card *drawCards(player *p, int cardNum);
void displayHand(card **hand, size_t len);
void displayCard(card *displayCard, size_t index);
card *removeCard(player *p, size_t index);
void addCard(player *p, card *addedCard);
void changeScore(player *p, card *addedCard);
int *isLineValid(player *p, char *line);
void addCardToField(card *addedCard);
void sortDescending(int *arr, size_t n);
void displayHiddenField(card **hiddenField);
void chooseCardsOnField(player *dealer, player *reciever);

// Static variables to keep track of the deck
static card **deck;
static size_t deckSize = CARD_COUNT;
static card ** field;
static size_t fieldSize = 0;

// Main game loop
int main(int argc, char *argv[]) {

    // Check if the user has entered the correct arguments to play the game
    if (argc != 3) {
        printf("In order to play, run ./negotiation [Player1_Name] [Player2_Name]\n");
        return 1;
    }

    createDeck();
    field = (card**) malloc(sizeof(card*) * 5);

    player *playerOne = (player*) malloc(sizeof(player));
    initializePlayer(playerOne, argv[1]);

    player *playerTwo = (player*) malloc(sizeof(player));
    initializePlayer(playerTwo, argv[2]);

    // Enter the main game loop after creating the players. Keep track of whose turn it is
    // Each player should have 8 turns with 2 decks
    player *dealer = playerOne;
    player *reciever = playerTwo;
    int roundCount = 0;

    // This will clear the screen
    printf("\e[1;1H\e[2J");
    printf("%s's turn!\n\n", dealer->name);

    while (deckSize > 0) {
        char *line = NULL;
        size_t sizeCap = 0;
        ssize_t lineLen = 0;

        // Clear screen hopefully
        printf("Current hand:\n");
        displayHand(dealer->hand, dealer->handSize);
        printf("\n");

        // displayNewTurnInfo();
        card *actionCard = drawCards(dealer, 1);
        printf("Action card: ");
        displayCard(actionCard, 0);
        printScores(dealer, reciever);

        while (1) {
            printf("To use action card, enter Y. To discard, enter N\n");
            printf("If you use the action card and the value is even, you will take a random card from your opponent\n");
            printf("And your opponents score will increase by the value. If the value is odd, you will give a random card\n");
            printf("to your opponent but their score will decrease by the value of the card\n");
            lineLen = getline(&line, &sizeCap, stdin);
            line[lineLen-1] = '\0';

            if (strcmp(line, "Y") == 0) {
                card *removedCard;
                if (actionCard->value % 2 == 0) {
                    removedCard = removeCard(reciever, (reciever->handSize)-1);
                    addCard(dealer, removedCard);
                } else {
                    card *removedCard = removeCard(dealer, (dealer->handSize)-1);
                    addCard(reciever, removedCard);
                }
                changeScore(reciever, removedCard);

                break;

            } else if (strcmp(line, "N") == 0) {
                card *removedCard =removeCard(dealer, (dealer->handSize)-1);
                free(removedCard);
                break;
            }
        }

        drawCards(dealer, 5);
        printf("\n");
        displayHand(dealer->hand, dealer->handSize);

        while (1) {
            printf("Pick five cards to place on the field\n");
            printf("Enter the cards by entering their indecies in a row\n");
            printf("Do not put any spaces at the beginning or end. EX: 3 5 8 2 9\n");
            lineLen = getline(&line, &sizeCap, stdin);

            int *indecies = isLineValid(dealer, line);
            if (indecies != NULL) {
                for (size_t i = 0; i < 5; i++) {
                    card *removedCard = removeCard(dealer, indecies[i]-1);
                    addCardToField(removedCard);
                }

                break;
            }
        }

        printf("\nCurrent Field:\n");
        displayHand(field, fieldSize);

        printf("Turn finished. Moving to %s's turn!\n", reciever->name);
        player *temp = dealer;
        dealer = reciever;
        reciever = temp;
        sleep(5);

        // The new dealer will pick the cards
        printf("\e[1;1H\e[2J");
        printf("%s's turn!\n\n", dealer->name);
        chooseCardsOnField(dealer, reciever);
    }

    printf("%s's score: %d. %s's score: %d\n", dealer->name, dealer->score,
                        reciever->name, reciever->score);
    if (dealer->score > reciever->score) {
        printf("%s wins!\n", dealer->name);
    } else if (reciever->score > dealer->score) {
        printf("%s wins!\n", reciever->name);
    } else {
        printf("It's a tie!\n");
    }

    return 0;
}

// Initialzies players
void initializePlayer(player *p, char *name) {
    p->name = strdup(name);
    p->hand = (card**) malloc(sizeof(card*) * 20);
    p->handSize = 0;
    p->score = 0;
    drawCards(p, 4);
}

// Simple helper to print scores
void printScores(player *dealer, player *reciever) {
    printf("Your score: %d. Your opponent's score: %d\n", dealer->score, reciever->score);
}

// Creates the playing deck with two standard decks
void createDeck() {
    deck = (card**) malloc(sizeof(card*) * CARD_COUNT);

    // Two decks, four suits, 13 cards each
    for (size_t i = 0; i < 2; i++) {
        for (size_t j = 1; j <= 4; j++) {
            for (size_t k = 1; k <= 13; k++) {
                card *newCard = (card*) malloc(sizeof(card));
                newCard->suit = j;
                newCard->value = k;

                size_t index = (i * 52) + ((j-1) * 13) + (k-1);
                deck[index] = newCard;
            }
        }
    }

    shuffle(deck, CARD_COUNT);
}

// Code to shuffle an array taken from StackOverflow
// https://www.geeksforgeeks.org/shuffle-a-given-array/
void shuffle(card **arr, size_t n) {

    // Use a different seed value so that we don't get same
    // result each time we run this program
    srand (time(NULL));

    // Start from the last element and swap one by one. We don't
    // need to run for the first element that's why i > 0
    for (int i = n-1; i > 0; i--)
    {
        // Pick a random index from 0 to i
        int j = rand() % (i+1);

        // Swap arr[i] with the element at random index
        card *temp = arr[i];
        arr[i] = arr[j];
        arr[j] = temp;
    }
}

// Draws cards for the player and puts them in the players deck. If only one card
// Is draw, that will get returned. Will return NULL otherwise
card *drawCards(player *p, int cardNum) {
    card *newCard;
    for (size_t i = 0; i < cardNum; i++) {
        newCard = deck[deckSize - 1];
        deck[deckSize - 1] = NULL;
        deckSize--;

        p->hand[p->handSize] = newCard;
        p->handSize += 1;
    }

    if (cardNum == 1) {
        return newCard;
    }

    return NULL;
}

// Prints out the players hand
void displayHand(card **hand, size_t len) {
    for (size_t i = 0; i < len; i++) {
        displayCard(hand[i], i);
    }
}

// Helper to display a card
void displayCard(card *displayCard, size_t index) {
    size_t suitVal = displayCard->suit;
    char *suit;
    if (suitVal == 1) {
        suit = "Hearts";
    } else if (suitVal == 2) {
        suit = "Spades";
    } else if (suitVal == 3) {
        suit = "Clubs";
    } else {
        suit = "Diamonds";
    }

    int val = (int) displayCard->value;
    char *valStr;
    if (val <= 10) {
        // Cant use atoi for some reason on my machine
        printf("%zu) [%d of %s]\n", index + 1, val, suit);
        return;
    } else if (val == 11) {
        valStr = "J";
    } else if (val == 12) {
        valStr = "Q";
    } else {
        valStr = "K";
    }

    printf("%zu) [%s of %s]\n", index + 1, valStr, suit);
}

// Removes a card and returns it. Adjusts the players hand
card *removeCard(player *p, size_t index) {
    card *removedCard = p->hand[index];
    memmove((p->hand + index), (p->hand + index + 1), p->handSize - index);
    p->handSize--;

    return removedCard;
}

// Adds a card to the players hand
void addCard(player *p, card *addedCard) {
    p->hand[p->handSize] = addedCard;
    p->handSize++;
}

// Changes the score of the player based on the card
void changeScore(player *p, card *addedCard) {
    if (addedCard->value % 2 == 0) {
        p->score += addedCard->value;
    } else {
        p->score -= addedCard->value;
    }
}

// Will check if the player entered a parsable line
int *isLineValid(player *p, char *line) {
    int *indecies = (int*) malloc(sizeof(int) * 5);
    char *token = strtok(line, " ");
    int index = atoi(token);

    if (token == NULL || index == 0 || index >= p->handSize) {
        free(indecies);
        return NULL;
    } else {
        indecies[0] = atoi(token);
    }

    for (size_t i = 1; i < 5; i++) {
        token = strtok(NULL, " ");
        index = atoi(token);

        // Check if the character was not NAN or if there were no more charaters
        if (token == NULL || index == 0 || index >= p->handSize) {
            free(indecies);
            return NULL;
        } else {

            // Check if there are ducplicate indecies
            for (size_t j = 0; j < i; j++) {
                if (indecies[j] == index) {
                    free(indecies);
                    return NULL;
                }
            }

            indecies[i] = index;
        }
    }

    sortDescending(indecies, 5);
    return indecies;
}

// Will add a card to the playing field
void addCardToField(card *addedCard) {
    field[fieldSize] = addedCard;
    fieldSize++;
}

// Method to sort an array in descending order. Taken from
// https://www.sanfoundry.com/c-program-sort-array-descending-order/
void sortDescending(int *arr, size_t n) {
    for (int i = 0; i < n; ++i) {
        for (int j = i + 1; j < n; ++j) {
            if (arr[i] < arr[j]) {
                int a = arr[i];
                arr[i] = arr[j];
                arr[j] = a;
            }
        }
    }
}

// Helper method that will print out the hidden field for a player
void displayHiddenField(card **hiddenField) {
    for (size_t i = 0; i < 5; i++) {
        if (hiddenField[i] == NULL) {
            printf("%zu) [??? of ???]\n", i + 1);
        } else {
            displayCard(hiddenField[i], i);
        }
    }
}

// Frees the cards in the field and resets the field;
void clearField() {
    for (size_t i = 0; i < 5; i++) {
        free(field[i]);
        field[i] = NULL;
    }

    fieldSize = 0;
}

// Method that will let the dealer choose cards from the field
void chooseCardsOnField(player *dealer, player *reciever) {
    card **hiddenField = (card**) calloc(5, sizeof(int));
    int flippedCards = 0;
    displayHiddenField(hiddenField);

    int index = -1;
    while (!(index >= 1 && index <= 5)) {
        printf("Pick one card to flip (You must flip at least one card).\n");
        scanf("%d", &index);
    }

    hiddenField[index - 1] = field[index - 1];
    flippedCards++;
    printf("\nYou picked: ");
    displayCard(field[index - 1], index - 1);
    changeScore(dealer, field[index - 1]);
    printf("Your score: %d. Your opponent's score: %d\n", dealer->score, reciever->score);

    char *line = NULL;
    size_t sizeCap = 0;
    ssize_t lineLen = 0;

    while (1) {
        printf("\n");
        displayHiddenField(hiddenField);
        printf("Enter a index to flip another card. Otherwise, enter 'Done'\n");
        lineLen = getline(&line, &sizeCap, stdin);
        line[lineLen - 1] = '\0';

        if (strcmp(line, "Done") == 0) {
            break;
        }

        int index = atoi(line);
        if (index >= 1 && index <= 5 && hiddenField[index - 1] == NULL) {
            hiddenField[index - 1] = field[index - 1];
            flippedCards++;
            printf("\nYou picked: ");
            displayCard(field[index - 1], index - 1);
            changeScore(dealer, field[index - 1]);
            printf("Your score: %d. Your opponent's score: %d\n", dealer->score, reciever->score);

            if (flippedCards == 5) {
                break;
            }
        }
    }

    if (flippedCards != 5 && dealer->handSize > 0) {
        printf("\nThere are still hidden cards. Sacrifice a random card to your opponent and the hidden cards will be revealed\n");
        while (1) {
            printf("Enter Y to sacrifice. Enter N to pass.\n");
            lineLen = getline(&line, &sizeCap, stdin);
            line[lineLen-1] = '\0';

            if (strcmp(line, "Y") == 0) {
                card *removedCard = removeCard(dealer, dealer->handSize - 1);
                break;

            } else if (strcmp(line, "N") == 0) {
                clearField();
                return;
            }
        }
    }

    displayHand(field, fieldSize);
    while (1) {
        printf("Enter Done to once finished viewing the cards\n");
        lineLen = getline(&line, &sizeCap, stdin);
        line[lineLen - 1] = '\0';
        if (strcmp(line, "Done") == 0) {
            break;
        }
    }
}
