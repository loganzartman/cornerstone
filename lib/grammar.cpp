#include "grammar.h"
#include "macros.h"
#include "matcher.h"

auto operator<< (std::ostream& o, Grammar::Type t) -> std::ostream&
  { return o << t->name; }

Grammar::Grammar(Texp t)
  {
    CHECK(t.value == "Grammar", "value at root of given texp is not 'Grammar'");
    for (auto& child : t)
      {
        types.emplace_back(child.value, child[0]);
      }
  }

std::optional<Grammar::Type> Grammar::parseType(std::string_view s) const
  {
    auto iter = std::find_if(types.cbegin(), types.cend(), [s](const TypeRecord& tr) { return tr.name == s; });
    if (iter != types.cend())
      return iter;
    else
      return std::nullopt;
  }

const Texp& Grammar::getProduction(Type type)
  { return type->production; }

void UnionMatch(const Grammar& g,
                std::string_view parent_type_name,
                const Texp& texp, 
                const Texp& proof,
                std::vector<std::pair<std::string_view, std::function<void(const Texp&, const Texp&)>>> cases)
  {
    CHECK(g.parseType(parent_type_name), "parent choice '" + std::string(parent_type_name) + "' not in grammar")
    Grammar::Type texp_type = proof_type(g, proof, parent_type_name);
    for (auto& [case_name, case_f] : cases) 
      {
        Grammar::Type case_type = CHECK_UNWRAP(g.parseType(case_name), "case '" + std::string(case_name) + "' not in grammar.");
        if (case_type == texp_type)
          {
            case_f(texp, proof);
            return;
          }
      }
    CHECK(false, texp_type->name + " is unhandled in " + std::string(parent_type_name) + "()'s type switch");
  }