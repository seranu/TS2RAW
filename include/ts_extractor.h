#ifndef _TS_EXTRACTOR_H_
#define _TS_EXTRACTOR_H_

#include <string>

namespace ts2raw {

class TSExtractor {
public:
    
    static int Extract(
        const std::string& aInputFilename,
        const std::string& aOutputVideoFilename,
        const std::string& aOutputAudioFilename
    );

private:
};

}

#endif //_TS_EXTRACTOR_H_