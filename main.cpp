
#include "sgg/graphics.h"
#include <string>
#include <iostream>

using namespace std;

// Constants
const int WINDOW_WIDTH = 1200;
const int WINDOW_HEIGHT = 600;
const int PLAYER_WIDTH = 50;
const int PLAYER_HEIGHT = 100;
const int PLAYER_SPEED = 200;
const int ATTACK_DAMAGE = 10;


class Player {
public:
    float x, y; 
    int health;
    bool attacking;
    int playerNum;

    Player(float startX, float startY, int num) {
        x = startX;
        y = startY;
        health = 100;
        attacking = false;
        playerNum = num;
    }

    void moveLeft(float ms) {
        x -= PLAYER_SPEED * ms / 1000.0f;
        if (x < 0) x = 0;
    }

    void moveRight(float ms) {
        x += PLAYER_SPEED * ms / 1000.0f;
        if (x > WINDOW_WIDTH - PLAYER_WIDTH) x = WINDOW_WIDTH - PLAYER_WIDTH;
    }

    void attack(Player& opponent) {
        if (!attacking && abs(x - opponent.x) < PLAYER_WIDTH) {
            opponent.health -= ATTACK_DAMAGE;
            if (opponent.health < 0) opponent.health = 0;
            attacking = true; 
        }
    }

    void draw() {
        graphics::Brush brush;
        brush.fill_color[0] = (playerNum == 1) ? 1.0f : 0.0f; 
        brush.fill_color[1] = 0.0f;
        brush.fill_color[2] = (playerNum == 2) ? 1.0f : 0.0f; 
        graphics::drawRect(x + PLAYER_WIDTH / 2, y + PLAYER_HEIGHT / 2, PLAYER_WIDTH, PLAYER_HEIGHT, brush);
    }

    void update(float ms) {
        if (attacking) {
            attacking = false;
        }
    }

    int getHealth() const {
        return health;
    }
};

// Game State
Player player1(100, WINDOW_HEIGHT - PLAYER_HEIGHT - 50, 1);
Player player2(WINDOW_WIDTH - 150, WINDOW_HEIGHT - PLAYER_HEIGHT - 50, 2);

void drawHealthBar(float x, float y, int health) {
    graphics::Brush brush;
    brush.fill_color[0] = 1.0f - (health / 100.0f);
    brush.fill_color[1] = health / 100.0f;
    brush.fill_color[2] = 0.0f;
    graphics::drawRect(x, y, health * 2, 20, brush);
}

void update(float ms) {
    // Player 1 controls
    if (graphics::getKeyState(graphics::SCANCODE_A)) {
        player1.moveLeft(ms);
    }
    if (graphics::getKeyState(graphics::SCANCODE_D)) {
        player1.moveRight(ms);
    }
    if (graphics::getKeyState(graphics::SCANCODE_W)) {
        player1.attack(player2);
    }

    
    if (graphics::getKeyState(graphics::SCANCODE_LEFT)) {
        player2.moveLeft(ms);
    }
    if (graphics::getKeyState(graphics::SCANCODE_RIGHT)) {
        player2.moveRight(ms);
    }
    if (graphics::getKeyState(graphics::SCANCODE_UP)) {
        player2.attack(player1);
    }

    player1.update(ms);
    player2.update(ms);
}

void draw() {
    graphics::Brush brush;

    brush.fill_color[0] = 0.2f;
    brush.fill_color[1] = 0.2f;
    brush.fill_color[2] = 0.2f;
    graphics::drawRect(WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2, WINDOW_WIDTH, WINDOW_HEIGHT, brush);

    
    player1.draw();
    player2.draw();

    drawHealthBar(100, 50, player1.getHealth());
    drawHealthBar(900, 50, player2.getHealth());

    if (player1.getHealth() <= 0 || player2.getHealth() <= 0) {
        string winner = (player1.getHealth() > 0) ? "Player 1 Wins!" : "Player 2 Wins!";
        brush.fill_color[0] = 1.0f;
        brush.fill_color[1] = 1.0f;
        brush.fill_color[2] = 1.0f;
        graphics::drawText(WINDOW_WIDTH / 2 - 100, WINDOW_HEIGHT / 2, 30, winner, brush);
    }
}

int main() {
    graphics::createWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Kombat Arena");

    graphics::setDrawFunction(draw);
    graphics::setUpdateFunction(update);

    graphics::setCanvasSize(WINDOW_WIDTH, WINDOW_HEIGHT);
    graphics::setCanvasScaleMode(graphics::CANVAS_SCALE_FIT);

    graphics::Brush br;
    br.fill_color[0] = 0.1f;
    br.fill_color[1] = 0.1f;
    br.fill_color[2] = 0.1f;
    graphics::setWindowBackground(br);

    graphics::startMessageLoop();

    return 0;
}

