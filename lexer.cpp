#include <string>
#include <vector>

enum class TokenType {
  Word,
  StringLiteral,
  Variable,
  Semicolon,
  Pipe,
  RedirectIn,
  RedirectOut,
  RedirectAppend,
  EndOfInput,
};

struct Token {
  TokenType type;
  std::string value;
};

std::vector<Token> tokenize(const std::string& input) {
  std::vector<Token> tokens;
  std::string buffer;
  size_t pos = 0;

  auto create_token = [&](std::string& buffer) -> void {
    if (!buffer.empty()) {
      tokens.push_back({TokenType::Word, buffer});
      buffer.clear();
    }
  };

  while (pos < input.size()) {
    char c = input[pos];

    if (std::isspace(static_cast<unsigned char>(c))) {
      create_token(buffer);
      ++pos;
    } else if (c == ';') {
      create_token(buffer);
      tokens.push_back({TokenType::Semicolon, ";"});
      ++pos;
    } else if (c == '|') {
      create_token(buffer);
      tokens.push_back({TokenType::Pipe, "|"});
      ++pos;
    } else {
      buffer.push_back(c);
      ++pos;
    }
  }

  create_token(buffer);
  tokens.push_back({TokenType::EndOfInput, ""});

  return tokens;
}