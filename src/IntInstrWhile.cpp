#include "IntInstrWhile.hpp"

IntInstrWhile::IntInstrWhile(std::string val1, std::string val2, Comparator comp, IntInstrBlock block, IntInstrBlock valInitBlock): val1(val1), val2(val2), comp(comp), block(block)
{
  switch(this->comp)
  {
    case Comparator::EQ:
    case Comparator::NEQ:
      this->subBlock = valInitBlock;
      this->subBlock.addInstr((IntInstrAbstr*)new IntInstr(IntInstrType::SUB,"",this->val2,this->val1));
      this->revSubBlock = valInitBlock;
      this->revSubBlock.addInstr((IntInstrAbstr*)new IntInstr(IntInstrType::SUB,"",this->val1,this->val2));
      break;
    case Comparator::LT:
    case Comparator::GTE:
      this->subBlock = valInitBlock;
      this->subBlock.addInstr((IntInstrAbstr*)new IntInstr(IntInstrType::SUB,"",this->val2,this->val1));
      break;
    case Comparator::GT:
    case Comparator::LTE:
      this->subBlock = valInitBlock;
      this->subBlock.addInstr((IntInstrAbstr*)new IntInstr(IntInstrType::SUB,"",this->val1,this->val2));
      break;
  }
}

void IntInstrWhile::dbgPrint()
{
  printf("WHILE %s",this->val1.c_str());

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

  printf("%s DO\n",this->val2.c_str());

  this->block.dbgPrint();

  printf("ENDWHILE\n");
}

InstructionRegistry IntInstrWhile::translateEq(VariableRegistry& variables)
{
  lint next = variables.newLabel();
  lint cmds = variables.newLabel();
  lint endwhile = variables.newLabel();

  InstructionRegistry ir = this->subBlock.translate(variables);
  ir.addInstruction(Instr::JZERO,next);
  lint cmdsAddr = ir.addInstruction(Instr::JUMP,endwhile) + 1;
  ir.append(this->block.translate(variables));

  ir.append(this->subBlock.translate(variables));
  ir.addInstruction(Instr::JZERO,next);
  lint nextAddr = ir.addInstruction(Instr::JUMP,endwhile) + 1;
  ir.append(this->revSubBlock.translate(variables));
  lint endwhileAddr = ir.addInstruction(Instr::JZERO,cmds) + 1;


  ir.setLabel(nextAddr,next);
  ir.setLabel(cmdsAddr,cmds);
  ir.setLabel(endwhileAddr,endwhile);

  return ir;
}

InstructionRegistry IntInstrWhile::translateNeq(VariableRegistry& variables)
{
  lint next = variables.newLabel();
  lint cmds = variables.newLabel();
  lint endwhile = variables.newLabel();

  InstructionRegistry ir = this->subBlock.translate(variables);
  ir.addInstruction(Instr::JZERO,next);
  lint nextAddr = ir.addInstruction(Instr::JUMP,cmds) + 1;
  ir.append(this->revSubBlock.translate(variables));
  lint cmdsAddr = ir.addInstruction(Instr::JZERO,endwhile) + 1;
  ir.append(this->block.translate(variables));

  ir.append(this->subBlock.translate(variables));
  ir.addInstruction(Instr::JZERO,next);
  lint endwhileAddr = ir.addInstruction(Instr::JUMP,cmds) + 1;

  ir.setLabel(nextAddr,next);
  ir.setLabel(cmdsAddr,cmds);
  ir.setLabel(endwhileAddr,endwhile);

  return ir;
}

InstructionRegistry IntInstrWhile::translateLt(VariableRegistry& variables)
{
  lint endwhile = variables.newLabel();
  lint whil = variables.newLabel();

  InstructionRegistry ir = this->subBlock.translate(variables);

  lint whileAddr = ir.addInstruction(Instr::JZERO,endwhile) + 1;
  ir.append(this->block.translate(variables));
  ir.append(this->subBlock.translate(variables));
  ir.addInstruction(Instr::JZERO,endwhile);
  lint endwhileAddr = ir.addInstruction(Instr::JUMP,whil) + 1;

  ir.setLabel(endwhileAddr,endwhile);
  ir.setLabel(whileAddr,whil);

  return ir;
}

InstructionRegistry IntInstrWhile::translateGt(VariableRegistry& variables)
{
  lint endwhile = variables.newLabel();
  lint whil = variables.newLabel();

  InstructionRegistry ir = this->subBlock.translate(variables);

  lint whileAddr = ir.addInstruction(Instr::JZERO,endwhile) + 1;
  ir.append(this->block.translate(variables));
  ir.append(this->subBlock.translate(variables));
  ir.addInstruction(Instr::JZERO,endwhile);
  lint endwhileAddr = ir.addInstruction(Instr::JUMP,whil) + 1;

  ir.setLabel(endwhileAddr,endwhile);
  ir.setLabel(whileAddr,whil);

  return ir;
}

InstructionRegistry IntInstrWhile::translateLte(VariableRegistry& variables)
{
  lint cmds = variables.newLabel();
  lint endwhile = variables.newLabel();

  InstructionRegistry ir = this->subBlock.translate(variables);
  ir.addInstruction(Instr::JZERO,cmds);
  lint cmdsAddr = ir.addInstruction(Instr::JUMP,endwhile) + 1;
  ir.append(this->block.translate(variables));
  ir.append(this->subBlock.translate(variables));
  lint endwhileAddr = ir.addInstruction(Instr::JZERO,cmds) + 1;

  ir.setLabel(cmdsAddr,cmds);
  ir.setLabel(endwhileAddr,endwhile);

  return ir;
}

InstructionRegistry IntInstrWhile::translateGte(VariableRegistry& variables)
{
  lint cmds = variables.newLabel();
  lint endwhile = variables.newLabel();

  InstructionRegistry ir = this->subBlock.translate(variables);
  ir.addInstruction(Instr::JZERO,cmds);
  lint cmdsAddr = ir.addInstruction(Instr::JUMP,endwhile) + 1;
  ir.append(this->block.translate(variables));
  ir.append(this->subBlock.translate(variables));
  lint endwhileAddr = ir.addInstruction(Instr::JZERO,cmds) + 1;

  ir.setLabel(cmdsAddr,cmds);
  ir.setLabel(endwhileAddr,endwhile);

  return ir;
}

InstructionRegistry IntInstrWhile::translate(VariableRegistry& variables)
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

void IntInstrWhile::optimize()
{
  this->subBlock.optimize();
  this->revSubBlock.optimize();
  this->block.optimize();
}
