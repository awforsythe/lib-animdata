# By default, 'make' will build and run tests
.PHONY: test wasm clean
all: test

# Our static library is built to lib/ from the files in src/
LIB_X64=lib/x64/libanimdata.a
LIB_WASM=lib/wasm/libanimdata.js
SRCS=$(wildcard src/*.cpp)

# We can build object code to obj/ for each TU by invoking g++ (for x64 builds, e.g. 
# Linux static lib or test binary) or emcc (for wasm builds with emscripten)
OBJS_X64=$(subst src/,obj/x64/,$(subst .cpp,.o,$(SRCS)))
obj/x64/%.o: src/%.cpp
	@mkdir -p obj/x64/
	$(CXX) -o $@ -I include -c src/$(basename $(@F)).cpp

OBJS_WASM=$(subst src/,obj/wasm/,$(subst .cpp,.o,$(SRCS)))
obj/wasm/%.o: src/%.cpp
	@mkdir -p obj/wasm/
	emcc -o $@ -I include -c src/$(basename $(@F)).cpp

# Once object files are built, we can link them to a static library for x64 builds, or
# generate our final WebAssembly module
$(LIB_X64): $(OBJS_X64)
	@mkdir -p $(dir $(LIB_X64))
	ar rsv $(LIB_X64) $(OBJS_X64)

$(LIB_WASM): $(OBJS_WASM)
	@mkdir -p $(dir $(LIB_WASM))
	emcc -lembind $(OBJS_WASM) -sEXPORTED_RUNTIME_METHODS=cwrap -o $(LIB_WASM)

# We can build bin/test from the source in tests/, linking against the lib
TESTSRCS=$(wildcard tests/*.h)
TESTBIN=bin/test
$(TESTBIN): $(LIB_X64) $(TESTSRCS) tests/main.cpp
	@mkdir -p bin
	$(CXX) -o bin/test -I include -I tests tests/main.cpp $(LIB_X64)

# 'make test' will build the test binary and run it, to test the source
test: $(TESTBIN)
	@$(TESTBIN)

# 'make wasm' will compile the library to a WebAssembly module
wasm: $(LIB_WASM)

# 'make clean' will delete all build artifacts
clean:
	rm -rf obj/
	rm -rf lib/
	rm -rf bin/
