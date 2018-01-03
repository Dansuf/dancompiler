#include <iostream>

#include "InstructionRegistry.hpp"

const std::string InstructionRegistry::instrStr[19] = {"GET","PUT","LOAD","LOADI","STORE","STOREI","ADD","ADDI","SUB","SUBI","SHR","SHL","INC","DEC","ZERO","JUMP","JZERO","JODD","HALT"};

bool InstructionRegistry::isLabelled(lint addr)
{
  bool labelled = false;
  for(auto &label : this->labels)
  {
    if(label.second == addr)
    {
      labelled = true;
      break;
    }
  }
  return labelled;
}

bool InstructionRegistry::isAnyLabelled(lint firstAddr, lint lastAddr)
{
  bool labelled = false;
  for(auto &label : this->labels)
  {
    for(lint i = firstAddr; i <= lastAddr; i++)
    {
      if(label.second == i)
      {
        labelled = true;
        break;
      }
    }
    if(labelled) break;
  }
  return labelled;
}

void InstructionRegistry::deleteInstructions(lint firstInstr, lint lastInstr)
{
  //TODO check if addresses in bounds of this->instructions
  lint shift = lastInstr - firstInstr + 1;

  for(lint i = lastInstr+1; i < this->instructions.size(); i++)
  {
    this->instructions[i-shift] = this->instructions[i];
  }

  for(auto &label : this->labels)
  {
    if(label.second > lastInstr)
    {
      label.second -= shift;
    }
  }
}


std::vector<std::pair<Instr,lint>>& InstructionRegistry::getInstructions()
{
  return this->instructions;
}

std::unordered_map<lint,lint>& InstructionRegistry::getLabels()
{
  return this->labels;
}

lint InstructionRegistry::addInstruction(Instr i, lint param)
{
  this->instructions.push_back(std::make_pair(i,param));
  return this->instructions.size()-1;
}

lint InstructionRegistry::addInstruction(Instr i)
{
  this->instructions.push_back(std::make_pair(i,0));
  return this->instructions.size()-1;
}

void InstructionRegistry::modInstructionParam(lint addr, lint param)
{
  //TODO case with addr out of range
  auto instr = this->instructions.at(addr);
  this->instructions[addr] = std::make_pair(instr.first,param);
}

void InstructionRegistry::setLabel(lint addr, lint labelId)
{
  //TODO wrong addr, wrong labelid, labelid already used etc
  labels.insert(std::make_pair(labelId,addr));
}

void InstructionRegistry::translateLabels()
{
  for(auto& instr : instructions)
  {
    if(instr.first == Instr::JUMP || instr.first == Instr::JODD || instr.first == Instr::JZERO)
    {
      if(instr.second < 0)
      {
        instr.second = labels.at(instr.second); //TODO out of range
      }
    }
  }
}

lint InstructionRegistry::append(InstructionRegistry ir)
{
  lint lastAddr = this->instructions.size();

  auto instructions = ir.getInstructions();
  this->instructions.insert(this->instructions.end(),instructions.begin(),instructions.end());

  auto labels = ir.getLabels();

  for(auto label : labels) //TODO better ??
  {
    this->labels.insert(std::make_pair(label.first,label.second+lastAddr));
  }

  return this->instructions.size()-1;
}

void InstructionRegistry::optimize()
{
  // Eliminate LOAD in STORE-LOAD pairs
  for(lint i = 0; i < this->instructions.size()-1; i++)
  {

    if(instructions[i].second == instructions[i+1].second && instructions[i].first == Instr::STORE && instructions[i+1].first == Instr::LOAD)
    {

      if(!this->isAnyLabelled(i,i+1))
      {
        this->deleteInstructions(i+1,i+1);
      }
    }
  }
}

std::ostream& InstructionRegistry::print( std::ostream &stream )
{
  for(auto instruction : instructions)
  {
    switch(instruction.first)
    {
      case Instr::LOAD:
      case Instr::LOADI:
      case Instr::STORE:
      case Instr::STOREI:
      case Instr::ADD:
      case Instr::ADDI:
      case Instr::SUB:
      case Instr::SUBI:
      case Instr::JUMP:
      case Instr::JZERO:
      case Instr::JODD:
        stream << this->instrStr[(int)instruction.first] << " " << instruction.second << std::endl;
        break;
      default:
        stream << this->instrStr[(int)instruction.first] << std::endl;
    }
  }
  return stream;
}
