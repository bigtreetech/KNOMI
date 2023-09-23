#!/bin/bash
# This script will take an animated GIF and delete every other frame
# Accepts two parameters: input file and output file
# Usage: ./<scriptfilename> input.gif output.gif

# Make a optimized copy of the gif
gifsicle --colors 32 -o $2 -O=3 -i --crop-transparency --lossy=90 $1

# Get the number of frames
numframes=`gifsicle $1 -I | grep -P "\d+ images" --only-matching | grep -P "\d+" --only-matching`

# Deletion
let i=0
while [[ $i -lt $numframes  ]]; do
    rem=$(( $i % 2 ))
     
    if [ $rem -eq 0 ]
    then
        gifsicle $2 --delete "#"$(($i/2)) -o $2 
    fi

    let i=i+1 
done
