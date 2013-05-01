#include "SymbolList.hpp"
#include "global.hpp"

#include <stdexcept>

/********************----- CLASS: SymbolList -----********************/
SymbolList::SymbolList(Symbol &&i_symbol)
:m_flags(SymbolList::Flags::F_DEFAULT), m_symbolArray(nullptr), m_symbolCount(0)
{
  if(i_symbol.isEpsilon())
  {
    m_flags = static_cast<SymbolList::Flags>(enum_value(m_flags) | enum_value(SymbolList::Flags::F_EPSILON));
  }
  m_symbolArray = reinterpret_cast<Symbol*>(::operator new(sizeof(Symbol)));
  m_symbolArray = new(m_symbolArray) Symbol(std::forward<Symbol>(i_symbol));
  m_symbolCount = 1;
}

SymbolList::SymbolList(Symbol const &i_symbol)
:m_flags(SymbolList::Flags::F_DEFAULT), m_symbolArray(nullptr), m_symbolCount(0)
{
  if(i_symbol.isEpsilon())
  {
    m_flags = static_cast<SymbolList::Flags>(enum_value(m_flags) | enum_value(SymbolList::Flags::F_EPSILON));
  }

  m_symbolArray = reinterpret_cast<Symbol*>(::operator new(sizeof(Symbol)));
  m_symbolArray = new(m_symbolArray) Symbol(i_symbol);
  m_symbolCount = 1;
}

SymbolList::SymbolList(SymbolList &&i_symbolList)
:m_flags(i_symbolList.m_flags), m_symbolArray(i_symbolList.m_symbolArray), m_symbolCount(i_symbolList.m_symbolCount)
{
  /***** Remove ownership from other list *****/
  i_symbolList.m_symbolArray = nullptr;
  i_symbolList.m_symbolCount = 0;
}

SymbolList::SymbolList(SymbolList const &i_symbolList)
:m_flags(i_symbolList.m_flags), m_symbolArray(nullptr), m_symbolCount(0)
{
  size_t const nextSymbolCount = i_symbolList.count();
  Symbol *const nextSymbolArray = reinterpret_cast<Symbol*>(::operator new(sizeof(Symbol) * nextSymbolCount));

  /***** Copy remote array *****/
  for(size_t i=0; i<i_symbolList.m_symbolCount; ++i)
  {
    new (&nextSymbolArray[i]) Symbol(i_symbolList.m_symbolArray[i]);
  }

  /***** Finish *****/
  m_symbolArray = nextSymbolArray;
  m_symbolCount = nextSymbolCount;
}

SymbolList::SymbolList()
:m_symbolArray(nullptr), m_symbolCount(0)
{
}

SymbolList::~SymbolList()
{
  this->clear();
}

void SymbolList::append(SymbolList &&i_symbolList)
{
  SymbolList::Flags nextFlags = static_cast<SymbolList::Flags>(enum_value(m_flags) | enum_value(i_symbolList.m_flags));
  size_t const nextSymbolCount = m_symbolCount+i_symbolList.count();
  Symbol *const nextSymbolArray = reinterpret_cast<Symbol*>(::operator new(sizeof(Symbol) * nextSymbolCount));

  /***** Move local array *****/
  for(size_t i=0; i<m_symbolCount; ++i)
  {
    new (&nextSymbolArray[i]) Symbol(std::move(m_symbolArray[i]));
  }

  /***** Move remote array *****/
  for(size_t i=0; i<i_symbolList.m_symbolCount; ++i)
  {
    new (&nextSymbolArray[m_symbolCount+i]) Symbol(std::move(i_symbolList.m_symbolArray[i]));
  }

  /***** Finalize local array *****/
  this->clear();
  m_flags = nextFlags;
  m_symbolArray = nextSymbolArray;
  m_symbolCount = nextSymbolCount;

  /***** Finalize remote array *****/
  i_symbolList.clear();
}

void SymbolList::append(SymbolList const &i_symbolList)
{
  SymbolList::Flags nextFlags = static_cast<SymbolList::Flags>(enum_value(m_flags) | enum_value(i_symbolList.m_flags));
  size_t const nextSymbolCount = m_symbolCount+i_symbolList.count();
  Symbol *const nextSymbolArray = reinterpret_cast<Symbol*>(::operator new(sizeof(Symbol) * nextSymbolCount));

  /***** Move local array *****/
  for(size_t i=0; i<m_symbolCount; ++i)
  {
    new (&nextSymbolArray[i]) Symbol(std::move(m_symbolArray[i]));
  }

  /***** Copy remote array *****/
  for(size_t i=0; i<i_symbolList.m_symbolCount; ++i)
  {
    new (&nextSymbolArray[m_symbolCount+i]) Symbol(i_symbolList.m_symbolArray[i]);
  }

  /***** Finalize local array *****/
  this->clear();
  m_flags = nextFlags;
  m_symbolArray = nextSymbolArray;
  m_symbolCount = nextSymbolCount;
}

void SymbolList::clear()
{
  m_flags = SymbolList::Flags::F_DEFAULT;
  if(m_symbolArray != nullptr)
  {
    for(size_t i=0; i<m_symbolCount; ++i)
    {
      m_symbolArray[i].~Symbol();
    }
    m_symbolCount = 0;

    ::operator delete(m_symbolArray);
    m_symbolArray = nullptr;
  }
}

bool SymbolList::containsEpsilon() const
{
  if(m_symbolCount < 1 || (enum_value(m_flags)&enum_value(SymbolList::Flags::F_EPSILON)))
  {
    return true;
  }

  return false;
}

size_t SymbolList::count() const
{
  return m_symbolCount;
}

Symbol const &SymbolList::get(size_t const i_index) const
{
  if(i_index >= m_symbolCount)
  {
    throw std::out_of_range(std::to_string(i_index));
  }

  return m_symbolArray[i_index];
}

bool SymbolList::isEmpty() const
{
  return (this->count() == 0);
}

std::string SymbolList::toString() const
{
  std::string outputString;
  for(size_t i=0; i<this->count(); ++i)
  {
    if(i!=0) outputString += " ";
    outputString += this->get(i).toString();
  }

  return outputString;
}

Symbol const &SymbolList::operator [](size_t const i_index) const
{
  return this->get(i_index);
}

bool SymbolList::operator ==(SymbolList const &i_symbolList) const
{
  /***** Check # of symbols *****/
  if(m_symbolCount != i_symbolList.m_symbolCount)
  {
    return false;
  }

  /***** Check symbol size *****/
  for(size_t i=0; i<m_symbolCount; ++i)
  {
    Symbol const &leftSymbol = this->get(i);
    Symbol const &rightSymbol = i_symbolList.get(i);

    if(leftSymbol.sizeBytes() != rightSymbol.sizeBytes())
    {
      return false;
    }
  }

  /***** Check symbol contents *****/
  for(size_t i=0; i<m_symbolCount; ++i)
  {
    if(this->get(i) != i_symbolList.get(i))
    {
      return false;
    }
  }

  return true;
}

bool SymbolList::operator !=(SymbolList const &i_symbolList) const
{
  return !(*this == i_symbolList);
}

SymbolList &SymbolList::operator +=(SymbolList &&i_symbolList)
{
  this->append(std::forward<SymbolList>(i_symbolList));

  return *this;
}

SymbolList &SymbolList::operator +=(SymbolList const &i_symbolList)
{
  this->append(i_symbolList);

  return *this;
}
/**************************************************/

/********************----- Operators -----********************/
SymbolList operator +(SymbolList &&i_A, SymbolList &&i_B)
{
  SymbolList returnList(std::forward<SymbolList>(i_A));
  returnList.append(std::forward<SymbolList>(i_B));
  return returnList;
}
/**************************************************/
