#!/bin/bash

# Just copy to util folder where the `data_generator` executable is and run it
# Set the values for each data you want to generate
values="10 30 50 80 100 150 200 250 300 400 500 600"

for i in ${values}
do
    ./data_generator -n ${i} -m ${i} -l ${i} -p 1 -o i${i}.txt
done
