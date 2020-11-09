// if we will reassign input and output stream
// #define STREAM_REASSIGNING

#include "token.h"
#include "variable.h"
#include <iostream>
#include <string>
#include <vector>
#include <cmath>


double expression(TokenStream& ts);

void print_help()
{
  std::cout << "==========================================\n"
            << "                Calculator                \n"
            << "\n"
            << "  1) help - print manual                  \n"
            << "  2) exit - exit calculator               \n"
            << "  3) let, const - define variable or      \n"
            << "  constant                                \n"
            << "  4) ';' or '\\n' - end of statement      \n"
            << "  Constants:                              \n";

  for (Variable& v : var_scope.get_table())
    if (v.is_constant)
      std::cout << "   - " << v.name << " = " << v.value << "\n";

  std::cout << "==========================================\n";
}

double declaration(TokenStream& ts)
{
  Token t = ts.get();

  switch (t.kind)
  {
    case CONST:
    case LET:
    {
      bool is_const;
      if (t.kind == CONST) is_const = true;
      else if (t.kind == LET) is_const = false;

      t = ts.get();
      if (t.kind == ENDLINE) std::cin.putback(ENDLINE);
      if (t.kind != NAME) throw std::runtime_error{"declaration(): variable name required in declaration"};
      std::string var_name{ t.name };

      t = ts.get();
      if (t.kind == ENDLINE) std::cin.putback(ENDLINE);
      if (t.kind != '=') throw std::runtime_error{"declaration(): missed '=' in declaration"};

      double d{ expression(ts) };
      var_scope.define(var_name, d, is_const);

      return d;
    }

    default:
      throw std::runtime_error{"declaration(): undefined keyword to declaration"};
  }
}

double function(TokenStream& ts)
{
  Token fname{ ts.get() };

  Token t{ ts.get() };
  if (t.kind != '(')
  {
    if (t.kind == ENDLINE || t.kind == PRINT) std::cin.putback(t.kind);
    throw std::runtime_error{ "function(): primary '(' expected" };
  }

  double d{ expression(ts) };

  t = ts.get();
  if (t.kind != ')')
  {
    if (t.kind == ENDLINE || t.kind == PRINT) std::cin.putback(t.kind);
    throw std::runtime_error{ "function(): primary ')' expected" };
  }

  if      (fname.name == "sin")    return sin(d);
  else if (fname.name == "cos")    return cos(d);
  else if (fname.name == "tg")     return tan(d);
  else if (fname.name == "ctg")    return 1. / tan(d);
  else if (fname.name == "arcsin") return asin(d);
  else if (fname.name == "arccos") return acos(d);
  else if (fname.name == "arctg")  return atan(d);
  else if (fname.name == "arcctg") return acos(-1)/2 - atan(d);
  else if (fname.name == "abs")    return fabs(d);
  else if (fname.name == "sqrt")   return sqrt(d);
  else if (fname.name == "ch")     return cosh(d);
  else if (fname.name == "sh")     return sinh(d);
  else if (fname.name == "th")     return tanh(d);
  else if (fname.name == "cth")    return 1. / tanh(d);
  else if (fname.name == "exp")    return exp(d);
  else if (fname.name == "ln")     return log(d);
  else if (fname.name == "lg")     return log(d) / log(10);
}

double assignment(TokenStream& ts)
{
  Token t = ts.get();
  var_scope.get(t.name);
  Token t1 = ts.get();

  if (t1.kind == INIT)
  {
    double d{ expression(ts) };
    Token tmp = ts.get();
    if (tmp.kind == ENDLINE) std::cin.putback(ENDLINE);
    var_scope.set(t.name, d);
    return d;
  }
  else
  {
    ts.putback(t1);
    ts.putback(t);
    return expression(ts);
  }
}

double primary(TokenStream& ts)
{
  Token t = ts.get();

  switch (t.kind)
  {
    case '(':
    {
      double d = expression(ts);
      t = ts.get();
      if (t.kind != ')')
        throw std::runtime_error{"primary(): ')' expected"};
      return d;
    }

    case '{':
    {
      double d = expression(ts);
      t = ts.get();
      if (t.kind != '}')
        throw std::runtime_error{"primary(): '}' expected"};
      return d;
    }

    case '[':
    {
      double d = expression(ts);
      t = ts.get();
      if (t.kind != ']')
        throw std::runtime_error{"primary(): ']' expected"};
      return d;
    }

    case NUMBER:
      return t.value;

    case NAME:
      return var_scope.get(t.name);

    case FUNC:
      ts.putback(t);
      return function(ts);

    case '-':
      return -primary(ts);

    case '+':
      return primary(ts);

    default:
      if (t.kind == ENDLINE || t.kind == PRINT) std::cin.putback(t.kind);
      throw std::runtime_error{"primary(): primary expected"};
  }
}

double preterm(TokenStream& ts)
{
  double left = primary(ts);
  Token t = ts.get();

  while (true)
  {
    switch (t.kind)
    {
      case '^':
        left = pow(left, primary(ts));
        t = ts.get();
        break;

      default:
        ts.putback(t);
        return left;
    }
  }
}

double term(TokenStream& ts)
{
  double left = preterm(ts);
  Token t = ts.get();

  while (true)
  {
    switch (t.kind)
    {
      case '*':
        left *= preterm(ts);
        t = ts.get();
        break;

      case '/':
      {
        double d = preterm(ts);
        t = ts.get();
        if (d == 0)
        {
          if (t.kind == ENDLINE) std::cin.putback(ENDLINE);
          throw std::runtime_error{ "term(): '/' divide by zero" };
        }
        left /= d;
        break;
      }

      case '%':
      {
        double d = preterm(ts);
        t = ts.get();
        if (d == 0)
        {
          if (t.kind == ENDLINE) std::cin.putback(ENDLINE);
          throw std::runtime_error{ "term(): '%' divide by zero" };
        }
        left -= int(left / d) * d;
        break;
      }

      default:
        ts.putback(t);
        return left;
    }
  }
}

double expression(TokenStream& ts)
{
  double left = term(ts);
  Token t = ts.get();

  while (true)
  {
    switch (t.kind)
    {
      case '+':
        left += term(ts);
        t = ts.get();
        break;

      case '-':
        left -= term(ts);
        t = ts.get();
        break;

      default:
        ts.putback(t);
        return left;
    }
  }
}

double statement(TokenStream& ts)
{
  Token t = ts.get();

  switch (t.kind)
  {
    case LET:
    case CONST:
      ts.putback(t);
      return declaration(ts);

    case NAME:
      ts.putback(t);
      return assignment(ts);

    default:
      ts.putback(t);
      return expression(ts);
  }
}

Token calculate(TokenStream& ts)
{
  Token t = ts.get();

  switch (t.kind)
  {
    case PRINT:
      return Token{ PRINT };

    case ENDLINE:
      return Token{ ENDLINE };

    case QUIT:
      return Token{ QUIT };

    case HELP:
      return Token{ HELP };

    default:
      ts.putback(t);
      return Token{ NUMBER, statement(ts) };
  }
}

void run()
{
  TokenStream ts;
  std::cout.precision(8);

  while (std::cin.good())
  {
    try
    {
#ifndef STREAM_REASSIGNING
      std::cerr << PROMPT;
#endif
      Token res = calculate(ts);

      while (res.kind != ENDLINE)
      {
        if (res.kind == NUMBER)
          std::cout << RESULT << res.value << '\n';
        if (res.kind == HELP)
          print_help();
        if (res.kind == QUIT)
          return;

        Token new_res = calculate(ts);
        res = new_res;
      }
    }
    catch (TokenError& e)
    {
      std::cout << "TokenError: " << e.what << ".\n";
      ts.ignore(ENDLINE);
      continue;
    }
    catch (VariableError& e)
    {
      std::cout << "VariableError: " << e.what << ".\n";
      ts.ignore(ENDLINE);
      continue;
    }
    catch (std::runtime_error& e)
    {
      std::cout << "RunTimeError: " << e.what() << ".\n";
      ts.ignore(ENDLINE);
      continue;
    }
  }
}


int main()
{
  try
  {
    var_scope.define("pi", acos(-1.), true);
    var_scope.define("e",  2.718281828459045, true);
    var_scope.define("G",  6.67e-11, true);

    run();

    return 0;
  }
  catch (...)
  {
    std::cerr << "Something went wrong." << std::endl;
    return 2;
  }
}
