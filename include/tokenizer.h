/*******************************************************************************
 * AUTHOR      : Thuan Tang
 * ID          : 00991588
 * CLASS       : CS008
 * HEADER      : sql_tokenizer
 * NAMESPACE   : tok
 * DESCRIPTION : This header defines the tokenizer for SQL to extract tokens
 *      from a char string. The tokens generated have specific SQL states
 *      ID, which is used by the SQLParser class.
 *
 *      Specialized for command line arguments.
 ******************************************************************************/
#ifndef TOKENIZER_H
#define TOKENIZER_H

#include <cassert>          // assertions
#include <iostream>         // stream objects
#include <string>           // string objects
#include "state_machine.h"  // state_machine functions
#include "token.h"          // Token class

namespace tok {

enum { MAX_BUFFER = 1024 };

class Tokenizer {
public:
    // CONSTRUCTORS
    Tokenizer(std::size_t max_buf = MAX_BUFFER);
    Tokenizer(char str[], std::size_t max_buf = MAX_BUFFER);
    Tokenizer(const char str[], std::size_t max_buf = MAX_BUFFER);

    ~Tokenizer();

    // ACCESSORS
    bool done() const;               // true: there are no more tokens
    bool more() const;               // true: there are more tokens
    explicit operator bool() const;  // boolean conversion for extractor

    // MUTATORS
    void set_string(char str[]);        // set a new string as the input string
    void set_string(const char str[]);  // set a new string as the input string

    // FRIENDS
    friend Tokenizer& operator>>(Tokenizer& s, Token& t);

private:
    static int _table[state_machine::MAX_ROWS][state_machine::MAX_COLUMNS];
    static bool _need_init;  // check if _table is initialized

    char* _buffer;         // input string
    std::size_t _max_buf;  // max buffer size
    int _buffer_size;      // input string size
    int _pos;              // current position in the string

    void make_table(int _table[][state_machine::MAX_COLUMNS]);
    bool get_token(int start_state, std::string& token);
};

}  // namespace tok

#endif  // TOKENIZER_H