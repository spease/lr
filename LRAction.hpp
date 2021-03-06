#ifndef _LRACTION_HPP_
#define _LRACTION_HPP_

#include "LRState.hpp"
#include "Production.hpp"

#include <set>

/********************----- CLASS: LRAction -----********************/
class LRAction
{
  friend LRAction ACCEPT();
  friend LRAction REDUCE(Production const * const i_production);
  friend LRAction SHIFT(LRState const i_state);
public:
  enum class Type
  {
    ACCEPT,
    REDUCE,
    SHIFT
  };

  bool isAccept() const;
  bool isReduce() const;
  bool isShift() const;

  Production const &production() const;
  LRState const &state() const;

  std::string toString() const;
  bool operator==(LRAction const &i_otherAction) const;
protected:
  LRAction(Production const * const i_production);
  LRAction(LRState const i_state);
  LRAction(Type const i_type);

private:
  Production const * m_production;
  LRState m_state;
  Type m_type;
};
/**************************************************/

/********************----- Helper Functions -----********************/
LRAction ACCEPT();
LRAction REDUCE(Production const * const i_production);
LRAction SHIFT(LRState const i_state);
/**************************************************/

/********************----- Types -----********************/
typedef std::set<LRAction> LRActionSet;
/**************************************************/

#endif /* _LRACTION_HPP_ */
