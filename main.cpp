#include <iostream>
#include <cstdlib>
#include <ctime>

using namespace std;

int main() {
    srand(time(0));

    const int numPlayers = 4;

    for (int i=1; i<= numPlayers; ++i){
        cout << "Player " << i << ", press Enter to make the first move ...";
        cin.ignore();

        int diceRoll = (rand() % 6) + 1;
        cout << "Player " << i << " rolled: " << diceRoll << endl;

    }

    for (int i = 1; i <= numPlayers; ++i) {
        cout << "Player " << i << ", press Enter to roll the dice...";
        cin.ignore();

        int diceRoll = (rand() % 6) + 1;
        cout << "Player " << i << " rolled: " << diceRoll << endl;
    }

    return 0;
}
