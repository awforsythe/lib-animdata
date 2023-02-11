# lib-animdata

A simple library for storing and manipulating animation keyframe data.

On Linux or compatible: run `make test` to build and run tests; run `make clean` to
delete build artifacts. With the emscripten SDK installed, run `make wasm` to generate
a WebAssembly module.

On Windows: run `test` to build and run tests in Docker; run `wasm` to build a
WebAssembly module to `lib/wasm`; run `dev` to start an interactive development
environment in a Linux container.

To test the WebAssembly module after building, run `python -m http.server` and then
browse to http://localhost:8000 to load up a simple test page that loads the module and
runs an exported function. On that page, you should see:

- ad_example_func(2) = 4
