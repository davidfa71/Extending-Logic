#include <iostream>
#include <fstream>
#include <algorithm>
#include <stdlib.h>

#include "mytime.hpp"
#include "Transform.hpp"
#include "cuddAdapter.hpp"

int main(int argc, char *argv[])
{
    long start = get_cpu_time();
    if (argc < 3) {
        std::cerr << "Logic2BDD: Reads a file with a variable ordering and a collection of (extended) propositional logic constraints to synthesize a BDD" << std::endl;
            std::cerr << "Usage: Logic2BDD [ops] varfile expfile" << std::endl;
        std::cerr << "-arch <name>                   : Architecture for the Linux kernel" << std::endl;
        std::cerr << "-base <basename>               : Name of the resulting BDD file" << std::endl;
        std::cerr << "-cache-multiplier <number>     : Multiplies the default cache in CUDD by this number." << std::endl
                  << "                                 A bigger cache means slower lookups but faster applies" << std::endl;
        std::cerr << "-create-dimacs                 : Write a DIMACS file corresponding to the variable names and ordering and the constraints" << std::endl;
        std::cerr << "-constraint-reorder <heuristic>: A heuristic to reorder constraints after a reordering of variables. One of:" << std::endl;
        std::cerr << "  minimax                      : The constraints are ordered according to the position of its biggest variable. Smaller go first" << std::endl;
        std::cerr << "  minspan                      : The constraints are sorted acccording to span. Smaller spans go first." << std::endl;
        std::cerr << "  remember                     : A heuristic with memory regarding previous constraints" << std::endl;
        std::cerr << "  none                         : Does nothing. This is the default" << std::endl;
        std::cerr << std::endl;
        std::cerr << "-inflation <number>            : In order to trigger dynamic reordering of the BDD, the porcentual growth of the nodes in the BDD " << std::endl
                  << "                                 since applying last constraint has to be bigger than this number. By default, it is 10" << std::endl;
        std::cerr << "-line-length <number>          : The length of the space devoted in screen to show a constraint." << std::endl
                  << "                                 If the constraint in longer than this number, the line is broken" << std::endl;
        std::cerr << "-max-nodes <number>            : If the number of nodes in the BDD exceeds this number, dynamic reordering will not be performed" << std::endl;
        std::cerr << "-min-constraint <number>       : If the number of constraints applied to the BDD is less than this number," << std::endl
                  << "                                 dynamic reordering will not be performed" << std::endl;
        std::cerr << "-min-nodes <number>            : No dynamic reordering is performed until the minimum number of nodes is reached" << std::endl;
        std::cerr << "-no-dynamic-comp               : Do not use the connected components in dynamic reordering" << std::endl;
        std::cerr << "-noreorder                     : Do not perform dynamic reordering of variables" << std::endl;
        std::cerr << "-no-static-comp                : Do not reorder the variables by putting together variables related by constraints" << std::endl;
        std::cerr << "-related-vars                  : When dynamic reordering is performed, a variable does not go beyond the variables " << std::endl
                   << "                                 which are directly related in the constraints" << std::endl;
        std::cerr << "-reorder-every <number>        : Perform a dynamic reordering every <number> applications of constraints" << std::endl;
        std::cerr << "-reorder-interval              : The minimum number of constraints that are applied between consecutive applications " << std::endl
                  << "                                 of dynamic reordering. Used to space dynamic reorderings" << std::endl;
        std::cerr << "-reorder-method <name>         : The method to use for dynamic reordering of the variables in the BDD. One of:" << std::endl;
        std::cerr << "  CUDD_REORDER_SAME"            << std::endl;
        std::cerr << "  CUDD_REORDER_RANDOM"          << std::endl;
        std::cerr << "  CUDD_REORDER_RANDOM_PIVOT"    << std::endl;
        std::cerr << "  CUDD_REORDER_SIFT"            << std::endl;
        std::cerr << "  CUDD_REORDER_SIFT_CONVERGE"   << std::endl;
        std::cerr << "  CUDD_REORDER_SYMM_SIFT"       << std::endl;
        std::cerr << "  CUDD_REORDER_SYMM_SIFT_CONV"  << std::endl;
        std::cerr << "  CUDD_REORDER_GROUP_SIFT"      << std::endl;
        std::cerr << "  CUDD_REORDER_GROUP_SIFT_CONV" << std::endl;
        std::cerr << "  CUDD_REORDER_WINDOW2_CONV"    << std::endl;
        std::cerr << "  CUDD_REORDER_WINDOW3_CONV"    << std::endl;
        std::cerr << "  CUDD_REORDER_WINDOW4_CONV"    << std::endl;
        std::cerr << "  CUDD_REORDER_ANNEALING"       << std::endl;
        std::cerr << "  CUDD_REORDER_GENETIC"         << std::endl;
        std::cerr << "  CUDD_REORDER_EXACT"           << std::endl;
        std::cerr << std::endl;
        std::cerr << "-score [heuristic]             : Apply a heuristic to reorder variables in order to minimize the scoring function. One of:" << std::endl;
        std::cerr << "  perm                         : A sliding window of permutations of size window" << std::endl;
        std::cerr << "  sifting                      : Every variable is sifted to each position and it is finally moved to the optimal position" << std::endl;
        std::cerr << "  force                        : The FORCE heuristic" << std::endl;
        std::cerr << "  forceblocks                  : A version of force compatible with the use of blocks of variables" << std::endl;
        std::cerr << "  random                       : Randomly swaps pairs of variables to reduce the scoring function" << std::endl;
        std::cerr << "  other                        : Placeholder for experiments" << std::endl;
        std::cerr << std::endl;
        std::cerr << "-simulate                      : Simulate the synthesis of the BDD" << std::endl;
        
        std::cerr << "-suffix <string>               : The name of the resulting BDD. Otherwise, the name is 'unknown'" << std::endl;
        std::cerr << "-threshold                     : A threshold of improvement to continue performing the variable reordering heuristic"
                << std::endl;
        std::cerr << "-window                        : The size of the window for the perm heuristic" << std::endl;

        exit(-1);
    }
    
    for(int i = 0; i < argc; i++)
        std::cerr << argv[i] << " ";
    std::cerr << std::endl;
    
    kconf::Transform trans;
    std::string arch;
    int ai;
    for(ai = 1; ai < argc-2; ++ai)
    {
        std::string arg = argv[ai];
        if (arg == "-score") {
            trans.scoreReorder = true;
            trans.scoreMethod  = std::string(argv[++ai]);
            if (trans.scoreMethod == "file")
                trans.varOrderFile = std::string(argv[++ai]);
        
        }
        else if (arg == "-threshold") {
            trans.scoreThreshold = atof(argv[++ai]);
        }
        else if (arg == "-window") {
            trans.window = atoi(argv[++ai]);
            if (trans.window ==0) {
                std::cerr << argv[ai] << " is not a valid window size" << std::endl;
                exit(-1);
            }
        }
        else if (arg == "-no-static-comp") {
            trans.static_components = false;
        }
        else if (arg == "-no-dynamic-comp") {
            trans.dynamic_components = false;
        }
        else if (arg == "-base") {
            trans.base = std::string(argv[++ai]);
        }
        else if (arg == "-cudd") {
            trans.adapterType = "cudd";
        }
        //else if (arg == "-buddy") {
        //    trans.adapterType = "buddy";
        //}
        else if (arg == "-noreorder") {
            trans.reorder = false;
        }
        else if (arg == "-simulate") {
            trans.simulate = true;
        }
        else if (arg == "-constraint-reorder") {
            trans.constReorder = std::string(argv[++ai]);
        }
        else if (arg == "-arch") {
            arch = std::string(argv[++ai]);
        }
        else if (arg == "-inflation") {
            trans.inflation = atof(argv[++ai]);
            if (trans.inflation ==0) {
                std::cerr << argv[ai] << " is not a valid inflation" << std::endl;
                exit(-1);
            }
        }
        else if (arg == "-reorder-method") {
            trans.reorderMethod = std::string(argv[++ai]);
        }
        else if (arg == "-reorder-interval") {
            trans.reorderInterval = atoi(argv[++ai]);
            if (trans.reorderInterval ==0) {
                std::cerr << argv[ai] << " is not a valid reorder interval" << std::endl;
                exit(-1);
            }
        }
        else if (arg == "-min-nodes") {
            trans.minNodes = atoi(argv[++ai]);
            if (trans.minNodes ==0) {
            std::cerr << argv[ai] << " is not a valid minimal number of nodes" << std::endl;
            exit(-1);
            }
        }
        else if (arg == "-line-length") {
            trans.lineLength = atoi(argv[++ai]);
            if (trans.lineLength ==0) {
                std::cerr << argv[ai] << " is not a valid line length" << std::endl;
                exit(-1);
            }
        }
        else if (arg == "-max-nodes") {
            trans.maxNodes = atoi(argv[++ai]);
            if (trans.maxNodes ==0) {
                std::cerr << argv[ai] << " is not a valid maximal number of nodes" << std::endl;
                exit(-1);
            }
        }
        else if (arg == "-min-constraint") {
            trans.minConstraint = atoi(argv[++ai]);
            if (trans.minConstraint ==0) {
                std::cerr << argv[ai] << " is not a valid minimal constraint" << std::endl;
                exit(-1);
            }
        }
        else if (arg == "-reorder-every") {
            trans.reorderEvery = atoi(argv[++ai]);
            if (trans.reorderEvery ==0) {
                std::cerr << argv[ai] << " is not a valid argument to -reorder-every" << std::endl;
                exit(-1);
            }
            
        }
        else if (arg == "-cache-multiplier") {
            trans.cacheMultiplier = atof(argv[++ai]);
            if (trans.cacheMultiplier ==0) {
                std::cerr << argv[ai] << " is not a valid cache multiplier" << std::endl;
                exit(-1);
            }
        }
        else if (arg == "-suffix") {
            trans.suffix = std::string(argv[++ai]);
        }
        else if (arg == "-createdimacs") {
            trans.doCNF = true;
        }
        else if (arg == "-related-vars") {
            trans.relatedVars = true;
        }
        else  {
            std::cerr << "Unknown option " << arg << std::endl;
            exit(-1);
            }
        }
        
    trans.readVars(argv[ai]);
    trans.readExp(argv[ai+1]);
    trans.treatVarsAndConstraints();
    if (trans.base == "") trans.base = "unknown";
    std::string bddname     = "nothing.bdd";
    
    trans.buildBDD();
    synConst::delConst();
    long end = get_cpu_time();
    std::cout << "Time: " << showtime(end - start) << " ("
              << (end - start) << " ms)" << std::endl;

    return 0;

    
}

