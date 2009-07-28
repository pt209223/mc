
INCLUDE := -I invaders -I .

#  -march=native
CXXFLAGS := -Wall -Wextra -ggdb -O3 -static \
  -fomit-frame-pointer -frename-registers -ffast-math -DNDEBUG

CXXFLAGS_DEBUG := -Wall -Wextra -ggdb -fno-inline -DDEBUG

all: engine judge benchmark

debug: engine_debug judge_debug benchmark_debug

engine engine_debug judge judge_debug: invaders/*.h

engine: engine.cc
	g++ $(CXXFLAGS) $(INCLUDE) -o engine engine.cc

engine_debug: engine.cc
	g++ $(CXXFLAGS_DEBUG) $(INCLUDE) -o engine_debug engine.cc

judge: judge.cc gtp_server.cc
	g++ $(CXXFLAGS) $(INCLUDE) -o judge judge.cc gtp_server.cc

judge_debug: judge.cc gtp_server.cc
	g++ $(CXXFLAGS_DEBUG) $(INCLUDE) -o judge_debug judge.cc gtp_server.cc

benchmark: benchmark.cc
	g++ $(CXXFLAGS) $(INCLUDE) -o benchmark benchmark.cc

benchmark_debug: benchmark.cc
	g++ $(CXXFLAGS_DEBUG) $(INCLUDE) -o benchmark_debug benchmark.cc

clean:
	rm -f *.a *.o core core.* engine engine_debug judge judge_debug benchmark benchmark_debug

