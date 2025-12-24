#include "test.h"
#include "ll1_analyzer.h"

void test_1(std::string &prog) {
  LexicalAnalyzer lexer(prog);
  auto tokens = lexer.Tokenize();
  lexer.PrintErrors();
}

void test_2(std::string &prog) {
  LexicalAnalyzer lexer(prog);
  auto tokens = lexer.Tokenize();
  lexer.PrintErrors();
  tokens->PrintAll();
}

void test_5() {
  std::string prog = "";
  LexicalAnalyzer lexer(prog);
  LL1Analyzer ana(lexer.GetTable());
  auto res = ana.BuildTable();
  LL1Parser parser(lexer.GetTable());

  for (int i = static_cast<int>(NonTerminalType::threshold) + 1;
       i < static_cast<int>(NonTerminalType::end); i++) {
    auto nonterminal = parser.GetName(i);
    std::cout << nonterminal << ": " << std::endl;
    auto prods = res[static_cast<NonTerminalType>(i)];
    for (const auto &prod : prods) {
      std::cout << "\t" << parser.GetName(prod.first) << ": " << std::endl;
      std::cout << "\t\t" << nonterminal << " -> ";
      for (const auto &id : prod.second) {
        std::string str = "eplison";
        if (id != static_cast<int>(NonTerminalType::end)) {
          str = parser.GetName(id);
        }
        std::cout << str << " ";
      }
      std::cout << std::endl;
    }
    std::cout << std::endl;
  }
}

void test_7(std::string &prog) {
  LexicalAnalyzer lexer(prog);
  auto tokens = lexer.Tokenize();
  lexer.PrintErrors();
  tokens->PrintAll();

  // LL1 Parser
  LL1Parser parser(lexer.GetTable());
  auto trees = parser.ParseTokens(tokens);
  parser.PrintErrors();
  trees->PrintOutput();
  std::cout << "\n抽象化之后的语法树：\n";
  trees->Abstract();
  trees->PrintOutput();
}

void test_9(std::vector<std::string> &prods) {
  std::map<std::pair<int, std::string>, std::string> action_table;
  std::map<std::pair<int, std::string>, int> goto_table;

  auto res = SLR1Analyzer(prods);
  res.CreateSLRTable(action_table, goto_table);
  std::cout << "LR分析表：" << std::endl;

  auto x = res.GetTerminals();
  auto states = res.GetStates();
  auto y = res.GetNonT();
  x.push_back("#");

  // TODO: change the style
  std::cout << "---------------------------------------------------------------"
               "----action "
               "table----------------------------------------------------------"
               "---------"
            << std::endl;
  std::cout.setf(std::ios::left);
  for (auto it1 = x.begin(); it1 != x.end(); it1++) {
    if (it1 == x.begin())
      std::cout << std::setw(10) << " ";
    std::cout << std::setw(8) << *it1;
  }
  std::cout << std::endl;

  for (size_t i = 0; i < states.size(); i++) {
    std::cout << std::setw(10) << i;

    for (std::string t : x) {

      if (!action_table.empty()) {
        std::pair<int, std::string> title(i, t);
        std::cout << std::setw(8) << action_table[title];
      }

      else
        std::cout << std::setw(8);
    }
    std::cout << std::endl;
  }
  std::cout << std::endl;

  // print goto table
  y.erase(y.begin());

  std::cout << "---------------------------------------------------------------"
               "----goto "
               "table----------------------------------------------------------"
               "---------"
            << std::endl;
  std::cout.setf(std::ios::left);

  for (auto it1 = y.begin(); it1 != y.end(); it1++) {
    if (it1 == y.begin())
      std::cout << std::setw(10) << "";

    std::cout << std::setw(15) << *it1;
  }
  std::cout << std::endl;

  for (size_t i = 0; i < states.size(); i++) {
    std::cout << std::setw(10) << i;

    for (std::string t : y) {
      std::pair<int, std::string> title(i, t);

      if (goto_table[title] != 0) {
        std::cout << std::setw(15) << goto_table[title];
      } else
        std::cout << std::setw(15) << "";
    }
    std::cout << std::endl;
  }
}