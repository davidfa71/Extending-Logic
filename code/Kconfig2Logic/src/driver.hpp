// $Id: driver.h 17 2007-08-19 18:51:39Z tb $ 	
/** \file driver.h Declaration of the kconf::Driver class. */

#ifndef KCONF_DRIVER_H
#define KCONF_DRIVER_H

#include <string>
#include <vector>
#include <map>
#include <iomanip>
#include <tuple>
#include <iterator>
#include <sys/stat.h>
#include <dirent.h>
#include <unordered_map>
#include <unordered_set>

#include "OtherScorer.hpp"
#include "RandomScorer.hpp"
#include "forceBlockScorer.hpp"
#include "PermScorer.hpp"
#include "SiftScorer.hpp"
#include "forceScorer.hpp"
#include "maxOccSorter.hpp"
#include "minOccSorter.hpp"
#include "MenuBlock.hpp"
#include "configInfo.hpp"
#include "choiceInfo.hpp"
#include "synExp.hpp"
#include "eval.hpp"
#include "Components.hpp"
#include "OneComponent.hpp"
#include "MultiComponents.hpp"
#include "scanner.hpp"

/** The kconf namespace is used to encapsulate the three parser classes
 * kconf::Parser, kconf::Scanner and kconf::Driver */
namespace kconf {
   
/** The Driver class brings together all components. It creates an instance of
 * the Parser and Scanner classes and connects them. Then the input stream is
 * fed into the scanner object and the parser gets it's token
 * sequence. Furthermore the driver object is available in the grammar rules as
 * a parameter. Therefore the driver class contains a reference to the
 * structure into which the parsed data is saved. */
class Driver
{

private:
    int                             setToTrue  = 0;
    int                             setToFalse = 0;
    Scorer*                         scorer;
    std::vector<choiceInfo*>        myChoices;
    Components*                     pcomp;
    int                             numFilesProcessed;
    int                             numIfs;
    int                             numChoicesProcessed;
    int                             numChoiceMembersProcessed;
    int                             numConfigsProcessed;
    int                             numMenuConfigsProcessed;
    int                             numMenusProcessed;
    int                             numMainMenusProcessed;
    int                             bddVarCounter;
    int                             promptCounter;
    int                             resolvedVarsCounter;
    std::set<std::string>           artificial, internal, actuallyUsedVariables;
    //std::map<std::string, varInfo*> varCol;
    std::map<std::string, int>      typecounters;
    //primalGraph                     pg;
    //mcluster                        clu;
    std::map<std::string, std::pair<int, int> > firstLast;
    std::vector<std::pair<int, int> > vFirstLast;
    // We store them here and treat them later on.
    std::list<varInfo*>             varList;
    std::vector<MenuBlock*>         menuBlocks;
    std::set<std::string>           selectable;
    std::map<std::string, int>      varMap;
    std::vector<std::string>        variableList;
    std::vector<int>                pos2var, var2pos;
    void fixblocks();
    std::vector<synExp*> constraintList;
    std::map<std::string, int>      occurrences;
    
    std::vector<synExp*>            normalizeVars();
    std::vector<synExp*>            simplifyConstraints(std::vector<synExp*> pendingConstraints);
    std::vector<synExp*>            modify(std::vector<synExp*> pendingConstraints,
                                                     bool writeFiles);
    void reorderVariables();
    void reorderScorer(std::vector<synExp*> pendingConstraints);
    void createStructures(bool writeFiles,
                          std::string base,
                          std::vector<synExp*> pendingConstraints);
    void blocksAndComponents(std::vector<synExp*>& pendingConstraints,
                             bool writeFiles);
    
  public:
    std::unordered_set<std::string> configList;
    bool                            firstPass;
    void addToDirFile(configInfo* c);
    std::map<std::string, std::vector<std::string>>  dirFileBlockMap;
    bool                            useXOR            = false;
    bool                            components        = true;
    bool                            onlyBool          = false;
    bool                            no_select         = false;
    bool                            simplify          = true;
    bool                            outputComponents  = false;
    int                             maxMenuSize;
    bool                            isAddMenuBlocks   = false;
    double                          scoreThreshold    = 1.0;
    bool                            expand            = false;
    bool                            doCNF             = false;
    bool                            scoreReorder      = false;
    double                          inflation         = 10;
    bool                            simulate          = false;
    bool                            blocks            = true;
    std::string                     constReorder      = "none";
    std::string                     varReorder        = "none";
    std::string                     scoreMethod       = "none";
    std::string                     varOrderFile;
    bool                            pickup            = false;
    int                             reorderEvery      = 1;
    int                             reorderInterval   = 0;
    int                             minConstraint     = 0;
    int                             minNodes          = 0;
    int                             lineLength        = 95;
    int                             maxNodes          = std::numeric_limits<int>::max();
    double                          cacheMultiplier   = 10;
    int                             window            = 0;
    std::string                     reorderMethod, suffix;

    
    void addToSelectable(std::string s) { selectable.insert(s); }
    void addMenuBlocks(configInfo *c); 
    
    void addChoice(choiceInfo *c);
        
    void simplifyDependencies();
    void treatVarsAndConstraints(std::string base, bool writeFiles);
    void writeAlternatives(std::string dir);
    std::vector<synExp*> removeDummyDefinitions(std::vector<synExp*>& p);
    void readClusters(const std::string name);
 
    //primalGraph& getPrimalGraph() { return pg; }
    /// construct a new parser driver context
    Driver();
    // Create primal graph as input to mcl
    bool cluster;
    /// enable debug output in the flex scanner
    bool trace_scanning;

    /// enable debug output in the bison parser
    bool trace_parsing;

    /// stream name (file or input stream) used for error messages.
    std::string streamname;
    bool reorder = true;
    bool Narodytska;
    bool verbose;
    //std::vector<varInfo> menuStack;

    int  getPromptCounter();
    void end_report();
    void increaseIfBlocks();
    void increaseChoices();
    void increaseChoiceMembers();
    void increaseConfigs();
    void increaseMenuConfigs();
    void increaseResolved();
    void increaseFiles(int a);
    void increaseMenus();
    void increaseMainMenus();
    void increaseCounters(std::string s);
    void addConstraint(varInfo* vi);

    // Process default statements
    
    /** Invoke the scanner and parser on a file. Use parse_stream with a
     * std::ifstream if detection of file reading errors is required.
     * @param filename	input file name
     * @return		true if successfully parsed
     */
    bool parse_file(const std::string basedir, const std::string architecture);
    bool parse_file(const std::string base);

    // To demonstrate pure handling of parse errors, instead of
    // simply dumping them on the standard error output, we will pass
    // them to the driver using the following two member functions.

    /** Error handling with associated line number. This can be modified to
     * output the error e.g. to a dialog box. */
    void error(const class location& l, const std::string& m);

    /** General error handling. This can be modified to output the error
     * e.g. to a dialog box. */
    void error(const std::string& m);

    /** Pointer to the current lexer instance, this is used to connect the
     * parser to the scanner. It is used in the yylex macro. */
    Scanner* lexer;
    
    void add(varInfo* v);
    bool exists(std::string s);
    varInfo* get(std::string s);
    
    void increasePrompt();
    void addKeyValue(std::string key, std::string value);
};

} // namespace kconf

#endif // KCONF_DRIVER_H
