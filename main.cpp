#include "token.h"
#include "variable.h"
#include <iostream>
#include <string>
#include <vector>


double expression(TokenStream& ts);

double declaration(TokenStream& ts)
{
  Token kw = ts.get();
  bool is_const{};
  if (kw.kind == CONST) is_const = true;
  else if (kw.kind == LET) is_const = false;

  Token t = ts.get();
  if (t.kind != NAME)
    throw std::runtime_error{"declaration(): variable name required in initialization"};
  std::string var_name{ t.name };

  t = ts.get();
  if (t.kind != '=')
    throw std::runtime_error{"declaration(): missed '=' in declaration"};

  double d{ expression(ts) };
  var_scope.define(var_name, d, is_const);

  return d;
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

    case '-':
      return -primary(ts);

    case '+':
      return primary(ts);

    default:
      throw std::runtime_error{"primary(): primary expected"};
  }
}

double term(TokenStream& ts)
{
  double left = primary(ts);
  Token t = ts.get();

  while (true)
  {
    switch (t.kind)
    {
      case '*':
      {
        left *= primary(ts);
        t = ts.get();
        break;
      }

      case '/':
      {
        double d = primary(ts);
        if (d == 0) throw std::runtime_error{"term(): divide by zero"};
        left /= d;
        t = ts.get();
        break;
      }

      case '%':
      {
        double d = primary(ts);
        if (d == 0) throw std::runtime_error{"term(): divide by zero"};
        left -= int(left / d) * d;
        t = ts.get();
        break;
      }

      default:
      {
        ts.putback(t);
        return left;
      }
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
      {
        left += term(ts);
        t = ts.get();
        break;
      }

      case '-':
      {
        left -= term(ts);
        t = ts.get();
        break;
      }

      default:
      {
        ts.putback(t);
        return left;
      }
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
    {
      Token t1 = ts.get();
      if (t1.kind == INIT)
      {
        double d{ expression(ts) };
        return var_scope.set(t.name, d);
      }
      else
      {
        ts.putback(t1);
        ts.putback(t);
        return expression(ts);
      }
    }

    default:
      ts.putback(t);
      return expression(ts);
  }
}

void calculate()
{
  TokenStream ts;

  while (std::cin)
  {
    try
    {
      Token t = ts.get();

      while (t.kind == PRINT)
        t = ts.get();

      if (t.kind == HELP)
      {
        std::cout << RESULT << "I <3 U" << std::endl;
        continue;
      }

      if (t.kind == QUIT) return;

      ts.putback(t);
      std::cout << RESULT << statement(ts) << std::endl;
    }
    catch (std::runtime_error& e)
    {
      std::cerr << e.what() << std::endl;
      ts.ignore(PRINT);
    }
  }
}


int main()
{
  try
  {
    var_scope.define("pi", 3.1415926535, true);
    var_scope.define("e", 2.7182818284, true);

    calculate();

    return 0;
  }
  catch (TokenError& e)
  {
    std::cerr << "TokenError: " << e.what << '\n';
    return 1;
  }
  catch (VariableError& e)
  {
    std::cerr << "VariableError: " << e.what << '\n';
    return 1;
  }
  catch (std::exception& e)
  {
    std::cerr << "RunTimeError: " << e.what() <<'\n';
    return 1;
  }
  catch (...)
  {
    std::cerr << "Something went wrong.\n";
    return 2;
  }
}
