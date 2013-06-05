#include "LRParser.hpp"
#include "LRAction.hpp"
#include "LRState.hpp"

#include "Grammar.hpp"
#include "Production.hpp"

#include <iostream>

/********************----- CLASS: LRParser -----********************/
LRParser::LRParser(LRTable::Type const i_type, size_t const i_k, Grammar const &i_grammar)
:m_table(i_type, i_grammar)
{
  m_stackState.push(LRState(0));
}

bool LRParser::parse(Lex &i_lex)
{
  bool accepted=false;
  Symbol token = i_lex.pop();
  while(!m_stackState.empty())
  {
    LRState s=m_stackState.top();
    LRAction const &action=m_table.action(s, token);

#ifndef NDEBUG
    std::cout << "(" + std::to_string(s) + " + " + token.toString() + ") --> " + action.toString() << std::endl;
#endif

    if(action.isShift())
    {
      m_stackSymbol.push(token);
      m_stackState.push(action.state());
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
        m_stackSymbol.pop();
        m_stackState.pop();
      }
      m_stackSymbol.push(leftSymbol);
      m_stackState.push(m_table.path(s, leftSymbol));
    }
    else if(action.isAccept())
    {
      accepted=true;
      break;
    }
    else
    {
      throw std::runtime_error("NOES!");
    }
  }

  return accepted;
}
/**************************************************/


