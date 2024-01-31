#define GL_SILENCE_DEPRECATION

#include <ctime>
#include <string>
#include <iostream>
#include <cmath>
#include <iostream>
#include <cstdlib>
#include <vector>
#include <string>

#include "fssimplewindow.h"
#include "ysglfontdata.h"
#include "yssimplesound.h"

const int windowWidth = 800;
const int windowHeight = 600;

class Prologue {
public:
    void play(void);
    Prologue(int width, int height) {
        this->width = width;
        this->height = height;
    }
    class Star2D {
    public:
        double x, y;
        double brightness;
        Star2D(double x, double y, double brightness);
    };

private:
    int width, height;
    YsSoundPlayer player;
    YsSoundPlayer::SoundData wav;
    bool wavInitialized;
    void initialize(void);
    void display(void);
    void drawEarth(void);
    void drawMoon(void);
    void drawProjectile(double &x1, double &y1, double &x2, double &y2, double speed);
    std::vector<Star2D> stars;
    int numStars = 100;
    std::string wavFilename = "./demo2.wav";
    const double PI = 3.1415927;
    std::vector<std::string> textsToDisplay = {
        "..we find our place in the cosmos.",
        "Moon's allure fuels exploration"
    };
};


void Prologue::play() {
    initialize();
    double projectile_x1 = (width - 1) + std::cos((35 * PI) / 32.0) * 400;
    double projectile_y1 = (height - 1) + std::sin((35 * PI) / 32.0) * 400;
    
    double projectile_x2 = (width - 1) + std::cos((35.5 * PI) / 32.0) * 400;
    double projectile_y2 = (height - 1) + std::sin((35.5 * PI) / 32.0) * 400;
    
    double speed = 0;
    
    if (wavInitialized) {
        player.Start();
        player.PlayOneShot(wav);
        while (player.IsPlaying(wav) == YSTRUE) {
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            FsPollDevice();
            auto keyIn = FsInkey();
            if (keyIn == FSKEY_ESC) {
                FsCloseWindow();
                exit(0);
            }
            player.KeepPlaying();
            display();
            drawProjectile(projectile_x1, projectile_y1, projectile_x2, projectile_y2, speed);
            auto currentTime = player.GetCurrentPosition(wav);
            if (currentTime > 7) {
                glColor3ub(30, 246, 80);
                glRasterPos2i(width / 2 - 300, height / 2 - 100);
                YsGlDrawFontBitmap16x20(textsToDisplay.at(1).c_str());
            } else {
                glColor3ub(30, 246, 80);
                glRasterPos2i(width / 2 - 300, height / 2 - 100);
                YsGlDrawFontBitmap16x20(textsToDisplay.at(0).c_str());
            }
            FsSwapBuffers();
            speed += 1;
            FsSleep(100);
        }
        player.End();
    }
}

void Prologue::display() {
    FsPollDevice();
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glPointSize(static_cast<int>(rand() % 6));
    glBegin(GL_POINTS);
    for (int i = 0; i < numStars; i++) {
        glColor3d(stars[i].brightness, stars[i].brightness, stars[i].brightness);
        glVertex2d(stars[i].x, stars[i].y);
    }
    glEnd();
    drawEarth();
    drawMoon();
}

void Prologue::drawEarth(void) {
    glBegin(GL_POLYGON);
    for (int i = 32; i <= 48; ++i) {
        double angle = (i * PI) / 32.0;
        double x = (width - 1) + std::cos(angle) * 400;
        double y = (height - 1)+ std::sin(angle) * 400;
        if (i <= 34) {
            glColor3ub(0, 255, 0);
        } else {
            glColor3ub(31, 81, 255);
        }
        glVertex2d(x, y);
    }
    glVertex2d(width - 1, height - 1);
    glEnd();
}

void Prologue::drawMoon() {
    glColor3ub(194, 197, 204);
    glBegin(GL_POLYGON);
    for (int i = 0; i <= 16; ++i) {
        double angle = (i * PI) / 32.0;
        double x = -1 + std::cos(angle) * 150;
        double y = -1 + std::sin(angle) * 150;
        glVertex2d(x, y);
    }
    glVertex2d(-1, -1);
    glEnd();
}

void Prologue::drawProjectile(double &x1, double &y1, double &x2, double &y2, double speed) {
    glColor3ub(240, 230, 140);
    glBegin(GL_POLYGON);
    glVertex2d(x1 - speed, y1 - speed);
    glVertex2d(x1 - speed - 20, y1 - speed - 20);
    glVertex2d(x2 - speed - 20, y2 - speed - 20);
    glVertex2d(x2 - speed, y2 - speed);
    glEnd();
    
    glColor3ub(255,165,0);
    glBegin(GL_LINES);
    glVertex2d(x1 - speed, y1 - speed);
    glVertex2d(x1 - speed + 15, y1 - speed + 15);
    
    glVertex2d((x1 + x2)/2 - speed, (y1 + y2)/2 - speed);
    glVertex2d((x1 + x2)/2 - speed + 15, (y1 + y2)/2 - speed + 15);
    
    glVertex2d(x2 - speed, y2 - speed);
    glVertex2d(x2 - speed + 15, y2 - speed + 15);
    
    glEnd();
}

void Prologue::initialize() {
    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, width - 1, height - 1, 0, -1, 1);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glDisable(GL_DEPTH_TEST);
    glShadeModel(GL_SMOOTH);
    wavInitialized = wav.LoadWav(wavFilename.c_str()) == YSOK;
    stars.clear();
    for (int i = 0; i < numStars; i++) {
        double star_x = (rand() % width);
        double star_y = (rand() % height);
        double star_brightness = (rand() % 100) / 100.0;
        Prologue::Star2D star(star_x, star_y, star_brightness);
        stars.push_back(star);
    }
}


Prologue::Star2D::Star2D(double x, double y, double brightness) {
    this->x = x;
    this->y = y;
    this->brightness = brightness;
}


void set_rnd_seed() {
    srand(static_cast<unsigned int>(time(nullptr)));
}

class MidScene {
public:
    void play(void);
    MidScene(int width, int height) {
        this->width = width;
        this->height = height;
    }
    class Star2D {
    public:
        double x, y;
        double brightness;
        Star2D(double x, double y, double brightness) {
            this->x = x;
            this->y = y;
            this->brightness = brightness;
        }
    };
    class Cloud {
    public:
        void draw();
        Cloud(double x, double y) {
            this->x = x;
            this->y = y;
        }

    private:
        const double PI = 3.1415927;
        void drawCircle(double rx, double ry, double radius);
        double x, y;
    };


private:
    int width, height;
    YsSoundPlayer player;
    YsSoundPlayer::SoundData wav;
    bool wavInitialized;
    void initialize(void);
    void display(double cycle_x, double cycle_y);
    void drawMoon(void);
    void drawRiver(void);
    void drawLand(void);
    void drawMountains(void);
    void drawCloud(void);
    void drawCircle(double rx, double ry, double radius, bool fill);
    void drawHouse(void);
    void drawRectangle(double x1, double y1, double x2, double y2, unsigned char r, unsigned char g, unsigned char b);
    void drawTree(void);
    void drawCycleStickMan(double x, double y);
    std::vector<MidScene::Star2D> stars;
    std::vector<MidScene::Cloud> clouds;
    int numStars = 100;
    int numClouds = 3;
    std::string wavFilename = "./demo3.wav";
    const double PI = 3.1415927;
    std::vector<std::string> textsToDisplay = {
        "1960s India",
        "A man whispers to himself",
        "We'll reach the moon, and beyond as well",
        "Started with Bicycle carrying rocket parts"
    };
};

void MidScene::play() {
    initialize();
    double speed = 0;
    double cycle_x = 300, cycle_y = 500;
    if (wavInitialized) {
        player.Start();
        player.PlayOneShot(wav);
        while (player.IsPlaying(wav) == YSTRUE) {
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            FsPollDevice();
            auto keyIn = FsInkey();
            if (keyIn == FSKEY_ESC) {
                FsCloseWindow();
                exit(0);
            }
            player.KeepPlaying();
            display(cycle_x, cycle_y);
            auto currentTime = player.GetCurrentPosition(wav);
            if (currentTime > 5 && currentTime <= 8) {
                glColor3ub(30, 246, 80);
                glRasterPos2i(width / 2 - 150, height / 2 - 100);
                YsGlDrawFontBitmap16x20(textsToDisplay.at(1).c_str());
            } else if (currentTime > 8 && currentTime <= 13) {
                glColor3ub(30, 246, 80);
                glRasterPos2i(width / 2 - 350, height / 2 - 100);
                YsGlDrawFontBitmap16x20(textsToDisplay.at(2).c_str());
            } else if (currentTime > 13) {
                glColor3ub(30, 246, 80);
                glRasterPos2i(width / 2 - 350, height / 2 - 100);
                YsGlDrawFontBitmap16x20(textsToDisplay.at(3).c_str());
            } else {
                glColor3ub(30, 246, 80);
                glRasterPos2i(width / 2 - 150, height / 2 - 100);
                YsGlDrawFontBitmap16x20(textsToDisplay.at(0).c_str());
            }
            if (currentTime > 15) {
                cycle_x += 3;
            }
            FsSwapBuffers();
            speed += 1;
            FsSleep(100);
        }
        player.End();
    }
}

void MidScene::display(double cycle_x, double cycle_y) {
    FsPollDevice();
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glPointSize(4);
    glBegin(GL_POINTS);
    for (int i = 0; i < numStars; i++) {
        glColor3d(stars[i].brightness, stars[i].brightness, stars[i].brightness);
        glVertex2d(stars[i].x, stars[i].y);
    }
    glEnd();
    drawMoon();
    drawRiver();
    drawLand();
    drawMountains();
    drawCloud();
    drawHouse();
    drawTree();
    drawCycleStickMan(cycle_x, cycle_y);
}

void MidScene::drawRiver() {
    glColor3ub(0, 0, 139);
    glPointSize(8);
    glBegin(GL_POLYGON);
    glVertex2d(0, height);
    glVertex2d(0, height - 50);
    glVertex2d(width, height - 50);
    glVertex2d(width, height);
    glEnd();
}

void MidScene::drawLand() {
    glColor3ub(0, 100, 0);
    glPointSize(8);
    glBegin(GL_POLYGON);
    glVertex2d(0, height - 50);
    glVertex2d(0, height - 350);
    glVertex2d(width, height - 350);
    glVertex2d(width, height - 50);
    glEnd();
}

void MidScene::drawMountains() {
    glColor3ub(139, 69, 19);
    glBegin(GL_TRIANGLES);
    glVertex2d(0, height - 350);
    glVertex2d(150, height - 450);
    glVertex2d(250, height - 350);
    glEnd();
    
    glBegin(GL_TRIANGLES);
    glVertex2d(250, height - 350);
    glVertex2d(500, height - 450);
    glVertex2d(650, height - 350);
    glEnd();
    
    glBegin(GL_TRIANGLES);
    glVertex2d(650, height - 350);
    glVertex2d(730, height - 450);
    glVertex2d(800, height - 350);
    glEnd();
}

void MidScene::drawMoon() {
    glColor3ub(255, 255, 240);
    glBegin(GL_POLYGON);
    for (int i = 0; i <= 64; ++i) {
        double angle = (i * PI) / 32.0;
        double x = width/2 + std::cos(angle) * 50;
        double y = 50 + std::sin(angle) * 50;
        glVertex2d(x, y);
    }
    glVertex2d(-1, -1);
    glEnd();
}

void MidScene::drawCloud() {
    for (Cloud &cloud: clouds) {
        cloud.draw();
    }
}

void MidScene::drawHouse() {
    // base
    drawRectangle(50, 500, 250, 350, 135, 206, 250);
    
    // door
    drawRectangle(100, 500, 200, 420, 119, 136, 153);
    
    // roof
    drawRectangle(50, 350, 250, 300, 222, 184, 135);
    glBegin(GL_TRIANGLES);
    glVertex2d(50, 350);
    glVertex2d(50, 300);
    glVertex2d(30, 350);
    
    glVertex2d(250, 350);
    glVertex2d(250, 300);
    glVertex2d(280, 350);
    glEnd();
    
    // windows
    drawRectangle(60, 400, 100, 370, 255, 255, 102);
    drawRectangle(200, 400, 240, 370, 255, 255, 102);
}

void MidScene::drawTree() {
    drawRectangle(650, 500, 670, 370, 205, 133, 63);
    glColor3ub(34, 139, 34);
    glBegin(GL_TRIANGLES);
    glVertex2d(660, 300);
    glVertex2d(620, 450);
    glVertex2d(700, 450);
    glEnd();
}

void MidScene::drawCycleStickMan(double x, double y) {
    // cycle
    
    // cycle tires
    glColor3ub(100, 100, 100);
    drawCircle(x, y, 35, true);
    drawCircle(x + 100, y, 35, true);
    
    // cycle frame
    glColor3ub(255, 140, 0);
    glBegin(GL_LINE_LOOP);
    glVertex2d(x, y);
    glVertex2d(x + 40, y - 40);
    glVertex2d(x + 100, y - 40);
    glVertex2d(x + 60, y);
    glEnd();
    
    glBegin(GL_LINES);
    glVertex2d(x + 100, y - 40);
    glVertex2d(x + 100, y);
    
    glVertex2d(x + 100, y - 40);
    glVertex2d(x + 100, y - 30);
    
    glVertex2d(x + 60, y);
    glVertex2d(x + 40, y - 40);
    
    glVertex2d(x + 40, y - 40);
    glVertex2d(x, y - 60);
    
    glVertex2d(x + 100, y - 40);
    glVertex2d(x + 100, y - 60);
    glEnd();
    
    drawRectangle(x + 100 - 10, y - 60, x + 100 + 10, y - 70, 0, 0, 0);
    drawRectangle(x - 20, y - 60, x + 20, y - 70, 0, 0, 0);
    drawRectangle(x - 25, y - 70, x + 25, y - 80, 240, 128, 128);
    
    // cone
    glColor3ub(255, 215, 0);
    glBegin(GL_TRIANGLES);
    glVertex2d(x, y - 160);
    glVertex2d(x - 25, y - 80);
    glVertex2d(x + 25, y - 80);
    glEnd();
    
    // stickman
    // Head
    glColor3ub(210, 180, 140);
    drawCircle(x + 65, y - 100, 15, false);

    // Body
    glBegin(GL_LINES);
    glVertex2d(x + 65, y - 85);
    glVertex2d(x + 65, y - 5);
    
    glVertex2d(x + 65, y - 5);
    glVertex2d(x + 75, y + 15);
    
    glVertex2d(x + 65, y - 5);
    glVertex2d(x + 55, y + 15);
    
    glVertex2d(x + 65, y - 55);
    glVertex2d(x + 20, y - 70);

    glVertex2d(x + 65, y - 55);
    glVertex2d(x + 65 + 30, y - 60);
    
    glEnd();
}

void MidScene::drawCircle(double rx, double ry, double radius, bool fill) {
    if (fill) {
        glBegin(GL_POLYGON);
    } else {
        glBegin(GL_LINE_LOOP);
    }
    for (int i = 0; i <= 64; ++i) {
        double angle = (i * PI) / 32.0;
        double x = rx + std::cos(angle) * radius;
        double y = ry + std::sin(angle) * radius;
        glVertex2d(x, y);
    }
    glEnd();
}

void MidScene::drawRectangle(double x1, double y1, double x2, double y2, unsigned char r, unsigned char g, unsigned char b) {
    glColor3ub(r, g, b);
    glBegin(GL_POLYGON);
    glVertex2d(x1, y1);
    glVertex2d(x1, y2);
    glVertex2d(x2, y2);
    glVertex2d(x2, y1);
    glEnd();
}

void MidScene::initialize() {
    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, width - 1, height - 1, 0, -1, 1);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glDisable(GL_DEPTH_TEST);
    glShadeModel(GL_SMOOTH);
    wavInitialized = wav.LoadWav(wavFilename.c_str()) == YSOK;
    stars.clear();
    for (int i = 0; i < numStars; i++) {
        double star_x = (rand() % width);
        double star_y = (rand() % 400);
        double star_brightness = (rand() % 100) / 100.0;
        MidScene::Star2D star(star_x, star_y, star_brightness);
        stars.push_back(star);
    }
    clouds.clear();
    double start_cloudx = 200;
    double start_cloudy = 150;
    for (int i = 0; i < numClouds; ++i) {
        MidScene::Cloud cloud(start_cloudx, start_cloudy - 40);
        clouds.push_back(cloud);
        start_cloudx += 300;
    }
}

void MidScene::Cloud::draw() {
    glColor3ub(255, 255, 255);
    drawCircle(x, y, 30);
    drawCircle(x + 40, y, 40);
    drawCircle(x + 80, y, 30);
    x += 2;
    if (x > 800) {
        x = 0;
    }
}

void MidScene::Cloud::drawCircle(double rx, double ry, double radius) {
    glBegin(GL_POLYGON);
    for (int i = 0; i <= 64; ++i) {
        double angle = (i * PI) / 32.0;
        double x = rx + std::cos(angle) * radius;
        double y = ry + std::sin(angle) * radius;
        glVertex2d(x, y);
    }
    glEnd();
}

class Epilogue {
public:
    void play(void);
    Epilogue(int width, int height) {
        this->width = width;
        this->height = height;
    }
    class Star2D {
    public:
        double x, y;
        double brightness;
        Star2D(double x, double y, double brightness) {
            this->x = x;
            this->y = y;
            this->brightness = brightness;
        }
    };
    
private:
    int width, height;
    YsSoundPlayer player;
    YsSoundPlayer::SoundData wav;
    bool wavInitialized;
    void initialize(void);
    void display1(double rocket_x, double rocket_y);
    void display2(double rocket_x, double rocket_y);
    void drawBuildings();
    void drawRectangle(double x1, double y1, double x2, double y2, unsigned char r, unsigned char g, unsigned char b);
    void drawRocket(double rocket_x, double rocket_y);
    void drawCircle(double rx, double ry, double radius, bool fill);
    void drawEarth(void);
    void drawThrusters(double rocket_x, double rocket_y);
    std::string wavFilename = "./demo4.wav";
    std::vector<Epilogue::Star2D> stars;
    int numStars = 100;
    const double PI = 3.1415927;
    std::vector<std::string> textsToDisplay = {
        "July 14, 2023",
        "India launched Chandrayan III",
        "Against all odds, India soars into the cosmos!",
        "From Earth to the Moon, dreams do come true.",
        "India fourth to moon landing.",
        "And, 1st to land near lunar south pole"
    };
};

void Epilogue::play() {
    initialize();
    double rocket_x = 400, rocket_y = 420;
    bool flag = true;
    if (wavInitialized) {
        player.Start();
        player.PlayOneShot(wav);
        while (player.IsPlaying(wav) == YSTRUE) {
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            FsPollDevice();
            auto keyIn = FsInkey();
            if (keyIn == FSKEY_ESC) {
                FsCloseWindow();
                exit(0);
            }
            player.KeepPlaying();
            if (flag) {
                display1(rocket_x, rocket_y);
            } else {
                display2(rocket_x, rocket_y);
            }
            auto currentTime = player.GetCurrentPosition(wav);
            if (currentTime > 3 && currentTime <= 5) {
                glColor3ub(30, 246, 80);
                glRasterPos2i(width / 2 - 150, height - 100);
                YsGlDrawFontBitmap16x20(textsToDisplay.at(0).c_str());
            } else if (currentTime > 5 && currentTime <= 8) {
                glColor3ub(30, 246, 80);
                glRasterPos2i(width / 2 - 150, height - 100);
                YsGlDrawFontBitmap16x20(textsToDisplay.at(1).c_str());
            } else if (currentTime > 8 && currentTime <= 18) {
                glColor3ub(30, 246, 80);
                glRasterPos2i(width/2 - 350, height - 100);
                YsGlDrawFontBitmap16x20(textsToDisplay.at(2).c_str());
            } else if (currentTime > 18 && currentTime <= 20) {
                glColor3ub(30, 246, 80);
                glRasterPos2i(width / 2 - 350, height - 400);
                YsGlDrawFontBitmap16x20(textsToDisplay.at(3).c_str());
            } else if (currentTime > 20 && currentTime <= 25) {
                glColor3ub(30, 246, 80);
                glRasterPos2i(width / 2 - 350, height - 400);
                YsGlDrawFontBitmap16x20(textsToDisplay.at(4).c_str());
            } else if (currentTime > 25) {
                glColor3ub(30, 246, 80);
                glRasterPos2i(width / 2 - 350, height - 400);
                YsGlDrawFontBitmap16x20(textsToDisplay.at(5).c_str());
            }
            FsSwapBuffers();
            if (flag) {
                rocket_y -= 3;
            } else {
                if (rocket_y <= height - 100) {
                    rocket_y += 7;
                }
            }
            if (rocket_y < -80) {
                flag = false;
            }
            FsSleep(100);
        }
        player.End();
    }
}

void Epilogue::display1(double rocket_x, double  rocket_y) {
    glClearColor(0.15, 0.15, 0.15, 1.0);
    glPointSize(4);
    glBegin(GL_POINTS);
    for (int i = 0; i < numStars; i++) {
        glColor3d(stars[i].brightness, stars[i].brightness, stars[i].brightness);
        glVertex2d(stars[i].x, stars[i].y);
    }
    glEnd();
    // draw road
    glColor3ub(205, 133, 63);
    glBegin(GL_POLYGON);
    glVertex2d(0, 600);
    glVertex2d(0, 450);
    glVertex2d(800, 450);
    glVertex2d(800, 600);
    glEnd();
    
    // draw buildings
    drawBuildings();
        
    // holder
    drawRectangle(550, 450, 580, 300, 30, 144, 255);
    drawRectangle(490, 320, 550, 300, 30, 144, 255);
    
    // draw rocket
    drawRocket(rocket_x, rocket_y);
    
    // draw thrusters
    drawThrusters(rocket_x, rocket_y);
}

void Epilogue::display2(double rocket_x, double  rocket_y) {
    glClearColor(0.15, 0.15, 0.15, 1.0);
    glPointSize(4);
    glBegin(GL_POINTS);
    for (int i = 0; i < numStars; i++) {
        glColor3d(stars[i].brightness, stars[i].brightness, stars[i].brightness);
        glVertex2d(stars[i].x, stars[i].y);
    }
    glEnd();
    
    // draw moon
    glColor3ub(255, 255, 240);
    glBegin(GL_POLYGON);
    for (int i = 32; i <= 64; ++i) {
        double angle = (i * PI) / 32.0;
        double x = width/2 + std::cos(angle) * width;
        double y = height + 650 + std::sin(angle) * width;
        glVertex2d(x, y);
    }
    glEnd();
    drawRocket(rocket_x, rocket_y);
    drawThrusters(rocket_x, rocket_y);
    drawEarth();
}

void Epilogue::drawEarth() {
    glBegin(GL_POLYGON);
    for (int i = 0; i <= 64; ++i) {
        double angle = (i * PI) / 32.0;
        double x = (width - 100) + std::cos(angle) * 50;
        double y = (50)+ std::sin(angle) * 50;
        if (i <= 4) {
            glColor3ub(0, 255, 0);
        } else {
            glColor3ub(31, 81, 255);
        }
        glVertex2d(x, y);
    }
    glEnd();
}

void Epilogue::drawRocket(double rocket_x, double rocket_y) {
    // rocket
    drawRectangle(rocket_x, rocket_y, rocket_x + 100, rocket_y - 120, 205, 92, 92);
    
    glBegin(GL_TRIANGLES);
    glVertex2d(rocket_x, rocket_y - 120);
    glVertex2d(rocket_x + 50, rocket_y - 120 - 50);
    glVertex2d(rocket_x + 100, rocket_y - 120);
    glEnd();
    
    glColor3ub(255, 165, 0);
    drawCircle(rocket_x + 50, rocket_y - 100, 10, true);
    
    glColor3ub(255, 255, 255);
    drawCircle(rocket_x + 50, rocket_y - 60, 10, true);
    
    glColor3ub(0, 128, 0);
    drawCircle(rocket_x + 50, rocket_y - 20, 10, true);
    
    // fins
    glColor3ub(255, 255, 255);
    glBegin(GL_POLYGON);
    glVertex2d(rocket_x, rocket_y - 100);
    glVertex2d(rocket_x, rocket_y - 10);
    glVertex2d(rocket_x - 10, rocket_y - 10);
    glVertex2d(rocket_x - 10, rocket_y - 100 + 10);
    glEnd();
    
    glColor3ub(255, 255, 255);
    glBegin(GL_POLYGON);
    glVertex2d(rocket_x + 100, rocket_y - 100);
    glVertex2d(rocket_x + 100, rocket_y - 10);
    glVertex2d(rocket_x + 100 + 10, rocket_y - 10);
    glVertex2d(rocket_x + 100 + 10, rocket_y - 100 + 10);
    glEnd();
    
    glColor3ub(0, 0, 0);
    glBegin(GL_POLYGON);
    glVertex2d(rocket_x, rocket_y);
    glVertex2d(rocket_x - 10, rocket_y + 10);
    glVertex2d(rocket_x + 110, rocket_y + 10);
    glVertex2d(rocket_x + 100, rocket_y);
    glEnd();
    
    // drawing boundary
    glColor3ub(0, 0, 0);
    glBegin(GL_LINE_LOOP);
    glVertex2d(rocket_x, rocket_y);
    glVertex2d(rocket_x, rocket_y - 120);
    glVertex2d(rocket_x + 100, rocket_y - 120);
    glVertex2d(rocket_x + 100, rocket_y);
    glEnd();
    
    glBegin(GL_LINE_LOOP);
    glVertex2d(rocket_x, rocket_y - 120);
    glVertex2d(rocket_x + 50, rocket_y - 120 - 50);
    glVertex2d(rocket_x + 100, rocket_y - 120);
    glEnd();
    
    glBegin(GL_LINE_LOOP);
    glVertex2d(rocket_x, rocket_y - 100);
    glVertex2d(rocket_x, rocket_y - 10);
    glVertex2d(rocket_x - 10, rocket_y - 10);
    glVertex2d(rocket_x - 10, rocket_y - 100 + 10);
    glEnd();
    
    glBegin(GL_LINE_LOOP);
    glVertex2d(rocket_x + 100, rocket_y - 100);
    glVertex2d(rocket_x + 100, rocket_y - 10);
    glVertex2d(rocket_x + 100 + 10, rocket_y - 10);
    glVertex2d(rocket_x + 100 + 10, rocket_y - 100 + 10);
    glEnd();
}

void Epilogue::drawThrusters(double rocket_x, double rocket_y) {
    glBegin(GL_LINES);
    for (int i = 0; i <= 100; i+=10) {
        if (i % 2 == 0) {
            glColor3ub(255, 215, 0);
        } else {
            glColor3ub(255, 140, 0);
        }
        glVertex2d(rocket_x + i, rocket_y);
        glVertex2d(rocket_x + i, rocket_y + 40);
    }
    glEnd();
}

void Epilogue::drawCircle(double rx, double ry, double radius, bool fill) {
    if (fill) {
        glBegin(GL_POLYGON);
    } else {
        glBegin(GL_LINE_LOOP);
    }
    for (int i = 0; i <= 64; ++i) {
        double angle = (i * PI) / 32.0;
        double x = rx + std::cos(angle) * radius;
        double y = ry + std::sin(angle) * radius;
        glVertex2d(x, y);
    }
    glEnd();
}

void Epilogue::drawBuildings() {
    // building 1
    drawRectangle(10, 450, 90, 250, 100, 100, 100);
    
    drawRectangle(15, 300, 25, 290, 173, 216, 230);
    drawRectangle(35, 300, 45, 290, 173, 216, 230);
    drawRectangle(55, 300, 65, 290, 173, 216, 230);
    drawRectangle(75, 300, 85, 290, 173, 216, 230);
    
    drawRectangle(15, 320, 25, 310, 173, 216, 230);
    drawRectangle(35, 320, 45, 310, 173, 216, 230);
    drawRectangle(55, 320, 65, 310, 173, 216, 230);
    drawRectangle(75, 320, 85, 310, 173, 216, 230);
    
    drawRectangle(15, 350, 25, 340, 173, 216, 230);
    drawRectangle(35, 350, 45, 340, 173, 216, 230);
    drawRectangle(55, 350, 65, 340, 173, 216, 230);
    drawRectangle(75, 350, 85, 340, 173, 216, 230);
    
    drawRectangle(15, 380, 25, 370, 173, 216, 230);
    drawRectangle(35, 380, 45, 370, 173, 216, 230);
    drawRectangle(55, 380, 65, 370, 173, 216, 230);
    drawRectangle(75, 380, 85, 370, 173, 216, 230);
    
    drawRectangle(15, 410, 25, 400, 173, 216, 230);
    drawRectangle(35, 410, 45, 400, 173, 216, 230);
    drawRectangle(55, 410, 65, 400, 173, 216, 230);
    drawRectangle(75, 410, 85, 400, 173, 216, 230);
    
    // building 2
    drawRectangle(75, 450, 160, 150, 100, 100, 100);
    
    drawRectangle(85, 200, 95, 190, 173, 216, 230);
    drawRectangle(105, 200, 115, 190, 173, 216, 230);
    drawRectangle(125, 200, 135, 190, 173, 216, 230);
    drawRectangle(145, 200, 155, 190, 173, 216, 230);
    
    drawRectangle(85, 240, 95, 220, 255, 223, 0);
    drawRectangle(105, 240, 115, 220, 255, 223, 0);
    drawRectangle(125, 240, 135, 220, 255, 223, 0);
    drawRectangle(145, 240, 155, 220, 255, 223, 0);
    
    drawRectangle(85, 260, 95, 250, 173, 216, 230);
    drawRectangle(105, 260, 115, 250, 173, 216, 230);
    drawRectangle(125, 260, 135, 250, 173, 216, 230);
    drawRectangle(145, 260, 155, 250, 173, 216, 230);
    
    drawRectangle(85, 300, 95, 290, 173, 216, 230);
    drawRectangle(105, 300, 115, 290, 173, 216, 230);
    drawRectangle(125, 300, 135, 290, 173, 216, 230);
    drawRectangle(145, 300, 155, 290, 173, 216, 230);
    
    drawRectangle(85, 320, 95, 310, 173, 216, 230);
    drawRectangle(105, 320, 115, 310, 173, 216, 230);
    drawRectangle(125, 320, 135, 310, 173, 216, 230);
    drawRectangle(145, 320, 155, 310, 173, 216, 230);
    
    drawRectangle(85, 350, 95, 340, 173, 216, 230);
    drawRectangle(105, 350, 115, 340, 173, 216, 230);
    drawRectangle(125, 350, 135, 340, 173, 216, 230);
    drawRectangle(145, 350, 155, 340, 173, 216, 230);
    
    drawRectangle(85, 380, 95, 370, 173, 216, 230);
    drawRectangle(105, 380, 115, 370, 173, 216, 230);
    drawRectangle(125, 380, 135, 370, 173, 216, 230);
    drawRectangle(145, 380, 155, 370, 173, 216, 230);
}

void Epilogue::drawRectangle(double x1, double y1, double x2, double y2, unsigned char r, unsigned char g, unsigned char b) {
    glColor3ub(r, g, b);
    glBegin(GL_POLYGON);
    glVertex2d(x1, y1);
    glVertex2d(x1, y2);
    glVertex2d(x2, y2);
    glVertex2d(x2, y1);
    glEnd();
}

void Epilogue::initialize() {
    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, width - 1, height - 1, 0, -1, 1);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glDisable(GL_DEPTH_TEST);
    glShadeModel(GL_SMOOTH);
    wavInitialized = wav.LoadWav(wavFilename.c_str()) == YSOK;
    
    stars.clear();
    for (int i = 0; i < numStars; i++) {
        double star_x = (rand() % width);
        double star_y = (rand() % height);
        double star_brightness = (rand() % 100) / 100.0;
        Epilogue::Star2D star(star_x, star_y, star_brightness);
        stars.push_back(star);
    }
}

class Intro {
public:
    void play(void);
    Intro(int width, int height) {
        this->width = width;
        this->height = height;
    }
    class Star3D {
    public:
        double x, y, z;
        double speed;
        double brightness;
        Star3D(double x, double y, double z, double speed, double brightness) {
            this->x = x;
            this->y = y;
            this->z = z;
            this->speed = speed;
            this->brightness = brightness;
        }
    };
    
private:
    int width, height;
    YsSoundPlayer player;
    YsSoundPlayer::SoundData wav;
    bool wavInitialized;
    void initialize(void);
    void display(void);
    void setUp2D(void);
    std::vector<Star3D> stars;
    int numStars = 1000;
    std::string wavFilename = "./demo1.wav";
    std::vector<std::string> textsToDisplay = {
        "In a universe where dreams reach",
        "beyond the stars..."
    };
};

void Intro::play() {
    initialize();
    if (wavInitialized) {
        player.Start();
        player.PlayOneShot(wav);
        while (player.IsPlaying(wav) == YSTRUE) {
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            FsPollDevice();
            auto keyIn = FsInkey();
            if (keyIn == FSKEY_ESC) {
                FsCloseWindow();
                exit(0);
            }
            player.KeepPlaying();
            display();
            setUp2D();
            glColor3ub(30, 246, 80);
            glRasterPos2i(width / 2 - 250, height / 2 - 100);
            YsGlDrawFontBitmap16x20(textsToDisplay.at(0).c_str());
            glRasterPos2i(width / 2 - 150, height / 2);
            YsGlDrawFontBitmap16x20(textsToDisplay.at(1).c_str());
            FsSwapBuffers();
            FsSleep(20);
        }
        player.End();
    }
}

void Intro::display() {
    glEnable(GL_DEPTH_TEST);
    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60, static_cast<double>(width) / height, 1, 1000);
    glMatrixMode(GL_MODELVIEW);
    
    FsPollDevice();
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glPointSize(4);
    glBegin(GL_POINTS);
    for (int i = 0; i < numStars; i++) {
        stars[i].z += stars[i].speed;
        if (stars[i].z > 250) {
            stars[i].z = -250;
        }
        glColor3d(stars[i].brightness, stars[i].brightness, stars[i].brightness);
        glVertex3d(stars[i].x, stars[i].y, stars[i].z);
    }
    glEnd();
}

void Intro::setUp2D(void) {
    int wid,hei;
    FsGetWindowSize(wid,hei);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0,(float)wid-1,(float)hei-1,0,-1,1);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glDisable(GL_DEPTH_TEST);
}

void Intro::initialize() {
    wavInitialized = wav.LoadWav(wavFilename.c_str()) == YSOK;
    stars.clear();
    for (int i = 0; i < numStars; i++) {
        double star_x = (rand() % width) - (width / 2);
        double star_y = (rand() % height) - (height / 2);
        double star_z = 250 - (rand() % 500);
        double star_speed = 1.0 + (rand() % 10) / 10.0;
        double star_brightness = (rand() % 100) / 100.0;
        Intro::Star3D star(star_x, star_y, star_z, star_speed, star_brightness);
        stars.push_back(star);
    }
}


int main(void) {
    FsChangeToProgramDir();
    FsOpenWindow(0, 0, windowWidth,windowHeight, 1);
    Intro intro(windowWidth, windowHeight);
    Prologue prologue(windowWidth, windowHeight);
    MidScene midscene(windowWidth, windowHeight);
    Epilogue epilogue(windowWidth, windowHeight);
    
    intro.play();
    prologue.play();
    midscene.play();
    epilogue.play();
    
    FsCloseWindow();
    return 0;
}
