#pragma once
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
  EndOfInput
};

struct Token {
  TokenType type;
  std::string value;
};

std::vector<Token> tokenize(const std::string& input);
