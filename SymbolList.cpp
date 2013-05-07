#include "SymbolList.hpp"
#include "global.hpp"

#include <limits>
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
SymbolList::SymbolList(SymbolList const &i_symbolList, size_t const i_position, size_t const i_length)
:m_flags(SymbolList::Flags::F_DEFAULT), m_symbolArray(nullptr), m_symbolCount(0)
{
  size_t copyLength = i_length;
  size_t copyPosition = i_position;

  if(copyPosition >= i_symbolList.count())
  {
    throw std::out_of_range(std::to_string(copyPosition));
  }

  if(i_length == std::numeric_limits<size_t>::max())
  {
    copyLength = i_symbolList.count()-copyPosition;
  }
  else if((copyPosition+copyLength) > i_symbolList.count())
  {
    throw std::out_of_range(std::to_string(copyPosition+copyLength));
  }

  m_symbolArray = reinterpret_cast<Symbol*>(::operator new(sizeof(Symbol)*copyLength));
  for(size_t i=0; i<copyLength; ++i)
  {
    Symbol const &otherListSymbol=i_symbolList[copyPosition+i];
    m_symbolArray = new(m_symbolArray) Symbol(otherListSymbol);
    if(otherListSymbol.isEpsilon())
    {
      m_flags = static_cast<SymbolList::Flags>(enum_value(m_flags) | enum_value(SymbolList::Flags::F_EPSILON));
    }
    ++m_symbolCount;
  }
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

CompareResult SymbolList::compare(SymbolList const &i_symbolList) const
{
  /***** Same thing *****/
  if(this == &i_symbolList)
  {
    return CompareResult::LESS;
  }

  /***** Check symbol count *****/
  if(m_symbolCount < i_symbolList.m_symbolCount)
  {
    return CompareResult::GREATER;
  }
  else if(m_symbolCount > i_symbolList.m_symbolCount)
  {
    return CompareResult::GREATER;
  }

  /***** Check symbols *****/
  for(size_t i=0; i<m_symbolCount; ++i)
  {
    CompareResult cr=this->get(i).compare(i_symbolList.get(i));
    if(cr == CompareResult::LESS)
    {
      return CompareResult::LESS;
    }
    else if(cr == CompareResult::GREATER)
    {
      return CompareResult::GREATER;
    }
  }

  /***** They are equal *****/
  return CompareResult::EQUAL;
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

SymbolList SymbolList::sublist(size_t const i_position, size_t const i_length) const
{
  return SymbolList(*this, i_position, i_length);
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

bool SymbolList::operator <(SymbolList const &i_symbolList) const
{
  return (this->compare(i_symbolList)==CompareResult::LESS);
}

bool SymbolList::operator >(SymbolList const &i_symbolList) const
{
  return (this->compare(i_symbolList)==CompareResult::GREATER);
}

bool SymbolList::operator ==(SymbolList const &i_symbolList) const
{
  return (this->compare(i_symbolList)==CompareResult::EQUAL);
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
