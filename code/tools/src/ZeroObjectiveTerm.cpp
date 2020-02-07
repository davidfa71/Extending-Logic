#include "ZeroObjectiveTerm.hpp"

                   ZeroObjectiveTerm::ZeroObjectiveTerm(double xCoefficient) : ObjectiveTerm(xCoefficient) { }; 
  ObjectiveTerm*   ZeroObjectiveTerm::clone() {
			return new ZeroObjectiveTerm(getCoefficient());
                   }

  bool             ZeroObjectiveTerm::isZeroObjectiveTerm() { return true; }

  void             ZeroObjectiveTerm::translate(std::vector<int>& tr) { throw std::logic_error("Error, calling ZeroObjectiveTerm::translate");  };
  std::vector<int> ZeroObjectiveTerm::getFactors()                    { throw std::logic_error("Error, calling ZeroObjectiveTerm::getFactors"); };
  ObjectiveTerm*   ZeroObjectiveTerm::setToTrue(int var)              { throw std::logic_error("Error, calling ZeroObjectiveTerm::setToTrue");  };
  ObjectiveTerm*   ZeroObjectiveTerm::setToTrue(int var, std::map<int, ObjectiveTerm*>& map1, std::map<std::pair<int, int>, ObjectiveTerm*>& map2) 
                                                                      { throw std::logic_error("Error, calling ZeroObjectiveTerm::setToTrue");  };
  ObjectiveTerm*   ZeroObjectiveTerm::setToFalse(int var)             { throw std::logic_error("Error, calling ZeroObjectiveTerm::setToFalse"); };
  bool             ZeroObjectiveTerm::filter(int min, int max)        { throw std::logic_error("Error, calling ZeroObjectiveTerm::filter1");    };
  bool             ZeroObjectiveTerm::filter(std::set<int> set)       { throw std::logic_error("Error, calling ZeroObjectiveTerm::filter2");    };
  ObjectiveTerm*   ZeroObjectiveTerm::simplify(const std::vector<char>& v) { throw std::logic_error("Error, calling ZeroObjectiveTerm::simplify");    };

double              ZeroObjectiveTerm::evaluate(const std::vector<bool>& values) {
    throw std::logic_error("Error, calling ZeroObjectiveTerm::evaluate");
}

