#include <iostream>
#include <fstream>
#include <algorithm>
#include <stdlib.h>

#include <execinfo.h>
#include <signal.h>
#include <unistd.h>

#include "mytime.hpp"
#include "driver.hpp"

int main(int argc, char *argv[])
{
    long start = get_cpu_time();
    if (argc == 1) {
        std::cerr << "Kconfig: Creates a translation of a Kconfig system to Boolean logic" << std::endl;
        std::cerr << "Usage Kconfig2Logic [options] basedir" << std::endl;
        std::cerr << "Options            : " << std::endl;
        std::cerr << "-arch <arch>       : architecture (for the Linux kernel)" << std::endl;
        std::cerr << "-expand            : Flatten expressions, but not so much as in CNF form" << std::endl;
        std::cerr << "-nocomp            : Do not group variables in connected components for variable ordering" << std::endl;
        std::cerr << "-nosimplify        : Do not perform the simplificaction phase for the translation" << std::endl;
        std::cerr << "-noblocks          : Do not block together variables in choices for variable ordering purposes" << std::endl;
        std::cerr << "-onlybool          : Translate only boolean configs, ignore tristate, string, int and hex"
         << std::endl;
        std::cerr << "-suffix            : A suffix to add to the system name in the output .var and .exp" << std::endl;
        std::cerr << "-score <heuristic> : Perforn some heuristic reordering of the variables to minimize a scoring function. Heuristic is one of:" << std::endl;
        std::cerr << "  perm             : A sliding window of permutations of size window (see -window below)" << std::endl;
        std::cerr << "  sifting          : A static reordering of variables in which each variable is tried in every position to minimize the score" << std::endl;
        std::cerr << "  force            : Force heuristic" << std::endl;
        std::cerr << "  forceblocks      : A version of force compatible with blocks" << std::endl;
        std::cerr << "  random           : Self-explanatory" << std::endl;
        std::cerr << "  other            : Placeholder for different experiments" << std::endl;
        std::cerr << "  file <filename>  : Read variable ordering from a file" << std::endl;
        std::cerr << "-threshold <number>: A threshold of improvement to continue performing the reordering heuristic for variables" << std::endl;
        std::cerr << "-toCNF             : Convert the translation to CNF. It also generates a DIMACS file" << std::endl;
        std::cerr << "-use-XOR           : Use propositional logic + XOR construction to speed up BDD synthesis" << std::endl;
        std::cerr << "-window <number>   : Window size for the permutation variable ordering heuristic" << std::endl;

        exit(-1);
    }
    
    for(int i = 0; i < argc; i++)
        std::cerr << argv[i] << " ";
    std::cerr << std::endl;
    
    kconf::Driver driver;
    std::string arch;
    int ai;

    for(ai = 1; ai < argc-1; ++ai)
    {
        std::string arg = std::string(argv[ai]);
        std::cerr << "option is " << arg << std::endl;
        if (arg == "-c") {
            driver.cluster = true;
        }
        else if (arg == "-use-XOR") {
            driver.useXOR = true;
        }
        else if (arg == "-onlybool") {
            driver.onlyBool = true;
        }
        else if (arg == "-nocomp") {
            driver.components = false;
        }
        else if (arg == "-expand") {
            driver.expand = true;
        }
        else if (arg == "-nosimplify") {
            driver.simplify = false;
        }
        else if (arg == "-toCNF") {
            driver.doCNF = true;
        }
        else if (arg == "-noblocks") {
            driver.blocks = false;
            driver.isAddMenuBlocks = false;
        }
        else if (arg == "-arch") {
            arch = std::string(argv[++ai]);
        }
        else if (arg == "-window") {
            driver.window = atoi(argv[++ai]);
            if (driver.window ==0) {
            std::cerr << argv[ai] << " is not a valid window size" << std::endl;
            exit(-1);
            }
        }
        else if (arg == "-suffix") {
            driver.suffix = std::string(argv[++ai]);
        }
        else if (arg == "-score") {
            driver.scoreReorder = true;
            driver.scoreMethod  = std::string(argv[++ai]);
            if (driver.scoreMethod == "file")
                driver.varOrderFile = std::string(argv[++ai]);
            
        }
        else if (arg == "-threshold") {
            driver.scoreThreshold = atof(argv[++ai]);
            if (driver.scoreThreshold < 0) {
            std::cerr << argv[ai] << " is not a valid treshold" << std::endl;
            exit(-1);
            }
        }
        else {
            std::string arg = std::string(argv[ai]);
            if (arg.find('=') < arg.length()) {
                std::string key   = std::string(arg, 0, arg.find('='));
                std::string value = std::string(arg, arg.find('=')+1, arg.length() - arg.find('='));
                driver.addKeyValue(key, value);
                }
            else  {
                std::cerr << "Unknown option " << arg << std::endl;
                exit(-1);
                }
        }
        
    }
    
    if (driver.verbose)
        std::cout << get_timestamp() <<" Parsing" << std::endl;
    if (arch != "")
        driver.parse_file(argv[ai], arch);
    else
        driver.parse_file(argv[ai]);
    
    std::string fil = argv[ai];
    if (fil[fil.length()-1] == '/')
        fil = std::string(fil, 0, fil.length()-1);
    
    std::string base = std::string(fil, fil.rfind('/')+1, fil.length() - fil.rfind('/'));
    if (arch != "") base += "-"+arch;
    driver.simplifyDependencies();
    driver.treatVarsAndConstraints(base, true);
    driver.writeAlternatives(argv[ai]);
    driver.end_report();
    long end = get_cpu_time();
    std::cout << "Time: " << showtime(end - start) << " ("
                << (end - start) << " ms)" << std::endl;

    return 0;

    
}

