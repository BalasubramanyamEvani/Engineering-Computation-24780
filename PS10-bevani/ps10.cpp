#include <ctype.h>
#include <stdio.h>
#include <string>
#include <cstdlib>
#include <iostream>

class TextString {
   protected:
    char *str;

   public:
    TextString();
    TextString(const char from[]);
    TextString(const TextString &str);
    ~TextString();
    void CleanUp();
    void Set(const char from[]);
    void Add(const char from[]);
    void Add(const char c);
    void BackSpace(void);
    const char *Fgets(FILE *fp);
    void DeleteLastControlCode(void);
    const char *GetPointer(void) const;
    const int Strlen(void) const;
    void Print(void) const;
    const TextString &operator=(const TextString &from);
};

TextString::TextString() : str(NULL) {
}

TextString::TextString(const char from[]) : str(NULL) {
    Set(from);
}

TextString::TextString(const TextString &from) : str(NULL) {
    Set(from.GetPointer());
}

TextString::~TextString() {
    CleanUp();
}

void TextString::CleanUp() {
    if (NULL != str) {
        delete[] str;
        str = NULL;
    }
}

void TextString::Set(const char from[]) {
    if (NULL != from) {
        CleanUp();

        const int n = static_cast<int>(strlen(from));
        str = new char[n + 1];
        if (NULL != str) {
            strcpy(str, from);
        }
    }
}

void TextString::Add(const char from[]) {
    if (NULL != from) {
        const int nCurrent = Strlen();
        const int nFrom = static_cast<int>(strlen(from));

        char *newStr = new char[nCurrent + nFrom + 1];
        if (NULL != newStr) {
            strcpy(newStr, GetPointer());
            strcpy(newStr + nCurrent, from);
            if (NULL != str) {
                delete[] str;
            }
            str = newStr;
        }
    }
}

void TextString::Add(const char c) {
    const int nCurrent = Strlen();

    char *newStr = new char[nCurrent + 2];
    if (NULL != newStr) {
        strcpy(newStr, GetPointer());
        newStr[nCurrent] = c;
        newStr[nCurrent + 1] = 0;
        if (NULL != str) {
            delete[] str;
        }
        str = newStr;
    }
}

void TextString::BackSpace(void) {
    if (NULL != str) {
        int n = Strlen();
        if (0 < n) {
            str[n - 1] = 0;
        }
    }
}

void TextString::DeleteLastControlCode(void) {
    int n;
    for (n = static_cast<int>(strlen(str) - 1); 0 <= n; n--) {
        if (0 != isprint(str[n])) {
            break;
        }
        str[n] = 0;
    }
}

const char *TextString::Fgets(FILE *fp) {
    if (NULL != fp) {
        CleanUp();

        int firstLine = 1;
        const int nBuf = 8;
        char buf[nBuf + 1];

        buf[nBuf] = 0;
        for (;;) {
            if (NULL == fgets(buf, nBuf, fp)) {
                if (0 != firstLine) {
                    return NULL;
                } else {
                    break;
                }
            } else {
                firstLine = 0;
                Add(buf);

                int nChar = static_cast<int>(strlen(buf));
                if (buf[nChar - 1] == '\n' || buf[nChar - 1] == '\a') {
                    break;
                }
            }
        }

        DeleteLastControlCode();
        return str;
    }
    return NULL;
}

const char *TextString::GetPointer(void) const {
    if (NULL != str) {
        return str;
    } else {
        return "";
    }
}

const int TextString::Strlen(void) const {
    return static_cast<int>(strlen(GetPointer()));
}

void TextString::Print(void) const {
    if (NULL != str) {
        printf("%s\n", str);
    }
}

const TextString &TextString::operator=(const TextString &from) {
    Set(from.GetPointer());
    return *this;
}

bool containsExtraneousChars(TextString &input) {
    int inputLength = input.Strlen();
    const char *str = input.GetPointer();
    for (int i = 0; i < inputLength; ++i) {
        if (str[i] != '/' &&
            str[i] != ' ' && str[i] != '-' &&
            str[i] != 'X' && str[i] != 'x' &&
            (str[i] < '0' || str[i] > '9')) {
            return true;
        }
    }
    return false;
}

int ScoreFromFrame(const int *pinsKnocked, int frameNo) {
    int score = 0;
    int sindex = (frameNo - 1) * 2;
    
    // same logic for frames 1 to 9
    // if not a spare and a strike read normally
    // if spare read one extra shot
    // if strike read two extra shots
    if (frameNo >= 1 && frameNo <= 9) {
        score += pinsKnocked[sindex];
        // got a strike
        if (pinsKnocked[sindex] == 10) {
            score += pinsKnocked[sindex + 2];
            // got one more strike
            if (pinsKnocked[sindex + 2] == 10) {
                // reached 10th frame then take next value
                // and don't go into next frame
                if (sindex + 2 == (10 - 1) * 2) {
                    score += pinsKnocked[sindex + 3];
                }
                // else go into next frame
                else {
                    score += pinsKnocked[sindex + 4];
                }
            } else {
                score += pinsKnocked[sindex + 3];
            }
        }
        // else got a spare
        else if (score + pinsKnocked[sindex + 1] == 10) {
            score += pinsKnocked[sindex + 1] + pinsKnocked[sindex + 2];
        }
        // else just sum two vals
        else {
            score += pinsKnocked[sindex + 1];
        }
    }
    // frame 10
    else if (frameNo == 10) {
        score += pinsKnocked[sindex];
        score += pinsKnocked[sindex + 1];
        score += pinsKnocked[sindex + 2];
    }
    return score;
}

int main(void) {
    std::string inStr;
    TextString inLine;
    const int numFrames = 10;
    
    // take input from user
    std::getline(std::cin, inStr);
    
    // set textString object to
    inLine.Set(inStr.c_str());

    // if the input string length is < 21
    if (inLine.Strlen() < 21) {
        std::cout << "Too Short" << std::endl;
        return EXIT_FAILURE;
    }
    
    // check if string length > 21
    // then limit input to 22 chars (+1 for null terminate)
    if (inLine.Strlen() > 21) {
        char trimmed[21 + 1];
        const char *str = inLine.GetPointer();
        for (int i = 0; i < 21; ++i) {
            trimmed[i] = str[i];
        }
        trimmed[21] = '\0'; // null terminate
        inLine.Set(trimmed);
    }

    if (containsExtraneousChars(inLine)) {
        std::cout << "Wrong Character" << std::endl;
        return EXIT_FAILURE;
    }

    // initialize an array to store pinsKnocked
    int pinsKnocked[22];
    for (int i = 0; i < 22; ++i) {
        pinsKnocked[i] = 0;
    }
    
    // fill the pinsKnocked with inLine chars
    int inputLength = inLine.Strlen();
    const char *str = inLine.GetPointer();
    for (int i = 0; i < inputLength; ++i) {
        // if encounterd ‘0’ to ‘9’
        if (str[i] >= '0' && str[i] <= '9') {
            pinsKnocked[i] = str[i] - '0';
        }
        // ‘-‘ or ‘ ‘
        else if (str[i] == ' ' || str[i] == '-') {
            pinsKnocked[i] = 0;
        }
        // ‘X’ or ‘x strike
        else if (str[i] == 'X' || str[i] == 'x') {
            pinsKnocked[i] = 10;
        }
        // ‘/’ spare
        else {
            pinsKnocked[i] = 10 - pinsKnocked[i - 1];
        }
    }
    
    // initialize finale score to 0
    int totalScore = 0;
    
    // iterate over all frames and calculate score
    for (int i = 1; i <= numFrames; ++i) {
        totalScore += ScoreFromFrame(pinsKnocked, i);
    }
    
    // print out the final total score
    std::cout << totalScore << std::endl;
    return 0;
}
