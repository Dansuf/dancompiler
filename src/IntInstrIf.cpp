#include <cstdio>
#include <string>

#include "IntInstrIf.hpp"


void IntInstrIf::dbgPrint()
{
  printf("IF %s",this->val1.c_str());

  switch(this->comp)
  {
    case Comparator::EQ:
      printf(" = ");
      break;
    case Comparator::NEQ:
      printf(" <> ");
      break;
    case Comparator::LT:
      printf(" < ");
      break;
    case Comparator::GT:
      printf(" > ");
      break;
    case Comparator::LTE:
      printf(" <= ");
      break;
    case Comparator::GTE:
      printf(" >= ");
      break;
    default:
      printf(" ? ");
  }

  printf("%s THEN\n",this->val2.c_str());

  this->block.dbgPrint();

  if(!this->elBlock.empty())
  {
    printf("ELSE\n");
    this->elBlock.dbgPrint();
  }

  printf("ENDIF\n");
}

InstructionRegistry IntInstrIf::translateEq(VariableRegistry& variables)
{
  lint next = variables.newLabel();
  lint cmds1 = variables.newLabel();
  lint cmds2;
  if(!this->elBlock.empty()) cmds2 = variables.newLabel();
  lint endif = variables.newLabel();

  IntInstr sub = IntInstr(IntInstrType::SUB,"",this->val2,this->val1);
  IntInstr revSub = IntInstr(IntInstrType::SUB,"",this->val1,this->val2);

  lint endifAddr, cmds2Addr;

  InstructionRegistry ir = sub.translate(variables);
  ir.addInstruction(Instr::JZERO,next);
  lint nextAddr = ir.addInstruction(Instr::JUMP,this->elBlock.empty() ? endif : cmds2)+1;
  ir.append(revSub.translate(variables));
  ir.addInstruction(Instr::JZERO,cmds1);
  lint cmds1Addr = ir.addInstruction(Instr::JUMP,this->elBlock.empty() ? endif : cmds2) + 1;
  if(this->elBlock.empty())
  {
    endifAddr = ir.append(this->block.translate(variables)) + 1;
  }
  else
  {
    ir.append(this->block.translate(variables));
    cmds2Addr = ir.addInstruction(Instr::JUMP,endif) + 1;
    endifAddr = ir.append(this->elBlock.translate(variables)) + 1;
  }


  ir.setLabel(nextAddr,next);
  ir.setLabel(cmds1Addr,cmds1);
  if(!this->elBlock.empty()) ir.setLabel(cmds2Addr,cmds2);
  ir.setLabel(endifAddr,endif);

  return ir;
}

InstructionRegistry IntInstrIf::translateNeq(VariableRegistry& variables)
{
  lint next = variables.newLabel();
  lint cmds1 = variables.newLabel();
  lint cmds2;
  if(!this->elBlock.empty()) cmds2 = variables.newLabel();
  lint endif = variables.newLabel();

  IntInstr sub = IntInstr(IntInstrType::SUB,"",this->val2,this->val1);
  IntInstr revSub = IntInstr(IntInstrType::SUB,"",this->val1,this->val2);

  lint endifAddr, cmds2Addr;

  InstructionRegistry ir = sub.translate(variables);
  ir.addInstruction(Instr::JZERO,next);
  lint nextAddr = ir.addInstruction(Instr::JUMP,cmds1) + 1;
  ir.append(revSub.translate(variables));
  lint cmds1Addr = ir.addInstruction(Instr::JZERO,this->elBlock.empty() ? endif : cmds2) + 1;
  if(this->elBlock.empty())
  {
    endifAddr = ir.append(this->block.translate(variables)) + 1;
  }
  else
  {
    ir.append(this->block.translate(variables));
    cmds2Addr = ir.addInstruction(Instr::JUMP,endif) + 1;
    endifAddr = ir.append(this->elBlock.translate(variables)) + 1;
  }

  ir.setLabel(nextAddr,next);
  ir.setLabel(cmds1Addr,cmds1);
  if(!this->elBlock.empty()) ir.setLabel(cmds2Addr,cmds2);
  ir.setLabel(endifAddr,endif);

  return ir;
}

InstructionRegistry IntInstrIf::translateLt(VariableRegistry& variables)
{
  lint endif = variables.newLabel();
  lint cmds2;
  if(!this->elBlock.empty()) cmds2 = variables.newLabel();

  IntInstr sub = IntInstr(IntInstrType::SUB,"",this->val2,this->val1);

  InstructionRegistry ir = sub.translate(variables);

  lint cmds2Addr, endifAddr;

  if(this->elBlock.empty())
  {
    ir.addInstruction(Instr::JZERO,endif);
    endifAddr = ir.append(this->block.translate(variables)) + 1;
  }
  else
  {
    ir.addInstruction(Instr::JZERO,cmds2);
    ir.append(this->block.translate(variables));
    cmds2Addr = ir.addInstruction(Instr::JUMP,endif) + 1;
    endifAddr = ir.append(this->elBlock.translate(variables)) + 1;
  }

  ir.setLabel(endifAddr,endif);
  if(!this->elBlock.empty()) ir.setLabel(cmds2Addr,cmds2);

  return ir;
}

InstructionRegistry IntInstrIf::translateGt(VariableRegistry& variables)
{
  lint endif = variables.newLabel();
  lint cmds2;
  if(!this->elBlock.empty()) cmds2 = variables.newLabel();

  IntInstr sub = IntInstr(IntInstrType::SUB,"",this->val1,this->val2);

  InstructionRegistry ir = sub.translate(variables);

  lint cmds2Addr, endifAddr;

  if(this->elBlock.empty())
  {
    ir.addInstruction(Instr::JZERO,endif);
    endifAddr = ir.append(this->block.translate(variables)) + 1;
  }
  else
  {
    ir.addInstruction(Instr::JZERO,cmds2);
    ir.append(this->block.translate(variables));
    cmds2Addr = ir.addInstruction(Instr::JUMP,endif) + 1;
    endifAddr = ir.append(this->elBlock.translate(variables)) + 1;
  }

  ir.setLabel(endifAddr,endif);
  if(!this->elBlock.empty()) ir.setLabel(cmds2Addr,cmds2);

  return ir;
}

InstructionRegistry IntInstrIf::translateLte(VariableRegistry& variables)
{
  lint cmds1 = variables.newLabel();
  lint endif = variables.newLabel();

  IntInstr sub = IntInstr(IntInstrType::SUB,"",this->val1,this->val2);

  InstructionRegistry ir = sub.translate(variables);
  ir.addInstruction(Instr::JZERO,cmds1);
  ir.append(this->elBlock.translate(variables));
  lint cmds1Addr = ir.addInstruction(Instr::JUMP,endif) + 1;
  lint endifAddr = ir.append(this->block.translate(variables)) + 1;

  ir.setLabel(cmds1Addr,cmds1);
  ir.setLabel(endifAddr,endif);

  return ir;
}

InstructionRegistry IntInstrIf::translateGte(VariableRegistry& variables)
{
  lint cmds1 = variables.newLabel();
  lint endif = variables.newLabel();

  IntInstr sub = IntInstr(IntInstrType::SUB,"",this->val2,this->val1);

  InstructionRegistry ir = sub.translate(variables);
  ir.addInstruction(Instr::JZERO,cmds1);
  ir.append(this->elBlock.translate(variables));
  lint cmds1Addr = ir.addInstruction(Instr::JUMP,endif) + 1;
  lint endifAddr = ir.append(this->block.translate(variables)) + 1;

  ir.setLabel(cmds1Addr,cmds1);
  ir.setLabel(endifAddr,endif);

  return ir;
}

InstructionRegistry IntInstrIf::translate(VariableRegistry& variables)
{
  switch(this->comp)
  {
    case Comparator::EQ:
      return this->translateEq(variables);
      break;
    case Comparator::NEQ:
      return this->translateNeq(variables);
      break;
    case Comparator::LT:
      return this->translateLt(variables);
      break;
    case Comparator::GT:
      return this->translateGt(variables);
      break;
    case Comparator::LTE:
      return this->translateLte(variables);
      break;
    case Comparator::GTE:
      return this->translateGte(variables);
      break;
  }
}

void IntInstrIf::optimize()
{
  this->block.optimize();
  this->elBlock.optimize();
}
