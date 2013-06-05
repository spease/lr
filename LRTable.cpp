#include "LRTable.hpp"

#include <stdexcept>

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
  LRItemSet items;
  switch(i_type)
  {
  case Type::LR:
    items=LRTable::buildLRItems(g);
    break;
  }
  if(items.empty())
  {
    throw std::range_error("No items built from grammar.");
  }

#ifndef NDEBUG
  printItemSet("Items", items);
#endif

  /***** Populate table *****/
  for(LRItemSet::const_iterator isit=items.begin(); isit!=items.end(); ++isit)
  {
    if(isit->lookahead() == END() && isit->rightPosition() >= isit->production().right().count() && isit->production().left()[0] == i_grammar.startSymbol())
    {
      this->insertAction(LRState(isit->iteration()), END(), ACCEPT());
    }
    else if(isit->rightPosition() >= isit->production().right().count())
    {
      this->insertAction(LRState(isit->iteration()), isit->lookahead(), REDUCE(&isit->production()));
    }
  }
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

#if 0
LRItemSet LRTable::computeKernelItems(LRItemSet const &i_nonkernelItems, size_t const i_iteration, Grammar const &i_grammar)
{
}

LRItemSet LRTable::computeNonkernelItems(LRItemSet const &i_kernelItems, size_t const i_iteration, Grammar const &i_grammar)
{
}

LRItemSet LRTable::buildLALRItems(Grammar const &i_grammar)
{
  std::vector<LRItemSet> kernelItems;
  std::vector<LRItemSet> nonkernelItems;
  size_t iteration=0;

  bool itemAdded=true;
  while(itemAdded)
  {
    LRItemSet currentKernelItems;
    LRItemSet currentNonkernelItems;

    /***** Build kernel items *****/
    if(iteration == 0)
    {
      currentKernelItems.insert(LRItem(&i_grammar[0], 0, END(), iteration));
    }
    else
    {
      currentKernelItems = LRTable::computeKernelItems(nonkernelItems[iteration-1], iteration, i_grammar);
    }

    /***** Build nonkernel items *****/
    currentNonkernelItems = LRTable::computeNonkernelItems(currentKernelItems, iteration, i_grammar);

    /***** Allocate *****/
    if(currentKernelItems.size() > 0 || currentNonkernelItems.size() > 0)
    {
      kernelItems.push_back(LRItemSet());
      nonkernelItems.push_back(LRItemSet());
    }

    /***** Save *****/
    if(currentKernelItems.size() > 0)
    {
      kernelItems[iteration].insert(currentKernelItems.begin(), currentKernelItems.end());
    }
    if(currentNonkernelItems.size() > 0)
    {
      nonkernelItems[iteration].insert(currentNonkernelItems.begin(), currentNonkernelItems.end());
    }
  }
}
#endif

LRItemSet LRTable::buildLRItems(Grammar const &i_grammar)
{
  LRItemSet items;
  std::vector<LRItemSet> states;
  size_t iteration=0;

  /***** Do start state *****/
  LRItemSet const startState = {LRItem(&i_grammar[0], 0, END(), iteration)};
  LRItemSet const startStateClosure = LRTable::closure(startState, iteration, i_grammar);
  items.insert(startStateClosure.begin(), startStateClosure.end());
  states.push_back(startStateClosure);
  iteration=states.size();

  bool itemAdded = true;
  while(itemAdded)
  {
    itemAdded = false;
    for(size_t i=0; i<states.size(); ++i)
    {
      LRItemSet const &sourceState=states[i];
      LRItemSet currentState;
      for(SymbolSet::const_iterator ait=i_grammar.alphabetBegin(); ait!=i_grammar.alphabetEnd(); ++ait)
      {
        LRItemSet pathsResult = LRTable::computePaths(sourceState, iteration, *ait, i_grammar);
        if(pathsResult.size() < 1)
        {
          continue;
        }

        for(LRItemSet::const_iterator prit=pathsResult.begin(); prit!=pathsResult.end(); ++prit)
        {
          LRItemSet::const_iterator findResult = items.find(*prit);
          if(findResult == items.end())
          {
            currentState.insert(*prit);
            itemAdded = true;
          }
        }

        if(currentState.size() > 0)
        {
          items.insert(currentState.begin(), currentState.end());
          states.push_back(currentState);
          iteration=states.size();
        }
      }
    }
  }

  return items;
}

LRItemSet LRTable::closure(LRItemSet const &i_itemSet, size_t const i_iteration, Grammar const &i_grammar)
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
          LRItem finalItem(&p, 0, *esit, i_iteration);
          
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

void LRTable::insertAction(LRState const &i_state, SymbolList const &i_symbolList, LRAction const &i_action)
{
  if(i_state >= m_actions.size())
  {
    m_actions.resize(i_state+1);
  }

  m_actions[i_state].insert(std::pair<SymbolList, LRAction>(i_symbolList, i_action));
}

void LRTable::insertPath(LRState const &i_state, SymbolList const &i_symbolList, LRState const &i_destinationState)
{
  if(i_state >= m_paths.size())
  {
    m_paths.resize(i_state+1);
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

LRItemSet LRTable::computePaths(LRItemSet const &i_itemSet, size_t const i_iteration, Symbol const &i_symbol, Grammar const &i_grammar)
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
      outputSet.insert(LRItem(&isit->production(), isit->rightPosition()+1, isit->lookahead(), i_iteration));
    }
  }

  return LRTable::closure(outputSet, i_iteration, i_grammar);
}
/**************************************************/
