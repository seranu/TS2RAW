#include "ts_extractor.h"
#include <iostream>

int main(int argc, char *argv[]) {
  if (argc == 4) {
    try {
	  ts2raw::TSExtractor::Extract(argv[1], argv[2], argv[3]);
	  std::cout << "Success!\n";
    } catch (const ts2raw::utils::TSException &ex) {
      std::cout << "Exception raised during extract; Message: " << ex.what()
                << "\n";
      return -1;
    }
  } else {
    std::cout << "Usage: " << argv[0]
              << " <input_ts_file> <output_video_file> <output_audio_file>\n";
    return -1;
  }
  return 0;
}