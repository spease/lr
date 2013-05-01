#include "LRItem.hpp"

#include <stddef.h>

/********************----- CLASS: LRItem -----********************/
LRItem::LRItem(Production const * const i_productionPointer, size_t const i_rightPosition, SymbolList const &i_lookAhead)
:m_productionPointer(i_productionPointer), m_rightPosition(i_rightPosition), m_lookAhead(i_lookAhead)
{
}
/**************************************************/
