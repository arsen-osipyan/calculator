#include "token.h"
#include <iostream>
#include <string>
#include <vector>


Token TokenStream::get()
{
  if (!buffer.empty())         // get tokens from buffer before getting from console input
  {
    Token tmp{ buffer[buffer.size()-1] };
    buffer.pop_back();
    return tmp;
  }

  char ch;
  while (true)
  {
    std::cin.get(ch);
    if (std::cin.eof()) return Token{QUIT};
    if (ch == ENDLINE || !isspace(ch)) break;
  }

  switch (ch)
  {
    case ENDLINE:
    case PRINT:
    case INIT:
    case '(': case ')':
    case '{': case '}':
    case '[': case ']':
    case '%': case '^':
    case '+': case '-':
    case '*': case '/':
      return Token{ ch };      // every symbol represents itself

    case '.':                  // float numbers
    case '0': case '1': case '2': case '3': case '4':
    case '5': case '6': case '7': case '8': case '9':
    {
      while (ch == '.')
      {
        char tbr;
        std::cin.get(tbr);
        if (!isdigit(tbr))
        {
          std::cin.putback(tbr);
          throw TokenError{ "bad token" };
        }
      }
      std::cin.putback(ch);    // first digit returns to input stream
      double val;
      std::cin >> val;         // reading float number
      if (!std::cin.good() && !std::cin.eof())
      {
        std::cin.clear();
        throw TokenError{ "bad token" };
      }
      return Token{ NUMBER, val };
    }

    default:
    {
      if (isalpha(ch) || ch == '_' || ch == '$')           // reading variable name or declaration key word
      {
        std::string s;
        s += ch;
        while (std::cin.get(ch) && (isalpha(ch) || isdigit(ch) || ch == '_' || ch == '$')) s += ch;
        std::cin.putback(ch);

        if (s == DECLKEY)  return Token{ LET };
        if (s == CONSTKEY) return Token{ CONST };
        if (s == QUITKEY)  return Token{ QUIT };
        if (s == HELPKEY)  return Token{ HELP };
        return Token{ NAME, s };
      }
      throw TokenError{ "bad token" };
    }
  }
}

void TokenStream::putback(Token t)
{
  buffer.push_back(t);
}

void TokenStream::ignore(char break_char)
{
  buffer.clear();
  while (true)
  {
    char ch;
    std::cin.get(ch);
    if (ch == break_char) return;
  }
}
