//
//  eval.h
//  myKconf
//
//  Created by david on 15/10/14.
//  Copyright (c) 2014 david. All rights reserved.
//

#ifndef __myKconf__eval__
#define __myKconf__eval__

#include <stdio.h>

#include "synExp.hpp"
#include "symbolTable.hpp"
#include "configInfo.hpp"

synExp *eval(synExp* exp);

#endif /* defined(__myKconf__eval__) */
