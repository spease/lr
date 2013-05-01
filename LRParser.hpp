#ifndef _LRPARSER_HPP_
#define _LRPARSER_HPP_

#include "Symbol.hpp"
#include "SymbolList.hpp"

#include <set>
#include <unordered_map>

class Grammar;
class Production;

typedef std::set<Symbol> SymbolSet;
typedef std::unordered_map<Symbol, SymbolSet> SymbolMap;

/********************----- CLASS: LRParser -----********************/
class LRParser
{
public:
  LRParser();
  virtual ~LRParser(){}

  SymbolSet first(Symbol const &i_symbol) const;
  SymbolSet follow(Symbol const &i_symbol) const;
  bool setGrammar(Grammar const * const i_grammarPointer);

  static SymbolMap buildFirst(Grammar const &i_grammar);
  //static SymbolMap buildFirst(Grammar const * const i_grammarPointer);
  //static SymbolList buildFirstNonterminal(Grammar const * const i_grammarPointer, Symbol const &i_symbol);
  //static SymbolList buildFirstProduction(Grammar const * const i_grammarPointer, Production const &p);
  static SymbolMap buildFollow(Grammar const * const i_grammarPointer);
private:
  LRParser(LRParser const &)=delete;
  LRParser(LRParser &&)=delete;
  LRParser &operator =(LRParser const &)=delete;
  LRParser &operator =(LRParser &&)=delete;

  Grammar const *m_grammarPointer;
  SymbolMap m_first;
  SymbolMap m_follow;
};
/**************************************************/

bool operator ==(SymbolMap const &i_A, SymbolMap const &i_B);
bool operator !=(SymbolMap const &i_A, SymbolMap const &i_B);

#endif /* _LRPARSER_HPP_ */
