//
//  mytime.h
//  BDDSampler
//
//  Created by david on 15/07/13.
//  Copyright (c) 2013 David. All rights reserved.
//

#ifndef BDDSampler_mytime_h
#define BDDSampler_mytime_h

#include <string>

#ifdef __MACH__
#include <mach/clock.h>
#include <mach/mach.h>
#endif

#ifndef _WINDOWS_
#include <sys/time.h>
#include <sys/resource.h>
#else
#include <Windows.h>
#include <ctime>
#include <iostream>
#endif


std::string showtime(long ttime);
double get_cpu_time();
std::string get_timestamp();


#endif
