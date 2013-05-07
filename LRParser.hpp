#ifndef _LRPARSER_HPP_
#define _LRPARSER_HPP_

#include "LRItem.hpp"
#include "Symbol.hpp"
#include "SymbolList.hpp"

class Grammar;
class Production;

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
  static SymbolMap buildFollow(SymbolMap const &i_first, Grammar const &i_grammar);
  static LRItemSet buildItems(SymbolMap const &i_firstMap, Grammar const &i_grammar);

  static SymbolSet first(Symbol const &i_symbol, SymbolMap const &i_symbolMap);
  static SymbolSet firstList(SymbolList const &i_symbolList, SymbolMap const &i_symbolMap);
  static LRItemSet closure(LRItemSet const &i_item, SymbolMap const &i_first, Grammar const &i_grammar);
  static LRItemSet paths(LRItemSet const &i_itemSet, Symbol const &i_symbol);

private:
  LRParser(LRParser const &)=delete;
  LRParser(LRParser &&)=delete;
  LRParser &operator =(LRParser const &)=delete;
  LRParser &operator =(LRParser &&)=delete;

  SymbolSet m_closureCache;
  Grammar const *m_grammarPointer;
  SymbolMap m_first;
  SymbolMap m_follow;
  LRItemSet m_items;
};
/**************************************************/

bool operator ==(SymbolMap const &i_A, SymbolMap const &i_B);
bool operator !=(SymbolMap const &i_A, SymbolMap const &i_B);

#endif /* _LRPARSER_HPP_ */
