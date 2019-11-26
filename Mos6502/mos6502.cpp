#include <stdint.h>
#include <cassert>

using u8 = uint8_t;
using u16 = uint16_t;
using i8 = int8_t;

class Mos6502 {
	u8 A;
	u8 X;
	u8 Y;

	union {
		struct {
			bool N : 1;
			bool V : 1;
			bool _ : 1;
			bool B : 1;
			bool D : 1;
			bool I : 1;
			bool Z : 1;
			bool C : 1;
		};
		u8 raw;
	} flags;

	u16 PC;
	u8 S;



	void movGeneric(const u8 src, u8& dest) {
		dest = src;
		flags.Z = !dest;
		flags.N = dest >> 7;
	}
	bool conditionalBranch(bool condition, i8 value) {
		if(condition)
			PC += value;
		return condition;
	}

	void ADC(const u8 value) {
		// TODO: Add support for decimal mode
		u16 sum = A + value + flags.C;
		flags.C = sum > 0xff;
		flags.V = ~(A ^ value) & (A ^ sum) & 0x80;
		A = sum;
		flags.Z = !A;
		flags.N = A >> 7;
	}
	void AND(const u8 value) {
		A &= value;
		flags.Z = !A;
		flags.N = A >> 7;
	}
	void ASL(u8& value) {
		flags.C = value >> 7;
		value >>= 1;
		flags.Z = !value;
		flags.N = value >> 7;
	}
	bool BCC(const i8 value) { return conditionalBranch(!flags.C, value); }
	bool BCS(const i8 value) { return conditionalBranch( flags.C, value); }
	bool BEQ(const i8 value) { return conditionalBranch( flags.Z, value); }
	void BIT(const u8 value) {
		u8 result = A & value;
		flags.Z = !result;
		flags.N = result >> 7;
		flags.V = result >> 6 & 1;
	}
	bool BMI(const i8 value) { return conditionalBranch( flags.N, value); }
	bool BNE(const i8 value) { return conditionalBranch(!flags.Z, value); }
	bool BPL(const i8 value) { return conditionalBranch(!flags.N, value); }
	void BRK() {
		// TODO: Implement interrupts
	}
	bool BVC(const i8 value) { return conditionalBranch(!flags.V, value); }
	bool BVS(const i8 value) { return conditionalBranch( flags.V, value); }
	void CLC() { flags.C = 0; }
	void CLD() { flags.D = 0; }
	void CLI() { flags.I = 0; }
	void CLV() { flags.V = 0; }
	void CMP(const u8 value) {
		flags.Z = A == value;
		flags.N = (A - value) >> 7;
		flags.C = value > A;
	}
	void CPX(const u8 value) {
		flags.Z = X == value;
		flags.N = (X - value) >> 7;
		flags.C = value > X;
	}
	void CPY(const u8 value) {
		flags.Z = Y == value;
		flags.N = (Y - value) >> 7;
		flags.C = value > Y;
	}
	void DEC(u8& value) {
		value--;
		flags.Z = !value;
		flags.N = value >> 7;
	}
	void DEX() { DEC(X); }
	void DEY() { DEC(Y); }
	void EOR(const u8 value) {
		A ^= value;
		flags.Z = !A;
		flags.N = A >> 7;
	}
	void INC(u8& value) {
		value++;
		flags.Z = !value;
		flags.N = value >> 7;
	}
	void INX() { INC(X); }
	void INY() { INC(Y); }
	void JMP(const u8 value) { PC = value; }
	void JSR(const u8 value) {
		pushWord(PC + 1);
		PC = value;
	}
	void LDA(const u8 value) { movGeneric(value, A); }
	void LDX(const u8 value) { movGeneric(value, X); }
	void LDY(const u8 value) { movGeneric(value, Y); }
	void LSR(u8& value) {
		flags.C = value;
		value >>= 1;
		flags.N = 0;
		flags.Z = !value;
	}
	void NOP() { /* Does nothing */ }
	void ORA(const u8 value) {
		A |= value;
		flags.Z = !A;
		flags.N = A >> 7;
	}
	void PHA() { pushByte(A); }
	void PHP() { pushByte(flags.raw); }
	void PLA() { A = popByte(); }
	void PLP() { flags.raw = popByte(); }
	void ROL(u8& value) {
		bool temp = flags.C;
		flags.C = value >> 7;
		value = value << 1 | temp;
		flags.N = value >> 7;
		flags.Z = !value;
	}
	void ROR(u8& value) {
		bool temp = flags.C;
		flags.C = value;
		value = value >> 1 | temp << 7;
		flags.N = value >> 7;
		flags.Z = !value;
	}
	void RTI() {
		flags.raw = popByte();
		PC = popWord();
	}
	void RTS() { PC = popWord() + 1; }
	void SBC(const u8 value) { ADC(~value); }
	void SEC() { flags.C = 1; }
	void SED() { flags.D = 1; }
	void SEI() { flags.I = 1; }
	void STA(const u16 address) { writeByte(address, A); }
	void STX(const u16 address) { writeByte(address, X); }
	void STY(const u16 address) { writeByte(address, Y); }
	void TAX() { movGeneric(A, X); }
	void TAY() { movGeneric(A, Y); }
	void TSX() { movGeneric(S, X); }
	void TXA() { movGeneric(X, A); }
	void TXS() { movGeneric(X, S); }
	void TYA() { movGeneric(Y, A); }

	void invalidOpcode() { assert(!"Invalid opcode"); }



public:
	void reset() {
		flags.B = 1;
		flags.D = 0;
		flags.I = 1;
		flags._ = 1;
		PC = readWord(0xfffc);
		S = 0xff;
	}

	virtual u8 readByte(u16 addresss);
	virtual void writeByte(u16 address, u8 value);

	u16 readWord(u16 address) { return (u16)readByte(address + 1) << 8 | readByte(address); }
	u16 writeWord(u16 address, u16 value) {
		writeByte(address, value);
		writeByte(address + 1, value >> 8);
	}

	u8 popByte() { return readByte(++S); }
	void pushByte(u8 value) { writeByte(S--, value); }
	u16 popWord() { return popByte() | (u16)popByte() >> 8; }
	void pushWord(u16 value) {
		pushByte(value >> 8);
		pushByte(value);
	}

	const u8 operandImm() { return readByte(PC + 1); }
	const u8 operandImmWord() { return readWord(PC + 1); }
	const u8 operandZpg() { return readByte(readByte(PC + 1)); }
	const u8 operandAbs() { return readByte(readWord(PC + 1)); }

	void step() {
		switch(readByte(PC)) {
		case 0x00: // BRK
			//BRK();
			// TODO: Implement interrupts
			assert(!"BRK hasn't been implemented yet");
			break;
		case 0x01:
		case 0x05: // ORA zpg
			ORA(operandZpg());
			PC += 2;
			break;
		case 0x06:
		case 0x08: // PHP
			PHP();
			PC++;
			break;
		case 0x09: // ORA #
			ORA(operandImm());
			PC += 2;
			break;
		case 0x0a:
		case 0x0d:
		case 0x0e:
		case 0x10: // BPL rel
			BPL(operandImm());
			PC += 2;
			break;
		case 0x11:
		case 0x15:
		case 0x16:
		case 0x18: // CLC
			CLC();
			PC++;
			break;
		case 0x19:
		case 0x1d:
		case 0x1e:
		case 0x20: // JSR abs
			JSR(operandImmWord());
			break;
		case 0x21:
		case 0x24: // BIT zpg
			BIT(operandZpg());
			PC += 2;
			break;
		case 0x25: // AND zpg
			AND(operandZpg());
			PC += 2;
			break;
		case 0x26:
		case 0x28: // PLP
			PLP();
			PC++;
			break;
		case 0x29: // AND #
			AND(operandImm());
			PC += 2;
			break;
		case 0x2a:
		case 0x2c: // BIT abs
			BIT(operandAbs());
			PC += 3;
			break;
		case 0x2d:
		case 0x2e:
		case 0x30: // BMI rel
			BMI(operandImm());
			PC += 2;
			break;
		case 0x31:
		case 0x35:
		case 0x36:
		case 0x38: // SEC
			SEC();
			PC++;
			break;
		case 0x39:
		case 0x3c:
		case 0x3d:
		case 0x3e:
		case 0x40: // RTI
			RTI();
			PC++;
			break;
		case 0x41:
		case 0x45: // EOR zpg
			EOR(operandZpg());
			PC += 2;
			break;
		case 0x46:
		case 0x48: // PHA
			PHA();
			PC++;
			break;
		case 0x49: // EOR #
			EOR(operandImm());
			PC += 2;
			break;
		case 0x4a:
		case 0x4c: // JMP abs
			JMP(operandImmWord());
			PC += 3;
			break;
		case 0x4d:
		case 0x4e:
		case 0x50: // BVC rel
			BVC(operandImm());
			PC += 2;
			break;
		case 0x51:
		case 0x55:
		case 0x56:
		case 0x58: // CLI
			CLI();
			PC++;
			break;
		case 0x59:
		case 0x5c:
		case 0x5d:
		case 0x5e:
		case 0x60: // RTS
			RTS();
			PC++;
			break;
		case 0x61:
		case 0x65: // ADC zpg
			ADC(operandZpg());
			PC += 2;
			break;
		case 0x66:
		case 0x68: // PLA
			PLA();
			PC++;
			break;
		case 0x69: // ADC #
			ADC(operandImm());
			PC += 2;
			break;
		case 0x6a:
		case 0x6c:
		case 0x6d:
		case 0x6e:
		case 0x70: // BVS rel
			BVS(operandImm());
			PC += 2;
			break;
		case 0x71:
		case 0x75:
		case 0x76:
		case 0x78: // SEI
			SEI();
			PC++;
			break;
		case 0x79:
		case 0x7c:
		case 0x7d:
		case 0x7e:
		case 0x81:
		case 0x84:
		case 0x85:
		case 0x86:
		case 0x88: // DEY
			DEY();
			PC++;
			break;
		case 0x8a: // TXA
			TXA();
			PC++;
			break;
		case 0x8c:
		case 0x8d:
		case 0x8e:
		case 0x90: // BCC rel
			BCC(operandImm());
			PC += 2;
			break;
		case 0x91:
		case 0x94:
		case 0x95:
		case 0x96:
		case 0x98: // TYA
			TYA();
			PC++;
			break;
		case 0x99:
		case 0x9a: // TXS
			TXS();
			PC++;
			break;
		case 0x9c:
		case 0x9d:
		case 0xa0: // LDY #
			LDY(operandImm());
			PC += 2;
			break;
		case 0xa1:
		case 0xa2: // LDX #
			LDX(operandImm());
			PC += 2;
			break;
		case 0xa4: // LDY zpg
			LDY(operandZpg());
			PC += 2;
			break;
		case 0xa5: // LDA zpg
			LDA(operandZpg());
			PC += 2;
			break;
		case 0xa6: // LDX zpg
			LDX(operandZpg());
			PC += 2;
			break;
		case 0xa8: // TAY
			TAY();
			PC++;
			break;
		case 0xa9: // LDA #
			LDA(operandImm());
			PC += 2;
			break;
		case 0xaa: // TAX
			TAX();
			PC++;
			break;
		case 0xac: // LDY abs
			LDY(operandAbs());
			PC += 3;
			break;
		case 0xad:
		case 0xae:
		case 0xb0: // BCS rel
			BCS(operandImm());
			PC += 2;
			break;
		case 0xb1:
		case 0xb4:
		case 0xb5:
		case 0xb6:
		case 0xb8: // CLV
			CLV();
			PC++;
			break;
		case 0xb9:
		case 0xba: // TSX
			TSX();
			PC++;
			break;
		case 0xbc:
		case 0xbd:
		case 0xbe:
		case 0xc0: // CPY #
			CPY(operandImm());
			PC += 2;
			break;
		case 0xc1:
		case 0xc4: // CPY zpg
			CPY(operandZpg());
			PC += 2;
			break;
		case 0xc5: // CMP zpg
			CMP(operandZpg());
			PC += 2;
			break;
		case 0xc6:
		case 0xc8: // INY
			INY();
			PC++;
			break;
		case 0xc9: // CMP #
			CMP(operandImm());
			PC += 2;
			break;
		case 0xca: // DEX
			DEX();
			PC++;
			break;
		case 0xcc: // CPY abs
			CPY(operandAbs());
			PC += 3;
			break;
		case 0xcd:
		case 0xce:
		case 0xd0: // BNE rel
			BNE(operandImm());
			PC += 2;
			break;
		case 0xd1:
		case 0xd5:
		case 0xd6:
		case 0xd8: // CLD
			CLD();
			PC++;
			break;
		case 0xd9:
		case 0xdc:
		case 0xdd:
		case 0xde:
		case 0xe0: // CPX #
			CPX(operandImm());
			PC += 2;
			break;
		case 0xe1:
		case 0xe4: // CPX zpg
			CPX(operandZpg());
			PC += 2;
			break;
		case 0xe5: // SBC zpg
			SBC(operandZpg());
			PC += 2;
			break;
		case 0xe6:
		case 0xe8: // INX
			INX();
			PC++;
			break;
		case 0xe9: // SBC #
			SBC(operandImm());
			PC += 2;
			break;
		case 0xea: // NOP
			NOP();
			PC++;
			break;
		case 0xec: // CPX abs
			CPX(operandAbs());
			PC += 3;
			break;
		case 0xed:
		case 0xee:
		case 0xf0: // BEQ rel
			BEQ(operandImm());
			PC += 2;
			break;
		case 0xf1:
		case 0xf5:
		case 0xf6:
		case 0xf8: // SED
			SED();
			PC++;
			break;
		case 0xf9:
		case 0xfc:
		case 0xfd:
		case 0xfe:

		default: // Invalid
			invalidOpcode();
			break;
		}
	}

	Mos6502() {
		reset();
	}
	
};
