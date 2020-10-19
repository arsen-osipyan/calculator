#ifndef TOKEN_H
#define TOKEN_H


#include <string>
#include <vector>


constexpr char NUMBER{ '8' };            // For numbers
constexpr char QUIT{ 'q' };              // For quit
constexpr char PRINT{ ';' };             // For print
constexpr char HELP{ 'h' };              // For help
constexpr char NAME{ 'a' };              // For variable name
constexpr char LET{ 'l' };               // For DECLKEY keyword
constexpr char CONST{ 'c' };             // For CONSTKEY keyword
constexpr char INIT{ '=' };              // For variable initialization

const std::string PROMPT{ "> " };        // String before inputing statement
const std::string RESULT{ "= " };        // String before printing result
const std::string DECLKEY{ "let" };      // For variable declaration
const std::string CONSTKEY{ "const" };   // For constant declaration
const std::string HELPKEY{ "help" };     // For get help
const std::string QUITKEY{ "quit" };     // For quit


class Token
{
  public:
    char kind;
    double value;
    std::string name;

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

    TokenStream()
      : buffer{ } { }

  private:
    std::vector<Token> buffer;
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
