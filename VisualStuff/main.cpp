#include <iostream>
#include <sfml/Graphics.hpp>
#include "SearchBar.h"

int main() {
    const int WIDTH = 500;
    const int HEIGHT = 900;

    sf::RenderWindow window(sf::VideoMode(WIDTH, HEIGHT), "Song Searcher");

    sf::Color Light_Grey(230,230,230);
    sf::Color Not_Black(20,20,20);

    while(window.isOpen()) {
        // Polling for events, e.g.
        sf::Event event;
        while(window.pollEvent(event)) {
            if(event.type == sf::Event::Closed) {
                window.close();
            }
        }
        window.clear(Light_Grey);

        // drawPane is the pane to be drawn to before we put it to screen
        sf::RenderTexture drawPane;
        drawPane.create(WIDTH, HEIGHT);
        auto size = drawPane.getSize(); // convenient size vector

        SearchBar bar;
        drawPane.draw(bar);

        // Convert the pane to a texture, to a sprite, so we can draw it to the screen.
        sf::Texture paneTexture = drawPane.getTexture();
        sf::Sprite pane(paneTexture);
        window.draw(pane);
        window.display();
    }
    return 0;
}
