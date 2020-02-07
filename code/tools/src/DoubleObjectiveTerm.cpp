#include "SingleObjectiveTerm.hpp"
#include "DoubleObjectiveTerm.hpp"

                   DoubleObjectiveTerm::DoubleObjectiveTerm(double xCoefficient, int factor1, int factor2) : 
                    ObjectiveTerm(xCoefficient), first(factor1), second(factor2) {}; 
  ObjectiveTerm*   DoubleObjectiveTerm::clone() {
			return new DoubleObjectiveTerm(getCoefficient(), first, second);
                   };
  void             DoubleObjectiveTerm::translate(std::vector<int>& tr) { first = tr[first]; second = tr[second];}
  std::vector<int> DoubleObjectiveTerm::getFactors()                    { std::vector<int> res; res.push_back(first); res.push_back(second); return res; };
  ObjectiveTerm*   DoubleObjectiveTerm::setToTrue(int var)              {
                     if (var == first)  return new SingleObjectiveTerm(getCoefficient(), second);
                     if (var == second) return new SingleObjectiveTerm(getCoefficient(), first);
                     else return clone();
                   };
  ObjectiveTerm*   DoubleObjectiveTerm::setToTrue(int var, std::map<int, ObjectiveTerm*>& map1, std::map<std::pair<int, int>, ObjectiveTerm*>& map2)
                   { 
                     if (var == first) { 
                        if (map1.find(second) != map1.end()) {
			   double other = map1[second]->getCoefficient();
                           map1[second]->setCoefficient(other+getCoefficient());
                           return NULL;
                        }
                        
                        map1[second] = new SingleObjectiveTerm(getCoefficient(), second); 
                        return map1[second];
                     } 
                     if (var == second) {
                         if (map1.find(first) != map1.end()) {
                           double other = map1[first]->getCoefficient();
                           map1[first]->setCoefficient(other+getCoefficient());
                           return NULL;
                        }
			map1[first] = new SingleObjectiveTerm(getCoefficient(), first);
                        return map1[first];
		     }
                     else return clone(); 
                   };
  ObjectiveTerm*   DoubleObjectiveTerm::setToFalse(int var)       { 
                   if (var == first || var == second) return NULL; 
                   else return clone(); 
                   };
  bool             DoubleObjectiveTerm::filter(int min, int max)  { return (first >= min && first <= max && second >= min && second <= max);  }
  bool             DoubleObjectiveTerm::filter(std::set<int> set) { return ((set.find(first) != set.end()) && set.find(second) != set.end()); }
  ObjectiveTerm*   DoubleObjectiveTerm::simplify(const std::vector<char>& v) {
			if (v[first] == 'f' || v[second] == 'f') return NULL;
			if (v[first] == 't') {
			   if (v[second] == 't') return new ZeroObjectiveTerm(getCoefficient());
			   return new SingleObjectiveTerm(getCoefficient(), second);
                        }
                        if (v[second] == 't') return new SingleObjectiveTerm(getCoefficient(), first);
			return clone();

  }
double           DoubleObjectiveTerm::evaluate(const std::vector<bool>& values) {
    if (values[first] && values[second]) return getCoefficient();
    return 0;
}
