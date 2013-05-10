#include "LexText.hpp"

/********************----- CLASS: LexText -----********************/
LexText::LexText(std::string const &i_filepath)
:m_input(i_filepath.c_str())
{
}

LexText::~LexText()
{
}

Symbol LexText::pop()
{
  char c;
  std::string s;
  bool cAlnum = false;
  bool cSpace = false;

  /***** Skip whitespace *****/
  while(!m_input.eof())
  {
    c = m_input.peek();
    if(isspace(c))
    {
      m_input.get();
    }
    else
    {
      break;
    }
  }

  if(m_input.eof())
  {
    return END();
  }

  /***** Parse tokens *****/
  //1: Parse letters/numbers together
  //2: Parse all other symbols one character at a time
  //3: Skip whitespace
  while(!m_input.eof())
  {
    c = m_input.get();

    cAlnum = isalnum(c);
    cSpace = isspace(c);

    if(!cAlnum && s.size() > 0)
    {
      break;
    }

    if(!cSpace)
    {
      s += c;
    }

    if(!cAlnum)
    {
      break;
    }
  }

  /***** Skip whitespace *****/
  while(!m_input.eof())
  {
    c = m_input.peek();
    if(isspace(c))
    {
      m_input.get();
    }
    else
    {
      break;
    }
  }

  Symbol outputSymbol(Symbol::Type::T_TERMINAL, s.c_str());
  return outputSymbol;
}
/**************************************************/
