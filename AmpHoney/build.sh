#!/bin/bash
echo "[?] Building.."
gcc src/*.c -std=c99 -s -Wall -o bin/out
echo "[!] Built"
