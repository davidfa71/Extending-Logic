// $Id: synExpdriver.cc 39 2008-08-03 10:07:15Z tb $
/** \file synExpdriver.cc Implementation of the kconf::synExpDriver class. */

#include <fstream>
#include <sstream>

#include "synExpDriver.hpp"
#include "synExpParser.hpp"
#include "synExpScanner.hpp"

namespace kconf {
    
    synExpDriver::synExpDriver()
    : trace_scanning(false),
    trace_parsing(false)
    {
        
    }
    
    std::vector<synExp*> synExpDriver::getConstraints() { return vexp; }

    bool synExpDriver::parse_string(const std::string s) {
        
        std::istringstream in(s);
        synExpScanner scanner("string", &in);
        scanner.set_debug(trace_scanning);
        this->lexer = &scanner;
        
        synExpParser parser(*this);
        
        parser.set_debug_level(trace_parsing);
        if (parser.parse() != 0) {
            exit(-1);
        };
        return 0;
    }

    
    bool synExpDriver::parse_file(const std::string filename) {
        
        std::ifstream in(filename.c_str());
        
        if (!in.good()) {
            std::cout << "Could not open file: " << filename << std::endl;
            exit(-1);
        }
        
        
        synExpScanner scanner(filename, &in);
        scanner.set_debug(trace_scanning);
        this->lexer = &scanner;
        
        synExpParser parser(*this);
        
        parser.set_debug_level(trace_parsing);
        if (parser.parse() != 0) {
            exit(-1);
        };
        return 0;
    }
    
    void synExpDriver::error(const class location& l,
                       const std::string& m)
    {
        std::cerr << l << ": " << m << std::endl;
    }
    
    void synExpDriver::error(const std::string& m)
    {
        std::cerr << m << std::endl;
    }
    
    void synExpDriver::addExp(synExp* s) { vexp.push_back(s); }
 } // namespace kconf



