#define GL_SILENCE_DEPRECATION

#include <iostream>
#include <string>
#include <fstream>
#include <sstream>

#include "fssimplewindow.h"

#define uchar unsigned char

const std::string CONCORDE_FILE = "concorde.txt";
const std::string REDARROW_FILE = "redarrow.txt";

std::vector<std::string> Parse(std::string line) {
    std::vector<std::string> words;
    std::istringstream ss(line);
    std::string currentWord;
    while (ss >> currentWord) {
        words.push_back(currentWord);
    }
    return words;
}

class Line {
public:
    int x1, y1, x2, y2;
    uchar r, g ,b;
    Line() : x1(0), y1(0), x2(0), y2(0), r(0), g(0), b(0) {};
};

class Drawing {
public:
    Drawing() {};
    ~Drawing();
    void CleanUp();
    bool Open(std::string filename);
    void Draw(int offset_x, int offset_y);

private:
    unsigned int nLines = 0;
    Line *lines = nullptr;
    
    Drawing(const Drawing &from);
    const Drawing &operator = (const Drawing &from);
};

Drawing::~Drawing() {
    CleanUp();
}

void Drawing::CleanUp() {
    if (lines != nullptr) {
        delete []lines;
        lines = nullptr;
    }
    nLines = 0;
}

bool Drawing::Open(std::string filename) {
    std::ifstream ifp(filename);
    if (ifp.is_open()) {
        CleanUp(); // no resource leak
        std::string str;
        int itr = 0;
        while (!ifp.eof()) {
            std::getline(ifp, str);
            auto words = Parse(str);
            if (words.size() == 1) {
                nLines = atoi(words[0].c_str());
                lines = new Line[nLines];
            } else if (words.size() == 7) {
                lines[itr].x1 = atoi(words[0].c_str());
                lines[itr].y1 = atoi(words[1].c_str());
                lines[itr].x2 = atoi(words[2].c_str());
                lines[itr].y2 = atoi(words[3].c_str());
                lines[itr].r = atoi(words[4].c_str());
                lines[itr].g = atoi(words[5].c_str());
                lines[itr].b = atoi(words[6].c_str());
                itr += 1;
            }
        }
        if (itr != nLines) {
            CleanUp(); // no resource leak
            return false;
        }
        return true;
    }
    return false;
}

void Drawing::Draw(int offset_x, int offset_y) {
    glBegin(GL_LINES);
    for (int i = 0; i < nLines; ++i) {
        glColor3ub(lines[i].r, lines[i].g, lines[i].b);
        glVertex2i(offset_x + lines[i].x1, offset_y + lines[i].y1);
        glVertex2i(offset_x + lines[i].x2, offset_y + lines[i].y2);
    }
    glEnd();
}

int main(void) {
    FsChangeToProgramDir();
    FsOpenWindow(0, 0, 800, 600, 1);
    Drawing drawing;
    if (!drawing.Open(CONCORDE_FILE)) {
        std::cout << "Error opening " << CONCORDE_FILE << std::endl;
        exit(1);
    }
    int offset_x = 400, offset_y = 300;
    bool exitApp = false;
    for (;;) {
        FsPollDevice();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        auto keyIn = FsInkey();
        switch (keyIn) {
            case FSKEY_R:
                if (!drawing.Open(REDARROW_FILE)) {
                    std::cout << "Error opening " << REDARROW_FILE << std::endl;
                    exit(1);
                }
                break;
            case FSKEY_C:
                if (!drawing.Open(CONCORDE_FILE)) {
                    std::cout << "Error opening " << CONCORDE_FILE << std::endl;
                    exit(1);
                }
                break;
            case FSKEY_UP:
                offset_y -= 10;
                break;
            case FSKEY_DOWN:
                offset_y += 10;
                break;
            case FSKEY_RIGHT:
                offset_x += 10;
                break;
            case FSKEY_LEFT:
                offset_x -= 10;
                break;
            case FSKEY_ESC:
                exitApp = true;
                break;
            default:
                break;
        }
        if (exitApp) {
            break;
        }
        drawing.Draw(offset_x, offset_y);
        FsSwapBuffers();
        FsSleep(20);
    }
    FsCloseWindow();
    return 0;
}
