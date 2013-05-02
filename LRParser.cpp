#include "LRParser.hpp"

#include "Grammar.hpp"
#include "Production.hpp"

#ifndef NDEBUG
#include <iostream>
void printSymbolMap(std::string const &i_name, SymbolMap const &i_map)
{
  std::cout << "===== " << i_name << " =====" << std::endl;
  for(SymbolMap::const_iterator mit=i_map.begin(); mit!=i_map.end(); ++mit)
  {
    std::cout << "\t" << mit->first.toString() << " ==> ";

    SymbolSet const &ss=mit->second;
    for(SymbolSet::const_iterator sit=ss.begin(); sit!=ss.end(); ++sit)
    {
      std::cout << sit->toString() << " ";
    }
    std::cout << std::endl;
  }
  std::cout << "==================================================" << std::endl;
}
#endif

/********************----- CLASS: LRParser -----********************/
LRParser::LRParser()
{
  m_grammarPointer = nullptr;
}

bool LRParser::setGrammar(Grammar const * const i_grammarPointer)
{
  if(i_grammarPointer == nullptr)
  {
    return false;
  }

  Grammar const &g = *i_grammarPointer;

  /***** Build FIRST map *****/
  SymbolMap first=LRParser::buildFirst(g);
  if(first.empty())
  {
    return false;
  }

  /***** Build FOLLOW map *****/
  SymbolMap follow=LRParser::buildFollow(first, g);
  if(follow.empty())
  {
    return false;
  }

  m_first = first;
  m_follow = follow;
  m_grammarPointer = i_grammarPointer;

  return true;
}

SymbolMap LRParser::buildFirst(Grammar const &i_grammar)
{
  Grammar const &g=i_grammar;
  if(!g.isContextFree())
  {
    return SymbolMap();
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

      SymbolSet ss;
      if(right.isEmpty() || (right.containsEpsilon() && right.count() == 1))
      {
        /***** Add epsilon for empty rule *****/
        ss.insert(EPS());
      }
      else
      {
        for(size_t x=0; x<right.count(); ++x)
        {
          Symbol const &rightSymbol = right[x];
          try
          {
            SymbolSet const &rightSet = LRParser::first(rightSymbol, prevMap);

            /***** Add FIRST of element to FIRST of rule *****/
            ss.insert(rightSet.begin(), rightSet.end());

            if(rightSet.find(EPS()) == rightSet.end())
            {
              /***** Remove epsilon - this rule must always have something *****/
              if(x==right.count()-1)
              {
                ss.erase(EPS());
              }
              break;
            }
          }
          catch(std::out_of_range)
          {
#ifndef NDEBUG
            std::cerr << "FIRST: Symbol '" << rightSymbol.toString() << "' has no first entry yet." << std::endl;
#endif
            break;
          }
        }
      }

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

SymbolMap LRParser::buildFollow(SymbolMap const &i_first, Grammar const &i_grammar)
{
  Grammar const &g=i_grammar;
  if(!g.isContextFree())
  {
    return SymbolMap();
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
      firstSet = LRParser::first(right[right.count()-1], i_first);

      for(size_t x=right.count()-1; x-- > 0;)
      {
        Symbol const &rightSymbol = right[x];

        /***** Add FIRST(rightSymbol) onto FOLLOW(rightSymbol) *****/
        if(rightSymbol.isNonterminal())
        {
          currentMap[rightSymbol].insert(firstSet.begin(), firstSet.end());
        }

        /***** If no epsilon, reset firstSet *****/
        SymbolSet const &rightSet = LRParser::first(rightSymbol, i_first);
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
          SymbolSet const &leftSet=prevMap.at(leftSymbol);
          currentMap[rightSymbol].insert(leftSet.begin(), leftSet.end());
        }

        /***** Stop processing if FIRST() does not contain epsilon *****/
        SymbolSet const &rightSet = LRParser::first(rightSymbol, i_first);
        if(rightSet.find(EPS()) == rightSet.end())
        {
          break;
        }
      }
    }
  }

  return prevMap;
}

SymbolSet LRParser::first(Symbol const &i_symbol) const
{
  return LRParser::first(i_symbol, m_first);
}

SymbolSet LRParser::first(Symbol const &i_symbol, SymbolMap const &i_firstMap)
{
  if(i_symbol.isTerminal())
  {
    SymbolSet ss = {i_symbol};
    return ss;
  }

  return i_firstMap.at(i_symbol);
}

SymbolSet LRParser::follow(Symbol const &i_symbol) const
{
  return m_follow.at(i_symbol);
}
/**************************************************/

/********************----- Operators -----********************/
bool operator ==(SymbolMap const &i_A, SymbolMap const &i_B)
{
  if(i_A.size() != i_B.size())
  {
    return false;
  }

  for(SymbolMap::const_iterator ait=i_A.begin(); ait!=i_A.end(); ++ait)
  {
    SymbolMap::const_iterator bit=i_B.find(ait->first);
    if(bit==i_B.end() || ait->second != bit->second)
    {
      return false;
    }
  }

  return true;
}
bool operator !=(SymbolMap const &i_A, SymbolMap const &i_B)
{
  return !(i_A == i_B);
}
/**************************************************/
