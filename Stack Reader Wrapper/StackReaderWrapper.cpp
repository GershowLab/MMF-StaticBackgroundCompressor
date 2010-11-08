#include "StackReaderWrapper.h"
#include "cv.h"
#include "highgui.h"
#include "StackReader.h"
#include "LinearStackCompressor.h"
#include "BlankMetaData.h"
#include <sstream>
#include <ostream>

using namespace std;

void *createStackReader(const char* fname) {
  //  ofstream os("c:\\stackreadlog.txt");
    if (fname == NULL) {
   //     os << "fname was null " << endl;
        return NULL;
    }
 //   os << "trying to open " << fname << endl;
    StackReader *sr = new StackReader(fname);
    if (sr == NULL) {
 //       os << "sr is null" << endl;
        return NULL;
    }
    if (!sr->dataFileOk()) {
 //       os << "data file not OK" << endl;
        delete sr;
        return NULL;
    }
 //   os << "returning sr" << endl;
 //   os.close();
    return (void *) sr;
}

void destroyStackReader(void* SR) {
    StackReader *sr = (StackReader *) SR;
    if (sr != NULL) {
        delete sr;
    }
}

void *getFrame(void* SR, int frameNumber) {
    StackReader *sr = (StackReader *) SR;
    if (sr == NULL) {
        return NULL;
    }
    if (!sr->dataFileOk()) {
        return NULL;
    }
    IplImage *dst = NULL;

    sr->getFrame(frameNumber, &dst);
  //  ofstream os("c:\\stackreadlog.txt");
//    os << "dst = " << (unsigned long) dst << "  dst has width " << dst->width << " and height " << dst->height << endl;
 //   os.close();
    return (void *) dst;
}

void *getBackground(void* SR, int frameNumber, int frameRange) {
    StackReader *sr = (StackReader *) SR;
    if (sr == NULL) {
        return NULL;
    }
    if (!sr->dataFileOk()) {
        return NULL;
    }
    IplImage *dst = NULL;
    sr->getBackground(frameNumber, &dst, frameRange);
    return (void *) dst;
}

int getTotalFrames(void* SR) {
    StackReader *sr = (StackReader *) SR;
    if (sr == NULL || !sr->dataFileOk()) {
        return -1;
    }
    return sr->getTotalFrames();

}

void compressImageStack(const char* fstub, const char* extension, const char* outname, int startFrame, int endFrame, int diffThresh, int smallDimMinSize, int lgDimMinSize) {
//    string stub(fstub);
    LinearStackCompressor lsc;
    lsc.setThresholds(0, diffThresh, smallDimMinSize, lgDimMinSize);
    lsc.setIntervals(128, 1);
    lsc.setOutputFileName(outname);
    int nframes = endFrame - startFrame;
    lsc.startRecording(nframes);
    stringstream s;
    string ext(extension);
    if (ext.at(0) != '.') {
        ext.insert(0, 1, '.');
    }
    for (int j = startFrame; j < endFrame; ++j) {
        s.str("");
        s << fstub << j << ext;
        IplImage *im = cvLoadImage(s.str().c_str(), 0);
    //    assert(im != NULL);
        cout << "loaded image " << j <<"\n";
        BlankMetaData *bmd = new BlankMetaData;
        lsc.newFrame(im, bmd);
        cvReleaseImage(&im);
   //     cout << "added image to stack\n";
    }

    lsc.stopRecording();
}

