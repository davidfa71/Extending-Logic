// $Id: Tansform.cc 39 2008-08-03 10:07:15Z tb $
/** \file Tansform.cc Implementation of the kconf::Tansform class. */

#include <fstream>
#include <sstream>

#include "Transform.hpp"

namespace kconf {

Transform::Transform() {}


void    Transform::readVars(std::string varFile) {
    // We read the variables in order
    std::ifstream vf(varFile.c_str());
    if (!vf.good()) {
        throw std::invalid_argument("Couldn't open file "+varFile);
    }
    std::string temp;
    while (vf >> temp)
        variables.push_back(temp);
}

void    Transform::readExp(std::string expFile) {
    // We parse the expression file
    kconf::synExpDriver driver;
    driver.parse_file(expFile);
    expList  = driver.getConstraints();
    std::cerr << "Read " << expList.size() << " expressions" << std::endl;
    // We initialize the order and the inverse mapping
    for(int x = 0; x < variables.size(); x++) {
        var2pos.push_back(x);
        pos2var.push_back(x);
        varMap[variables.at(x)] = x;
    }
    
}
void    addSymbols2Table(std::vector<std::string> vars) {
    for(std::string s : vars) {
        //std::cerr << "Adding " << s << " to symbol table" << std::endl;
        configInfo *c = new configInfo();
        c->setValue(new synSymbol(s));
        c->setName(s);
        c->setVartype("boolean");
        symbolTable::addSymbol(s, c);
    }
}

std::vector<synExp*> Transform::simplifyConstraints(std::vector<synExp*> pendingConstraints) {
    bool changes;
    std::vector<synExp*>  otherConstraints;
    int numchanges = 0;
    std::set<std::string> rep;
    int setToTrue  = 0;
    int setToFalse = 0;
    std::cerr << "Simplify Constraints.There are " << pendingConstraints.size()
              << " constraints to simplify" << std::endl;
    do {
        changes = false;
        numchanges = 0;
        int counter = 1;
        otherConstraints.clear();
        for(synExp *e : pendingConstraints) {
            bool keep = true;
            //std::cerr << "Simplifying #" << tempcounter++ << " expression " << e << std::endl;
            bool simplified = false;
            if (e->isSymbol()) {
                // Constraint is a variable
                std::string sn = e->getSymbol();
                configInfo *w = symbolTable::getSymbol(sn);
                synExp* wValue;
                if (w == NULL) {
                    throw std::logic_error("Symbol "+sn+" not defined in symbol table\n");
                    std::cerr << "Symbol " << sn << " is not defined in the symbol table" << std::endl;
                    wValue = synFalse;
                    keep = false;
                }
                else
                    wValue = w->getValue();
                
                if (wValue != synTrue) {
                    simplified = true;
                    // If we have A and later not A or vice versa we may
                    // flip A from true to false forever. So we check and
                    // later we'll get an unsatisfiable set
                    if (wValue != synTrue && wValue != synFalse) {
                        setToTrue++;
                        numchanges++;
                        w->setValue(synTrue);
                        changes = true;
                        std::cerr << "1 We set var " << std::setw(50) << sn << " from "
                        << std::setw(60)    << wValue << " to true "
                        << " expression (" << counter << ") " << e << std::endl;
                    }
                    else if (wValue == synFalse) {
                        std::ostringstream ost;
                        ost << "1 Var " << std::setw(50) << sn << " is true and false "
                        << " in expression (" << counter << ") " << e << std::endl;
                        throw std::logic_error(ost.str());
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
                        wValue = synFalse;
                        keep = false;
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
            
            synExp *before = e->copy_exp();
            if (!e->isLiteral()) {
                e = eval(e);
            }
            if (e == synFalse) {
                std::ostringstream ost;
                ost << "There was a contradiction simplifying " << before << " to false" << std::endl;
                throw std::logic_error(ost.str());
            }
            counter++;
            if (e != synTrue && keep)
                otherConstraints.push_back(e);
       
        }
        pendingConstraints.clear();
        pendingConstraints = otherConstraints;
        rep.clear();
    }
    while (changes);
    
    std::cerr << pendingConstraints.size() << " constraints " << std::endl;
    return pendingConstraints;
}
void Transform::treatVarsAndConstraints() {
    addSymbols2Table(variables);
    std::cerr << "There are " << expList.size() << " expressions " << std::endl;
    expList = simplifyConstraints(expList);
    std::cerr << "And now there are " << expList.size() << " expressions " << std::endl;

    int counter = 0;
    for(const std::string& s : variables)
        varMap[s] = counter++;
    
    // We create numeric indices for the constraints
    for(synExp* s: expList)
        if (s != synTrue)
            s->computeIndices(varMap);
    
    // We start with the identity order
    pos2var.resize(variables.size());
    var2pos.resize(variables.size());
    for(int x = 0; x < variables.size(); x++) {
        pos2var[x] = x;
        var2pos[x] = x;
    }
    
    computeComponents();
    reorderVariables();
    

    for(int x = 0; x < var2pos.size(); x++) {
        if (var2pos[pos2var[x]] != x) {
            std::cerr << "Type 1 error x " << x
            << " pos2var[" << x << "] " << pos2var[x]
            << " var2pos[" << pos2var[x] << "] " << var2pos[pos2var[x]] << std::endl;
            exit(-1);
        }
        if (pos2var[var2pos[x]] != x) {
            std::cerr << "Type 2 error x " << x << std::endl;
            exit(-1);
        }
    }
    int min = std::numeric_limits<int>::max();
    int max = -1;
    for(int p : var2pos) {
        if (p < min) min = p;
        if (p > max) max = p;
    }
    if (min != 0 || max != variables.size()-1) {
        std::cerr << "1 Error in treatVarSAndConstraints. min is " << min << " and max is " << max << std::endl;
        exit(-1);
    }

    
    
    
    min = std::numeric_limits<int>::max();
    max = -1;
    for(int p : var2pos) {
        if (p < min) min = p;
        if (p > max) max = p;
    }
    if (min != 0 || max != variables.size()-1) {
        std::cerr << "2 Error in treatVarSAndConstraints. min is " << min << " and max is " << max << std::endl;
        exit(-1);
    }
    
    int ncomp = 0;
    if (outputComponents) {
        std::cerr << "Number of components " << pcomp->getNumComponents() << std::endl;
        for(std::pair<int, int> p : pcomp->listComponents()) {
            std::cerr << "Component " << ncomp << " starts in " << std::setw(6) << p.first
            << " length " << std::setw(6) << p.second << " :" << std::endl;
            std::ostringstream ostvar, ostexp;
            ostvar << base+"-component-" << ncomp << ".var";
            std::ofstream ofvar(ostvar.str().c_str());
            std::set<int> varSet;
            for(int x = p.first; x < p.first+p.second; x++) {
                ofvar << variables[pos2var[x]] << std::endl;
                varSet.insert(pos2var[x]);
            }
            ofvar.close();
            ostexp << base+"-component-" << ncomp << ".exp";
            std::ofstream ofexp(ostexp.str().c_str());
            for(synExp* s: expList) {
                bool found = false;
                for(int index : s->giveSymbolIndices())
                    if (varSet.find(index) != varSet.end()) {
                        found = true;
                        break;
                    }
                if (found)
                    ofexp << s << std::endl;
            }
            ofexp.close();
            ncomp++;
        }
    }
    
    // variables is the inverse of varMap, it does no longer indicate ordering.
    // This vector DOES indicate variable ordering.
    var2pos.clear();
    var2pos.resize(variables.size());
    int c = 0;
    for(int var : pos2var) {
        var2pos[var] = c++;
        ordering.push_back(variables[var]);
    }
    
    for(synExp* s: expList) {
        if (s != synTrue) {
            s->computeIndices(varMap);
        }
    }
    if (doCNF) {
        std::ofstream dimacs(base+".dimacs");
        dimacs << "p cnf " <<  varMap.size() << " " << expList.size() << std::endl;
        for(synExp* s : expList) {
            for(int i : s->toDimacs(varMap))
                // First variable has to be 1, not 0. And so on...
                dimacs << i << " ";
            dimacs << "0" << std::endl;
        }
    }
    delete pcomp;
}
void Transform::reorderVariables() {
    if (scoreReorder) {
        if (scoreMethod == "perm") {
            PermScorer scorer(varMap, expList, menuBlocks, pos2var, var2pos, window, scoreThreshold);
            scorer.permGo();
        } else
            if (scoreMethod == "sifting") {
                SiftScorer scorer(varMap, expList, menuBlocks, pos2var, var2pos);
                scorer.siftGo(scoreThreshold);
            }
            else
                if (scoreMethod == "force") {
                    blocks = false;
                    forceScorer scorer(varMap, expList, pos2var, var2pos);
                    scorer.force();
                }
                else if (scoreMethod == "forceblocks") {
                    forceBlockScorer scorer(varMap, expList, menuBlocks, pos2var, var2pos);
                    scorer.forceBlocks();
                }
                else
                    if (scoreMethod == "random") {
                        RandomScorer scorer(varMap, expList, menuBlocks, pos2var, var2pos);
                        scorer.randomHeuristic(scoreThreshold);
                    }
                    else
                        if (scoreMethod == "other") {
                            SiftScorer scorer(varMap, expList, menuBlocks, pos2var, var2pos);
                            scorer.siftGo(scoreThreshold);
                            
                            OtherScorer scorer2(varMap, expList, menuBlocks, pos2var, var2pos);
                            scorer2.otherHeuristic(scoreThreshold);
                        }
                        else
                            if (scoreMethod == "file") {
                                std::ifstream vf(varOrderFile);
                                if (!vf) {
                                    std::cerr << "Couldn't open " << varOrderFile << std::endl;
                                    exit(-1);
                                }
                                int c = 0;
                                pos2var.clear();
                                var2pos.clear();
                                var2pos.resize(variables.size());
                                std::string var;
                                try {
                                    while (vf >> var) {
                                        pos2var.push_back(varMap.at(var));
                                        var2pos[varMap.at(var)] = c++;
                                    }
                                }
                                catch(std::exception e) {
                                    std::cerr << "Error looking up variable " << var << " from file " << varOrderFile << " in variable list." << std::endl;
                                    exit(-1);
                                }
                                vf.close();
                                if (c != variables.size()) {
                                    std::cerr << "Number of variables read does not match. Read " << c << " variables and there are " << variables.size() << std::endl;
                                    exit(-1);
                                }
                                std::cerr << "Successfully read " << c << " variables from file " << varOrderFile << ". varMap size is " << varMap.size() << std::endl;
                            }
        
                            else {
                                std::cerr << "Scoring method " << scoreMethod << " unknown." << std::endl;
                                exit(-1);
                            }
        
    }
}
void Transform::computeComponents() {
    if (static_components) {
        pcomp = new MultiComponents(var2pos, pos2var);
        for(synExp* s : expList) {
            pcomp->join(s, false);
        }
        
        pcomp->reorder();
        pcomp->checkLengths();
        int biggest = -1;
        for(std::pair<int, int> p : pcomp->listComponents())
            if (p.second > biggest)
                biggest = p.second;
        
        std::cerr << pcomp->getNumComponents() << " components. Biggest "
        << biggest << " number of variables " << pos2var.size() << std::endl;
        
        pos2var = pcomp->getOrder();
        var2pos.clear();
        var2pos.resize(variables.size());
        int c = 0;
        for(auto var: pos2var)
            var2pos[var] = c++;
        
    }
    else
        pcomp = new OneComponent(var2pos, pos2var);
}

constraintSorter* Transform::newCnstOrder(const std::string & s) {
    constraintSorter* sorter = NULL;
    if (s == "minimax")
        sorter = new minmaxSorter(adapter, expList);
    else
        if (s == "minspan")
            sorter = new minspanSorter(adapter, expList);
        else if (s == "remember")
            sorter = new RememberSorter(adapter, expList);
        else if (s == "none")
            sorter = new nullSorter(adapter, expList);
    //        else if (s == "exp")
    //            sorter = new expSorter(varMap, varList, clBound, consList, comp, var2pos, pos2var, adapter);
    //        else if (s == "comp")
    //            sorter = new compSorter(varMap, varList, clBound, consList, comp, var2pos, pos2var);
        else if (s != "none") {
            std::cerr << "Unknown constraint sorter " << s << std::endl;
            exit(-1);
        }
    return sorter;
}

void Transform::buildBDD() {
    std::string varfilename, statsfile, tempBDDName, appliedExpFilename, unappliedExpFilename;
    if (suffix == "") {
        varfilename          = base + ".reorder";
        statsfile            = base + ".data";
        tempBDDName          = base + ".temp";
        appliedExpFilename   = base + ".applied";
        unappliedExpFilename = base + ".unapplied";
    }
    else {
        varfilename          = base + "-" + suffix + ".reorder";
        statsfile            = base + "_" + suffix + ".data";
        tempBDDName          = base + "-" + suffix + ".temp";
        appliedExpFilename   = base + "-" + suffix + ".applied";
        unappliedExpFilename = base + "-" + suffix + ".unapplied";
    }
    
    //if (adapterType == "cudd")
    adapter = new cuddAdapter(cacheMultiplier);
    //else
    //if (adapterType == "buddy")
    //    adapter = new buddyAdapter();
    //else {
    //    std::cout << "No BDD engine specified. Exiting" << std::endl;
    //    exit(-1);
    //}
    if (dynamic_components)
        adapter->useComponents(var2pos, pos2var);
    else
        adapter->changeOrder(pos2var);
    
    for(std::string var : variables)
        adapter->newVar(var, "boolean");
    
    // Now apply the ordering that was computed before...
    adapter->shuffle(ordering);
    
    
    if (relatedVars)
        adapter->setRelatedVars();
    
    // We need to add the blocks for XOR expressions
    for(auto exp : expList) {
        adapter->setXORBlocks(exp);
    }
    // We set the order in var2pos
    adapter->syncOrder();
    //adapter->checkOrder();
    synExp::numVars(variables.size());
    
    bddBuilder myBuilder(expList);
    
    myBuilder.setAdapter(adapter);
    myBuilder.setVarFileName(varfilename);
    myBuilder.setStatsFileName(statsfile);

    constraintSorter* cs = newCnstOrder(constReorder);
    myBuilder.setConstraintSorter(cs);
    
    if (reorder)
        myBuilder.setReorderMethod(reorderMethod);
    else
        myBuilder.setReorderMethod("noreorder");
    
    myBuilder.setReorderInterval(reorderInterval);
    myBuilder.setMinNodes(minNodes);
    myBuilder.setLineLength(lineLength);
    myBuilder.setMaxNodes(maxNodes);
    myBuilder.setMinConstraint(minConstraint);
    myBuilder.setReorderEvery(reorderEvery);
    myBuilder.setBlocks(blocks);
    myBuilder.setTempBDDName(tempBDDName);
    myBuilder.setAppliedExpFilename(appliedExpFilename);
    myBuilder.setUnappliedExpFileName(unappliedExpFilename);
    
    myBuilder.build(inflation);
    std::cout << "BDD built: " << adapter->nodecount() << " nodes" << std::endl;
    adapter->saveBDD(base, suffix);
    delete adapter;
    delete cs;
}


} // namespace kconf



