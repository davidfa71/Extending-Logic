#!/bin/bash
echo "Processing "$1
rm ${1%.xml}-noXOR.log >/dev/null 2>&1
../code/bin/splot2model $1 >>${1%.xml}-noXOR.log 2>&1
mv ${1%.xml}.exp ${1%.xml}-noXOR.exp
../code/bin/Logic2BDD  -line-length 70 -min-nodes 100000 -constraint-reorder minspan -base ${1%.xml}-noXOR  -cudd -no-static-comp ${1%.xml}.var ${1%.xml}-noXOR.exp >>${1%.xml}-noXOR.log 2>&1 &
rm ${1%.xml}-XOR.log >/dev/null 2>&1
../code/bin/splot2model -use-XOR  $1 >>${1%.xml}-XOR.log 2>&1
mv ${1%.xml}.exp ${1%.xml}-XOR.exp
../code/bin/Logic2BDD -line-length 70 -min-nodes 100000 -constraint-reorder minspan -base ${1%.xml}-XOR  -cudd -no-static-comp ${1%.xml}.var ${1%.xml}-XOR.exp >>${1%.xml}-XOR.log 2>&1 
wait
rm *.data *.reorder >/dev/null 2>&1 
