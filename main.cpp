#include <iostream>
#include <cstdlib>
#include <ctime>
#include <vector>
#include <limits>

using namespace std;

const int HOME_POSITION = 100;
const int BOARD_SIZE = 52;

class Board {
public:
    static vector<int> getPathForPlayer(int playerIndex) {
        vector<int> path(BOARD_SIZE);
        for (int i = 0; i < BOARD_SIZE; i++) {
            path[i] = i; // Linear path
        }
        return path;
    }
};

class Token {
public:
    int position;
    bool inPlay;

    Token() {
        position = -1; // -1 indicates the token is not yet in play
        inPlay = false;
    }

    void move(int steps, const vector<int>& path) {
        if (position == -1) {
            return;
        }
        position += steps;
        if (position >= BOARD_SIZE) {
            position -= BOARD_SIZE;
        }
        position = path[position];
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
    int playerIndex;

    Player(int index, int tokenCount = 4) : playerIndex(index) {
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

    bool onlyOneTokenInPlay() const {
        int inPlayCount = 0;
        for (const auto& token : tokens) {
            if (token.inPlay) {
                inPlayCount++;
            }
        }
        return inPlayCount == 1;
    }

    void moveToken(int tokenIndex, int steps, const Board& board) {
        if (tokenIndex >= 0 && tokenIndex < tokens.size() && tokens[tokenIndex].inPlay) {
            tokens[tokenIndex].move(steps, board.getPathForPlayer(playerIndex));
        } else {
            cout << "Invalid token index or token is not in play.\n";
        }
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
        for (int i = 0; i < numPlayers; i++) {
            if (rolls[i] > highestRoll) {
                highestRoll = rolls[i];
                startingPlayer = i;
            }
        }
    }

    return startingPlayer;
}

void playerTurn(Player& player, const Board& board) {
    int diceRoll = rollDice();
    cout << "You rolled a " << diceRoll << endl;

    if (diceRoll == 6) {
        if (!player.hasTokensInPlay()) {
            cout << "You rolled a 6. No tokens are in play, so you must enter a token into play.\n";
            player.enterTokenIntoPlay();
        } else if (player.onlyOneTokenInPlay()) {
            cout << "You rolled a 6 and you only have one token in play. Moving the token 6 spaces.\n";
            for (int i = 0; i < player.tokens.size(); i++) {
                if (player.tokens[i].inPlay) {
                    player.moveToken(i, 6, board);
                    break;
                }
            }
        } else {
            cout << "You rolled a 6. Choose an option:\n";
            cout << "1. Move a token into play.\n";
            cout << "2. Move a token 6 spaces.\n";
            int choice;
            cin >> choice;
            if (choice == 1) {
                player.enterTokenIntoPlay();
            } else if (choice == 2) {
                cout << "Choose a token to move 6 spaces (1-" << player.tokens.size() << "):\n";
                int tokenIndex;
                cin >> tokenIndex;
                while (tokenIndex < 1 || tokenIndex > player.tokens.size() || !player.tokens[tokenIndex - 1].inPlay) {
                    cout << "Invalid choice. Choose a valid token to move 6 spaces (1-" << player.tokens.size() << "):\n";
                    cin >> tokenIndex;
                }
                player.moveToken(tokenIndex - 1, 6, board);
            } else {
                cout << "Invalid choice. Try again.\n";
            }
        }
    } else {
        if (player.hasTokensInPlay()) {
            if (player.onlyOneTokenInPlay()) {
                for (int i = 0; i < player.tokens.size(); i++) {
                    if (player.tokens[i].inPlay) {
                        cout << "Automatically moving the only token in play " << diceRoll << " spaces.\n";
                        player.moveToken(i, diceRoll, board);
                        break;
                    }
                }
            } else {
                cout << "Choose a token to move " << diceRoll << " spaces (1-" << player.tokens.size() << "):\n";
                int tokenIndex;
                cin >> tokenIndex;
                while (tokenIndex < 1 || tokenIndex > player.tokens.size() || !player.tokens[tokenIndex - 1].inPlay) {
                    cout << "Invalid choice. Choose a valid token to move " << diceRoll << " spaces (1-" << player.tokens.size() << "):\n";
                    cin >> tokenIndex;
                }
                player.moveToken(tokenIndex - 1, diceRoll, board);
            }
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

    int firstPlayer = chooseToStart(numPlayers);

    vector<Player> players;
    for (int i = 0; i < numPlayers; i++) {
        players.push_back(Player(i));
    }

    Board board;
    int currentPlayer = firstPlayer;
    while (true) {
        cout << "Player " << currentPlayer + 1 << "'s turn. Press Enter to roll the dice.";
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cin.get();

        playerTurn(players[currentPlayer], board);
        displayBoard(players);

        if (players[currentPlayer].allTokensInHome()) {
            cout << "Player " << currentPlayer + 1 << " has won the game!\n";
            break;
        }

        currentPlayer = (currentPlayer + 1) % numPlayers;
    }

    return 0;
}
