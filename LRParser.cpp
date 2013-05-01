#include "LRParser.hpp"

#include "Grammar.hpp"
#include "Production.hpp"

#include <algorithm>
#include <iostream>

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
  SymbolMap first=LRParser::buildFirst(g);
  if(first.empty())
  {
    return false;
  }

  SymbolMap follow=LRParser::buildFollow(i_grammarPointer);
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
        ss.insert(EPS());
      }
      else
      {
        for(size_t x=0; x<right.count(); ++x)
        {
          Symbol const &rightSymbol = right[x];
          if(rightSymbol.isTerminal())
          {
            ss.insert(rightSymbol);
            break;
          }
          else if(rightSymbol.isEpsilon())
          {
            ss.insert(rightSymbol);
          }
          else
          {
            SymbolSet const &prevSet = prevMap[rightSymbol];
            if(!prevSet.empty())
            {
              ss.insert(prevSet.begin(), prevSet.end());
            }

            if(prevSet.find(EPS()) == prevSet.end())
            {
              break;
            }
          }
        }
      }

      /***** Add list *****/
      currentMap[leftSymbol].insert(ss.begin(), ss.end());
    }

    std::cout << "--------------------------------------------------" << std::endl;
    mapChanged = (prevMap != currentMap);
    if(mapChanged)
    {
      std::cout << prevMap.size() << " != " << currentMap.size() << std::endl;
      prevMap = std::move(currentMap);
    }
  }

  return prevMap;
}

SymbolMap LRParser::buildFollow(Grammar const * const i_grammarPointer)
{
  if(i_grammarPointer == nullptr)
  {
    return SymbolMap();
  }

  Grammar const &g = *i_grammarPointer;
  if(!g.isContextFree())
  {
    return SymbolMap();
  }

  SymbolMap sm;
  SymbolSet ss = {END()};

  sm[g.startSymbol()] = ss;

  return sm;
}

SymbolSet LRParser::first(Symbol const &i_symbol) const
{
  if(i_symbol.isTerminal())
  {
    SymbolSet ss = {i_symbol};
    return ss;
  }

  return m_first.at(i_symbol);
}

SymbolSet LRParser::follow(Symbol const &i_symbol) const
{
  return m_follow.at(i_symbol);
}
/**************************************************/

/********************----- Operators -----********************/
bool operator ==(SymbolMap const &i_A, SymbolMap const &i_B)
{
  bool result=true;
  if(i_A.size() != i_B.size())
  {
    return false;
  }

  for(SymbolMap::const_iterator ait=i_A.begin(); ait!=i_A.end(); ++ait)
  {
    SymbolMap::const_iterator bit=i_B.find(ait->first);
    if(bit==i_B.end() || ait->second != bit->second)
    {
      result=false;
    }

    std::cout << ait->first.toString() << " ==>" << std::endl << "\t";
    std::for_each(ait->second.begin(), ait->second.end(), [](Symbol const &s){std::cout << s.toString() << " ";});
    std::cout << std::endl << "\t";
    std::for_each(bit->second.begin(), bit->second.end(), [](Symbol const &s){std::cout << s.toString() << " ";});
    std::cout << std::endl;
  }

  return result;
}
bool operator !=(SymbolMap const &i_A, SymbolMap const &i_B)
{
  return !(i_A == i_B);
}
/**************************************************/
