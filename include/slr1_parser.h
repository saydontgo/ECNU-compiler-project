#include "error_reporter.h"
#include "slr1_analyzer.h"
#include "token.h"
#include <map>
#include <memory>
#include <stack>
class SLR1Parser {
public:
  auto ParsingTokens(std::shared_ptr<const TokenStream> tokens)
      -> std::stack<std::string>;
  void PrintErrors();
  void PrintOutput();

private:
  std::shared_ptr<SLR1Analyzer> ana_;
  std::shared_ptr<ErrorReporter> reporter_;
  std::map<std::pair<int, std::string>, std::string> action_table_;
  std::map<std::pair<int, std::string>, int> goto_table_;
};