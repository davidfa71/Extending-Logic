#!/bin/bash
SYSTEMS="Toybox axTLS BusyBox uClibc Fiasco Dell EmbToolKit Automotive02"
echo "Clauses"
printf "     System          no-XOR       XOR\n"
echo   "----------------    --------     ------"
echo "NAME;TYPE;NUM" >clauseNumbers.csv
for sys in $SYSTEMS
  do
        clausenoXOR=`wc -l $sys/$sys-noXOR.exp | awk '{ print $1 }'`
        clauseXOR=`wc -l $sys/$sys-XOR.exp  | awk '{ print $1 }'`
        printf "%15s %10d %10d\n" $sys $clausenoXOR $clauseXOR
        echo "$sys;standard;$clausenoXOR" >>clauseNumbers.csv
        echo "$sys;extended;$clauseXOR" >>clauseNumbers.csv
  done
echo
echo "Translation"
printf "     System          no-XOR       XOR\n"
echo   "----------------    --------     ------"
echo "NAME;TYPE;TIME" >translationTimes.csv
for sys in $SYSTEMS
  do
  transnoXOR=$(grep Time $sys/$sys-noXOR.log | awk '{ print $(NF-1)}' | sed 's/(//'| head -n 1)
  transXOR=$(grep Time $sys/$sys-XOR.log   | awk '{ print $(NF-1)}' | sed 's/(//'| head -n 1)
  printf "%15s %10d %10d\n" $sys $transnoXOR $transXOR
  echo "$sys;standard;$transnoXOR" >>translationTimes.csv
  echo "$sys;extended;$transXOR"   >>translationTimes.csv
  done
echo
echo "Building"
printf "     System          no-XOR       XOR\n"
echo   "----------------    --------     ------"
echo "NAME;TYPE;TIME" >buildTimes.csv
for sys in $SYSTEMS
  do
  buildnoXOR=$(grep Time $sys/$sys-noXOR.log | awk '{ print $(NF-1)}' | sed 's/(//'| tail -1)
  buildXOR=$(grep Time $sys/$sys-XOR.log   | awk '{ print $(NF-1)}' | sed 's/(//'| tail -1)
  printf "%15s %10d %10d\n" $sys $buildnoXOR $buildXOR
    echo "$sys;standard;$buildnoXOR}" >>buildTimes.csv
  echo "$sys;extended;$buildXOR"   >>buildTimes.csv
  done

