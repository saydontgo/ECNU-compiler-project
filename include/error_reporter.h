#pragma once
#include <iostream>
#include <unordered_map>
#include <vector>

enum class ErrorLevel { Warning, Error };

enum class ErrorCode {
  UnexpectedCharacter,
  UnterminatedString,
  InvalidEscapeSequence,
  MissingTerminatingCharacter,
  UnterminatedComment,
  EmptyCharacterConstant,

  // newly added for LL(1) parser
  MissingSymbol,
  UnexpectedCharacterChinese
  // newly added for LL(1) parser
};

namespace ErrorMessages {
static const std::unordered_map<ErrorCode, std::string> kMessageTable = {
    {ErrorCode::UnexpectedCharacter, "Unexpected character: '{}'."},
    {ErrorCode::UnterminatedString, "Unterminated string literal."},
    {ErrorCode::InvalidEscapeSequence, "Invalid escape sequence: '\\{}'."},
    {ErrorCode::MissingTerminatingCharacter,
     "Missing terminating ' character."},
    {ErrorCode::UnterminatedComment, "Unterminated comment block."},
    {ErrorCode::EmptyCharacterConstant, "Empty character constant."},

    // newly added for LL(1) parser
    {ErrorCode::MissingSymbol, "缺少\"{}\""},
    {ErrorCode::UnexpectedCharacterChinese, "多余\"{}\""},
    // newly added for LL(1) parser
};
}

struct LexError {
  ErrorLevel level_;
  ErrorCode code_;
  std::string formattedMessage_;
  int line_;
  int column_;
};

class ErrorReporter {
public:
  void Report(ErrorLevel level, ErrorCode code, int line, int column,
              const std::string &extra); // push_back method for ErrorReporter

  void PrintAll();

  // newly added for LL(1) parser
  void PrintLL1();
  // newly added for LL(1) parser
private:
  std::vector<LexError> errors_;
};