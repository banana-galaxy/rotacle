#include <iostream>
#include <SFML/Graphics.hpp>
#include <cmath>


class Mouse {
    bool preState = false;
    bool state = false;
public:
    void update(bool pressed) {
        preState = state;
        state = pressed;
    }

    bool getPressed() {
        if (preState != state && state) {
            return true;
        } else {
            return false;
        }
    }

};

class Line {


public:
    double firstAngle;
    int firstPoint[2] = {-1, -1};
    int secondPoint[2] = {-1, -1};
    int axle[2] = {-1, -1};

    int firstRadius = -1;
    double secondAngle;
    int secondRadius = -1;
    int windowWidth;
    int windowHeight;
    bool moving = false;
    float turningSpeed = 1;

    Line(int width, int height) {
        windowWidth = width;
        windowHeight = height;
    }

    bool isSetup() {
        if (firstPoint[0] != -1 && secondPoint[0] != -1 && axle[0] != -1 && firstRadius != -1 && secondRadius != -1) {
            return true;
        }
        return false;
    }

    void setNextPoint(int x, int y) {
        if (firstPoint[0] == -1) {
            firstPoint[0] = x;
            firstPoint[1] = y;
        } else if (secondPoint[0] == -1) {
            secondPoint[0] = x;
            secondPoint[1] = y;
        } else if (axle[0] == -1) {
            int absDeltaX = abs(secondPoint[0] - firstPoint[0]);
            int absDeltaY = abs(secondPoint[1] - firstPoint[1]);
            int deltaX = secondPoint[0] - firstPoint[0];
            int deltaY = secondPoint[1] - firstPoint[1];
            // y = mx + b
            // b = y - mx
            bool set = true;
            if (absDeltaX > absDeltaY) {
                if (firstPoint[0] < secondPoint[0] && (x < firstPoint[0] || x > secondPoint[0])) {
                    set = false;
                } else if (secondPoint[0] < firstPoint[0] && (x < secondPoint[0] || x > firstPoint[0])) {
                    set = false;
                } else {
                    bool negSlope = false;
                    if (deltaX>0 && deltaY>0 || deltaX<0 && deltaY<0) {
                        negSlope = true;
                    }
                    double slope = (double) (negSlope ? -absDeltaY : absDeltaY) / (double) absDeltaX;
                    double yIntercept = windowHeight - firstPoint[1] - slope * firstPoint[0];
                    y = windowHeight - (slope * x + yIntercept);
                }
            } else {
                if (firstPoint[1] < secondPoint[1] && (y < firstPoint[1] || y > secondPoint[1])) {
                    set = false;
                } else if (secondPoint[1] < firstPoint[1] && (y < secondPoint[1] || y > firstPoint[1])) {
                    set = false;
                } else {
                    bool negSlope = false;
                    if (deltaY>0 && deltaX>0 || deltaY<0 && deltaX<0) {
                        negSlope = true;
                    }
                    double slope = (double) (negSlope ? -absDeltaX : absDeltaX) / (double) absDeltaY;
                    double xIntercept = windowWidth - firstPoint[0] - slope * firstPoint[1];
                    x = windowWidth - (slope * y + xIntercept);
                }
            }

            if (set) {
                axle[0] = x;
                axle[1] = y;

                firstAngle = atan2(firstPoint[1] - axle[1], firstPoint[0] - axle[0]);
                secondAngle = atan2(secondPoint[1] - axle[1], secondPoint[0] - axle[0]);
                firstRadius = sqrt(pow(firstPoint[0] - axle[0], 2) + pow(firstPoint[1] - axle[1], 2));
                secondRadius = sqrt(pow(secondPoint[0] - axle[0], 2) + pow(secondPoint[1] - axle[1], 2));
            }
        }
    }

    void move() {
        firstAngle++;
        secondAngle++;


    }

    bool isMoving() {
        return moving;
    }

    void startMovement() {
        moving = true;
    }

    void stopMovement() {
        moving = false;
    }

    sf::RectangleShape getDrawable() {
        sf::RectangleShape line(sf::Vector2f((float)(firstRadius+secondRadius), 1.f));
        line.setPosition((float)firstPoint[0], (float)firstPoint[1]);
        double angle = atan2(secondPoint[1] - firstPoint[1], secondPoint[0] - firstPoint[0]);
        line.rotate(angle*180/M_PI);
        return line;
    }
};

class Vector2 {
public:
    int x = 0;
    int y = 0;
    Vector2() = default;
    Vector2(int x, int y) {
        this->x = x;
        this->y = y;
    }
    Vector2& operator=(Vector2 other) {
        x = other.x;
        y = other.y;
        return *this;
    }
};

class TextBox{
public:
    Vector2 dimensions;
    Vector2 position;
    sf::RectangleShape box;
    TextBox(Vector2 dimensions, Vector2 position) {
        this->dimensions = dimensions;
        this->position = position;
    }

    void setSize(Vector2 size) {
        dimensions = size;
    }

    void setPosition(Vector2 position) {
        this->position = position;
    }

    void render(sf::RenderWindow &window) {
        box.setSize(sf::Vector2f((float)dimensions.x, (float)dimensions.y));
        box.setPosition((float)position.x, (float)position.y);
        window.draw(box);
    }
};

int main() {
    // Setup

    // window
    sf::RenderWindow window(
            sf::VideoMode(640, 480),
            "Hello World");

    unsigned int width = window.getSize().x;
    unsigned int height = window.getSize().y;

    // drawing grid
    sf::Image image;
    image.create(width, height, sf::Color::Black);
    sf::Texture texture;
    sf::Sprite sprite;

    for (int x = 0; x < width; x++) {
        for (int y = 0; y < height; y++) {
            image.setPixel(x,y,{0,0,0});
        }
    }

    // Control
    Mouse mouse;

    // Line
    Line line(width, height);

    // Circle
    sf::CircleShape circle;
    float circleRadius = 10.0f;
    circle.setRadius(circleRadius);
    circle.setFillColor(sf::Color::White);

    // Text box
    TextBox box({50,20}, {60,60});
    std::cout << box.dimensions.x << "\n";

//    sf::RectangleShape dot(sf::Vector2f(1.f, 1.f));
//    sf::RectangleShape vline(sf::Vector2f(5.f, 150.f));

    // Game Loop
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
        }

        // debug info
        //std::cout << line.axle[1] << "\n";
        //std::cout << line.firstAngle << "\n";
        //std::cout << sf::Mouse::getPosition(window).y << "\n";
        // std::cout << firstPoint[0] << " " << secondPoint[0] << "\n";

        // Logic
        mouse.update(sf::Mouse::isButtonPressed(sf::Mouse::Left));

        if (mouse.getPressed()) {
            // grid drawing
            sf::Vector2i pos = sf::Mouse::getPosition(window);/**/
//            if ((pos.x > 0 && pos.x < width) && ((pos.y > 0 && pos.y < height))) {
//                image.setPixel(pos.x, pos.y, {255, 255, 255});
//            }

            if (!line.isSetup()) {
                line.setNextPoint(pos.x, pos.y);
            }// else if (!line.isMoving()) {
//                line.startMovement();
//            } else {
//                line.stopMovement();
//            }

//            if(pos.x > 0 && pos.x < width && pos.y > 0 && pos.y < height) {
//                if (firstPoint[0] == 0) {
//                    firstPoint[0] = pos.x;
//                    firstPoint[1] = pos.y;
//                } else {
//                    secondPoint[0] = pos.x;
//                    secondPoint[1] = pos.y;
//                }
//            }
        }

//        if (line.isMoving()) {
//            line.move();
//        }


        // Drawing
        window.clear();
        texture.loadFromImage(image);
        sprite.setTexture(texture);
        window.draw(sprite);
        if (line.isSetup()) {
            window.draw(line.getDrawable());
        }
        if (line.firstPoint[0] != -1) {
            circle.setPosition((float)line.firstPoint[0]-circleRadius, (float)line.firstPoint[1]-circleRadius);
            window.draw(circle);
        }
        if (line.secondPoint[0] != -1) {
            circle.setPosition((float)line.secondPoint[0]-circleRadius, (float)line.secondPoint[1]-circleRadius);
            window.draw(circle);
        }
        if (line.axle[0] != -1) {
            circle.setPosition((float)line.axle[0]-circleRadius, (float)line.axle[1]-circleRadius);
            window.draw(circle);
        }
        box.render(window);
        window.display();
    }
    return 0;
}