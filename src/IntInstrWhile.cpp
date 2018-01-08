#include "IntInstrWhile.hpp"

IntInstrWhile::IntInstrWhile(std::string val1, std::string val2, Comparator comp, IntInstrBlock block, IntInstrBlock valInitBlock): val1(val1), val2(val2), comp(comp), block(block)
{
  this->valInitBlock = valInitBlock;
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

  lint cmdsAddr, nextAddr, endwhileAddr;

  InstructionRegistry ir;

  if(VariableRegistry::isConst(this->val2) && VariableRegistry::getConstVal(this->val2) <= 15 )
  {
    int constant = VariableRegistry::getConstVal(this->val2);

    ir = this->valInitBlock.translate(variables);
    IntInstr::addLoadInstruction(variables,ir,this->val1);

    for(int i = 0; i < constant; i++)
    {
      ir.addInstruction(Instr::JZERO,endwhile);
      ir.addInstruction(Instr::DEC);
    }

    ir.addInstruction(Instr::JZERO,cmds);
    cmdsAddr =ir.addInstruction(Instr::JUMP,endwhile) + 1;
    ir.append(this->block.translate(variables));

    ir.append(this->valInitBlock.translate(variables));
    IntInstr::addLoadInstruction(variables,ir,this->val1);

    for(int i = 0; i < constant; i++)
    {
      ir.addInstruction(Instr::JZERO,endwhile);
      ir.addInstruction(Instr::DEC);
    }

    endwhileAddr = ir.addInstruction(Instr::JZERO,cmds) + 1;
  }
  else if(VariableRegistry::isConst(this->val2))
  {
    std::string tmp = variables.genVariable();

    ir.append(this->valInitBlock.translate(variables));
    IntInstr::addLoadInstruction(variables,ir,this->val2);
    ir.addInstruction(Instr::STORE,variables.getIndex(tmp));
    IntInstr::addSubInstruction(variables,ir,this->val1);
    ir.addInstruction(Instr::JZERO,next);
    cmdsAddr = ir.addInstruction(Instr::JUMP,endwhile) + 1;
    ir.append(this->block.translate(variables));

    ir.append(this->subBlock.translate(variables));
    ir.addInstruction(Instr::JZERO,next);
    nextAddr = ir.addInstruction(Instr::JUMP,endwhile) + 1;
    ir.append(this->valInitBlock.translate(variables));
    IntInstr::addLoadInstruction(variables,ir,this->val1);
    IntInstr::addSubInstruction(variables,ir,tmp);
    endwhileAddr = ir.addInstruction(Instr::JZERO,cmds) + 1;
  }
  else
  {
    ir = this->subBlock.translate(variables);
    ir.addInstruction(Instr::JZERO,next);
    cmdsAddr = ir.addInstruction(Instr::JUMP,endwhile) + 1;
    ir.append(this->block.translate(variables));

    ir.append(this->subBlock.translate(variables));
    ir.addInstruction(Instr::JZERO,next);
    nextAddr = ir.addInstruction(Instr::JUMP,endwhile) + 1;
    ir.append(this->revSubBlock.translate(variables));
    endwhileAddr = ir.addInstruction(Instr::JZERO,cmds) + 1;
  }


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

  lint cmdsAddr, nextAddr, endwhileAddr;

  InstructionRegistry ir;

  if(VariableRegistry::isConst(this->val2) && VariableRegistry::getConstVal(this->val2) <= 15 )
  {
    int constant = VariableRegistry::getConstVal(this->val2);

    ir = this->valInitBlock.translate(variables);
    IntInstr::addLoadInstruction(variables,ir,this->val1);

    for(int i = 0; i < constant; i++)
    {
      ir.addInstruction(Instr::JZERO,cmds);
      ir.addInstruction(Instr::DEC);
    }

    cmdsAddr = ir.addInstruction(Instr::JZERO,endwhile) + 1;

    ir.append(this->block.translate(variables));

    ir.append(this->valInitBlock.translate(variables));
    IntInstr::addLoadInstruction(variables,ir,this->val1);

    endwhileAddr;
    for(int i = 0; i < constant; i++)
    {
      ir.addInstruction(Instr::JZERO,cmds);
      ir.addInstruction(Instr::DEC);
    }
    ir.addInstruction(Instr::JZERO,endwhile);
    endwhileAddr = ir.addInstruction(Instr::JUMP,cmds) + 1;
  }
  else if(VariableRegistry::isConst(this->val2))
  {
    std::string tmp = variables.genVariable();

    ir.append(this->valInitBlock.translate(variables));
    IntInstr::addLoadInstruction(variables,ir,this->val2);
    ir.addInstruction(Instr::STORE,variables.getIndex(tmp));
    IntInstr::addSubInstruction(variables,ir,this->val1);
    ir.addInstruction(Instr::JZERO,next);
    nextAddr = ir.addInstruction(Instr::JUMP,cmds) + 1;
    ir.append(this->valInitBlock.translate(variables));
    IntInstr::addLoadInstruction(variables,ir,this->val1);
    IntInstr::addSubInstruction(variables,ir,tmp);
    cmdsAddr = ir.addInstruction(Instr::JZERO,endwhile) + 1;
    ir.append(this->block.translate(variables));

    ir.append(this->valInitBlock.translate(variables)); //TODO what if theres unused array variable but its precomputed?? - optimize
    ir.addInstruction(Instr::LOAD,variables.getIndex(tmp));
    IntInstr::addSubInstruction(variables,ir,this->val1);
    ir.addInstruction(Instr::JZERO,next);
    endwhileAddr = ir.addInstruction(Instr::JUMP,cmds) + 1;
  }
  else
  {
    ir = this->subBlock.translate(variables);
    ir.addInstruction(Instr::JZERO,next);
    nextAddr = ir.addInstruction(Instr::JUMP,cmds) + 1;
    ir.append(this->revSubBlock.translate(variables));
    cmdsAddr = ir.addInstruction(Instr::JZERO,endwhile) + 1;
    ir.append(this->block.translate(variables));

    ir.append(this->subBlock.translate(variables));
    ir.addInstruction(Instr::JZERO,next);
    endwhileAddr = ir.addInstruction(Instr::JUMP,cmds) + 1;
  }

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
      if(VariableRegistry::isConst(this->val1) && !VariableRegistry::isConst(this->val2))
      {
        std::string tmp = this->val1;
        this->val1 = this->val2;
        this->val2 = tmp;
      }
      else if(VariableRegistry::isConst(this->val1) && VariableRegistry::isConst(this->val2))
      {
        if(VariableRegistry::getConstVal(this->val1) != VariableRegistry::getConstVal(this->val2))
        {
          return InstructionRegistry();
        }
      }
      return this->translateEq(variables);
      break;
    case Comparator::NEQ:
      if(VariableRegistry::isConst(this->val1) && !VariableRegistry::isConst(this->val2))
      {
        std::string tmp = this->val1;
        this->val1 = this->val2;
        this->val2 = tmp;
      } else if(VariableRegistry::isConst(this->val1) && VariableRegistry::isConst(this->val2))
      {
        if(VariableRegistry::getConstVal(this->val1) == VariableRegistry::getConstVal(this->val2))
        {
          return InstructionRegistry();
        }
      }
      return this->translateNeq(variables);
      break;
    case Comparator::LT:
      if(VariableRegistry::isConst(this->val1) && VariableRegistry::isConst(this->val2))
      {
        if(VariableRegistry::getConstVal(this->val1) >= VariableRegistry::getConstVal(this->val2))
        {
          return InstructionRegistry();
        }
      }
      return this->translateLt(variables);
      break;
    case Comparator::GT:
      if(VariableRegistry::isConst(this->val1) && VariableRegistry::isConst(this->val2))
      {
        if(VariableRegistry::getConstVal(this->val1) <= VariableRegistry::getConstVal(this->val2))
        {
          return InstructionRegistry();
        }
      }
      return this->translateGt(variables);
      break;
    case Comparator::LTE:
      if(VariableRegistry::isConst(this->val1) && VariableRegistry::isConst(this->val2))
      {
        if(VariableRegistry::getConstVal(this->val1) > VariableRegistry::getConstVal(this->val2))
        {
          return InstructionRegistry();
        }
      }
      return this->translateLte(variables);
      break;
    case Comparator::GTE:
      if(VariableRegistry::isConst(this->val1) && VariableRegistry::isConst(this->val2))
      {
        if(VariableRegistry::getConstVal(this->val1) < VariableRegistry::getConstVal(this->val2))
        {
          return InstructionRegistry();
        }
      }
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
