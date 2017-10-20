#include <iostream>
#include "ts_extractor.h"

int main(int argc, char* argv[]) {
	if (argc == 4) {
		int res = ts2raw::TSExtractor::Extract(argv[1], argv[2], argv[3]);
		std::cout << "Result: " << res << "\n";
	}
	else {
		std::cout << "Usage: " << argv[0] << " <input_ts_file> <output_audio_file> <output_video_file\n";
		return -1;
	}
	return 0;
}