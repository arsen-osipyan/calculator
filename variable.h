#ifndef VARIABLE_H
#define VARIABLE_H


#include <string>
#include <vector>


class Variable
{
  public:
    std::string name;
    double value;

    Variable(std::string n, double val)
      : name{ n }, value{ val } { }
};

class VariableError
{
  public:
    std::string what;

    VariableError () = delete;
    VariableError (std::string str)
      : what{ str } { }
};


extern std::vector<Variable> var_table;


double get_value(std::string s);
bool is_declared(std::string var);
double define_name(std::string var, double val);
double set_value(std::string var, double val);


#endif // VARIABLE_H
