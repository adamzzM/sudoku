#include <iostream>
#include <thread>
#include <chrono>
#include <iomanip>
#include <cstdlib>
#include <fstream>
#include <string>
#include <ctime>
#include <cctype>
#include <limits>

using namespace std;
using namespace std::chrono;  // sleep for the swag
using namespace this_thread;

static int extratime = 0;
static int HintC = 3;
static bool game_status = true;

#define RESET "\x1b[0m"
#define RED "\x1b[31m"
#define GREEN "\x1b[32m"
#define YELLOW "\x1b[33m"
#define BLUE "\x1b[34m"
#define MAGENTA "\x1b[35m"
#define CYAN "\x1b[36m"
#define WHITE "\x1b[37m"
#define CLEAR "\x1b[2J\x1b[H"
#define AQUA "\x1b[96m"
#define ORANGE "\x1b[38;5;208m"

bool FillRemaining(int Board9x9[9][9], int, int);

int main()
{
    srand((unsigned)time(NULL)); // seed the random generator
    auto start = high_resolution_clock::now();

    int Board9x9Solved[9][9] = { 0 }, Board9x9UnSolved[9][9] = { 0 }, UserSolvedIndexes[9][9] = { 0 };
    int Row, Col, num;
    string Diff_Mode;
    string EnterNameMessage = "Please enter your name : ";
    string PlayAgainMessage =  "Do you want to play again ?";
    string PlayAgainMessage2 = "        (y / n)";
    char play_status;
    int rounds;
    string Name;

    do {
        cout << RESET;
        extratime = 0;
        rounds = 0;
        HintC = 3;
        game_status = true;
        Name = "";

        for (int i = 0; i < 9; i++) {
            for (int j = 0; j < 9; j++) {
                Board9x9Solved[i][j] = 0;
                Board9x9UnSolved[i][j] = 0;
                UserSolvedIndexes[i][j] = 0;
            }
        }
        for (char c : EnterNameMessage) {
            sleep_for(milliseconds(20));
            cout << c;
        }
        getline(cin, Name);
        cout << AQUA << "Loading: [";
        for (int i = 1; i <= 20; i++) {
            cout << "#";
            sleep_for(milliseconds(20)); // extra swag
        }
        cout << "] 100% !" << endl << endl;
        // chatgpt part start 

        cout << R"(     __          __  _  
)"; sleep_for(milliseconds(20));
        cout << R"(     \ \        / / | | 
)"; sleep_for(milliseconds(20));
        cout << R"(      \ \  /\  / /__| | ___ ___  _ __ ___   ___          
)"; sleep_for(milliseconds(20));
        cout << R"(       \ \/  \/ / _ \ |/ __/ _ \| '_ ` _ \ / _ \
)"; sleep_for(milliseconds(20));
        cout << R"(        \  /\  /  __/ | (_| (_) | | | | | |  __/
)"; sleep_for(milliseconds(20));
        cout << R"(         \/  \/ \___|_|\___\___/|_| |_| |_|\___|
)"; sleep_for(milliseconds(20));
        cout << R"(       _              ___                  
)"; sleep_for(milliseconds(20));
        cout << R"(      | |            / _ \                 
)"; sleep_for(milliseconds(20));
        cout << R"(      | |_ ___      | | | |_   _ _ __      
)"; sleep_for(milliseconds(20));
        cout << R"(      | __/ _ \     | | | | | | | '__|     
)"; sleep_for(milliseconds(20));
        cout << R"(      | || (_) |    | |_| | |_| | |        
)"; sleep_for(milliseconds(20));
        cout << R"(       \__\___/      \___/ \__,_|_|                                                    
)"; sleep_for(milliseconds(20));
        cout << R"(        _____           _       _          
)"; sleep_for(milliseconds(20));
        cout << R"(       / ____|         | |     | |         
)"; sleep_for(milliseconds(20));
        cout << R"(      | (___  _   _  __| | ___ | | ___   _ 
)"; sleep_for(milliseconds(20));
        cout << R"(       \___ \| | | |/ _` |/ _ \| |/ / | | |
)"; sleep_for(milliseconds(20));
        cout << R"(       ____) | |_| | (_| | (_) |   <| |_| |
)"; sleep_for(milliseconds(20));
        cout << R"(      |_____/ \__,_|\__,_|\___/|_|\_\\__,_|
)" << RESET << endl; sleep_for(milliseconds(20));

        // chatGPT part end 

        // generate solved board
        // first we start by filling the diagonal boxes because they're easier to compute
        for (int k = 0; k < 9; k = k + 3) {
            for (int i = 0; i < 3; i++) {
                for (int j = 0; j < 3; j++) {
                    int num;
                    bool condChe = true;
                    do {
                        condChe = true;
                        int row = k + i;
                        int col = k + j;
                        num = rand() % (9 - 1 + 1) + 1;
                        for (int x = 0; x <= 8; x++) {
                            if (Board9x9Solved[row][x] == num) { condChe = false; break; }
                            if (Board9x9Solved[x][col] == num) { condChe = false; break; }
                        }
                        int sRow = row - row % 3, sCol = col - col % 3;
                        for (int z = 0; z < 3 && condChe; z++) for (int y = 0; y < 3; y++) if (Board9x9Solved[z + sRow][y + sCol] == num) { condChe = false; break; };

                    } while (!condChe);
                    Board9x9Solved[k + i][k + j] = num;
                }
            }
        }
        // then we fill the remaining cells using a backtracking algorithm
        FillRemaining(Board9x9Solved, 0, 3);

        // copy the solved board to the unsolved board

        for (int i = 0; i <= 8; i++) {
            for (int j = 0; j <= 8; j++) {
                Board9x9UnSolved[i][j] = Board9x9Solved[i][j];
            }
        }

        // how many do we remove? 

        int count, ans;
        do {
            cout << AQUA << "===============================\n";
            sleep_for(milliseconds(50));
            cout << GREEN << "[1] Easy Mode\n";
            sleep_for(milliseconds(50));
            cout << YELLOW << "[2] Medium Mode\n";
            sleep_for(milliseconds(50));
            cout << ORANGE << "[3] Hard Mode\n";
            sleep_for(milliseconds(50));
            cout << RED << "[4] Impossible Mode\n";
            sleep_for(milliseconds(50));
            cout << MAGENTA << "[5] 67 Mode ~ meme\n";
            sleep_for(milliseconds(50));

            cout << AQUA << "===============================\n" << RESET;
            sleep_for(milliseconds(50));
            cout << "Choose Game Mode Difficulty: ";
            // TO validate the input :
            while (!(cin >> ans)) {
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                cout << RED << "Enter a valid number ( 1-5 ): " << RESET;
            }
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            switch (ans) {
            case 1:
                count = 40;
                Diff_Mode = "easy";
                break;
            case 2:
                count = 50;
                Diff_Mode = "medium";
                break;
            case 3:
                count = 60;
                Diff_Mode = "hard";
                break;
            case 4:
                count = 70;
                Diff_Mode = "impossible";
                break;
            case 5:
                count = 67;
                Diff_Mode = "67";
                break;

            default: cout << RED << "\nEnter A Number Between 1 and 5 ...\n" << RESET;
            }
        } while (ans > 5 || ans < 1);

        cout << endl << endl;
        // remove some of the digits to create the puzzle
        int countRemoved = 0;

        // testing purposes
        // count = 4;

        while (countRemoved < count) {
            int i = rand() % (9);
            int j = rand() % (9);
            if (Board9x9UnSolved[i][j] != 0) {
                Board9x9UnSolved[i][j] = 0;
                countRemoved++;
            }
        }
        // main game loop
        start = high_resolution_clock::now(); // begin timer
        while (game_status) {
            while (true) {
                // draw board
                if (Diff_Mode == "easy")
                    cout << GREEN ;
                if (Diff_Mode == "medium")
                    cout << YELLOW;
                if (Diff_Mode == "hard")
                    cout << ORANGE;
                if (Diff_Mode == "impossible")
                    cout << RED;
                if (Diff_Mode == "67")
                    cout << MAGENTA;
                cout << "|1 2 3 |4 5 6 |7 8 9 | ";
                cout << "         Round: " << rounds << endl;

                auto end = high_resolution_clock::now();
                end += seconds(extratime);
                auto duration = duration_cast<seconds>(end - start);
                long long totalSeconds = duration.count();
                long long minutes = totalSeconds / 60;
                long long seconds2 = totalSeconds % 60;

                cout << "______________________";
                cout << "          Time: [" << minutes << ":";
                    if (seconds2 < 10)
                        cout << 0 << seconds2  << "]" << endl << RESET;
                    else
                        cout << seconds2 << "]" << endl << RESET;
                for (int i = 0; i <= 8; i++) {
                    cout << "|";
                    for (int j = 0; j <= 8; j++) {
                        if (Board9x9UnSolved[i][j] != 0 && Board9x9UnSolved[i][j] % 11 == 0) {
                            cout << CYAN << Board9x9UnSolved[i][j] / 11 << " " << RESET; // Refers to the user's assured values
                        }
                        else if (Board9x9UnSolved[i][j] == UserSolvedIndexes[i][j] && Board9x9UnSolved[i][j] != 0) {
                            cout << GREEN << Board9x9UnSolved[i][j] << " " << RESET; // Prints the hint's inputs
                        }
                        else if (Board9x9UnSolved[i][j] % 10 == 0 && Board9x9UnSolved[i][j] != 0) {
                            cout << RED << Board9x9UnSolved[i][j] / 10 << " " << RESET; // flagged as wrong
                        }
                        else {
                            if (Board9x9UnSolved[i][j] == 0)
                                cout << ". ";
                            else {
                                cout << Board9x9UnSolved[i][j] << " ";
                            }
                        }
                        if ((j + 1) % 3 == 0) cout << "|";
                    }
                    sleep_for(milliseconds(40));
                    if (Diff_Mode == "easy")
                        cout << GREEN << " " << i + 1 << RESET;
                    if (Diff_Mode == "medium")
                        cout << YELLOW << " " << i + 1 << RESET;
                    if (Diff_Mode == "hard")
                        cout << ORANGE << " " << i + 1 << RESET;
                    if (Diff_Mode == "impossible")
                        cout << RED << " " << i + 1 << RESET;
                    if (Diff_Mode == "67")
                        cout << MAGENTA << " " << i + 1 << RESET;

                    cout << endl;
                    if ((i + 1) % 3 == 0 && i != 8)
                        cout << "-------+------+-------" << endl;
                    if (i == 8)
                        cout << "______________________" << endl;
                    sleep_for(milliseconds(40));
                }


                bool gameEnded = true;
                // Checks if the game has alr ended

                for (int i = 0; i < 9; i++) {
                    for (int j = 0; j < 9; j++) {
                        if (Board9x9UnSolved[i][j] < 10) {
                            if (Board9x9UnSolved[i][j] != Board9x9Solved[i][j]) {
                                gameEnded = false;
                                break;
                            }
                        }
                        else {
                            if (Board9x9UnSolved[i][j] % 11 == 0) {
                                if (Board9x9UnSolved[i][j] / 11 != Board9x9Solved[i][j]) {
                                    gameEnded = false;
                                    break;
                                }
                            }
                            else if (Board9x9UnSolved[i][j] % 10 == 0) {
                                if (Board9x9UnSolved[i][j] / 10 != Board9x9Solved[i][j]) {
                                    gameEnded = false;
                                    break;
                                }
                            }
                        }
                    }if (!gameEnded) break;
                }
                if (gameEnded) { break; }


                char choice;
                cout << YELLOW << "\n[Commands]: 'h (hints)' (" << HintC << " left) | 's' (sure) | 'q' (quit) | 'p' (play)\n";
                cout << "Enter command: " << RESET;
                
                // TO validate the input :
                while (!(cin >> choice) or (tolower(choice) != 'h' && tolower(choice) != 's' && tolower(choice) != 'q' && tolower(choice) != 'p')) {
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                    cout << RED << "Invalid Command! Enter (h, s, q, or p): " << RESET;
                }

                if (choice == 'h') {
                    if (HintC > 0) {
                        int r;
                        if (HintC > 0) {
                            sleep_for(milliseconds(40));
                            cout << YELLOW << "\n======================================\nHint List:\n";
                            sleep_for(milliseconds(40));
                            cout << "[1] Highlights up to 3 of your wrong inputs ( +30 seconds ).\n";
                            sleep_for(milliseconds(40));
                            cout << "[2] Answer 3 cells for you ( +60 seconds ).\n";
                            sleep_for(milliseconds(40));
                            cout << "======================================\nChoose [1 or 2]: " << RESET;
                            sleep_for(milliseconds(40));
                            // TO validate the input :

                            while (!(cin >> r) || r < 1 || r > 2) {
                                cin.clear();
                                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                                cout << RED << "Invalid Input! Please enter a valid number (1 or 2).\n" << RESET;
                            }
                            switch (r) {
                            case 1:
                                extratime += 30;
                                {
                                    int countH1 = 3;
                                    cout << YELLOW << "The Cells Colored In Red are incorrect." << endl << RESET;
                                    // for the hint system iterate over the whole board and checks if there is any incorrect value --->
                                    // -> and multiplies it by 10 to mark it with a unique value so we can print it in a different color
                                    for (int i = 0; i < 9; i++)
                                        for (int j = 0; j < 9; j++)
                                            if (countH1 > 0) {
                                                if (Board9x9UnSolved[i][j] != Board9x9Solved[i][j] && Board9x9UnSolved[i][j] != 0 && Board9x9UnSolved[i][j] < 10) {
                                                    Board9x9UnSolved[i][j] = Board9x9UnSolved[i][j] * 10; // mark as wrong
                                                    Board9x9UnSolved[i][j] = Board9x9UnSolved[i][j] % 11 == 0 ? Board9x9UnSolved[i][j] / 11 : Board9x9UnSolved[i][j]; // overwrite sure 
                                                    countH1--;
                                                }
                                                else if (Board9x9UnSolved[i][j] / 11 != Board9x9Solved[i][j] && Board9x9UnSolved[i][j] != 0 && Board9x9UnSolved[i][j] % 11 == 0) {
                                                    Board9x9UnSolved[i][j] = Board9x9UnSolved[i][j] * 10; // mark as wrong
                                                    Board9x9UnSolved[i][j] = Board9x9UnSolved[i][j] % 11 == 0 ? Board9x9UnSolved[i][j] / 11 : Board9x9UnSolved[i][j]; // overwrite sure 
                                                    countH1--;
                                                }
                                            }
                                }
                                break;
                            case 2:
                                extratime += 60;
                                {
                                    int countH2 = 3;
                                    int m, l;
                                    int attempts = 0;
                                    while (countH2 > 0 && attempts < 1000) {
                                        m = rand() % (9);
                                        l = rand() % (9);
                                        // for the hint system iterate over the entire row and the entire column and remove any duplicates of our special number
                                        // then place the correct number in the cell
                                        if ((Board9x9UnSolved[m][l] != Board9x9Solved[m][l] && Board9x9UnSolved[m][l] < 10)
                                            or(Board9x9UnSolved[m][l] / 10 != Board9x9Solved[m][l] && Board9x9UnSolved[m][l] % 10 == 0)
                                            or (Board9x9UnSolved[m][l] / 11 != Board9x9Solved[m][l] && Board9x9UnSolved[m][l] % 11 == 0)) {
                                            int correctNum = Board9x9Solved[m][l];
                                            for (int x = 0; x < 9; x++) {
                                                if (Board9x9Solved[m][x] == correctNum) {
                                                    Board9x9UnSolved[m][x] = 0;
                                                    UserSolvedIndexes[m][x] = 0;
                                                }
                                                if (Board9x9Solved[x][l] == correctNum) {
                                                    Board9x9UnSolved[x][l] = 0;
                                                    UserSolvedIndexes[x][l] = 0;
                                                }
                                            }
                                            Board9x9UnSolved[m][l] = correctNum;
                                            UserSolvedIndexes[m][l] = correctNum;
                                            countH2--;
                                        }
                                        attempts++;
                                    }
                                }
                                break;
                            }
                            HintC--; rounds++;
                        }
                    }
                    else {
                        cout << RED << "No hints left!\n" << RESET;
                        cout << RED << "try harder TIME OUT\n" << RESET;
                        sleep_for(milliseconds(2000)); // punish the user 
                    }
                    cout << CLEAR;
                }

                else if (choice == 's') {
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                    int r, c;
                    cout << "Enter Row and Column to mark as Sure (e.g. 1 1): ";
                    // TO validate the input :

                    while (!(cin >> r >> c) || (r < 1 || r > 9 || c < 1 || c > 9)) {
                        if (r < 1 || r > 9 || c < 1 || c > 9) {
                            cout << RED << "Enter a number between 1 and 9: " << RESET;
                        }
                        else {
                            cin.clear();
                            cin.ignore(numeric_limits<streamsize>::max(), '\n');
                            cout << RED << "Enter a (NUMBER): " << RESET;
                        }
                    }
                    r--; c--;

                    if (Board9x9UnSolved[r][c] != 0) {
                        if (Board9x9UnSolved[r][c] % 11 == 0) {
                            cout << RED << "You already assigned this value as sure!\n" << RESET;
                        }
                        else if (UserSolvedIndexes[r][c] != Board9x9Solved[r][c]) {
                            cout << RED << "This value has been entered by the computer you cannot mark it as sure!\n" << RESET;
                        }
                        else {
                            Board9x9UnSolved[r][c] = Board9x9UnSolved[r][c] * 11;
                            cout << CYAN << "Cell marked as Sure (Blue)!\n\n" << RESET;
                        }
                    }
                    else {
                        cout << RED << "You haven't solved this cell yet, so you can't be sure about it!\n" << RESET;
                    }
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                }
                else if (choice == 'q') {
                    game_status = false;
                    cout << GREEN << R"(   _____                 _ _                _  
)"; sleep_for(milliseconds(20));
                    cout << R"(  / ____|               | | |              | | 
)"; sleep_for(milliseconds(20));
                    cout << R"( | |  __  ___   ___   __| | |__  _   _ ___ | | 
)"; sleep_for(milliseconds(20));
                    cout << R"( | | |_ |/ _ \ / _ \ / _` | '_ \| | | / _ \  | 
)"; sleep_for(milliseconds(20));
                    cout << R"( | |__| | (_) | (_) | (_| | |_) | |_| | __/ _| 
)"; sleep_for(milliseconds(20));
                    cout << R"(  \_____|\___/ \___/ \__,_|_.__/ \__, |\___(_) 
)"; sleep_for(milliseconds(20));
                    cout << R"(                                  __/ |        
)"; sleep_for(milliseconds(20));
                    cout << R"(                                 |___/         
)" << RESET << endl; sleep_for(milliseconds(20))
;
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');

                    break;
                }
                else if (choice == 'p') {
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                    break;
                }
                else {
                    cout << CLEAR;
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                    cout << RED << "Invalid Command! Please try again.\n" << RESET;
                }

            }
            if (!game_status) break;
            bool gameEnded = true;
            for (int i = 0; i < 9; i++) {
                for (int j = 0; j < 9; j++) {
                    if (Board9x9Solved[i][j] < 10) {
                        if (Board9x9Solved[i][j] != Board9x9UnSolved[i][j]) {
                            gameEnded = false;
                            break;
                        }
                    }
                    else {
                        if (Board9x9UnSolved[i][j] % 11 == 0) {
                            if (Board9x9Solved[i][j] != Board9x9Solved[i][j] / 11) {
                                gameEnded = false;
                                break;
                            }
                        }
                        else if (Board9x9UnSolved[i][j] % 10 == 0) {
                            if (Board9x9Solved[i][j] != Board9x9Solved[i][j] / 10) {
                                gameEnded = false;
                                break;
                            }
                        }
                    }
                }if (!gameEnded) { break; }
            }
            if (gameEnded) {
                break;
            }

            // take row , column and num from user and make sure they're numbers then validate
            cout << "Enter the Row: ";
            while (!(cin >> Row) || Row < 1 || Row > 9) {
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                cout << RED << "Enter a valid number between 1 and 9: " << RESET;
            }
            cin.ignore(numeric_limits<streamsize>::max(), '\n');

            cout << "Enter the Column: ";
            while (!(cin >> Col) || Col < 1 || Col > 9) {
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                cout << RED << "Enter a valid number between 1 and 9: " << RESET;
            }
            cin.ignore(numeric_limits<streamsize>::max(), '\n');


            cout << "Enter the Number: ";
            while (!(cin >> num) or (num > 9) or (num < 1)) {
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                cout << RED << "Enter a valid number between 1 and 9: " << RESET;
            }
            cin.ignore(numeric_limits<streamsize>::max(), '\n');

            Row--;
            Col--;

            // Checks if the input is valid or not ( does not mean it has to be the correct value in this cell ).

            if (!Board9x9UnSolved[Row][Col] or UserSolvedIndexes[Row][Col]) {
                cout << CLEAR;
                bool valid = true;
                for (int x = 0; x <= 8; x++) {
                    if (Board9x9UnSolved[Row][x] == num) valid = false;
                    if (Board9x9UnSolved[Row][x] % 11 == 0) if (Board9x9UnSolved[Row][x] / 11 == num) valid = false;

                    if (Board9x9UnSolved[x][Col] == num) valid = false;
                    if (Board9x9UnSolved[x][Col] % 11 == 0) if (Board9x9UnSolved[x][Col] / 11 == num) valid = false;
                }
                int sRow = Row - Row % 3, sCol = Col - Col % 3;
                for (int i = 0; i < 3; i++)
                    for (int j = 0; j < 3; j++)
                        if (Board9x9UnSolved[i + sRow][j + sCol] < 10) {
                            if (Board9x9UnSolved[i + sRow][j + sCol] == num) valid = false;
                        }
                        else if (Board9x9UnSolved[i + sRow][j + sCol] % 11 == 0) {
                            if (Board9x9UnSolved[i + sRow][j + sCol] / 11 == num) valid = false;
                        }
                if ((Board9x9UnSolved[Row][Col] == num && Board9x9UnSolved[Row][Col] < 10) or
                    (Board9x9UnSolved[Row][Col] / 11 == num and Board9x9UnSolved[Row][Col] % 11 == 0)) {
                    cout << RED << "You've already placed that number in this cell..\n";
                }
                else if (valid) {
                    cout << GREEN << "Valid Input!" << endl;
                    Board9x9UnSolved[Row][Col] = num;
                    UserSolvedIndexes[Row][Col] = num;
                }
                else
                    cout << endl << RED << "Invalid, Try again.." << RESET << endl;
            }
            else {
                cout << RED << "Invalid Input, There was already a number in this cell!" << endl << RESET;

            }
            rounds++;
        }

        if (!game_status) cout << "You Exited the Game." << endl;
        else {  cout << GREEN << "You Completed the game, Congratulations!" << RESET << endl; }

        // End timer , print final results board and saves it into a file

        auto end = high_resolution_clock::now();
        end += seconds(extratime);
        auto duration = duration_cast<seconds>(end - start);
        long long totalSeconds = duration.count();
        long long minutes = totalSeconds / 60;
        long long seconds2 = totalSeconds % 60;
        sleep_for(milliseconds(40));
        cout << "Name: " << Name << "\nTime taken: [" << minutes << ":";
        if (seconds2 < 10)
            cout << 0 << seconds2 << "]" << endl;
        else
            cout << seconds2 << "]" << endl;
        sleep_for(milliseconds(40));
        cout << "Hints Used: " << 3 - HintC << endl;
        sleep_for(milliseconds(40));
        cout << "Total Rounds: " << rounds << endl;
        sleep_for(milliseconds(40));

        cout << "Solution Board: " << endl;
        sleep_for(milliseconds(40));
        cout << "______________________" << endl;
        for (int i = 0; i <= 8; i++) {
            cout << "|";
            for (int j = 0; j <= 8; j++) {
                cout << GREEN << Board9x9Solved[i][j] << RESET << " ";
                if ((j + 1) % 3 == 0) cout << "|";
            }
            sleep_for(milliseconds(40));
            cout << endl;
            if ((i + 1) % 3 == 0 && i != 8)
                cout << "-------+------+-------" << endl;
            if (i == 8)
                cout << "______________________" << endl;
            sleep_for(milliseconds(40));
        }
        cout << endl << endl << AQUA;
        for (char c : PlayAgainMessage) {
            sleep_for(milliseconds(40));
            cout << c;
        } cout << endl;
        for (char c : PlayAgainMessage2) {
            sleep_for(milliseconds(40));
            cout << c;
        } cout << endl;
        cout << RESET;
        cin >> play_status;
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        while (tolower(play_status) != 'y' and tolower(play_status) != 'n') {
            cout << RED << "You can only enter ( y / n ) : " << RESET;
            cin >> play_status;
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }

        fstream MyFile;
        MyFile.open("PlayersLog.txt", ios::app);        // LOG
        if (MyFile.is_open())
        {
            MyFile << "================================================\n";
            MyFile << "Name: " << Name << "\nTime Taken: [" << minutes << ":";
            if (seconds2 < 10)
                MyFile << 0 << seconds2 << "]" << endl;
            else
                MyFile << seconds2 << "]" << endl;
            MyFile << "Hints Used: " << 3 - HintC << endl;
            MyFile << "Difficulty Mode: " << Diff_Mode << endl;
            MyFile << "Rounds: " << rounds << endl;
            MyFile << "Solution Board: " << endl;
            MyFile << "______________________" << endl;
            for (int i = 0; i <= 8; i++) {
                MyFile << "|";
                for (int j = 0; j <= 8; j++) {
                    MyFile << Board9x9Solved[i][j] << " ";
                    if ((j + 1) % 3 == 0) MyFile << "|";
                }
                MyFile << endl;
                if ((i + 1) % 3 == 0 && i != 8)
                    MyFile << "-------+------+-------" << endl;
                if (i == 8)
                    MyFile << "______________________" << endl;
            }
            MyFile << "Player's Board: " << endl;
            MyFile << "______________________" << endl;
            for (int i = 0; i <= 8; i++) {
                MyFile << "|";
                for (int j = 0; j <= 8; j++) {
                    if (Board9x9UnSolved[i][j] % 11 == 0) Board9x9UnSolved[i][j] /= 11;
                    if (Board9x9UnSolved[i][j] % 10 == 0) Board9x9UnSolved[i][j] /= 10;
                    MyFile << Board9x9UnSolved[i][j] << " ";
                    if ((j + 1) % 3 == 0) MyFile << "|";
                }
                MyFile << endl;
                if ((i + 1) % 3 == 0 && i != 8)
                    MyFile << "-------+------+-------" << endl;
                if (i == 8)
                    MyFile << "______________________" << endl;
            }
            MyFile << "================================================\n\n";
            MyFile.close();
        }

    } while (tolower(play_status) == 'y');
    return 0;
}

bool FillRemaining(int Board9x9[9][9], int i, int j) {
    if (i == 8 && j == 9) return true;
    if (j == 9) { i++; j = 0; }
    if (Board9x9[i][j] != 0) return FillRemaining(Board9x9, i, j + 1);

    for (int num = 1; num <= 9; num++) {


        bool check = true;
        int row = i;
        int col = j;
        for (int x = 0; x <= 8; x++) {
            if (Board9x9[row][x] == num) { check = false; break; };
            if (Board9x9[x][col] == num) { check = false; break; }
        }
        int sRow = row - row % 3, sCol = col - col % 3;
        for (int i = 0; i < 3 && check; i++) for (int j = 0; j < 3; j++) if (Board9x9[i + sRow][j + sCol] == num) { check = false; break; }

        if (check) {
            Board9x9[i][j] = num;
            if (FillRemaining(Board9x9, i, j + 1)) return true;
            Board9x9[i][j] = 0;
        }
    }
    return false;
}

