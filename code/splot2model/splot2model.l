/* enable c++ scanner class generation */
%option c++
/* the manual says "somewhat more optimized" */
%option batch
/* enable scanner to generate debug output. disable this for release
 * versions. */
%option debug
/* We'll handle the files manually */
%option noyywrap

%{
    #include <libgen.h>
    #include <iostream>
    #include <iomanip>
    #include <string>
    #include <map>
    #include <list>
    #include <vector>
    #include <set>
    #include <sstream>
    #include <fstream>
    #include <stdlib.h>
    #include <algorithm>
    
    #include "synExp.hpp"
    #include "mytime.hpp"
    #include "eval.hpp"
    #include "simplify.hpp"

    #include "VariableTable.hpp"
   
    int longname     = 0;
    bool useXOR      = false;
    bool beforeId    = true;
    const int startNode = 1;
    std::string rootString;
    typedef enum {rootNode, mandatoryNode, optionalNode, groupNode, groupPartNode } nodeType;
    typedef enum { orCard, xorCard, otherCard } groupCard;
    typedef enum { elTree, elConstraint, elOther, elEOF } element;
    groupCard theCard;
    element theElement = elOther;
    int newcount = 1;
    int idtabcount = 0, tabcount = 0;
    int low, high;
    nodeType cardtype;
    std::string lexeme;
    bool rules = false;
    std::list<std::list<int> > clauses;
    std::vector<std::string> varnames;
    std::vector<synExp*> expressions;
    
    
    class node {
        
        public :
        
        typedef std::vector<int>::iterator iterator;
        groupCard          theCard;
        std::string             name;
        nodeType           cardtype;
        int                numberOfChildren, low, high, father, number;
        std::vector<int>        children;
        node::iterator     begin() { return children.begin(); };
        node::iterator     end()   { return children.end();   };
        node()  {};
    };

    
    std::map<int, node>    nodes;
    std::map<std::string, node> nodeMap;
    std::vector<std::string>    stack;
    std::list<int>         clause;
    std::set<int>          ecrSet;
    
    std::map<int, node> myMap;

%}

newline   \n
space     [\ ]
tab	  \t
id        [a-zA-Z\_0-9\.\"\-\+\$\/\>\<]* 
root      :r
mandatory :m
optional  :o
group     :g
groupPart :
xorCard   \[1,1\]
orCard    \[1,\*\]

otherCard \[[0-9\*]+,[0-9\*]+\]
%%
{space}+                 {                                           }
{tab}	                 { if (beforeId) tabcount++;                 }
\<feature\_tree\>        { theElement = elTree;                      }
\<constraints\>          { theElement = elConstraint; return elOther;} //Just to rearrange the nodes
\<![^\>]*\>              {                                          }
\<[^\><]*\>              { theElement = elOther;  lexeme = "" ;      }
{newline}                {   
			    if (beforeId == false) {
                            beforeId = true;
                            tabcount = 0;
                            
                            return theElement;
                            }
                         }
(c|C)[a-zA-Z\_0-9]+:     { clause.clear();                                                     }
{root}                   { cardtype = rootNode;                                                }
{mandatory}              { cardtype = mandatoryNode;                                           }
{optional}               { cardtype = optionalNode;                                            }
{group}                  { beforeId = false; idtabcount = tabcount; cardtype = groupNode;      }
{groupPart}              { beforeId = false; idtabcount = tabcount; cardtype = groupPartNode;  }
(or)|(OR)                {                                                                     }
{id}                     {  
			    beforeId = false;
                            if (theElement == elTree) {
                                idtabcount = tabcount;
                                if (lexeme == "") {
                                    lexeme = yytext;
                                    transform(lexeme.begin(), lexeme.end(), lexeme.begin(), ::tolower);
                                }
                                else  {
                                std::ostringstream ost;
                                ost << lexeme << "_" << yytext;
                                lexeme = ost.str();
                                transform(lexeme.begin(), lexeme.end(), lexeme.begin(), ::tolower);
                                }
                            }
                            else
                                if (theElement == elConstraint) {
                                    if (nodeMap.find(yytext)==nodeMap.end()) {
                                        std::cerr << "Unknown feature " << std::setw(20) << yytext << " in constraints" << std::endl;
                                    }
                                    clause.push_back(nodeMap[yytext].number);
                                    ecrSet.insert(nodeMap[yytext].number);
                                };
                        }
\~{id}                  { beforeId = false;
                          if (theElement == elConstraint) {
                              std::string aux = std::string(yytext, 1, strlen(yytext)-1);
                              if (nodeMap.find(aux)==nodeMap.end()) {
                                  std::cerr << "Unknown feature " << std::setw(20) << aux << " in constraints" << std::endl;
                              }
                              clause.push_back(-nodeMap[aux].number);
                              ecrSet.insert(-nodeMap[aux].number);
                          }
                        };

{orCard}    {  theCard     =  orCard;  low = 1; high = -1;       }
{xorCard}   {  theCard     =  xorCard; low = 1; high =  1;       }
{otherCard} {  theCard     =  otherCard; 
    std::string txt  =  std::string(yytext);
    int coma    =  txt.find(',');
    low         =  atoi(std::string(txt,1,coma-1).c_str());
    high        =  atoi(std::string(txt, coma+1, txt.length() - coma - 2).c_str());
}
\({id}\)    {  lexeme = std::string(yytext,1, strlen(yytext)-2);  };
<<EOF>>     {  theElement = elEOF; return theElement; }
.           {                                            };
%%

std::string newName() {
    std::ostringstream ost;
    ost << "Aux_" << newcount++;
    return ost.str();
}; 

void scan(std::string filename) {
    std::list<std::string> printStack;
    std::map<int, std::string>     stack;
    std::ifstream in(filename.c_str());
    FlexLexer* lexer = new yyFlexLexer(&in);
    node theNode;
    // Needs to start at 1. 0 cannot be used
    // for the root node, because -0 does not
    // exist, so the root cannot be negated
    int nodeCounter = startNode;
    element res = (element)(lexer->yylex());
    while (res != elTree && res != elEOF) {
        res = (element)(lexer->yylex());
    }
    while (res == elTree)  {
        // Group nodes don't have a name
        if (lexeme == "") lexeme = newName();
        if (nodeMap.find(lexeme) != nodeMap.end()) {
            std::string name;
            // The name is repeting, we have to differentiate it
            int myCount = 2;
            do {
                std::ostringstream ost;
                ost << lexeme << "_" << myCount++;
                name = ost.str();
            } while (nodeMap.find(name) != nodeMap.end());
            lexeme = name;
        }
        if (lexeme.length() > longname)
        longname = lexeme.length() + 1;
        
        if (cardtype == rootNode)
            rootString = lexeme;
        
        theNode.name     = lexeme;
        theNode.number   = nodeCounter;
        theNode.cardtype = cardtype;
        theNode.low      = low;
        theNode.high     = high;
        theNode.theCard  = theCard;

        if (idtabcount != 0)
            theNode.father  = nodeMap[stack[idtabcount-1]].number;
        else
            theNode.father  = -1;
            
        nodeMap[lexeme]    = theNode;
        stack[idtabcount]  = lexeme;
        
        if (idtabcount != 0) {
            // We link the node to the parent
            nodeMap[stack[idtabcount-1]].children.push_back(nodeCounter);
        }
        
        idtabcount  = 0;
        lexeme    = "";
        theNode.children.clear();
        res = (element)(lexer->yylex());
        nodeCounter++;
    } // while (res == 1)
    for(auto p : nodeMap) {
        if (p.second.high == -1)
            p.second.high = p.second.children.size();
        nodes[p.second.number] = p.second;
    }
    
    // Now the constraints...
    
    while (res != elConstraint && res != elEOF) {
        res = (element)(lexer->yylex());
    }
    while (res == elConstraint)  {
        if (!clause.empty()) {
            clauses.push_back(clause);
            clause.clear();
        }
        res = (element)(lexer->yylex());
    };

   std::cerr << "Number of clauses " << clauses.size() << std::endl;
}

bool next_index(std::vector<int>& index, int top) {
    int size = index.size();
    for(int pos = size-1; pos > -1; pos--)
    if (index[pos] + size - pos < top) {
        int counter = index[pos];
        for(int newpos = pos; newpos < size; newpos++)
        index[newpos] = ++counter;
        return true;
    }
    return false;
}

void fillExpressions(int k) {
    std::vector<int> oneClause;
    if (nodes[k].cardtype != groupNode)
        varnames.push_back(nodes[k].name);
    //std::cerr << "k " << k << " name " << nodes[k].name << " low " << nodes[k].low << " high " << nodes[k].high  << std::endl;
    if (k != startNode) { // 0 is the root, has no parent to imply
        // The child implies its parent
        if (nodes[k].cardtype != groupPartNode && nodes[k].cardtype != groupNode) {
            synExp *child2parent = makeOr(makeNot(new synSymbol(nodes[k].name)),
                                          new synSymbol(nodes[nodes[k].father].name));
            expressions.push_back(child2parent);
        }
        // If the node is a group part node we point to the real parent
        // not the auxiliary node
        if (nodes[k].cardtype == groupPartNode) {
            synExp* group2parent = makeOr(makeNot(new synSymbol(nodes[k].name)),
                                    new synSymbol(nodes[nodes[nodes[k].father].father].name));
            expressions.push_back(group2parent);
        }
        // If the node is an auxiliary node, we do nothing
    }
    
    if (nodes[k].cardtype != groupNode) {
        for(int l = 0; l < nodes[k].children.size(); l++) {
            if (nodes[nodes[k].children[l]].cardtype == mandatoryNode) {
                // The child is mandatory or group, so the parent implies the child
                synExp *parent2child = makeOr(makeNot(new synSymbol(nodes[k].name)),
                                        new synSymbol(nodes[nodes[k].children[l]].name));
                expressions.push_back(parent2child);
            }
            fillExpressions(nodes[k].children[l]); // Recursive call
        }
    }
    else { // Node is a group node
        // Parent implies child, the group node is "mandatory" for the parent
        for(int l = 0; l < nodes[k].children.size(); l++) {
            fillExpressions(nodes[k].children[l]); // Recursive call
        }
        oneClause.clear();
        
        // Let's go with the low restriction [ atleast ]
        // we have to form all the sets of children.size()-low+1
        // and add -k to the clause
        bool xorGroup = false;
        if  (nodes[k].low == 1 && nodes[k].high == 1 && useXOR)
            xorGroup = true;
        
        if (xorGroup) {
            std::vector<synExp*> vXOR;
            for(int i : nodes[k].children)
                vXOR.push_back(new synSymbol(nodes[i].name));
            synExp *parent2XOR = makeOr(makeNot(new synSymbol(nodes[nodes[k].father].name)),
                                makeXOR(vXOR));
                                
            expressions.push_back(parent2XOR);
        }
        if  (nodes[k].low > 0 && !xorGroup) {
            std::vector<int> indices_low(nodes[k].children.size() - nodes[k].low + 1);
            int counter = 0;
            // initialization of the indices
            for(int x = 0; x < indices_low.size(); x++) indices_low[x] = counter++;
            // we add the REAL parent (not the aux)
            do {
                synExp *parent = makeNot(new synSymbol(nodes[nodes[k].father].name));
                synExp *clause = synFalse;
                for(int x = 0; x < indices_low.size(); x++)
                    clause = makeOr(clause, new synSymbol(nodes[nodes[k].children[indices_low[x]]].name));
                
                expressions.push_back(makeOr(parent, clause));
            } while (next_index(indices_low, nodes[k].children.size()));
        }
        // nodes[k].low = 0, then there is nothing to add
        // Let's go with the high, [atmost]
        if  (nodes[k].high != nodes[k].children.size() && !xorGroup) {
            // We have to form all the subsets of high+1 and add -k
            std::vector<int> indices_high(nodes[k].high+1);
            int counter = 0;
            for(int x = 0; x < indices_high.size(); x++) indices_high[x] = counter++;
            // we add the parent
            do {
                synExp *parent = makeNot(new synSymbol(nodes[nodes[k].father].name));
                synExp *clause = synFalse;
                for(int x = 0; x < indices_high.size(); x++)
                    clause = makeOr(clause, makeNot(new synSymbol(nodes[nodes[k].children[indices_high[x]]].name)));

                expressions.push_back(makeOr(parent, clause));
            } while (next_index(indices_high, nodes[k].children.size()));
        }
    }// else { // Node is a group node
}

void fillExpressions() {
    // The root is now 1, instead of 0
    fillExpressions(startNode);
    for(auto c : clauses) {
        synExp* clause = synFalse;
        for(auto lit : c) {
            if (lit < 0)
                clause = makeOr(clause, makeNot(new synSymbol(nodes[-lit].name)));
            else
                clause = makeOr(clause, new synSymbol(nodes[lit].name));
        }
        if (clause != synTrue)
           expressions.push_back(clause);
    }
}

void printTree(std::string filename) {
    std::ofstream tree(filename.c_str());
    for(auto p : nodes) {
        tree << p.first << " " << p.second.name << " " << p.second.children.size() << " : ";
        for(int c : p.second.children)
            tree << c << " ";
        tree << std::endl;
    }
}

void printExpressions(std::string expFile) {
  std::ofstream fExpFile(expFile.c_str());
  for(synExp* exp : expressions)
    fExpFile << exp << std::endl;
  
}

void printVars(std::string varFile) {
    std::ofstream fVarFile(varFile.c_str());
    for(std::string var : varnames)
        fVarFile << var << " ";
        
    fVarFile << std::endl;
}

void printDIMACS(std::string dimacsFile) {
    std::ofstream dimacs(dimacsFile);
    std::unordered_map<std::string, int> varMap;
    int c = 0;
    for(std::string s : varnames)
        varMap[s] = c++;
        
    dimacs << "p cnf " <<  varnames.size() << " " << expressions.size() << std::endl;
    for(synExp* s : expressions) {
        for(int i : s->toDimacs(varMap))
        // First variable has to be 1, not 0. And so on...
        dimacs << i << " ";
        dimacs << "0" << std::endl;
    }
}

int main(int argc, char** argv) {
    long start = get_cpu_time();

    int paramCounter = 1;
    if (argc > 1 and (std::string(argv[paramCounter]) == "-use-XOR")) {
        useXOR = true;
        paramCounter++;
    }
    if ((argc - paramCounter) != 1) {
        std::cerr << "Usage : " << argv[0] << " [-use-XOR] <filename>" << std::endl;
        exit(-1);
    }
    std::string inputfile = argv[paramCounter];
    scan(inputfile);

    // The root has to be in
    expressions.push_back(new synSymbol(nodes[startNode].name));
    fillExpressions();
    addSymbols2Table(varnames);
    expressions = simplifyConstraints(expressions);
    size_t t = inputfile.find('.');
    std::string basename   = std::string(inputfile, 0, t);
    std::string varFile    = basename + ".var";
    std::string expFile    = basename + ".exp";
    std::string treeFile   = basename + ".tree";
    std::string dimacsFile = basename + ".dimacs";
    printTree(treeFile);
    printExpressions(expFile);
    printVars(varFile);
    if (!useXOR)
        printDIMACS(dimacsFile);
        
    long end = get_cpu_time();
    std::cout << "Time: " << showtime(end - start) << " ("
    << (end - start) << " ms)" << std::endl;
        
    return 0;
    
}
