#include "error_reporter.h"

void ErrorReporter::Report(ErrorLevel level, ErrorCode code, int line,
                           int column, const std::string &extra) {
  std::string messageTemplate = ErrorMessages::kMessageTable.at(code);

  // replace placeholder with string `extra`
  std::string formatted = messageTemplate;
  size_t pos = formatted.find("{}");
  if (pos != std::string::npos) {
    formatted.replace(pos, 2, extra);
  }

  errors_.push_back({level, code, formatted, line, column});
}

void ErrorReporter::PrintAll() {
  if (errors_.empty()) {
    return;
  }

  std::cout << std::endl;
  for (const auto &error : errors_) {
    std::string level =
        error.level_ == ErrorLevel::Warning ? "Warning " : "Error ";
    std::cout << level << "[line " << error.line_ << ", col " << error.column_
              << "]: " << error.formattedMessage_ << std::endl;
  }
  std::cout << std::endl;
}

// newly added for LL(1) parser
void ErrorReporter::PrintLL1() {
  if (errors_.empty()) {
    return;
  }

  for (const auto &error : errors_) {
    std::string level =
        error.level_ == ErrorLevel::Warning ? "Warning " : "Error ";
    std::cout << "语法错误,第" << error.line_ << "行,"
              << error.formattedMessage_ << std::endl;
  }
}
// newly added for LL(1) parser