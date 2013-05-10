#ifndef _LEXTEXT_HPP_
#define _LEXTEXT_HPP_

#include "Lex.hpp"
#include "Symbol.hpp"

#include <fstream>
#include <string>

/********************----- CLASS: LexText -----********************/
class LexText : public Lex
{
public:
  LexText(std::string const &i_filepath);
  virtual ~LexText();

  virtual Symbol pop() override;
private:
  std::ifstream m_input;
};
/**************************************************/

#endif /* _LEXTEXT_HPP_ */
