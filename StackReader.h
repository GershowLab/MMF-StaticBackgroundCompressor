/* 
 * File:   StackReader.h
 * Author: Marc
 *
 * Created on October 27, 2010, 4:32 PM
 *
 * used to read compressed stacks (generated by LinearStackCompressor or its subclasses)
 * from disk
 * this is the highest level reading function in the library;
 *
 * create the stackreader by passing a file name
 * access frames using the getFrame command
 * generate a meta data file using getSupplementalData
 *
 * in separate projects:
 * see also the supplemental dll wrapper: StackReaderWrapper
 * see also the example movie player: StackPlayer
 *
 * example usage:
 *   StackReader sr("\\\\labnas2\\Phototaxis\\exponentialstack.mmf"); //open stack for reading
 *   sr.createSupplementalDataFile("c:\\testcs5_dat.dat"); //create a metadata file
 *   sr.playMovie(); //play a movie on the screen
 */

#ifndef STACKREADER_H
#define	STACKREADER_H

#include "StaticBackgroundCompressor.h"
#include "ExtraDataWriter.h"
#include <map>
#include <string>
#include <cv.h>

class StackReader {
public:
    StackReader();
    StackReader(const char *fname);
    virtual ~StackReader();
    virtual void setInputFileName (const char *fname);
    virtual void openInputFile ();
    virtual void closeInputFile ();

    virtual void getBackground (int frameNum, IplImage **dst, int frameRange = 0);
    virtual void getFrame (int frameNum, IplImage **dst);
    virtual void annotatedFrame (int frameNum, IplImage **dst);
    virtual void playMovie (int startFrame = 0, int endFrame = -1, int delay_ms = 50, char *windowName = NULL, bool annotated = true);

    virtual CvSize getImageSize ();

    virtual void createSupplementalDataFile(const char *fname);
    virtual ExtraDataWriter *getSupplementalData();

    virtual inline bool dataFileOk(){
        return (infile != NULL && !infile->fail());
    }

    virtual inline int getTotalFrames () {
        return totalFrames;
    }

    virtual inline bool isError() {
        return iserror;
    }

    virtual inline std::string getError() {
        return errormessage;
    }

    virtual std::string diagnostics();

    virtual CvRect getLargestROI ();

    /* virtual void decimateStack(const char *outputname, int decimationCount = 2);
     * writes out a copy of the compressed movie, but only a subset of the frames
     *
     * outputname - name of the file to save the new movie in (cannot be the same as the old file name)
     * decimationCount - factor to reduce the file size by (e.g. decimation count of 4 means to record every 4th frame)
     *
     * returns 0 on sucess, < 0 on error
     */
    virtual int decimateStack(const char *outputname, int thresholdAboveBackground, int smallDimMinSize, int lgDimMinSize, int decimationCount = 2);

protected:
    std::string fname;
    std::ifstream *infile;
    std::map<int, std::ifstream::pos_type> keyframelocations; //start frame, place in file

    int totalFrames;
    int startFrame;
    int endFrame;
    StaticBackgroundCompressor *sbc;

    virtual void init();

    virtual void parseInputFile();
    virtual void setSBC(int frameNum);
    virtual inline void setError (std::string err) {
        iserror = true;
        errormessage = err;
    }

    CvRect validROI;

    bool iserror;
    std::string errormessage;

private:
      StackReader(const StackReader& orig);
  
};

#endif	/* STACKREADER_H */

