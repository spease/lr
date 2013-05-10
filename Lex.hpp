#ifndef _LEX_HPP_
#define _LEX_HPP_

#include "Symbol.hpp"

/********************----- CLASS: Lex -----********************/
class Lex
{
public:
  virtual Symbol pop()=0;
private:
};
/**************************************************/

#endif /* _LEX_HPP_ */
