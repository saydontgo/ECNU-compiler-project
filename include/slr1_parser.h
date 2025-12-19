#include "error_reporter.h"
#include "slr1_analyzer.h"
#include "token.h"
#include <map>
#include <memory>
#include <stack>
#include <iomanip>

extern std::string EMPTYCH;
class SLR1Parser {
public:
  SLR1Parser(std::vector<std::string> &prods);
  auto ParsingTokens(std::shared_ptr<const TokenStream> tokens)
      -> std::stack<std::string>;
  void PrintErrors();
  void PrintTables();
  void PrintOutput(std::stack<std::string> &results);

private:
  std::shared_ptr<SLR1Analyzer> ana_;
  std::shared_ptr<ErrorReporter> reporter_;
  std::map<std::pair<int, std::string>, std::string> action_table_;
  std::map<std::pair<int, std::string>, int> goto_table_;
};