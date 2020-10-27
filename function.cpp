#include "function.h"
#include "token.h"
#include "variable.h"
#include <string>
#include <vector>


FunctionScope func_scope;


bool FunctionScope::is_declared (std::string f_name)
{
  for (Function& f : func_table)
    if (f.func_name == f_name) return true;

  return false;
}

void FunctionScope::define (std::string f_name, std::string a_name, std::vector<Token> f)
{
  if (is_declared(f_name))
    throw FunctionError{ "function " + f_name + " is already defined" };

  func_table.push_back(Function(f_name, a_name, f));
}

Function FunctionScope::get (std::string f_name)
{
  for (Function& f : func_table)
    if (f.func_name == f_name) return f;

  throw FunctionError{ "undefined function " + f_name + "" };
}

std::vector<Token> FunctionScope::calc (std::string f_name, double a_val)
{
  Function f{ get(f_name) };
  std::vector<Token> new_expr{};

  for (Token& t : f.func)
  {
    if (t.kind == NAME && t.name == f.arg_name)
      new_expr.push_back(Token{NUMBER, a_val});
    else
      new_expr.push_back(t);
  }

  return new_expr;
}
