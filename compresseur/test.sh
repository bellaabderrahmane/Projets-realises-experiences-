#!/bin/bash

# Define the directory where the files are located
IMG_TEST="IMG_TEST"

# Check if the decompressor executable exists in the current directory
if [ ! -f "./QTC.lossless/decompreseur" ]; then
    echo "Error: decompreseur executable not found in the current directory!"
    exit 1
fi

# Check if the IMG_TEST directory exists
if [ ! -d "$IMG_TEST" ]; then
    echo "Error: IMG_TEST directory not found!"
    exit 1
fi

# Loop through each file in the IMG_TEST directory
for file in "$IMG_TEST"/*; do
    # Check if it's a regular file (not a directory)
    if [ -f "$file" ]; then
        # Get the filename (without the path)
        filename=$(basename "$file")
        echo "Processing file: $filename"
        
        # Pass only the filename to the decompressor
        ./QTC.lossless/decompreseur "$filename"
    fi
done

echo "All files processed."
