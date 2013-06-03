#ifndef _SYMBOL_HPP_
#define _SYMBOL_HPP_

#include "global.hpp"

#include <set>
#include <stack>
#include <string>
#include <unordered_map>

class Symbol;

/********************----- CLASS: Symbol -----********************/
class Symbol
{
public:
  enum class Type
  {
    T_NONE=0,
    T_END,
    T_EPSILON,
    T_NONTERMINAL,
    T_TERMINAL
  };

  Symbol(Symbol::Type const &i_type, char const * const i_value=nullptr);
  Symbol(Symbol const &i_symbol);
  Symbol(Symbol &&i_symbol);
  virtual ~Symbol();

  CompareResult compare(Symbol const &i_symbol) const;

  bool isEND() const;
  bool isEpsilon() const;
  bool isNonterminal() const;
  bool isTerminal() const;

  size_t sizeBytes() const;

  std::string toString() const;

  bool operator <(Symbol const &i_otherSymbol) const;
  bool operator !=(Symbol const &i_otherSymbol) const;
  bool operator ==(Symbol const &i_otherSymbol) const;
  Symbol &operator=(Symbol &&i_symbol);
private:
  void clear();

  Symbol::Type m_type;
  uint8_t *m_value;
  size_t m_valueSizeBytes;
};
/**************************************************/

/********************----- Helpers -----********************/
Symbol END();
Symbol EPS();
Symbol NT(char const * const i_name);
Symbol T(char const * const i_value);
/**************************************************/

/********************----- Hash Function -----********************/
namespace std
{
  template <>
  struct hash<Symbol> : public unary_function<Symbol, size_t>
  {
    size_t operator ()(Symbol const &i_symbol) const
    {
      return i_symbol.sizeBytes();
    }
  };
}
/**************************************************/

/********************----- Types -----********************/
typedef std::set<Symbol> SymbolSet;
typedef std::unordered_map<Symbol, SymbolSet> SymbolMap;
typedef std::stack<Symbol> SymbolStack;
/**************************************************/

/********************----- Operators -----********************/
bool operator ==(SymbolMap const &i_A, SymbolMap const &i_B);
bool operator !=(SymbolMap const &i_A, SymbolMap const &i_B);
/**************************************************/

/********************----- Helper Functions -----********************/
void printSymbolMap(std::string const &i_name, SymbolMap const &i_map);
/**************************************************/

#endif  /* _SYMBOL_HPP_ */
