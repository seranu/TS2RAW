CPPFLAGS=-g --std=c++11
all:
	g++ $(CPPFLAGS) main.cpp src/ts_extractor.cpp src/ts_packet.cpp src/utils.cpp src/pes_stream.cpp src/pes_packet.cpp src/ts_reader.cpp -Iinclude -o build/extract.out
	g++ $(CPPFLAGS) test/tests.cpp src/ts_extractor.cpp src/ts_packet.cpp src/utils.cpp src/pes_stream.cpp src/pes_packet.cpp src/ts_reader.cpp -Iinclude -o build/test.out
test:
	g++ $(CPPFLAGS) test/tests.cpp src/ts_extractor.cpp src/ts_packet.cpp src/utils.cpp src/pes_stream.cpp src/pes_packet.cpp src/ts_reader.cpp -Iinclude -o build/test.out
clean:
	rm build/*