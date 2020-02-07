#!/bin/bash
echo "Processing "$1
rm ${1}-noXOR.log >/dev/null 2>&1
../code/bin/Kconfig2Logic -score sifting -threshold 0 -onlybool -noblocks -score sifting -onlybool -toCNF ../systems/$1 >${1}-noXOR.log 2>&1
mv $1.exp $1-noXOR.exp
mv $1.ord $1-noXOR.ord
mv $1.var $1-noXOR.var
sort $1-noXOR.exp | uniq >temp
mv temp $1-noXOR.exp
../code/bin/Logic2BDD -score file $1-noXOR.ord -line-length 70 -min-nodes 100000 -reorder-method CUDD_REORDER_SIFT -constraint-reorder minspan -base ${1}-noXOR  -cudd -no-static-comp ${1}-noXOR.var ${1}-noXOR.exp >>${1}-noXOR.log 2>&1 &
rm ${1}-XOR.log >/dev/null 2>&1
../code/bin/Kconfig2Logic -onlybool -noblocks -score sifting -threshold 0  -onlybool -use-XOR -toCNF ../systems/$1 >${1}-XOR.log 2>&1
mv $1.exp $1-XOR.exp
sort $1-XOR.exp | uniq >temp
mv temp   $1-XOR.exp
mv $1.var $1-XOR.var
mv $1.ord $1-XOR.ord
../code/bin/Logic2BDD -score file $1-XOR.ord -line-length 70 -min-nodes 100000 -reorder-method CUDD_REORDER_SIFT -constraint-reorder minspan -base ${1}-XOR  -cudd -no-static-comp ${1}-XOR.var ${1}-XOR.exp >>${1}-XOR.log 2>&1
wait
rm *.data *.reorder >/dev/null 2>&1
