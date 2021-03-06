vpath %.cc src
vpath %.h src

HEADERS = parser.h socket.h client.h server.h
SOURCES = main.cc parser.cc socket.cc server.cc


TARGET := udp_echo
BINDIR := bin/
BINDIR := $(patsubst %/,%,$(BINDIR))/
TARGET := $(BINDIR)$(TARGET)

BUILDDIR := obj/
BUILDDIR := $(patsubst %/,%,$(BUILDDIR))/

OBJECTS = $(addprefix $(BUILDDIR), $(patsubst %.cc,%.o,$(SOURCES)))

CXXFLAGS += -std=c++11 -Wextra -O2

BOOSTFLAGS = -L/usr/local/lib -lboost_program_options

.PHONY: all
all: $(BUILDDIR) $(BINDIR) $(TARGET)

$(BUILDDIR): 
	mkdir -p $(BUILDDIR)

$(BINDIR):
	mkdir -p $(BINDIR)

$(TARGET): $(OBJECTS)
	$(CXX) $(CXXFLAGS) $(BOOSTFLAGS) $^ -o $@

$(BUILDDIR)%.o: %.cc $(HEADERS)
	$(CXX) -c $(CXXFLAGS) $< -o $@


.PHONY: check
check: all
	echo 127.0.0.1 5555 > $(BINDIR)client_addr
	echo 192.168.1.100 6666 >> $(BINDIR)client_addr
	echo 192.168.1.101 9999 >> $(BINDIR)client_addr
	./$(TARGET) -l -p 5555  & echo $$! > $(BINDIR)pid.tmp
	./$(TARGET) -l -p 6666  & echo $$! >> $(BINDIR)pid.tmp
	./$(TARGET) -i $(BINDIR)client_addr -t 1 -c 5 -s 64
	kill $$(cat $(BINDIR)pid.tmp) && rm $(BINDIR)pid.tmp
	rm $(BINDIR)client_addr

.PHONY: clean
clean:
	$(RM) $(OBJECTS)
	$(RM) -r $(BUILDDIR)

.PHONY: cleanall
cleanall: clean
	$(RM) $(TARGET)
	$(RM) -r $(BINDIR)
