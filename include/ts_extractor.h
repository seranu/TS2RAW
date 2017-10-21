#ifndef _TS_EXTRACTOR_H_
#define _TS_EXTRACTOR_H_

#include <string>

namespace ts2raw {

// class that can be used to separate video and 
// audio data from input TS stream file
class TSExtractor {
public:
    
    // Extract video and audio raw data from TS stream  
    //   <aInputFilename>        file path of input TS stream file
    //   <aOutputVideoFilename>  file path of output video file
    //   <aOutputAudioFilename>  file path of output audio file
    static int Extract(
        const std::string& aInputFilename,
        const std::string& aOutputVideoFilename,
        const std::string& aOutputAudioFilename
    );

private:
};

}

#endif //_TS_EXTRACTOR_H_