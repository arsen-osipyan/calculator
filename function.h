#ifndef FUNCTION_H
#define FUNCTION_H


#include "variable.h"
#include "token.h"
#include <string>
#include <vector>


class Function
{
  public:
    std::string func_name;
    std::string arg_name;
    std::vector<Token> func;

    Function (std::string f_name, std::string a_name, std::vector<Token> f)
      : func_name{ f_name }, arg_name{ a_name }, func{ f } { }
};

class FunctionError
{
  public:
    std::string what;

    FunctionError () = delete;
    FunctionError (std::string str)
      : what{ str } { }
};

class FunctionScope
{
  private:
    std::vector<Function> func_table;

  public:
    FunctionScope () = default;

    std::vector<Function> get_table() const { return func_table; }
    Function get (std::string f_name);
    bool is_declared (std::string f_name);
    void define (std::string f_name, std::string a_name, std::vector<Token> f);
    std::vector<Token> calc (std::string f_name, double a_val);
};

extern FunctionScope func_scope;


#endif // FUNCTION_H
