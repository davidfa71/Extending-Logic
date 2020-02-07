// $Id: driver.cc 39 2008-08-03 10:07:15Z tb $
/** \file driver.cc Implementation of the kconf::Driver class. */

#include <fstream>
#include <sstream>

#include "driver.hpp"
#include "parser.hpp"
#include "scanner.hpp"

namespace kconf {
    
    bool blocksort(MenuBlock* a, MenuBlock* b) {
        std::string a1 = a->getFirst();
        std::string b1 = b->getFirst();
        return (symbolTable::getSymbol(a1)->getPos() < symbolTable::getSymbol(b1)->getPos());
    }
    
    Driver::Driver()
    : trace_scanning(false),
    trace_parsing(false)
    {
        cluster                   = false;
        verbose                   = false;
        Narodytska                = false;
        bddVarCounter             = 0;
        numFilesProcessed         = 1;
        numIfs                    = 0;
        numChoicesProcessed       = 0;
        numChoiceMembersProcessed = 0;
        numConfigsProcessed       = 0;
        numMenuConfigsProcessed   = 0;
        numMenusProcessed         = 0;
        numMainMenusProcessed     = 0;
        promptCounter             = 0;
    }
    
    void Driver::addToDirFile(configInfo* c) {
        std::string fileName = c->getFile();
        std::string fileNameStart;
        size_t bar = fileName.find('/');
        //std::cerr << "Filename is " << c->getFile() << std::endl;
        while (bar < fileName.length()) {
            std::string nextBit = std::string(fileName, 0, bar);
            fileNameStart = fileNameStart + nextBit + "/";
            dirFileBlockMap[fileNameStart].push_back(c->getName());
            //std::cerr << "Adding " << c->getName() << " to " << fileNameStart << std::endl;
            fileName = std::string(fileName, bar+1, fileName.length()-bar);
            bar = fileName.find('/');
        }
        fileNameStart = fileNameStart + fileName;
        dirFileBlockMap[fileNameStart].push_back(c->getName());
        //std::cerr << "Adding " << c->getName() << " to " << fileNameStart << std::endl;
    }

    
    void Driver::addMenuBlocks(configInfo *c) {
        //std::cerr << "MenuBlock: " << m << std::endl;
        if (blocks && isAddMenuBlocks && (c->getContentSize() <= maxMenuSize)) {
            MenuBlock *m = new MenuBlock(c->getContentBegin(),
                                         c->getContentEnd(),
                                         c->getPrompText(),
                                         "menu");
            menuBlocks.push_back(m);
        }
    }
    void Driver::addChoice(choiceInfo *c) {
        myChoices.push_back(c);
    }
    
    int  Driver::getPromptCounter()                 { return promptCounter;        }
    void Driver::increaseIfBlocks()                 { numIfs++;                    }
    void Driver::increaseChoices()                  { numChoicesProcessed++;       }
    void Driver::increaseChoiceMembers()            { numChoiceMembersProcessed++; }
    void Driver::increaseConfigs()                  { numConfigsProcessed++;       }
    void Driver::increaseMenuConfigs()              { numMenuConfigsProcessed++;   }
    void Driver::increaseResolved()                 { resolvedVarsCounter++;       }
    void Driver::increaseFiles(int a = 1)           { numFilesProcessed+= a;       }
    void Driver::increaseMenus()                    { numMenusProcessed++;         }
    void Driver::increaseMainMenus()                { numMainMenusProcessed++;     }
    
    void Driver::increaseCounters(std::string s)    { typecounters[s]++;         }
    
    void Driver::addConstraint(varInfo* vi)         {
    }
    
    bool Driver::exists(std::string s)   { return symbolTable::getSymbol(s) != NULL;  }
    varInfo* Driver::get(std::string s)  { return symbolTable::getSymbol(s); }
    
    
    void Driver::increasePrompt()          { promptCounter++;}
    
    void Driver::add(varInfo* v) {
        varList.push_back(v);
    }
    
    bool Driver::parse_file(const std::string base) {
        std::list<std::string> fileList;
        fileList.push_back(base+"/Config.in");
        fileList.push_back(base+"/Config.src");
        fileList.push_back(base+"/Kconfig");
        fileList.push_back(base+"/config/Config.in");
        fileList.push_back(base+"/config/Config.src");
        fileList.push_back(base+"/src/Config.in");
        fileList.push_back(base+"/src/Config.src");
        fileList.push_back(base+"/src/Kconfig");
        fileList.push_back(base+"/extra/Configs/Config.in");
        
        std::ifstream in;
        std::string filename;
        
        for(std::list<std::string>::iterator itl = fileList.begin(); itl != fileList.end(); itl++) {
            filename = *itl;
            in.open(itl->c_str());
            if (in.good()) break;
        }
        //Check the name to see if its a config file
        DIR* directory = opendir(base.c_str());
        
        if(directory == NULL)
            in.open(base.c_str());
        
        if (!in.good()) {
            std::cerr << "Could not open file:" << std::endl;
            std::copy(fileList.begin(), fileList.end(),
                      std::ostream_iterator<std::string>(std::cerr, "\n"));
            std::cerr << "Also couldn't open " << base << std::endl;
            exit(-1);
        }
        
        
        
        Scanner scanner(base, filename, &in);
        scanner.set_debug(trace_scanning);
        this->lexer = &scanner;
        
        Parser parser(*this);
        firstPass = true;
        parser.set_debug_level(trace_parsing);
        if (parser.parse() != 0) {
            exit(-1);
        };
        in.close();
        return 0;
    }
    
    bool Driver::parse_file(const std::string base, const std::string arch)
    {
        streamname = "/arch/" + arch + "/Kconfig";
        std::string filename = base + streamname;
        std::ifstream in(filename.c_str());
        if (!in.good()) {
            std::cerr << "arch: Couldn't open " << filename << std::endl;
            return false;
        }
        //if (verbose) std::cout << "Opened " << filename << std::endl << std::flush;
        Scanner scanner(base, filename, &in);
        scanner.set_debug(trace_scanning);
        this->lexer = &scanner;
        
        //f (verbose) std::cout << "Parsing " << filename << std::endl << std::flush;
        Parser parser(*this);
        configInfo *synArch = new configInfo();
        synArch->setName("ARCH");
        synArch->setValue(new synString(arch));
        parser.set_debug_level(trace_parsing);
        symbolTable::addSymbol(synArch->getName(), synArch);
        if (parser.parse() != 0) {
            exit(-1);
        };
        return 0;
    }
    
    void Driver::end_report() {
        std::cout << std::endl;
        std::cout << "Files processed      : " << numFilesProcessed         << std::endl;
        std::cout << "If blocks processed  : " << numIfs                    << std::endl;
        std::cout << "Choices processed    : " << numChoicesProcessed       << std::endl;
        std::cout << "Choice members       : " << numChoiceMembersProcessed << std::endl;
        std::cout << "Configs processed    : " << numConfigsProcessed       << std::endl;
        std::cout << "Menus processed      : " << numMenusProcessed         << std::endl;
        std::cout << "Main menus processed : " << numMainMenusProcessed     << std::endl;
        std::cout << "Menuconfig processed : " << numMenuConfigsProcessed   << std::endl;

        std::cout << std::endl;
        for(std::map<std::string, int>::iterator itm = typecounters.begin(); itm != typecounters.end(); itm++)
            std::cout << std::setw(8) << itm->first << ": " << std::setw(6) << itm->second << " variables" << std::endl;
    }
    
    void Driver::error(const class location& l,
                       const std::string& m)
    {
        std::cerr << l << ": " << m << std::endl;
    }
    
    void Driver::error(const std::string& m)
    {
        std::cerr << m << std::endl;
    }
    
    std::vector<synExp*> Driver::normalizeVars() {
        std::vector<synExp*> res;
        int counterF = 0;
        int counterT = 0;
        
        for(symbolTable::iterator it = symbolTable::begin(); it != symbolTable::end(); it++) {
            configInfo* v = *it;
            if (v->isArtificial())
                artificial.insert(v->getName());
            if (v->isInternal())
                internal.insert(v->getName());
            // We used to iterate on varList, but that excludes variables with no config declaration
            // and they must be included, otherwise they don't have a type
            
            // The symbol's value may have been established previously, due to a select f.i.
            // Or by hand in the command line options
            // We want to keep that value.
            
            // We set the selectable flag where appropriate so we can set some variables
            // to false automatically in giveConstraint
            
            // Now we know which variable has a config and which hasn't
            // Possibly because it is defined in a different architecture
            
            // We ask the symbol table to reflect that by setting value to synFalse
            // on those symbols/variables
            
            // Finally we only do that if the config is not selectable, to avoid inconsistencies
            int tdec = symbolTable::timesDeclared(v->getName());
            if (tdec == 0 &&
                !configInfo::isSelectable(v->getName()) &&
                !v->isArtificial() &&
                !v->isStringValue()) {
                counterF++;
                std::cerr << "1 We set var " << std::setw(50) << v->getName()
                          << " to false (not declared)" << std::endl;
                v->setValue(synFalse);
                res.push_back(makeNot(new synSymbol(v->getName())));
            }
            else counterT++;
        }
        
        for(varInfo* v : varList)
            if (!onlyBool || v->getVartype() == "boolean")
                for(synExp* e : v->giveConstraint())
                    res.push_back(e);
        
        return res;
    }
    
    std::vector<synExp*> Driver::simplifyConstraints(std::vector<synExp*> pendingConstraints) {
        bool changes;
        std::vector<synExp*>  otherConstraints;
        int numchanges = 0;
        std::set<std::string> rep;
        std::cerr << "Simplify Constraints" << std::endl;
        do {
            changes = false;
            numchanges = 0;
            int counter = 1;
            otherConstraints.clear();
            for(synExp *e : pendingConstraints) {
                bool keep = true;
                bool simplified = false;
                if (e->isSymbol()) {
                    // Constraint is a variable
                    std::string sn = e->getSymbol();
                    configInfo *w = symbolTable::getSymbol(sn);
                    synExp* wValue;
                    if (w == NULL) {
                        keep = false;
                        //throw std::logic_error("Symbol "+sn+" not defined in symbol table\n");
                        wValue = synFalse;
                    }
                    else  wValue = w->getValue();
                    
                    if (wValue != synTrue) {
                        simplified = true;
                        // If we have A and later not A or vice versa we may
                        // flip A from true to false forever. So we check and
                        // later we'll get an unsatisfiable set
                        if (wValue != synTrue && wValue != synFalse) {
                            setToTrue++;
                            std::cerr << "1 We set var " << std::setw(50) << sn << " from "
                            << std::setw(60)    << wValue   << " to true  "
                            << " expression (" << counter << ") " << e << std::endl;
                            numchanges++;
                            w->setValue(synTrue);
                            changes = true;
                        }
                        else if (wValue == synFalse) {
                            std::cerr << "1 Var " << std::setw(50) << sn << " is true and false "
                            << " in expression (" << counter << ") " << e << std::endl;
                            exit(-1);
                        }
                        
                    }
                }
                else {
                    if (e->isNot() && e->first()->isSymbol()) {
                        // Constraint is "not Variable"
                        std::string sn = e->first()->getSymbol();
                        varInfo *w = symbolTable::getSymbol(sn);
                        synExp* wValue;
                        if (w == NULL) {
                            keep = false;
                            wValue = synFalse;
                        }
                        else
                            wValue = w->getValue();
                        
                        if (wValue != synFalse) {
                            simplified = true;
                            if (wValue != synTrue && wValue != synFalse) {
                                numchanges++;
                                setToFalse++;
                                std::cerr << "2 We set var " << std::setw(50) << sn << " from "
                                << std::setw(60)    << wValue << " to false "
                                << " expression (" << counter << ") " << e << std::endl;
                                w->setValue(synFalse);
                                changes = true;
                            }
                            else
                                if (wValue == synTrue) {
                                    std::cerr << "2 Var " << std::setw(50) << sn << " is true and false "
                                          << " in expression (" << counter << ") " << e << std::endl;
                                    exit(-1);
                                }
                        }
                    }
                    

                }
                // If simplified is true, expression was "x" or "not x"
                // we want to keep those constraints, otherwise the variable
                // would disappear and the model would be wrong.

                //synExp *before = e->copy_exp();
                if (!e->isLiteral()) {
                    e = eval(e);
                }
                
                counter++;
                
                //if (before != synTrue) {
                //    std::ostringstream ost;
                //    e->print(ost);
                //    std::string s = ost.str();
                //    std::ostringstream ost2;
                //    before->print(ost2);
                //     if (rep.find(s) == rep.end()) {
                //        if (ost2.str() != s) {
                //            changes = true;
                //            std::cerr << "Inserting. Before "
                //                      << std::setw(60) << before
                //                      << " now "       << std::setw(60) << e << std::endl;
                //        }
                        if (e != synTrue && keep)
                            otherConstraints.push_back(e);
                //        rep.insert(s);
                //    }
                //}
                //before->destroy();
                //e->destroy();
            }
            std::cerr << "Numchanges " << std::setw(7) << numchanges
            << std::setw(7) << " otherConstraints " << otherConstraints.size() << std::endl;
            pendingConstraints.clear();
            pendingConstraints = otherConstraints;
            rep.clear();
        }
        while (changes);
        

        return pendingConstraints;
    }

    
    std::vector<synExp*> Driver::modify(std::vector<synExp*> pendingConstraints,
                                        bool writeFiles) {
        std::vector<synExp*> simplerConsts;
        std::set<std::string> rep;
        for(synExp* e : pendingConstraints)
            if (expand) {
                std::list<synExp*> le = expandImplication(e);
                for(synExp* s : le)
                    simplerConsts.push_back(s);
            }
            else
                if (doCNF) {
                    //std::cerr << "to CNF of " << e << std::endl;
                        std::list<synExp*> le = toCNF(e);
                    for(synExp* s : le)
                        if (s != synTrue) {
                            //std::cerr << "   " << s << std::endl;
                                simplerConsts.push_back(s);
                        }
                    //std::cerr << "---" << std::endl;
                }
                else
                    simplerConsts.push_back(e);
        
        return simplerConsts;
    }
    
    void Driver::reorderVariables() {
        if (varReorder == "max") {
            blocks = false;
            variableSorter *vsorter = new maxOccSorter(occurrences);
            vsorter->mysort(variableList.begin(), variableList.end());
            return;
        }
        if (varReorder == "min") {
            blocks = false;
            variableSorter *vsorter = new minOccSorter(occurrences);
            vsorter->mysort(variableList.begin(), variableList.end());
            return;
        }
        if (varReorder == "read") {
            std::ifstream in("variables.txt");
            variableList.clear();
            std::string t;
            while (in >>t)
                variableList.push_back(t);
            return;
        }
        if (varReorder != "none") {
            std::cerr << "Unknown varReorder option " << varReorder << std::endl;
            exit(-1);
        }
        
    }
    
    void Driver::reorderScorer(std::vector<synExp*> pendingConstraints) {
        if (!scoreReorder) return;
        if (scoreMethod == "perm") {
            PermScorer scorer(varMap, pendingConstraints, menuBlocks, pos2var, var2pos, window, scoreThreshold);
            scorer.permGo();
            return;
        }
        if (scoreMethod == "sifting") {
            SiftScorer scorer(varMap, pendingConstraints, menuBlocks, pos2var, var2pos);
            scorer.siftGo(scoreThreshold);
            return;
        }
        if (scoreMethod == "force") {
            blocks = false;
            forceScorer scorer(varMap, pendingConstraints, pos2var, var2pos);
            scorer.force();
            return;
        }
        if (scoreMethod == "forceblocks") {
            forceBlockScorer scorer(varMap, pendingConstraints, menuBlocks, pos2var, var2pos);
            scorer.forceBlocks();
            return;
        }
        if (scoreMethod == "random") {
            RandomScorer scorer(varMap, pendingConstraints, menuBlocks, pos2var, var2pos);
            scorer.randomHeuristic(scoreThreshold);
            return;
        }
        if (scoreMethod == "other") {
            SiftScorer scorer(varMap, pendingConstraints, menuBlocks, pos2var, var2pos);
            scorer.siftGo(scoreThreshold);
            OtherScorer scorer2(varMap, pendingConstraints, menuBlocks, pos2var, var2pos);
            scorer2.otherHeuristic(scoreThreshold);
            return;
        }
        if (scoreMethod == "file") {
            std::ifstream vf(varOrderFile);
            int c = 0;
            pos2var.clear();
            var2pos.clear();
            var2pos.resize(varMap.size());
            std::string var;
            std::set<std::string> svars;
            while (vf >> var) {
                svars.insert(var);
                pos2var.push_back(varMap[var]);
                var2pos[varMap[var]] = c++;
            }
            vf.close();
            if (c != varMap.size()) {
                std::cerr << "Successfully read " << c << " variables from file " << varOrderFile << ". varMap size is " << varMap.size() << std::endl;
                
                for(std::pair<std::string, int> p : varMap)
                    if (svars.find(p.first) == svars.end())
                        std::cerr << p.first << " ";
            }
            return;
        }
        std::cerr << "Scoring method " << scoreMethod << " unknown." << std::endl;
        exit(-1);
    }
    
    void Driver::createStructures(bool writeFiles,
                                  std::string base,
                                  std::vector<synExp*> pendingConstraints) {
        
        std::set<int> check;
        for(int v : pos2var) {
            check.insert(v);
        }
        
        if (check.size() != pos2var.size()) {
            std::cerr   << "check size is " << check.size()
                        << " pos2var size is " << pos2var.size()
                        << std::endl;
            for(int x = 0; x < pos2var.size(); x++)
                if (check.find(x) == check.end())
                    std::cerr << std::setw(5) << x << " is not in check" << std::endl;
            
            //std::cerr << "check: ";
            //for(int x : check)
            //    std::cerr << x << " ";
            //std::cerr << std::endl;
            exit(-1);
        }
        // Check that var2pos and pos2var are inverses
        for(int x = 0; x < var2pos.size(); x++) {
            if (var2pos[pos2var[x]] != x) {
                std::cerr << "Type 1 error x " << x
                << " pos2var[" << x << "] " << pos2var[x]
                << " var2pos[" << pos2var[x] << "] " << var2pos[pos2var[x]] << std::endl;
                exit(-1);
            }
            if (pos2var[var2pos[x]] != x) {
                std::cerr << "Type 2 error x " << x
                          << " var2pos[" << x << "]=" << var2pos[x]
                          << " pos2var[" << var2pos[x] << "] = " << pos2var[var2pos[x]]
                          << std::endl;
                for(int x = 0; x <  var2pos.size(); x++)
                    std::cerr << std::setw(5) << x
                              << std::setw(5) << var2pos[x]
                              << std::setw(5) << pos2var[x]
                              << std::endl;
                exit(-1);
            }
        }
        
        
        // variableList is the inverse of varMap, it does no longer indicate ordering.
        
        // This vector DOES indicate variable ordering. We write it to a file.
        std::vector<std::string> var4File;
        for(int var : pos2var) {
            std::string sx = variableList.at(var);
            if (sx == "") {
                std::cerr << "Variable at position " << var << " has no name" << std::endl;
                exit(-1);
            }
            var4File.push_back(sx);
        }
        
        if (writeFiles) {
            std::ofstream ov(base+".var");
            for(auto s : variableList)
                ov << s << " ";
            
            ov << std::endl;
            ov.close();
            std::cerr << "Writing variables in order of occurrence to " << base+".var" << std::endl;

            
            if (scoreReorder) {
                std::ofstream vf(base+".ord");
                std::cerr << "Writing ordered variables to " << base+".ord" << std::endl;
                for(const std::string& s : var4File) {
                    if (s != "")
                        vf << s << " ";
                    else {
                        std::cerr << "Variable name is empty" << std::endl;
                        exit(-1);
                    }
                }
                vf << std::endl;
                vf.close();
            }
            std::ofstream ef(base+".exp");
            std::cerr << "Writing expressions to " << base+".exp" << std::endl;

            for(synExp* s : pendingConstraints)
                ef << s << std::endl;
            
            ef.close();
            // If XOR is used, there can be no DIMACS
            if (doCNF && !useXOR) {
                std::ofstream dimacs(base+".dimacs");
                dimacs << "p cnf " <<  varMap.size() << " " << pendingConstraints.size() << std::endl;
                for(synExp* s : pendingConstraints) {
                    for(int i : s->toDimacs(varMap))
                        // First variable has to be 1, not 0. And so on...
                        dimacs << i << " ";
                    dimacs << "0" << std::endl;
                }
            }
        }
        
        
        for(synExp* s: pendingConstraints) {
            if (s != synTrue) {
                // Not a very good idea after all!
                //for(const std::string ss : s->giveSymbols())
                //if (symbolTable::getSymbol(ss)->getVartype() == "tristate")
                //    s->addModules();
                //s->computeIndices(varMap);
                constraintList.push_back(s);
            }
            s->destroy();
        }

    }
    
    void Driver::blocksAndComponents(std::vector<synExp*>& pendingConstraints,
                                     bool writeFiles) {
        varMap.clear();
        variableList.clear();
        // The ordering of the variables is changed here
        if (blocks)
            fixblocks();
        
        int counter = 0;
        for(symbolTable::iterator itv = symbolTable::begin();
            itv != symbolTable::end(); itv++) {
            configInfo* v = *itv;
            
            if (!onlyBool || v->getVartype() == "boolean")
                // We used to iterate on varList, but that excludes variables with no config declaration
                // and they must be included
                if (!v->isChoice() &&
                    std::find(variableList.begin(), variableList.end(),
                              v->getName()) == variableList.end()) {
                    v->setPos(counter);
                    variableList.push_back(v->getName());
                }
        }
        // We add at the end the symbols that appear in expressions but are not
        // defined anywhere
        
        for(std::string s : symbolTable::getUndefined())
            if (std::find(variableList.begin(), variableList.end(), s) == variableList.end()) {
                variableList.push_back(s);
                pendingConstraints.push_back(makeNot(new synSymbol(s)));
                std::cerr << "Using undefined symbol " << s << std::endl;
                symbolTable::getSymbol(s)->setValue(synFalse);
            }
        
        counter = 0;
        for(const std::string& s : variableList)
            varMap[s] = counter++;
        
        // We start with the identity order
        pos2var.resize(varMap.size());
        var2pos.resize(varMap.size());
        for(int x = 0; x < varMap.size(); x++) {
            pos2var[x] = x;
            var2pos[x] = x;
        }
        
        if (!blocks) {
            MultiComponents comp(var2pos, pos2var);
            
            // We process the constraints to find out the final components
            for(synExp* s : pendingConstraints) {
                comp.join(s, false);
            }
            std::cerr   <<  "There are "
            << std::setw(6) << comp.getNumComponents() << " components" << std::endl;
            int ncomp = 0;
            comp.reorder();
            if (outputComponents) {
                std::cerr << "Number of components " << comp.getNumComponents() << std::endl;
                for(std::pair<int, int> p : comp.listComponents()) {
                    std::cerr << "Component " << ncomp << " starts in " << std::setw(6) << p.first
                    << " length " << std::setw(6) << p.second << " :" << std::endl;
                    std::ostringstream ostvar, ostexp;
                    ostvar << "component-" << ncomp << ".var";
                    std::ofstream ofvar;
                    if (writeFiles) ofvar.open(ostvar.str().c_str());
                    std::set<int> varSet;
                    for(int x = p.first; x < p.first+p.second; x++) {
                        if (writeFiles) ofvar << variableList[pos2var[x]] << std::endl;
                        varSet.insert(pos2var[x]);
                    }
                    if (writeFiles) ofvar.close();
                    ostexp << "component-" << ncomp << ".exp";
                    std::ofstream ofexp;
                    if (writeFiles) ofexp.open(ostexp.str().c_str());
                    for(synExp* s: pendingConstraints) {
                        bool found = false;
                        for(int index : s->giveSymbolIndices())
                            if (varSet.find(index) != varSet.end()) {
                                found = true;
                                break;
                            }
                        if (found)
                            if (writeFiles) ofexp << s << std::endl;
                    }
                    if (writeFiles) ofexp.close();
                    ncomp++;
                }
            }
        }
        else {
            // Only one component. Needed for blocks
            std::cerr << "Only one component" << std::endl;
            OneComponent comp(var2pos.size());
        }
        
    }
    // We remove the definition of a dummy variable (internal and simple)
    // Internal means that it doesn't have a prompt
    // Simple means the value is always true unless no definitions apply
    std::vector<synExp*> Driver::removeDummyDefinitions(std::vector<synExp*>& p) {
        std::vector<synExp*> q;
        std::map<std::string, int> freq;
        for(synExp* s : p) {
            for(std::string ss : s->giveSymbols()) {
                freq[ss]++;
            }
        }
        // Do NOT join both loops
        bool problems = false;
        for(synExp* s : p) {
            //std::cerr << "Evaluating " << s << std::endl;
            bool use = true;
            for(std::string ss : s->giveSymbols()) {
                //configInfo *c = symbolTable::getSymbol(ss);

                // Commented out because it is impossible to
                // pass validation tests if legitimate constraints
                // are discarded
//
//                if (c != NULL) {
//                    if (symbolTable::timesDeclared(ss) > 0 &&
//                        freq[ss] == 1 &&
//                        c->isDummy()) {
//                        //c->isInternal() &&
//                        //!c->isChoiceMember()) {
//
//                        // Can't eliminate internals e.g. HAVE_DOT_CONFIG
//                        // bc it does not validate (they are written in .config)
//
//                        //if (c->isDummy() && freq[ss] == 1) {
//                        // It doesn't work with artificial because it discards important
//                        // stuff like USBDEBUG_DEFAULT_PORT_EQ_0 -> USBDEBUG_DEFAULT_PORT
//                        //if (c->isArtificial() && freq[ss] == 1) {
//                        // for now...
//                        use = false;
//                        symbolTable::deleteSymbol(ss);
//                        std::cerr << "Discarding " << s << " because of " << ss
//                                << " declared " << symbolTable::timesDeclared(ss) << " times"
//                                << ". freq " << freq[ss] << " internal " << c->isInternal()
//                                << std::endl;
//                        //}
//                        if (onlyBool && c->isStringValue()) {
//                            std::cerr << "OnlyBool but " << ss << " is type " << c->getVartype()
//                            << " in expression " << s << std::endl;
//                            use = false;
//                            problems = true;
//                        }
//                    }
//                }
            }
                s->deleteSymbols();
                if (use)
                    q.push_back(s);
        }
        if (problems)
            throw std::logic_error("String values are used in boolean translations. Not possible to do a purely Boolean model.");
        
        return q;
    }
    
    void Driver::simplifyDependencies() {
        for(varInfo *v : varList)
            v->simplifyDependency();
    }
void Driver::writeAlternatives(std::string dir) {
    std::string filename = dir + "/symbols";
    std::cerr << "Symbols file " << filename << std::endl;
    std::ofstream symbols(filename);
    for(varInfo* v : varList) {
        if (std::string(v->getName(), 0, 7) != "choice_") {
            symbols << v->getName() << " ";
            if (v->getVartype() == "string" ||
                v->getVartype() == "int" ||
                v->getVartype() == "hex")
                for(auto s : configInfo::stringAlternatives[v->getName()])
                    symbols << s << " ";
            symbols << std::endl;
        }
    }
    
}

    void Driver::treatVarsAndConstraints(std::string base, bool writeFiles) {
        std::vector<synExp*> pendingConstraints;
        std::cerr   << "Before treatment. "         << varList.size()   << " variables" << std::endl;
        if (suffix != "")
            base = base + "-" + suffix;
        
        pendingConstraints = normalizeVars();
        if (simplify)
            pendingConstraints = simplifyConstraints(pendingConstraints);
        pendingConstraints = removeDummyDefinitions(pendingConstraints);
        blocksAndComponents(pendingConstraints, writeFiles);

        // If expand or CNF have been selected...
        pendingConstraints = modify(pendingConstraints, writeFiles);

        if (simplify)
            pendingConstraints = simplifyConstraints(pendingConstraints);

        scorer = new Scorer(varMap, pendingConstraints, pos2var, var2pos);
       
        // We create numeric indices for the constraints
        for(synExp* s: pendingConstraints) {
            if (s != synTrue) {
                //std::cerr << "Creating indices for " << s << std::endl;
                s->computeIndices(varMap);
                s->deleteSymbols();
            }
        }

        std::set<int> check;
        for(int v : pos2var) {
            if (check.find(v) != check.end()) {
                std::cerr << "driver: var " << v << ", " <<  variableList[v]
                            << " is repeated in pos2var" << std::endl;
                
                exit(-1);
            }
            check.insert(v);
        }
        std::cout << "At least " << setToTrue << " variables are always true" << std::endl;
        std::cout << "At least " << setToFalse << " variables are always false " << std::endl;
        reorderScorer(pendingConstraints);
        createStructures(writeFiles, base, pendingConstraints);
        std::cout << "Symbols in symbolTable " << symbolTable::size() << std::endl;

    }
    
    void Driver::fixblocks() {
        
        // CreateBlocks changes the order of the variables in the symbol table
        for(auto ci : myChoices)
            for(auto m : ci->createBlocks())
                menuBlocks.push_back(m);

        // Now we add the directory and file blocks
        //for(std::pair<std::string, std::vector<std::string>> p : dirFileBlockMap){
        //    MenuBlock* block = new MenuBlock(p.second.begin(), p.second.end(), p.first, "dirFile");
        //    menuBlocks.push_back(block);
        //}
        
        std::vector<MenuBlock*>::iterator itlp = menuBlocks.begin();
        std::vector<MenuBlock*>::iterator other = itlp;
        while(itlp != menuBlocks.end()) {
            if (! (*itlp)->fix()) {
                other = itlp;
                itlp = menuBlocks.erase(other);
                
            }
            else {
                itlp++;
            }
        }
            
    }
    void Driver::addKeyValue(std::string key, std::string value) {
        configInfo *var = new configInfo();
        // if we don't set HasConfig, value gets set to synFalse later on
        var->setHasConfig();
        var->setName(key);
        if (value == "y") {
            var->setValue(synTrue);
        }
        else
            if (value == "m") {
                var->setValue(synModule);
            }
            else
                if (value == "n") {
                    var->setValue(synFalse);
                }
                else {
                    //std::cerr   << "Wrong value in \"key = value\": "
                    //            << "*" << key << "* = *" << value << "*" <<std::endl;
                    //exit(-1);
                    var->setValue(new synString(value));
                    var->setVartype("string");
                }
        std::cerr << "Adding symbol " << key << " with value " << value << " to symbol table" << std::endl;
        symbolTable::addSymbol(key, var);
    }
    
} // namespace kconf



