CXXFLAGS=-fpermissive

SRCS=jack-client-test.cc
OBJS=$(SRCS:.cc=.o)
BIN=jack-client-test

$(BIN): $(OBJS)
	$(CXX) $(LDFLAGS) -o $(BIN) $(OBJS) $(LOADLIBES) $(LDLIBS) -ljack -ljackserver

clean:
	rm -f $(OBJS) $(BIN)
