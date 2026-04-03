#!/bin/sh
# Author: Zixuan Zhu
filesdir=$1
searchstr=$2
varnum=$#
if [ "$varnum" -ne "2" ]; then 
    echo "Please enter two parameters"
    exit 1
fi

if [ -d "$filesdir" ]; then
    files=$(find "$filesdir" -type f | wc -l)
    filesmatch=$(grep -r "$searchstr" "$filesdir" | wc -l)
    echo "The number of files are $files and the number of matching lines are $filesmatch"
else 
    echo "Not a file dir"
    exit 1
fi