//
//  kconf_id.h
//  OtherKconf
//
//  Created by david on 13/09/13.
//  Copyright (c) 2013 david. All rights reserved.
//

#ifndef kconf_id_h
#define kconf_id_h

#include <string.h>

enum symbol_type {
        S_UNKNOWN, S_BOOLEAN, S_TRISTATE, S_INT, S_HEX, S_STRING, S_OTHER
};

#define TF_COMMAND      0x0001
#define TF_PARAM        0x0002
#define TF_OPTION       0x0004

typedef kconf::Parser::token token;
typedef kconf::Parser::token_type token_type;

typedef  struct kconf_id {
   	int name;
	token_type token;
	unsigned int flags;
	enum symbol_type stype;
      std::string type2string() {
          switch (stype) {
              case S_UNKNOWN  : return "unknown";
              case S_BOOLEAN  : return "boolean";
              case S_TRISTATE : return "tristate";
              case S_INT      : return "int";
              case S_HEX      : return "hex";
              case S_STRING   : return "string";
              case S_OTHER    : return "other";
                                }
                }
} kconf_id; // kconf_id

#endif
