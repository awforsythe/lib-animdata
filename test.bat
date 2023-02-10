@echo off
rem Build and run tests in a container, invoked directly from a Windows CLI.
docker build -f test.Dockerfile -t lib-animdata-test . && docker run --rm lib-animdata-test
