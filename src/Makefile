HEADERS = parser.h socket.h client.h
SOURCES = main.cc parser.cc socket.cc

CXXFLAGS += -std=c++11 -Wextra

BOOSTFLAGS = -lboost_program_options

udp_echo: $(SOURCES) $(HEADERS)
	$(CXX) $(CXXFLAGS) $(BOOSTFLAGS) $(SOURCES) -o $@
