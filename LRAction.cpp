#include "LRAction.hpp"

#include <memory>

/********************----- CLASS: LRAction -----********************/
LRAction::LRAction(Production const * const i_production)
:m_production(i_production),m_state(0),m_type(LRAction::Type::REDUCE)
{
  if(m_production == nullptr)
  {
    throw std::bad_weak_ptr();
  }
}

LRAction::LRAction(LRState const i_state)
:m_production(nullptr),m_state(i_state),m_type(LRAction::Type::SHIFT)
{
}

LRAction::LRAction(LRAction::Type const i_type)
:m_production(nullptr),m_state(0),m_type(i_type)
{
}

bool LRAction::isAccept() const
{
  return (m_type == LRAction::Type::ACCEPT);
}

bool LRAction::isReduce() const
{
  return (m_type == LRAction::Type::REDUCE);
}

bool LRAction::isShift() const
{
  return (m_type == LRAction::Type::SHIFT);
}

Production const &LRAction::production() const
{
  if(m_production == nullptr)
  {
    throw std::bad_weak_ptr();
  }

  return (*m_production);
}

LRState const &LRAction::state() const
{
  return m_state;
}
/**************************************************/

/********************----- Helper Functions -----********************/
LRAction ACCEPT()
{
  return LRAction(LRAction::Type::ACCEPT);
}

LRAction REDUCE(Production const * const i_production)
{
  return LRAction(i_production);
}

LRAction SHIFT(LRState const i_state)
{
  return LRAction(i_state);
}
/**************************************************/

