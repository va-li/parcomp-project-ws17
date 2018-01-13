import sys
import os
from copy import copy, deepcopy
import time

if len(sys.argv) != 2:
    print("There is not enough arguments.")
    exit(1)

filename = sys.argv[1]

#print("Opening file: {}".format(filename))

if not os.path.exists(filename):
    print("File does not exist.")
    exit(1)

file = open("input.txt", "r")

firstline = file.readline().split(';')

if len(firstline) != 3:
    print("The X;Y;Z is incorrect!")
    exit(1)

x = int(firstline[0])
y = int(firstline[1])
z = int(firstline[2])

if x == 0 or y == 0 or z == 0:
    print("X;Y;Z are not numbers or == 0")
    exit(1)

arr = [[[0 for k in range(x)] for j in range(y)] for i in range(z)]

i = 0
j = 0
k = 0

for line in file:
    arr[k][j][i] = float(line)
    if not i == x - 1:
        i += 1
    else:
        i = 0
        if not j == y - 1:
            j += 1
        else:
            j = 0
            k += 1

if not i == 0 or not j == 0 or not k == z:
    print("The number of element is not x*y*z")
    exit(1)

file.close()

print("The file is parsed successfully.")

def calc_one_stencil_27(org, copy, i, j, k):
    copy[k][j][i] = 0
    for a in range(-1, 2):
        for b in range(-1, 2):
            for c in range(-1, 2):
                copy[k][j][i] += org[k+a][j+b][i+c]
    copy[k][j][i] /= 27


def calc_one_stencil_7(org, copy, i, j, k):
    copy[k][j][i] = org[k][j][i]
    copy[k][j][i] += org[k][j+1][i]
    copy[k][j][i] += org[k][j-1][i]
    copy[k][j][i] += org[k][j][i+1]
    copy[k][j][i] += org[k][j][i-1]
    copy[k][j][i] += org[k+1][j][i]
    copy[k][j][i] += org[k-1][j][i]
    copy[k][j][i] /= 7

def copy_all(src, dest):
    for i in range(0, x):
        for j in range(0, y):
            for k in range(0, z):
                dest[i][j][k] = src[i][j][k]

def stencil_calculate(org, copy, func):
    for k in range(1, z-1):
        # Copy boundary values
        #for i in range(0, x):
        #   copy[k][0][i] = org[k][0][i]

        #for j in range(0, y):
        #    copy[k][j][0] = org[k][j][0]

        # Perform stencil update
        for i in range(1, x-1):
            for j in range(1, y-1):
                    func(org, copy, k, j, i)
    copy_all(copy, org)


stencil_seven = deepcopy(arr)
stencil_twentyseven = deepcopy(arr)


def print_mat(mat):
    for i in range(0, x):
        for j in range(0, y):
            for k in range(0, z):
                print("{:f}".format(mat[i][j][k]))

temp_arr = deepcopy(arr)
for i in range(0, 100):
    stencil_calculate(stencil_seven, temp_arr, calc_one_stencil_7)

print_mat(stencil_seven)

#temp_arr = deepcopy(arr)
#for i in range(0, 100):
#    stencil_calculate(stencil_twentyseven, temp_arr, calc_one_stencil_27)

