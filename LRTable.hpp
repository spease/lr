#ifndef _LRTABLE_HPP_
#define _LRTABLE_HPP_

#include "Grammar.hpp"
#include "LRAction.hpp"
#include "LRItem.hpp"
#include "LRState.hpp"
#include "Symbol.hpp"

#include <unordered_map>
#include <vector>

/********************----- CLASS: LRTable -----********************/
class LRTable
{
public:
  enum class Type
  {
    GLR,
    LALR,
    LR,
  };

  LRTable(LRTable::Type const i_type, Grammar const &i_grammar);

  LRAction action(LRState const &i_currentState, SymbolList const &i_token) const;
  LRState path(LRState const &i_currentState, SymbolList const &i_symbol) const;
protected:
  static LRItemSet buildItems(Grammar const &i_grammar);
  static LRItemSet closure(LRItemSet const &i_item, size_t const i_iteration, Grammar const &i_grammar);
  static LRItemSet computePaths(LRItemSet const &i_itemSet, size_t const i_iteration, Symbol const &i_symbol, Grammar const &i_grammar);

  void insertAction(LRState const &i_state, SymbolList const &i_symbolList, LRAction const &i_action);
  void insertPath(LRState const &i_state, SymbolList const &i_symbolList, LRState const &i_destinationState);

private:
  typedef std::unordered_multimap<SymbolList, LRAction> ActionRow;
  typedef std::unordered_multimap<SymbolList, LRState> PathRow;
  typedef std::vector<ActionRow> ActionTable;
  typedef std::vector<PathRow> PathTable;

  ActionTable m_actions;
  PathTable m_paths;
};
/**************************************************/

#endif /* _LRTABLE_HPP_ */
