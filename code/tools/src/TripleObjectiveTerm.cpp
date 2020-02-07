#include "ZeroObjectiveTerm.hpp"
#include "SingleObjectiveTerm.hpp"
#include "DoubleObjectiveTerm.hpp"
#include "TripleObjectiveTerm.hpp"

                   TripleObjectiveTerm::TripleObjectiveTerm(double xCoefficient, int factor1, int factor2, int factor3) : 
                      ObjectiveTerm(xCoefficient),  first(factor1), second(factor2), third(factor3) {}; 
  ObjectiveTerm*   TripleObjectiveTerm::clone() {
			return new TripleObjectiveTerm(getCoefficient(), first, second, third);
                   };
  void             TripleObjectiveTerm::translate(std::vector<int>& tr) { first = tr[first]; second = tr[second]; third = tr[third];}
  std::vector<int> TripleObjectiveTerm::getFactors()              { 
                     std::vector<int> res; 
                     res.push_back(first); 
                     res.push_back(second); 
                     res.push_back(third); 
                     return res; 
                   };
  ObjectiveTerm*   TripleObjectiveTerm::setToTrue(int var)        {
                     if (var == first)  return new DoubleObjectiveTerm(getCoefficient(),  second, third);
                     if (var == second) return new DoubleObjectiveTerm(getCoefficient(),  first,  third);
                     if (var == third)  return new DoubleObjectiveTerm(getCoefficient(),  first,  second);
                     else return clone();
                   };

  ObjectiveTerm*   TripleObjectiveTerm::auxiliary(int one, int two, std::map<std::pair<int, int>, ObjectiveTerm*>& map) {
                    std::pair<int, int> p(one, two);
                        if (map.find(p) != map.end()) {
                           double other = map[p]->getCoefficient();
                           map[p]->setCoefficient(other+getCoefficient());
                           return NULL;
                        }
                        map[p] =  new DoubleObjectiveTerm(getCoefficient(),  one, two);
                        return map[p];
                   }
                    
  ObjectiveTerm*   TripleObjectiveTerm::setToTrue(int var, std::map<int, ObjectiveTerm*>& map1, std::map<std::pair<int, int>, ObjectiveTerm*>& map2)
                   { 
                     if (var == first)   return auxiliary(second, third, map2);
                     if (var == second)  return auxiliary(first, third, map2);
                     if (var == third)   return auxiliary(first, second, map2);
                     else return clone(); 
                   };
  
  ObjectiveTerm*   TripleObjectiveTerm::setToFalse(int var)       { 
                   if (var == first || var == second || var == third) return NULL; 
                   else return clone(); 
                   };
  bool             TripleObjectiveTerm::filter(int min, int max)  { return (first  >= min && first  <= max && 
                                                                            second >= min && second <= max &&
                                                                            third  >= min && third  <= max);  
                   }
  bool             TripleObjectiveTerm::filter(std::set<int> set) { return ((set.find(first)  != set.end()) && 
                                                                             set.find(second) != set.end() &&
                                                                             set.find(third)  != set.end()); 
                   }
   ObjectiveTerm*   TripleObjectiveTerm::simplify(const std::vector<char>& v) {
                        if (v[first] == 'f' || v[second] == 'f' || v[third] == 'f') return NULL;

                        if (v[first] == 't') {
			   if (v[second] == 't') {
				if (v[third] == 't') return new ZeroObjectiveTerm(getCoefficient());
                                return new SingleObjectiveTerm(getCoefficient(), third);
			   }
			   if (v[third] == 't') return new SingleObjectiveTerm(getCoefficient(), second);
			   return new DoubleObjectiveTerm(getCoefficient(), second, third);
                        }
			// v[first] is NOT 't'
			if (v[second] == 't') {
			   if (v[third] =='t') return new SingleObjectiveTerm(getCoefficient(), first);
			   return new DoubleObjectiveTerm(getCoefficient(), first, third);
			}
			// v[first] is NOT 't' and v[second] is not 't'
			if (v[third] == 't') return new DoubleObjectiveTerm(getCoefficient(), first, second);
			
			return clone();
  }

double           TripleObjectiveTerm::evaluate(const std::vector<bool>& values) {
    if (values[first] && values[second] && values[third]) return getCoefficient();
    return 0;
}
