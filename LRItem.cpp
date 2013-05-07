#include "LRItem.hpp"

#include <stddef.h>

#include <memory>

/********************----- CLASS: LRItem -----********************/
LRItem::LRItem(Production const * const i_productionPointer, size_t const i_rightPosition, SymbolList const &i_lookahead)
:m_productionPointer(i_productionPointer), m_rightPosition(i_rightPosition), m_lookahead(i_lookahead)
{
}

CompareResult LRItem::compare(LRItem const &i_otherItem) const
{
  //This operator is used for insertion into a set
  //Therefore I've tried to order the comparisons such that
  //they sort in an order that makes sense for parsing
  //to minimize the number of comparisons made

  /***** 0: If same item *****/
  if(this == &i_otherItem)
  {
    return CompareResult::EQUAL;
  }

  /***** 1: Check position *****/
  if(m_rightPosition < i_otherItem.m_rightPosition)
  {
    return CompareResult::LESS;
  }
  else if(m_rightPosition > i_otherItem.m_rightPosition)
  {
    return CompareResult::GREATER;
  }

  /***** 2: Check production *****/
  if(m_productionPointer < i_otherItem.m_productionPointer)
  {
    return CompareResult::LESS;
  }
  else if(m_productionPointer > i_otherItem.m_productionPointer)
  {
    return CompareResult::GREATER;
  }

  /***** 3: Check lookahead *****/
  if(m_lookahead < i_otherItem.m_lookahead)
  {
    return CompareResult::LESS;
  }
  else if(m_lookahead > i_otherItem.m_lookahead)
  {
    return CompareResult::GREATER;
  }

  /***** They are equal *****/
  return CompareResult::EQUAL;
}

SymbolList const &LRItem::lookahead() const
{
  return m_lookahead;
}

Production const &LRItem::production() const
{
  if(m_productionPointer == nullptr)
  {
    throw std::runtime_error("Null pointer.");
  }

  return (*m_productionPointer);
}

size_t LRItem::rightPosition() const
{
  return m_rightPosition;
}

bool LRItem::operator <(LRItem const &i_otherItem) const
{
  return (this->compare(i_otherItem) == CompareResult::LESS);
}

bool LRItem::operator ==(LRItem const &i_otherItem) const
{
  return (this->compare(i_otherItem) == CompareResult::EQUAL);
}

bool LRItem::operator >(LRItem const &i_otherItem) const
{
  return (this->compare(i_otherItem) == CompareResult::GREATER);
}
/**************************************************/
