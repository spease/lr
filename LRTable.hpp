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

  std::string toString() const;
protected:
  static LRItemSetVector buildLALRItems(Grammar const &i_grammar);
  static LRItemSetVector buildLRItems(Grammar const &i_grammar);
  static LRItemSet closure(LRItemSet const &i_item, Grammar const &i_grammar);
  static LRItemSet computeKernelItems(LRItemSet const &i_kernelItems, LRItemSet const &i_nonkernelItems, Grammar const &i_grammar);
  static LRItemSet computeNonkernelItems(LRItemSet const &i_kernelItems, Grammar const &i_grammar);
  static LRItemSet computePaths(LRItemSet const &i_itemSet, Symbol const &i_symbol, Grammar const &i_grammar);

  void insertAction(LRState const &i_state, SymbolList const &i_symbolList, LRAction const &i_action);
  void insertPath(LRState const &i_state, SymbolList const &i_symbolList, LRState const &i_destinationState);

private:
  typedef std::unordered_multimap<SymbolList, LRAction> ActionRow;
  typedef std::unordered_multimap<SymbolList, LRState> PathRow;
  typedef std::vector<ActionRow> ActionTable;
  typedef std::vector<PathRow> PathTable;

  ActionTable m_actions;
  PathTable m_paths;
  LRTable::Type m_type;
};
/**************************************************/

#endif /* _LRTABLE_HPP_ */
