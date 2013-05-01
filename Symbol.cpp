#include "Symbol.hpp"

#include <string.h>

/********************----- CLASS: Symbol -----********************/
Symbol::Symbol(Symbol::Type const &i_type, char const * const i_value)
:m_type(i_type), m_value(nullptr), m_valueSizeBytes(0)
{
  if(i_value != nullptr)
  {
    size_t const inputValueLength = strlen(i_value);
    if(inputValueLength > 0)
    {
      m_value = new uint8_t[inputValueLength+1];
      memcpy(m_value, i_value, inputValueLength+1);
      m_valueSizeBytes = inputValueLength+1;
    }
  }
}

Symbol::Symbol(Symbol const &i_symbol)
:m_type(i_symbol.m_type), m_value(nullptr), m_valueSizeBytes(0)
{
  if(i_symbol.m_valueSizeBytes > 0)
  {
    m_value = new uint8_t[i_symbol.m_valueSizeBytes];
    memcpy(m_value, i_symbol.m_value, i_symbol.m_valueSizeBytes);
    m_valueSizeBytes = i_symbol.m_valueSizeBytes;
  }
}

Symbol::Symbol(Symbol &&i_symbol)
:m_type(i_symbol.m_type), m_value(i_symbol.m_value), m_valueSizeBytes(i_symbol.m_valueSizeBytes)
{
  i_symbol.m_value = nullptr;
  i_symbol.m_valueSizeBytes = 0;
}

Symbol::~Symbol()
{
  if(m_value != nullptr)
  {
    delete[] m_value;
    m_valueSizeBytes = 0;
  }
}

bool Symbol::isEND() const
{
  return (m_type == Symbol::Type::T_END);
}

bool Symbol::isEpsilon() const
{
  return (m_type == Symbol::Type::T_EPSILON);
}

bool Symbol::isNonterminal() const
{
  return (m_type == Symbol::Type::T_NONTERMINAL);
}

bool Symbol::isTerminal() const
{
  return (m_type == Symbol::Type::T_TERMINAL || m_type == Symbol::Type::T_EPSILON || m_type == Symbol::Type::T_END);
}

size_t Symbol::sizeBytes() const
{
  return m_valueSizeBytes;
}

std::string Symbol::toString() const
{
  std::string outputValue;
  switch(m_type)
  {
    case Symbol::Type::T_EPSILON:
      outputValue += "EPS";
      break;
    case Symbol::Type::T_NONTERMINAL:
      outputValue += "NT";
      break;
    case Symbol::Type::T_TERMINAL:
      outputValue += "T";
      break;
    default:
      outputValue += "?";
  }

  if(!this->isEpsilon())
  {
    outputValue += "(";
    outputValue += std::string(reinterpret_cast<char const * const>(m_value), m_valueSizeBytes);
    outputValue += ")";
  }

  return outputValue;
}

bool Symbol::operator <(Symbol const &i_otherSymbol) const
{
  if(this->sizeBytes() < i_otherSymbol.sizeBytes())
  {
    return true;
  }
  else if(this->sizeBytes() > i_otherSymbol.sizeBytes())
  {
    return false;
  }
  else if(m_value == nullptr && i_otherSymbol.m_value != nullptr)
  {
    return true;
  }
  else if(m_value != nullptr && i_otherSymbol.m_value == nullptr)
  {
    return false;
  }
  else if(m_value == i_otherSymbol.m_value)
  {
    return false;
  }

  int const memResult = memcmp(m_value, i_otherSymbol.m_value, m_valueSizeBytes);
  return (memResult < 0);
}

bool Symbol::operator !=(Symbol const &i_otherSymbol) const
{
  return !(*this == i_otherSymbol);
}

bool Symbol::operator ==(Symbol const &i_otherSymbol) const
{
  if(i_otherSymbol.m_type != m_type)
  {
    return false;
  }
  if(i_otherSymbol.m_valueSizeBytes != m_valueSizeBytes)
  {
    return false;
  }
  if(memcmp(i_otherSymbol.m_value, m_value, m_valueSizeBytes))
  {
    return false;
  }

  return true;
}

/**************************************************/

/********************----- Helpers -----********************/
Symbol END()
{
  return Symbol(Symbol::Type::T_END);
}

Symbol EPS()
{
  return Symbol(Symbol::Type::T_EPSILON);
}

Symbol NT(char const * const i_name)
{
  return Symbol(Symbol::Type::T_NONTERMINAL, i_name);
}

Symbol T(char const * const i_value)
{
  return Symbol(Symbol::Type::T_TERMINAL, i_value);
}
/**************************************************/
