#pragma once
#include <vector>
#include <utility>
#include <unordered_map>

#include "general.hpp"

enum class Instr
{
  GET,
  PUT,
  LOAD,
  LOADI,
  STORE,
  STOREI,
  ADD,
  ADDI,
  SUB,
  SUBI,
  SHR,
  SHL,
  INC,
  DEC,
  ZERO,
  JUMP,
  JZERO,
  JODD,
  HALT
};

class InstructionRegistry
{
  const static std::string instrStr[19];

  std::vector<std::pair<Instr,lint>> instructions;

  std::unordered_map<lint,lint> labels; // <label id, address>

public:

  std::vector<std::pair<Instr,lint>>& getInstructions();
  std::unordered_map<lint,lint>& getLabels();

  lint addInstruction(Instr i, lint param);
  lint addInstruction(Instr i);

  lint append(InstructionRegistry ir);

  void modInstructionParam(lint addr, lint param);

  void setLabel(lint addr, lint labelId);
  void translateLabels();

  std::ostream& print( std::ostream &stream );
};
