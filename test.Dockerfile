FROM gcc:9.5

WORKDIR /build

COPY Makefile .
COPY include include
COPY src src
COPY tests tests

CMD ["make", "test"]
