#ifndef _LRITEM_HPP_
#define _LRITEM_HPP_

#include "SymbolList.hpp"

#include <stddef.h>

class Production;

/********************----- CLASS: LRItem -----********************/
class LRItem
{
public:
  LRItem(Production const * const i_productionPtr, size_t const i_rightPosition, SymbolList const &i_lookAhead);
private:
  Production const *m_productionPointer;
  size_t m_rightPosition;
  SymbolList m_lookAhead;
};
/**************************************************/

#endif /* _LRITEM_HPP_ */
