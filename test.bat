@echo off
rem Build and run tests in a container, invoked directly from a Windows CLI.
docker build -t lib-animdata . && docker run --rm lib-animdata
