#include "token.h"
#include <iostream>
#include <string>
#include <vector>


Token TokenStream::get()
{
  if (!buffer.empty())         // getting tokens from buffer before getting from console input
  {
    Token tmp{ buffer[buffer.size()-1] };
    buffer.pop_back();
    return tmp;
  }

  char ch;
  //std::cin >> std::noskipws;
  while (true)
  {
    std::cin.get(ch);
    if (ch == '\n' || !isspace(ch)) break;
  }
  //std::cin >> std::skipws;


  switch (ch)
  {
    case PRINT:
    case NAME:
    case INIT:
    case '(': case ')':
    case '{': case '}':
    case '[': case ']':
    case '%':
    case '+': case '-':
    case '*': case '/':
      return Token{ ch };      // Every symbol represents itself

    case '\n':
      return Token{ PRINT };

    case '.':                  // Float numbers
    case '0': case '1': case '2': case '3': case '4':
    case '5': case '6': case '7': case '8': case '9':
    {
      std::cin.putback(ch);    // First digit returns to input stream
      double val;
      std::cin >> val;         // Reading float number
      return Token{ NUMBER, val };
    }

    default:
    {
      if (isalpha(ch) || ch == '_')           // Reading variable name or declaration key word
      {
        std::string s;
        s += ch;
        while (std::cin.get(ch) && (isalpha(ch) || isdigit(ch) || ch == '_')) s += ch;
        std::cin.putback(ch);
        if (s == DECLKEY) return Token{ LET };
        if (s == CONSTKEY) return Token{ CONST };
        if (s == QUITKEY) return Token{ QUIT };
        if (s == HELPKEY) return Token{ HELP };
        return Token{ NAME, s };
      }
      throw TokenError("bad token");
    }
  }
}

void TokenStream::putback(Token t)
{
  buffer.push_back(t);
}

void TokenStream::ignore(char token_kind)
{
  while (true)
  {
    Token t{ get() };
    if (t.kind == token_kind)
    {
      putback(t);
      return;
    }
  }
}
