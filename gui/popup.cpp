
#include "popup.h"
#include <iostream>

PopUp::PopUp() : isOpen(false), font(nullptr) {}

PopUp::PopUp(const std::string& title, const std::string& message)
    : isOpen(false), title(title), message(message), font(nullptr) {
}

void PopUp::setFont(sf::Font* f) {
    font = f;
}

void PopUp::show(const std::string& t, const std::string& msg, const std::vector<std::pair<std::string, std::function<void()>>>& buttons) {
    title = t;
    message = msg;
    popupButtons = buttons;
    isOpen = true;
}

void PopUp::close() {
    isOpen = false;
}

bool PopUp::getIsOpen() const {
    return isOpen;
}

void PopUp::handleClick(sf::Vector2i mousePos, int windowSize) {
    if (!isOpen) return;

    int centerX = windowSize / 2;
    int centerY = windowSize / 2;
    int buttonY = centerY + 20;
    int buttonSpacing = 120;

    // Cast size to int to prevent warning
    int startX = centerX - (static_cast<int>(popupButtons.size()) * buttonSpacing) / 2 + 50;

    for (size_t i = 0; i < popupButtons.size(); i++) {
        float btnX = static_cast<float>(startX + i * buttonSpacing);
        if (mousePos.x > btnX && mousePos.x < btnX + 100 &&
            mousePos.y > buttonY && mousePos.y < buttonY + 50) {
            popupButtons[i].second();
            close();
            break;
        }
    }
}

void PopUp::draw(sf::RenderWindow& window, int windowSize) {
    if (!isOpen || !font) return;

    int centerX = windowSize / 2;
    int centerY = windowSize / 2;

    sf::RectangleShape overlay({ static_cast<float>(windowSize), static_cast<float>(windowSize) });
    overlay.setFillColor(sf::Color(0, 0, 0, 180));
    window.draw(overlay);

    sf::RectangleShape dialog({ 400.f, 250.f });
    dialog.setPosition({ centerX - 200.f, centerY - 125.f });
    dialog.setFillColor(sf::Color::White);
    dialog.setOutlineThickness(3.f);
    dialog.setOutlineColor(sf::Color::Black);
    window.draw(dialog);

    // SFML 3.0: Font first
    sf::Text titleText(*font, title, 28);
    titleText.setFillColor(sf::Color::Black);
    sf::FloatRect titleBounds = titleText.getLocalBounds();
    titleText.setPosition({ centerX - titleBounds.size.x / 2.f, centerY - 100.f });
    window.draw(titleText);

    sf::Text messageText(*font, message, 20);
    messageText.setFillColor(sf::Color(60, 60, 60));
    sf::FloatRect msgBounds = messageText.getLocalBounds();
    messageText.setPosition({ centerX - msgBounds.size.x / 2.f, centerY - 50.f });
    window.draw(messageText);

    int buttonY = centerY + 20;
    int buttonSpacing = 120;
    // Cast to int
    int startX = centerX - (static_cast<int>(popupButtons.size()) * buttonSpacing) / 2 + 50;

    for (size_t i = 0; i < popupButtons.size(); i++) {
        drawButton(window, popupButtons[i].first,
            static_cast<float>(startX + i * buttonSpacing),
            static_cast<float>(buttonY),
            sf::Color(100, 150, 255));
    }
}

void PopUp::drawButton(sf::RenderWindow& window, const std::string& text,
    float x, float y, const sf::Color& color) {
    sf::RectangleShape button({ 100.f, 50.f });
    button.setPosition({ x, y });
    button.setFillColor(color);
    button.setOutlineThickness(2.f);
    button.setOutlineColor(sf::Color::Black);
    window.draw(button);

    // SFML 3.0: Font first
    sf::Text buttonText(*font, text, 20);
    buttonText.setFillColor(sf::Color::Black);

    sf::FloatRect textBounds = buttonText.getLocalBounds();
    buttonText.setPosition({
        x + 50.f - textBounds.size.x / 2.f,
        y + 25.f - textBounds.size.y / 2.f - 5.f
        });

    window.draw(buttonText);
}
