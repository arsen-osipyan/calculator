#include "variable.h"
#include <string>
#include <vector>


SymbolTable var_scope;


double SymbolTable::get (std::string s)
{
  for (Variable& v : var_table)
    if (v.name == s) return v.value;

  throw VariableError("undefined variable " + s);
}

bool SymbolTable::is_declared (std::string var)
{
  for (Variable& v : var_table)
    if (v.name == var) return true;

  return false;
}

double SymbolTable::define (std::string var, double val, bool is_const)
{
  if (is_declared((var)))
    throw VariableError("variable " + var + " is already defined");

  var_table.push_back(Variable(var, val, is_const));
  return val;
}

double SymbolTable::set (std::string var, double val)
{
  if (!is_declared(var))
    throw VariableError("variable " + var + " is not defined");

  for (Variable& v : var_table)
    if (v.name == var && v.is_constant) throw VariableError("variable " + var + " is constant");

  for (Variable& v : var_table)
    if (v.name == var) v.value = val;

  return val;
}
