#define GL_SILENCE_DEPRECATION

#include <iostream>
#include "fssimplewindow.h"

void drawBitMap(char *pattern) {
    for (int i = 0; i < 16; ++i) {
        for (int j = 0; j < 16; ++j) {
            char el = pattern[i * 16 + j];
            if (el == '1') {
                int start_x = j * 16;
                int start_y = i * 16;
                glColor3ub(0, 0, 0);
                glBegin(GL_POLYGON);
                glVertex2i(start_x, start_y);
                glVertex2i(start_x, start_y + 16);
                glVertex2i(start_x + 16, start_y + 16);
                glVertex2i(start_x + 16, start_y);
                glEnd();
            }
        }
    }
}

int main(void) {
    // Character for the cell (x,y) will be pattern[y*16+x]
    char pattern[]= {
        // By the way, if you want to define a long string, you can write like this.
        "...1........1..."
        "...11......11..."
        "...1.1....1.1..."
        "...1..1..1..1..."
        "...1..1111..1..."
        "..1..........1.."
        "..1..11..11..1.."
        "..1..........1.."
        ".1..111..111..1."
        ".1.1.1.11.1.1.1."
        "1..1.1.11.1.1..1"
        "1...111..111...1"
        ".1............1."
        ".1.....11.....1."
        "..111......111.."
        ".....111111....."
    };
    FsOpenWindow(0, 0, 256, 256, 1);
    for (;;) {
        FsPollDevice();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        auto keyIn = FsInkey();
        if (keyIn == FSKEY_ESC) {
            break;
        }
        drawBitMap(pattern);
        FsSwapBuffers();
        FsSleep(20);
    }
    FsCloseWindow();
    return 0;
}

