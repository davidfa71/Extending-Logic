#!/bin/bash
SYSTEMS="Toybox axTLS BusyBox uClibc Fiasco Dell EmbToolKit Automotive02"
for sys in $SYSTEMS
  do
  rm $sys/$sys.tree $sys/$sys.var $sys/$sys.dimacs 2>/dev/null
  rm $sys/$sys-noXOR.var $sys/$sys-noXOR.exp  $sys/$sys-noXOR.ord  $sys/$sys-noXOR.reorder $sys/$sys-noXOR.data $sys/$sys-noXOR.dddmp $sys/$sys-noXOR.log 2>/dev/null
  rm $sys/$sys-noXOR.temp.dddmp $sys/$sys-noXOR.applied $sys/$sys-noXOR.unapplied 2>/dev/null
  rm $sys/$sys-XOR.var $sys/$sys-XOR.exp  $sys/$sys-XOR.ord  $sys/$sys-XOR.reorder $sys/$sys-XOR.data $sys/$sys-XOR.dddmp $sys/$sys-XOR.log 2>/dev/null
  rm $sys/$sys-XOR.temp.dddmp $sys/$sys-XOR.applied $sys/$sys-XOR.unapplied 2>/dev/null
  done
rm *.csv 2>/dev/null
