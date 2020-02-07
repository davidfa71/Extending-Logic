// $Id: driver.h 17 2007-08-19 18:51:39Z tb $ 	
/** \file driver.h Declaration of the kconf::Driver class. */

#ifndef KCONF_SYNEXPDRIVER_H
#define KCONF_SYNEXPDRIVER_H

#include <string>
#include <vector>
#include <map>
#include <iomanip>
#include <tuple>

#include "synExp.hpp"

/** The kconf namespace is used to encapsulate the three parser classes
 * kconf::Parser, kconf::Scanner and kconf::Driver */
namespace kconf {

/** The Driver class brings together all components. It creates an instance of
 * the Parser and Scanner classes and connects them. Then the input stream is
 * fed into the scanner object and the parser gets it's token
 * sequence. Furthermore the driver object is available in the grammar rules as
 * a parameter. Therefore the driver class contains a reference to the
 * structure into which the parsed data is saved. */
class synExpDriver
{

private:

    std::vector<synExp*> vexp;
    
  public:
    
   
    synExpDriver();
    std::vector<synExp*> getConstraints();
    /// enable debug output in the flex scanner
    bool trace_scanning;

    /// enable debug output in the bison parser
    bool trace_parsing;

    /// stream name (file or input stream) used for error messages.
    std::string streamname;
      // Process default statements
    
    /** Invoke the scanner and parser on a file. Use parse_stream with a
     * std::ifstream if detection of file reading errors is required.
     * @param filename	input file name
     * @return		true if successfully parsed
     */
    bool parse_file(const std::string filename);
    bool parse_string(const std::string s);

    // To demonstrate pure handling of parse errors, instead of
    // simply dumping them on the standard error output, we will pass
    // them to the driver using the following two member functions.

    /** Error handling with associated line number. This can be modified to
     * output the error e.g. to a dialog box. */
    void error(const class location& l, const std::string& m);

    /** General error handling. This can be modified to output the error
     * e.g. to a dialog box. */
    void error(const std::string& m);

    /** Pointer to the current lexer instance, this is used to connect the
     * parser to the scanner. It is used in the yylex macro. */
    class synExpScanner* lexer;
    
    void addExp(synExp* s);
   };

} // namespace kconf

#endif // KCONF_SYNEXPDRIVER_H
