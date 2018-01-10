#include "IntInstrFor.hpp"

IntInstrFor::IntInstrFor(std::string iterator, std::string val1, std::string val2, bool downTo, IntInstrBlock block, IntInstrBlock valInitBlock) :
                          iterator(iterator), val1(val1), val2(val2), downTo(downTo), block(block)
{
  if(!downTo)
  {
    this->valInitBlock = valInitBlock;
  }
  else
  {
    this->valInitBlock = valInitBlock;
  }
}

void IntInstrFor::dbgPrint()
{
  printf("FOR %s FROM %s",this->iterator.c_str(),this->val1.c_str());
  if(this->downTo)
    printf(" DOWNTO ");
  else
    printf(" TO ");
  printf("%s DO\n",this->val2.c_str());
  this->block.dbgPrint();
  printf("ENDFOR\n");
}

InstructionRegistry IntInstrFor::translate(VariableRegistry& variables)
{
  //TODO don't create iterator variable if not used
  lint loop = variables.newLabel();
  lint loop1 = variables.newLabel();
  lint endfor = variables.newLabel();
  lint altfor = variables.newLabel();
  lint endaltfor = variables.newLabel();

  lint loopAddr, loop1Addr, endforAddr, altforAddr, endaltforAddr;

  InstructionRegistry ir;

  if(!downTo && VariableRegistry::isConst(this->val1) && VariableRegistry::isConst(this->val2) && VariableRegistry::getConstVal(this->val1) > VariableRegistry::getConstVal(this->val2)
      || downTo &&  VariableRegistry::isConst(this->val1) && VariableRegistry::isConst(this->val2) && VariableRegistry::getConstVal(this->val1) < VariableRegistry::getConstVal(this->val2))
  {
    return InstructionRegistry();
  }
  else if(downTo && VariableRegistry::isConst(this->val2) && VariableRegistry::getConstVal(this->val2) == 0)
  {
    IntInstr::addLoadInstruction(variables,ir,this->val1);
    ir.addInstruction(Instr::JZERO,endaltfor);
    loop1Addr = ir.addInstruction(Instr::STORE,variables.getIndex(this->iterator));
    ir.append(this->block.translate(variables));
    ir.addInstruction(Instr::LOAD,variables.getIndex(this->iterator));
    ir.addInstruction(Instr::DEC);
    ir.addInstruction(Instr::JZERO,endaltfor);
    endaltforAddr = ir.addInstruction(Instr::JUMP,loop1) + 1;
    ir.addInstruction(Instr::STORE,variables.getIndex(this->iterator));
    endforAddr = ir.append(this->block.translate(variables)) + 1;
  }
  else
  {
    lint tmp = variables.getForCounter(); // it's not a counter exactly but well

    ir = this->valInitBlock.translate(variables);
    IntInstr::addLoadInstruction(variables,ir,this->val2);
    if(downTo) ir.addInstruction(Instr::JZERO,altfor);
    ir.addInstruction(Instr::STORE,tmp);
    IntInstr::addLoadInstruction(variables,ir,this->val1);
    ir.addInstruction(Instr::STORE,variables.getIndex(this->iterator));
    if(downTo)
    {
      ir.addInstruction(Instr::INC);
    }
    ir.addInstruction(Instr::SUB,tmp);

    if(downTo)
    {
      loopAddr = ir.addInstruction(Instr::JZERO,endfor) + 1;
    }
    else
    {
      ir.addInstruction(Instr::JZERO,loop);
      loopAddr = ir.addInstruction(Instr::JUMP,endfor) + 1;
    }
    ir.append(this->block.translate(variables));
    ir.addInstruction(Instr::LOAD,variables.getIndex(this->iterator));
    if(downTo)
    {
      ir.addInstruction(Instr::DEC);
    }
    else
    {
      ir.addInstruction(Instr::INC);
    }
    ir.addInstruction(Instr::STORE,variables.getIndex(this->iterator));
    if(downTo)
    {
      ir.addInstruction(Instr::INC);
      ir.addInstruction(Instr::SUB,tmp);
      ir.addInstruction(Instr::JZERO,endfor);
      altforAddr = ir.addInstruction(Instr::JUMP,loop) + 1;

      IntInstr::addLoadInstruction(variables,ir,this->val1);
      ir.addInstruction(Instr::JZERO,endaltfor);
      loop1Addr = ir.addInstruction(Instr::STORE,variables.getIndex(this->iterator));
      ir.append(this->block.translate(variables));
      ir.addInstruction(Instr::LOAD,variables.getIndex(this->iterator));
      ir.addInstruction(Instr::DEC);
      ir.addInstruction(Instr::JZERO,endaltfor);
      endaltforAddr = ir.addInstruction(Instr::JUMP,loop1) + 1;
      ir.addInstruction(Instr::STORE,variables.getIndex(this->iterator));
      endforAddr = ir.append(this->block.translate(variables)) + 1;
    }
    else
    {
      ir.addInstruction(Instr::SUB,tmp);
      endforAddr = ir.addInstruction(Instr::JZERO,loop) + 1;
    }
  }

  ir.setLabel(loopAddr,loop);
  ir.setLabel(endforAddr,endfor);
  if(downTo)
  {
    ir.setLabel(loop1Addr,loop1);
    ir.setLabel(endaltforAddr,endaltfor);
    ir.setLabel(altforAddr,altfor);
  }

  return ir;
}

void IntInstrFor::optimize()
{
  this->valInitBlock.optimize();
  this->block.optimize();
}
