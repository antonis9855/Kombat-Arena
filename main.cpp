#include "sgg/graphics.h"
#include <vector>
#include <string>
#include <algorithm>

enum class ScreenState { MENU, GAME, EXIT };

class GameState; 


class GameObject {
protected:
    static int next_id;
    GameState* state;
    std::string name;
    int id;
    bool active;

public:
    GameObject(GameState* gs, const std::string& n = "")
        : state(gs), name(n), active(true) {
        id = ++next_id;
    }
    virtual ~GameObject() {}
    virtual void update(float dt) {}
    virtual void draw() {}
    bool isActive() const { return active; }
    void setActive(bool a) { active = a; }
    const std::string& getName() const { return name; }
};
int GameObject::next_id = 0;


enum class AnimState { IDLE, PUNCHING, KO };

class Fighter : public GameObject {
private:
    float x = 0.f, y = 0.f, speed = 200.f;
    float health = 100.f;
    bool jumping = false;
    float vy = 0.f, punchCooldown = 0.f;
    AnimState anim = AnimState::IDLE;
    std::string spriteIdle, spritePunch, spriteKO;

public:
    Fighter(GameState* gs, const std::string& name) : GameObject(gs, name) {}

    
    void setSprites(const std::string& idle,
        const std::string& punch,
        const std::string& ko) {
        spriteIdle = idle;
        spritePunch = punch;
        spriteKO = ko;
    }
    void setPosition(float px, float py) { x = px; y = py; }
    float getHealth() const { return health; }
    void setHealth(float h) { health = h; }

    
    virtual void update(float dt) override;
    virtual void draw() override;

    
    void checkPunch(Fighter& other);
    void resolveOverlap(Fighter& other);
};

void Fighter::update(float dt) {
    if (anim == AnimState::KO) return;

    using namespace graphics;
    bool left = false, right = false, jump = false, punch = false;

    if (name == "Player1") {
        left = getKeyState(SCANCODE_A);
        right = getKeyState(SCANCODE_D);
        jump = getKeyState(SCANCODE_W);
        punch = getKeyState(SCANCODE_G);
    }
    else {
        left = getKeyState(SCANCODE_LEFT);
        right = getKeyState(SCANCODE_RIGHT);
        jump = getKeyState(SCANCODE_UP);
        punch = getKeyState(SCANCODE_RCTRL);
    }

    if (left) x -= speed * dt;
    if (right) x += speed * dt;
    if (x < 50.f) x = 50.f;
    if (x > 750.f) x = 750.f;

    if (!jumping && jump) { jumping = true; vy = 300.f; }
    if (jumping) {
        y += vy * dt;
        vy -= 600.f * dt;
        if (y < 0.f) { y = 0.f; jumping = false; vy = 0.f; }
    }

    if (punchCooldown > 0.f) {
        punchCooldown -= dt;
        if (punchCooldown < 0.f) punchCooldown = 0.f;
    }
    if (punch && punchCooldown <= 0.f) {
        anim = AnimState::PUNCHING;
        punchCooldown = 0.5f;
    }
    else if (anim != AnimState::KO && punchCooldown < 0.45f && !jumping) {
        anim = AnimState::IDLE;
    }
}

void Fighter::draw() {
    using namespace graphics;
    std::string sprite = (anim == AnimState::KO) ? spriteKO :
        (anim == AnimState::PUNCHING) ? spritePunch : spriteIdle;
    Brush br;
    br.outline_opacity = 0.f;
    br.texture = sprite;
    float groundY = 380.f;
    drawRect(x, groundY - y, 80.f, 110.f, br);
}

void Fighter::checkPunch(Fighter& other) {
    if (anim == AnimState::PUNCHING && punchCooldown > 0.45f) {
        float dx = x - other.x;
        if (dx < 0) dx = -dx;
        if (dx < 60.f && other.health > 0.f) {
            other.health -= 3.f;
            if (other.health < 0.f) other.health = 0.f;
            if (other.health <= 0.f) other.anim = AnimState::KO;
        }
    }
}

void Fighter::resolveOverlap(Fighter& other) {
    float r = 30.f;
    float dx = x - other.x;
    float dist = dx < 0 ? -dx : dx;
    float overlap = (2 * r) - dist;
    if (overlap > 0.f) {
        float half = overlap * 0.5f;
        if (dx > 0.f) { x += half; other.x -= half; }
        else { x -= half; other.x += half; }
    }
    if (x > other.x) {
        float mid = (x + other.x) * 0.5f;
        x = mid - r; other.x = mid + r;
    }
}


class MenuButton : public GameObject {
private:
    float x, y, width, height;
    std::string label;
public:
    MenuButton(GameState* gs, const std::string& lbl, float px, float py, float w, float h)
        : GameObject(gs, lbl), x(px), y(py), width(w), height(h), label(lbl) {
    }
    bool isInside(float mx, float my) {
        float halfW = width * 0.5f;
        float halfH = height * 0.5f;
        return mx >= (x - halfW) && mx <= (x + halfW) &&
            my >= (y - halfH) && my <= (y + halfH);
    }
    virtual void draw() override {
        using namespace graphics;
        Brush br;
        br.outline_opacity = 1.f;
        br.outline_width = 2.f;
        br.fill_color[0] = br.fill_color[1] = br.fill_color[2] = 0.2f;
        drawRect(x, y, width, height, br);

        if (label == "Play") setFont("assets\\start_font.ttf");
        br.outline_opacity = 0.f;
        br.fill_color[0] = br.fill_color[1] = br.fill_color[2] = 1.f;
        drawText(x - 40.f, y + 8.f, 24.f, label.c_str(), br);
    }
};

class GameState {
private:
    std::vector<GameObject*> objects;
public:
    ScreenState currentScreen = ScreenState::MENU;
    bool running = true;
    std::string menuBackgroundPath = "assets\\background.png";
    std::string arenaBackgroundPath = "assets\\arena_bg.png";

    void init();
    void update(float dt);
    void draw();
    std::vector<GameObject*>& getObjects() { return objects; }
    Fighter* getFighter(const std::string& name) {
        for (auto* obj : objects) {
            Fighter* f = dynamic_cast<Fighter*>(obj);
            if (f && f->getName() == name) return f;
        }
        return nullptr;
    }
    ~GameState() {
        for (auto* obj : objects) delete obj;
        objects.clear();
    }
};

void GameState::init() {
    objects.push_back(new MenuButton(this, "Play", 400.f, 250.f, 200.f, 50.f));

    Fighter* f1 = new Fighter(this, "Player1");
    f1->setSprites("assets\\player1_idle.png", "assets\\player1_punch.png", "assets\\player1_ko.png");
    f1->setPosition(200.f, 0.f);
    objects.push_back(f1);

    Fighter* f2 = new Fighter(this, "Player2");
    f2->setSprites("assets\\player2_idle.png", "assets\\player2_punch.png", "assets\\player2_ko.png");
    f2->setPosition(600.f, 0.f);
    objects.push_back(f2);
}

void GameState::update(float dt) {
    for (auto* obj : objects) obj->update(dt);
    Fighter* p1 = getFighter("Player1");
    Fighter* p2 = getFighter("Player2");
    if (p1 && p2) {
        if (p1->getHealth() > 0.f && p2->getHealth() > 0.f) {
            p1->checkPunch(*p2);
            p2->checkPunch(*p1);
        }
        p1->resolveOverlap(*p2);
    }
}

void GameState::draw() {
    using namespace graphics;
    Brush br;
    if (currentScreen == ScreenState::MENU) {
        if (!menuBackgroundPath.empty()) {
            br.outline_opacity = 0.f;
            br.texture = menuBackgroundPath;
            drawRect(400.f, 300.f, 800.f, 600.f, br);
        }
        br.fill_color[0] = br.fill_color[1] = br.fill_color[2] = 1.f;
        drawText(280, 100, 40, "MY MENU", br);
        for (auto* obj : objects) obj->draw();
        drawText(220, 550, 20, "Use mouse to click the button, or ESC to quit.", br);
    }
    else if (currentScreen == ScreenState::GAME) {
        if (!arenaBackgroundPath.empty()) {
            br.outline_opacity = 0.f;
            br.texture = arenaBackgroundPath;
            drawRect(400.f, 300.f, 800.f, 600.f, br);
        }
        for (auto* obj : objects) {
            
            if (dynamic_cast<Fighter*>(obj))
                obj->draw();
        }
    }
}


static GameState* g_gameState = nullptr;

void sgg_update(float ms) {
    float dt = ms * 0.001f;
    using namespace graphics;
    if (!g_gameState || !g_gameState->running) {
        destroyWindow();
        return;
    }
    if (getKeyState(SCANCODE_ESCAPE)) {
        if (g_gameState->currentScreen == ScreenState::MENU) {
            g_gameState->currentScreen = ScreenState::EXIT;
            g_gameState->running = false;
            return;
        }
        else {
            g_gameState->currentScreen = ScreenState::MENU;
            return;
        }
    }
    if (g_gameState->currentScreen == ScreenState::MENU) {
        MouseState m;
        getMouseState(m);
        if (m.button_left_pressed) {
            float mx = (float)m.cur_pos_x;
            float my = (float)m.cur_pos_y;
            for (auto* obj : g_gameState->getObjects()) {
                MenuButton* mb = dynamic_cast<MenuButton*>(obj);
                if (mb && mb->isInside(mx, my) && mb->getName() == "Play") {
                    g_gameState->currentScreen = ScreenState::GAME;
                    Fighter* p1 = g_gameState->getFighter("Player1");
                    Fighter* p2 = g_gameState->getFighter("Player2");
                    if (p1) { p1->setPosition(200.f, 0.f); p1->setHealth(100.f); }
                    if (p2) { p2->setPosition(600.f, 0.f); p2->setHealth(100.f); }
                }
            }
        }
    }
    else if (g_gameState->currentScreen == ScreenState::GAME) {
        g_gameState->update(dt);
    }
    if (g_gameState->currentScreen == ScreenState::EXIT)
        g_gameState->running = false;
}

void sgg_draw() {
    if (g_gameState && g_gameState->running)
        g_gameState->draw();
}

int main() {
    using namespace graphics;
    createWindow(800, 600, "OOP Kombat Arena");
    setUpdateFunction(sgg_update);
    setDrawFunction(sgg_draw);
    setCanvasSize(800, 600);
    setCanvasScaleMode(CANVAS_SCALE_FIT);

    Brush bg;
    bg.fill_color[0] = 0.2f;
    bg.fill_color[1] = 0.2f;
    bg.fill_color[2] = 0.2f;
    setWindowBackground(bg);

    setFont("assets\\myfont.ttf");
    g_gameState = new GameState();
    g_gameState->init();
    playSound("assets\\soundtrack.mp3", 0.5f, true);
    startMessageLoop();
    delete g_gameState;
    g_gameState = nullptr;
    return 0;
}

