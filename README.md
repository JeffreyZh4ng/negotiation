# card-game

Original card game made by Jeffrey Zhang.

#Instructions
To run, first compile negotiations.c with gcc then run
the output (usually ./a.out)

# Rules:
Each player starts with 4 cards

Even cards are worth + points, odd cards are worth - points. Jack = 11, Queen = 12 and King = 13

At the start of each players turn, they will draw 6 cards. The first card they draw is the action card. If the drawer of the card chooses to take the action, the opponent will have the card added to their score. If the action card is negative (the opponents score will decrease), the opponent will take a card from the drawer. If the action card is positive (the opponents score will be increased), the drawer will take a card from the opponent. If no action is taken, the card is revealed and discarded.

The current player will then place 5 cards face down on the field.

The opponent must flip 1 card maximum that the opponent has placed down. The flipped card is added to the score. After the opponent has finished flipping the cards, the opponent can give a random card to their opponent to reveal all the cards that the other player has placed down. This can reveal a significant amount of information. Otherwise, the cards are discarded.

The player with the highest score in the end wins

# Explanation:
I designed this game so that the computer would have to switch places as little as possible.
One player will pick card to place on the field, then the other player will flip a few cards
then they will pick cards to put on the field for the other person.

After each turn, the screen is cleared so that the other player cant see the opponents cards.
My objective was to create a game that would allow a player who is good at counting cards to
win a majority of the time. Probabilities can be reduces with taking / giving cards
to your opponent and sacrificing a card to see the field can give a lot of info but has
a fair cost to it.

#Side Note:
Unfortunately I was rather rushed with school work and I cant garuntee the program is bug free
