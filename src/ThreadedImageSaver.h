#ifndef THREADEDIMAGESAVER_H
#define THREADEDIMAGESAVER_H

#include "ofMain.h"
#include "ofThread.h"

class ThreadedImageSaver : public ofThread, public ofImage {
public:
    ~ThreadedImageSaver()
    {
        stopThread();
    }

    string fileName;
    void threadedFunction() {
        if(lock()) {
            saveImage(fileName);
            unlock();
        } else {
            printf("ThreadedImageSaver - cannot save %s cos I'm locked", fileName.c_str());
        }
        stopThread();
    }
    void saveThreaded(string fileName) {
        this->fileName = fileName;
        startThread(false, false);   // blocking, verbose
    }
};

#endif // THREADEDIMAGESAVER_H
