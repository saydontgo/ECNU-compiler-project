#include "lexical_analyzer.h"
#include "ll1_analyzer.h"
#include "ll1_parser.h"
#include "slr1_analyzer.h"
#include "slr1_parser.h"
#include "test.h"
/* 不要修改这个标准输入函数 */
void read_prog(std::string &prog) {
  char c;
  while (scanf("%c", &c) != EOF) {
    prog += c;
  }

  prog += '\n';
}
/* 你可以添加其他函数 */
std::vector<std::string> prods = {
    "program->compoundstmt",
    "stmt->ifstmt|whilestmt|assgstmt|compoundstmt",
    "compoundstmt->{ stmts }",
    "stmts->stmt stmts|E",
    "ifstmt->if ( boolexpr ) then stmt else stmt",
    "whilestmt->while ( boolexpr ) stmt",
    "assgstmt->ID = arithexpr ;",
    "boolexpr->arithexpr boolop arithexpr",
    "boolop-><|>|<=|>=|==",
    "arithexpr->multexpr arithexprprime",
    "arithexprprime->+ multexpr arithexprprime|- multexpr arithexprprime|E",
    "multexpr->simpleexpr multexprprime",
    "multexprprime->* simpleexpr multexprprime|/ simpleexpr multexprprime|E",
    "simpleexpr->ID|NUM|( arithexpr )"};
// 打印分析表
void TableTest() {
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
  std::cout << "****************action****************" << std::endl;
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

  std::cout << "****************goto******************" << std::endl;
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

void Analysis(std::string &prog) {
  /********* Begin *********/
  LexicalAnalyzer lexer(prog);
  auto tokens = lexer.Tokenize();
  lexer.PrintErrors();
  // tokens->PrintAll();

  // LL1 Parser
  // LL1Parser parser(lexer.GetTable());
  // auto trees = parser.ParseTokens(tokens);
  // parser.PrintErrors();
  // trees->PrintOutput();
  // trees->Abstract();
  // trees->PrintOutput();

  // SLR1 Parser
  SLR1Parser parser(prods);
  auto res = parser.ParsingTokens(tokens);
  parser.PrintErrors();
  parser.PrintOutput(res);
  /********* End *********/
}

void ReadFiles(const char *filename, std::string &prog) {
  prog.clear();
  // using files to read
  std::ifstream ifs;
  ifs.open(filename, std::ios::in);
  if (!ifs.is_open()) {
    std::cout << "read fail." << std::endl;
    std::abort();
  }
  char c;
  while ((c = ifs.get()) != EOF) {
    prog += c;
  }
  prog += '\n';
}

void Testinfo(std::string &&fname) {
  std::cout << "loading test...\n"
            << "test name : " << fname << "\n"
            << "you can refer to test_data/" << fname << " to read the input\n";
  std::cout << "result: \n";
}

int main(int argc, char *argv[]) {
  std::string prog = "";
  // read_prog(prog);

  if (argc < 2 || argc > 3) {
    std::cout << "insufficent or reduntant parameters" << std::endl;
    std::abort();
  }

  switch (argv[1][0]) {
  case '1':
    Testinfo("project1/my_test3_所有错误形式.in");
    ReadFiles("../test_data/project1/my_test3_所有错误形式.in", prog);
    test_1(prog);
    break;
  case '2':
    Testinfo("project1/my_test1_单引号与转义符.in");
    ReadFiles("../test_data/project1/my_test1_单引号与转义符.in", prog);
    test_2(prog);
    break;
  case '5':
    test_5();
    break;
  case '7':
    if (argc != 3) {
      std::cout << "please specify a file to test" << std::endl;
      std::abort();
    }
    ReadFiles(argv[2], prog);
    Testinfo(argv[2]);
    test_7(prog);
    break;
  case '9':
    test_9(prods);
    break;
  default:
    std::cout << "test case does not match. Choose a number from (1, 2, 5, 7) "
                 "to start testing.\n";
  }
  // Analysis(prog);
  // test_ll1Analyzer();

  // TableTest();
  return 0;
}
