#define GL_SILENCE_DEPRECATION

#include <math.h>
#include <stdio.h>
#include <time.h>
#include <iostream>
#include <string>
#include "fssimplewindow.h"


class Mark {
public:
    char state;
    Mark(void) {
        state = ' ';
    }
    // drawing permanent state related to object's state variable
    void draw(double cx, double cy);
    
    // drawing temporary state when mouse hovered over a mark
    void drawHover(double cx, double cy, char newState);

private:
    const double YsPi = 3.1415927;
    void drawCircle(double cx, double cy, double rad, unsigned char r, unsigned char g, unsigned char b);
    void drawCross(double cx, double cy);
};

void Mark::drawCircle(double cx, double cy, double rad, unsigned char r, unsigned char g, unsigned char b) {
    glColor3ub(r, g, b);
    glBegin(GL_LINE_LOOP);
    for (int i = 0; i < 64; ++i) {
        double ang = YsPi * (double)i / 32.0;
        double s = sin(ang);
        double c = cos(ang);
        double x = cx + rad * c;
        double y = cy + rad * s;
        glVertex2d(x, y);
    }
    glEnd();
}

void Mark::drawCross(double cx, double cy) {
    glBegin(GL_LINES);
    glVertex2d(cx - 80, cy - 80);
    glVertex2d(cx + 80, cy + 80);
    glVertex2d(cx - 80, cy + 80);
    glVertex2d(cx + 80, cy - 80);
    glEnd();
}

void Mark::draw(double cx, double cy) {
    glDisable(GL_LINE_STIPPLE);
    if (state == 'O') {
        drawCircle(cx, cy, 80, 0, 0, 0);
    } else if (state == 'X') {
        drawCross(cx, cy);
    }
}

void Mark::drawHover(double cx, double cy, char newState) {
    glEnable(GL_LINE_STIPPLE);
    glLineStipple(1 ,0xff00);
    if (newState == 'O') {
        drawCircle(cx, cy, 80, 0, 0, 0);
    } else if (newState == 'X') {
        drawCross(cx, cy);
    }
}

class Board {
private:
    char turn = 'X';
    Mark marks[9];
    int getBucketNumber(int c);
    int getMarkIndex(int mx, int my);
    int getCenterFromMouseCoordinate(int m);
    
public:
    void draw(int mx, int my);
    bool updateBoard(int mx, int my);
    void flipTurn(void);
    char whoWon();
};

char Board::whoWon() {
    if ((turn == marks[0].state && turn == marks[1].state && turn == marks[2].state) ||
        (turn == marks[3].state && turn == marks[4].state && turn == marks[5].state) ||
        (turn == marks[6].state && turn == marks[7].state && turn == marks[8].state) ||
        
        (turn == marks[0].state && turn == marks[3].state && turn == marks[6].state) ||
        (turn == marks[1].state && turn == marks[4].state && turn == marks[7].state) ||
        (turn == marks[2].state && turn == marks[5].state && turn == marks[8].state) ||
        
        (turn == marks[0].state && turn == marks[4].state && turn == marks[8].state) ||
        (turn == marks[2].state && turn == marks[4].state && turn == marks[6].state)) {
        return turn;
    }
    bool all_filled = true;
    for (auto mark: marks) {
        if (mark.state == ' ') {
            all_filled = false;
        }
    }
    return all_filled ? 'd': '0';
}

// I'm assuming the while screen is divided into
// buckets of length 200 horizontally and vertically
// if a valid region is selected then this function
// return appropriate bucket
int Board::getBucketNumber(int c) {
    return c >= 0 &&  c <= 599 ? (c / 200): -1;
}

// function takes mouse coordinate and outputs
// which Mark (cell) it belongs to
//| 0 | 3 | 6 |
//|___|___|___|
//| 1 | 4 | 7 |
//|___|___|___|
//| 2 | 5 | 8 |
// Equivalent to: mouse coordinate and convert to column coordinate
int Board::getMarkIndex(int mx, int my) {
    int xb = getBucketNumber(mx);
    int yb = getBucketNumber(my);
    if (xb != -1 && yb != -1) {
        return yb * 3 + xb;
    }
    return -1;
}

// Equivalent to: computing center of the column (Mark/cell) from the column coordinate
int Board::getCenterFromMouseCoordinate(int m) {
    return getBucketNumber(m) * 200 + 100;
}

bool Board::updateBoard(int mx, int my) {
    int markIndex = getMarkIndex(mx, my);
    if (markIndex != -1 && marks[markIndex].state == ' ') {
        marks[markIndex].state = turn;
        return true;
    }
    return false;
}

void Board::flipTurn() {
    if (turn == 'X') {
        turn = 'O';
    } else {
        turn = 'X';
    }
}

void Board::draw(int mx, int my) {
    glDisable(GL_LINE_STIPPLE);
    glBegin(GL_LINES);
    glVertex2i(0, 200);
    glVertex2i(600, 200);

    glVertex2i(0, 400);
    glVertex2i(600, 400);

    glVertex2i(200, 0);
    glVertex2i(200, 600);

    glVertex2i(400, 0);
    glVertex2i(400, 600);
    glEnd();
    
    for (int y = 0; y < 3; ++y) {
        for (int x = 0; x < 3; ++x) {
            double cx = x * 200 + 100;
            double cy = y * 200 + 100;
            marks[y * 3 + x].draw(cx, cy);
        }
    }
    
    int markIndex = getMarkIndex(mx, my);
    if (markIndex != -1 && marks[markIndex].state == ' ') {
        double cx = getCenterFromMouseCoordinate(mx);
        double cy = getCenterFromMouseCoordinate(my);
        marks[markIndex].drawHover(cx, cy, turn);
    }
}

int main(void) {
    FsOpenWindow(0, 0, 600, 600, 1);
    int lb, mb, rb, mx, my;
    int mouseEvent;
    Board board;
    bool boardUpdated = false;
    while (FsInkey() == FSKEY_NULL) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        FsPollDevice();
        mouseEvent = FsGetMouseEvent(lb, mb, rb, mx, my);
        switch (mouseEvent) {
            case FSMOUSEEVENT_LBUTTONDOWN:
                // Assumption here is that board will not update everytime
                // if there is an invalid click then updateBoard returns False
                boardUpdated = board.updateBoard(mx, my);
                break;
            default:
                break;
        }
        board.draw(mx, my);
        FsSwapBuffers();
        char res = board.whoWon();
        if (res != '0') {
            if (res == 'd') {
                std::cout << "It's a draw" << std::endl;
            } else {
                std::cout << res << " Wins" << std::endl;
            }
            break;
        }
        // Flip turn only when board is updated
        if (boardUpdated) {
            board.flipTurn();
            boardUpdated = false;
        }
    }
    FsCloseWindow();
    return 0;
}
