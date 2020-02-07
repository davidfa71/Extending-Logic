//
//  MultiComponents.cpp
//  myKconf
//
//  Created by David on 01/12/15.
//  Copyright ?? 2015 david. All rights reserved.
//

#include "MultiComponents.hpp"

MultiComponents::MultiComponents(std::vector<int>& var2pos, std::vector<int>& pos2var) :
var2pos(var2pos), pos2var(pos2var){ init(int(var2pos.size()));  numMultiComponents = int(var2pos.size()); }

void   MultiComponents::init(int num) {
    rank.clear();
    rank.resize(num);
    parent.clear();
    parent.resize(num);
    start.clear();
    start.resize(num);
    length.clear();
    length.resize(num);
    changed.clear();
    changed.resize(num);
    numMultiComponents = num;
    maxLength     = 1;
    outofsync     = false;
    for(int var = 0; var < num; var++)
        makeSet(var);
}

void MultiComponents::makeSet(int var) {
    parent[var]     = var;
    rank[var]       = 0;
    start[var]      = var2pos[var];
    length[var]     = 1;
    changed[var]    = 1;
}


void MultiComponents::changeOrder(std::vector<int> x) {
    pos2var = x;
    int c = 0;
    for(int v : pos2var)
        var2pos[v] = c++;
}

std::vector<int> MultiComponents::getOrder() {
    return pos2var;
}
void MultiComponents::newVariable() {
    int var = var2pos.size();
    numMultiComponents++;
    var2pos.push_back(var);
    pos2var.push_back(var);
    parent.push_back(var);
    rank.push_back(0);
    start.push_back(var);
    length.push_back(1);
    changed.push_back(1);
}

bool MultiComponents::alreadyJoined(int x, int y) {
    return (find(x) == find(y));
}
// Joins two components just as makeUnion, but also returns 
// the differential in entropy
float MultiComponents::joinEntropy(int x, int y) {
    float res;
    int  xRoot   = find(x);
    int  yRoot   = find(y);
    
    if (xRoot == yRoot)
        return 0;
    

    float pr1 = length.at(xRoot)/(float)var2pos.size();
    float pr2 = length.at(yRoot)/(float)var2pos.size();
    res =  pr1*log2(pr1) + pr2*log2(pr2);
    numMultiComponents--;
    int newRep, other;
    if (rank.at(xRoot) < rank.at(yRoot)) {
        newRep = xRoot;
        other  = yRoot;
    }
    else {
        newRep = yRoot;
        other  = xRoot;
        if (rank.at(xRoot) == rank.at(yRoot))
            rank.at(newRep)++;
    }
    parent.at(other)  = newRep;
    length.at(newRep) += length.at(other);
    if (length.at(newRep) > maxLength)
        maxLength = length.at(newRep);
    

    if (start.at(other) < start.at(newRep))
        start.at(newRep) = start.at(other);
    
    float pr = length.at(newRep)/(float)var2pos.size();
    res -= pr*log2(pr);
    return res;
}
bool MultiComponents::makeUnion(int x, int y, bool changeOrder) {
    bool changes = false;
    int  xRoot   = find(x);
    int  yRoot   = find(y);
    
    
    //std::cerr << "Union " << x << " rep " << xRoot << ": start " << start[xRoot] << ", length " << length[xRoot]
    //          << ". " << y << " rep " << yRoot << ": start " << start[yRoot] << " length " << length[yRoot]
    //          << std::endl;
    
    if (xRoot == yRoot)
        return changes;
    
    //std::cerr << "MakeUnion(" << x << ", " << y << ") roots " << xRoot << ", " << yRoot << " startå " << start[xRoot] << ", "  << start[yRoot] << std::endl;
    numMultiComponents--;
    int newRep, other;
    if (rank.at(xRoot) < rank.at(yRoot)) {
        newRep = xRoot;
        other  = yRoot;
    }
    else {
        newRep = yRoot;
        other  = xRoot;
        if (rank.at(xRoot) == rank.at(yRoot))
            rank.at(newRep)++;
    }
    //std::cerr << "newRep " << newRep << " other " << other << std::endl;
    //std::cerr << "length newRep " << length.at(newRep) << " length other " << length.at(other) << std::endl;
    parent.at(other)  = newRep;
    length.at(newRep) += length.at(other);
    //std::cerr << "length newRep " << length.at(newRep) << std::endl;
    if (length.at(newRep) > maxLength)
        maxLength = length.at(newRep);
    

    if (start.at(other) < start.at(newRep))
        start.at(newRep) = start.at(other);
    //std::cerr << "start newRep " << start.at(newRep) << std::endl;
    return true;
}

int MultiComponents::getLength(int x) { return length[x]; }

//bool MultiComponents::makeUnion(int x, int y, bool changeOrder) {
//    bool changes = false;
//    int  fixStart, left, right, posleft = 0, posright = 0;
//    int  leftAndRight, rightAndLeft;
//    int  xRoot   = find(x);
//    int  yRoot   = find(y);
//
//
//    if (changeOrder) check();
//    //std::cerr << "Union " << x << " rep " << xRoot << ": start " << start[xRoot] << ", length " << length[xRoot]
//    //          << ". " << y << " rep " << yRoot << ": start " << start[yRoot] << " length " << length[yRoot]
//    //          << std::endl;
//
//    if (xRoot == yRoot)
//        return changes;
//
//    //std::cerr << "MakeUnion(" << x << ", " << y << ") roots " << xRoot << ", " << yRoot << " start " << start[xRoot] << ", "  << start[yRoot] << std::endl;
//    numMultiComponents--;
//    // Which one is left, which one is right
//    if (start[xRoot] < start[yRoot]) {
//        left     = xRoot;
//        right    = yRoot;
//    }
//    else {
//        left     = yRoot;
//        right    = xRoot;
//    }
//
//    //std::cerr << "left " << left << " start " << start[left] << " right " << right << " start " << start[right] << std::endl;
//
//    if (changeOrder) {
//        posleft  = var2pos.at(left);
//        posright = var2pos.at(right);
//        bool flipped = false;
//        int small, big;
//        // The small component moves towards the big one.
//        if (length.at(left) < length.at(right)) {
//            small = left;
//            big   = right;
//        }
//        else {
//            small = right;
//            big   = left;
//        }
//        // What would be the distance from leftvar to rightvar if LR
//        leftAndRight = (start.at(left) + length.at(left) - posleft - 1) +
//        (posright - start.at(right));
//        //std::cerr << " posleft " << posleft << " posright " << posright << std::endl;
//        //std::cerr << "leftAndRight " << leftAndRight << std::endl;
//
//        // What would be the distance from rightvar to leftvar if RL (left becomes right and vv)
//        rightAndLeft = (start.at(right) + length.at(right) - posright - 1) +
//        (posleft - start.at(left));
//
//        //std::cerr <<"rightAndLeft " << rightAndLeft << std::endl;
//        bool LR;
// //       if (leftAndRight <= rightAndLeft)
//            LR        = true;
// //       else
// //           LR        = false;
//        if (LR == true && (start.at(left) + length.at(left)) == start.at(right)) {
//            // We never set outofsync to false. Only sync does that.
//            // If outofsync was true before and we set it to false, we screw up the ordering
//            //outofsync    = false;
//            start.at(right) = start.at(left);
//            //changes |= flipped;
//        }
//        else {
//            outofsync = changes = true;
//            std::vector<int> newOrder, block, between;
//            if (LR) {
//                for(int x = start.at(left); x < start.at(left)+length.at(left); x++)
//                  block.push_back(pos2var.at(x));
//                //std::cerr   << "Right, start " << start.at(right) << " length is " << length.at(right)
//                //            << " pos2var size is " << pos2var.size() << std::endl;
//                for(int x = start.at(right); x < start.at(right)+length.at(right); x++) {
//                    block.push_back(pos2var.at(x));
//                }
//
//
//            }
//            else {
//                for(int x = start.at(right); x < start.at(right)+length.at(right); x++)
//                    block.push_back(pos2var.at(x));
//
//                for(int x = start.at(left); x < start.at(left)+length.at(left); x++)
//                    block.push_back(pos2var.at(x));
//
//
//            }
//            // MultiComponents in between
//
//            for(int x = start.at(left) + length.at(left); x < start.at(right); x++)
//                between.push_back(pos2var.at(x));
//
//
//            //std::cerr << "block size " << block.size() << " between size " << between.size() << std::endl;
//            //std::cerr << "start.at(left) " << start.at(left) << " LR " << LR << std::endl;
//            // Everything before left component is copied as is.
//
//            for(int x = 0; x < start.at(left); x++)
//                newOrder.push_back(pos2var.at(x));
//
//            // What now?
//            int dif;
//            if (small == left) {
//                dif = newOrder.size() - (start.at(left) + length.at(left));
//                for(int x : between) {
//                    newOrder.push_back(x);
//                    start.at(x) += dif;
//                }
//                int blockstart = newOrder.size();
//                for(int x : block) {
//                    start.at(x) = blockstart;
//                    newOrder.push_back(x);
//                }
//            }
//            else {
//                int blockstart = newOrder.size();
//                for(int x : block) {
//                    start.at(x) = blockstart;
//                    newOrder.push_back(x);
//                }
//                dif = newOrder.size() - (start.at(left) + length.at(left));
//                //std::cerr << "small is right. dif " << dif << std::endl;
//
//                for(int x : between) {
//                    newOrder.push_back(x);
//                    start.at(x) += dif;
//                }
//            }
//            // Everything after right is also copied as is
//
//            for(int x = newOrder.size(); x < parent.size(); x++)
//                newOrder.push_back(pos2var.at(x));
//
//            // Install the new order
//            for(int z = 0; z < newOrder.size(); z++)
//                pos2var.at(z) = newOrder[z];
//
//            int c = 0;
//            for(int v : pos2var)
//                var2pos.at(v) = c++;
//            //std::cerr << "Here!" << std::endl;
//        }
//    }
//
//    int newRep, other;
//    if (rank.at(left) < rank.at(right)) {
//        newRep = left;
//        other  = right;
//    }
//    else {
//        newRep = right;
//        other  = left;
//        if (rank.at(left) == rank.at(right))
//            rank.at(newRep)++;
//    }
//    //std::cerr << "newRep " << newRep << " other " << other << std::endl;
//    //std::cerr << "length newRep " << length.at(newRep) << " length other " << length.at(other) << std::endl;
//    parent.at(other)  = newRep;
//    length.at(newRep) += length.at(other);
//    if (length.at(newRep) > maxLength)
//        maxLength = length.at(newRep);
//
//    //    posleft  = std::find(order.begin(), order.end(), x) - order.begin();
//    //    posright = std::find(order.begin(), order.end(), y) - order.begin();
//    //
//    //    if (LR) {
//    //        minReorder[newRep] = posleft;
//    //        maxReorder[newRep] = posright;
//    //    }
//    //    else {
//    //        minReorder[newRep] = posright;
//    //        maxReorder[newRep] = posleft;
//    //    }
//    if (changeOrder)
//        check();
//    else {
//        if (start.at(other) < start.at(newRep))
//            start.at(newRep) = start.at(other);
//    }
//    return changes;
//}

void MultiComponents::flip(int x) {
    std::list<int> temp;
    int pos = start[x];
    for(int c = 0; c != length[x]; c++) {
        temp.push_front(pos2var[pos++]);
    }
    pos = start[x];
    for(int var : temp) {
        pos2var[pos] = var;
        var2pos[var] = pos++;
    }
}

int MultiComponents::find(int x) {
    if (parent[x] == x)
        return x;
    
    return find(parent[x]);
}

bool MultiComponents::join(synExp* s, bool changeOrder) {
    //played.insert(s);
    return internalJoin(s, changeOrder);
}
bool MultiComponents::internalJoin(synExp* s, bool changeOrder) {
    bool changes = false;
    //std::cerr << std::endl;
    //printComponents();
    std::set<int>::iterator its1 = s->giveSymbolIndices().begin();
    for(std::set<int>::iterator its2 = its1; its2 != s->giveSymbolIndices().end(); its2++)
            if (its1 != its2) {
                //std::cerr << std::endl << "MakeUnion (" << *its1 << ", " << *its2 << ")" << std::endl;
                changes |= makeUnion(*its1, *its2, changeOrder);
                //printComponents();
                //compConstraints[find(*its1)].push_back(s);
                //reorder();
                changed[find(*its1)] = 1;
            }
    return changes;
}
// Watch out. The first bit in the pair is a position, not a variable
// The set is of the for <position, length>
std::set<std::pair<int, int> > MultiComponents::listChangedComponents() {
    check();
    int l = 0;
    std::set<std::pair<int, int> > theSet;
    while (l < pos2var.size()) {
        int ol = pos2var[l];
        int fol = find(ol);
        int ll = length[fol];
        if (changed[fol] == 1) {
            theSet.insert(std::pair<int, int>(l, ll));
        }
        l += ll;
    }
    return theSet;
}

// Watch out. The first bit in the pair is a position, not a variable
// The set is of the for <position, length>
std::set<std::pair<int, int> > MultiComponents::listComponents() {
    //check(order);
    int l = 0;
    std::set<std::pair<int, int> > theSet;
    while (l < pos2var.size()) {
        int ol = pos2var[l];
        int fol = find(ol);
        int ll = length[fol];
        theSet.insert(std::pair<int, int>(l, ll));
        l += ll;
    }
    return theSet;
}

void MultiComponents::printComponents() {
    //check(order);
    int l = 0;
    std::set<std::pair<int, int> > theSet;
    while (l < pos2var.size()) {
        std::cerr << "< ";
        int ol  = pos2var[l];
        int fol = find(ol);
        int ll  = length[fol];
        for(int a = 0; a < ll; a++)
            std::cerr << pos2var[l++] << " ";
        
        std::cerr << ">";
    }
}
void MultiComponents::setUnchanged(int x) {
     changed[find(x)] = 0;
}

void MultiComponents::showInfo(int x = 0) {
    int max;
    if (x+10 < parent.size()) max = x+30;
    else max = int(parent.size());
    for(int a = x; a < max; a++) {
        std::cerr   << " pos " << std::setw(5) << a << " var " << std::setw(4) << pos2var[a]
                    << " component " << std::setw(4) << find(pos2var[a]) << " start "
                    << std::setw(4) << start[find(pos2var[a])]
                    << " length " << std::setw(4) << length[find(pos2var[a])] << std::endl;
    }
}

void MultiComponents::reorder() {
    //std::cerr << "-----Reorder " << std::endl;
    int c = 0;
    std::map<int, float> avg;
    std::map<int, int>   num;
    for(int i : pos2var) {
        avg[find(i)] += var2pos.at(i);
        num[find(i)]++;
    }

    for(std::pair<int, int> p : num)
        avg[p.first] /= (float) p.second;
    

    std::vector<orderHelper> h;
    for(int i : pos2var) {
        h.push_back(orderHelper(i, var2pos.at(i), start[find(i)], avg.at(find(i))));
    }
    orderHelper o(0, 0, 0, 0);
    std::sort(h.begin(), h.end(), o);
    pos2var.clear();
    for(orderHelper& x : h) {
        pos2var.push_back(x.id);
        var2pos[x.id] = c++;
    }
    
    int pos = 0;
    while (pos < pos2var.size()) {
        //std::cerr << "start at " << find(var2pos[pos]) << " = " << pos << std::endl;
        start.at(find(pos2var[pos])) = pos;
        pos += length.at(find(pos2var[pos]));
    }
    //check();
    //std::cerr << "----- End Reorder " << std::endl;

}
void MultiComponents::check() {
    std::set<int> s;
    for(int q : pos2var) {
        s.insert(q);
    }
    if (s.size() != parent.size()) {
        showInfo(0);
        std::cerr << "Error. parent size " << parent.size() << " only " << s.size()
            << " different values " << std::endl;
        
        exit(-1);
    }
    int lastcomponent = -1, remaining = 0;
    for(int a = 0; a < parent.size(); a++) {
        if (find(pos2var[a]) != lastcomponent) {
            lastcomponent = find(pos2var[a]);
            if (remaining != 0) {
                int x = 0;
                if (a > 5) x = a - 5;
                showInfo(x);
                std::cerr << "Error processing pos " << a << " remaining " << remaining << std::endl;
                exit(-1);
            }
            if (a != start[find(pos2var[a])]) {
                int x = 0;
                if (a > 5) x = a - 5;
                showInfo(x);
                std::cerr << "Error processing pos " << a << " start is " << start[find(pos2var[a])] << std::endl;
                exit(-1);
            }
            remaining = length[find(pos2var[a])] - 1;
        }
        else remaining--;
    }
}

//void MultiComponents::replayConstraints() {
//    std::cerr << "Replaying " << played.size() << " expressions ----------------------------------" << std::endl;
//    for(auto exp : played) {
//        //std::cerr << "Replaying " << (void*) exp;
//        //std::cerr << " " << exp << std::endl;
//        internalJoin(exp, false);
//    }
//}
void MultiComponents::checkLengths() {
    int sumLengths = 0;
    std::set<std::pair<int, int> > theSet = listComponents();
    for(auto pair : theSet) {
        sumLengths += pair.second;
    }
    if (sumLengths != var2pos.size()) {
        std::ostringstream ost;
        ost << "In checkLengths, sumLengths is "  << sumLengths << " and var2pos size is " << var2pos.size() << std::endl;
        throw std::logic_error(ost.str());
    }
}
float MultiComponents::computeEntropy() {
    reorder();
    check();
    int sumLengths = 0;
    std::set<std::pair<int, int> > theSet = listComponents();
    float res = 0;
    int  size = int(var2pos.size());
    for(auto pair : theSet) {
        float ss = pair.second;
        res += -ss*log2(ss/size);
        sumLengths += ss;
    }
    if (sumLengths != var2pos.size()) {
        std::ostringstream ost;
        ost << "In computeEntropy, sumLengths is "  << sumLengths << " and var2pos size is " << var2pos.size() << std::endl;
        throw std::logic_error(ost.str());
    }
    return res /size;
}

