#include "sgg/graphics.h"
#include <iostream>
#include <cmath>
#include <vector>

static const float WINDOW_WIDTH = 800.0f;
static const float WINDOW_HEIGHT = 400.0f;


struct Fighter
{
    float x;            
    float y;            
    float speed;        
    float health;       
    bool  isPunching;
    float punchCooldown;
    float punchRange;
    float radius;       
    std::string name;
    std::string spritePath;

    
    graphics::scancode_t keyLeft;
    graphics::scancode_t keyRight;
    graphics::scancode_t keyPunch;
};


struct GameState
{
    std::vector<Fighter> fighters;
    bool running = true;
};

static GameState* g_game = nullptr;


void updateFighter(Fighter& f, float dt)
{
    if (f.punchCooldown > 0.0f)
    {
        f.punchCooldown -= dt;
        if (f.punchCooldown < 0.0f)
            f.punchCooldown = 0.0f;
    }

    if (graphics::getKeyState(f.keyLeft))
        f.x -= f.speed * dt;
    if (graphics::getKeyState(f.keyRight))
        f.x += f.speed * dt;

 
    if (graphics::getKeyState(f.keyPunch) && f.punchCooldown <= 0.f)
    {
        f.isPunching = true;
        f.punchCooldown = 0.5f;
    }
    else
    {
        f.isPunching = false;
    }

    if (f.health <= 0.f)
        f.health = 0.f;
}


void drawFighter(const Fighter& f)
{
    graphics::Brush br;
    br.texture = f.spritePath;
    br.outline_opacity = 0.f;
   
    graphics::drawRect(f.x, f.y, 80, 80, br);

    br.texture = "";
    br.fill_color[0] = br.fill_color[1] = br.fill_color[2] = 1.0f;
    graphics::drawText(f.x - 20, f.y - 50, 20, f.name.c_str(), br);

   
    float barW = 60.f, barH = 6.f;
    float pct = f.health / 100.f;
    
    br.fill_color[0] = br.fill_color[1] = br.fill_color[2] = 0.3f;
    graphics::drawRect(f.x, f.y - 40, barW, barH, br);
    
    br.fill_color[0] = 1.f - pct; 
    br.fill_color[1] = pct;
    br.fill_color[2] = 0.f;
    graphics::drawRect(f.x - (barW * (1.f - pct)) / 2.f, f.y - 40, barW * pct, barH, br);
}


void checkPunch(Fighter& A, Fighter& B)
{
    if (A.isPunching && B.health > 0.f)
    {
        float dx = A.x - B.x;
        float dy = A.y - B.y;
        float dist = std::sqrt(dx * dx + dy * dy);
        if (dist < (A.punchRange + B.radius))
        {
            B.health -= 10.f;
            std::cout << "[Punch] " << A.name << " hits " << B.name << "!\n";
        }
    }
}


void sgg_update(float ms)
{
    float dt = ms * 0.001f;
    if (!g_game || !g_game->running)
    {
        graphics::destroyWindow();
        return;
    }

   
    for (auto& f : g_game->fighters)
        updateFighter(f, dt);


    if (g_game->fighters.size() == 2)
    {
        checkPunch(g_game->fighters[0], g_game->fighters[1]);
        checkPunch(g_game->fighters[1], g_game->fighters[0]);
    }

    g_game->fighters.erase(
        std::remove_if(g_game->fighters.begin(), g_game->fighters.end(),
            [](const Fighter& f) { return f.health <= 0.f; }),
        g_game->fighters.end()
    );

    if (graphics::getKeyState(graphics::SCANCODE_ESCAPE) ||
        g_game->fighters.empty())
    {
        g_game->running = false;
    }
}

void sgg_draw()
{
    if (!g_game || !g_game->running)
        return;

    
    for (auto& f : g_game->fighters)
        drawFighter(f);


    graphics::Brush br;
    br.fill_color[0] = br.fill_color[1] = br.fill_color[2] = 1.f;
    br.outline_opacity = 0.f;
    graphics::drawText(10, 40, 30, "Kombat Arena - Using scancode_t", br);
}


int main()
{
    
    graphics::createWindow((unsigned int)WINDOW_WIDTH, (unsigned int)WINDOW_HEIGHT, "Kombat Arena");
    graphics::setUpdateFunction(sgg_update);
    graphics::setDrawFunction(sgg_draw);

    
    graphics::setCanvasSize(WINDOW_WIDTH, WINDOW_HEIGHT);
    graphics::setCanvasScaleMode(graphics::CANVAS_SCALE_FIT);

    
    graphics::Brush bg;
    bg.fill_color[0] = 0.1f;
    bg.fill_color[1] = 0.1f;
    bg.fill_color[2] = 0.1f;
    graphics::setWindowBackground(bg);


    g_game = new GameState();

    Cooldown = 0.f;
        f.punchRange = 50.f;
        f.radius = 30.f;
        f.spritePath = "assets\\fighter1.png";

        
        f.keyLeft = graphics::SCANCODE_A;
        f.keyRight = graphics::SCANCODE_D;
        f.keyPunch = graphics::SCANCODE_G;

        g_game->fighters.push_back(f);
    }


    {
        Fighter f;
        f.name = "Ken";
        f.x = 600.f;
        f.y = 200.f;
        f.speed = 200.f;
        f.health = 100.f;
        f.isPunching = false;
        f.punchCooldown = 0.f;
        f.punchRange = 50.f;
        f.radius = 30.f;
        f.spritePath = "assets\\fighter2.png";

        f.keyLeft = graphics::SCANCODE_LEFT;
        f.keyRight = graphics::SCANCODE_RIGHT;
        f.keyPunch = graphics::SCANCODE_RCTRL;

        g_game->fighters.push_back(f);
    }

    graphics::startMessageLoop();

    delete g_game;
    g_game = nullptr;
    return 0;
}

