#!/bin/bash
cd Toybox  
../runKconfig.sh Toybox &
cd ..
cd axTLS  
../runKconfig.sh axTLS &
cd ..
cd BusyBox
../runKconfig.sh BusyBox &
cd ..
cd Fiasco
../runKconfig.sh Fiasco &
cd ..
cd uClibc 
../runKconfig.sh uClibc &
cd ..
cd Dell
../runSPLOT.sh Dell.xml &
cd ..
cd Automotive02 
../runSPLOT.sh Automotive02.xml &
cd ..
cd EmbToolKit
../runKconfig.sh EmbToolKit &
cd ..
wait
./gatherResults.sh
