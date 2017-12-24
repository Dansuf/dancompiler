#pragma once

#include <string>

#include "InstructionRegistry.hpp"
#include "VariableRegistry.hpp"

enum class IntInstrType: int
{
  GET = 0, PUT = 1, ADD= 2, SUB = 3, MUL=4, DIV=5, MOD=6, SET=7, HALT = 8
};

class IntInstrAbstr
{
public:
  virtual void dbgPrint() = 0;
  virtual InstructionRegistry translate(VariableRegistry& variables) = 0;
  virtual void optimize() = 0;
};

class IntInstr : IntInstrAbstr
{
  static const std::string instrStr[];

  IntInstrType type;

  std::string val1;
  std::string val2;
  std::string val3;

  static void addLoadInstruction(VariableRegistry& variables, InstructionRegistry& instructions, std::string variable);
  static void addAddInstruction(VariableRegistry& variables, InstructionRegistry& instructions, std::string variable);
  static void addSubInstruction(VariableRegistry& variables, InstructionRegistry& instructions, std::string variable);
  static lint addStoreInstruction(VariableRegistry& variables, InstructionRegistry& instructions, std::string variable);
  static void addMulInstruction(VariableRegistry& variables, InstructionRegistry& instructions, std::string variable1, std::string variable2, std::string out);
  static void addDivInstruction(VariableRegistry& variables, InstructionRegistry& instructions, std::string variable1, std::string variable2, std::string out, std::string remainder);
public:


  IntInstr(IntInstrType type,std::string val1,std::string val2,std::string val3): type(type), val1(val1), val2(val2), val3(val3){};

  IntInstr(IntInstrType type,std::string val1,std::string val2): type(type), val1(val1), val2(val2){};

  IntInstr(IntInstrType type,std::string val1): type(type), val1(val1){};

  IntInstr(IntInstrType type): type(type){};

  void dbgPrint() override;

  std::string getTypeAsStr();

  InstructionRegistry translate(VariableRegistry& variables) override;

  void optimize() override;
};
