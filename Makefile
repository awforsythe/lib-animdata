# By default, 'make' will build and run tests
.PHONY: test clean
all: test

# Our static library is built to lib/ from the files in src/
LIB=lib/libanimdata.a
SRCS=$(wildcard src/*.cpp)

# We can build object code to obj/ for each TU by invoking g++ on its source file
OBJS=$(subst src/,obj/,$(subst .cpp,.o,$(SRCS)))
obj/%.o: src/%.cpp
	@mkdir -p obj
	$(CXX) -o $@ -I include -c src/$(basename $(@F)).cpp

# Once object files are built, we can link them to a static library
$(LIB): $(OBJS)
	@mkdir -p lib
	ar rsv $(LIB) $(OBJS)

# We can build bin/test from the source in tests/, linking against the lib
TESTSRCS=$(wildcard tests/*.h)
TESTBIN=bin/test
$(TESTBIN): $(LIB) $(TESTSRCS) tests/main.cpp
	@mkdir -p bin
	$(CXX) -o bin/test -I include -I tests tests/main.cpp $(LIB)

# 'make test' will build the test binary and run it, to test the source
test: $(TESTBIN)
	@$(TESTBIN)

# 'make clean' will delete all build artifacts
clean:
	rm -rf obj/
	rm -rf lib/
