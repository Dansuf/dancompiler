#include <cstdio>

#include "IntInstrBlock.hpp"

void IntInstrBlock::addInstr(IntInstrAbstr* instr)
{
  this->instructions.push_back(std::shared_ptr<IntInstrAbstr>(instr));
}

void IntInstrBlock::dbgPrint()
{
  for(auto instr : this->instructions)
  {
    instr->dbgPrint();
  }
}

InstructionRegistry IntInstrBlock::translate(VariableRegistry& variables)
{
  InstructionRegistry out;
  for(auto instruction : instructions)
  {
    out.append(instruction->translate(variables));
  }
  return out;
}

void IntInstrBlock::optimize()
{
  for(auto instruction : instructions)
  {
    instruction->optimize();
  }
}
