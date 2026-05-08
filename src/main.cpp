#include <SFML/Graphics.hpp>
#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/ConvexShape.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/System/Angle.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <SFML/Window/Mouse.hpp>
#include <SFML/Window/Window.hpp>
#include <SFML/Window/WindowEnums.hpp>

#include <optional>
#include <vector>

auto assignment1(sf::RenderWindow &window, std::vector<sf::FloatRect> &boundingBoxes) -> void
{
    // A red circle with a green outline
    sf::CircleShape circle(50.f);
    circle.setFillColor(sf::Color::Red);
    circle.setOutlineThickness(10.f);
    circle.setOutlineColor(sf::Color::Green);
    circle.setPosition({50.0, 50.0});
    boundingBoxes.emplace_back(circle.getGlobalBounds());
    window.draw(circle);

    // An orange rectangle
    sf::RectangleShape rect({50.0, 80.0});
    rect.setFillColor(sf::Color{255, 128, 0});
    rect.setPosition({190, 50});
    boundingBoxes.emplace_back(rect.getGlobalBounds());
    window.draw(rect);

    // A blue line
    sf::RectangleShape line({5.0, 80.0});
    line.setFillColor(sf::Color::Blue);
    line.setPosition({270, 50});
    boundingBoxes.emplace_back(line.getGlobalBounds());
    window.draw(line);

    // An octagon with a texture
    sf::Texture tex("assets/textures/red_text.jpg");
    sf::CircleShape octagon(50, 8);
    octagon.setTexture(&tex);
    octagon.setPosition({320, 50});
    boundingBoxes.emplace_back(octagon.getGlobalBounds());
    window.draw(octagon);

    // A purple "Hello, world!" text
    sf::Font font("assets/fonts/Urban Heroes.ttf");
    sf::Text text(font);
    text.setCharacterSize(90);
    text.setString("HELLO, WORLD!");
    text.setFillColor(sf::Color{102, 0, 153});
    text.setPosition({50, 200});
    boundingBoxes.emplace_back(text.getGlobalBounds());
    window.draw(text);
}

auto assignment2(sf::CircleShape octagon, sf::Time elapsed) -> void { return; }

auto startGame() -> void
{
    // Create a window without titlebar, resize or close buttons and with a size of 800x600 pixels
    // with a white background and set the title to "My first SFML game".
    const auto *TITLE = "My first SFML game";
    const auto HORIZONTAL_RESOLUTION = 800U;
    const auto VERTICAL_RESOLUTION = 600U;

    sf::RectangleShape rotatingRect({20, 75});
    rotatingRect.setPosition({250, 360});
    rotatingRect.setFillColor(sf::Color::Black);
    rotatingRect.setOrigin({10, 75 / 2});
    auto isFocused = true;
    const auto degreesPerSecond = 180.0F;

    sf::ConvexShape player(4);
    player.setPoint(0, {0.f, 10.f});
    player.setPoint(1, {40.f, 0.f});
    player.setPoint(2, {0.f, -10.f});
    player.setPoint(3, {-20.f, 0.f});

    player.setFillColor(sf::Color::Cyan);

    player.setPosition({400, 450});
    auto savePosition{player.getPosition()};
    const auto playerMovementPerSecond = 200.F;

    sf::RenderWindow window(sf::VideoMode({HORIZONTAL_RESOLUTION, VERTICAL_RESOLUTION}), TITLE,
                            sf::Style::None);
    sf::Clock clock;
    while (window.isOpen())
    {
        while (const std::optional event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
            {
                window.close();
            }
            if (event->is<sf::Event::FocusLost>())
            {
                isFocused = false;
            }
            if (event->is<sf::Event::FocusGained>())
            {
                isFocused = true;
            }
        }

        sf::Time elapsedTime = clock.restart();
        window.clear(sf::Color::White);
        std::vector<sf::FloatRect> boundingBoxes{};
        boundingBoxes.emplace_back(rotatingRect.getGlobalBounds());

        assignment1(window, boundingBoxes);

        // assignment 2:
        if (isFocused)
        {
            rotatingRect.rotate(sf::degrees(degreesPerSecond * elapsedTime.asSeconds()));
            auto playerBounds = player.getGlobalBounds();

            // collision check
            auto canMoveLeft{true};
            auto canMoveRight{true};
            auto canMoveUp{true};
            auto canMoveDown{true};
            for (const auto otherBox : boundingBoxes)
            {
                if (const std::optional intersection = playerBounds.findIntersection(otherBox))
                {
                    auto intersectionCenter{intersection.value().getCenter()};
                    if (intersectionCenter.x < player.getPosition().x)
                    {
                        canMoveLeft = false;
                    }
                    if (intersectionCenter.x > player.getPosition().x)
                    {
                        canMoveRight = false;
                    }
                    if (intersectionCenter.y < player.getPosition().y)
                    {
                        canMoveUp = false;
                    }
                    if (intersectionCenter.y > player.getPosition().y)
                    {
                        canMoveDown = false;
                    }
                }
            }

            // movement
            auto distance{playerMovementPerSecond * elapsedTime.asSeconds()};
            if (canMoveRight && sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right))
            {
                player.move({distance, 0});
            }
            else if (canMoveLeft && sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left))
            {
                player.move({-distance, 0});
            }
            else if (canMoveUp && sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Up))
            {
                player.move({0, -distance});
            }
            else if (canMoveDown && sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Down))
            {
                player.move({0, distance});
            }
        }

        window.draw(rotatingRect);

        // assignment 3:
        window.draw(player);

        // assignment 4:
        auto mousePosition{sf::Mouse::getPosition(window)};
        sf::View view{};
        view.setCenter(player.getPosition() + static_cast<sf::Vector2f>(mousePosition));
        window.setView(view);
        window.display();
    }
}

auto main() -> int
{
    startGame();
    return 0;
}
