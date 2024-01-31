//
//  ps4-1.cpp
//
//  Created by Balasubramanyam Evani on 9/23/23.
//

#define GL_SILENCE_DEPRECATION

#include <stdio.h>
#include <time.h>

#include <cmath>
#include <iostream>

#include "fssimplewindow.h"

// Constants for screen dimensi1ons and scaling factors
const int SCREEN_WIDTH = 600;
const int SCREEN_HEIGHT = 600;
const double PI = 3.141592653589;

void drawCross(double cx, double cy, double length) {
    glBegin(GL_LINES);
    glVertex2d(cx, cy);
    glVertex2d(cx + length, cy + length);
    glEnd();

    glBegin(GL_LINES);
    glVertex2d(cx, cy);
    glVertex2d(cx - length, cy - length);
    glEnd();

    glBegin(GL_LINES);
    glVertex2d(cx, cy);
    glVertex2d(cx - length, cy + length);
    glEnd();

    glBegin(GL_LINES);
    glVertex2d(cx, cy);
    glVertex2d(cx + length, cy - length);
    glEnd();
}

void drawCircle(double cx, double cy, double radius) {
    glBegin(GL_LINE_LOOP);
    for (int i = 0; i <= 64; ++i) {
        double theta = (i * PI) / 32;
        double x = cx + radius * std::cos(theta);
        double y = cy + radius * std::sin(theta);
        glVertex2d(x, y);
    }
    glEnd();
}

void drawLine(int x1, int y1, int x2, int y2) {
    glBegin(GL_LINES);
    glVertex2i(x1, y1);
    glVertex2i(x2, y2);
    glEnd();
}

void PrintBoard(char b[9]) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    int section_width = SCREEN_WIDTH / 3;
    int section_height = SCREEN_HEIGHT / 3;

    // drawing vertical lines
    for (int i = 1; i < 3; ++i) {
        drawLine(section_width * i, 0, section_width * i, SCREEN_HEIGHT);
    }

    // drawing horizontal lines
    for (int i = 1; i < 3; ++i) {
        drawLine(0, section_height * i, SCREEN_WIDTH, section_height * i);
    }

    double len = (section_width / 2) - 10.0;

    for (int y = 0; y < 3; ++y) {
        for (int x = 0; x < 3; ++x) {
            char c = b[y * 3 + x];
            if (c != ' ') {
                double cx = x * section_width + (section_width / 2);
                double cy = y * section_height + (section_height / 2);
                if (c == 'X') {
                    drawCross(cx, cy, len);
                } else if (c == 'O') {
                    drawCircle(cx, cy, len);
                }
            }
            printf("%c", c);
            if (2 != x) {
                printf("|");
            }
        }
        printf("\n");
        if (2 != y) {
            printf("-+-+-\n");
        }
    }
    FsSwapBuffers();
}

bool Check3inaRow(char board[9], char turn) {
    if ((turn == board[0] && turn == board[1] && turn == board[2]) ||
        (turn == board[3] && turn == board[4] && turn == board[5]) ||
        (turn == board[6] && turn == board[7] && turn == board[8]) ||

        (turn == board[0] && turn == board[3] && turn == board[6]) ||
        (turn == board[1] && turn == board[4] && turn == board[7]) ||
        (turn == board[2] && turn == board[5] && turn == board[8]) ||

        (turn == board[0] && turn == board[4] && turn == board[8]) ||
        (turn == board[2] && turn == board[4] && turn == board[6])) {
        return true;
    }
    return false;
}

int main(void) {
    FsOpenWindow(50, 50, SCREEN_WIDTH, SCREEN_HEIGHT, 1);

    char board[9];
    for (auto &b : board) {
        b = ' ';
    }
    char turn = 'X';
    for (;;) {
        FsPollDevice();
        PrintBoard(board);
        printf("%c's turn.\n", turn);
        printf("Enter X and Y:");
        int x, y;

        scanf("%d%d", &x, &y);
        if (0 <= x && x < 3 && 0 <= y && y < 3) {
            if (' ' != board[y * 3 + x]) {
                printf("Already Taken.\n");
                continue;
            }
            board[y * 3 + x] = turn;
            if (true == Check3inaRow(board, turn)) {
                printf("%c wins!\n", turn);
                PrintBoard(board);
                auto t0 = time(NULL);
                while (time(NULL) - t0 < 3) {
                }
                break;
            }
            if ('X' == turn) {
                turn = 'O';
            } else {
                turn = 'X';
            }
        } else {
            printf("X and Y needs to be between 0 and 2.\n");
        }
    }
    FsCloseWindow();
    return 0;
}
