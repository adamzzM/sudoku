
#include "board.h"
#include <stdlib.h>
#include <time.h>
#include <iostream>
#include <vector>
#include <algorithm>

void zeroThatShit(int board[9][9]) {
    for (int i = 0; i < 9; i++) {
        for (int j = 0; j < 9; j++) {
            board[i][j] = 0;
        }
    }
}
void copyArr(int arr1[9][9], int arr2[9][9]) {
    for (int i = 0; i < 9; i++) {
        for (int j = 0; j < 9; j++) {
            arr2[i][j] = arr1[i][j];
        }
    }
}

board::board(int removeCount, std::string modeName) {
    hints_remaining = 3;
    difficultyMode = modeName;
    zeroThatShit(solution);
    fillBoard(solution);
    zeroThatShit(items);
    copyArr(solution, items);
    removeSome(items, removeCount);
}

int board::randomNumber(int min, int max) {
    return min + rand() % (max - min + 1);
}

bool board::fillRemaining(int items[9][9], int i, int j) {
    if (i == 8 && j == 9) return true;
    if (j == 9) { i++; j = 0; }
    if (items[i][j] != 0) return fillRemaining(items, i, j + 1);

    for (int num = 1; num <= 9; num++) {
        if (check(num, items, i, j)) {
            items[i][j] = num;
            if (fillRemaining(items, i, j + 1)) return true;
            items[i][j] = 0;
        }
    }
    return false;
}

bool board::selectCell(int row, int col) {
    if (row < 0 || row >= 9 || col < 0 || col >= 9) return false;
    selected_cell = std::make_pair(row, col);
    return true;
}

bool board::check(int num, int items[9][9], int row, int col) {
    // Standard Sudoku check (Row, Col, 3x3 Box)
    for (int x = 0; x <= 8; x++) {
        if (x != col) { // Skip self
            int val = items[row][x];
            // Normalize values before checking
            if (val % 11 == 0 && val != 0) val /= 11;
            else if (val % 13 == 0) val /= 13;
            else if (val % 17 == 0) val /= 17;
            if (val == num) return false;
        }
        if (x != row) { // Skip self
            int val = items[x][col];
            if (val % 11 == 0 && val != 0) val /= 11;
            else if (val % 13 == 0) val /= 13;
            else if (val % 17 == 0) val /= 17;
            if (val == num) return false;
        }
    }
    int sRow = row - row % 3, sCol = col - col % 3;
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            int currentR = i + sRow;
            int currentC = j + sCol;
            if (currentR == row && currentC == col) continue;

            int val = items[currentR][currentC];
            if (val % 11 == 0 && val != 0) val /= 11;
            else if (val % 13 == 0) val /= 13;
            else if (val % 17 == 0) val /= 17;

            if (val == num) return false;
        }
    }
    return true;
}

bool board::isValid(int row, int col, int num) {
    return check(num, items, row, col);
}

bool board::isImmutable(int row, int col) const {
    int val = items[row][col];
    // Fixed (%11) and Correct Hints (%17) cannot be changed.
    // Wrong Hints (%13) CAN be changed because the user entered them.
    if (val != 0 && (val % 11 == 0 || val % 17 == 0)) {
        return true;
    }
    return false;
}

bool board::placeNumInternal(std::optional<std::pair<int, int>> selected, int num) {
    if (!selected.has_value()) return false;
    int r = selected->first;
    int c = selected->second;

    // 1. Check Immutability
    if (isImmutable(r, c)) return false;

    // 2. Allow clearing (0)
    if (num == 0) {
        items[r][c] = 0;
        return true;
    }

    // 3. Check Validity (Row/Col/Box) - NOT Solution
    if (isValid(r, c, num)) {
        items[r][c] = num;
        return true;
    }

    return false;
}

bool board::placeNum(int num) {
    return placeNumInternal(selected_cell, num);
}

std::optional<std::pair<int, int>> board::getSelectedCell() const {
    return selected_cell;
}

int board::getCell(int row, int col) const {
    return items[row][col];
}

bool board::checkVictory() {
    for (int i = 0; i < 9; i++) {
        for (int j = 0; j < 9; j++) {
            int val = items[i][j];
            if (val % 11 == 0) val /= 11;
            else if (val % 13 == 0) val /= 13;
            else if (val % 17 == 0) val /= 17;

            if (val == 0 || !isValid(i,j,val)) {
                return false;
            }
        }
    }
    return true;
}

void board::fillDiagonal(int Board9x9[9][9]) {
    for (int k = 0; k < 9; k = k + 3) {
        for (int i = 0; i < 3; i++) {
            for (int j = 0; j < 3; j++) {
                int num;
                do {
                    num = randomNumber(1, 9);
                } while (!check(num, Board9x9, k + i, k + j));
                Board9x9[k + i][k + j] = num;
            }
        }
    }
}

void board::fillBoard(int items[9][9]) {
    fillDiagonal(items);
    fillRemaining(items, 0, 3);
}

void board::removeSome(int items[9][9], int count) {
    int counter = 0;
    while (counter < count) {
        int i = randomNumber(0, 8);
        int j = randomNumber(0, 8);
        if (items[i][j] != 0) {
            items[i][j] = 0;
            counter++;
        }
    }
    // mark fixed cells
    for (int i = 0; i < 9; i++) {
        for (int j = 0; j < 9; j++) {
            if (items[i][j] != 0) {
                items[i][j] = items[i][j] * 11;
            }
        }
    }
}

void board::markWrongHintInternal(int items[9][9], int solution[9][9]) {
    std::vector<std::pair<int, int>> wrongCells;
    for (int i = 0; i < 9; i++) {
        for (int j = 0; j < 9; j++) {
            // Find cells that are NOT locked but don't match solution
            int val = items[i][j];
            if (val != 0 && val % 11 != 0 && val % 17 != 0) {
                // Clean value to check against solution
                int cleanVal = (val % 13 == 0) ? val / 13 : val;
                if (cleanVal != solution[i][j]) {
                    wrongCells.push_back({ i, j });
                }
            }
        }
    }

    int countToMark = std::min(3, static_cast<int>(wrongCells.size()));
    for (int k = 0; k < countToMark; k++) {
        int idx = randomNumber(0, static_cast<int>(wrongCells.size()) - 1);
        auto [i, j] = wrongCells[idx];

        // Only multiply if not already marked
        if (items[i][j] % 13 != 0) items[i][j] = items[i][j] * 13;

        wrongCells[idx] = wrongCells.back();
        wrongCells.pop_back();
    }
}

bool board::markWrongHint() {
    if (hints_remaining <= 0) return false;
    hints_remaining--;
    markWrongHintInternal(items, solution);
    return true;
}

bool board::getHintRight() {
    if (hints_remaining <= 0) return false;

    std::vector<std::pair<int, int>> emptyCells;
    for (int i = 0; i < 9; i++) {
        for (int j = 0; j < 9; j++) {
            if (items[i][j] == 0) emptyCells.push_back({ i,j });
        }
    }

    if (emptyCells.empty()) return false;
    hints_remaining--;
    int countToReveal = std::min(3, static_cast<int>(emptyCells.size()));

    for (int k = 0; k < countToReveal; k++) {
        int idx = randomNumber(0, static_cast<int>(emptyCells.size()) - 1);
        int row = emptyCells[idx].first;
        int col = emptyCells[idx].second;
        items[row][col] = solution[row][col] * 17; // Mark immutable
        emptyCells[idx] = emptyCells.back();
        emptyCells.pop_back();
    }
    return true;
}

int board::getHints() const { return hints_remaining; }
std::string board::getDifficulty() const { return difficultyMode; }

bool board::saveToFile(const std::string& filename, const std::string& playerName) {
    std::ofstream file(filename);
    if (!file.is_open()) return false;
    file << playerName << "\n" << difficultyMode << "\n" << hints_remaining << "\n";
    for (int i = 0; i < 9; i++) { for (int j = 0; j < 9; j++) file << items[i][j] << " "; file << "\n"; }
    for (int i = 0; i < 9; i++) { for (int j = 0; j < 9; j++) file << solution[i][j] << " "; file << "\n"; }
    return true;
}

bool board::loadFromFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) return false;
    std::string dummyName;
    std::getline(file, dummyName);
    file >> difficultyMode >> hints_remaining;
    for (int i = 0; i < 9; i++) for (int j = 0; j < 9; j++) file >> items[i][j];
    for (int i = 0; i < 9; i++) for (int j = 0; j < 9; j++) file >> solution[i][j];
    return true;
}
