#ifndef BOARD_H
#define BOARD_H

#include <optional>
#include <utility>
#include <string>
#include <fstream>

class board {
private:
    int items[9][9];
    int solution[9][9];
    int hints_remaining;
    std::string difficultyMode;
    std::optional<std::pair<int, int>> selected_cell;

    int randomNumber(int min, int max);
    bool fillRemaining(int items[9][9], int i, int j);
    bool check(int num, int items[9][9], int row, int col);
    bool isValid(int row, int col, int num);
    bool placeNumInternal(std::optional<std::pair<int, int>> selected, int num);
    void fillDiagonal(int Board9x9[9][9]);
    void fillBoard(int items[9][9]);
    void removeSome(int items[9][9], int count);
    void markWrongHintInternal(int items[9][9], int solution[9][9]);

public:
    board(int removeCount = 40, std::string modeName = "easy");

    bool selectCell(int row, int col);
    std::optional<std::pair<int, int>> getSelectedCell() const;
    bool placeNum(int num);
    int getCell(int row, int col) const;

    // Checks if a cell is locked (Fixed or Hint)
    bool isImmutable(int row, int col) const;

    int getHints() const;
    std::string getDifficulty() const;

    bool markWrongHint();
    bool checkVictory();
    bool getHintRight();

    bool saveToFile(const std::string& filename, const std::string& playerName);
    bool loadFromFile(const std::string& filename);
};

#endif
