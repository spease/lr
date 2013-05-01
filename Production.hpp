#ifndef _PRODUCTION_HPP_
#define _PRODUCTION_HPP_

#include "Symbol.hpp"
#include "SymbolList.hpp"

#include <vector>

/********************----- CLASS: Production -----********************/
class Production
{
public:
  Production(SymbolList &&i_left, SymbolList &&i_right=SymbolList());
  Production(SymbolList const &i_left, SymbolList const &i_right=SymbolList());

  SymbolList const &left() const;
  SymbolList const &right() const;

  std::string toString() const;
private:
  SymbolList m_left;
  SymbolList m_right;
};
/**************************************************/

typedef std::vector<Production> ProductionVector;

/********************----- Operators -----********************/
Production operator>>=(SymbolList &&i_left, SymbolList &&i_right);
/**************************************************/

#endif /* _PRODUCTION_HPP_ */
