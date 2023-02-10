@echo off
rem Build a WASM module using the emscripten/emsdk Docker container.
docker build -f wasm.Dockerfile -t lib-animdata-wasm . && docker run --rm -v %~dp0\lib\wasm:/build/lib/wasm lib-animdata-wasm
