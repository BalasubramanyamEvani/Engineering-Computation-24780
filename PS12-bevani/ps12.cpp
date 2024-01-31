#define GL_SILENCE_DEPRECATION

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include <vector>

#include "fssimplewindow.h"

// (1) Use virtual, override, and const keywords correctly.
// (2) Make Move, Draw, and KeyIn functions of the base classes (GameObject and KeyResponder) pure virtual function.
// (3) in function void ShootingGame::KeyIn(int key), identify if the ptr is a kind of KeyResponder, and send a key code if it is.

class GameObject {
   public:
    int x, y;
    virtual void Move(void) = 0;
    virtual void Draw(void) const = 0;
};

class GameObjectWithState : public GameObject {
   public:
    int state = 0;
};

class KeyResponder {
   public:
    virtual void KeyIn(const int key) = 0;
};

class Missile : public GameObjectWithState {
   public:
    int v = -10;
    void Shoot(int sx, int sy);
    void Disappear(void);

    void Move(void) override;
    void Draw(void) const override;
};

void Missile::Shoot(int sx, int sy) {
    state = 1;
    x = sx;
    y = sy - 10;
}

void Missile::Disappear(void) {
    state = 0;
}

void Missile::Move(void) {
    if (0 != state) {
        y += v;
        if (y < 0) {
            Disappear();
        }
    }
}
void Missile::Draw(void) const {
    if (0 != state) {
        glColor3f(1, 0, 0);
        glBegin(GL_LINES);
        glVertex2i(x, y);
        glVertex2i(x, y + 10);
        glEnd();
    }
}

class Target : public GameObjectWithState {
   public:
    int v, w, h;

    Target() {
        state = 1;
    }
    void Initialize(void);
    void Disappear(void);
    bool CheckCollision(const Missile m) const;

    void Move(void) override;
    void Draw(void) const override;
};

void Target::Initialize(void) {
    x = 0;
    y = 100 + rand() % 20;
    v = rand() % 10 + 10;
    w = 100;
    h = 20;
    state = 1;
}

void Target::Move(void) {
    if (0 != state) {
        x += v;
        if (800 < x) {
            x = 0;
        }
    }
}

void Target::Disappear(void) {
    state = 0;
}

void Target::Draw(void) const {
    if (0 != state) {
        glColor3f(0, 1, 1);
        glBegin(GL_QUADS);
        glVertex2i(x, y);
        glVertex2i(x + w, y);
        glVertex2i(x + w, y + h);
        glVertex2i(x, y + h);
        glEnd();
    }
}

bool Target::CheckCollision(const Missile m) const {
    return (
        x <= m.x && m.x <= x + w &&
        y <= m.y && m.y <= y + h);
}

class Explosion : public GameObjectWithState {
   public:
    enum {
        NUM_PARTICLES = 64
    };
    class Particle {
       public:
        unsigned char r, g, b;
        int x, y, vx, vy;
    };
    int step = 0;
    std::vector<Particle> particles;
    void Explode(int x, int y);
    void Move(void) override;
    void Draw(void) const override;
};

void Explosion::Explode(int x, int y) {
    step = 0;
    state = 1;
    particles.resize(NUM_PARTICLES);
    for (auto &p : particles) {
        p.x = x;
        p.y = y;
        p.vx = rand() % 11 - 5;
        p.vy = rand() % 11 - 5;
        p.r = rand() % 256;
        p.g = rand() % 256;
        p.b = rand() % 256;
    }
}

void Explosion::Move(void) {
    if (0 != state) {
        for (auto &p : particles) {
            p.x += p.vx;
            p.y += p.vy;
        }
        ++step;
        if (30 < step) {
            state = 0;
        }
    }
}

void Explosion::Draw(void) const {
    if (0 != state) {
        glBegin(GL_POINTS);
        for (auto p : particles) {
            glColor3ub(p.r, p.g, p.b);
            glVertex2i(p.x, p.y);
        }
        glEnd();
    }
}

class Player : public GameObject, public KeyResponder {
   public:
    Player() {
        x = 400;
        y = 550;
    }

    void KeyIn(const int key) override;
    void Move(void) override;
    void Draw(void) const override;
};

void Player::KeyIn(const int key) {
    switch (key) {
        case FSKEY_LEFT:
            x -= 15;
            if (x < 0) {
                x = 800;
            }
            break;
        case FSKEY_RIGHT:
            x += 15;
            if (800 < x) {
                x = 0;
            }
            break;
            // Space key requires a knowledge of the missile status.
            // ShootingGame class knows both player and missile status.
            // Therefore processed by ShootingGame class.
    }
}

void Player::Move(void) {
    // Player moves in response to key strokes.
    // Therefore, nothing needs to be done in here.
}

void Player::Draw(void) const {
    glColor3f(0, 1, 0);

    glBegin(GL_QUADS);

    glVertex2i(x - 15, y);
    glVertex2i(x + 15, y);
    glVertex2i(x + 15, y - 10);
    glVertex2i(x - 15, y - 10);

    glVertex2i(x - 5, y - 20);
    glVertex2i(x - 5, y);
    glVertex2i(x + 5, y);
    glVertex2i(x + 5, y - 20);

    glEnd();
}

class ShootingGame {
   public:
    enum {
        nTargets = 5,
        nMissiles = 3,
        nExplosions = 5,  // Cannot be greater than nTargets anyway.
    };

    Player player;
    Missile m[nMissiles];
    Target t[nTargets];
    Explosion explosions[nExplosions];
    int endingSequence = 0;

    std::vector<GameObject *> objects;

    void Initialize(void);
    void RunOneStep(int key);
    void Move(void);
    void Draw(void) const;
    void KeyIn(const int key);
    void ProcessCollision(void);
    void StartExplosion(int x, int y);
    unsigned int GetNumberOfTargetsAlive(void) const;
    unsigned int GetNumberOfExplosionsActive(void) const;
};

void ShootingGame::Initialize(void) {
    for (auto &T : t) {
        T.Initialize();
        objects.push_back(&T);
    }
    for (auto &M : m) {
        objects.push_back(&M);
    }
    for (auto &X : explosions) {
        objects.push_back(&X);
    }
    objects.push_back(&player);
}

void ShootingGame::RunOneStep(int key) {
    KeyIn(key);
    Move();
    ProcessCollision();
}

void ShootingGame::Move(void) {
    for (auto ptr : objects) {
        ptr->Move();
    }
    if (0 < endingSequence && endingSequence < 200) {
        ++endingSequence;
        if (0 == endingSequence % 8) {
            StartExplosion(rand() % 800, rand() % 400);
        }
    }
}

void ShootingGame::Draw(void) const {
    for (auto ptr : objects) {
        ptr->Draw();
    }
}

void ShootingGame::KeyIn(const int key) {
    if (FSKEY_SPACE == key) {
        for (auto &M : m) {
            if (0 == M.state) {
                M.Shoot(player.x, player.y);
                break;
            }
        }
    }
    for (auto ptr : objects) {
        // Send key stroke only if a class can take a key stroke.
        // Check if ptr is a kind of KeyResponder class, and if so
        // send the key stroke by KeyIn function.
         auto keyResponder = dynamic_cast<KeyResponder *>(ptr);
         if(nullptr!=keyResponder)
         {
             keyResponder->KeyIn(key);
         }
    }
}

void ShootingGame::ProcessCollision(void) {
    for (auto &T : t) {
        for (auto &M : m) {
            if (0 != M.state && 0 != T.state && true == T.CheckCollision(M)) {
                T.Disappear();
                M.Disappear();

                if (0 == GetNumberOfTargetsAlive()) {
                    endingSequence = 1;
                }
                StartExplosion(M.x, M.y);

                printf("Hit!\n");
            }
        }
    }
}

void ShootingGame::StartExplosion(int x, int y) {
    for (auto &X : explosions) {
        if (0 == X.state) {
            X.Explode(x, y);
            return;
        }
    }
}

unsigned int ShootingGame::GetNumberOfTargetsAlive(void) const {
    int nAlive = 0;
    for (auto &T : t) {
        nAlive += T.state;
    }
    return nAlive;
}

unsigned int ShootingGame::GetNumberOfExplosionsActive(void) const {
    int n = 0;
    for (auto &X : explosions) {
        n += X.state;
    }
    return n;
}

int main(void) {
    srand(time(NULL));

    ShootingGame game;
    game.Initialize();

    FsOpenWindow(0, 0, 800, 600, 1);
    glPointSize(2);
    for (;;) {
        FsPollDevice();
        auto key = FsInkey();
        if (FSKEY_ESC == key) {
            break;
        }

        game.RunOneStep(key);
        if (0 == game.GetNumberOfTargetsAlive() &&
            0 == game.GetNumberOfExplosionsActive()) {
            break;
        }

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        game.Draw();
        FsSwapBuffers();

        FsSleep(10);
    }
    return 0;
}
