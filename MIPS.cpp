#include<iostream>
#include<string>
#include<vector>
#include<bitset>
#include<fstream>

using namespace std;

// Internal ALU control codes.
#define ADDU (1)
#define SUBU (3)
#define AND (4)
#define OR  (5)
#define NOR (7)

// Memory size.
// In reality, the memory size should be 2^32, but for this lab and space reasons,
// we keep it as this large number, but the memory is still 32-bit addressable.
#define MemSize (65536)


class RF
{
  public:
    bitset<32> ReadData1, ReadData2; 
    RF()
    { 
      Registers.resize(32);  
      Registers[0] = bitset<32> (0);  
    }

    void ReadWrite(bitset<5> RdReg1, bitset<5> RdReg2, bitset<5> WrtReg, bitset<32> WrtData, bitset<1> WrtEnable)
    {   
      /**
       * @brief Reads or writes data from/to the Register.
       *
       * This function is used to read or write data from/to the register, depending on the value of WrtEnable.
       * Put the read results to the ReadData1 and ReadData2.
       */
      // TODO: implement!
      unsigned long address_first_read = RdReg1.to_ulong();
      ReadData1 = Registers[address_first_read]; 
      unsigned long address_second_read = RdReg2.to_ulong();
      ReadData2 = Registers[address_second_read];
      if (WrtEnable == 1) {
        if (WrtReg != 0) {
          unsigned long address_write = WrtReg.to_ulong();
          Registers[address_write] = WrtData;
        }
      }   
    }

    void OutputRF()
    {
      ofstream rfout;
      rfout.open("RFresult.txt",std::ios_base::app);
      if (rfout.is_open())
      {
        rfout<<"A state of RF:"<<endl;
        for (int j = 0; j<32; j++)
        {        
          rfout << Registers[j]<<endl;
        }

      }
      else cout<<"Unable to open file";
      rfout.close();

    }     
  private:
    vector<bitset<32> >Registers;
};


class ALU
{
  public:
    bitset<32> ALUresult;
    bitset<32> ALUOperation (bitset<3> ALUOP, bitset<32> oprand1, bitset<32> oprand2)
    {   
      /**
       * @brief Implement the ALU operation here.
       *
       * ALU operation depends on the ALUOP, which are definded as ADDU, SUBU, etc. 
       */
      // TODO: implement!
      if (ALUOP == ADDU) {
        ALUresult = bitset<32>(oprand1.to_ulong() + oprand2.to_ulong());
      }
      else if (ALUOP == SUBU) {
        ALUresult = bitset<32>(oprand1.to_ulong() - oprand2.to_ulong());
      }
      else if (ALUOP == AND) {
        ALUresult = oprand1 & oprand2;
      }
      else if (ALUOP == OR) {
        ALUresult = oprand1 | oprand2;
      }
      else if (ALUOP == NOR) {
        ALUresult = ~(oprand1 | oprand2);
      } 

      return ALUresult;
    }            
};


class INSMem
{
  public:
    bitset<32> Instruction;
    INSMem()
    {       IMem.resize(MemSize); 
      ifstream imem;
      string line;
      int i=0;
      imem.open("imem.txt");
      if (imem.is_open())
      {
        while (getline(imem,line))
        {      
          IMem[i] = bitset<8>(line);
          i++;
        }

      }
      else cout<<"Unable to open file";
      imem.close();

    }

    bitset<32> ReadMemory (bitset<32> ReadAddress) 
    {    
      // TODO: implement!
      /**
       * @brief Read Instruction Memory (IMem).
       *
       * Read the byte at the ReadAddress and the following three byte,
       * and return the read result. 
       */
      unsigned long address_read = ReadAddress.to_ulong();
      unsigned long first_instruction = IMem[address_read].to_ulong();
      unsigned long second_instruction = IMem[address_read + 1].to_ulong();
      unsigned long third_instruction = IMem[address_read + 2].to_ulong();
      unsigned long fourth_instruction = IMem[address_read + 3].to_ulong();
      Instruction = bitset<32>(first_instruction << 24 | second_instruction << 16 | third_instruction << 8 | fourth_instruction);
      return Instruction;     
    }     

  private:
    vector<bitset<8> > IMem;

};

class DataMem    
{
  public:
    bitset<32> readdata;  
    DataMem()
    {
      DMem.resize(MemSize); 
      ifstream dmem;
      string line;
      int i=0;
      dmem.open("dmem.txt");
      if (dmem.is_open())
      {
        while (getline(dmem,line))
        {      
          DMem[i] = bitset<8>(line);
          i++;
        }
      }
      else cout<<"Unable to open file";
      dmem.close();

    }  
    bitset<32> MemoryAccess (bitset<32> Address, bitset<32> WriteData, bitset<1> readmem, bitset<1> writemem) 
    {    
      /**
       * @brief Reads/writes data from/to the Data Memory.
       *
       * This function is used to read/write data from/to the DataMem, depending on the readmem and writemem.
       * First, if writemem enabled, WriteData should be written to DMem, clear or ignore the return value readdata,
       * and note that 32-bit WriteData will occupy 4 continious Bytes in DMem. 
       * If readmem enabled, return the DMem read result as readdata.
       */
      // TODO: implement!
      if (readmem == 1) {
        unsigned long address_read = Address.to_ulong();
        unsigned long first_data = DMem[address_read].to_ulong();
        unsigned long second_data = DMem[address_read + 1].to_ulong();
        unsigned long third_data = DMem[address_read + 2].to_ulong();
        unsigned long fourth_data = DMem[address_read + 3].to_ulong();
        readdata = bitset<32>(first_data << 24 | second_data << 16 | third_data << 8 | fourth_data);
      }
      if (writemem == 1) {
        unsigned long address_write = Address.to_ulong();
        unsigned long write_data = WriteData.to_ulong();
        unsigned long first_data_write = write_data >> 24;
        unsigned long second_data_write = write_data >> 16;
        unsigned long third_data_write = write_data >> 8;
        unsigned long fourth_data_write = write_data;
        DMem[address_write] = bitset<8>(first_data_write);
        DMem[address_write + 1] = bitset<8>(second_data_write);
        DMem[address_write + 2] = bitset<8>(third_data_write);
        DMem[address_write + 3] = bitset<8>(fourth_data_write);
      }
      return readdata;     
    }   

    void OutputDataMem()
    {
      ofstream dmemout;
      dmemout.open("dmemresult.txt");
      if (dmemout.is_open())
      {
        for (int j = 0; j< 1000; j++)
        {     
          dmemout << DMem[j]<<endl;
        }

      }
      else cout<<"Unable to open file";
      dmemout.close();

    }             

  private:
    vector<bitset<8> > DMem;

};  



int main()
{
  RF myRF;
  ALU myALU;
  INSMem myInsMem;
  DataMem myDataMem;
  bitset<32> PC(0); // 32-bit program counter, initialized to 0

  while (1)  // TODO: implement!
  {
    // Fetch: fetch an instruction from myInsMem.
    bitset<32> instruction = myInsMem.ReadMemory(PC);

    // If current instruction is "11111111111111111111111111111111", then break; (exit the while loop)
    std::string s = "";
    for (int i = 0; i < 32; i++) {
      s += "1";
    }
    if (instruction == bitset<32>(s)) {
      break;
    }
    // decode(Read RF): get opcode and other signals from instruction, decode instruction
    // Execute: after decoding, ALU may run and return result
    // Read/Write Mem: access data memory (myDataMem)
    // Write back to RF: some operations may write things to RF
    bitset<6> opcode = (instruction >> 26).to_ulong();
    bitset<32> next_pc = bitset<32>(PC.to_ulong() + 4);

    if (opcode == bitset<6>("000000")) {
      bitset<5> rs = (instruction >> 21).to_ulong();
      bitset<5> rt = (instruction >> 16).to_ulong();
      bitset<5> rd = (instruction >> 11).to_ulong();
      bitset<5> shamt = (instruction >> 6).to_ulong();
      bitset<6> funct = (instruction >> 0).to_ulong();
      myRF.ReadWrite(rs, rt, bitset<5>(0), bitset<32>(0), bitset<1>(0));
      bitset<32> result;
      if (funct == bitset<6>("100001")) {
         result = myALU.ALUOperation(ADDU, myRF.ReadData1, myRF.ReadData2);
      }
      else if (funct == bitset<6>("100011")) {
        result = myALU.ALUOperation(SUBU, myRF.ReadData1, myRF.ReadData2);
      }
      else if (funct == bitset<6>("100100")) {
        result = myALU.ALUOperation(AND, myRF.ReadData1, myRF.ReadData2);
      }
      else if (funct == bitset<6>("100101")) {
        result = myALU.ALUOperation(OR, myRF.ReadData1, myRF.ReadData2);
      }
      else if (funct == bitset<6>("100111")) {
        result = myALU.ALUOperation(NOR, myRF.ReadData1, myRF.ReadData2);
      }
      myRF.ReadWrite(bitset<5>(0), bitset<5>(0), rd, result, bitset<1>(1));
    }
    else if (opcode == bitset<6>("001001")) {
      bitset<5> rs = (instruction >> 21).to_ulong();
      bitset<5> rt = (instruction >> 16).to_ulong();
      bitset<16> immediate = (instruction >> 0).to_ulong();
      myRF.ReadWrite(rs, rt, bitset<5>(0), bitset<32>(0), bitset<1>(0));
      unsigned long immediate_value = immediate.to_ulong();
      bitset<32> sign_extended_immediate;
      if ((immediate_value >> 15) & 0x1) {
        sign_extended_immediate = bitset<32>(immediate_value | 0xFFFF0000);
      } else {
        sign_extended_immediate = bitset<32>(immediate_value);
      }
      bitset<32> result = myALU.ALUOperation(ADDU, myRF.ReadData1, sign_extended_immediate);
      myRF.ReadWrite(bitset<5>(0), bitset<5>(0), rt, result, bitset<1>(1));
    }
    else if (opcode == bitset<6>("000100")) {
      bitset<5> rs = (instruction >> 21).to_ulong();
      bitset<5> rt = (instruction >> 16).to_ulong();
      bitset<16> immediate = (instruction >> 0).to_ulong();
      myRF.ReadWrite(rs, rt, bitset<5>(0), bitset<32>(0), bitset<1>(0));
      bitset<32> diff = myALU.ALUOperation(SUBU, myRF.ReadData1, myRF.ReadData2);
      branch_taken = (diff == bitset<32>(0));
      unsigned long immediate_value = immediate.to_ulong();
      bool branch_taken = (diff == bitset<32>(0));
      bitset<32> sign_extended_immediate;
      if ((immediate_value >> 15) & 0x1) {
        sign_extended_immediate = bitset<32>(immediate_value | 0xFFFF0000);
      } else {
        sign_extended_immediate = bitset<32>(immediate_value);
      }
      if (branch_taken) {
        next_pc = bitset<32>(PC.to_ulong() + 4 + (sign_extended_immediate.to_ulong() << 2));
      }
    }
    else if (opcode == bitset<6>("000010")) {
      bitset<26> address = bitset<26>((instruction >> 0).to_ulong());
      unsigned long target = (address.to_ulong() << 2) | ((PC.to_ulong() + 4) & 0xF0000000);
      next_pc = bitset<32>(target);
    }
    else if (opcode == bitset<6>("100011")) {
      bitset<5> rs = bitset<5>((instruction >> 21).to_ulong());
      bitset<5> rt = bitset<5>((instruction >> 16).to_ulong());
      bitset<16> immediate = bitset<16>((instruction >> 0).to_ulong());
      myRF.ReadWrite(rs, rt, bitset<5>(0), bitset<32>(0), bitset<1>(0));
      unsigned long immediate_value = immediate.to_ulong();
      bitset<32> sign_extended_immediate;
      if ((immediate_value >> 15) & 0x1) {
        sign_extended_immediate = bitset<32>(immediate_value | 0xFFFF0000);
      } else {
        sign_extended_immediate = bitset<32>(immediate_value);
      }
      bitset<32> address = myALU.ALUOperation(ADDU, myRF.ReadData1, sign_extended_immediate);
      bitset<32> loaded_value = myDataMem.MemoryAccess(address, bitset<32>(0), bitset<1>(1), bitset<1>(0));
      myRF.ReadWrite(bitset<5>(0), bitset<5>(0), rt, loaded_value, bitset<1>(1));
    }
    else if (opcode == bitset<6>("101011")) {
      bitset<5> rs = bitset<5>((instruction >> 21).to_ulong());
      bitset<5> rt = bitset<5>((instruction >> 16).to_ulong());
      bitset<16> immediate = bitset<16>((instruction >> 0).to_ulong());
      myRF.ReadWrite(rs, rt, bitset<5>(0), bitset<32>(0), bitset<1>(0));
      unsigned long immediate_value = immediate.to_ulong();
      bitset<32> sign_extended_immediate;
      if ((immediate_value >> 15) & 0x1) {
        sign_extended_immediate = bitset<32>(immediate_value | 0xFFFF0000);
      } else {
        sign_extended_immediate = bitset<32>(immediate_value);
      }
      
      bitset<32> address = myALU.ALUOperation(ADDU, myRF.ReadData1, sign_extended_immediate);
      myDataMem.MemoryAccess(address, myRF.ReadData2, bitset<1>(0), bitset<1>(1));
    }
    else if (opcode == bitset<6>("111111")) {
      break;
    }
    // Update PC
    PC = next_pc;

    /**** You don't need to modify the following lines. ****/
    myRF.OutputRF(); // dump RF;    
  }
  myDataMem.OutputDataMem(); // dump data mem

  return 0;
}
