#!/bin/sh
# Author: Zixuan Zhu
writefile=$1
writestr=$2
varnum=$#
if [ "$varnum" -ne "2" ]; then 
    echo "Place enter two paras"
    exit 1
fi

dir=$(dirname "$writefile")

mkdir -p "$dir" || {
    echo "Failed to create directory"
    exit 1
}

echo "$writestr" > "$writefile" || {
    echo "Failed to write file"
    exit 1
}