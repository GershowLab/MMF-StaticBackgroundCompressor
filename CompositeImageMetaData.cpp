/* 
 * File:   CompositeImageMetaData.cpp
 * Author: Marc
 * 
 * Created on November 5, 2010, 10:00 AM
 */

#include "CompositeImageMetaData.h"
#include "ImageMetaDataLoader.h"

#include <vector>
#include <sstream>
#include <algorithm>
using namespace std;

CompositeImageMetaData::CompositeImageMetaData() {
}

CompositeImageMetaData::CompositeImageMetaData(const CompositeImageMetaData& orig) {
}

CompositeImageMetaData::~CompositeImageMetaData() {
    for(vector<ImageMetaData *>::iterator it = imd.begin(); it != imd.end(); ++it) {
        if (*it != NULL) {
          //  cout << "*it = " << (unsigned long) *it << endl;
          //  cout << "cimd: deleting a " << hex << (*it)->idCode()<<dec << endl;
            delete (*it);
            *it = NULL;
        }
    }
}

void CompositeImageMetaData::toDisk(std::ofstream& os)const {
    unsigned long id = idCode();
    //removeNulls();
    int len = imd.size();
    os.write((char *) &id, sizeof(id));
    os.write((char *) &len, sizeof(len));
    for(vector<ImageMetaData *>::const_iterator it = imd.begin(); it != imd.end(); ++it) {
        if (*it != NULL) {
            (*it)->toDisk(os);
        }
    }
}

void CompositeImageMetaData::removeNulls() {
    vector<ImageMetaData *>::iterator newend = remove(imd.begin(), imd.end(), (ImageMetaData *) NULL);
    imd.erase(newend, imd.end());
}

string CompositeImageMetaData::saveDescription()const{
    stringstream os;
    os << "Composite Meta Data: idcode (unsigned long) = " << hex << idCode() << dec << ", int number of Image Meta Datas stored, then each Meta Data in succession:\n";
    for(vector<ImageMetaData *>::const_iterator it = imd.begin(); it != imd.end(); ++it) {
        if (*it != NULL) {
            os << (*it)->saveDescription();
        }
    }
    return os.str();

}

void CompositeImageMetaData::addMetaData(ImageMetaData* md) {
    if (md != NULL) {
        imd.push_back(md);
    }
}

int CompositeImageMetaData::sizeOnDisk()const {
    int sod = sizeof(unsigned long) + sizeof(int);
    for(vector<ImageMetaData *>::const_iterator it = imd.begin(); it != imd.end(); ++it) {
        if (*it != NULL) {
            sod+= (*it)->sizeOnDisk();
        }
    }
    return sod;
}

CompositeImageMetaData *CompositeImageMetaData::fromFile(std::ifstream& is) {
    CompositeImageMetaData *cmd = new CompositeImageMetaData();
    int len;
    is.read((char *) &len, sizeof(len));
    for (int j = 0; j < len; ++j) {
        cmd->addMetaData(ImageMetaDataLoader::fromFile(is));
    }
    return cmd;

}

std::map<std::string, double> CompositeImageMetaData::getFieldNamesAndValues() const {
    map<std::string, double> fnav, snav;
    for (vector<ImageMetaData *>::const_iterator it = imd.begin(); it != imd.end(); ++it) {
        if (*it == NULL) {
            continue;
        }
        snav = (*it)->getFieldNamesAndValues();
        fnav.insert(snav.begin(), snav.end());
    }
    return fnav;
}

std::vector<const ImageMetaData *> CompositeImageMetaData::getMetaDataVector() {
    std::vector<const ImageMetaData *> v;
    v.insert(v.begin(), imd.begin(), imd.end());
    return v;
}

ImageMetaData *CompositeImageMetaData::clone() const{
    CompositeImageMetaData *cimd = new CompositeImageMetaData();
     for(vector<ImageMetaData *>::const_iterator it = imd.begin(); it != imd.end(); ++it) {
        if (*it != NULL) {
            cimd->imd.push_back((*it)->clone());
        }
    }
    return cimd;
}