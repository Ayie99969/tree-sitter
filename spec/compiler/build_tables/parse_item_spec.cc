#include "compiler/compiler_spec_helper.h"
#include "compiler/build_tables/parse_item.h"
#include "compiler/syntax_grammar.h"
#include "compiler/helpers/rule_helpers.h"

using namespace rules;
using namespace build_tables;

START_TEST

describe("ParseItemSet::transitions())", [&]() {
  it("computes the ParseItemSet that would occur after consuming each lookahead symbol, along with its precedence", [&]() {
    SyntaxGrammar grammar{{
      SyntaxVariable("rule_0", VariableTypeNamed, {
        Production({
          {Symbol(11, true), 0, AssociativityNone, 101},
          {Symbol(12, true), 0, AssociativityNone, 102},
          {Symbol(13), 5, AssociativityNone, 103},
          {Symbol(14, true), 0, AssociativityNone, 104},
        }),
        Production({
          {Symbol(11, true), 0, AssociativityNone, 105},
          {Symbol(12, true), 0, AssociativityNone, 106},
          {Symbol(15), 6, AssociativityNone, 107},
        })
      }),
      SyntaxVariable("rule_1", VariableTypeNamed, {
        Production({
          {Symbol(15), 7, AssociativityNone, 109},
          {Symbol(16, true), 0, AssociativityNone, 110},
        })
      }),
      SyntaxVariable("rule_2", VariableTypeNamed, {
        Production({
          {Symbol(18, true), 0, AssociativityNone, 111},
        })
      })
    }, {}, {}};

    auto production = [&](int variable_index, int production_index) -> const Production & {
      return grammar.variables[variable_index].productions[production_index];
    };

    ParseItemSet item_set({

      // Two symbols into the first production for rule_0
      {
        ParseItem(Symbol(0), production(0, 0), 2),
        LookaheadSet({ Symbol(21, true) })
      },

      // Two symbols into the second production for rule_0
      {
        ParseItem(Symbol(0), production(0, 1), 2),
        LookaheadSet({ Symbol(21, true) })
      },

      // At the beginning of the first production for rule_1
      {
        ParseItem(Symbol(1), production(1, 0), 0),
        LookaheadSet({ Symbol(22, true) })
      },

      // At the end of the first production for rule_2
      {
        ParseItem(Symbol(2), production(2, 0), 1),
        LookaheadSet({ Symbol(22, true) })
      }
    });

    AssertThat(item_set.transitions(), Equals(ParseItemSet::TransitionMap({

      // For the first item, symbol 13 is next, with precedence 5.
      {
        Symbol(13),
        {
          ParseItemSet({
            {
              ParseItem(Symbol(0), production(0, 0), 3),
              LookaheadSet({ Symbol(21, true) })
            }
          }),
          PrecedenceRange(5, 5)
        }
      },

      // For the second and third item, symbol 15 is next, with two different
      // precedence values.
      {
        Symbol(15),
        {
          ParseItemSet({
            {
              ParseItem(Symbol(0), production(0, 1), 3),
              LookaheadSet({ Symbol(21, true) })
            },
            {
              ParseItem(Symbol(1), production(1, 0), 1),
              LookaheadSet({ Symbol(22, true) })
            },
          }),
          PrecedenceRange(6, 7)
        }
      },

      // The third item is at the end of its production: no transitions.
    })));
  });
});

END_TEST
