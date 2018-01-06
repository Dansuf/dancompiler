#include <cstdio>
#include <vector>

#include "IntInstr.hpp"
#include "InstructionRegistry.hpp"

const std::string IntInstr::instrStr[] = {"GET","PUT","ADD","SUB","MUL","DIV","MOD","SET","HALT"};

/*
 * @return address to the first generated instruction
 */
lint IntInstr::addLoadInstruction(VariableRegistry& variables, InstructionRegistry& instructions, std::string argument)
{
  lint firstAddr;
  if(VariableRegistry::isConst(argument))
  {
    lint val = VariableRegistry::getConstVal(argument);

    std::vector<bool> bits;

    while(val > 0)
    {
      bits.push_back(val%2 == 0 ? false : true);
      val /= 2;
    }

    firstAddr = instructions.addInstruction(Instr::ZERO);

    for(auto it = bits.rbegin(); it != bits.rend(); it++)
    {
      if(*it) instructions.addInstruction(Instr::INC);
      if(it+1 != bits.rend())instructions.addInstruction(Instr::SHL);
    }
  }
  else
  {
    Instr i = VariableRegistry::isPointer(argument) ? Instr::LOADI : Instr::LOAD;
    firstAddr = instructions.addInstruction(i,variables.getIndex(argument));
  }
  return firstAddr;
}

void IntInstr::addAddInstruction(VariableRegistry& variables, InstructionRegistry& instructions, std::string argument)
{
  Instr i = VariableRegistry::isPointer(argument) ? Instr::ADDI : Instr::ADD;
  instructions.addInstruction(i,variables.getIndex(argument));
}

void IntInstr::addSubInstruction(VariableRegistry& variables, InstructionRegistry& instructions, std::string argument)
{
  Instr i = VariableRegistry::isPointer(argument) ? Instr::SUBI : Instr::SUB;
  instructions.addInstruction(i,variables.getIndex(argument));
}

lint IntInstr::addStoreInstruction(VariableRegistry& variables, InstructionRegistry& instructions, std::string argument)
{
  Instr i = VariableRegistry::isPointer(argument) ? Instr::STOREI : Instr::STORE;
  return instructions.addInstruction(i,variables.getIndex(argument));
}

void IntInstr::addMulInstruction(VariableRegistry& variables, InstructionRegistry& instructions, std::string argument1, std::string argument2, std::string out)
{
  std::string tmp1 = variables.getAssemblerTemp();
  std::string tmp2 = variables.getAssemblerTemp();

  lint altstart = variables.newLabel();
  lint start = variables.newLabel();
  lint odd = variables.newLabel();
  lint realodd = variables.newLabel();
  lint altodd = variables.newLabel();
  lint altaltodd = variables.newLabel();
  lint altaltaltodd = variables.newLabel();
  lint end = variables.newLabel();
  lint altend = variables.newLabel();
  lint altaltend = variables.newLabel();

  IntInstr::addLoadInstruction(variables, instructions, argument1);
  instructions.addInstruction(Instr::STORE,variables.getIndex(tmp1));
  instructions.addInstruction(Instr::JODD,realodd);
  //IntInstr::addLoadInstruction(variables, instructions, argument2);
  //instructions.addInstruction(Instr::STORE,variables.getIndex(tmp2));
  //instructions.addInstruction(Instr::ZERO);
  //IntInstr::addStoreInstruction(variables,instructions,out);
  //instructions.addInstruction(Instr::LOAD,variables.getIndex(tmp1));
  instructions.addInstruction(Instr::SHR);
  instructions.addInstruction(Instr::JZERO,end);
  instructions.addInstruction(Instr::STORE,variables.getIndex(tmp1));
  instructions.addInstruction(Instr::JODD,altaltodd);
  IntInstr::addLoadInstruction(variables, instructions, argument2);
  instructions.addInstruction(Instr::SHL);
  instructions.addInstruction(Instr::STORE,variables.getIndex(tmp2));

  lint altstartAddr = instructions.addInstruction(Instr::LOAD,variables.getIndex(tmp1));
  instructions.addInstruction(Instr::SHR);
  instructions.addInstruction(Instr::JZERO,altaltend);
  instructions.addInstruction(Instr::STORE,variables.getIndex(tmp1));
  instructions.addInstruction(Instr::JODD,altaltaltodd);
  instructions.addInstruction(Instr::LOAD,variables.getIndex(tmp2));
  instructions.addInstruction(Instr::SHL);
  instructions.addInstruction(Instr::STORE,variables.getIndex(tmp2));
  instructions.addInstruction(Instr::JUMP,altstart);
  lint altaltaltoddAddr = instructions.addInstruction(Instr::LOAD,variables.getIndex(tmp2));
  instructions.addInstruction(Instr::SHL);
  instructions.addInstruction(Instr::STORE,variables.getIndex(tmp2));
  IntInstr::addStoreInstruction(variables,instructions,out);

  lint startAddr = instructions.addInstruction(Instr::LOAD,variables.getIndex(tmp1));
  instructions.addInstruction(Instr::SHR);
  instructions.addInstruction(Instr::JZERO,end);
  instructions.addInstruction(Instr::STORE,variables.getIndex(tmp1));
  instructions.addInstruction(Instr::JODD,odd);
  instructions.addInstruction(Instr::LOAD,variables.getIndex(tmp2));
  instructions.addInstruction(Instr::SHL);
  instructions.addInstruction(Instr::STORE,variables.getIndex(tmp2));
  instructions.addInstruction(Instr::JUMP,start);

  lint oddAddr = instructions.addInstruction(Instr::LOAD,variables.getIndex(tmp2));
  instructions.addInstruction(Instr::SHL);
  instructions.addInstruction(Instr::STORE,variables.getIndex(tmp2));
  IntInstr::addAddInstruction(variables,instructions,out);
  IntInstr::addStoreInstruction(variables,instructions,out);
  instructions.addInstruction(Instr::LOAD,variables.getIndex(tmp1));
  instructions.addInstruction(Instr::SHR);
  instructions.addInstruction(Instr::JZERO,end);
  instructions.addInstruction(Instr::STORE,variables.getIndex(tmp1));
  instructions.addInstruction(Instr::JODD,odd);
  instructions.addInstruction(Instr::LOAD,variables.getIndex(tmp2));
  instructions.addInstruction(Instr::SHL);
  instructions.addInstruction(Instr::STORE,variables.getIndex(tmp2));
  instructions.addInstruction(Instr::JUMP,start);
  //IntInstr::addLoadInstruction(variables, instructions, argument2);
  //instructions.addInstruction(Instr::STORE,variables.getIndex(tmp2));
  //IntInstr::addStoreInstruction(variables,instructions,out);
  lint realoddAddr = instructions.addInstruction(Instr::LOAD,variables.getIndex(tmp1));
  instructions.addInstruction(Instr::SHR);
  instructions.addInstruction(Instr::JZERO,altend);
  instructions.addInstruction(Instr::STORE,variables.getIndex(tmp1));
  instructions.addInstruction(Instr::JODD,altodd);
  IntInstr::addLoadInstruction(variables, instructions, argument2);
  IntInstr::addStoreInstruction(variables,instructions,out);
  instructions.addInstruction(Instr::SHL);
  instructions.addInstruction(Instr::STORE,variables.getIndex(tmp2));
  instructions.addInstruction(Instr::LOAD,variables.getIndex(tmp1));
  instructions.addInstruction(Instr::SHR);
  instructions.addInstruction(Instr::JZERO,end);
  instructions.addInstruction(Instr::STORE,variables.getIndex(tmp1));
  instructions.addInstruction(Instr::JODD,odd);
  instructions.addInstruction(Instr::LOAD,variables.getIndex(tmp2));
  instructions.addInstruction(Instr::SHL);
  instructions.addInstruction(Instr::STORE,variables.getIndex(tmp2));
  lint altoddAddr = instructions.addInstruction(Instr::JUMP,start) + 1;
  IntInstr::addLoadInstruction(variables, instructions, argument2);
  IntInstr::addStoreInstruction(variables,instructions,out);
  instructions.addInstruction(Instr::SHL);
  instructions.addInstruction(Instr::STORE,variables.getIndex(tmp2));
  IntInstr::addAddInstruction(variables,instructions,out);
  IntInstr::addStoreInstruction(variables,instructions,out);
  lint altaltoddAddr = instructions.addInstruction(Instr::JUMP,start) + 1;
  IntInstr::addLoadInstruction(variables, instructions, argument2);
  instructions.addInstruction(Instr::SHL);
  instructions.addInstruction(Instr::STORE,variables.getIndex(tmp2));
  IntInstr::addStoreInstruction(variables,instructions,out);
  instructions.addInstruction(Instr::LOAD,variables.getIndex(tmp1));
  instructions.addInstruction(Instr::SHR);
  instructions.addInstruction(Instr::JZERO,end);
  instructions.addInstruction(Instr::STORE,variables.getIndex(tmp1));
  instructions.addInstruction(Instr::JODD,odd);
  instructions.addInstruction(Instr::LOAD,variables.getIndex(tmp2));
  instructions.addInstruction(Instr::SHL);
  instructions.addInstruction(Instr::STORE,variables.getIndex(tmp2));
  lint altendAddr = instructions.addInstruction(Instr::JUMP,start) + 1;
  IntInstr::addLoadInstruction(variables, instructions, argument2);
  lint altaltendAddr = IntInstr::addStoreInstruction(variables,instructions,out);
  lint endAddr = altaltendAddr + 1;


  instructions.setLabel(startAddr,start);
  instructions.setLabel(altstartAddr,altstart);
  instructions.setLabel(oddAddr,odd);
  instructions.setLabel(realoddAddr,realodd);
  instructions.setLabel(altoddAddr,altodd);
  instructions.setLabel(altaltoddAddr,altaltodd);
  instructions.setLabel(altaltaltoddAddr,altaltaltodd);
  instructions.setLabel(endAddr,end);
  instructions.setLabel(altendAddr,altend);
  instructions.setLabel(altaltendAddr,altaltend);

  variables.freeAssemblerTemps();
}

void IntInstr::addDivInstruction(VariableRegistry& variables, InstructionRegistry& instructions, std::string argument1, std::string argument2, std::string out, std::string remainder)
{
  bool remainderNeeded = true;
  bool resultNeeded = true;
  if(out.empty())
  {
    resultNeeded = false;
  }
  if(remainder.empty())
  {
    remainderNeeded = false;
    remainder = variables.getAssemblerTemp();
  }

  std::string scaledDivisor = variables.getAssemblerTemp();
  std::string tmp, tmp2;
  if(VariableRegistry::isConst(argument1) || VariableRegistry::isConst(argument2))
  {
    tmp = variables.getAssemblerTemp();
  }
  if(out == argument2 || remainder == argument2)
  {
    tmp2 = variables.getAssemblerTemp();
  }

  lint normaldiv = variables.newLabel();
  lint whil = variables.newLabel();
  lint end = variables.newLabel();
  lint divzero = variables.newLabel();
  lint endwhile = variables.newLabel();
  lint next = variables.newLabel();
  lint next2 = variables.newLabel();
  lint finish = variables.newLabel();
  lint afinish = variables.newLabel();
  lint finalloop = variables.newLabel();


  if(VariableRegistry::isConst(argument1))
  {
    IntInstr::addLoadInstruction(variables,instructions,argument1);
    instructions.addInstruction(Instr::STORE,variables.getIndex(tmp));
  }
  IntInstr::addLoadInstruction(variables,instructions,argument2);
  if(VariableRegistry::isConst(argument2))
  {
    instructions.addInstruction(Instr::STORE,variables.getIndex(tmp));
  }
  if(out == argument2 || remainder == argument2)
  {
    instructions.addInstruction(Instr::STORE,variables.getIndex(tmp2));
  }
  instructions.addInstruction(Instr::JZERO,divzero);
  instructions.addInstruction(Instr::SHL);

  if(VariableRegistry::isConst(argument1))
  {
    IntInstr::addSubInstruction(variables,instructions,tmp);
  }
  else
  {
    IntInstr::addSubInstruction(variables,instructions,argument1);
  }
  instructions.addInstruction(Instr::JZERO,normaldiv);

  IntInstr::addLoadInstruction(variables,instructions,argument1);
  IntInstr::addStoreInstruction(variables,instructions,remainder);
  instructions.addInstruction(Instr::INC);
  if(VariableRegistry::isConst(argument2))
  {
    IntInstr::addSubInstruction(variables,instructions,tmp);
  }
  else if(out == argument2 || remainder == argument2)
  {
    IntInstr::addSubInstruction(variables,instructions,tmp2);
  }
  else
  {
    IntInstr::addSubInstruction(variables,instructions,argument2);
  }
  instructions.addInstruction(Instr::JZERO,next2);

  instructions.addInstruction(Instr::DEC);
  IntInstr::addStoreInstruction(variables,instructions,remainder);
  if(resultNeeded)
  {
    instructions.addInstruction(Instr::ZERO);
    instructions.addInstruction(Instr::INC);
    IntInstr::addStoreInstruction(variables,instructions,out);
  }
  if(out == argument2 || remainder == argument2)
  {
    instructions.addInstruction(Instr::LOAD,variables.getIndex(tmp2));
  }
  else
  {
    IntInstr::addLoadInstruction(variables,instructions,argument2);
  }
  instructions.addInstruction(Instr::SHR);
  instructions.addInstruction(Instr::STORE,variables.getIndex(scaledDivisor));
  instructions.addInstruction(Instr::INC);
  if(VariableRegistry::isConst(argument2))
  {
    IntInstr::addSubInstruction(variables,instructions,tmp);
  }
  else if(out == argument2 || remainder == argument2)
  {
    IntInstr::addSubInstruction(variables,instructions,tmp2);
  }
  else
  {
    IntInstr::addSubInstruction(variables,instructions,argument2);
  }
  instructions.addInstruction(Instr::JZERO,end);
  lint next2Addr = instructions.addInstruction(Instr::JUMP,endwhile) + 1;

  if(resultNeeded)
  {
    instructions.addInstruction(Instr::ZERO);
    IntInstr::addStoreInstruction(variables,instructions,out);
  }
  if(out == argument2 || remainder == argument2)
  {
    instructions.addInstruction(Instr::LOAD,variables.getIndex(tmp2));
  }
  else
  {
    IntInstr::addLoadInstruction(variables,instructions,argument2);
  }
  instructions.addInstruction(Instr::SHR);
  instructions.addInstruction(Instr::STORE,variables.getIndex(scaledDivisor));
  instructions.addInstruction(Instr::INC);
  if(VariableRegistry::isConst(argument2))
  {
    IntInstr::addSubInstruction(variables,instructions,tmp);
  }
  else if(out == argument2 || remainder == argument2)
  {
    IntInstr::addSubInstruction(variables,instructions,tmp2);
  }
  else
  {
    IntInstr::addSubInstruction(variables,instructions,argument2);
  }
  instructions.addInstruction(Instr::JZERO,end);
  instructions.addInstruction(Instr::JUMP,endwhile);

  lint normaldivAddr = IntInstr::addLoadInstruction(variables,instructions,argument2);
  instructions.addInstruction(Instr::SHL);
  instructions.addInstruction(Instr::STORE,variables.getIndex(scaledDivisor));
  instructions.addInstruction(Instr::SHL);
  if(VariableRegistry::isConst(argument1))
  {
    IntInstr::addSubInstruction(variables,instructions,tmp);
  }
  else
  {
    IntInstr::addSubInstruction(variables,instructions,argument1);
  }
  instructions.addInstruction(Instr::JZERO,whil);
  IntInstr::addLoadInstruction(variables,instructions,argument1);
  instructions.addInstruction(Instr::SUB,variables.getIndex(scaledDivisor));
  IntInstr::addStoreInstruction(variables,instructions,remainder);
  instructions.addInstruction(Instr::INC);
  if(VariableRegistry::isConst(argument2))
  {
    IntInstr::addSubInstruction(variables,instructions,tmp);
  }
  else if(out == argument2 || remainder == argument2)
  {
    IntInstr::addSubInstruction(variables,instructions,tmp2);
  }
  else
  {
    IntInstr::addSubInstruction(variables,instructions,argument2);
  }
  instructions.addInstruction(Instr::JZERO,afinish);
  if(resultNeeded)
  {
    instructions.addInstruction(Instr::ZERO);
    instructions.addInstruction(Instr::INC);
    IntInstr::addStoreInstruction(variables,instructions,out);
  }
  instructions.addInstruction(Instr::LOAD,variables.getIndex(scaledDivisor));
  instructions.addInstruction(Instr::SHR);
  instructions.addInstruction(Instr::STORE,variables.getIndex(scaledDivisor));
  instructions.addInstruction(Instr::JUMP,endwhile);

  lint whileAddr = instructions.addInstruction(Instr::LOAD,variables.getIndex(scaledDivisor));
  instructions.addInstruction(Instr::SHL);
  instructions.addInstruction(Instr::STORE,variables.getIndex(scaledDivisor));
  instructions.addInstruction(Instr::SHL);
  if(VariableRegistry::isConst(argument1))
  {
    IntInstr::addSubInstruction(variables,instructions,tmp);
  }
  else
  {
    IntInstr::addSubInstruction(variables,instructions,argument1);
  }
  instructions.addInstruction(Instr::JZERO,whil);

  IntInstr::addLoadInstruction(variables,instructions,argument1);
  instructions.addInstruction(Instr::SUB,variables.getIndex(scaledDivisor));
  IntInstr::addStoreInstruction(variables,instructions,remainder);
  instructions.addInstruction(Instr::INC);
  if(VariableRegistry::isConst(argument2))
  {
    IntInstr::addSubInstruction(variables,instructions,tmp);
  }
  else if(out == argument2 || remainder == argument2)
  {
    IntInstr::addSubInstruction(variables,instructions,tmp2);
  }
  else
  {
    IntInstr::addSubInstruction(variables,instructions,argument2);
  }
  instructions.addInstruction(Instr::JZERO,afinish);
  if(resultNeeded)
  {
    instructions.addInstruction(Instr::ZERO);
    instructions.addInstruction(Instr::INC);
    IntInstr::addStoreInstruction(variables,instructions,out);
  }
  instructions.addInstruction(Instr::LOAD,variables.getIndex(scaledDivisor));
  instructions.addInstruction(Instr::SHR);
  instructions.addInstruction(Instr::STORE,variables.getIndex(scaledDivisor));

  lint endwhileAddr = IntInstr::addLoadInstruction(variables,instructions,remainder);
  instructions.addInstruction(Instr::INC);
  instructions.addInstruction(Instr::SUB,variables.getIndex(scaledDivisor));
  instructions.addInstruction(Instr::JZERO,next);
  instructions.addInstruction(Instr::DEC);
  IntInstr::addStoreInstruction(variables,instructions,remainder);
  instructions.addInstruction(Instr::INC);
  if(VariableRegistry::isConst(argument2))
  {
    IntInstr::addSubInstruction(variables,instructions,tmp);
  }
  else if(out == argument2 || remainder == argument2)
  {
    IntInstr::addSubInstruction(variables,instructions,tmp2);
  }
  else
  {
    IntInstr::addSubInstruction(variables,instructions,argument2);
  }
  instructions.addInstruction(Instr::JZERO,finish);
  if(resultNeeded)
  {
    IntInstr::addLoadInstruction(variables,instructions,out);
    instructions.addInstruction(Instr::SHL);
    instructions.addInstruction(Instr::INC);
    IntInstr::addStoreInstruction(variables,instructions,out);
  }
  instructions.addInstruction(Instr::LOAD,variables.getIndex(scaledDivisor));
  instructions.addInstruction(Instr::SHR);
  instructions.addInstruction(Instr::STORE,variables.getIndex(scaledDivisor));
  lint nextAddr = instructions.addInstruction(Instr::JUMP,endwhile) + 1;
  if(resultNeeded)
  {
    IntInstr::addLoadInstruction(variables,instructions,out);
    instructions.addInstruction(Instr::SHL);
    IntInstr::addStoreInstruction(variables,instructions,out);
  }
  instructions.addInstruction(Instr::LOAD,variables.getIndex(scaledDivisor));
  instructions.addInstruction(Instr::SHR);
  instructions.addInstruction(Instr::STORE,variables.getIndex(scaledDivisor));
  instructions.addInstruction(Instr::INC);
  if(VariableRegistry::isConst(argument2))
  {
    IntInstr::addSubInstruction(variables,instructions,tmp);
  }
  else if(out == argument2 || remainder == argument2)
  {
    IntInstr::addSubInstruction(variables,instructions,tmp2);
  }
  else
  {
    IntInstr::addSubInstruction(variables,instructions,argument2);
  }
  instructions.addInstruction(Instr::JZERO,end);
  lint finishAddr = instructions.addInstruction(Instr::JUMP,endwhile) + 1;

  if(resultNeeded)
  {
    IntInstr::addLoadInstruction(variables,instructions,out);
    instructions.addInstruction(Instr::SHL);
    instructions.addInstruction(Instr::INC);
    IntInstr::addStoreInstruction(variables,instructions,out);
  }
  instructions.addInstruction(Instr::LOAD,variables.getIndex(scaledDivisor));
  instructions.addInstruction(Instr::SHR);
  instructions.addInstruction(Instr::STORE,variables.getIndex(scaledDivisor));
  instructions.addInstruction(Instr::INC);
  if(VariableRegistry::isConst(argument2))
  {
    IntInstr::addSubInstruction(variables,instructions,tmp);
  }
  else if(out == argument2 || remainder == argument2)
  {
    IntInstr::addSubInstruction(variables,instructions,tmp2);
  }
  else
  {
    IntInstr::addSubInstruction(variables,instructions,argument2);
  }
  lint finalloopAddr = instructions.addInstruction(Instr::JZERO,end) + 1;
  if(resultNeeded)
  {
    IntInstr::addLoadInstruction(variables,instructions,out);
    instructions.addInstruction(Instr::SHL);
    IntInstr::addStoreInstruction(variables,instructions,out);
  }
  instructions.addInstruction(Instr::LOAD,variables.getIndex(scaledDivisor));
  instructions.addInstruction(Instr::SHR);
  instructions.addInstruction(Instr::STORE,variables.getIndex(scaledDivisor));
  instructions.addInstruction(Instr::INC);
  if(VariableRegistry::isConst(argument2))
  {
    IntInstr::addSubInstruction(variables,instructions,tmp);
  }
  else if(out == argument2 || remainder == argument2)
  {
    IntInstr::addSubInstruction(variables,instructions,tmp2);
  }
  else
  {
    IntInstr::addSubInstruction(variables,instructions,argument2);
  }
  instructions.addInstruction(Instr::JZERO,end);
  instructions.addInstruction(Instr::JUMP,finalloop);

  lint afinishAddr = instructions.addInstruction(Instr::JUMP,endwhile) + 1;

  if(resultNeeded)
  {
    instructions.addInstruction(Instr::ZERO);
    instructions.addInstruction(Instr::INC);
    IntInstr::addStoreInstruction(variables,instructions,out);
  }
  instructions.addInstruction(Instr::LOAD,variables.getIndex(scaledDivisor));
  instructions.addInstruction(Instr::SHR);
  instructions.addInstruction(Instr::STORE,variables.getIndex(scaledDivisor));
  instructions.addInstruction(Instr::INC);
  if(VariableRegistry::isConst(argument2))
  {
    IntInstr::addSubInstruction(variables,instructions,tmp);
  }
  else if(out == argument2 || remainder == argument2)
  {
    IntInstr::addSubInstruction(variables,instructions,tmp2);
  }
  else
  {
    IntInstr::addSubInstruction(variables,instructions,argument2);
  }
  instructions.addInstruction(Instr::JZERO,end);
  if(resultNeeded)
  {
    IntInstr::addLoadInstruction(variables,instructions,out);
    instructions.addInstruction(Instr::SHL);
    IntInstr::addStoreInstruction(variables,instructions,out);
  }
  instructions.addInstruction(Instr::LOAD,variables.getIndex(scaledDivisor));
  instructions.addInstruction(Instr::SHR);
  instructions.addInstruction(Instr::STORE,variables.getIndex(scaledDivisor));
  instructions.addInstruction(Instr::INC);
  if(VariableRegistry::isConst(argument2))
  {
    IntInstr::addSubInstruction(variables,instructions,tmp);
  }
  else if(out == argument2 || remainder == argument2)
  {
    IntInstr::addSubInstruction(variables,instructions,tmp2);
  }
  else
  {
    IntInstr::addSubInstruction(variables,instructions,argument2);
  }
  instructions.addInstruction(Instr::JZERO,end);
  instructions.addInstruction(Instr::JUMP,finalloop);

  lint divzeroAddr = instructions.addInstruction(Instr::ZERO);
  lint endAddr;
  if(resultNeeded) endAddr = IntInstr::addStoreInstruction(variables,instructions,out) + 1;
  if(remainderNeeded) endAddr = IntInstr::addStoreInstruction(variables,instructions,remainder) + 1;


  instructions.setLabel(normaldivAddr,normaldiv);
  instructions.setLabel(whileAddr,whil);
  instructions.setLabel(endAddr,end);
  instructions.setLabel(divzeroAddr,divzero);
  instructions.setLabel(endwhileAddr,endwhile);
  instructions.setLabel(nextAddr,next);
  instructions.setLabel(next2Addr,next2);
  instructions.setLabel(finishAddr,finish);
  instructions.setLabel(afinishAddr,afinish);
  instructions.setLabel(finalloopAddr,finalloop);

  variables.freeAssemblerTemps();
}

std::string IntInstr::getTypeAsStr()
{
    int id = static_cast<int>(this->type);
    return instrStr[id];
}

void IntInstr::dbgPrint()
{
  printf("%s",this->getTypeAsStr().c_str());
  if(!this->val1.empty())
  {
    printf(" %s",this->val1.c_str());
    if(!this->val2.empty())
    {
      printf(" %s",this->val2.c_str());
      if(!this->val3.empty())
      {
        printf(" %s",this->val3.c_str());
      }
    }
  }
  printf("\n");
}

InstructionRegistry IntInstr::translate(VariableRegistry& variables)
{
  InstructionRegistry out;
  std::string tmp;
  std::string val3;
  switch(this->type)
  {
    case IntInstrType::GET:
      out.addInstruction(Instr::GET);
      IntInstr::addStoreInstruction(variables,out,this->val1);
      break;
    case IntInstrType::PUT:
      IntInstr::addLoadInstruction(variables,out,this->val1);
      out.addInstruction(Instr::PUT);
      break;
    case IntInstrType::SET:
      IntInstr::addLoadInstruction(variables,out,this->val2);
      if(!this->val1.empty()) IntInstr::addStoreInstruction(variables,out,this->val1);
      break;
    case IntInstrType::ADD:
      if(!VariableRegistry::isConst(this->val2) && VariableRegistry::isConst(this->val3))
      {
        tmp = this->val2;
        this->val2 = this->val3;
        this->val3 = tmp;
      }
      if(VariableRegistry::isConst(this->val2) && VariableRegistry::getConstVal(this->val2) == 1)
      {
        IntInstr::addLoadInstruction(variables,out,this->val3);
        out.addInstruction(Instr::INC);
      }
      else
      {
        IntInstr::addLoadInstruction(variables,out,this->val2);
        IntInstr::addAddInstruction(variables,out,this->val3);
      }
      IntInstr::addStoreInstruction(variables,out,this->val1);
      break;
    case IntInstrType::SUB:
      val3 = this->val3;
      if(!VariableRegistry::isConst(this->val2) && VariableRegistry::isConst(this->val3))
      {
        tmp = variables.getAssemblerTemp();
        IntInstr::addLoadInstruction(variables,out,this->val3);
        IntInstr::addStoreInstruction(variables,out,tmp);
        val3 = tmp;
      }
      IntInstr::addLoadInstruction(variables,out,this->val2);
      IntInstr::addSubInstruction(variables,out,val3);
      if(!this->val1.empty()) IntInstr::addStoreInstruction(variables,out,this->val1);
      variables.freeAssemblerTemps();
      break;
    case IntInstrType::MUL:
      IntInstr::addMulInstruction(variables, out, this->val2, this->val3, this->val1);
      break;
    case IntInstrType::DIV:
      IntInstr::addDivInstruction(variables, out, this->val2, this->val3, this->val1,"");
      break;
    case IntInstrType::MOD:
      IntInstr::addDivInstruction(variables, out, this->val2, this->val3, "", this->val1);
      break;
    case IntInstrType::HALT:
      out.addInstruction(Instr::HALT);
      break;
  }
  return out;
}

void IntInstr::optimize()
{
  lint val2, val3, val;
  // Constant reduction
  switch(this->type)
  {
    case IntInstrType::ADD:
      if(VariableRegistry::isConst(this->val2) && VariableRegistry::isConst(this->val3))
      {
        this->type = IntInstrType::SET;
        this->val2 = VariableRegistry::toConst(VariableRegistry::getConstVal(this->val2) + VariableRegistry::getConstVal(this->val3));
        this->val3 = "";
      }
      break;
    case IntInstrType::SUB:
      if(VariableRegistry::isConst(this->val2) && VariableRegistry::isConst(this->val3))
      {
        this->type = IntInstrType::SET;
        val2 = VariableRegistry::getConstVal(this->val2);
        val3 = VariableRegistry::getConstVal(this->val3);
        if(val3 >= val2)
          val = 0;
        else
          val = val2 - val3;
        this->val2 = VariableRegistry::toConst(val);
        this->val3 = "";
      }
      break;
    case IntInstrType::MUL:
      if(VariableRegistry::isConst(this->val2) && VariableRegistry::isConst(this->val3))
      {
        this->type = IntInstrType::SET;
        this->val2 = VariableRegistry::toConst(VariableRegistry::getConstVal(this->val2) * VariableRegistry::getConstVal(this->val3));
        this->val3 = "";
      }
      break;
    case IntInstrType::DIV:
      if(VariableRegistry::isConst(this->val2) && VariableRegistry::isConst(this->val3))
      {
        this->type = IntInstrType::SET;
        if(VariableRegistry::getConstVal(this->val3) == 0) this->val2 = VariableRegistry::toConst(0);
        else
        {
          this->val2 = VariableRegistry::toConst(VariableRegistry::getConstVal(this->val2) / VariableRegistry::getConstVal(this->val3));
          this->val3 = "";
        }
      }
    case IntInstrType::MOD:
      if(VariableRegistry::isConst(this->val2) && VariableRegistry::isConst(this->val3))
      {
        this->type = IntInstrType::SET;
        if(VariableRegistry::getConstVal(this->val3) == 0) this->val2 = VariableRegistry::toConst(0);
        else
        {
          this->val2 = VariableRegistry::toConst(VariableRegistry::getConstVal(this->val2) % VariableRegistry::getConstVal(this->val3));
          this->val3 = "";
        }
      }
      break;
  }
}
