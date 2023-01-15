FROM gcc:9.5

WORKDIR /usr/src/app

COPY Makefile .
COPY include include
COPY src src
COPY tests tests

RUN make
