#include "token.h"
#include "variable.h"
#include <iostream>
#include <string>
#include <vector>

// Bugs
// 1. Infinite printing "TokenError" when reading "."
// 2. Twice prompt printing when "TokenError"


void print_help()
{
  std::cout << "==========================================\n"
            << "                Calculator                \n"
            << "\n"
            << "  1) help - print manual                  \n"
            << "  2) quit - exit calculator               \n"
            << "  3) let, const - define variable or      \n"
            << "  constant                                \n"
            << "  4) ';' or '\\n' - end of statement      \n"
            << "  Constants:                              \n";

  for (Variable& v : var_scope.get_table())
  {
    if (v.is_constant)
    {
      std::cout << "   - " << v.name << " = " << v.value << "\n";
    }
  }

  std::cout << "==========================================\n";
}

double expression(TokenStream& ts);

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
      if (t.kind != NAME) throw std::runtime_error{"declaration(): variable name required in initialization"};
      std::string var_name{ t.name };

      t = ts.get();
      if (t.kind != '=') throw std::runtime_error{"declaration(): missed '=' in declaration"};

      double d{ expression(ts) };
      var_scope.define(var_name, d, is_const);

      return d;
    }

    default:
      throw std::runtime_error{"declaration(): undefined keyword to declaration"};
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

Token calculate(TokenStream& ts)
{
  Token t = ts.get();

  switch (t.kind)
  {
    case PRINT:
      return Token { PRINT };

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
  std::cout.precision(15);

  while (true)
  {
    try
    {
      std::cout << PROMPT;
      Token res = calculate(ts);

      while (res.kind != ENDLINE)
      {
        if (res.kind == NUMBER)
          std::cout << RESULT << res.value << '\n';
        if (res.kind == HELP)
          print_help();
        if (res.kind == QUIT)
          return;

        res = calculate(ts);
      }
    }
    catch (TokenError& e)
    {
      std::cout << "TokenError: " << e.what << ".\n";
      ts.clean(); // realise ignore cleaning
    }
    catch (VariableError& e)
    {
      std::cout << "VariableError: " << e.what << ".\n";
      ts.clean();
    }
    catch (std::runtime_error& e)
    {
      std::cerr << "RunTimeError: " << e.what() << "." << std::endl;
      ts.clean();
    }
  }
}


int main()
{
  try
  {
    var_scope.define("pi", 3.1415926535, true);
    var_scope.define("e", 2.7182818284, true);

    run();

    return 0;
  }
  catch (...)
  {
    std::cerr << "Something went wrong." << std::endl;
    return 2;
  }
}
