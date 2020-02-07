//
//  humanNums.cpp
//  myKconf
//
//  Created by david on 15/10/14.
//  Copyright (c) 2014 david. All rights reserved.
//

#include "humanNums.hpp"

std::string showHuman(double n) {
    std::ostringstream ost;
    ost.setf(std::ios_base::fixed);
    if (n < 1E3)
        ost << n;
        else
        if (n < 1E6)
            //ost << std::setw(3) << (n/1000) << "k";
            ost << (n/1000) << "k";
        else {
            //std::cout.precision(2);
            //ost << (n/1000000.0) << "M";
            //std::cout.unsetf ( std::ios::floatfield );
            //ost <<  std::setprecision(2) << std::setw(6) << (n/1E6) << "M";
            ost <<  std::setprecision(2) << (n/1E6) << "M";
        }
        return ost.str();
}

std::string showHuman(int n) {
    std::ostringstream ost;
    ost.setf(std::ios_base::fixed);
    if (n < 1E3)
        ost << n;
    else
        if (n < 1E6)
            //ost << std::setw(3) << (n/1000) << "k";
            ost << (n/1000) << "k";
        else {
            std::cout.precision(2);
            //ost << (n/1000000.0) << "M";
            //std::cout.unsetf ( std::ios::floatfield );
            //ost  << std::setprecision(2) << std::setw(6) << (n/1E6) << "M";
            ost  << std::setprecision(2)  << (n/1E6) << "M";
        }
    return ost.str();
}
