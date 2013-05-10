#ifndef _LEXTEXT_HPP_
#define _LEXTEXT_HPP_

#include "Symbol.hpp"

#include <fstream>
#include <string>

/********************----- CLASS: LexText -----********************/
class LexText
{
public:
  LexText(std::string const &i_filepath);
  virtual ~LexText();

  Symbol pop();
private:
  std::ifstream m_input;
};
/**************************************************/

#endif /* _LEXTEXT_HPP_ */
