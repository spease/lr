#include "Symbol.hpp"

#include <string.h>
#include "global.hpp"

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

CompareResult Symbol::compare(Symbol const &i_otherSymbol) const
{
  if(this == &i_otherSymbol)
  {
    return CompareResult::EQUAL;
  }

  if(this->sizeBytes() < i_otherSymbol.sizeBytes())
  {
    return CompareResult::LESS;
  }
  else if(this->sizeBytes() > i_otherSymbol.sizeBytes())
  {
    return CompareResult::GREATER;
  }

  if(m_value == nullptr && i_otherSymbol.m_value != nullptr)
  {
    return CompareResult::LESS;
  }
  else if(m_value != nullptr && i_otherSymbol.m_value == nullptr)
  {
    return CompareResult::GREATER;
  }
  else if(m_value == i_otherSymbol.m_value)
  {
    return CompareResult::EQUAL;
  }

  int const memResult = memcmp(m_value, i_otherSymbol.m_value, m_valueSizeBytes);
  if(memResult < 0)
  {
    return CompareResult::LESS;
  }
  else if(memResult > 0)
  {
    return CompareResult::GREATER;
  }

  return CompareResult::EQUAL;
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
  bool shouldOutputValue = false;
  std::string outputValue;
  switch(m_type)
  {
    case Symbol::Type::T_END:
      outputValue += "END";
      break;
    case Symbol::Type::T_EPSILON:
      outputValue += "EPS";
      break;
    case Symbol::Type::T_NONTERMINAL:
      outputValue += "NT";
      shouldOutputValue = true;
      break;
    case Symbol::Type::T_TERMINAL:
      outputValue += "T";
      shouldOutputValue = true;
      break;
    default:
      outputValue += "?";
  }

  if(shouldOutputValue)
  {
    outputValue += "(";
    outputValue += std::string(reinterpret_cast<char const * const>(m_value), m_valueSizeBytes);
    outputValue += ")";
  }

  return outputValue;
}

bool Symbol::operator <(Symbol const &i_otherSymbol) const
{
  return (this->compare(i_otherSymbol) == CompareResult::LESS);
}

bool Symbol::operator !=(Symbol const &i_otherSymbol) const
{
  return !(this->compare(i_otherSymbol) == CompareResult::EQUAL);
}

bool Symbol::operator ==(Symbol const &i_otherSymbol) const
{
  return (this->compare(i_otherSymbol) == CompareResult::EQUAL);
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
