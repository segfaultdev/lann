#!/usr/bin/sh

gcc $(find . -name "*.c") -O3 -Iinclude -Wall -o lann
