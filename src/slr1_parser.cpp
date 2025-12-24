#include "slr1_parser.h"

SLR1Parser::SLR1Parser(std::vector<std::string> &prods)
    : ana_(std::make_shared<SLR1Analyzer>(prods)),
      reporter_(std::make_shared<ErrorReporter>()) {
  ana_->CreateSLRTable(action_table_, goto_table_);
}
auto SLR1Parser::ParsingTokens(std::shared_ptr<const TokenStream> tokens)
    -> std::stack<std::string> {
  std::stack<std::string> Analysis;
  std::stack<std::string> results;

  Analysis.push("#");
  Analysis.push("0");

  int token_index = 0;
  auto token = tokens->TokenAt(token_index);
  int flag = 0;
  int report = 0;
  while (1) {
    std::pair<int, std::string> title(stoi(Analysis.top()), token.lexeme_);
    std::string res = action_table_[title];
    // std::cout << res << std::endl;
    // shift
    if (res[0] == 's') {
      int state = stoi(res.substr(1));
      Analysis.push(token.lexeme_);
      Analysis.push(std::to_string(state));
      if (token_index + 1 >= int(tokens->Size())) {
        token = Token({-1, -1, "#", token.line_, -1});
        continue;
      }
      token = tokens->TokenAt(++token_index);
    }
    // reduce
    else if (res[0] == 'r') {
      int pos = stoi(res.substr(1));
      std::string left;
      int b = 2 * ana_->RightNum(left, pos);
      results.push(ana_->GetResult(pos));
      while (b > 0) {
        Analysis.pop();
        b--;
      }

      int s1 = stoi(Analysis.top());
      Analysis.push(left);
      std::pair<int, std::string> t(s1, left);
      Analysis.push(std::to_string(goto_table_[t]));
    } else if (res[0] == 'a') {
      return results;
    } else if (res.length() == 0 && token.lexeme_ != EMPTYCH) {
      if (!flag) {
        token_index--;
      }
      flag = 0;
      token = Token({-1, -1, EMPTYCH, token.line_, -1});
    } else {
      if (!report)
        reporter_->Report(ErrorLevel::Error, ErrorCode::MissingSymbol, 4,
                          token.col_, ";");
      flag = !flag;
      report = 1;
      token = Token({-1, -1, ";", 4, -1});
    }
  }
}

void SLR1Parser::PrintErrors() { reporter_->PrintLL1(); }

void SLR1Parser::PrintTables() {
  ana_->CreateSLRTable(action_table_, goto_table_);
  std::cout << "SLR Table：" << std::endl;

  auto x = ana_->GetTerminals();
  auto states = ana_->GetStates();
  auto y = ana_->GetNonT();
  x.push_back("#");

  // TODO: visualize the two table
  std::cout << "action  table:" << std::endl;
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

      if (!action_table_.empty()) {
        std::pair<int, std::string> title(i, t);
        std::cout << std::setw(8) << action_table_[title];
      }

      else
        std::cout << std::setw(8);
    }
    std::cout << std::endl;
  }
  std::cout << std::endl;

  // print goto table
  y.erase(y.begin());

  std::cout << "goto table" << std::endl;
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

      if (goto_table_[title] != 0) {
        std::cout << std::setw(15) << goto_table_[title];
      } else
        std::cout << std::setw(15) << "";
    }
    std::cout << std::endl;
  }
}

std::string GetLeft(std::string s) {
  std::string r;
  for (size_t i = 0; i < s.length(); i++) {
    if (s[i] == '-') {
      return r;
    } else {
      r.push_back(s[i]);
    }
  }
  return r;
}

std::string GetRight(std::string s) {
  std::string r;
  bool f = false;
  for (size_t i = 0; i < s.length(); i++) {
    if (f) {
      r.push_back(s[i]);
    }
    if (s[i] == '-' && !f) {
      f = true;
      i++;
    }
  }
  return r;
}

void SLR1Parser::PrintOutput(std::stack<std::string> &results) {
  std::string current;
  std::vector<std::string> curL;
  current = GetLeft(results.top());
  std::cout << current << " => " << std::endl;
  current = GetRight(results.top());
  std::cout << current << " => " << std::endl;
  curL = Split(current, " ");

  results.pop();

  while (results.size() != 0) {
    current = GetLeft(results.top());

    for (int i = (int)curL.size() - 1; i >= 0; i--) {
      if (current == curL[i]) {
        auto pos = curL.erase(curL.begin() + i);
        current = GetRight(results.top());
        std::vector<std::string> tmp;
        tmp = Split(current, " ");

        if (curL.size() > 0) {
          for (auto s : tmp) {
            if (s != EMPTYCH) {
              pos = curL.insert(pos, s);
              pos++;
            }
          }
        } else {
          for (auto s : tmp) {
            if (s != EMPTYCH)
              curL.push_back(s);
          }
        }

        current.clear();
        for (auto i = curL.begin(); i != curL.end();) {
          current.append(*i);
          i++;
          if (i != curL.end()) {
            current.append(" ");
          }
        }
        std::cout << current;
        results.pop();

        if (results.size() != 0) {
          std::cout << " => " << std::endl;
        }
        break;
      }
    }
  }
}