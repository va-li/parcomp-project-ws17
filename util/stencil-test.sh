#!/bin/bash

stencil=0
mode1=1
mode2=3

check_correct=true

if ${check_correct}; then
    ./parser_test -i ../util/input.txt -m ${stencil} -p ${mode1} > o1.txt
    ./parser_test -i ../util/input.txt -m ${stencil} -p ${mode2} > o2.txt
    diff o1.txt o2.txt > o3.txt

    echo "o1.txt" | xargs wc -l
    echo "o2.txt" | xargs wc -l
    echo "o3.txt" | xargs wc -l
fi

time ./parser_test -s -i ../util/input.txt -m ${stencil} -p ${mode1} #> /dev/null
echo "Time to run './parser_test -s -i ../util/input.txt -m ${stencil} -p ${mode1}'"
echo ""

time ./parser_test -s -i ../util/input.txt -m ${stencil} -p ${mode2} #> /dev/null
echo "Time to run './parser_test -s -i ../util/input.txt -m ${stencil} -p ${mode2}'"
