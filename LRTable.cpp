#include "LRTable.hpp"

#include <stdexcept>
#include <iostream>

/********************----- CLASS: LRTable -----********************/
LRTable::LRTable(LRTable::Type const i_type, Grammar const &i_grammar)
:m_type(i_type)
{
  Grammar const &g=i_grammar;

  /***** Check that grammar is context-free *****/
  if(!g.isContextFree())
  {
    throw std::logic_error("Grammar is not context-free");
  }

  /***** Build items *****/
  LRItemSetVector states;
  switch(i_type)
  {
  case Type::LALR:
    states=LRTable::buildLALRItems(g);
    break;
  case Type::LR:
    states=LRTable::buildLRItems(g);
    break;
  }
  if(states.empty())
  {
    throw std::range_error("No items built from grammar.");
  }

  /***** Populate table *****/
  for(size_t i=0; i<states.size(); ++i)
  {
    /***** Build actions *****/
    for(LRItemSet::const_iterator isit=states[i].begin(); isit!=states[i].end(); ++isit)
    {
      if(isit->rightPosition() < isit->production().right().count() && isit->production().right()[isit->rightPosition()].isNonterminal())
      {
        Symbol const nextSymbol = isit->production().right()[isit->rightPosition()];
        LRItemSet const currentPath=this->computePaths(states[i], nextSymbol, i_grammar);
        for(size_t j=0; j<states.size(); ++j)
        {
          if(j==i)
          {
            continue;
          }

          if(currentPath == states[j])
          {
            this->insertAction(LRState(i), nextSymbol, SHIFT(LRState(j)));
          }
        }
      }
      else if(isit->rightPosition() >= isit->production().right().count())
      {
        this->insertAction(LRState(i), isit->lookahead(), REDUCE(&isit->production()));
      }
      else if(isit->lookahead() == END() && isit->rightPosition() >= isit->production().right().count() && isit->production().left()[0] == i_grammar.startSymbol())
      {
        this->insertAction(LRState(i), END(), ACCEPT());
      }
    }

    /***** Build paths *****/
    for(SymbolSet::const_iterator ait=i_grammar.alphabetBegin(); ait!=i_grammar.alphabetEnd(); ++ait)
    {
      if(ait->isNonterminal())
      {
        LRItemSet const currentPath=this->computePaths(states[i], *ait, i_grammar);
        for(size_t j=0; j<states.size(); ++j)
        {
          if(currentPath == states[j])
          {
            this->insertPath(LRState(i),*ait, LRState(j));
          }
        }
      }
    }
  }

#ifndef NDEBUG
  std::cout << this->toString() << std::endl;
#endif
}

LRAction LRTable::action(LRState const &i_currentState, SymbolList const &i_symbolList) const
{
  std::pair<ActionRow::const_iterator, ActionRow::const_iterator> actionPair = m_actions.at(i_currentState).equal_range(i_symbolList);

  /***** Nothing *****/
  if(actionPair.first == actionPair.second)
  {
    throw std::out_of_range(i_symbolList.toString());
  }

  /***** More than one ******/
  ActionRow::const_iterator actionNext = ++actionPair.first;
  if(actionNext != actionPair.second)
  {
    throw std::range_error("Shift/Shift conflict");
  }

  return actionPair.first->second;
}

LRItemSet LRTable::computeKernelItems(LRItemSet const &i_kernelItems, LRItemSet const &i_nonkernelItems, Grammar const &i_grammar)
{
  LRItemSet outputItems;

  for(LRItemSet::const_iterator kit=i_kernelItems.begin(); kit!=i_kernelItems.end(); ++kit)
  {
    SymbolList const &right=kit->production().right();
    if(kit->rightPosition() < right.count())
    {
      outputItems.insert(LRItem(&kit->production(), kit->rightPosition()+1, kit->lookahead()));
    }
  }

  for(LRItemSet::const_iterator nit=i_nonkernelItems.begin(); nit!=i_nonkernelItems.end(); ++nit)
  {
    SymbolList const &right=nit->production().right();
    if(nit->rightPosition() < right.count())
    {
      outputItems.insert(LRItem(&nit->production(), nit->rightPosition()+1, nit->lookahead()));
    }
  }

  return outputItems;
}

LRItemSet LRTable::computeNonkernelItems(LRItemSet const &i_kernelItems, Grammar const &i_grammar)
{
  LRItemSet nonkernelItems;

  /***** Generate items from kernel items *****/
  for(LRItemSet::const_iterator kit=i_kernelItems.begin(); kit!=i_kernelItems.end(); ++kit)
  {
    SymbolList const &right=kit->production().right();
    if(kit->rightPosition() >= right.count())
    {
      continue;
    }

    Symbol const &nextSymbol = right[kit->rightPosition()];
    if(!nextSymbol.isNonterminal())
    {
      continue;
    }

    ProductionConstPtrVector productionPointers=i_grammar.productionPointers(nextSymbol);
    if(productionPointers.empty())
    {
      continue;
    }

    for(Production const *p: productionPointers)
    {
      nonkernelItems.insert(LRItem(p, 0));
    }
  }

  /***** Generate items from previously generated items *****/
  bool itemAdded = true;
  while(itemAdded)
  {
    itemAdded = false;

    for(LRItemSet::const_iterator nit=nonkernelItems.begin(); nit!=nonkernelItems.end(); ++nit)
    {
      /***** Check if end of rule *****/
      SymbolList const &right=nit->production().right();
      if(nit->rightPosition() >= right.count())
      {
        continue;
      }

      /***** Get next symbol, if nonterminal *****/
      Symbol const &nextSymbol = right[nit->rightPosition()];
      if(!nextSymbol.isNonterminal())
      {
        continue;
      }

      /***** Find production rules *****/
      ProductionConstPtrVector productionPointers=i_grammar.productionPointers(nextSymbol);
      if(productionPointers.empty())
      {
        continue;
      }

      /***** Maybe insert production rules *****/
      for(Production const *p:productionPointers)
      {
        /***** Check if item exists *****/
        LRItem item(p, 0);
        if(nonkernelItems.find(item) != nonkernelItems.end())
        {
          continue;
        }
      
        /***** Insert item *****/
        nonkernelItems.insert(std::move(item));
        itemAdded = true;
      }
    }
  }

  /***** Return *****/
  return nonkernelItems;
}

LRItemSetVector LRTable::buildLALRItems(Grammar const &i_grammar)
{
  std::vector<LRItemSet> outputItems;

  bool itemAdded=true;
  size_t iteration=0;
  LRItemSet kernelItems;
  LRItemSet nonkernelItems;
  while(itemAdded)
  {
    /***** Build kernel items *****/
    if(iteration == 0)
    {
      kernelItems.insert(LRItem(&i_grammar[0], 0, END()));
    }
    else
    {
      kernelItems = LRTable::computeKernelItems(kernelItems, nonkernelItems, i_grammar);
    }

    /***** Build nonkernel items *****/
    nonkernelItems = LRTable::computeNonkernelItems(kernelItems, i_grammar);

    /***** Store them *****/
    outputItems[iteration].insert(kernelItems.begin(), kernelItems.end());
    outputItems[iteration].insert(nonkernelItems.begin(), nonkernelItems.end());

    ++iteration;
  }

  return outputItems;
}

LRItemSetVector LRTable::buildLRItems(Grammar const &i_grammar)
{
  std::vector<LRItemSet> states;

  /***** Do start state *****/
  LRItemSet const startState = {LRItem(&i_grammar[0], 0, END())};
  LRItemSet const startStateClosure = LRTable::closure(startState, i_grammar);
  states.push_back(startStateClosure);

  bool itemAdded = true;
  while(itemAdded)
  {
    itemAdded = false;
    for(size_t i=0; i<states.size(); ++i)
    {
      LRItemSet const &sourceState=states[i];
      for(SymbolSet::const_iterator ait=i_grammar.alphabetBegin(); ait!=i_grammar.alphabetEnd(); ++ait)
      {
        LRItemSet currentState = LRTable::computePaths(sourceState, *ait, i_grammar);
        if(currentState.size() < 1)
        {
          continue;
        }

        bool stateFound=false;
        for(std::vector<LRItemSet>::const_iterator vit=states.begin(); vit!=states.end(); ++vit)
        {
          if(*vit == currentState)
          {
            stateFound = true;
            break;
          }
        }

        if(!stateFound)
        {
          states.push_back(currentState);
          itemAdded = true;
        }
      }
    }
  }

#ifndef NDEBUG
  printItemSetVector("States", states);
#endif

  return states;
}

LRItemSet LRTable::closure(LRItemSet const &i_itemSet, Grammar const &i_grammar)
{
  if(!i_grammar.isContextFree())
  {
    throw std::logic_error("Tried to build first set for non-context-free grammar");
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
      SymbolSet expectedSymbols = i_grammar.firstList(currentRightEnding);

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

LRItemSet LRTable::computePaths(LRItemSet const &i_itemSet, Symbol const &i_symbol, Grammar const &i_grammar)
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

  return LRTable::closure(outputSet, i_grammar);
}


void LRTable::insertAction(LRState const &i_state, SymbolList const &i_symbolList, LRAction const &i_action)
{
  if(i_state >= m_actions.size())
  {
    m_actions.resize(i_state+1);
  }

  /***** Don't insert duplicate *****/
  std::pair<ActionRow::const_iterator, ActionRow::const_iterator> similarActions = m_actions[i_state].equal_range(i_symbolList);
  for(ActionRow::const_iterator ait=similarActions.first; ait!=similarActions.second; ++ait)
  {
    if(ait->second == i_action)
    {
      return;
    }
  }

  m_actions[i_state].insert(std::pair<SymbolList, LRAction>(i_symbolList, i_action));
}

void LRTable::insertPath(LRState const &i_state, SymbolList const &i_symbolList, LRState const &i_destinationState)
{
  if(i_state >= m_paths.size())
  {
    m_paths.resize(i_state+1);
  }

  /***** Don't insert duplicate *****/
  std::pair<PathRow::const_iterator, PathRow::const_iterator> similarPaths = m_paths[i_state].equal_range(i_symbolList);
  for(PathRow::const_iterator pit=similarPaths.first; pit!=similarPaths.second; ++pit)
  {
    if(pit->second == i_destinationState)
    {
      return;
    }
  }

  m_paths[i_state].insert(std::pair<SymbolList, LRState>(i_symbolList, i_destinationState));
}

LRState LRTable::path(LRState const &i_currentState, SymbolList const &i_symbolList) const
{
  std::pair<PathRow::const_iterator, PathRow::const_iterator> pathPair = m_paths.at(i_currentState).equal_range(i_symbolList);

  /***** Nothing *****/
  if(pathPair.first == pathPair.second)
  {
    throw std::out_of_range(i_symbolList.toString());
  }

  /***** More than one *****/
  PathRow::const_iterator pathNext = ++pathPair.first;
  if(pathNext != pathPair.second)
  {
    throw std::range_error("Reduce/reduce conflict");
  }

  return pathPair.first->second;
}

std::string LRTable::toString() const
{
  std::string outputString;
  for(size_t stateIndex=0; stateIndex<std::max(m_actions.size(), m_paths.size()); ++stateIndex)
  {
    outputString += std::to_string(stateIndex) + " ACTIONS:\n";
    if(stateIndex < m_actions.size())
    {
      for(ActionRow::const_iterator ait=m_actions[stateIndex].begin(); ait!=m_actions[stateIndex].end(); ++ait)
      {
        outputString += "\t (" + ait->first.toString() + ") -> " + ait->second.toString() + "\n";
      }
    }

    outputString += std::to_string(stateIndex) + " PATHS:\n";
    if(stateIndex < m_paths.size())
    {
      for(PathRow::const_iterator pit=m_paths[stateIndex].begin(); pit!=m_paths[stateIndex].end(); ++pit)
      {
        outputString += "\t (" + pit->first.toString() + ") -> " + std::to_string(pit->second) + "\n";
      }
    }

    outputString += "\n";
  }

  return outputString;
}
/**************************************************/
