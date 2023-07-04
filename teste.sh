#!/bin/bash
while IFS= read -r line; do
    echo $line >> result.txt
    ./main $line >> result.txt
    echo ' ' >> result.txt
done < "$1"