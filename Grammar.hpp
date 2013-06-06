#ifndef _GRAMMAR_HPP_
#define _GRAMMAR_HPP_

#include "Symbol.hpp"
#include "Production.hpp"

#include <vector>

/********************----- CLASS: Grammar -----********************/
class Grammar
{
public:
  Grammar();
  virtual ~Grammar(){}

  void add(Production &&i_production);

  SymbolSet::const_iterator alphabetBegin() const;
  SymbolSet::const_iterator alphabetEnd() const;
  bool isContextFree() const;
  ProductionConstPtrVector productionPointers(SymbolList const &i_left) const;
  size_t productionCount() const;
  Symbol const &startSymbol() const;

  SymbolSet first(Symbol const &i_symbol) const;
  SymbolSet firstList(SymbolList const &i_symbolList) const;

  std::string toString() const;
  Grammar &operator |= (Symbol &&i_symbol);
  Grammar &operator |= (Production &&i_production);
  Production const &operator[](size_t const i_ruleIndex) const;

  enum class AnalysisFlags
  {
    CONTEXTFREE=(1<<0),
    DEFAULT=CONTEXTFREE,
  };
  enum class CacheFlags
  {
    BUILTFIRST=(1<<0),
    BUILTFOLLOW=(1<<1),
    DEFAULT=0,
  };
protected:
  SymbolMap buildFirst() const;
  SymbolMap buildFollow() const;

  static SymbolSet first(Symbol const &i_symbol, SymbolMap const &i_firstMap);
  static SymbolSet firstList(SymbolList const &i_symbolList, SymbolMap const &i_firstMap);

private:
  Grammar(Grammar const &)=delete;
  Grammar(Grammar &&)=delete;
  Grammar &operator =(Grammar const &)=delete;
  Grammar &operator =(Grammar &&)=delete;

  SymbolSet m_alphabet;
  AnalysisFlags m_analysisFlags;

  mutable CacheFlags m_cacheFlags;
  mutable SymbolMap m_cacheFirst;
  mutable SymbolMap m_cacheFollow;

  ProductionVector m_productions;
};
/**************************************************/

ENUM_GENERATE_HELPERS(Grammar::AnalysisFlags)
ENUM_GENERATE_HELPERS(Grammar::CacheFlags)

#endif /* _GRAMMAR_HPP_ */
