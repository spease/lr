#include "Grammar.hpp"
#include "global.hpp"

/********************----- CLASS: Grammar -----********************/
Grammar::Grammar()
{
  m_analysisFlags = AnalysisFlags::F_DEFAULT;
}

bool Grammar::isContextFree() const
{
  return (enum_value(m_analysisFlags) & enum_value(AnalysisFlags::F_CONTEXTFREE));
}

void Grammar::add(Production &&i_production)
{
  m_productions.push_back(std::forward<Production>(i_production));

  if(m_productions.back().left().count() > 1)
  {
    m_analysisFlags = static_cast<AnalysisFlags>(enum_value(m_analysisFlags) & (~enum_value(AnalysisFlags::F_CONTEXTFREE)));
  }
}

size_t Grammar::productionCount() const
{
  return m_productions.size();
}

Symbol const &Grammar::startSymbol() const
{
  return m_productions.at(0).left()[0];
}

std::string Grammar::toString() const
{
  std::string returnString;

  for(ProductionVector::const_iterator pit=m_productions.begin(); pit!=m_productions.end(); ++pit)
  {
    returnString += pit->toString();
    returnString += "\n";
  }

  return returnString;
}

Grammar &Grammar::operator |=(Production &&i_production)
{
  this->add(std::forward<Production>(i_production));

  return (*this);
}

Grammar &Grammar::operator |=(Symbol &&i_symbol)
{
  this->add(Production(i_symbol));

  return (*this);
}

Production const &Grammar::operator [](size_t const i_ruleIndex) const
{
  return m_productions.at(i_ruleIndex);
}
