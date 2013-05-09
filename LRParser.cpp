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

void printItemSet(std::string const &i_name, LRItemSet const &i_itemSet)
{
  std::cout << "===== " << i_name << " =====" << std::endl;
  for(LRItemSet::const_iterator lit=i_itemSet.begin(); lit!=i_itemSet.end();++lit)
  {
    //for(size_t i=0; i<3 && lit!=i_itemSet.end(); ++i,++lit)
    {
      std::cout << lit->toString() << " ";
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

      SymbolSet ss = LRParser::firstList(right, prevMap);

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
          SymbolMap::const_iterator leftSetIterator=prevMap.find(leftSymbol);
          if(leftSetIterator != prevMap.end())
          {
            SymbolSet const &leftSet=leftSetIterator->second;
            currentMap[rightSymbol].insert(leftSet.begin(), leftSet.end());
          }
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

LRItemSet LRParser::buildItems(SymbolMap const &i_firstMap, Grammar const &i_grammar)
{
  LRItemSet outputSet;

  LRItemSet augmentedStartItem = {LRItem(&i_grammar[0], 0, END())};
  outputSet = LRParser::closure(augmentedStartItem, i_firstMap, i_grammar);

  bool itemAdded = true;
  while(itemAdded)
  {
    itemAdded = false;
    for(LRItemSet::const_iterator osit=outputSet.begin(); osit!=outputSet.end(); ++osit)
    {
      LRItemSet currentItemSet={*osit};
      for(SymbolSet::const_iterator ait=i_grammar.alphabetBegin(); ait!=i_grammar.alphabetEnd(); ++ait)
      {
        LRItemSet pathsResult = LRParser::paths(currentItemSet, *ait, i_firstMap, i_grammar);
        if(pathsResult.size() < 1)
        {
          continue;
        }

        for(LRItemSet::const_iterator prit=pathsResult.begin(); prit!=pathsResult.end(); ++prit)
        {
          LRItemSet::const_iterator findResult = outputSet.find(*prit);
          if(findResult == outputSet.end())
          {
            outputSet.insert(*prit);
            itemAdded = true;
          }
        }
      }
    }
  }

  return outputSet;
}

LRItemSet LRParser::closure(LRItemSet const &i_itemSet, SymbolMap const &i_first, Grammar const &i_grammar)
{
  if(!i_grammar.isContextFree())
  {
    return LRItemSet();
  }

  LRItemSet outputSet(i_itemSet);

  bool itemAdded = true;
  while(itemAdded)
  {
    itemAdded = false;

    /***** Go through all the items passed in *****/
    for(LRItemSet::const_iterator lit=outputSet.begin(); lit!=outputSet.end(); ++lit)
    {
      LRItem const &currentItem = (*lit);
      SymbolList const &currentRight = currentItem.production().right();
      if(currentItem.rightPosition() >= currentRight.count())
      {
        continue;
      }
      Symbol const &currentRightSymbol=currentRight[currentItem.rightPosition()];

      /***** Compute up and coming symbols *****/
      SymbolList currentRightEnding=currentRight.sublist(currentItem.rightPosition()+1);
      currentRightEnding += currentItem.lookahead();
      SymbolSet expectedSymbols = LRParser::firstList(currentRightEnding, i_first);

      /***** Go through all the productions matching the next symbol *****/
      for(size_t productionIndex=0; productionIndex<i_grammar.productionCount(); ++productionIndex)
      {
        Production const &p=i_grammar[productionIndex];
        if(p.left()[0] != currentRightSymbol)
        {
          continue;
        }

        for(SymbolSet::const_iterator esit=expectedSymbols.begin(); esit!=expectedSymbols.end(); ++esit)
        {
          LRItem finalItem(&p, 0, *esit);
          
          LRItemSet::const_iterator osit=outputSet.find(finalItem);
          if(osit == outputSet.end())
          {
            outputSet.insert(std::move(finalItem));
            itemAdded = true;
          }
        }
      }
    }
  }

  return outputSet;
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

SymbolSet LRParser::firstList(SymbolList const &i_symbolList, SymbolMap const &i_firstMap)
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
      Symbol const &i_symbolListSymbol = i_symbolList[x];
      try
      {
        SymbolSet const &i_symbolListSet = LRParser::first(i_symbolListSymbol, i_firstMap);

        /***** Add FIRST of element to FIRST of rule *****/
        ss.insert(i_symbolListSet.begin(), i_symbolListSet.end());

        if(i_symbolListSet.find(EPS()) == i_symbolListSet.end())
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
        std::cerr << "FIRST: Symbol '" << i_symbolListSymbol.toString() << "' has no first entry yet." << std::endl;
#endif
        break;
      }
    }
  }

  return ss;
}

SymbolSet LRParser::follow(Symbol const &i_symbol) const
{
  return m_follow.at(i_symbol);
}

LRItemSet LRParser::paths(LRItemSet const &i_itemSet, Symbol const &i_symbol, SymbolMap const &i_firstMap, Grammar const &i_grammar)
{
  LRItemSet outputSet;

  for(LRItemSet::const_iterator isit=i_itemSet.begin(); isit!=i_itemSet.end(); ++isit)
  {
    SymbolList const &right=isit->production().right();
    if(isit->rightPosition() >= right.count())
    {
      continue;
    }

    if(right[isit->rightPosition()] == i_symbol)
    {
      outputSet.insert(LRItem(&isit->production(), isit->rightPosition()+1, isit->lookahead()));
    }
  }

  return LRParser::closure(outputSet, i_firstMap, i_grammar);
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

  /***** Build items *****/
  LRItemSet items=LRParser::buildItems(first, g);
  if(items.empty())
  {
    return false;
  }

#ifndef NDEBUG
  printItemSet("Items", items);
#endif

  m_first = first;
  m_follow = follow;
  m_grammarPointer = i_grammarPointer;
  m_items = items;

  return true;
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
