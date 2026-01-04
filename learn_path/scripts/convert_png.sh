#!/bin/bash

mkdir -p images

for file in *.ppm; do
	ffmpeg -i "$file" "images/${file%.pmm}.png"
done