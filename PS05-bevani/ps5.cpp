#define GL_SILENCE_DEPRECATION

#include <time.h>
#include <iostream>
#include <cmath>
#include <cstdlib>
#include <sstream>
#include "fssimplewindow.h"
#include "ysglfontdata.h"

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 800
#define SCALE_FACTOR 10

const double PI = 3.1415927;
int NBalls = 25;
const int NStars = 300;
const int OBSTACLE_RADIUS = 20;
const int NBeams = 3;

typedef struct beam {
    double x;
    double y;
    double radius;
    bool launched;
} beam_t;

typedef struct spaceship {
    int sx;
    int sy;
    beam_t beam[NBeams];
    bool hit;
} spaceship_t;

typedef struct obstacle {
    double x;
    double y;
    double vx;
    double vy;
    double radius;
    bool destroyed;
} obstacle_t;

typedef struct star {
    int x;
    int y;
    uint8_t color_r;
    uint8_t color_g;
    uint8_t color_b;
} star_t;

obstacle_t obstacles[50];
star_t stars[NStars];

void draw_spaceship(spaceship_t &spaceship, bool spaceship_hit_state) {
    int sx = spaceship.sx * SCALE_FACTOR;
    int sy = SCREEN_HEIGHT - spaceship.sy * SCALE_FACTOR;
    
    if (!spaceship_hit_state) {
        glColor3ub(192, 192, 192);
    }
    
    glBegin(GL_TRIANGLE_FAN);
    if (spaceship_hit_state) {
        glColor3ub(150 + rand() % 106, 150 + rand() % 106, 150 + rand() % 106);
    }
    glVertex2i(sx, sy);
    
    if (spaceship_hit_state) {
        glColor3ub(150 + rand() % 106, 150 + rand() % 106, 150 + rand() % 106);
    }
    glVertex2i(sx - 4 * SCALE_FACTOR, sy + 2 * SCALE_FACTOR);
    
    if (spaceship_hit_state) {
        glColor3ub(150 + rand() % 106, 150 + rand() % 106, 150 + rand() % 106);
    }
    glVertex2i(sx, sy - 5 * SCALE_FACTOR);
    
    if (spaceship_hit_state) {
        glColor3ub(150 + rand() % 106, 150 + rand() % 106, 150 + rand() % 106);
    }
    glVertex2i(sx + 4 * SCALE_FACTOR, sy + 2 * SCALE_FACTOR);
    glEnd();
}

void draw_star(star_t &star) {
    glColor3ub(star.color_r, star.color_g, star.color_b);
    glPointSize(4);
    glBegin(GL_POINTS);
    glVertex2i(star.x, star.y);
    glEnd();
}

void draw_background() {
    glBegin(GL_QUADS);
    glColor3ub(61, 55, 55);
    glVertex2i(0, 0);
    
    glColor3ub(61, 55, 55);
    glVertex2i(SCREEN_WIDTH, 0);
    
    glColor3ub(92, 86, 86);
    glVertex2i(SCREEN_WIDTH, SCREEN_HEIGHT);
    
    glColor3ub(92, 86, 86);
    glVertex2i(0, SCREEN_HEIGHT);
    glEnd();
}

void init_stars() {
    for (int i = 0; i < NStars; ++i) {
        stars[i].x = rand() % SCREEN_WIDTH;
        stars[i].y = rand() % SCREEN_HEIGHT;
        stars[i].color_r = rand() % 156 + 100;
        stars[i].color_g = rand() % 156 + 100;
        stars[i].color_b = rand() % 156 + 100;
    }
}

void draw_stars(bool spaceship_hit_state) {
    for (int i = 0; i < NStars; ++i) {
        if (!spaceship_hit_state) {
            stars[i].y += 4;
            if (stars[i].y >= SCREEN_HEIGHT) {
                stars[i].y = -1;
            }
        }
        draw_star(stars[i]);
    }
}

void init_obstacles() {
    NBalls = 25 + rand() % 26;
    for (int i = 0; i < NBalls; ++i) {
        obstacles[i].x = 20 + rand() % (SCREEN_WIDTH / 10 - 20);
        obstacles[i].y = -rand() % 20;
        
        obstacles[i].vx = -10 + rand() % 21;
        obstacles[i].vy = rand() % 11 + 10;
        obstacles[i].radius = OBSTACLE_RADIUS;
        obstacles[i].destroyed = false;
    }
}

void draw_obstacle(obstacle_t obstacle) {
    glColor3ub(255, 69, 0);
    glBegin(GL_POLYGON);
    for (int i = 0; i < 64; ++i) {
        double angle = (i * PI) / 32.0;
        double x = obstacle.x * SCALE_FACTOR + std::cos(angle) * obstacle.radius;
        double y = obstacle.y * SCALE_FACTOR + std::sin(angle) * obstacle.radius;
        
        if (i % 4 == 0) {
            glColor3ub(255, 102, 0);
        } else {
            glColor3ub(255, 69, 0);
        }
        
        glVertex2d(x, y);
    }
    glEnd();
}

bool check_obstacle_collision(obstacle_t &obstacle1, obstacle_t &obstacle2) {
    double dx = obstacle2.x - obstacle1.x;
    double dy = obstacle2.y - obstacle1.y;
    double d = std::sqrt(dx * dx + dy * dy);
    if(d < 2.0) {
        double vx = obstacle2.vx - obstacle1.vx;
        double vy = obstacle2.vy - obstacle1.vy;

        double dot = dx * vx + dy * vy;
        return dot < 0.0;
    }
    return false;
}

bool check_beam_hit(beam_t &beam, obstacle_t &obstacle1) {
    double dx = beam.x - obstacle1.x * SCALE_FACTOR;
    double dy = beam.y - obstacle1.y * SCALE_FACTOR;
    return std::sqrt(dx * dx + dy * dy) <= (obstacle1.radius + beam.radius);
}

void obstacle_bounce(obstacle_t &obstacle1, obstacle_t &obstacle2) {
    double nx = obstacle2.x - obstacle1.x;
    double ny = obstacle2.y - obstacle1.y;
    double d = std::sqrt(nx * nx + ny * ny);
    if(d != 0) {
        nx /= d;
        ny /= d;

        double k1 = obstacle1.x * nx + obstacle1.y * ny;
        double k2 = obstacle2.x * nx + obstacle2.y * ny;

        obstacle1.x += nx * (k2 - k1);
        obstacle1.y += ny * (k2 - k1);

        obstacle2.x += nx * (k1 - k2);
        obstacle2.y += ny * (k1 - k2);
    }
}

void draw_obstacles(double dt, beam_t *beams) {
    for (int i = 0; i < NBalls; ++i) {
        if (!obstacles[i].destroyed) {
            obstacles[i].x += obstacles[i].vx * dt;
            obstacles[i].y += obstacles[i].vy * dt;
            if (obstacles[i].y * SCALE_FACTOR >= SCREEN_HEIGHT || obstacles[i].x * SCALE_FACTOR < 0 || obstacles[i].x * SCALE_FACTOR > SCREEN_WIDTH) {
                obstacles[i].destroyed = true;
            }
            
            for(int j = 0; j < NBalls; ++j) {
                if(i != j && check_obstacle_collision(obstacles[i], obstacles[j])) {
                    obstacle_bounce(obstacles[i], obstacles[j]);
                }
            }
            
            for (int j = 0; j < NBeams; ++j) {
                if (beams[j].launched && check_beam_hit(beams[j], obstacles[i])) {
                    obstacles[i].destroyed = true;
                    break;
                }
            }
            
            if (!obstacles[i].destroyed) {
                draw_obstacle(obstacles[i]);
            }
        }
    }
}

bool all_obstacles_gone() {
    int cntr = NBalls;
    for (int i = 0; i < NBalls; ++i) {
        if (obstacles[i].destroyed) {
            cntr -= 1;
        }
    }
    return cntr == 0;
}

bool spaceship_boundar_hit(int x1, int y1, int x2, int y2, int a, int b, int c, double cx, double cy, double radius) {
    double dist = (std::abs(a * cx + b * cy + c)) / (std::sqrt(a * a + b * b) + 1e-9);
    int dx = x2 - x1;
    int dy = y2 - y1;
    double dotAB = (double)dx * dx + (double)dy * dy;
    double ac_x = cx - x1;
    double ac_y = cy - y1;
    double dotAC = dx * ac_x + dy * ac_y;
    return radius >= dist && ((dotAC == 0 || dotAC == dotAB) || (dotAC > 0 && dotAC < dotAB));
}

bool check_spaceship_hit(spaceship_t &spaceship) {
    int sx = spaceship.sx * SCALE_FACTOR;
    int sy = SCREEN_HEIGHT - spaceship.sy * SCALE_FACTOR;
    
    int xs[4] = {sx - 4 * SCALE_FACTOR, sx, sx + 4 * SCALE_FACTOR, sx};
    int ys[4] = {sy + 2 * SCALE_FACTOR, sy, sy + 2 * SCALE_FACTOR, sy - 5 * SCALE_FACTOR};
    
    for (int i = 0; i < NBalls; ++i) {
        if (!obstacles[i].destroyed) {
            for (int j = 0; j < 4; ++j) {
                int x1 = xs[j];
                int y1 = ys[j];
                
                int x2 = xs[(j + 1) %  4];
                int y2 = ys[(j + 1) %  4];
                
                int a = y1 - y2;
                int b = x2 - x1;
                int c = x1 * y2 - x2 * y1;

                if (spaceship_boundar_hit(x1, y1, x2, y2, a, b, c, obstacles[i].x * SCALE_FACTOR, obstacles[i].y * SCALE_FACTOR, obstacles[i].radius)) {
                    return true;
                }
            }
        }
    }
    return false;
}

void draw_beam(beam_t &beam) {
    beam.y -= 5;
    if (beam.y < 0) {
        beam.launched = false;
    }
    if (beam.launched) {
        glColor3ub(255, 0, 0);
        glBegin(GL_POLYGON);
        for (int i = 0; i < 64; ++i) {
            double angle = (i * PI) / 32.0;
            double x = beam.x + std::cos(angle) * beam.radius;
            double y = beam.y + std::sin(angle) * beam.radius;
            glVertex2d(x, y);
        }
    }
    glEnd();
}

void draw_square(int c, int r, bool color) {
    if (color) {
        glColor3ub(255, 255, 255);
    } else {
        glColor3ub(113, 117, 114);
    }
    glBegin(GL_POLYGON);
    glVertex2d(c, r);
    glVertex2d(c + 100, r);
    glVertex2d(c + 100, r + 100);
    glVertex2d(c, r + 100);
    glEnd();
}

void draw_finsh_line() {
    for (int i = 0; i < 101; i += 100) {
        bool color = (i / 100) % 2 == 0;
        for (int j = 0; j < SCREEN_WIDTH; j += 100) {
            draw_square(j, i, color);
            color = !color;
        }
    }
}

void set_rnd_seed() {
    srand((unsigned int) time(NULL));
}

int main(void) {
    set_rnd_seed();
    bool exit_app = false;
    init_stars();
    
    int sx = 40;
    int sy = 10;
    spaceship_t spaceship;
    spaceship.sx = sx;
    spaceship.sy = sy;
    spaceship.hit = false;
    
    for (int i = 0; i < NBeams; ++i) {
        spaceship.beam[i].launched = false;
        spaceship.beam[i].x = -1;
        spaceship.beam[i].y = -1;
    }
    int n_shot = 0;

    double dt = 0.020;
    bool init_obs = true;
    
    bool spaceship_hit_state = false;
    long long after_affect_cntr = 0;
    long long after_affect_time = 5500;
    
    FsOpenWindow(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 1);
    glShadeModel(GL_SMOOTH);
    
    bool start_playing = false;
    
    for(;;) {
        glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
        FsPollDevice();
        auto key = FsInkey();
        switch (key) {
            case FSKEY_ESC:
                exit(0);
                break;
            case FSKEY_ENTER:
                start_playing = true;
                break;
        }
        
        if (start_playing) {
            break;
        }
        
        draw_background();
        draw_stars(spaceship_hit_state);
        
        glPushAttrib(GL_CURRENT_BIT);
        glColor3ub(0, 238, 127);
        glRasterPos2i(SCREEN_WIDTH / 2 - 150, SCREEN_HEIGHT / 2);
        YsGlDrawFontBitmap16x20("Stay Alive for 30s");
        
        glRasterPos2i(SCREEN_WIDTH / 2 - 350, SCREEN_HEIGHT / 2 + 100);
        YsGlDrawFontBitmap16x20("Ready? Press ENTER to continue, ESC to exit!");
        glPopAttrib();
        
        FsSwapBuffers();
        FsSleep(20);
    }
    
    long long alive_time = 0;
    std::stringstream final_ss;
    
    bool game_end_state = false;
    
    for(;;) {
        glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
        FsPollDevice();
        auto key = FsInkey();
        switch (key) {
            case FSKEY_RIGHT:
                if (!game_end_state && !spaceship_hit_state && (spaceship.sx + 3) * SCALE_FACTOR <= SCREEN_WIDTH - 4 * SCALE_FACTOR) {
                    spaceship.sx += 3;
                }
                break;
            case FSKEY_LEFT:
                if (!game_end_state && !spaceship_hit_state && (spaceship.sx - 3) * SCALE_FACTOR >= 4 * SCALE_FACTOR) {
                    spaceship.sx -= 3;
                }
                break;
            case FSKEY_SPACE:
                if (!game_end_state && !spaceship_hit_state) {
                    for (int i = 0; i < NBeams; ++i) {
                        if (!spaceship.beam[i].launched) {
                            ++n_shot;
                            spaceship.beam[i].launched = true;
                            spaceship.beam[i].x = spaceship.sx * SCALE_FACTOR;
                            spaceship.beam[i].y = SCREEN_HEIGHT - spaceship.sy * SCALE_FACTOR - 5 * SCALE_FACTOR;
                            spaceship.beam[i].radius = 5;
                            break;
                        }
                    }
                }
                break;
            case FSKEY_ESC:
                exit_app = true;
            default:
                break;
        }
        if (exit_app) {
            break;
        }
        
        // draws background with stars
        draw_background();
        draw_stars(spaceship_hit_state || game_end_state);
        
        // draws spaceship
        draw_spaceship(spaceship, spaceship_hit_state || game_end_state);
        
        if (!game_end_state && !spaceship_hit_state && init_obs) {
            init_obstacles();
            init_obs = false;
        }
        
        if (!game_end_state && !spaceship_hit_state && n_shot > 0) {
            for (int i = 0; i < NBeams; ++i) {
                if (spaceship.beam[i].launched) {
                    draw_beam(spaceship.beam[i]);
                }
            }
        }
        
        // draw obstacles
        if (!game_end_state && !spaceship_hit_state) {
            draw_obstacles(dt, spaceship.beam);
        }
        
        if (!game_end_state && !spaceship_hit_state && check_spaceship_hit(spaceship)) {
            spaceship_hit_state = true;
            alive_time = (int) std::round((double) alive_time / 1000);
            final_ss << "Spaceship Hit, Alive Time: " << alive_time << "s, Try Again!";
        }
        
        if (!game_end_state && !spaceship_hit_state && all_obstacles_gone()) {
            init_obs = true;
        }
        
        if (game_end_state) {
            draw_finsh_line();
        }
        
        if (spaceship_hit_state || game_end_state) {
            if (spaceship_hit_state) {
                glPushAttrib(GL_CURRENT_BIT);
                glColor3ub(255, 0, 0);
                glRasterPos2i(SCREEN_WIDTH / 2 - 300, SCREEN_HEIGHT / 2);
            } else {
                glPushAttrib(GL_CURRENT_BIT);
                glColor3ub(30, 246, 80);
                glRasterPos2i(SCREEN_WIDTH / 2 - 250, SCREEN_HEIGHT / 2);
                spaceship.sy += 2;
            }
            YsGlDrawFontBitmap16x20(final_ss.str().c_str());
            glPopAttrib();
            
            after_affect_cntr += FsPassedTime();
            if (after_affect_cntr >= after_affect_time) {
                break;
            }
        } else {
            alive_time += FsPassedTime();
        }
        
        if (!game_end_state && !spaceship_hit_state && alive_time >= 30 * 1000) {
            final_ss << "Congratulations! You reached Home";
            game_end_state = true;
        }
        
        FsSwapBuffers();
        if (spaceship_hit_state) {
            FsSleep(200);
        } else {
            FsSleep(20);
        }
    }
    FsCloseWindow();
    return 0;
}
