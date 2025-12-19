#include "symbol_table.h"

void SymbolTable::RegisterKeyword(const std::string &keyword) {
  table_.insert({keyword, {SymbolCategory::Keyword, keyword}});
}

SymbolTable::SymbolTable() {
  RegisterKeyword("auto");
  RegisterKeyword("break");
  RegisterKeyword("case");
  RegisterKeyword("char");
  RegisterKeyword("const");

  RegisterKeyword("continue");
  RegisterKeyword("default");
  RegisterKeyword("do");
  RegisterKeyword("double");
  RegisterKeyword("else");

  RegisterKeyword("enum");
  RegisterKeyword("extern");
  RegisterKeyword("float");
  RegisterKeyword("for");
  RegisterKeyword("goto");

  RegisterKeyword("if");
  RegisterKeyword("int");
  RegisterKeyword("long");
  RegisterKeyword("register");
  RegisterKeyword("return");

  RegisterKeyword("short");
  RegisterKeyword("signed");
  RegisterKeyword("sizeof");
  RegisterKeyword("static");
  RegisterKeyword("struct");

  RegisterKeyword("switch");
  RegisterKeyword("typedef");
  RegisterKeyword("union");
  RegisterKeyword("unsigned");
  RegisterKeyword("void");

  RegisterKeyword("volatile");
  RegisterKeyword("while");

  // newly added for LL(1) parser
  RegisterKeyword("ID");
  RegisterKeyword("NUM");
  RegisterKeyword("then");
  // newly added for LL(1) parser
}

auto SymbolTable::Lookup(const std::string &symbol) const
    -> std::optional<SymbolEntry> {
  auto it = table_.find(symbol);
  if (it == table_.end()) {
    return std::nullopt;
  }

  return it->second;
}

void SymbolTable::InsertIdentifier(const std::string &identifier) {
  table_.insert({identifier, {SymbolCategory::Identifier, identifier}});
}
