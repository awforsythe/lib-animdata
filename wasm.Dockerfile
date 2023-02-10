FROM emscripten/emsdk:3.1.31

WORKDIR /build

COPY Makefile .
COPY include include
COPY src src

CMD ["make", "wasm"]
