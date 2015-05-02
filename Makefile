vpath %.cc src
vpath %.h src

HEADERS = parser.h socket.h client.h server.h
SOURCES = main.cc parser.cc socket.cc


TARGET := udp_echo
BINDIR := bin/
BINDIR := $(patsubst %/, %, $(BINDIR))/
TARGET := $(BINDIR)$(TARGET)

BUILDDIR := obj/
BUILDDIR := $(patsubst %/, %, $(BUILDDIR))/

OBJECTS = $(addprefix $(BUILDDIR), $(patsubst %.cc, %.o, $(SOURCES)))

CXXFLAGS += -std=c++11 -Wextra -O2

BOOSTFLAGS = -L/usr/local/lib -lboost_program_options

all: $(BUILDDIR) $(BINDIR) $(TARGET)

$(BUILDDIR): 
	mkdir -p $(BUILDDIR)

$(BINDIR):
	mkdir -p $(BINDIR)

$(TARGET): $(OBJECTS)
	$(CXX) $(CXXFLAGS) $(BOOSTFLAGS) $^ -o $@

$(BUILDDIR)%.o: %.cc $(HEADERS)
	$(CXX) -c $(CXXFLAGS) $< -o $@


test: all
	echo 127.0.0.1 5555 > $(BINDIR)client_addr
	./$(TARGET) -l -p 5555 & echo $$! > $(BINDIR)pid.tmp

.PHONY: clean
clean:
	$(RM) $(OBJECTS)
	@if [ -d $(BUILDDIR) ]; then rmdir --ignore-fail-on-non-empty $(BUILDDIR); fi
	@echo $(RM) $(BUILDDIR)

.PHONY: cleanall
cleanall: clean
	$(RM) $(TARGET)
	@if [ -d $(BINDIR) ]; then rmdir --ignore-fail-on-non-empty $(BINDIR); fi
	@echo $(RM) $(BINDIR)
