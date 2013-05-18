#include "LRItem.hpp"
#include "Production.hpp"

#include <stddef.h>

#include <memory>

/********************----- CLASS: LRItem -----********************/
LRItem::LRItem(Production const * const i_productionPointer, size_t const i_rightPosition, SymbolList const &i_lookahead, size_t const i_iteration)
:m_productionPointer(i_productionPointer), m_rightPosition(i_rightPosition), m_lookahead(i_lookahead), m_iteration(i_iteration)
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

size_t LRItem::iteration() const
{
  return m_iteration;
}

SymbolList const &LRItem::lookahead() const
{
  return m_lookahead;
}

Production const &LRItem::production() const
{
  if(m_productionPointer == nullptr)
  {
    throw std::bad_weak_ptr();
  }

  return (*m_productionPointer);
}

size_t LRItem::rightPosition() const
{
  return m_rightPosition;
}

std::string LRItem::toString() const
{
  std::string outputString;

  outputString += "[";
  outputString += m_productionPointer->left().toString();
  outputString += " ::= ";
  outputString += m_productionPointer->right().sublist(0, m_rightPosition).toString();
  outputString += " . ";
  if(m_rightPosition < m_productionPointer->right().count())
  {
    outputString += m_productionPointer->right().sublist(m_rightPosition).toString();
  }
  outputString += " , ";
  outputString += m_lookahead.toString();
  outputString += "]";

  return outputString;
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
