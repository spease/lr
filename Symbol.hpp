#ifndef _SYMBOL_HPP_
#define _SYMBOL_HPP_

#include <string>

class Symbol;

/********************----- CLASS: Symbol -----********************/
class Symbol
{
public:
  enum class Type
  {
    T_END,
    T_EPSILON,
    T_NONTERMINAL,
    T_TERMINAL
  };

  Symbol(Symbol::Type const &i_type, char const * const i_value=nullptr);
  Symbol(Symbol const &i_symbol);
  Symbol(Symbol &&i_symbol);
  virtual ~Symbol();

  bool isEND() const;
  bool isEpsilon() const;
  bool isNonterminal() const;
  bool isTerminal() const;

  size_t sizeBytes() const;

  std::string toString() const;

  bool operator <(Symbol const &i_otherSymbol) const;
  bool operator !=(Symbol const &i_otherSymbol) const;
  bool operator ==(Symbol const &i_otherSymbol) const;

  static Symbol END();
  static Symbol EPS();
  static Symbol NT(char const * const i_name);
  static Symbol T(char const * const i_value);
private:
  Symbol &operator =(Symbol const &)=delete;
  Symbol &operator =(Symbol &&)=delete;
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

#endif  /* _SYMBOL_HPP_ */
