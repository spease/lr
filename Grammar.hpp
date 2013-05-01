#ifndef _GRAMMAR_HPP_
#define _GRAMMAR_HPP_

#include "Production.hpp"

#include <vector>

/********************----- CLASS: Grammar -----********************/
class Grammar
{
public:
  Grammar();
  virtual ~Grammar(){}

  bool isContextFree() const;

  void add(Production &&i_production);
  size_t productionCount() const;
  Symbol const &startSymbol() const;

  std::string toString() const;
  Grammar &operator |= (Symbol &&i_symbol);
  Grammar &operator |= (Production &&i_production);
  Production const &operator[](size_t const i_ruleIndex) const;
private:
  Grammar(Grammar const &)=delete;
  Grammar(Grammar &&)=delete;
  Grammar &operator =(Grammar const &)=delete;
  Grammar &operator =(Grammar &&)=delete;
  enum class AnalysisFlags
  {
    F_CONTEXTFREE=(1<<0),
    F_DEFAULT=F_CONTEXTFREE,
  };

  AnalysisFlags m_analysisFlags;

  ProductionVector m_productions;
};
/**************************************************/

#endif /* _GRAMMAR_HPP_ */
