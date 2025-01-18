#include "sgg/graphics.h"
#include <string>
#include <vector>
#include <iostream>

/***************************************************
 * Window & Canvas
 ***************************************************/
static const float WINDOW_WIDTH = 800.f;
static const float WINDOW_HEIGHT = 600.f;

/***************************************************
 * Screen states
 ***************************************************/
enum class ScreenState
{
    MENU,
    GAME,
    EXIT
};

/***************************************************
 * Single button
 ***************************************************/
struct Button
{
    float x = 0.f;
    float y = 0.f;
    float width = 0.f;
    float height = 0.f;
    std::string label;
};

/***************************************************
 * AnimState
 ***************************************************/
enum class AnimState
{
    IDLE,
    PUNCHING,
    KO
};

/***************************************************
 * Player: x,y, speed, health, jumping, ...
 ***************************************************/
struct Player
{
    float x = 0.f;
    float y = 0.f;
    float speed = 200.f;
    float health = 100.f;
    bool  jumping = false;
    float vy = 0.f;
    float punchCooldown = 0.f;

    AnimState anim = AnimState::IDLE;

    // 3 PNGs
    std::string spriteIdle;
    std::string spritePunch;
    std::string spriteKO;
};

/***************************************************
 * GameState
 ***************************************************/
struct GameState
{
    ScreenState currentScreen = ScreenState::MENU;
    bool running = true;

    // backgrounds
    std::string menuBackgroundPath = "assets\\background.png";
    std::string arenaBackgroundPath = "assets\\arena_bg.png";

    std::vector<Button> menuButtons;

    // two players
    Player player1;
    Player player2;
};

// global pointer
static GameState* g_game = nullptr;

/***************************************************
 * isInsideButton
 ***************************************************/
bool isInsideButton(const Button& btn, float mx, float my)
{
    float halfW = btn.width * 0.5f;
    float halfH = btn.height * 0.5f;
    return !(mx < btn.x - halfW || mx >(btn.x + halfW) ||
        my < btn.y - halfH || my >(btn.y + halfH));
}

/***************************************************
 * drawButton
 ***************************************************/
void drawButton(const Button& btn)
{
    graphics::Brush br;
    br.outline_opacity = 1.f;
    br.outline_width = 2.f;
    br.fill_color[0] = 0.2f;
    br.fill_color[1] = 0.2f;
    br.fill_color[2] = 0.2f;
    graphics::drawRect(btn.x, btn.y, btn.width, btn.height, br);

    if (btn.label == "Play")
        graphics::setFont("assets\\start_font.ttf");

    br.outline_opacity = 0.f;
    br.fill_color[0] = 1.f;
    br.fill_color[1] = 1.f;
    br.fill_color[2] = 1.f;
    graphics::drawText(btn.x - 40.f, btn.y + 8.f, 24.f, btn.label.c_str(), br);
}

/***************************************************
 * drawMenu
 ***************************************************/
void drawMenu()
{
    if (!g_game->menuBackgroundPath.empty())
    {
        graphics::Brush br;
        br.texture = g_game->menuBackgroundPath;
        br.outline_opacity = 0.f;
        graphics::drawRect(400.f, 300.f, 800.f, 600.f, br);
    }

    graphics::Brush br;
    br.outline_opacity = 0.f;
    br.fill_color[0] = 1.f;
    br.fill_color[1] = 1.f;
    br.fill_color[2] = 1.f;
    graphics::drawText(280, 100, 40, "MY MENU", br);

    for (auto& b : g_game->menuButtons)
        drawButton(b);

    graphics::drawText(220, 550, 20, "Use mouse to click, or ESC to quit.", br);
}

/***************************************************
 * pickSprite
 ***************************************************/
std::string pickSprite(const Player& p)
{
    if (p.anim == AnimState::KO)       return p.spriteKO;
    if (p.anim == AnimState::PUNCHING) return p.spritePunch;
    return p.spriteIdle;
}

/***************************************************
 * drawGame: no alpha_mode
 ***************************************************/
void drawGame()
{
    // 1) arena background
    if (!g_game->arenaBackgroundPath.empty())
    {
        graphics::Brush br;
        br.texture = g_game->arenaBackgroundPath;
        br.outline_opacity = 0.f;
        graphics::drawRect(400.f, 300.f, 800.f, 600.f, br);
    }

    float groundY = 380.f;
    float spriteW = 80.f;
    float spriteH = 110.f;

    // Player1
    {
        std::string sprite = pickSprite(g_game->player1);

        graphics::Brush br;
        br.outline_opacity = 0.f;
      
        br.texture = sprite;

        float px = g_game->player1.x;
        float py = groundY - g_game->player1.y;
        graphics::drawRect(px, py, spriteW, spriteH, br);
    }

    // Player2
    {
        std::string sprite = pickSprite(g_game->player2);

        graphics::Brush br;
        br.outline_opacity = 0.f;
        br.texture = sprite;

        float px = g_game->player2.x;
        float py = groundY - g_game->player2.y;
        graphics::drawRect(px, py, spriteW, spriteH, br);
    }

    // health bars top corners
    graphics::Brush br;
    br.outline_opacity = 0.f;

    // p1
    float p1pct = g_game->player1.health / 100.f;
    float barW1 = 200.f, barH = 20.f;
    float barX1 = barW1 * 0.5f + 10.f;
    float barY = 10.f;
    br.fill_color[0] = 0.3f; br.fill_color[1] = 0.3f; br.fill_color[2] = 0.3f;
    graphics::drawRect(barX1, barY, barW1, barH, br);
    br.fill_color[0] = 1.f - p1pct;
    br.fill_color[1] = p1pct;
    br.fill_color[2] = 0.f;
    float fillW1 = barW1 * p1pct;
    graphics::drawRect(barX1 - (barW1 - fillW1) / 2.f, barY, fillW1, barH, br);

    // p2
    float p2pct = g_game->player2.health / 100.f;
    float barW2 = 200.f;
    float barX2 = WINDOW_WIDTH - barW2 * 0.5f - 10.f;
    br.fill_color[0] = 0.3f; br.fill_color[1] = 0.3f; br.fill_color[2] = 0.3f;
    graphics::drawRect(barX2, barY, barW2, barH, br);
    br.fill_color[0] = 1.f - p2pct;
    br.fill_color[1] = p2pct;
    br.fill_color[2] = 0.f;
    float fillW2 = barW2 * p2pct;
    graphics::drawRect(barX2 - (barW2 - fillW2) / 2.f, barY, fillW2, barH, br);

    br.fill_color[0] = br.fill_color[1] = br.fill_color[2] = 1.f;
    if (g_game->player1.health <= 0.f)
        graphics::drawText(300.f, 200.f, 40.f, "Player 2 Wins!", br);
    if (g_game->player2.health <= 0.f)
        graphics::drawText(300.f, 200.f, 40.f, "Player 1 Wins!", br);

    // instructions
    graphics::drawText(10.f, 590.f, 15.f,
        "P1: A/D=move, W=jump, G=punch | P2: Left/Right=move, Up=jump, RCTRL=punch", br);
}

/***************************************************
 * updatePlayer
 ***************************************************/
void updatePlayer(Player& p, float dt,
    graphics::scancode_t leftKey,
    graphics::scancode_t rightKey,
    graphics::scancode_t jumpKey,
    graphics::scancode_t punchKey)
{
    if (p.anim == AnimState::KO) return;

    if (graphics::getKeyState(leftKey))
        p.x -= p.speed * dt;
    if (graphics::getKeyState(rightKey))
        p.x += p.speed * dt;

    if (p.x < 50.f)  p.x = 50.f;
    if (p.x > 750.f) p.x = 750.f;

    if (!p.jumping && graphics::getKeyState(jumpKey))
    {
        p.jumping = true;
        p.vy = 300.f;
    }
    if (p.jumping)
    {
        p.y += p.vy * dt;
        p.vy -= 600.f * dt;
        if (p.y < 0.f)
        {
            p.y = 0.f;
            p.jumping = false;
            p.vy = 0.f;
        }
    }

    if (p.punchCooldown > 0.f)
    {
        p.punchCooldown -= dt;
        if (p.punchCooldown < 0.f) p.punchCooldown = 0.f;
    }

    if (graphics::getKeyState(punchKey) && p.punchCooldown <= 0.f)
    {
        p.punchCooldown = 0.5f;
        p.anim = AnimState::PUNCHING;
    }
    else
    {
        if (p.anim != AnimState::KO)
        {
            if (p.punchCooldown < 0.45f && !p.jumping)
                p.anim = AnimState::IDLE;
        }
    }
}

/***************************************************
 * checkPunch => damage=3
 ***************************************************/
void checkPunch(Player& A, Player& B)
{
    if (A.anim == AnimState::PUNCHING)
    {
        if (A.punchCooldown > 0.45f)
        {
            float dx = A.x - B.x;
            if (dx < 0.f) dx = -dx;
            if (dx < 60.f && B.health>0.f)
            {
                B.health -= 3.f;
                if (B.health < 0.f) B.health = 0.f;
                if (B.health <= 0.f)
                    B.anim = AnimState::KO;
            }
        }
    }
}

/***************************************************
 * updateGame
 ***************************************************/
void updateGame(float dt)
{
    auto& p1 = g_game->player1;
    auto& p2 = g_game->player2;

    updatePlayer(p1, dt,
        graphics::SCANCODE_A,
        graphics::SCANCODE_D,
        graphics::SCANCODE_W,
        graphics::SCANCODE_G);

    updatePlayer(p2, dt,
        graphics::SCANCODE_LEFT,
        graphics::SCANCODE_RIGHT,
        graphics::SCANCODE_UP,
        graphics::SCANCODE_RCTRL);

    if (p1.health > 0.f && p2.health > 0.f)
    {
        checkPunch(p1, p2);
        checkPunch(p2, p1);
    }
}

/***************************************************
 * sgg_draw
 ***************************************************/
void sgg_draw()
{
    if (!g_game || !g_game->running) return;

    switch (g_game->currentScreen)
    {
    case ScreenState::MENU:
        drawMenu();
        break;
    case ScreenState::GAME:
        drawGame();
        break;
    case ScreenState::EXIT:
        break;
    }
}

/***************************************************
 * sgg_update
 ***************************************************/
void sgg_update(float ms)
{
    float dt = ms * 0.001f;
    if (!g_game || !g_game->running)
    {
        graphics::destroyWindow();
        return;
    }

    if (graphics::getKeyState(graphics::SCANCODE_ESCAPE))
    {
        if (g_game->currentScreen == ScreenState::MENU)
        {
            g_game->currentScreen = ScreenState::EXIT;
            g_game->running = false;
            return;
        }
        else
        {
            g_game->currentScreen = ScreenState::MENU;
            return;
        }
    }

    if (g_game->currentScreen == ScreenState::MENU)
    {
        graphics::MouseState m;
        graphics::getMouseState(m);
        if (m.button_left_pressed)
        {
            float mx = (float)m.cur_pos_x;
            float my = (float)m.cur_pos_y;
            for (auto& b : g_game->menuButtons)
            {
                if (isInsideButton(b, mx, my))
                {
                    if (b.label == "Play")
                    {
                        g_game->currentScreen = ScreenState::GAME;
                        auto& p1 = g_game->player1;
                        auto& p2 = g_game->player2;
                        p1.x = 200.f; p1.y = 0.f; p1.health = 100.f;
                        p1.jumping = false; p1.vy = 0.f; p1.punchCooldown = 0.f; p1.anim = AnimState::IDLE;
                        p2.x = 600.f; p2.y = 0.f; p2.health = 100.f;
                        p2.jumping = false; p2.vy = 0.f; p2.punchCooldown = 0.f; p2.anim = AnimState::IDLE;
                    }
                }
            }
        }
    }
    else if (g_game->currentScreen == ScreenState::GAME)
    {
        updateGame(dt);
    }

    if (g_game->currentScreen == ScreenState::EXIT)
    {
        g_game->running = false;
    }
}

/***************************************************
 * main
 ***************************************************/
int main()
{
    graphics::createWindow(800, 600, "NoAlphaBlend Older SGG Example");
    graphics::setUpdateFunction(sgg_update);
    graphics::setDrawFunction(sgg_draw);

    graphics::setCanvasSize(800, 600);
    graphics::setCanvasScaleMode(graphics::CANVAS_SCALE_FIT);

    {
        graphics::Brush bg;
        bg.fill_color[0] = 0.2f;
        bg.fill_color[1] = 0.2f;
        bg.fill_color[2] = 0.2f;
        graphics::setWindowBackground(bg);
    }

    graphics::setFont("assets\\myfont.ttf");

    g_game = new GameState();

    // single "Play" button
    {
        Button b;
        b.x = 400.f; b.y = 250.f;
        b.width = 200.f; b.height = 50.f;
        b.label = "Play";
        g_game->menuButtons.push_back(b);
    }

    // player1
    g_game->player1.spriteIdle = "assets\\player1_idle.png";
    g_game->player1.spritePunch = "assets\\player1_punch.png";
    g_game->player1.spriteKO = "assets\\player1_ko.png";

    // player2
    g_game->player2.spriteIdle = "assets\\player2_idle.png";
    g_game->player2.spritePunch = "assets\\player2_punch.png";
    g_game->player2.spriteKO = "assets\\player2_ko.png";

    // backgrounds
    g_game->menuBackgroundPath = "assets\\background.png";
    g_game->arenaBackgroundPath = "assets\\arena_bg.png";

    g_game->player1.x = 200.f;
    g_game->player2.x = 600.f;

    graphics::startMessageLoop();
    delete g_game;
    g_game = nullptr;
    return 0;
}
