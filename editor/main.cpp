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
#include <FileExplorer.hpp>

// TODO third case for move
// TODO

int main([[maybe_unused]]int argc, char** argv) {
    sf::Font font;
    if(!font.loadFromFile("font.ttf")) return 1;

    std::wstring filePath;

    sf::ContextSettings settings;
    settings.antialiasingLevel = 4;

    sf::RenderWindow greetWindow(sf::VideoMode(900, 600), "", sf::Style::None, settings);
    greetWindow.setFramerateLimit(60);

    FileExplorer fe(argv[0], font, {});

    Button newButton(font, {650, 50}, L"Создать", [&]() {
        filePath = L"";
        greetWindow.close();
    });

    Button openButton(font, {650, 150}, L"Открыть", [&]() {
        filePath = fe.getActivePath();
        greetWindow.close();
    });

    sf::Clock gcl;

    while (greetWindow.isOpen())
    {
        float dt = gcl.restart().asSeconds();

        sf::Event event;
        while (greetWindow.pollEvent(event))
        {
            if (
                event.type == sf::Event::Closed || 
                (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape)
            ) {
                greetWindow.close();
                return 0;
            }

            fe.updateInput(event, dt);

            newButton.update(event, greetWindow);
            openButton.update(event, greetWindow);
        }

        fe.update(dt);

        greetWindow.clear(sf::Color::White);
        greetWindow.draw(fe);
        greetWindow.draw(newButton);
        greetWindow.draw(openButton);
        greetWindow.display();
    }

    bool lControlPressed{};
    bool isMoving{};

    float scale = 1.0;
    float scaleSpeed{};

    sf::Vector2f offset;

    bool hasImage{};
    sf::Texture bgImage;

    Graph g;

    if(filePath.empty()) {
        hasImage = bgImage.loadFromFile("map.jpg");

        g.pushVertex({L"Альдрун", {100, 100}});
        g.pushVertex({L"Балмора", {200, 400}});
        g.pushVertex({L"Кальдера", {600, 200}});

        g.insertEdge(0, 1, {L"Гильдия Магов", {255, 20, 20}, 20});
        g.insertEdge(1, 2, {L"Силт-Страйдер", {255, 200, 0}, 10});
        g.insertEdge(2, 0, {L"Лодка", {0, 20, 255}, 5});
    } else {
        nl::json js;
        try {
            std::ifstream is(filePath.c_str());
            is >> js;
        } catch (nl::json::parse_error const &e) {
            std::cout << e.what();
            return 1;
        }

        g.loadFromJson(js["graph"], 1080.0f);

        auto buf = js["image"]["bytes"].get<std::vector<unsigned char>>();

        hasImage = bgImage.loadFromMemory(buf.data(), buf.size());
    }
    
    sf::Sprite bgSprite;
    float bgImageScale{1080.0f / bgImage.getSize().y};

    if(hasImage) {
        bgImage.setSmooth(true);
        bgSprite.setTexture(bgImage);
        bgSprite.setScale({bgImageScale, bgImageScale});
        bgSprite.setColor({255, 255, 255, 100});
    }

    sf::RenderWindow window(sf::VideoMode(1920, 1080), "", sf::Style::Fullscreen, settings);
    window.setFramerateLimit(60);

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

    RoundRect sliderX;
    sliderX.setPosition({10, 1050});
    sliderX.setSize({1270, 20});
    sliderX.setFillColor(sf::Color(230, 230, 230));
    sliderX.setRoundingPercent(100.0f);

    RoundRect sliderY;
    sliderY.setPosition({1290, 10});
    sliderY.setSize({20, 1030});
    sliderY.setFillColor(sf::Color{230, 230, 230});
    sliderY.setRoundingPercent(100.0f);

    sf::Vector2f sliderOffsetSpped{};

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
                if(hasImage) {
                    std::ifstream imageFile("map.jpg", std::ios::binary);
                    std::vector<unsigned char> buffer(std::istreambuf_iterator<char>(imageFile), {});
                    resJson["image"] = nl::json::binary_t(buffer);
                }
                resJson["graph"] = g.packToJson(1080.0);
                std::ofstream os("map.json", std::ios::trunc);
                os << std::setw(2) << resJson;
            }

            if(lControlPressed && event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::D) {
                std::cout << "scale: " << scale << "\n";
                std::cout << "offset: " << offset.x << ", " << offset.y << "\n";
            }

            g.updateInput(event, lControlPressed);

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

        isMoving = false;
        if(sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
            if(mousePos.x <= sliderX.getSize().x && mousePos.y >= sliderX.getPosition().y) {
                sliderOffsetSpped.x -= delta(toVec2f(mousePos), toVec2f(oldPos)).x * dt * 8.0f;
                isMoving = true;
            }
            if(mousePos.y <= sliderY.getSize().y && mousePos.x >= sliderY.getPosition().x && mousePos.x <= sliderY.getPosition().x + sliderY.getSize().x) {
                sliderOffsetSpped.y -= delta(toVec2f(mousePos), toVec2f(oldPos)).y * dt * 8.0f;
                isMoving = true;
            }
        }

        sliderOffsetSpped = mult(sliderOffsetSpped, 1.0f - 8.0 * dt);
        offset += sliderOffsetSpped;

        scale += scaleSpeed;
        scaleSpeed *= (1.0f - 8.0 * dt);

        if(hasImage) {
            bgSprite.setScale({bgImageScale * scale, bgImageScale * scale});
            bgSprite.setPosition(offset);
        }

        if(!isMoving) {
            g.update(window, scale, offset, ((hasImage) ? sf::Vector2f{bgImage.getSize().x * bgImageScale, bgImage.getSize().y * bgImageScale} : sf::Vector2f{1320, 1080}));
        }

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

        window.draw(sliderX);
        window.draw(sliderY);

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