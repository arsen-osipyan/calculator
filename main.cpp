/* TODO:
 * 1. Глава 7 (calculator08buggy пофиксить)
 * 2. Упражнения в конце главе с 1 по 9
 * 3. Поделить на файлы
 * 4. Тесты написать минимум 50
 */

/* Что сделать с файлами:
 * token.h (содержит описание классов и функций и константы)
 * token.cpp (содержит реализацию классов и функций)
 *   - Лексемы
 *   - Поток ввода
 *   - Константы (ключевые слова)
 * variable.h (объявить через extern переменные и таблици символов)
 * variable.cpp
 * calculator.cpp
 *
 * ifndef использовать
 */

#include <std_lib_facilities.h>
#include "token.h"
#include "variable.h"



double expression(Token_stream& ts);



double declaration(Token_stream& ts)
{
  Token t = ts.get();
  if (t.kind != NAME)
    error("declaration(): variable name required in initialization");
  string var_name{ t.name };

  t = ts.get();
  if (t.kind != '=')
    error("declaration(): missed '=' in declaration ", var_name);

  double d{ expression(ts) };
  define_name(var_name, d);

  return d;
}

double primary(Token_stream& ts)
{
  Token t = ts.get();
  switch (t.kind)
  {
  case '(':
  {
    double d = expression(ts);
    t = ts.get();
    if (t.kind != ')')
      error("primary(): ')' expected");
    return d;
  }

  case '{':
  {
    double d = expression(ts);
    t = ts.get();
    if (t.kind != '}')
      error("primary(): '}' expected");
    return d;
  }

  case NUMBER:
    return t.value;

  case '-':
    return -primary(ts);

  case '+':
    return primary(ts);

  case 'a':
    return get_value(t.name);

  default:
    error("primary(): primary expected");
  }
}

double term(Token_stream& ts)
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
      if (d == 0) error("term(): divide by zero");
      left /= d;
      t = ts.get();
      break;
    }

    case '%':
    {
      double d = primary(ts);
      if (d == 0) error("term(): divide by zero");
      left = fmod(left, d);
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

double expression(Token_stream& ts)
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

double statement(Token_stream& ts)
{
  Token t = ts.get();

  switch (t.kind)
  {
  case LET:
    return declaration(ts);

  case NAME:
  {
    Token t1 = ts.get();
    if (t1.kind == '=')
    {
      double d{ primary(ts) };
      ts.putback(t1);
      ts.putback(t);
      set_value(t.name, d);
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
  Token_stream ts;
  try
  {
    while (cin)
    {
      cout << PROMPT;
      Token t = ts.get();

      while (t.kind == PRINT) t = ts.get();
      if (t.kind == QUIT) return;

      ts.putback(t);
      cout << RESULT << statement(ts) << '\n';
    }

  }
  catch (exception& e)
  {
    cout << e.what() << '\n';
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

    // keep_window_open();
    return 0;
  }
  catch (exception& e)
  {
    cerr << e.what() <<'\n';
    // keep_window_open(ERROR_EXIT);
    return 1;
  }
  catch (...)
  {
    cerr << "Oops: unknown exception!\n";
    // keep_window_open(ERROR_EXIT);
    return 2;
  }
}
