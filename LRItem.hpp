#ifndef _LRITEM_HPP_
#define _LRITEM_HPP_

#include "SymbolList.hpp"
#include "global.hpp"

#include <stddef.h>
#include <set>

class Production;

/********************----- CLASS: LRItem -----********************/
class LRItem
{
public:
  LRItem(Production const * const i_productionPtr, size_t const i_rightPosition, SymbolList const &i_lookahead);
  LRItem(Production const * const i_productionPtr, size_t const i_rightPosition);

  CompareResult compare(LRItem const &i_otherItem) const;

  SymbolList const &lookahead() const;
  Production const &production() const;
  size_t rightPosition() const;

  std::string toString() const;

  bool operator <(LRItem const &i_otherItem) const;
  bool operator ==(LRItem const &i_otherItem) const;
  bool operator >(LRItem const &i_otherItem) const;
private:
  Production const *m_productionPointer;
  size_t m_rightPosition;
  SymbolList m_lookahead;
  size_t m_iteration;
};
/**************************************************/

/********************----- Types -----********************/
typedef std::set<LRItem> LRItemSet;
typedef std::vector<LRItemSet> LRItemSetVector;
/**************************************************/

/********************----- Helper Functions -----********************/
void printItemSet(std::string const &i_name, LRItemSet const &i_itemSet);
void printItemSetVector(std::string const &i_name, std::vector<LRItemSet> const &i_itemSetVector);
/**************************************************/

#endif /* _LRITEM_HPP_ */
