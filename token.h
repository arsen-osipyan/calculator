﻿#ifndef TOKEN_H
#define TOKEN_H


#include <string>
#include <vector>


const char NUMBER{ '8' };                // For numbers
const char QUIT{ 'q' };                  // For quit
const char PRINT{ ';' };                 // For print
const char NAME{ 'a' };                  // For variable name
const char LET{ 'l' };                   // For DECLKEY keyword
const char CONST{ 'c' };                 // For CONSTKEY keyword
const char INIT{ '=' };                  // For variable initialization
const std::string PROMPT{ "> " };        // String before inputing statement
const std::string RESULT{ "= " };        // String before printing result
const std::string ERROR_EXIT{ "~~" };    // String to exit from console when error
const std::string DECLKEY{ "let" };      // Key word for variable declaration
const std::string CONSTKEY{ "const" };   // Key word for constant declaration


class Token
{
  public:
    char kind;
    double value;
    std::string name;

    // Initialize kind
    Token(char ch)
      : kind{ ch } { }

    // Initialize kind & value
    Token(char ch, double val)
      : kind{ ch }, value{ val }  { }

    // Initialize kind & name
    Token(char ch, std::string n)
      : kind{ ch }, name{ n } { }
};

class TokenStream
{
  public:
    Token get();
    void putback(Token t);
    void ignore(char c);

    // Initialize with zero buffer
    TokenStream()
      : buffer{ } { }

  private:
    std::vector<Token> buffer; // uppdate buffer to be vector<Token>
};

class TokenError
{
  public:
    std::string what;

    TokenError () = delete;
    TokenError (std::string str)
      : what{ str } { }
};


#endif // TOKEN_H