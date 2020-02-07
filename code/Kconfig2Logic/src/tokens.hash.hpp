//
//  tokens.hash.hpp
//  OtherKconf
//
//  Created by david on 17/09/13.
//  Copyright (c) 2013 david. All rights reserved.
//

#ifndef OtherKconf_tokens_hash_hpp
#define OtherKconf_tokens_hash_hpp

#include "kconf_id.hpp"

class Perfect_Hash {
    public :
    static kconf_id * kconf_id_lookup (const char *str, size_t len);
};


#endif
