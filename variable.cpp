#include "variable.h"
#include <string>
#include <vector>


std::vector<Variable> var_table{ };


double get_value(std::string s)
{
  for (const Variable& v : var_table)
    if (v.name == s) return v.value;
  throw VariableError("undefined variable");
}

bool is_declared(std::string var)
{
  for (const Variable& v : var_table)
    if (v.name == var) return true;
  return false;
}

double define_name(std::string var, double val)
{
  if (is_declared((var))) throw VariableError("variable is already defined");
  var_table.push_back(Variable(var, val));
  return val;
}

double set_value(std::string var, double val)
{
  if (!is_declared(var)) throw VariableError("variable is not defined");
  for (size_t i{0}; i < var_table.size(); ++i) if (var_table[i].name == var) var_table[i].value = val;
  return val;
}
