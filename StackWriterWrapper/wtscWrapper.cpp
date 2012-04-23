/* 
 * File:   wtscWrapper.cpp
 * Author: Marc
 * 
 * Created on March 16, 2011, 2:22 PM
 * 
 * (C) Marc Gershow; licensed under the Creative Commons Attribution Share Alike 3.0 United States License.
 * To view a copy of this license, visit http://creativecommons.org/licenses/by-sa/3.0/us/ or send a letter to
 * Creative Commons, 171 Second Street, Suite 300, San Francisco, California, 94105, USA.
 */

#include "wtscWrapper.h"
#include <sstream>
#include <iomanip>
#include <io.h>
#include <iostream>
#include <fstream>

using namespace std;

void wtscWrapper::init() {
    std::ofstream os("c:\\testingcs.txt");
    os << "initialize critical section called" << endl;
    InitializeCriticalSection(&protectedAction);
    os << "initialize critical section returned" << endl;
    wtsc = NULL;
    wtsc_old = NULL;
    limitFileSize = false;
    maximumBytesToWriteInOneFile = 2000000000;
    os << "init returned " << endl;
}

wtscWrapper::wtscWrapper() {
    init();
}

wtscWrapper::~wtscWrapper() {
    if (wtsc != NULL) {
        wtsc->finishRecording();
        wtsc->closeOutputFile();
        delete (wtsc);
    }
    if (wtsc_old != NULL) {
        wtsc_old->finishRecording();
        wtsc_old->closeOutputFile();
        delete(wtsc_old);
    }
    DeleteCriticalSection(&protectedAction);
}

void wtscWrapper::enterCS() {
    EnterCriticalSection(&protectedAction);
}

void wtscWrapper::leaveCS() {
    LeaveCriticalSection(&protectedAction);
}


wtscWrapper::wtscWrapper(const char *fname, int thresholdAboveBackground, int smallDimMinSize, int lgDimMinSize, int keyFrameInterval, double frameRate) {
    init();
    std::ofstream os("c:\\wtscwrapperargs.txt");
    os << "fname = " << fname << endl;
    os << "thresholdAboveBackground = " << thresholdAboveBackground << endl;
    os << "smallDimMinSize = " << smallDimMinSize << endl;
    os << "lgDimMinSize = " << lgDimMinSize << endl;
    os << "keyFrameInterval = " << keyFrameInterval << endl;
    os << "frameRate = " << frameRate << endl;
    
    this->thresholdAboveBackground = thresholdAboveBackground;
    this->smallDimMinSize = smallDimMinSize;
    this->lgDimMinSize = lgDimMinSize;
    this->keyFrameInterval = keyFrameInterval;
    this->frameRate = frameRate;
    string fn(fname);
    size_t ind = fn.find_last_of('.');
    filestub = fn.substr(0,ind);
    ext = fn.substr(ind+1);    
    
    os << "filestub = " << filestub << endl;
    os << "ext = " << ext << endl;
    
    
    newStackWriter();       
    assert (wtsc != NULL);
    os << "wtsc created and points to " << (intptr_t) wtsc << endl;
    
}

wtscWrapper::wtscWrapper(const char *fstub, const char *ext, int thresholdAboveBackground, int smallDimMinSize, int lgDimMinSize, int keyFrameInterval, double frameRate, uint64_t maxBytesToWrite) {
    init();
    this->thresholdAboveBackground = thresholdAboveBackground;
    this->smallDimMinSize = smallDimMinSize;
    this->lgDimMinSize = lgDimMinSize;
    this->keyFrameInterval = keyFrameInterval;
    this->frameRate = frameRate;
    maximumBytesToWriteInOneFile = maxBytesToWrite;
    limitFileSize = true;
    filestub = string(fstub);
    this->ext = string(ext);
    newStackWriter();
    assert (wtsc != NULL);
   
    
}

void wtscWrapper::newStackWriter() {
    ofstream os("c:\\newstackwriter.txt");
    os << "creating wtsc" << endl;
    wtsc = new WindowsThreadStackCompressor();
    assert (wtsc != NULL);
    os << "wtsc created and points to " << (intptr_t) wtsc << endl;
    stringstream ss;
    if (limitFileSize) {
        os << "limit file size is true" << endl;
        
        ss << filestub << "-" << setw(3) << setfill('0') << fileNumber << "." << ext;
       // fname = ss.str();
        fileNumber++;
    } else {
        os << "limit file size is false" << endl;
        ss << filestub << "." << ext;
    }
    
    os << "fname = " << ss.str() << endl;
    
    wtsc->setOutputFileName(ss.str().c_str());
    
    os << "set output filename returned ok " << endl;
    wtsc->setIntervals(keyFrameInterval, 1);
    
    os << "set intervals passed " << endl;
    wtsc->setThresholds(0, thresholdAboveBackground, smallDimMinSize, lgDimMinSize);
    os << "about to set frame rate " << endl;
    wtsc->setFrameRate(frameRate);
    os << "about to call start threads " << endl;
    wtsc->startThreads();
    os << "new stack writer completed OK" << endl;
}

int wtscWrapper::addFrame (void *ipl_im) {
    if (wtsc == NULL) {
        return -2;
    }
    if (ipl_im == NULL) {
        return -3;
    }
    enterCS();
    if (wtsc_old != NULL) {
        if (wtsc_old->nothingLeftToCompressOrWrite()) {
            wtsc_old->closeOutputFile();
            delete(wtsc_old);
            wtsc_old == NULL;
        }
    }
    if (limitFileSize && wtsc->numBytesWritten() >= (0.99*maximumBytesToWriteInOneFile)) {
        if (wtsc_old != NULL) {
            wtsc_old->finishRecording();
            wtsc_old->closeOutputFile();
            delete (wtsc_old);
        }
        wtsc_old = wtsc;
        wtsc_old->goIdle();
        newStackWriter();
    }
    md.replaceData("frameAddedTimeStamp", tim.getElapsedTimeInMilliSec());
    wtsc->newFrame((IplImage *) ipl_im, md.copy());
    md.clear();
    leaveCS();
    return 0;
}

int wtscWrapper::setMetaData(char* fieldname, double fieldvalue) {
    
    enterCS();
    md.replaceData(string(fieldname), fieldvalue);
    leaveCS();
    return 0;
}

int wtscWrapper::startRecording (int nframes) {     
    if (wtsc == NULL) {
        return -1;
    }
    enterCS();
    tim.start();
    wtsc->startRecording(nframes);
    leaveCS();
    return 0;
}

int wtscWrapper::stopRecording() {
    if (wtsc == NULL) {
        return -1;
    }
    enterCS();
    wtsc->stopRecording();
    leaveCS();
    return 0;
}

int64_t wtscWrapper::numBytesWritten () {
    if (wtsc == NULL) {
        return -1;
    }
    enterCS();
    int64_t nbr = wtsc->numBytesWritten();
    leaveCS();
    return nbr;
}
int wtscWrapper::getTimingStatistics (double *avgAddTime, double *avgCompressTime, double *avgWriteTime) {
    if (wtsc == NULL) {
        return -1;
    }
    enterCS();
    if (avgAddTime != NULL) {
        *avgAddTime = wtsc->NonthreadedTimer().getStatistics("adding frame to stack");;
    }
    if (avgCompressTime != NULL) {
        *avgCompressTime = wtsc->CompressionThreadTimer().getStatistics("compressing a frame");
    }
    if (avgWriteTime != NULL) {
        *avgWriteTime =  (wtsc->WritingThreadTimer().getStatistics("writing a stack to disk") + wtsc->WritingThreadTimer().getStatistics("deleting written stack from memory"))/wtsc->getKeyFrameInterval();
    }
    leaveCS();
    return 0;
}
int wtscWrapper::getNumStacksQueued (int *numToCompress, int *numToWrite) {
    if (wtsc == NULL) {
        return -1;
    }
    enterCS();
    int ntc, ntw;
    wtsc->numStacksWaiting(ntc, ntw);
    if (numToCompress != NULL) {
        *numToCompress = ntc;
    }
    if (numToWrite != NULL) {
        *numToWrite = ntw;
    }
    leaveCS();
    return 0;
}

int wtscWrapper::getTimingReport (char *dst, int maxchars) {
    if (wtsc == NULL) {
        return -1;
    }
    enterCS();
    string s = wtsc->generateTimingReport();
    if (dst != NULL) {
        s.copy(dst, maxchars);
    }
    leaveCS();
    return 0;
}
