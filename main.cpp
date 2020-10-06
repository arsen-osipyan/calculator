//
// This is example code from Chapter 6.7 "Trying the second version" of
// "Software - Principles and Practice using C++" by Bjarne Stroustrup
//

/*
  This file is known as calculator02buggy.cpp

I have inserted 5 errors that should cause this not to compile
  I have inserted 3 logic errors that should cause the program to give wrong results

    First try to find and remove the bugs without looking in the book.
    If that gets tedious, compare the code to that in the book (or posted source code)

      Happy hunting!

      */

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


      /// Customizing
      const char NUMBER{ '8' };        /// For numbers
const char QUIT{ 'q' };          /// For quit
const char PRINT{ ';' };         /// For print
const char NAME{ 'a' };          /// For variable name
const char LET{ 'L' };           /// For DECLKEY word
const char INIT{ '=' };          /// For variable initialization
const string PROMPT{ "> " };     /// String before inputing statement
const string RESULT{ "= " };     /// String before printing result
const string ERROR_EXIT{ "~~" }; /// String to exit from console when error
const string DECLKEY{ "let" };   /// Key word for variable declaration


class Token
{
  public:
  char kind;
  double value;
  string name;

  /// Initialize kind
  Token(char ch)
    : kind{ ch } { }

  /// Initialize kind & value
  Token(char ch, double val)
    : kind{ ch }, value{ val }  { }

  /// Initialize kind & name
  Token(char ch, string n)
    : kind{ ch }, name{ n } { }
};

class Token_stream
{
  public:
  Token get();
  void putback(Token t);
  void ignore(char c);

  /// Initialize with zero buffer
  Token_stream()
    : buffer{ } { }

  private:
  vector<Token> buffer; // uppdate buffer to be vector<Token>
};

class Variable
{
  public:
  string name;
  double value;

  /// Initialize with name & value
  Variable(string n, double val)
    : name{ n }, value{ val } { }
};


Token Token_stream::get()
{
  if (!buffer.empty())               /// If buffer is full
  {
    Token tmp{ buffer[buffer.size()-1] };
    buffer.pop_back();
    return tmp;
  }

  char ch;
  cin >> ch;              /// >> operator skips white space symbols

  switch (ch)
  {
  case PRINT:
  case QUIT:
  case NAME:
  case LET:
  case INIT:
  case '(': case ')':
  case '{': case '}':
  case '%':
  case '+': case '-':
  case '*': case '/':
  {
    return Token{ ch }; /// Every symbol represents itself
  }

  case '.':             /// Float numbers
  case '0': case '1': case '2': case '3': case '4':
  case '5': case '6': case '7': case '8': case '9':
  {
    cin.putback(ch);    /// First digit returns to input stream
    double val;
    cin >> val;         /// Reading float number
    return Token{ NUMBER, val };
  }

  default:
  {
    if (isalpha(ch))    /// Reading variable name or declaration key word
    {
      string s;
      s += ch;
      while (cin.get(ch) && (isalpha(ch) || isdigit(ch))) s += ch;
      cin.putback(ch);
      if (s == DECLKEY) return Token{ LET };
      return Token{ NAME, s };
    }
    error("Token_stream::get(): bad token");
  }
  }
}

void Token_stream::putback(Token t)
{
  buffer.push_back(t);
}

void Token_stream::ignore(char token_kind)
{
  while (true)
  {
    Token t{ get() };
    if (t.kind == token_kind)
    {
      putback(t);
      return;
    }
  }
}


double expression(Token_stream& ts);

vector<Variable> var_table{ };


double get_value(string s)
{
  for (const Variable& v : var_table)
    if (v.name == s) return v.value;
  error("get_value(): undefined variable ", s);
}

bool is_declared(string var)
{
  for (const Variable& v : var_table)
    if (v.name == var) return true;
  return false;
}

double define_name(string var, double val)
{
  if (is_declared((var))) error("define_name(): variable is already initialized ", var);
  var_table.push_back(Variable(var, val));
  return val;
}

double set_value(string var, double val)
{
  if (!is_declared(var)) error("set_value(): variable is not defined ", var);
  for (size_t i{0}; i < var_table.size(); ++i) if (var_table[i].name == var) var_table[i].value = val;
  return val;
}

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
