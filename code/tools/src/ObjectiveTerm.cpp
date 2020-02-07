#include "ObjectiveTerm.hpp"
       ObjectiveTerm::ObjectiveTerm(double xCoefficient) : coefficient(xCoefficient) {};
double ObjectiveTerm::getCoefficient() { return coefficient; }
bool   ObjectiveTerm::isZeroObjectiveTerm() { return false; }
void   ObjectiveTerm::setCoefficient(double xCoefficient) { coefficient = xCoefficient; }
ObjectiveTerm::~ObjectiveTerm() {};
