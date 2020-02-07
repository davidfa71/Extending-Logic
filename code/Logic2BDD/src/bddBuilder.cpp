//
//  bddBuilder.cpp
//  myKconf
//
//  Created by david on 11/10/14.
//  Copyright (c) 2014 david. All rights reserved.
//

#include "bddBuilder.hpp"

void bddBuilder::build(double inflation) {
    bool lastReorder      = true;
    int  nreorder         = 0;
    int  constLastReorder = 0;
    bool componentChanges = false;
    varFile.open(varFileName);
    statsFile.open(statsFileName);
    
    int  counter   = 1;
    long beginning = get_cpu_time();
    int  lastNodes = 100;
    writeNameOrder(adapter->giveOrder());
    // Main loop. A new constraint is added to the BDD each time
    for(std::vector<synExp*>::iterator itl = consList.begin(); itl != consList.end(); itl++) {
        
        long start = get_cpu_time();
        myOrder->reorderConstraints(itl, consList.end(), lastReorder || componentChanges);
        componentChanges = false;
        //myOrder->reorderConstraints(itl, consList.end(), lastReorder);
        synExp *pconst = *itl;
        printPartOne(pconst, counter);
        adapter->relateVars(pconst->giveSymbolIndices());
        adapter->apply(pconst);
        //adapter->syncOrder();
        int nn = adapter->nodecount();
        printPartTwo(nn);
        statsFile       << 100.0*counter/consList.size() << " " << nn << std::endl;
        lastReorder = false;
        if (counter > minConstraint ) {
            if (isReorderTime(counter, lastNodes, nn, constLastReorder, inflation)) {
                std::cout << " -> " << std::flush;
                adapter->syncOrder();
                componentChanges = true;
                nn = adapter->nodecount();
                std::cout << std::setw(8) << showHuman(nn);
                lastReorder      = true;
                if (isReorderTime(counter, lastNodes, nn, constLastReorder, inflation)) {
                    std::cout << std::endl;
                    constLastReorder = counter;
                    nreorder++;
                    reorderProtocol(itl);
                    std::cout << " => " << std::setw(8) << showHuman(adapter->nodecount());
                    writeNameOrder(adapter->giveOrder());
                }
            }
            
        }
        std::cout   << " " <<  std::setw(15)   << showtime(get_cpu_time() - start)      << " total "
        <<  std::setw(15)   << showtime(get_cpu_time() - beginning ) << std::endl;
        lastNodes = adapter->nodecount();
        counter++;
        myOrder->thisOneIsProcessed(pconst);
        pconst->thisOneIsProcessed();
    }
    // Destroy the expressions
    for(auto p : consList)
        p->destroy();
    
//    int nn = adapter->nodecount();
//    if (reorder && !lastReorder && nn < maxNodes) {
//        std::cout    << get_timestamp()      << " Last reordering. Nodes "
//                    << showHuman(lastNodes) << std::endl;
//        std::cout    << std::endl << "Syncing components..." << std::flush;
//        // If nreorder is 0 we don't need to sync anything and we save the time of building the
//        // interaction matrix, which could take a while
//        adapter->syncOrder();
//        std::cout << "done" << std::endl << std::flush;
//        adapter->reorder(reorderMethod);
//        std::cout   << get_timestamp()          << " Reordering done. "
//        << showHuman(adapter->nodecount())     << " nodes. " << std::endl << std::flush;
//    }
//    // Write the last order
//    writeNameOrder(adapter->giveOrder());
    // Delete the tempBDD, appliedExp and unappledExp?
    remove(tempBDDName.c_str());
    remove(appliedExpFilename.c_str());
    remove(unappliedExpFilename.c_str());
    //adapter->destroyInternal(internal);
    statsFile.close();
}

void bddBuilder::printOrder(std::vector<int> ord) {
    std::cout << std::endl << std::setw(5) << ord.size() << "              ";
    for(std::vector<int>::iterator itv = ord.begin(); itv != ord.end(); itv++)
        std::cout << *itv << " ";
    std::cout << std::endl;
    
}

void bddBuilder::writeNameOrder(const std::vector<std::string>& ord) {
    for(const std::string &s : ord)
        varFile << s << " ";
    
    varFile << std::endl;
}

//void bddBuilder::readVarOrder() {
//    std::cout << "Continuing from a previous order" << std::endl;
//    std::string var;
//    std::vector<std::string> tempvect;
//    int nvars   = 0;
//    int nconsts = 0;
//    int counter = 0;
//    std::ifstream in(varFileName.c_str());
//    if (in.good()) {
//        std::string temp, line;
//        while (std::getline(in, temp)) {
//            line = temp;
//        }
//        pos2var.clear();
//        std::istringstream ist(line);
 //       ist >> nconsts >> nvars;
//        std::cout << "nconsts " << nconsts << " nvars " << nvars << std::endl;
//        int c = 0;
//        var2pos.resize(varMap.size());
//        while (ist >> var) {
 //           var2pos[varMap[var]] = c;
//            pos2var[c++] = varMap[var];
//        }
//        reorderFromConst = nconsts;
//        std::cout   << "Pick up ordering from file. Reordering disabled until constraint #"
//                    << reorderFromConst << std::endl;
//    }
//}


//void bddBuilder::printPartOne(synExp* pconst, int counter) {
//    std::cout   << get_timestamp();
//    std::stringstream ost;
//    //ost  << std::setw(95) << std::left << pconst;
//    ost  << pconst;
//    std::string stConst = ost.str();
//    std::cout    << " Cnstr #" << std::setw(6)     << counter      << " "
//                << std::setw(3) << 100*counter/consList.size() << "% ";
//    std::string s;
//    s =  myOrder->giveInfo(pconst);
//    std::cout << s;
//    
//    std::cout << ": ";
//    if (stConst.length() <= lineLength)
//        std::cout <<  std::setw(lineLength) << std::left << stConst;
//    else
//        std::cout   <<   std::left <<  stConst  << std::endl
//        << std::setw(38+lineLength+s.length());
//}
void bddBuilder::printPartOne(synExp* pconst, int counter) {
    std::stringstream ost, sConst;
    ost << get_timestamp();
    ost << " Cnstr #" << std::setw(6)     << counter      << " "
        << std::setw(3) << 100*counter/consList.size() << "% ";
    ost << myOrder->giveInfo(pconst);
    ost << ": ";
    std::string beginning = ost.str();
    sConst << pconst;
    std::string stConst = sConst.str();
    std::cout << beginning; 
    if (stConst.length() <= lineLength)
        std::cout <<  std::setw(lineLength) << std::left << stConst;
    else
        std::cout << std::left << stConst  << std::endl
                  << std::setw(beginning.length()+lineLength) << " ";
}

void bddBuilder::printPartTwo(int nn) {
    std::cout       << std::right << " Nodes "        << std::setw(8) << std::right
                    << showHuman(nn)    << std::flush;
}


bool bddBuilder::isReorderTime(int counter, int lastNodes, int nn, int constLastReorder, double inflation) {
    return reorder && (counter > reorderFromConst) &&
    ((reorderEvery == 1 && reorder && nn > (1.0+inflation/100.0)*lastNodes && nn > minNodes && nn < maxNodes
      && counter - constLastReorder > reorderInterval)
     || (reorderEvery > 1 && counter % reorderEvery == 0));
    
}

void bddBuilder::reorderProtocol(std::vector<synExp*>::iterator itl) {
    // Write the BDD to a file
    adapter->saveBDD(tempBDDName, "");
    // Write the constraints already applied
    std::ofstream applied(appliedExpFilename);
    std::vector<synExp*>::iterator it = consList.begin();
    while (it != itl)
        applied << *it++ << std::endl;
    
    
    applied << *it << std::endl;
    applied.close();
    // Write the constraint not yet applied
    std::ofstream unapplied(unappliedExpFilename);
    it = itl;
    it++;
    while (it != consList.end())
        unapplied << *it++ << std::endl;
    
    unapplied.close();
    // Finally reorder
    adapter->reorder(reorderMethod);
}
