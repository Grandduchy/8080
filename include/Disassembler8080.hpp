#ifndef DISASSEMBLER8080_HPP
#define DISASSEMBLER8080_HPP

#include <array>

#include "State8080.hpp"

class Disassembler8080 {
public:
    using opcodePtr = void (Disassembler8080::*)(State8080&); // the type of pointer to opcode member functions

    Disassembler8080();
    void runCycle(State8080&);
    std::array<opcodePtr, 256> opcodeTable;
    static const std::array<const uint8_t, 256> opCycles;

    bool wasUnimplemented = false;
    bool wasTodo = false;

    void generateInterrupt(State8080& state, const uint8_t& interruptNum);

private:

    inline void setZero(State8080& state, const uint16_t& expr) const noexcept;
    inline void setSign(State8080& state, const uint16_t& expr) const noexcept;
    inline void setCarry(State8080& state, const uint32_t& expr, const uint16_t& maxVal) const noexcept;
    inline void setParity(State8080& state, const uint8_t& expr) const noexcept;


    void unimplemented(State8080&); // an opcode that is unimplemented and will never be done
    void todo(State8080&); // an opcode that is currently unimplemented but will eventually be done

    // The ending extension of an opcode ex D8 or D16 refers to immediate memory; the next byte after the instruction
    // to be loaded and in for use
    // Opcodes are arranged in the opcode's function order

    /// Carry Bit Instructions
    void OP_STC(State8080&); // 0x37
    void OP_CMC(State8080&); // 0x3F


    /// Immediate Instructions
       //MVI
    void OP_MVIB_D8(State8080&); // 0x06
    void OP_MVIC_D8(State8080&); // 0x0e
    void OP_MVID_D8(State8080&); // 0x16
    void OP_MVIE_D8(State8080&); // 0x1E
    void OP_MVIH_D8(State8080&); // 0x26
    void OP_MVIL_D8(State8080&); // 0x2E
    void OP_MVIM_D8(State8080&); // 0x36
    void OP_MVIA_D8(State8080&); // 0x3e

    void OP_LXIB_D16(State8080&); // 0x01
    void OP_LXID_D16(State8080&); // 0x11
    void OP_LXIH_D16(State8080&); // 0x21
    void OP_LXISP_D16(State8080&); // 0x31


    void OP_ADI_D8(State8080&); // 0xc6
    void OP_ACI_D8(State8080&); // 0xCE
    void OP_SUI_D8(State8080&); // 0xD6
    void OP_SBI_D8(State8080&); // 0xDE
    void OP_ANI_D8(State8080&); // 0xe6
    void OP_XRI_D8(State8080&); // 0xEE
    void OP_ORI_D8(State8080&); // 0xF6
    void OP_CPI_D8(State8080&); // 0xfe


    /// Direct Addressing Instructions
    void OP_STA_ADR(State8080&); // 0x32
    void OP_LDA_ADR(State8080&); // 0x3a
    void OP_SHLD(State8080&); // 0x22
    void OP_LHLD(State8080&); // 0x2A


    /// Jump instructions
    void OP_PCHL(State8080&);
    void OP_JUMP(State8080&);


    /// Call subroutine instructions
    void OP_CALL(State8080&);


    /// Return from subroutine instructions
    void OP_RET(State8080&);

    /// RST instructions
    void OP_RST(State8080&);


    /// Interrupt instructions
    void OP_DI(State8080&); // 0xF3
    void OP_EI(State8080&); // 0xfb


    /// I/O instructions
    void OP_OUTD8(State8080&); // 0xd3
    void OP_IND8(State8080&); // 0xDB


    /// Single Register Instructions
    void OP_CMA(State8080&); // 0x2F
    void OP_DAA(State8080&); // 0x27
        // INR
    void OP_INRB(State8080&); // 0x04
    void OP_INRC(State8080&); // 0x0C
    void OP_INRD(State8080&); // 0x14
    void OP_INRE(State8080&); // 0x1C
    void OP_INRH(State8080&); // 0x24
    void OP_INRL(State8080&); // 0x2C
    void OP_INRM(State8080&); // 0x34
    void OP_INRA(State8080&); // 0x3C
        // DCR
    void OP_DCRB(State8080&); // 0x05
    void OP_DCRC(State8080&); // 0x0d
    void OP_DCRD(State8080&); // 0x15
    void OP_DCRE(State8080&); // 0x1D
    void OP_DCRH(State8080&); // 0x25
    void OP_DCRL(State8080&); // 0x2D
    void OP_DCRM(State8080&); // 0x35
    void OP_DCRA(State8080&); // 0x3D


    /// NOP instruction
    void OP_NOP(State8080&); // 0x00


    /// Data transfer Instructions
    void OP_LDAXD(State8080&); // 0x1a
    void OP_LDAXB(State8080&); // 0x0A;
    void OP_MOV(State8080&); // 0x40 - 0x7F
    void OP_STAXB(State8080&); // 0x02
    void OP_STAXD(State8080&); // 0x12


    /// Register or memory to accumulator instructions
    void OP_REG_ACC(State8080&); // 0x80 - 0xBF

    /// Roatate accumulator instructions
    void OP_RRC(State8080&); // 0x0F
    void OP_RLC(State8080&); // 0x07
    void OP_RAL(State8080&); // 0x17
    void OP_RAR(State8080&); // 0x1F

    /// Register Pair Instructions
        // DAD
    void OP_DADB(State8080&); // 0x09
    void OP_DADD(State8080&); // 0x19
    void OP_DADH(State8080&); // 0x29
    void OP_DADSP(State8080&); // 0x39
        // INX
    void OP_INXB(State8080&); // 0x03
    void OP_INXD(State8080&); // 0x13
    void OP_INXH(State8080&); // 0x23
    void OP_INXSP(State8080&); // 0x33
        // DCX
    void OP_DCXB(State8080&); // 0x0B
    void OP_DCXD(State8080&); // 0x1B
    void OP_DCXH(State8080&); // 0x2B
    void OP_DCXSP(State8080&); // 0x3B
        // PUSH/POP
    void OP_POPB(State8080&); // 0xc1
    void OP_PUSHB(State8080&); // 0xc5
    void OP_POPD(State8080&); // 0xd1
    void OP_PUSHD(State8080&); //0xd5
    void OP_POPH(State8080&); // 0xe1
    void OP_PUSHH(State8080&); // 0xe5
    void OP_POPPSW(State8080&); // 0xf1
    void OP_PUSHPSW(State8080&); // 0xf5

    void OP_XTHL(State8080&); // 0xE3
    void OP_XCHG(State8080&); // 0xeb
    void OP_SPHL(State8080&); // 0xF9

    /// Halt Instruction
    void OP_HLT(State8080&); // 0x76


    // functions that perform an opcode's operation using the registers themselves.
    inline void LXI_D16(State8080&, uint8_t&, uint8_t&);
    inline void DCR(State8080&, uint8_t&);
    inline void INR(State8080&, uint8_t&);
    inline void DAD(State8080&, uint8_t&, uint8_t&);
    inline void MVI_D8(State8080&, uint8_t&);
    inline void INX(uint8_t&, uint8_t&);
    inline void DCX(uint8_t& regPair1, uint8_t& regPair2);
    inline void MOV(uint8_t& dst, uint8_t& src);
    inline void MOV_DST(State8080&, uint8_t& dst);// This function is only used for the H & L pair memory location as a src.
    inline void MOV_SRC(State8080&, uint8_t& src); // H&L is the dst
    inline void POP(State8080&, uint8_t& regPair1, uint8_t& regPair2);
    inline void PUSH(State8080&, uint8_t& regPair1, uint8_t& regPair2);
    inline void JUMP(State8080&, bool canJump) const noexcept;
    inline void CALL(State8080&, bool canJump) const noexcept;
    inline void RET(State8080&, bool canRet) const noexcept;
    inline void RST(State8080&, const uint8_t& resNum);

    inline void ADD(State8080&, const uint8_t& reg);
    inline void ADC(State8080&, const uint8_t& reg);
    inline void SUB(State8080&, const uint8_t& reg);
    inline void SBB(State8080&, const uint8_t& reg);
    inline void XRA(State8080&, const uint8_t& reg);
    inline void ANA(State8080&, const uint8_t& reg);
    inline void ORA(State8080&, const uint8_t& reg);
    inline void CMP(State8080&, const uint8_t& reg);

};



#endif // DISASSEMBLER8080_HPP
