#include <iostream>
#include <string>
#include <vector>
#include <cassert>

#include "parser.h"
#include "reader.h"
#include "macros.h"

using std::string;
using std::vector;

Texp Parser::_char()
  {
    std::string s;
    assert (*_r == '\'');
    s += *_r++;
    while (not (*_r == '\'' && _r.prev() != '\\')) 
      {
        CHECK(_r.hasNext(), "backbone: reached end of file while parsing character");
        s += *_r++;
      }
    assert(*_r == '\'');
    s += *_r++;
    return Texp(s);
  }

Texp Parser::_string()
  {
    std::string s;
    assert (*_r == '\"');
    s += *_r++;
    while (not (*_r == '\"' && _r.prev() != '\\')) 
      {
        CHECK(_r.hasNext(), "reached end of file while parsing string");
        s += *_r++;
      }
    assert(*_r == '\"');
    s += *_r++;
    return Texp(s);
  }

Parser::Parser(std::string_view v): _r(v) {}

void Parser::whitespace() 
  { while (std::isspace(*_r)) _r++; }

Texp Parser::texp() 
  {
    whitespace();
    // std::cout << _r.pos() << ": " << *_r << " ";
    if (*_r == '(') return list();
    return atom();
  }

Texp Parser::atom() 
  {
    // std::cout << "parsing atom\n";
    assert(*_r != ')'); //TODO assert *_r isn't special
    if (*_r == '\'') return _char();
    if (*_r == '\"') return _string();
    return Texp(word());
  }

Texp Parser::list()
  {
    // std::cout << "parsing list\n";
    assert(*_r++ == '(');
    
    auto curr = Texp(word());
    while (*_r != ')')
      {
        CHECK(_r.hasNext(), "reached end of file when parsing list");
        curr.push(texp());
        whitespace();
      }
    
    assert(*_r++ == ')');

    return curr;
  }

std::string Parser::word()
  {
    std::string s;
    whitespace();
    CHECK(_r.hasNext(), "reached end of file when parsing word");
    while (_r.hasNext() && *_r != '(' && *_r != ')' && not std::isspace(*_r)) 
      {
        s += *_r++;
      }
    return s;
  }

char Parser::operator*() 
  { return *_r; }

Reader::iterator Parser::end() 
  { return _r.end(); }

const Reader::iterator Parser::curr() 
  { return _r.curr(); }

Texp Parser::file(std::string filename)
  {
    Texp result(filename);

    whitespace();
    while(_r.hasNext())
      {
        result.push(texp());
        whitespace();
      }

    return result;
  }