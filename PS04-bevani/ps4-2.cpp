//
//  ps4-2.cpp
//
//  Created by Balasubramanyam Evani on 9/25/23.
//

#define GL_SILENCE_DEPRECATION

#include <cmath>
#include <cstdlib>
#include <iostream>
#include <vector>

#include "fssimplewindow.h"
#include "time.h"

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600

const double PI = 3.141592653589;
const double G = 9.81;

const int CANNON_POS_X = 10;
const int CANNON_POS_Y = 10;
const int CANNON_BASE_LEN = 10;
const int CANNON_LEN = 20;

const int NUM_OBSTACLES = 5;

int CANNON_BALL_STATE = 0;

double curr_cannon_head_x = 0.0;
double curr_cannon_head_y = 0.0;

void draw_cannon(int cannon_angle) {
    glColor3ub(0, 0, 255);
    glBegin(GL_QUADS);
    glVertex2i(CANNON_POS_X, SCREEN_HEIGHT - CANNON_POS_Y);
    glVertex2i(CANNON_POS_X, SCREEN_HEIGHT - CANNON_POS_Y - CANNON_BASE_LEN);
    glVertex2i(CANNON_POS_X + CANNON_BASE_LEN, SCREEN_HEIGHT - CANNON_POS_Y - CANNON_BASE_LEN);
    glVertex2i(CANNON_POS_X + CANNON_BASE_LEN, SCREEN_HEIGHT - CANNON_POS_Y);
    glEnd();

    double theta = (cannon_angle * PI) / 180.0;
    glColor3ub(0, 0, 255);
    glBegin(GL_LINES);
    glVertex2i(CANNON_POS_X + CANNON_BASE_LEN / 2, SCREEN_HEIGHT - CANNON_POS_Y - CANNON_BASE_LEN / 2);
    glVertex2d(CANNON_POS_X + (CANNON_BASE_LEN / 2) + CANNON_LEN * std::cos(theta), SCREEN_HEIGHT - CANNON_POS_Y - (CANNON_BASE_LEN / 2) - CANNON_LEN * std::sin(theta));
    glEnd();

    curr_cannon_head_x = CANNON_POS_X + (CANNON_BASE_LEN / 2) + CANNON_LEN * std::cos(theta);
    curr_cannon_head_y = SCREEN_HEIGHT - CANNON_POS_Y - (CANNON_BASE_LEN / 2) - CANNON_LEN * std::sin(theta);
}

void draw_circle(double cx, double cy, double radius, bool fill) {
    if (fill) {
        glBegin(GL_POLYGON);
    } else {
        glBegin(GL_LINE_LOOP);
    }
    for (int i = 0; i < 64; ++i) {
        double angle = (i * PI) / 32.0;
        double x = cx + std::cos(angle) * radius;
        double y = cy + std::sin(angle) * radius;
        glVertex2d(x, y);
    }
    glEnd();
}

void draw_cannon_ball(double cx, double cy, double radius) {
    switch (CANNON_BALL_STATE) {
        case 1:
            glColor3ub(0, 0, 255);
            break;
        case 2:
            glColor3ub(0, 255, 255);
            break;
        case 3:
            glColor3ub(255, 255, 0);
            break;
        case 4:
            glColor3ub(255, 0, 255);
            break;
        default:
            glColor3ub(255, 0, 0);
            break;
    }
    draw_circle(cx, cy, radius, true);
}

void draw_obstacles(int x, int y, int w, int h, bool hit) {
    glColor3ub(0, 255, 0);
    if (!hit) {
        glBegin(GL_QUADS);
    } else {
        glBegin(GL_LINE_LOOP);
    }
    glVertex2i(x, SCREEN_HEIGHT - y);
    glVertex2i(x, SCREEN_HEIGHT - y - h);
    glVertex2i(x + w, SCREEN_HEIGHT - y - h);
    glVertex2i(x + w, SCREEN_HEIGHT - y);
    glEnd();
}

void draw_target(int x, int y, bool hit) {
    glColor3ub(255, 0, 0);
    if (hit) {
        glBegin(GL_LINE_LOOP);
    } else {
        glBegin(GL_QUADS);
    }
    glVertex2i(x, SCREEN_HEIGHT - y);
    glVertex2i(x, SCREEN_HEIGHT - y - 100);
    glVertex2i(x + 100, SCREEN_HEIGHT - y - 100);
    glVertex2i(x + 100, SCREEN_HEIGHT - y);
    glEnd();
}

void draw_trajectory(std::vector<double> &ball_pos_xs, std::vector<double> &ball_pos_ys) {
    auto length = ball_pos_xs.size();
    glColor3ub(0, 0, 0);
    glBegin(GL_LINES);
    for (std::size_t i = 0; i < length - 1; ++i) {
        glVertex2d(ball_pos_xs[i] * 10, SCREEN_HEIGHT - ball_pos_ys[i] * 10);
        glVertex2d(ball_pos_xs[i + 1] * 10, SCREEN_HEIGHT - ball_pos_ys[i + 1] * 10);
    }
    glEnd();
}

bool check_collision(double ball_x, double ball_y, double obj_x, double obj_y, double obj_w, double obj_h) {
    double cx = obj_x + obj_w / 2;
    double cy = obj_y - obj_h / 2;

    double xdist = std::sqrt(std::pow((cx - ball_x), 2));
    double ydist = std::sqrt(std::pow((cy - ball_y), 2));

    if (xdist <= obj_w / 2 && ydist <= obj_h / 2) {
        return true;
    }

    return false;
}

void draw_explosions(double pos_x, double pos_y) {
    glColor3ub(255, 0, 0);
    double rand_radius = 4 + rand() % 9;
    draw_circle(pos_x, pos_y, rand_radius, false);
}

void set_random_seed() {
    srand((unsigned int)time(NULL));
}

int main(void) {
    set_random_seed();

    bool obstacle_hit[NUM_OBSTACLES];
    int obstacle_pos_x[NUM_OBSTACLES];
    int obstacle_pos_y[NUM_OBSTACLES];
    int obstacle_width[NUM_OBSTACLES];
    int obstacle_height[NUM_OBSTACLES];

    int cannon_angle = 45;
    int start_v = 2;

    bool can_shoot = true;
    double ball_velocity = 40;
    double vx = 0.0, vy = 0.0;
    double delta_t = 0.020;
    double curr_ball_x = 0.0, curr_ball_y = 0.0;
    std::vector<double> ball_pos_xs;
    std::vector<double> ball_pos_ys;

    int target_pos_x = SCREEN_WIDTH - 100, target_pos_y = SCREEN_HEIGHT - 100;
    bool target_hit = false;
    bool explosion_state = false;
    double explosion_affect_pos_x = -1.0;
    double explosion_affect_pos_y = -1.0;
    long long explosion_after_affect_time = 5500;
    long long explosion_after_affect_cntr = 0;

    for (int i = 0; i < NUM_OBSTACLES; ++i) {
        obstacle_hit[i] = false;
        obstacle_width[i] = (8 + rand() % 8) * 10;
        obstacle_height[i] = (8 + rand() % 8) * 10;
        obstacle_pos_x[i] = (10 + rand() % 46) * 10;
        obstacle_pos_y[i] = (5 + rand() % 41) * 10;
    }
    bool exit_app = false;

    FsOpenWindow(300, 150, SCREEN_WIDTH, SCREEN_HEIGHT, 1);

    for (;;) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        FsPollDevice();
        auto key = FsInkey();
        switch (key) {
            case FSKEY_ESC:
                exit_app = true;
                break;
            case FSKEY_UP:
                cannon_angle += 3;
                if (cannon_angle > 90) {
                    cannon_angle = 0;
                }
                break;
            case FSKEY_DOWN:
                cannon_angle -= 3;
                if (cannon_angle < 0) {
                    cannon_angle = 90;
                }
                break;
            case FSKEY_SPACE:
                if (can_shoot) {
                    ball_pos_xs.clear();
                    ball_pos_ys.clear();

                    curr_ball_x = curr_cannon_head_x / 10;
                    curr_ball_y = (SCREEN_HEIGHT - curr_cannon_head_y) / 10;

                    ball_pos_xs.push_back(curr_ball_x);
                    ball_pos_ys.push_back(curr_ball_y);

                    can_shoot = false;
                    vx = ball_velocity * std::cos((cannon_angle * PI) / 180);
                    vy = ball_velocity * std::sin((cannon_angle * PI) / 180);
                    CANNON_BALL_STATE += 1;
                }
                break;
        }

        if (explosion_state) {
            draw_explosions(explosion_affect_pos_x * 10, SCREEN_HEIGHT - explosion_affect_pos_y * 10);
        }

        if (exit_app) {
            break;
        }

        if (!can_shoot) {
            vy = vy - G * delta_t;        // v = u + at
            curr_ball_x += vx * delta_t;  // x displacement
            curr_ball_y += vy * delta_t;  // y displacement

            ball_pos_xs.push_back(curr_ball_x);
            ball_pos_ys.push_back(curr_ball_y);

            if (curr_ball_x * 10 > SCREEN_WIDTH || curr_ball_y < 0) {
                ball_pos_xs.clear();
                ball_pos_ys.clear();
                can_shoot = true;
            }
        }

        if (!can_shoot) {
            draw_cannon_ball(curr_ball_x * 10, SCREEN_HEIGHT - curr_ball_y * 10, 3);
            draw_trajectory(ball_pos_xs, ball_pos_ys);
        }

        // drawing cannon
        draw_cannon(cannon_angle);

        // drawing obstacles
        for (int i = 0; i < NUM_OBSTACLES; ++i) {
            if (!can_shoot && !obstacle_hit[i]) {
                bool hit = check_collision(curr_ball_x * 10, SCREEN_HEIGHT - curr_ball_y * 10, obstacle_pos_x[i], SCREEN_HEIGHT - obstacle_pos_y[i], obstacle_width[i], obstacle_height[i]);
                if (hit) {
                    obstacle_hit[i] = true;
                    can_shoot = true;
                    ball_pos_xs.clear();
                    ball_pos_ys.clear();
                }
            }
            draw_obstacles(obstacle_pos_x[i], obstacle_pos_y[i], obstacle_width[i], obstacle_height[i], obstacle_hit[i]);
        }

        // Assuming 20ms of sleep after each iteration and
        // no overhead of for loops and other instructions
        // in 20ms we expect to move down 0.2m i.e. 2 pixels
        if (!explosion_state) {
            if (target_pos_y <= 0) {
                start_v = -2;
            } else if (target_pos_y >= SCREEN_HEIGHT - 100) {
                start_v = 2;
            }
            target_pos_y -= start_v;
        }

        // drawing target
        draw_target(target_pos_x, target_pos_y, target_hit);
        if (!can_shoot && check_collision(curr_ball_x * 10, SCREEN_HEIGHT - curr_ball_y * 10, target_pos_x, SCREEN_HEIGHT - target_pos_y, 100, 100) && !explosion_state) {
            target_hit = true;
            can_shoot = true;
            explosion_affect_pos_x = curr_ball_x;
            explosion_affect_pos_y = curr_ball_y;
            ball_pos_xs.clear();
            ball_pos_ys.clear();
            explosion_state = true;
            continue;
        }

        // swapping secondary and primary buffers
        FsSwapBuffers();
        if (explosion_state) {
            explosion_after_affect_cntr += FsPassedTime();
            if (explosion_after_affect_cntr >= explosion_after_affect_time) {
                break;
            }
        }
        FsSleep(20);
    }
    FsCloseWindow();
    return 0;
}
