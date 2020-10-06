#include "token.h"
#include "variable.h"
#include <iostream>
#include <string>
#include <vector>


double expression(TokenStream& ts);

double declaration(TokenStream& ts)
{
  Token t = ts.get();
  if (t.kind != NAME)
    throw std::runtime_error{"declaration(): variable name required in initialization"};
  std::string var_name{ t.name };

  t = ts.get();
  if (t.kind != '=')
    throw std::runtime_error{"declaration(): missed '=' in declaration"};

  double d{ expression(ts) };
  define_name(var_name, d);

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

    case '-':
      return -primary(ts);

    case '+':
      return primary(ts);

    case NAME:
      return get_value(t.name);

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
      return declaration(ts);

    case NAME:
    {
      Token t1 = ts.get();
      if (t1.kind == INIT)
      {
        double d{ primary(ts) };
        return set_value(t.name, d);
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

  try
  {
    while (std::cin)
    {
      std::cout << PROMPT;
      Token t = ts.get();

      while (t.kind == PRINT) t = ts.get();
      if (t.kind == QUIT) return;

      ts.putback(t);
      std::cout << RESULT << statement(ts) << '\n';
    }
  }
  catch (std::exception& e)
  {
    std::cout << e.what() << '\n';
    ts.ignore(PRINT);
  }
}


int main()
{
  try
  {
    define_name("pi", 3.1415926535);
    define_name("e", 2.7182818284);

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
