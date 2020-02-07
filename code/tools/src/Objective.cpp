#include "Objective.hpp"
Objective::Objective() { fun.resize(0);};
Objective::Objective(const Objective& obj) {
    if (&obj == this) return;
    //throw std::logic_error("What are you doing copy constructing an Objective object?");
    independent = obj.getIndependent();
    min         = obj.getMin();
    max         = obj.getMax();
    precision  = obj.getPrecision();
    for(auto pobjterm : fun)
        delete pobjterm;
    fun.clear();
    for(auto pobjterm : obj.fun)
        add(pobjterm->clone());
    fun.resize(fun.size());
}

Objective& Objective::operator=(const Objective& obj) {
    if (&obj == this) return *this;
    independent = obj.getIndependent();
    min         = obj.getMin();
    max         = obj.getMax();
    precision   = obj.getPrecision();
    for(auto pobjterm : fun)
        delete pobjterm;
    fun.clear();
    for(auto pobjterm : obj.fun)
        add(pobjterm->clone());
    fun.resize(fun.size());
    return *this;
}  

void	   Objective::setMin(int xmin)             { min = xmin;             }
void	   Objective::setMax(int xmax)             { max = xmax;             }
void       Objective::setPrecision(int xPrecision) { precision = xPrecision; }
int	       Objective::getMin() const               { return min;             }
int 	   Objective::getMax() const               { return max;             }
int        Objective::getPrecision() const         { return precision;       }

Objective  Objective::setToTrue(int var) {
    Objective res;
    res.min       = min;
    res.max       = max;
    res.precision = precision;
    res.setIndependent(independent);
    for(auto x : fun) {
        ObjectiveTerm* o = x->setToTrue(var);
        if (o != NULL) {
            if (o->isZeroObjectiveTerm()) {
                res.addToIndependent(o->getCoefficient());
                delete o;
            }
            else
                res.add(o);
        }
    }
    res.fun.resize(res.fun.size());
    return res;
}

Objective  Objective::setToFalse(int var) {
    Objective res;
    res.min       = min;
    res.max       = max;
    res.precision = precision;
    res.setIndependent(independent);
    for(auto x : fun) {
        ObjectiveTerm* o = x->setToFalse(var);
        if (o != NULL)
            res.add(o);
    }
    res.fun.resize(res.fun.size());
    return res;
}

void    Objective::translate(std::vector<int>& tr) {
    for(auto objterm : fun)
        objterm->translate(tr);
}

double Objective::getIndependent() const   { return independent; }
void   Objective::setIndependent(double d) { independent = d;    };

void Objective::add(double d, std::vector<int> v) { 
    if (v.empty()) {
        independent = d;
        return;
    }
    if (v.size() == 1) {
        fun.push_back(new SingleObjectiveTerm(d, v[0]));
        return;
    }
    if (v.size() == 2) {
        fun.push_back(new DoubleObjectiveTerm(d, v[0], v[1]));
        return;
    }
    
    if (v.size() == 3) {
        fun.push_back(new TripleObjectiveTerm(d, v[0], v[1], v[2]));
        return;
    }
    std::ostringstream ost;
    ost << "Error, add an objective term of size " << v.size() << std::endl;
    throw std::logic_error(ost.str());
}

int  Objective::size() { return fun.size(); };

std::vector<int> Objective::getFactors(int index) { return fun[index]->getFactors(); }

std::ostream& operator<<(std::ostream& os, const Objective& obj) {
    os << " min " << obj.min << " max " << obj.max << "||";
    os << obj.getIndependent() << " ";
    for(auto p : obj.fun) {
        if (p->getCoefficient() >= 0)
            os << "+" << p->getCoefficient() << " ";
        else
            os << " " << p->getCoefficient() << " ";
        for(auto x : p->getFactors())
            os << " * " << x;
    }
    return os;
}

// Filters an objective function to the interesting part for a group of variables
// does not carry over the independent coefficient if there is one
Objective Objective::filter(std::set<int> variables) {
    Objective res;
    for(auto pobjterm : fun)
        if (pobjterm->filter(variables))
            res.add(pobjterm->clone());
    
    res.fun.resize(res.fun.size());
    return res;
}

Objective       Objective::filter(int minVar, int maxVar) {
    Objective res;
    for(auto pobjterm : fun)
        if (pobjterm->filter(minVar, maxVar))
            res.add(pobjterm->clone());
    
    res.fun.resize(res.fun.size());
    return res;
}

void    Objective::addToIndependent(double d) { independent += d;}
Objective::~Objective() {
    for(auto x : fun)
        delete x;
}

void   Objective::add(ObjectiveTerm* o) {
    fun.push_back(o);
};

Objective Objective::setTo(int var, bool value) {
    if (value)
        return setToTrue(var);
    else
        return setToFalse(var);
}

Objective    Objective::simplify(const std::vector<char>& v) {
    Objective res;
    res.setIndependent(independent);
    res.max = max;
    res.min = min;
    res.precision = precision;
    for(auto pot : fun) {
        ObjectiveTerm *r = pot->simplify(v);
        if (r != NULL)  {
            if (r->isZeroObjectiveTerm()) {
                res.addToIndependent(r->getCoefficient());
                delete r;
            }
            else
                res.add(r);
        }
    }
    res.fun.resize(res.fun.size());
    return res;
}

double  Objective::evaluate(const std::vector<bool>& values) {
    double result = independent;
    for(auto p : fun)
        result += p->evaluate(values);
    
    return result;
}
