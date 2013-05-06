#ifndef _SYMBOLLIST_HPP_
#define _SYMBOLLIST_HPP_

#include "Symbol.hpp"
#include "global.hpp"

#include <vector>

/********************----- CLASS: SymbolList -----********************/
class SymbolList
{
public:
  SymbolList(Symbol &&i_symbol);
  SymbolList(Symbol const &i_symbol);
  SymbolList(SymbolList &&i_symbolList);
  SymbolList(SymbolList const &i_symbolList);
  SymbolList();
  virtual ~SymbolList();

  void append(SymbolList const &i_symbolList);
  void append(SymbolList &&i_symbolList);

  void clear();

  CompareResult compare(SymbolList const &i_otherList) const;
  bool containsEpsilon() const;

  size_t count() const;

  Symbol const &get(size_t const i_index) const;

  bool isEmpty() const;

  std::string toString() const;

  Symbol const &operator [](size_t const i_index) const;
  bool operator <(SymbolList const &i_symbolList) const;
  bool operator==(SymbolList const &i_symbolList) const;
  bool operator!=(SymbolList const &i_symbolList) const;
  bool operator >(SymbolList const &i_symbolList) const;
  SymbolList &operator += (SymbolList &&i_symbolList);
  SymbolList &operator += (SymbolList const &i_symbolList);
private:
  SymbolList &operator =(SymbolList &&i_symbolList)=delete;
  SymbolList &operator =(SymbolList const &i_symbolList)=delete;
  enum class Flags
  {
    F_EPSILON=(1<<0),
    F_DEFAULT=0,
  };

  Flags m_flags;
  Symbol * m_symbolArray;
  size_t m_symbolCount;
};
/**************************************************/

/********************----- Operators -----********************/
SymbolList operator +(SymbolList &&i_A, SymbolList &&i_B);
/**************************************************/

#endif /* _SYMBOLLIST_HPP_ */
