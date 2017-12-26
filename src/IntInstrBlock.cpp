#include <cstdio>

#include "IntInstrBlock.hpp"

void IntInstrBlock::addInstr(IntInstrAbstr* instr)
{
  this->instructions.push_back(std::shared_ptr<IntInstrAbstr>(instr));
}

std::vector<std::shared_ptr<IntInstrAbstr>> IntInstrBlock::getInstructions()
{
  return this->instructions;
}

void IntInstrBlock::append(IntInstrBlock block)
{
  auto instructions = block.getInstructions();

  this->instructions.insert(this->instructions.end(),instructions.begin(),instructions.end());
}

bool IntInstrBlock::empty()
{
  return this->instructions.empty();
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
