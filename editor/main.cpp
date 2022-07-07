#include <iostream>
#include <vector>
#include <SFML/Graphics.hpp>
#include <sstream>
#include <locale>
#include <codecvt>
#include <cwctype> // std::tolower();
#include <fstream>

#include <Line.hpp>
#include <Graph.hpp>
#include <InputField.hpp>
#include <Button.hpp>
#include <Slider.hpp>
#include <RoundRect.hpp>

int main()
{
    bool lControlPressed{};

    float scale = 1.0;
    float scaleSpeed{};

    sf::Vector2f offset;

    bool hasImage{};
    sf::Texture bgImage;
    hasImage = bgImage.loadFromFile("map.jpg");
    
    sf::Sprite bgSprite;
    float bgImageScale{1080.0f / bgImage.getSize().y};

    if(hasImage) {
        bgImage.setSmooth(true);
        bgSprite.setTexture(bgImage);
        bgSprite.setScale({bgImageScale, bgImageScale});
        bgSprite.setColor({255, 255, 255, 100});
    }

    sf::ContextSettings settings;
    settings.antialiasingLevel = 4;

    sf::RenderWindow window(sf::VideoMode(1920, 1080), "", sf::Style::Fullscreen, settings);
    window.setFramerateLimit(60);

    sf::Font font;
    if(!font.loadFromFile("font.ttf")) return 1;
    
    Graph g;
    g.pushVertex({L"Альдрун", {100, 100}});
    g.pushVertex({L"Балмора", {200, 400}});
    g.pushVertex({L"Кальдера", {600, 200}});

    g.insertEdge(0, 1, {L"Гильдия Магов", {255, 20, 20}, 20});
    g.insertEdge(1, 2, {L"Силт-Страйдер", {255, 200, 0}, 10});
    g.insertEdge(2, 0, {L"Лодка", {0, 20, 255}, 5});

    sf::RectangleShape rect;
    rect.setFillColor(sf::Color::White);
    rect.setSize({600, 1080});
    rect.setPosition({1320, 0});
    rect.setOutlineColor({200, 200, 200});
    rect.setOutlineThickness(1);

    sf::RectangleShape lineBorder;
    lineBorder.setSize({600, 1});
    lineBorder.setPosition({1320, 290});
    lineBorder.setFillColor(sf::Color(200, 200, 200));

    InputField vertexInput(font, {1400, 100}, L"Имя вершины");
    Button vertexButton(font, {1400, 200}, L"Добавить", [&]() {
        if(!vertexInput.getString().empty()) {
            g.pushVertex({vertexInput.getString(), mult({100, 900}, 1.0f / scale)});
        }
    });
    sf::Text vertexText;
    vertexText.setFont(font);
    vertexText.setString(L"Добавить точку на карту:");
    vertexText.setCharacterSize(30);
    vertexText.setFillColor(sf::Color(100, 100, 100));
    vertexText.setPosition({1360, 30});

    sf::Text edgeText;
    edgeText.setFont(font);
    edgeText.setString(L"Добавить ребро:");
    edgeText.setCharacterSize(30);
    edgeText.setFillColor(sf::Color(100, 100, 100));
    edgeText.setPosition({1360, 300});

    sf::Text edgeColorText;
    edgeColorText.setFont(font);
    edgeColorText.setString(L"Цвет ребра:");
    edgeColorText.setCharacterSize(30);
    edgeColorText.setFillColor(sf::Color(100, 100, 100));
    edgeColorText.setPosition({1400, 550});

    InputField from(font, {1400, 370}, L"Откуда");
    InputField to(font, {1400, 470}, L"Куда");

    InputField edgeNameInput(font, {1400, 780}, L"Имя ребра");
    InputField edgeWeightInput(font, {1400, 880}, L"Вес ребра (цифра)");

    sf::Color sliderColor;

    Button edgeButton(font, {1400, 980}, L"Добавить", [&]() {
        float weight{};
        try {
            std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> converter;
            weight = std::stof(converter.to_bytes(edgeWeightInput.getString()));
        } catch (std::invalid_argument const &e) {
            return;
        }

        if(!edgeNameInput.getString().empty() && !from.getString().empty() && !to.getString().empty()) {
            size_t vertexIndex1{1000000};
            size_t vertexIndex2{1000000};
            for (size_t i = 0; i < g.getVertecies().size(); i++)
            {
                if(g.getVertecies()[i].id == from.getString()) {
                    vertexIndex1 = i;
                }
                if(g.getVertecies()[i].id == to.getString()) {
                    vertexIndex2 = i;
                }
            }

            if(vertexIndex1 < g.getVertecies().size() && vertexIndex2 < g.getVertecies().size() && vertexIndex1 != vertexIndex2) {
                g.insertEdge(
                    vertexIndex1,
                    vertexIndex2,
                    {
                        edgeNameInput.getString(),
                        sliderColor,
                        weight
                    }
                );
            }
        }
    });

    Slider redSlider({1400, 630});
    Slider greenSlider({1400, 680});
    Slider blueSlider({1400, 730});

    RoundRect colorRect;
    colorRect.setPosition({1770, 630});
    colorRect.setSize({100, 100});
    colorRect.setOutlineColor(sf::Color(230, 230, 230));
    colorRect.setOutlineThickness(6);
    colorRect.setRoundingPercent(40);

    sf::Clock cl;

    sf::Clock deltaClock;
    float dt;

    sf::Vector2i mousePos = sf::Mouse::getPosition(window);
    sf::Vector2i oldPos;

    while (window.isOpen())
    {
        oldPos = mousePos;
        mousePos = sf::Mouse::getPosition(window);

        dt = deltaClock.restart().asSeconds();

        sf::Event event;
        while (window.pollEvent(event))
        {
            if (
                event.type == sf::Event::Closed || 
                (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape)
            ) {
                window.close();
            }

            if(event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::LControl) {
                lControlPressed = true;
            }
            if(event.type == sf::Event::KeyReleased && event.key.code == sf::Keyboard::LControl) {
                lControlPressed = false;
            }

            if(lControlPressed && event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::S) {
                nl::json resJson;
                // if(hasImage) {
                //     std::ifstream imageFile("map.jpg", std::ios::binary);
                //     std::vector<unsigned char> buffer(std::istreambuf_iterator<char>(imageFile), {});
                //     resJson["image"] = nl::json::binary_t(buffer);
                // }
                resJson["graph"] = g.packToJson(1080.0);
                std::ofstream os("map.json", std::ios::trunc);
                os << std::setw(2) << resJson;
            }

            if(lControlPressed && event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::D) {
                std::cout << "scale: " << scale << "\n";
                std::cout << "offset: " << offset.x << ", " << offset.y << "\n";
            }

            g.updateInput(event);

            vertexInput.update(event, window);
            vertexButton.update(event, window);

            from.update(event, window);
            to.update(event, window);

            edgeNameInput.update(event, window);
            edgeWeightInput.update(event, window);
            edgeButton.update(event, window);

            if(event.mouseWheelScroll.delta > 0 && scale < 6)
                scaleSpeed += std::min(event.mouseWheelScroll.delta, 10.0f) * dt * scale * 0.2;
            else if(event.mouseWheelScroll.delta < 0 && scale > 1.0)
                scaleSpeed += event.mouseWheelScroll.delta * dt * scale * 0.2;
        }

        if(sf::Mouse::isButtonPressed(sf::Mouse::Middle)) {
            offset += delta(toVec2f(mousePos), toVec2f(oldPos));
        }

        scale += scaleSpeed;
        scaleSpeed *= (1.0f - 8.0 * dt);

        if(hasImage) {
            bgSprite.setScale({bgImageScale * scale, bgImageScale * scale});
            bgSprite.setPosition(offset);
        }

        g.update(window, scale, offset);

        redSlider.update(window);
        greenSlider.update(window);
        blueSlider.update(window);

        sliderColor = {
            (uint8_t)(redSlider.getValue() * 255),
            (uint8_t)(greenSlider.getValue() * 255),
            (uint8_t)(blueSlider.getValue() * 255),
        };

        colorRect.setFillColor(sliderColor);

        window.clear(sf::Color::White);

        if(hasImage) {
            window.draw(bgSprite);
        }
        g.draw(window, window, font, sf::Color::White, {100, 100, 100}, scale, offset);

        window.draw(rect);
        window.draw(lineBorder);
        window.draw(vertexText);
        window.draw(vertexInput);
        window.draw(vertexButton);

        window.draw(edgeText);
        window.draw(edgeColorText);
        window.draw(edgeNameInput);
        window.draw(edgeWeightInput);
        window.draw(edgeButton);

        window.draw(from);
        window.draw(to);

        window.draw(redSlider);
        window.draw(greenSlider);
        window.draw(blueSlider);
        window.draw(colorRect);

        window.display();
    }

    return 0;
}