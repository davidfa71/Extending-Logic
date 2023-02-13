#include <map>
#include <vector>

#include "synExp.hpp"
#include "VariableTable.hpp"
#include "eval.hpp"

void addSymbols2Table(std::vector<std::string>& vars);
int  simplifySymbol(const std::string& sn, synExp* value);
void makeReverseIndex(std::vector<synExp*>& pC);
void simplifyExpression(int numExp, synExp* e, std::unordered_set<int>& nextSimplify);
std::vector<synExp*> simplifyConstraints(std::vector<synExp*>& pendingConstraints);

