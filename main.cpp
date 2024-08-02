#include <iostream>
#include <cstdlib>
#include <ctime>
#include <vector>
#include <limits>

using namespace std;

const int HOME_POSITION = 100;
const int BOARD_SIZE = 52;

class Token {
public:
    int position;
    bool inPlay;

    Token() {
        position = -1; // -1 indicates the token is not yet in play
        inPlay = false;
    }

    void move(int steps) {
        if (position != -1) {
            position += steps;
            if (position >= BOARD_SIZE) {
                position -= BOARD_SIZE;
            }
        }
    }

    void enterPlay() {
        if (!inPlay) {
            position = 0; // Enter the board
            inPlay = true;
        }
    }

    bool hasWon() const {
        return position == HOME_POSITION;
    }
};

class Player {
public:
    vector<Token> tokens;

    Player(int tokenCount = 4) {
        for (int i = 0; i < tokenCount; i++) {
            tokens.push_back(Token());
        }
    }

    bool allTokensInHome() const {
        for (const auto& token : tokens) {
            if (!token.hasWon()) {
                return false;
            }
        }
        return true;
    }

    bool hasTokensInPlay() const {
        for (const auto& token : tokens) {
            if (token.inPlay && !token.hasWon()) {
                return true;
            }
        }
        return false;
    }

    void moveToken(int tokenIndex, int steps) {
        tokens[tokenIndex].move(steps);
    }

    void enterTokenIntoPlay() {
        for (auto& token : tokens) {
            if (!token.inPlay) {
                token.enterPlay();
                break;
            }
        }
    }
};

void displayBoard(const vector<Player>& players) {
    cout << "\nCurrent Board:\n";
    for (int i = 0; i < players.size(); i++) {
        cout << "Player " << i + 1 << " Tokens: ";
        for (const auto& token : players[i].tokens) {
            if (token.position == -1) {
                cout << "Out ";
            } else if (token.hasWon()) {
                cout << "Home ";
            } else {
                cout << token.position << " ";
            }
        }
        cout << endl;
    }
    cout << "\n";
}

int rollDice() {
    return (rand() % 6) + 1;
}

int chooseToStart(int numPlayers) {
    int currentPlayer = 0;
    int highestRoll = 0;
    int startingPlayer = -1;
    vector<int> rolls(numPlayers, 0);

    while (true) {
        cout << "Player " << currentPlayer + 1 << "'s turn. Press Enter to roll the dice.";
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cin.get();

        int diceRoll = rollDice();
        rolls[currentPlayer] = diceRoll;
        cout << "Player " << currentPlayer + 1 << " rolled a " << diceRoll << "\n";

        if (diceRoll == 6) {
            startingPlayer = currentPlayer;
            break;
        }

        if (diceRoll > highestRoll) {
            highestRoll = diceRoll;
            startingPlayer = currentPlayer;
        }

        currentPlayer = (currentPlayer + 1) % numPlayers;

        if (currentPlayer == 0) {
            break;
        }
    }

    if (startingPlayer == -1) {
        // Find the player with the highest roll if no one rolled a 6
        for (int i = 0; i < numPlayers; i++) {
            if (rolls[i] > highestRoll) {
                highestRoll = rolls[i];
                startingPlayer = i;
            }
        }
    }

    return startingPlayer;
}

void playerTurn(Player& player) {
    int diceRoll = rollDice();
    cout << "You rolled a " << diceRoll << endl;

    if (diceRoll == 6) {
        if (!player.hasTokensInPlay()) {
            cout << "You rolled a 6. No tokens are in play, so you must enter a token into play.\n";
            player.enterTokenIntoPlay();
        } else {
            cout << "You rolled a 6. Choose an option:\n";
            cout << "1. Move a token into play.\n";
            cout << "2. Move a token 6 spaces.\n";
            int choice;
            cin >> choice;
            if (choice == 1) {
                player.enterTokenIntoPlay();
            } else {
                cout << "Choose a token to move 6 spaces (1-" << player.tokens.size() << "):\n";
                int tokenIndex;
                cin >> tokenIndex;
                player.moveToken(tokenIndex - 1, 6);
            }
        }
    } else {
        if (player.hasTokensInPlay()) {
            cout << "Choose a token to move " << diceRoll << " spaces (1-" << player.tokens.size() << "):\n";
            int tokenIndex;
            cin >> tokenIndex;
            player.moveToken(tokenIndex - 1, diceRoll);
        } else {
            cout << "No tokens are in play. You must roll a 6 to enter a token into play.\n";
        }
    }
}

int main() {
    srand(time(0));

    int numPlayers;

    while (true) {
        cout << "Please Select Player Count (2-4): ";
        cin >> numPlayers;
        cout << "\n";

        if (numPlayers >= 2 && numPlayers <= 4) {
            break;
        }
        cout << "Invalid player count. Try again.\n";
    }

    // choose who to roll the dice first
    int firstPlayer = chooseToStart(numPlayers);

    vector<Player> players(numPlayers);

    int currentPlayer = firstPlayer;
    while (true) {
        cout << "Player " << currentPlayer + 1 << "'s turn. Press Enter to roll the dice.";
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cin.get();

        playerTurn(players[currentPlayer]);
        displayBoard(players);

        if (players[currentPlayer].allTokensInHome()) {
            cout << "Player " << currentPlayer + 1 << " has won the game!\n";
            break;
        }

        currentPlayer = (currentPlayer + 1) % numPlayers;
    }

    return 0;
}
