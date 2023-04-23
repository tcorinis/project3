//

#include "SearchBar.h"

void SearchBar::draw(RenderTarget &target, RenderStates states) const {
    target.draw(text);
    target.draw(icon);

    sf::RectangleShape line;
    line.setSize({(float) (WIDTH-40), 2});
    line.setPosition({20, (float) (target.getSize().y) - 30 - text.getCharacterSize()});
    line.setFillColor(sf::Color(0, 0, 0));
    target.draw(line);
}

SearchBar::SearchBar() {
    // Load font
    if (!font.loadFromFile("arial.ttf")) {
        throw std::runtime_error("Failed to load font. Is the font file present?");
    }

    // Create search bar text
    text.setFont(font);
    text.setString("FUCK thign.");
    text.setCharacterSize(24);
    text.setFillColor(sf::Color(0,0,0));
    text.setPosition(50, HEIGHT - 20);
    // Riddle me this: Why???
    text.scale(1, -1);

    // Load icon
    iconTexture.loadFromFile("Emolgeon.png");
    icon.setTexture(iconTexture);
    icon.setTextureRect(sf::Rect<int>(644, 376, 250, 250));
    icon.setPosition(20, HEIGHT-20);
    icon.scale(0.1, -0.1);
}
