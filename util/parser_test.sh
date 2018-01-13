#!/bin/bash

./parser_test -i ../util/input.txt -m 0 > o1.txt
./parser_test -i ../util/input.txt -m 1 > o2.txt
diff o1.txt o2.txt > o3.txt

echo "o1.txt" | xargs wc -l
echo "o2.txt" | xargs wc -l
echo "o3.txt" | xargs wc -l

time ./parser_test -s -i ../util/input.txt -m 0 > /dev/null
time ./parser_test -s -i ../util/input.txt -m 1 > /dev/null
