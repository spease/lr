#ifndef _LRPARSER_HPP_
#define _LRPARSER_HPP_

#include "Grammar.hpp"
#include "Lex.hpp"
#include "LRTable.hpp"

class Grammar;
class Production;

/********************----- CLASS: LRParser -----********************/
class LRParser
{
public:
  LRParser(LRTable::Type const i_type, size_t const i_k, Grammar const &i_grammar);
  virtual ~LRParser(){}

  bool parse(Lex &i_lex);

private:
  LRParser(LRParser const &)=delete;
  LRParser(LRParser &&)=delete;
  LRParser &operator =(LRParser const &)=delete;
  LRParser &operator =(LRParser &&)=delete;

  LRStateStack m_stackState;
  SymbolStack m_stackSymbol;
  LRTable m_table;
  LRTable::Type m_type;
};
/**************************************************/

#endif /* _LRPARSER_HPP_ */
