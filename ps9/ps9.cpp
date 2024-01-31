#define GL_SILENCE_DEPRECATION

#include <fstream>
#include <iostream>
#include <string>

#include "fssimplewindow.h"

// CharBitmap class must be initialized to an empty bitmap on construction.
// Also const qualifiers must be correctly used.
// If you look careful, there are hints for where to use const qualifiers.
class CharBitmap {
   protected:
    int wid, hei;
    char *dat;

   public:
    CharBitmap();
    ~CharBitmap();
    void CleanUp(void);

    bool Load(std::string fileName);

    CharBitmap(CharBitmap &incoming);
    CharBitmap &operator=(CharBitmap &incoming);
    void CopyFrom(CharBitmap &incoming);

    void Create(int wid, int hei);

    int GetWidth(void);
    int GetHeight(void);
    char *GetData(void);

    void Print(void);
    void Draw(void);
    char GetPixel(int x, int y);
};

// Default constructor.
CharBitmap::CharBitmap() {
    wid = 0;
    hei = 0;
    dat = nullptr;
}

// Destructor must clean up dynamically-allocated memory to make sure there will be no memory leak.
CharBitmap::~CharBitmap() {
    CleanUp();
}

// CleanUp function must clean up the data structure.
// Return dynamcially-allocated memory if any and then nullify the pointer and dimension.
void CharBitmap::CleanUp(void) {
    if (dat != nullptr) {
        delete[] dat;
        dat = nullptr;
    }
    wid = 0;
    hei = 0;
}

// Copy constructor must correctly copy the incoming bitmap.
// There must not be memory leak after copying.
// If you have a correct CopyFrom function, you can just call CopyFrom function.
CharBitmap::CharBitmap(CharBitmap &incoming) : dat(nullptr), wid(0), hei(0) {
    CopyFrom(incoming);
}

// Copy-assignment operator must correctly copy the incoming bitmap.
// There must not be memory leak after copying.
// If you have a correct CopyFrom function, you can call the function, and then
// return reference to this object.
CharBitmap &CharBitmap::operator=(CharBitmap &incoming) {
    CopyFrom(incoming);
    return *this;
}

// CopyFrom function must correctly copy incoming CharBitmap to this CharBitmap.
// There must not be memory leak during the process.
void CharBitmap::CopyFrom(CharBitmap &incoming) {
    if (incoming.GetData() != nullptr) {
        CleanUp();
        wid = incoming.GetWidth();
        hei = incoming.GetHeight();
        dat = new char[wid * hei + 1];
        if (dat != nullptr) {
            std::strcpy(dat, incoming.GetData());
        }
    }
}

// Load function reads a CharBitmap from a text-based data file.
// The first and the second lines are width and height respectively.
// Bitmap patten will then follow.
// The sample data for this assignment (pattern.txt) looks like:

// 16
// 16
// ....11....11....
// ...1..1..1..1...
// ...1.11..1.11...
// ...1.11..1.11...
// ...1.11..1.11...
// ...1.11..1.11...
// ...1.11..1.11...
// ...1.11111.11...
// ..1..........1..
// .1............1.
// 1....1....1....1
// 1..............1
// 1..............1
// 1......11......1
// .11..........11.
// ...1111111111...

// Also there must be no memory leak.
bool CharBitmap::Load(std::string fileName) {
    std::fstream ifp(fileName);
    if (ifp.is_open()) {
        CleanUp();
        std::string str;
        int itr1 = 0;
        int itr2 = 0;
        while (!ifp.eof()) {
            std::getline(ifp, str);
            if (itr1 == 0) {
                wid = atoi(str.c_str());
                itr1 += 1;
            } else if (itr1 == 1) {
                hei = atoi(str.c_str());
                // if wid or height is negative then
                // return load false
                // and since we are not allocating memory for
                // dat we don't need to deallocate it
                if (wid <= 0 || hei <= 0) {
                    // since wid or height or both is not valid
                    // reset these values to zero and return
                    // load as false
                    wid = 0;
                    hei = 0;
                    return false;
                }
                // if width and height is fine
                // then only allocate memory for bitmap
                dat = new char[wid * hei + 1];
                itr1 += 1;
            } else {
                for (auto &ch : str) {
                    if (ch == '.' || ch == '1') {
                        dat[itr2] = ch;
                        itr2 += 1;
                    }
                }
            }
        }
        dat[itr2] = '\0';  // end the string
        return true;
    }
    return false;
}

// Create function creates a bitmap with wid X hei dimension.
// There must not be memory leak.
// If wid or hei is zero or negative, make bitmap empty.
void CharBitmap::Create(int wid, int hei) {
    // NOTE: This has been checked in Load Function
    if (wid <= 0 || hei <= 0) {
        CleanUp();
        // clean up would
        // deallocate dat memory
        // and reset wid, hei to 0
    }
    // this is not called anywhere
}

// This function returns the width of the bitmap.
int CharBitmap::GetWidth(void) {
    return wid;
}

// This function returns the height of the bitmap.
int CharBitmap::GetHeight(void) {
    return hei;
}

// ?
// This function returns the data of the bitmap.
char *CharBitmap::GetData(void) {
    return dat;
}

// Print bitmap on the console (terminal).
void CharBitmap::Print(void) {
    if (dat != nullptr) {
        for (int i = 0; i < hei; ++i) {
            for (int j = 0; j < wid; ++j) {
                std::cout << dat[i * wid + j];
            }
            std::cout << std::endl;
        }
    }
}

// Draw bitmap on the graphics window.
// One pixel of this bitmap must be expanded to 16x16-pixel square on the graphics window.
void CharBitmap::Draw(void) {
    for (int i = 0; i < hei; ++i) {
        for (int j = 0; j < wid; ++j) {
            char el = GetPixel(i, j);
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

// Returns the pixel value at (x,y).  If (x,y) is outside of the pixel, return zero.
char CharBitmap::GetPixel(int x, int y) {
    int index = x * wid + y;
    if (index < wid * hei) {
        return dat[index];
    }
    return 0;
}

// Do not touch RealMain and main.  Leave these as is.
void RealMain(void) {
    FsOpenWindow(16, 16, 256, 256, 1);
    FsChangeToProgramDir();

    CharBitmap bmp0;
    if (true != bmp0.Load("pattern.txt")) {
        std::cout << "Load error." << std::endl;
        return;
    }
    std::cout << "Dimension: " << bmp0.GetWidth() << " X " << bmp0.GetHeight() << std::endl;
    bmp0.Print();

    CharBitmap bmp1(bmp0);  // For testing the copy constructor.

    CharBitmap bmp;
    bmp = bmp1;  // For testing the copy-assignment operator

    while (FSKEY_NULL == FsInkey()) {
        FsPollDevice();

        glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
        bmp.Draw();
        FsSwapBuffers();

        FsSleep(10);
    }
}

// At the end of program, Closing. should be printed on the console window.
// i.e., RealMain must not crash.
int main(void) {
    FsChangeToProgramDir();
    RealMain();
    // Returned from RealMain means the destructor did not crash.
    std::cout << "Closing." << std::endl;
    return 0;
}
