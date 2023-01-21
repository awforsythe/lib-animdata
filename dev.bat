@echo off
rem Run the supported build environment in a container.
rem From this prompt, you can run 'make test' etc. as normal.
docker run --rm -it -v %~dp0:/usr/src/app -w /usr/src/app gcc:9.5
