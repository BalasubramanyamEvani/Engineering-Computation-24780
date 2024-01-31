#define GL_SILENCE_DEPRECATION

#include "fssimplewindow.h"
#include "yssimplesound.h"
#include <algorithm>
#include <iostream>
#include <string>

class AudioVisualizer {
public:
    std::string filename;
    AudioVisualizer(std::string filename);
    bool initialize(void);
    void play(void);
    
private:
    YsSoundPlayer player;
    YsSoundPlayer::SoundData wav;
    bool initialized;
    int nSamples;
    void drawOriginLines(void);
    void drawWaveforms(void);
    void drawCurrentPosition(void);
};

AudioVisualizer::AudioVisualizer(std::string filename) {
    this->filename = filename;
}

bool AudioVisualizer::initialize(void) {
    initialized = wav.LoadWav(filename.c_str()) == YSOK;
    return initialized;
}

void AudioVisualizer::drawOriginLines(void) {
    glColor3ub(0, 0, 0);
    glBegin(GL_LINES);
    glVertex2i(0, 450);
    glVertex2i(799, 450);
    glEnd();
    
    glColor3ub(0, 0, 0);
    glBegin(GL_LINES);
    glVertex2i(0, 150);
    glVertex2i(799, 150);
    glEnd();
}

void AudioVisualizer::drawWaveforms(void) {
    this->nSamples = wav.GetNumSamplePerChannel();
    for (int channel = 0; channel < wav.GetNumChannel(); ++channel) {
        switch (channel) {
            case 0:
                glColor3ub(255, 0, 0);
                break;
            case 1:
                glColor3ub(0, 0, 255);
                break;
            default:
                continue;
        }
        glBegin(GL_LINE_STRIP);
        for (int x = 0; x < 800; ++x) {
            int pos = (int) x * this->nSamples / 800;
            int y = wav.GetSignedValue16(channel, pos);
            y = 150 + (channel * 300) - (int) ((double) y * 300 / 32768);
            glVertex2i(x, y);
        }
        glEnd();
    }
}

void AudioVisualizer::drawCurrentPosition(void) {
    auto currentTime = player.GetCurrentPosition(wav);
    auto currentPos = wav.SecToNumSample(currentTime);
    int dy1 = 150;
    int dy2 = 450;
    for (int channel = 0; channel < wav.GetNumChannel(); ++channel) {
        switch (channel) {
            case 0:
                glColor3ub(255, 0, 0);
                break;
            case 1:
                glColor3ub(0, 0, 255);
                break;
            default:
                continue;
        }
        glPointSize(8);
        glBegin(GL_POINTS);
        int y = wav.GetSignedValue16(channel, (int) currentPos);
        y = 150 + (channel * 300) - (int) ((double) y * 300 / 32768);
        glVertex2i((int) currentPos * 800 / this->nSamples, y);
        glEnd();
        if (channel == 0) {
            dy1 = y;
        } else {
            dy2 = y;
        }
    }
    
    glColor3ub(0, 255, 0);
    glBegin(GL_LINES);
    glVertex2i((int) currentPos * 800 / this->nSamples, dy1);
    glVertex2i((int) currentPos * 800 / this->nSamples, dy2);
    glEnd();
    
    glColor3ub(0, 255, 0);
    glBegin(GL_LINES);
    glVertex2i((int) currentPos * 800 / this->nSamples, dy1);
    glVertex2i((int) currentPos * 800 / this->nSamples, 0);
    glEnd();
    
    glColor3ub(0, 255, 0);
    glBegin(GL_LINES);
    glVertex2i((int) currentPos * 800 / this->nSamples, dy2);
    glVertex2i((int) currentPos * 800 / this->nSamples, 599);
    glEnd();
}

void AudioVisualizer::play(void) {
    if (initialized) {
        player.Start();
        player.PlayOneShot(wav);
        while (player.IsPlaying(wav) == YSTRUE) {
            glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
            FsPollDevice();
            auto keyIn = FsInkey();
            if (keyIn == FSKEY_ESC) {
                FsCloseWindow();
                exit(0);
            }
            player.KeepPlaying();
            drawOriginLines();
            drawWaveforms();
            drawCurrentPosition();
            FsSwapBuffers();
        }
        player.End();
    }
}

int main(void) {
    FsOpenWindow(0, 0, 800, 600, 1);
    for (;;) {
        std::string fileName;
        std::cout << "Enter WAV File Path:";
        std::getline(std::cin, fileName);
        if (fileName.empty()) {
            std::cout << "Exited" << std::endl;
            break;
        }
        AudioVisualizer audioVisualizer(fileName);
        if (!audioVisualizer.initialize()) {
            std::cout << "Failed to read, try again" << std::endl;
            continue;
        }
        audioVisualizer.play();
    }
    FsCloseWindow();
    return 0;
}
