#include "LRParser.hpp"
#include "LRAction.hpp"
#include "LRState.hpp"

#include "Grammar.hpp"
#include "Production.hpp"

/********************----- CLASS: LRParser -----********************/
LRParser::LRParser(LRTable::Type const i_type, size_t const i_k, Grammar const &i_grammar)
:m_grammarPointer(&i_grammar), m_k(i_k), m_table(i_type, i_grammar)
{
}

/*
bool LRParser::parse(Lex &i_lex)
{
  LRStateStack stateStack;
  SymbolStack symbolStack;
  Symbol token = i_lex.pop();
  while(true)
  {
    LRState s=stateStack.top();
    LRAction const &action=this->action(s, token);
    if(action.isShift())
    {
      symbolStack.push(token);
      stateStack.push(action.state());
      token=i_lex.pop();
    }
    else if(action.isReduce())
    {
      Production const &p = action.production();
      Symbol const &leftSymbol = p.left()[0];
      SymbolList const &right = p.right();
      size_t const popCount = right.count();
      for(size_t x=0; x<popCount; ++x)
      {
        symbolStack.pop();
        stateStack.pop();
      }
      symbolStack.push(leftSymbol);
      stateStack.push(this->path(s, leftSymbol));
    }
    else if(action.isAccept())
    {
      return true;
    }
    else
    {
      throw std::runtime_error("NOES!");
    }
  }
}
*/
/**************************************************/


