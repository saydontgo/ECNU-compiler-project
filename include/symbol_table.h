#pragma once
#include <iostream>
#include <optional>
#include <unordered_map>

enum class SymbolCategory {
  Keyword,
  Identifier,
};

struct SymbolEntry {
  SymbolCategory type_;
  std::string name_;
};

class SymbolTable {
public:
  SymbolTable();

  // return the entry if the symbol exists.
  auto Lookup(const std::string &symbol) const -> std::optional<SymbolEntry>;

  // insert identifier
  void InsertIdentifier(const std::string &identifier);

private:
  // register keyword
  void RegisterKeyword(const std::string &keyword);
  std::unordered_map<std::string, SymbolEntry> table_;
};