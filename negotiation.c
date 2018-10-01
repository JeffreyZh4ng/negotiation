#include <string.h>
#include <stdlib.h>

#define CARD_COUNT 104

typedef struct player {
    char *name;
    card **hand;
    size_t handSize;
    size_t score;
} player;

typedef struct card {
    int suite;
    int value;
} card;

// Forward declare functions
void createDeck();
void shuffle(card **array, size_t n);

// Static variables to keep track of the deck
static card **deck;
static size_t deckSize = CARD_COUNT;
card *drawCards(player *p, int cardNum);
void displayHand(player *p);

// Main game loop
int main(int argc, char *argv[]) {

    // Check if the user has entered the correct arguments to play the game
    if (argc != 3) {
        printf("In order to play, run ./negotiation [Player1_Name] [Player2_Name]\n");
        return 1;
    }

    createDeck();

    player *playerOne = (player*) malloc(sizeof(player));
    initializePlayer(playerOne);

    player *playerTwo = (player*) malloc(sizeof(player));
    initializePlayer(playerTwo);

    // Enter the main game loop after creating the players. Keep track of whose turn it is
    // Each player should have 8 turns with 2 decks
    player *dealer = playerOne;
    player *reciever = playerTwo;
    while (deckSize > 0) {

        // Clear screen hopefully
        printf("\e[1;1H\e[2J");

        // displayNewTurnInfo();
        card *actionCard = drawCards(currentPlayer, 1);
        drawCards(currentPlayer, 5);

    }

    cleanUp();
    return 0;
}

// Initialzies players
void initializePlayer(player *p) {
    p->name = strdup(argv[1]);
    p->hand = (card**) malloc(sizeof(card*) * 20);
    p->handSize = 0;
    p->score = 0;
    drawCards(p, 4);
}

// Creates the playing deck with two standard decks
void createDeck() {
    deck = (card**) malloc(sizeof(card*) * CARD_COUNT);

    // Two decks, four suites, 13 cards each
    for (size_t i = 0; i < 2; i++) {
        for (size_t j = 1; j <= 4; j++) {
            for (size_t k = 1; k <= 13; k++) {
                card *newCard = (card*) malloc(sizeof(card));
                newCard->suite = j;
                newCard->value = k;

                size_t index = (i * 52) + ((j-1) * 13) + (k-1);
                deck[index] = newCard
            }
        }
    }

    shuffle();
}

// Code to shuffle an array taken from StackOverflow
// https://stackoverflow.com/questions/6127503/shuffle-array-in-c
void shuffle(card **array, size_t n) {
    if (n > 1) {
        size_t i;
        for (size_t i = 0; i < n - 1; i++) {
          size_t j = i + rand() / (RAND_MAX / (n - i) + 1);
          card *t = *(array + j);
          *(array + j) = *(array + i);
          *(array + i) = t;
        }
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
        p->handSize++;
    }

    if (cardNum == 1) {
        return newCard;
    }

    return NULL;
}

void displayHand(player *p) {
    size_t len = p->handSize;
    for (size_t i = 0; i < len; i++) {
        printf("*-----*");
    }
    printf("\n");

    for (size_t i = 0; i < len; i++) {
        printf("|%zu   %zu|", (p->hand + i)->value, (p->hand + i)->suite);
    }
    printf("\n");

    for (size_t i = 0; i < 3; i++) {
        for (size_t i = 0; i < len; i++) {
            printf("|     |");
        }
        printf("\n");
    }
    
    for (size_t i = 0; i < len; i++) {
        printf("|%zu   %zu|", (p->hand + i)->suite), (p->hand + i)->value);
    }
    printf("\n");

    for (size_t i = 0; i < len; i++) {
        printf("*-----*");
    }
    printf("\n");
}
