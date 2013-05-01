#include "Production.hpp"

/********************----- CLASS: Production -----********************/
Production::Production(SymbolList &&i_left, SymbolList &&i_right)
:m_left(std::forward<SymbolList>(i_left)), m_right(std::forward<SymbolList>(i_right))
{
}

Production::Production(SymbolList const &i_left, SymbolList const &i_right)
:m_left(i_left), m_right(i_right)
{
}

SymbolList const &Production::left() const
{
  return m_left;
}

SymbolList const &Production::right() const
{
  return m_right;
}

std::string Production::toString() const
{
  std::string returnString;

  for(size_t i=0; i<m_left.count(); ++i)
  {
    returnString += m_left[i].toString();
    returnString += " ";
  }

  returnString += "=>";

  for(size_t i=0; i<m_right.count(); ++i)
  {
    returnString += " ";
    returnString += m_right[i].toString();
  }

  return returnString;
}
/**************************************************/

/********************----- Operators -----********************/
Production operator>>=(SymbolList &&i_left, SymbolList &&i_right)
{
  return Production(std::forward<SymbolList>(i_left), std::forward<SymbolList>(i_right));
}
/**************************************************/
