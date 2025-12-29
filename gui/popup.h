#ifndef POPUP_H 
#define POPUP_H 

#include <SFML/Graphics.hpp>
#include <string>
#include <functional>
#include <vector>

class PopUp {
private:
    bool isOpen;
    std::string title;
    std::string message;
    std::vector<std::pair<std::string, std::function<void()>>> popupButtons;
    sf::Font* font;

    void drawButton(sf::RenderWindow& window, const std::string& text, float x, float y, const sf::Color& color);

public:
    PopUp();
    PopUp(const std::string& title, const std::string& message);
    void setFont(sf::Font* f);
    void show(const std::string& t, const std::string& msg, const std::vector<std::pair<std::string, std::function<void()>>>& buttons);
    void close();
    bool getIsOpen() const;
    void handleClick(sf::Vector2i mousePos, int windowSize);
    void draw(sf::RenderWindow& window, int windowSize);
};

#endif
