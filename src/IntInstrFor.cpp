#include "IntInstrFor.hpp"

IntInstrFor::IntInstrFor(std::string iterator, std::string val1, std::string val2, bool downTo, IntInstrBlock block, IntInstrBlock valInitBlock) :
                          iterator(iterator), val1(val1), val2(val2), downTo(downTo), block(block)
{
  if(!downTo)
  {
    this->subBlock = valInitBlock;
    this->subBlock.addInstr((IntInstrAbstr*) new IntInstr(IntInstrType::SET,iterator,val1));
  }
  else
  {
    this->subBlock = valInitBlock;
    this->subBlock.addInstr((IntInstrAbstr*) new IntInstr(IntInstrType::SET,iterator,val1));
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
  lint endfor = variables.newLabel();

  lint ctr = variables.getForCounter();

  InstructionRegistry ir;
  if(downTo) //TODO better way
  {
    std::string val2 = this->val2;
    if(VariableRegistry::isConst(this->val2)) //TODO change it to IntInstrType::SUB or include case with val2 < 26
    {
      std::string tmp = variables.getAssemblerTemp();
      IntInstr::addLoadInstruction(variables,ir,this->val2);
      IntInstr::addStoreInstruction(variables,ir,tmp);
      val2 = tmp;
    }
    ir.append(this->subBlock.translate(variables));
    ir.addInstruction(Instr::INC);
    IntInstr::addSubInstruction(variables,ir,val2);
  }
  else
  {
    ir = this->subBlock.translate(variables);
    IntInstr::addLoadInstruction(variables,ir,val2);
    ir.addInstruction(Instr::INC);
    ir.addInstruction(Instr::SUB,variables.getIndex(iterator));
  }
  lint loopAddr = ir.addInstruction(Instr::JZERO,endfor) + 1;
  ir.addInstruction(Instr::STORE,ctr);
  ir.append(this->block.translate(variables));
  ir.addInstruction(Instr::LOAD,variables.getIndex(iterator));
  if(downTo) ir.addInstruction(Instr::DEC);
  else ir.addInstruction(Instr::INC);
  ir.addInstruction(Instr::STORE,variables.getIndex(iterator));
  ir.addInstruction(Instr::LOAD,ctr);
  ir.addInstruction(Instr::DEC);
  ir.addInstruction(Instr::JZERO,endfor);
  lint endforAddr = ir.addInstruction(Instr::JUMP,loop) + 1;

  ir.setLabel(loopAddr,loop);
  ir.setLabel(endforAddr,endfor);

  return ir;
}

void IntInstrFor::optimize()
{
  this->subBlock.optimize();
  this->block.optimize();
}

void IntInstrFor::propagateConstants(std::unordered_map<std::string, std::string>& constants)
{
  for(auto it = constants.begin(); it != constants.end();)
  {
    if(this->block.modifiesVariable(it->first)) //TODO different first run of the loop
    {
      constants.erase((it++)->first);
    }
    else
    {
      ++it;
    }
  }

  // this->subBlock.propagateConstants(constants);
  // for(auto it = constants.begin(); it != constants.end();)
  // {
  //   if(VariableRegistry::isTemp(it->first))
  //   {
  //     constants.erase((it++)->first);
  //   }
  //   else
  //   {
  //     ++it;
  //   }
  // }
  //
  this->block.propagateConstants(constants);

  for(auto& constant : constants)
  {
    if(constant.first == this->val1) this->val1 = constant.second;
    if(constant.first == this->val2) this->val2 = constant.second;
  }

  this->optimize();
}

bool IntInstrFor::modifiesVariable(std::string name)
{
  return this->block.modifiesVariable(name);
}
