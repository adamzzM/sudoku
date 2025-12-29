
#define _CRT_SECURE_NO_WARNINGS // warnings are for losers
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/System.hpp>
#include <SFML/Audio.hpp> 
#include <iostream>
#include <vector>
#include <ctime>
#include <cstdlib>
#include <string>
#include <fstream>
#include <cmath> 
#include "board.h"
#include "popup.h"

const int GRID_SIZE = 9;
const int CELL_SIZE = 60;
const int WINDOW_SIZE = GRID_SIZE * CELL_SIZE + 150;

enum class GameState {
    WELCOME,
    LOGIN,
    MENU,
    MODE_SELECT,
    PLAYING
};

class sudokuWindowUI {
private:
    board sudoku;
    sf::RenderWindow window;
    sf::Font font;
    PopUp popup;
    sf::Clock gameClock;
    sf::Time additionalTime;

    // Audio
    sf::SoundBuffer errorBuffer, clickBuffer, successBuffer;
    sf::Sound errorSound, clickSound, successSound;
    bool soundLoaded, clickLoaded, successLoaded;

    bool fontLoaded;
    GameState currentState;
    std::string playerName;
    std::string userInputName;

    int selectedRemoveCount = 40;
    std::string selectedDiffName = "easy";

    sf::Clock animClock; // animation

public:
    sudokuWindowUI() : window(sf::VideoMode({ static_cast<unsigned int>(WINDOW_SIZE), static_cast<unsigned int>(WINDOW_SIZE) }), "F5M Sudoku"),
        sudoku(40, "easy"),
        fontLoaded(false),
        soundLoaded(false), clickLoaded(false), successLoaded(false),
        errorSound(errorBuffer), clickSound(clickBuffer), successSound(successBuffer),
        currentState(GameState::WELCOME)
    {
        window.setFramerateLimit(60);
        srand(static_cast<unsigned>(time(NULL)));

        // Font loading
        if (font.openFromFile("MartianMonoNerdFontPropo-Bold.ttf")) fontLoaded = true;
        else if (font.openFromFile("MartianMonoNerdFontPropo-Bold.ttf")) fontLoaded = true;

        //  audio attempt "sometimes works"
        if (errorBuffer.loadFromFile("error.wav")) { soundLoaded = true; }
        if (clickBuffer.loadFromFile("click.wav")) { clickLoaded = true; }
        if (successBuffer.loadFromFile("success.wav")) { successLoaded = true; }

        if (!fontLoaded) std::cerr << "Failed to load font!" << std::endl;
    }

    void run() {
        while (window.isOpen()) {
            handleEvents();
            render();
        }
    }

private:
    void playSound(const std::string& type) {
        if (type == "error" && soundLoaded) errorSound.play();
        else if (type == "click" && clickLoaded) clickSound.play();
        else if (type == "success" && successLoaded) successSound.play();
    }

    void archiveLastGame() {
        std::ifstream inFile("last_game.txt");
        if (!inFile.is_open()) return;
        std::string pName, pMode, pHints;
        std::getline(inFile, pName);
        inFile >> pMode >> pHints;
        std::ofstream logFile("player_log.txt", std::ios::app);
        if (logFile.is_open()) {
            time_t now = time(0);
            char* dt = ctime(&now);
            std::string timeStr = dt ? dt : "Unknown";

            if (!timeStr.empty() && timeStr.back() == '\n') timeStr.pop_back();
            logFile << "Date: " << timeStr << " | Player: " << pName
                << " | Mode: " << pMode << " | Status: Abandoned/Overwritten\n";
        }
    }

    void handleEvents() {
        while (const std::optional event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                if (currentState == GameState::PLAYING) {
                    sudoku.saveToFile("last_game.txt", playerName);
                }
                window.close();
            }

            // Welcome Screen Event
            if (currentState == GameState::WELCOME) {
                if (event->is<sf::Event::KeyPressed>() || event->is<sf::Event::MouseButtonPressed>()) {
                    playSound("click");
                    currentState = GameState::LOGIN;
                }
            }
            // Login Typing
            else if (currentState == GameState::LOGIN) {
                if (const auto* textEvent = event->getIf<sf::Event::TextEntered>()) {
                    playSound("click");
                    if (textEvent->unicode == 8) {
                        if (!userInputName.empty()) userInputName.pop_back();
                    }
                    else if (textEvent->unicode == 13) {
                        if (!userInputName.empty()) {
                            playerName = userInputName;
                            currentState = GameState::MENU;
                        }
                    }
                    else if (textEvent->unicode < 128) {
                        userInputName += static_cast<char>(textEvent->unicode);
                    }
                }
            }

            if (const auto* mousePressed = event->getIf<sf::Event::MouseButtonPressed>()) {
                if (mousePressed->button == sf::Mouse::Button::Left) {
                    if (popup.getIsOpen()) {
                        popup.handleClick(mousePressed->position, WINDOW_SIZE);
                    }
                    else {
                        handleMouseClick(mousePressed->position);
                    }
                }
            }

            if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>()) {
                if (!popup.getIsOpen()) {
                    handleKeyPress(keyPressed->code);
                }
            }
        }
    }

    void handleMouseClick(sf::Vector2i pos) {
        if (currentState == GameState::LOGIN) {
            if (isBtnClicked(pos, WINDOW_SIZE / 2 - 50, WINDOW_SIZE / 2 + 50, 100, 50)) {
                if (!userInputName.empty()) {
                    playSound("success");
                    playerName = userInputName;
                    currentState = GameState::MENU;
                }
            }
        }
        else if (currentState == GameState::MENU) {
            playSound("click");
            if (isBtnClicked(pos, WINDOW_SIZE / 2 - 100, 200, 200, 50)) {
                currentState = GameState::MODE_SELECT;
            }
            else if (isBtnClicked(pos, WINDOW_SIZE / 2 - 100, 300, 200, 50)) {
                if (sudoku.loadFromFile("last_game.txt")) {
                    currentState = GameState::PLAYING;
                    gameClock.restart();
                }
                else {
                    popup.setFont(&font);
                    popup.show("Error", "No saved game found!", { {"OK", [this]() {}} });
                }
            }
            else if (isBtnClicked(pos, WINDOW_SIZE / 2 - 100, 400, 200, 50)) {
                archiveLastGame();
                sudoku = board(selectedRemoveCount, selectedDiffName);
                sudoku.saveToFile("last_game.txt", playerName);
                currentState = GameState::PLAYING;
                gameClock.restart();
                additionalTime = sf::Time::Zero;
            }
        }
        else if (currentState == GameState::MODE_SELECT) {
            playSound("click");
            int btnW = 200, btnH = 40, startY = 150, gap = 60;
            if (isBtnClicked(pos, WINDOW_SIZE / 2 - 100, startY, btnW, btnH)) { setMode(1); currentState = GameState::MENU; }
            else if (isBtnClicked(pos, WINDOW_SIZE / 2 - 100, startY + gap, btnW, btnH)) { setMode(2); currentState = GameState::MENU; }
            else if (isBtnClicked(pos, WINDOW_SIZE / 2 - 100, startY + gap * 2, btnW, btnH)) { setMode(3); currentState = GameState::MENU; }
            else if (isBtnClicked(pos, WINDOW_SIZE / 2 - 100, startY + gap * 3, btnW, btnH)) { setMode(4); currentState = GameState::MENU; }
            else if (isBtnClicked(pos, WINDOW_SIZE / 2 - 100, startY + gap * 4, btnW, btnH)) { setMode(5); currentState = GameState::MENU; }
        }
        else if (currentState == GameState::PLAYING) {
            int row = pos.y / CELL_SIZE;
            int col = pos.x / CELL_SIZE;
            if (row >= 0 and row < GRID_SIZE and col >= 0 and col < GRID_SIZE) {
                sudoku.selectCell(row, col);
            }

            // Buttons
            if (pos.y > GRID_SIZE * CELL_SIZE + 20 and pos.y < GRID_SIZE * CELL_SIZE + 70) {
                if (pos.x > 50 and pos.x < 200) {
                    if (sudoku.markWrongHint()) {
                        playSound("success");
                    }
                    else if (fontLoaded) {
                        playSound("error");
                        popup.setFont(&font);
                        popup.show("Loser", "You have no hints remaining", { {"OK", [this]() {}} });
                    }
                }
            }
            if (pos.y > GRID_SIZE * CELL_SIZE + 90 and pos.y < GRID_SIZE * CELL_SIZE + 140) {
                if (pos.x > 50 and pos.x < 200) {
                    if (sudoku.getHintRight()) {
                        playSound("success");
                    }
                    else {
                        additionalTime += sf::seconds(100.0f);
                        playSound("error");
                        if (fontLoaded) {
                            popup.setFont(&font);
                            popup.show("Stop with the greed", "No more hints +100s punishment", { {"OK", [this]() {}} });
                        }
                    }
                }
            }
        }
    }

    void setMode(int choice) {
        switch (choice) {
        case 1: selectedRemoveCount = 40; selectedDiffName = "easy"; break;
        case 2: selectedRemoveCount = 50; selectedDiffName = "medium"; break;
        case 3: selectedRemoveCount = 60; selectedDiffName = "hard"; break;
        case 4: selectedRemoveCount = 70; selectedDiffName = "impossible"; break;
        case 5: selectedRemoveCount = 67; selectedDiffName = "67"; break;
        }
    }

    bool isBtnClicked(sf::Vector2i mouse, int x, int y, int w, int h) {
        return (mouse.x >= x && mouse.x <= x + w && mouse.y >= y && mouse.y <= y + h);
    }

    void handleKeyPress(sf::Keyboard::Key key) {
        if (currentState != GameState::PLAYING) return;

        if (key == sf::Keyboard::Key::Escape) {
            sudoku.saveToFile("last_game.txt", playerName);
            currentState = GameState::MENU;
            return;
        }

        int num = -1;
        if (key >= sf::Keyboard::Key::Num0 && key <= sf::Keyboard::Key::Num9) {
            num = static_cast<int>(key) - static_cast<int>(sf::Keyboard::Key::Num0);
        }
        else if (key >= sf::Keyboard::Key::Numpad0 && key <= sf::Keyboard::Key::Numpad9) {
            num = static_cast<int>(key) - static_cast<int>(sf::Keyboard::Key::Numpad0);
        }

        if (num != -1) {
            if (sudoku.placeNum(num)) {
                if (num != 0) playSound("success");
            }
            else {
                if (num != 0) playSound("error");
                additionalTime += sf::seconds(10.0f);
            }
        }
        else if (key == sf::Keyboard::Key::Delete || key == sf::Keyboard::Key::Backspace) {
            sudoku.placeNum(0);
        }

        if (sudoku.checkVictory()) {
            playSound("success");
            sudoku.saveToFile("last_game.txt", playerName);
            popup.setFont(&font);
            popup.show("Congratulations!", "You Win!", { {"Menu", [this]() { currentState = GameState::MENU; }} });
        }
    }

    void render() {
        window.clear(sf::Color::White);
        if (fontLoaded) {
            if (currentState == GameState::WELCOME) renderWelcome();
            else if (currentState == GameState::LOGIN) renderLogin();
            else if (currentState == GameState::MENU) renderMenu();
            else if (currentState == GameState::MODE_SELECT) renderModeSelect();
            else if (currentState == GameState::PLAYING) renderGame();
            popup.draw(window, WINDOW_SIZE);
        }
        window.display();
    }

    void renderWelcome() {
        // Animation
        float time = animClock.getElapsedTime().asSeconds();
        float scale = 1.0f + 0.05f * std::sin(time * 2.0f);
        int colorVal = static_cast<int>(127 + 127 * std::sin(time * 3.0f));

        sf::RectangleShape bg({ static_cast<float>(WINDOW_SIZE), static_cast<float>(WINDOW_SIZE) });
        bg.setFillColor(sf::Color(10, 10, 30));
        window.draw(bg);

        // Faint Sudoku Grid Background
        for (int i = 0; i <= 9; i++) {
            sf::RectangleShape line({ static_cast<float>(WINDOW_SIZE), 2.f });
            line.setPosition({ 0.f, static_cast<float>(i * (WINDOW_SIZE / 9)) });
            line.setFillColor(sf::Color(255, 255, 255, 20)); // Very faint
            window.draw(line);

            sf::RectangleShape vline({ 2.f, static_cast<float>(WINDOW_SIZE) });
            vline.setPosition({ static_cast<float>(i * (WINDOW_SIZE / 9)), 0.f });
            vline.setFillColor(sf::Color(255, 255, 255, 20));
            window.draw(vline);
        }

        sf::Text title(font, "WELCOME TO\nF5M'S SUDOKU", 50);
        title.setStyle(sf::Text::Bold);
        title.setOrigin(title.getLocalBounds().size / 2.f);
        title.setPosition({ WINDOW_SIZE / 2.f, WINDOW_SIZE / 2.f - 50 });
        title.setScale({ scale, scale });
        title.setFillColor(sf::Color(255, colorVal, 255));
        window.draw(title);

        sf::Text sub(font, "Click or Press Any Key", 20);
        sub.setOrigin(sub.getLocalBounds().size / 2.f);
        sub.setPosition({ WINDOW_SIZE / 2.f, WINDOW_SIZE / 2.f + 120 });
        sub.setFillColor(sf::Color::White);
        window.draw(sub);
    }

    void renderLogin() {
        sf::Text title(font, "ENTER YOUR NAME:", 30);
        title.setFillColor(sf::Color::Black);
        title.setPosition({ static_cast<float>(WINDOW_SIZE / 2 - 130), static_cast<float>(WINDOW_SIZE / 2 - 100) });
        window.draw(title);

        sf::Text nameText(font, userInputName + "_", 30);
        nameText.setFillColor(sf::Color::Blue);
        nameText.setPosition({ static_cast<float>(WINDOW_SIZE / 2 - 100), static_cast<float>(WINDOW_SIZE / 2 - 40) });
        window.draw(nameText);

        drawButton("ENTER", WINDOW_SIZE / 2 - 50, WINDOW_SIZE / 2 + 50, sf::Color::Green);
    }

    void renderMenu() {
        // Procedural Background
        sf::RectangleShape bg({ static_cast<float>(WINDOW_SIZE), static_cast<float>(WINDOW_SIZE) });
        bg.setFillColor(sf::Color(240, 245, 255)); // Nice light blue
        window.draw(bg);

        // Faint grid for atmosphere
        for (int i = 0; i < WINDOW_SIZE; i += 40) {
            sf::RectangleShape line({ static_cast<float>(WINDOW_SIZE), 1.f });
            line.setPosition({ 0.f, static_cast<float>(i) });
            line.setFillColor(sf::Color(200, 200, 220));
            window.draw(line);

            sf::RectangleShape vline({ 1.f, static_cast<float>(WINDOW_SIZE) });
            vline.setPosition({ static_cast<float>(i), 0.f });
            vline.setFillColor(sf::Color(200, 200, 220));
            window.draw(vline);
        }

        // Title
        sf::Text title(font, "F5M's SUDOKU", 50);
        title.setStyle(sf::Text::Bold | sf::Text::Underlined);
        title.setFillColor(sf::Color(40, 40, 80));
        // Center text
        sf::FloatRect textRect = title.getLocalBounds();
        title.setOrigin(textRect.getCenter());
        title.setPosition({ WINDOW_SIZE / 2.0f, 80.0f });
        window.draw(title);

        // Credits (Top Left under title area)
        sf::Text credits(font, "Made by:\nMohammed Ahmed Rabie\nAdam Magdy Mohammed\nYassin Ibrahim Mohamed", 18);
        credits.setFillColor(sf::Color(80, 80, 80));
        credits.setPosition({ 20.f, 130.f });
        window.draw(credits);

        // Welcome User
        sf::Text user(font, "Player: " + playerName, 24);
        user.setFillColor(sf::Color(0, 100, 200));
        sf::FloatRect userRect = user.getLocalBounds();
        user.setOrigin(userRect.getCenter());
        user.setPosition({ WINDOW_SIZE / 2.0f, 160.0f });
        window.draw(user);

        // Buttons
        drawButton("Game Mode: " + selectedDiffName, WINDOW_SIZE / 2 - 100, 220, sf::Color(100, 150, 255), 200);
        drawButton("Continue Game", WINDOW_SIZE / 2 - 100, 300, sf::Color(100, 255, 100), 200);
        drawButton("Play New Game", WINDOW_SIZE / 2 - 100, 380, sf::Color(255, 100, 100), 200);
    }

    void renderModeSelect() {
        sf::Text title(font, "SELECT DIFFICULTY", 30);
        title.setFillColor(sf::Color::Black);
        title.setPosition({ static_cast<float>(WINDOW_SIZE / 2 - 140), 50.f });
        window.draw(title);
        int startY = 150, gap = 60;
        drawButton("Easy (40)", WINDOW_SIZE / 2 - 100, startY, sf::Color::Cyan, 200);
        drawButton("Medium (50)", WINDOW_SIZE / 2 - 100, startY + gap, sf::Color::Yellow, 200);
        drawButton("Hard (60)", WINDOW_SIZE / 2 - 100, startY + gap * 2, sf::Color(255, 165, 0), 200);
        drawButton("Impossible (70)", WINDOW_SIZE / 2 - 100, startY + gap * 3, sf::Color::Red, 200);
        drawButton("Mode 67", WINDOW_SIZE / 2 - 100, startY + gap * 4, sf::Color::Magenta, 200);
    }

    void renderGame() {
        drawGrid();
        drawNumbers();
        drawGameButtons();
        drawSideInfo();
    }

    int basicForRender(int num) {
        if (num % 11 == 0) return num / 11;
        else if (num % 13 == 0) return num / 13;
        else if (num % 17 == 0) return num / 17;
        else return num;
    }

    void drawSideInfo() {
        float sideX = GRID_SIZE * CELL_SIZE + 30.f;
        float startY = 50.f;
        sf::Text pName(font, playerName, 20);
        pName.setFillColor(sf::Color::Magenta);
        pName.setPosition({ sideX, startY - 30 });
        window.draw(pName);
        sf::Text diff(font, "Mode: " + sudoku.getDifficulty(), 16);
        diff.setFillColor(sf::Color::Black);
        diff.setPosition({ sideX, startY });
        window.draw(diff);
        sf::Text hintsLabel(font, "Hints: " + std::to_string(sudoku.getHints()), 24);
        hintsLabel.setFillColor(sf::Color::Black);
        hintsLabel.setPosition({ sideX, startY + 50.f });
        window.draw(hintsLabel);
        int seconds = static_cast<int>(gameClock.getElapsedTime().asSeconds() + additionalTime.asSeconds());
        char timeStr[10];
        sprintf(timeStr, "%02d:%02d", seconds / 60, seconds % 60);
        sf::Text timeValue(font, timeStr, 32);
        timeValue.setFillColor(sf::Color(0, 100, 200));
        timeValue.setPosition({ sideX, startY + 150.f });
        window.draw(timeValue);
        sf::Text escInfo(font, "Press ESC\nto Menu", 16);
        escInfo.setFillColor(sf::Color::Red);
        escInfo.setPosition({ sideX, startY + 250.f });
        window.draw(escInfo);
    }

    void drawGrid() {
        auto selectedCell = sudoku.getSelectedCell();
        auto [row, col] = selectedCell.value_or(std::make_pair(-1, -1));
        for (int i = 0; i < GRID_SIZE; i++) {
            for (int j = 0; j < GRID_SIZE; j++) {
                sf::RectangleShape cell({ CELL_SIZE - 2.f, CELL_SIZE - 2.f });
                cell.setPosition({ static_cast<float>(j * CELL_SIZE + 1), static_cast<float>(i * CELL_SIZE + 1) });

                if (i == row and j == col) cell.setFillColor(sf::Color(200, 220, 255));
                else if (sudoku.isImmutable(i, j)) cell.setFillColor(sf::Color(230, 230, 230));
                else cell.setFillColor(sf::Color::White);

                cell.setOutlineThickness(1.f);
                cell.setOutlineColor(sf::Color(200, 200, 200));
                window.draw(cell);
            }
        }
        for (int i = 0; i <= GRID_SIZE; i += 3) {
            sf::RectangleShape line({ static_cast<float>(GRID_SIZE * CELL_SIZE), 3.f });
            line.setPosition({ 0.f, static_cast<float>(i * CELL_SIZE - 1) });
            line.setFillColor(sf::Color::Black);
            window.draw(line);
            sf::RectangleShape vline({ 3.f, static_cast<float>(GRID_SIZE * CELL_SIZE) });
            vline.setPosition({ static_cast<float>(i * CELL_SIZE - 1), 0.f });
            vline.setFillColor(sf::Color::Black);
            window.draw(vline);
        }
    }

    void drawNumbers() {
        for (int i = 0; i < GRID_SIZE; i++) {
            for (int j = 0; j < GRID_SIZE; j++) {
                int value = sudoku.getCell(i, j);
                if (value != 0) {
                    sf::Text text(font, std::to_string(basicForRender(value)), 32);
                    text.setPosition({ static_cast<float>(j * CELL_SIZE + 20), static_cast<float>(i * CELL_SIZE + 10) });

                    if (value % 11 == 0) text.setFillColor(sf::Color::Black);
                    else if (value % 13 == 0) text.setFillColor(sf::Color(230, 50, 50));
                    else if (value % 17 == 0) text.setFillColor(sf::Color(0, 50, 220));
                    else text.setFillColor(sf::Color(0, 100, 200));

                    window.draw(text);
                }
            }
        }
    }

    void drawGameButtons() {
        drawButton("Mark 3X (Hint)", 50, GRID_SIZE * CELL_SIZE + 20, sf::Color(230, 80, 100));
        drawButton("Tell 3C (Hint)", 50, GRID_SIZE * CELL_SIZE + 90, sf::Color(100, 200, 100));
    }

    void drawButton(const std::string& text, float x, float y, const sf::Color& color, float width = 150.f) {
        sf::RectangleShape button({ width, 50.f });
        button.setPosition({ x, y });
        button.setFillColor(color);
        button.setOutlineThickness(2.f);
        button.setOutlineColor(sf::Color::Black);
        window.draw(button);
        sf::Text buttonText(font, text, 18);
        buttonText.setFillColor(sf::Color::Black);
        sf::FloatRect textBounds = buttonText.getLocalBounds();
        buttonText.setPosition({
            x + width / 2 - textBounds.size.x / 2.f,
            y + 25.f - textBounds.size.y / 2.f - 5.f
            });
        window.draw(buttonText);
    }
};

int main() {
    sudokuWindowUI game;
    game.run();
    return 0;
}
