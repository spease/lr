#include "LRParser.hpp"
#include "Grammar.hpp"
#include "Production.hpp"
#include "Symbol.hpp"

#include <iostream>

int main(int const argc, char const * const * const argv)
{
  Grammar g;
  /*
  g |= NT("goal") >>= NT("expr");
  g |= NT("expr") >>= NT("term") + T("+") + NT("expr") | NT("term");
  g |= NT("term") >>= NT("factor") + T("*") + NT("term") | NT("factor");
  g |= NT("factor") >>= T("0") | T("1") | T("2") | T("3") | T("4") | T("5") | T("6") | T("7") | T("8") | T("9");
  */
  /*
  g |= NT("E") >>= NT("E") + T("*") + NT("B");
  g |= NT("E") >>= NT("E") + T("+") + NT("B");
  g |= NT("E") >>= NT("B");
  g |= NT("B") >>= T("0");
  g |= NT("B") >>= T("1");
  */
  //g |= NT("B") >>= T("hello") + T(" ") + T("world") | T("goodbye") + T(" ") + T("world");
  g |= NT("start") >>= NT("start") + NT("expr");
  g |= NT("start");
  g |= NT("expr") >>= T("NR");
  g |= NT("expr") >>= NT("expr") + T("+") + NT("expr");
  g |= NT("start_$") >>= NT("start");
  
  LRParser p;
  bool pResult = p.setGrammar(&g);
  if(!pResult)
  {
    std::cerr << "Couldn't set grammar" << std::endl;
    return 1;
  }

  std::cout << g.toString();
  return 0;
}
