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
  this->clear();
}

void Symbol::clear()
{
  if(m_value != nullptr)
  {
    delete[] m_value;
    m_valueSizeBytes = 0;
  }
  m_type = Symbol::Type::T_NONE;
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
    outputValue += std::string(reinterpret_cast<char const * const>(m_value), m_valueSizeBytes-1);
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

Symbol &Symbol::operator =(Symbol &&i_symbol)
{
  if(this == &i_symbol)
  {
    return (*this);
  }

  this->clear();
  m_type = i_symbol.m_type;
  m_value = i_symbol.m_value;
  m_valueSizeBytes = i_symbol.m_valueSizeBytes;

  i_symbol.m_type = Symbol::Type::T_NONE;
  i_symbol.m_value = nullptr;
  i_symbol.m_valueSizeBytes = 0;

  return (*this);
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

/********************----- Helper Functions -----********************/
#include <iostream>
void printSymbolMap(std::string const &i_name, SymbolMap const &i_map)
{
  std::cout << "===== " << i_name << " =====" << std::endl;
  for(SymbolMap::const_iterator mit=i_map.begin(); mit!=i_map.end(); ++mit)
  {
    std::cout << "\t" << mit->first.toString() << " ==> ";

    SymbolSet const &ss=mit->second;
    for(SymbolSet::const_iterator sit=ss.begin(); sit!=ss.end(); ++sit)
    {
      std::cout << sit->toString() << " ";
    }
    std::cout << std::endl;
  }
  std::cout << "==================================================" << std::endl;
}
/**************************************************/

/********************----- Operators -----********************/
bool operator ==(SymbolMap const &i_A, SymbolMap const &i_B)
{
  if(i_A.size() != i_B.size())
  {
    return false;
  }

  for(SymbolMap::const_iterator ait=i_A.begin(); ait!=i_A.end(); ++ait)
  {
    SymbolMap::const_iterator bit=i_B.find(ait->first);
    if(bit==i_B.end() || ait->second != bit->second)
    {
      return false;
    }
  }

  return true;
}

bool operator !=(SymbolMap const &i_A, SymbolMap const &i_B)
{
  return !(i_A == i_B);
}
/**************************************************/
