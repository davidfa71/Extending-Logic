#ifndef _OBJECTIVETERM_HPP_
#define _OBJECTIVETERM_HPP_

#include <vector>
#include <set>
#include <iostream>
#include <map>

class ObjectiveTerm {
	public:
                ObjectiveTerm(double xCoefficient);
        virtual ObjectiveTerm*   clone()                         = 0;
        virtual void             translate(std::vector<int>& tr) = 0;
        virtual std::vector<int> getFactors()                    = 0;
        virtual ObjectiveTerm*   setToTrue(int var)              = 0;
	virtual ObjectiveTerm*   setToTrue(int var, std::map<int, ObjectiveTerm*>& map1, std::map<std::pair<int, int>, ObjectiveTerm*>& map2) = 0;
        virtual ObjectiveTerm*   setToFalse(int var)             = 0;
        virtual bool             filter(int min, int max)        = 0;
        virtual bool             filter(std::set<int> set)       = 0;
                double           getCoefficient(); 
                void             setCoefficient(double xCoefficient);
        virtual bool             isZeroObjectiveTerm();
        virtual ObjectiveTerm*   simplify(const std::vector<char>& v) = 0;
        virtual double           evaluate(const std::vector<bool>& values) = 0;

        virtual ~ObjectiveTerm();
        private:
	
		double coefficient;
};
#endif
