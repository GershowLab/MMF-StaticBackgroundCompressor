/* 
 * File:   StackWriterWrapper.cpp
 * Author: Marc
 *
 * Created on March 16, 2011, 3:07 PM
 * 
 * (C) Marc Gershow; licensed under the Creative Commons Attribution Share Alike 3.0 United States License.
 * To view a copy of this license, visit http://creativecommons.org/licenses/by-sa/3.0/us/ or send a letter to
 * Creative Commons, 171 Second Street, Suite 300, San Francisco, California, 94105, USA.
 */

#include <cstdlib>
#include <limits>

#include "wtscWrapper.h"
#include "NameValueMetaData.h"
#include "StackWriterWrapper.h"
#include "time.h"
using namespace std;

void *createBrightFieldStackWriter (const char *fname, int thresholdAboveBackground, int smallDimMinSize, int lgDimMinSize, int keyFrameInterval, double frameRate) {
    wtscWrapper *ww = new wtscWrapper();
    ww->wtsc.setOutputFileName(fname);
    ww->wtsc.setIntervals(keyFrameInterval, 1);
    ww->wtsc.setThresholds(0, thresholdAboveBackground, smallDimMinSize, lgDimMinSize);
    ww->wtsc.setFrameRate(frameRate);
    ww->wtsc.startThreads();
    return (void *) ww;
}

 void destroyStackWriter (void *sw) {
     wtscWrapper *ww = (wtscWrapper *) sw;
     
     ww->wtsc.finishRecording(); //these are already done by deleting ww, but it's not the biggest deal to short-cut the process
     ww->wtsc.closeOutputFile(); //to make clear this happens here

     delete (ww);
 }

int addFrame (void *sw, void *ipl_im) {
    if (sw == NULL) {
        return -1;
    }
    if (ipl_im == NULL) {
        return -1;
    }
    wtscWrapper *ww = (wtscWrapper *) sw;
    ww->enterCS();
    ww->md.replaceData("frameAddedTimeStamp", ww->tim.getElapsedTimeInMilliSec());
    ww->wtsc.newFrame((IplImage *) ipl_im, ww->md.copy());
    ww->md.clear();
    ww->leaveCS();
    return 0;
}

int setMetaData(void* sw, char* fieldname, double fieldvalue) {
    if (sw == NULL) {
        return -1;
    }
    wtscWrapper *ww = (wtscWrapper *) sw;
    ww->enterCS();
    ww->md.replaceData(string(fieldname), fieldvalue);
    ww->leaveCS();
    return 0;
}

int startRecording (void *sw, int nframes) {
     if (sw == NULL) {
        return -1;
    }
    wtscWrapper *ww = (wtscWrapper *) sw;
    ww->enterCS();
    ww->tim.start();
    ww->wtsc.startRecording(nframes);
    ww->leaveCS();
    return 0;
}

int stopRecording (void *sw) {
     if (sw == NULL) {
        return -1;
    }
    wtscWrapper *ww = (wtscWrapper *) sw;
    ww->enterCS();
    ww->wtsc.stopRecording();
    ww->leaveCS();
    return 0;
}

int64_t numBytesWritten (void *sw) {
    if (sw == NULL) {
        return -1;
    }
    wtscWrapper *ww = (wtscWrapper *) sw;
    ww->enterCS();
    int64_t nbr = ww->wtsc.numBytesWritten();
    ww->leaveCS();
    return nbr;
}
uint64_t maxBytesSupported() {
    return numeric_limits<streamoff>::max();
}
int getTimingStatistics (void *sw, double *avgAddTime, double *avgCompressTime, double *avgWriteTime) {
     if (sw == NULL) {
        return -1;
    }
    wtscWrapper *ww = (wtscWrapper *) sw;
    ww->enterCS();
    if (avgAddTime != NULL) {
        *avgAddTime = ww->wtsc.NonthreadedTimer().getStatistics("adding frame to stack");;
    }
    if (avgCompressTime != NULL) {
        *avgCompressTime = ww->wtsc.CompressionThreadTimer().getStatistics("compressing a frame");
    }
    if (avgWriteTime != NULL) {
        *avgWriteTime =  (ww->wtsc.WritingThreadTimer().getStatistics("writing a stack to disk") + ww->wtsc.WritingThreadTimer().getStatistics("deleting written stack from memory"))/ww->wtsc.getKeyFrameInterval();
    }
    ww->leaveCS();
    return 0;
}
int getNumStacksQueued (void *sw, int *numToCompress, int *numToWrite) {
     if (sw == NULL) {
        return -1;
    }
    wtscWrapper *ww = (wtscWrapper *) sw;
    ww->enterCS();
    int ntc, ntw;
    ww->wtsc.numStacksWaiting(ntc, ntw);
    if (numToCompress != NULL) {
        *numToCompress = ntc;
    }
    if (numToWrite != NULL) {
        *numToWrite = ntw;
    }
    ww->leaveCS();
    return 0;
}

int getTimingReport (void *sw, char *dst, int maxchars) {
    if (sw == NULL) {
        return -1;
    }
    wtscWrapper *ww = (wtscWrapper *) sw;
    ww->enterCS();
    string s = ww->wtsc.generateTimingReport();
    if (dst != NULL) {
        s.copy(dst, maxchars);
    }
    ww->leaveCS();
    return 0;
}
