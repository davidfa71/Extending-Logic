#ifndef _SINGLEOBJECTIVETERM_HPP_
#define _SINGLEOBJECTIVETERM_HPP_

#include <map>
#include <vector>
#include <set>
#include <iostream>

#include "ObjectiveTerm.hpp"
#include "ZeroObjectiveTerm.hpp"


class SingleObjectiveTerm : public ObjectiveTerm {
  public:
    SingleObjectiveTerm(double xCoefficient, int xterm);
    ObjectiveTerm*   clone();
    void             translate(std::vector<int>& tr);
    std::vector<int> getFactors();
    ObjectiveTerm*   setToTrue(int var);
    ObjectiveTerm*   setToTrue(int var, std::map<int, ObjectiveTerm*>& map1, std::map<std::pair<int, int>, ObjectiveTerm*>& map2);
    ObjectiveTerm*   setToFalse(int var);
    bool             filter(int min, int max);
    bool             filter(std::set<int> set);
    ObjectiveTerm*   simplify(const std::vector<char>& v);
    double           evaluate(const std::vector<bool>& values);

  private:
    double 	     first;
};
#endif
