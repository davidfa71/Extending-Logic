#include "ZeroObjectiveTerm.hpp"
#include "SingleObjectiveTerm.hpp"

                   SingleObjectiveTerm::SingleObjectiveTerm(double xCoefficient, int factor)  : ObjectiveTerm(xCoefficient), first(factor) { } 
  ObjectiveTerm*   SingleObjectiveTerm::clone() {
			return new SingleObjectiveTerm(getCoefficient(), first);
                   }
  void             SingleObjectiveTerm::translate(std::vector<int>& tr) { first = tr[first]; }
  std::vector<int> SingleObjectiveTerm::getFactors()              { std::vector<int> res; res.push_back(first); return res; };
  ObjectiveTerm*   SingleObjectiveTerm::setToTrue(int var)        { if (var == first) return new ZeroObjectiveTerm(getCoefficient()); else return clone(); }
  ObjectiveTerm*   SingleObjectiveTerm::setToTrue(int var, std::map<int, ObjectiveTerm*>& map1, std::map<std::pair<int, int>, ObjectiveTerm*>& map2)
                   { 
                      if (var == first) 
			return new ZeroObjectiveTerm(getCoefficient()); 

                      else return clone(); 
                   };
  ObjectiveTerm*   SingleObjectiveTerm::setToFalse(int var)       { if (var == first) return NULL;                                    else return clone(); };
  bool             SingleObjectiveTerm::filter(int min, int max)  { return (first >= min && first <= max); }
  bool             SingleObjectiveTerm::filter(std::set<int> set) { return set.find(first) != set.end();   };
  ObjectiveTerm*   SingleObjectiveTerm::simplify(const std::vector<char>& v) {
			if (v[first] == 't') return new ZeroObjectiveTerm(getCoefficient());
			if (v[first] == 'f') return NULL;
			return clone();
		   }

double           SingleObjectiveTerm::evaluate(const std::vector<bool>& values) {
    if (values[first]) return getCoefficient();
    return 0;
}


