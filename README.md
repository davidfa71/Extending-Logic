**Software artifact accompanying the paper "Using Extended Logical Primitives for
Efficient BDD Building" submitted to the IEEE Access journal**

The artifact has been tested on Linux and macOS.

It requires recent versions of flex, bison and gperf. In macOS you can get then using brew or mac ports. You can get them in Linux by typing:
```
sudo apt install flex bison gperf libfl-dev

```
Compilation instructions:

```
cd code
make
<enter root password to install CUDD libraries in /usr/local/lib>
cd ..
```

Use scripts at will. Examples:
./runAll.sh          -- This will translate all the feature models to standard and extended logic, build the corresponding BDDs and
                        print 3 tables corresponding to the number of clauses, the translation times and the BDD building times
                        respectively. All the times are in milliseconds.

./runAll-2threads.sh -- Same thing as runAll but instead of running everything in parallel, it uses only 2 threads. Useful if you
                        only can/want use 2 threads at a time.

./gatherResults.sh   -- The script to create the tables explained in the previous line and also the .csv files with the same info

./cleanResults.sh    -- A script to delete the temporary files created in the experiments, including the translations and the logs

After running runAll.sh, you will find several files in the directories corresponding to each project tested:

<name>-noXOR.log    -- A log of the standard translation + building of the BDD
<name->-noXOR.var   -- The names of the variables in the system in order of appearance
<name>-noXOR.exp    -- The clauses in the standard translation of the system
<name>-noXOR.ord    -- The variables in the system as ordered by the static heuristic to start building the BDD
<name>-noXOR.dddmp  -- The BDD for the standard translation in dddmp format

<name>.dimacs       -- The standard translation in DIMACS format

<name>-XOR.log      -- A log of the extended translation + building of the BDD
<name->-XOR.var     -- The names of the variables in the system in order of appearance
<name>-XOR.exp      -- The clauses in the extended translation of the system
<name>-XOR.ord      -- The variables in the system as ordered by the static heuristic to start building the BDD
<name>-XOR.dddmp    -- The BDD for the extended translation in dddmp format

