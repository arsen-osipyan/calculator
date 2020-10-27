#ifndef VARIABLE_H
#define VARIABLE_H


#include <string>
#include <vector>


class Variable
{
  public:
    std::string name;
    double value;
    bool is_constant;

    Variable(std::string s, double val, bool constant)
      : name{ s }, value{ val }, is_constant{ constant } { }
};

class VariableError
{
  public:
    std::string what;

    VariableError () = delete;
    VariableError (std::string str)
      : what{ str } { }
};

class VariableScope
{
  private:
    std::vector<Variable> var_table;

  public:
    VariableScope () = default;

    std::vector<Variable> get_table() const { return var_table; }
    double get(std::string s);
    bool is_declared (std::string var);
    double define (std::string var, double val, bool is_const);
    double set (std::string var, double val);
};

extern VariableScope var_scope;


#endif // VARIABLE_H
