#include "Grammar.hpp"
#include "global.hpp"

#include <iostream>

/********************----- CLASS: Grammar -----********************/
Grammar::Grammar()
:m_analysisFlags(AnalysisFlags::DEFAULT),m_cacheFlags(CacheFlags::DEFAULT)
{
}

bool Grammar::isContextFree() const
{
  return (enum_value(m_analysisFlags) & enum_value(AnalysisFlags::CONTEXTFREE));
}

void Grammar::add(Production &&i_production)
{
  m_productions.push_back(std::forward<Production>(i_production));

  Production const &p=m_productions.back();
  SymbolList const &left=p.left();
  SymbolList const &right=p.right();

  /***** Update alphabet *****/
  for(size_t i=0; i<left.count(); ++i)
  {
    m_alphabet.insert(left[i]);
  }

  for(size_t i=0; i<right.count(); ++i)
  {
    m_alphabet.insert(right[i]);
  }

  /***** Update flags *****/
  if(left.count() > 1)
  {
    m_analysisFlags &= ~(AnalysisFlags::CONTEXTFREE);
  }

  m_cacheFlags &= ~(CacheFlags::BUILTFIRST|CacheFlags::BUILTFOLLOW);
}

SymbolSet::const_iterator Grammar::alphabetBegin() const
{
  return m_alphabet.begin();
}

SymbolSet::const_iterator Grammar::alphabetEnd() const
{
  return m_alphabet.end();
}

SymbolMap Grammar::buildFirst() const
{
  Grammar const &g=(*this);
  if(!g.isContextFree())
  {
    throw std::logic_error("Tried to build first set for non-context-free grammar");
  }

  SymbolMap prevMap;
  SymbolMap currentMap;

  bool mapChanged = true;
  while(mapChanged)
  {
    for(size_t i=0; i<g.productionCount(); ++i)
    {
      Production const &production=g[i];
      Symbol const &leftSymbol=production.left()[0];
      SymbolList const &right=production.right();

      SymbolSet ss = Grammar::firstList(right, prevMap);

      /***** Add list *****/
      currentMap[leftSymbol].insert(ss.begin(), ss.end());
    }

#ifndef NDEBUG
    printSymbolMap("FIRST: prevMap", prevMap);
    printSymbolMap("FIRST: currentMap", currentMap);
#endif

    mapChanged = (prevMap != currentMap);
    if(mapChanged)
    {
      prevMap = std::move(currentMap);
    }
  }

  return prevMap;
}

SymbolMap Grammar::buildFollow() const
{
  Grammar const &g=(*this);
  if(!g.isContextFree())
  {
    throw std::logic_error("Tried to build follow set for non-context-free grammar");
  }

  SymbolMap prevMap;
  SymbolMap currentMap;

  SymbolSet startSet = {END()};
  currentMap[g.startSymbol()] = std::move(startSet);

  for(size_t i=0; i<g.productionCount(); ++i)
  {
    Production const &p=g[i];
    SymbolList const &right=p.right();
    SymbolSet firstSet;

    if(right.count() > 0)
    {
      firstSet = this->first(right[right.count()-1]);

      for(size_t x=right.count()-1; x-- > 0;)
      {
        Symbol const &rightSymbol = right[x];

        /***** Add FIRST(rightSymbol) onto FOLLOW(rightSymbol) *****/
        if(rightSymbol.isNonterminal())
        {
          currentMap[rightSymbol].insert(firstSet.begin(), firstSet.end());
        }

        /***** If no epsilon, reset firstSet *****/
        SymbolSet const &rightSet = this->first(rightSymbol);
        if(rightSet.find(EPS()) == rightSet.end())
        {
          firstSet.clear();
        }

        /***** Add FIRST(rightSymbol) onto firstSet *****/
        firstSet.insert(rightSet.begin(), rightSet.end());
      }
    }
  }

  while(prevMap != currentMap)
  {
#ifndef NDEBUG
    printSymbolMap("FOLLOW: prevMap", prevMap);
    printSymbolMap("FOLLOW: currentMap", currentMap);
#endif
    prevMap = currentMap;

    for(size_t i=0; i<g.productionCount(); ++i)
    {
      Production const &p=g[i];
      Symbol const &leftSymbol=p.left()[0];
      SymbolList const &right=p.right();

      for(size_t x=right.count(); x-- > 0;)
      {
        Symbol const &rightSymbol=right[x];

        /***** Move on if terminal *****/
        if(rightSymbol.isTerminal())
        {
          break;
        }

        /***** Add on *****/
        if(rightSymbol != leftSymbol)
        {
          SymbolMap::const_iterator leftSetIterator=prevMap.find(leftSymbol);
          if(leftSetIterator != prevMap.end())
          {
            SymbolSet const &leftSet=leftSetIterator->second;
            currentMap[rightSymbol].insert(leftSet.begin(), leftSet.end());
          }
        }

        /***** Stop processing if FIRST() does not contain epsilon *****/
        SymbolSet const &rightSet = this->first(rightSymbol);
        if(rightSet.find(EPS()) == rightSet.end())
        {
          break;
        }
      }
    }
  }

  return prevMap;
}

SymbolSet Grammar::first(Symbol const &i_symbol, SymbolMap const &i_firstMap)
{
  if(i_symbol.isTerminal())
  {
    SymbolSet ss = {i_symbol};
    return ss;
  }

  return i_firstMap.at(i_symbol);
}

SymbolSet Grammar::first(Symbol const &i_symbol) const
{
  if(!(m_cacheFlags & CacheFlags::BUILTFIRST))
  {
    //Order is necessary - buildFirst eventually calls first
    m_cacheFlags |= CacheFlags::BUILTFIRST;
    m_cacheFirst = this->buildFirst();
  }

  return Grammar::first(i_symbol, m_cacheFirst);
}

SymbolSet Grammar::firstList(SymbolList const &i_symbolList) const
{
  if(!(m_cacheFlags & CacheFlags::BUILTFIRST))
  {
    //Order is necessary - buildFirst eventually calls first
    m_cacheFlags |= CacheFlags::BUILTFIRST;
    m_cacheFirst = this->buildFirst();
  }

  return Grammar::firstList(i_symbolList, m_cacheFirst);
}

SymbolSet Grammar::firstList(SymbolList const &i_symbolList, SymbolMap const &i_firstMap)
{
  SymbolSet ss;
  if(i_symbolList.isEmpty() || (i_symbolList.containsEpsilon() && i_symbolList.count() == 1))
  {
    /***** Add epsilon for empty rule *****/
    ss.insert(EPS());
  }
  else
  {
    for(size_t x=0; x<i_symbolList.count(); ++x)
    {
      Symbol const &symbolListSymbol = i_symbolList[x];
      try
      {
        SymbolSet const &symbolListSet = Grammar::first(symbolListSymbol, i_firstMap);

        /***** Add FIRST of element to FIRST of rule *****/
        ss.insert(symbolListSet.begin(), symbolListSet.end());

        if(symbolListSet.find(EPS()) == symbolListSet.end())
        {
          /***** Remove epsilon - this rule must always have something *****/
          if(x==i_symbolList.count()-1)
          {
            ss.erase(EPS());
          }
          break;
        }
      }
      catch(std::out_of_range)
      {
#ifndef NDEBUG
        std::cerr << "FIRST: Symbol '" << symbolListSymbol.toString() << "' has no first entry yet." << std::endl;
#endif
        break;
      }
    }
  }

  return ss;
}

ProductionConstPtrVector Grammar::productionPointers(SymbolList const &i_left) const
{
  ProductionConstPtrVector outputVector;

  for(Production const &p : m_productions)
  {
    if(p.left() == i_left)
    {
      outputVector.push_back(&p);
    }
  }

  return outputVector;
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
  this->add(Production(std::forward<Symbol>(i_symbol)));

  return (*this);
}

Production const &Grammar::operator [](size_t const i_ruleIndex) const
{
  return m_productions.at(i_ruleIndex);
}
