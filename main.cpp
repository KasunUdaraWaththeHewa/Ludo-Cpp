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
    int maxChances = 3; // Maximum number of chances per turn
    int chances = 0; // Counter for the number of chances used

    while (chances < maxChances) {
        int diceRoll = rollDice();
        cout << "You rolled a " << diceRoll << endl;

        if (diceRoll == 6) {
            if (!player.hasTokensInPlay()) {
                // If no tokens are in play, enter a new token into play
                cout << "You rolled a 6. No tokens are in play, so you must enter a token into play.\n";
                player.enterTokenIntoPlay();
            } else {
                // Check if there are tokens not in play
                bool canEnterNewToken = false;
                for (const auto& token : player.tokens) {
                    if (!token.inPlay) {
                        canEnterNewToken = true;
                        break;
                    }
                }

                // If there are tokens not in play, give the player a choice
                if (canEnterNewToken) {
                    int choice = 0;
                    while (choice != 1 && choice != 2) {
                        cout << "You rolled a 6. Choose an option:\n";
                        cout << "1. Move a token 6 spaces.\n";
                        cout << "2. Enter a new token into play.\n";
                        cin >> choice;
                        if (choice != 1 && choice != 2) {
                            cout << "Invalid choice. Try again.\n";
                        }
                    }
                    if (choice == 1) {
                        // Prompt the user to choose a token to move 6 spaces
                        cout << "Choose a token to move 6 spaces (1-" << player.tokens.size() << "):\n";
                        int tokenIndex;
                        cin >> tokenIndex;
                        while (tokenIndex < 1 || tokenIndex > player.tokens.size() || !player.tokens[tokenIndex - 1].inPlay) {
                            cout << "Invalid choice. Choose a valid token to move 6 spaces (1-" << player.tokens.size() << "):\n";
                            cin >> tokenIndex;
                        }
                        player.moveToken(tokenIndex - 1, 6, board);
                    } else if (choice == 2) {
                        player.enterTokenIntoPlay();
                    }
                } else {
                    // If no tokens are left to be entered, automatically move a token
                    cout << "All tokens are in play. Automatically moving a token 6 spaces.\n";
                    for (int i = 0; i < player.tokens.size(); i++) {
                        if (player.tokens[i].inPlay && !player.tokens[i].hasWon()) {
                            player.moveToken(i, 6, board);
                            break;
                        }
                    }
                }
            }
            chances++; // Increase the chance count for each 6 rolled
            if (chances >= maxChances) {
                cout << "You have used all your chances for this turn.\n";
                break;
            }
        } else {
            // If it's not a 6, just move the token as usual
            if (player.hasTokensInPlay()) {
                if (player.onlyOneTokenInPlay()) {
                    // Automatically move the only token in play if it's not a 6
                    for (int i = 0; i < player.tokens.size(); i++) {
                        if (player.tokens[i].inPlay) {
                            cout << "Automatically moving the only token in play " << diceRoll << " spaces.\n";
                            player.moveToken(i, diceRoll, board);
                            break;
                        }
                    }
                } else {
                    // Prompt the user to choose a token to move
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
            break; // Exit the turn after rolling anything other than 6
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
        std::cout << "Player " << currentPlayer + 1 << "'s turn. Press Enter to roll the dice." << endl;
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