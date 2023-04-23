//
#include <sfml/Graphics.hpp>

#ifndef PROJECT2_SEARCHBAR_H
#define PROJECT2_SEARCHBAR_H

using sf::String;
using sf::Texture;
using sf::Sprite;
using sf::RenderTarget;
using sf::RenderStates;
using sf::Font;
using sf::Text;

class SearchBar : public sf::Drawable {
    const int WIDTH = 500;
    const int HEIGHT = 900;

    bool active;
    int cursorLocation;

    Font font;
    Text text;

    Texture iconTexture;
    Sprite icon;

public:
    SearchBar();
    void draw (RenderTarget &target, RenderStates states) const override;
    bool isActive() {return active;}
    void setActive(bool active);
    void moveCursor(int direction);
    void enterText(String text);
};


#endif //PROJECT2_SEARCHBAR_H
