#ifndef _ZEROOBJECTIVETERM_HPP_
#define _ZEROOBJECTIVETERM_HPP_

#include <map>
#include <vector>
#include <set>
#include <iostream>

#include "ObjectiveTerm.hpp"

class ZeroObjectiveTerm : public ObjectiveTerm {
  public:
    ZeroObjectiveTerm(double xCoefficient);
    ObjectiveTerm*   clone();
    void             translate(std::vector<int>& tr);
    std::vector<int> getFactors();
    ObjectiveTerm*   setToTrue(int var);
    ObjectiveTerm*   setToTrue(int var, std::map<int, ObjectiveTerm*>& map1, std::map<std::pair<int, int>, ObjectiveTerm*>& map2);
    ObjectiveTerm*   setToFalse(int var);
    bool             filter(int min, int max);
    bool             filter(std::set<int> set);
    bool             isZeroObjectiveTerm();
    ObjectiveTerm*   simplify(const std::vector<char>& v);
    double           evaluate(const std::vector<bool>& values);
};
#endif
