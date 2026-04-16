#!/bin/bash

i=0
while true
do
    i=$((i + 1))
    j=$((i * i))
    k=$((j % 1000))
done
