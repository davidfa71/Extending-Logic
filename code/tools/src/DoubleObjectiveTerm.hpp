#ifndef _DOUBLEOBJECTIVETERM_HPP_
#define _DOUBLEOBJECTIVETERM_HPP_

#include <map>
#include <vector>
#include <set>
#include <iostream>

#include "ObjectiveTerm.hpp"

class DoubleObjectiveTerm : public ObjectiveTerm {
  public:
    DoubleObjectiveTerm(double xCoefficient, int factor1, int factor2);
    ObjectiveTerm*   clone();
    void             translate(std::vector<int>& tr);
    std::vector<int> getFactors();
    ObjectiveTerm*   setToTrue(int var, std::map<int, ObjectiveTerm*>& map1, std::map<std::pair<int, int>, ObjectiveTerm*>& map2);
    ObjectiveTerm*   setToTrue(int var);
    ObjectiveTerm*   setToFalse(int var);
    bool             filter(int min, int max);
    bool             filter(std::set<int> set);
    ObjectiveTerm*   simplify(const std::vector<char>& v);
    
    double           evaluate(const std::vector<bool>& values);

  private:
    double 	     first, second;
};
#endif
