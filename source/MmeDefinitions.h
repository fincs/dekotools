#pragma once
#include <stdint.h>

namespace mme
{
	enum class Operation : int32_t
	{
		Alu = 0,
		AddImmediate = 1,
		ExtractInsert = 2,
		ExtractShiftLeftImmediate = 3,
		ExtractShiftLeftRegister = 4,
		Read = 5,
		Branch = 7,
	};

	enum class AluOperation : int32_t
	{
		Add = 0,
		AddWithCarry = 1,
		Subtract = 2,
		SubtractWithBorrow = 3,
		Xor = 8,
		Or = 9,
		And = 10,
		AndNot = 11,
		Nand = 12
	};

	enum class ResultOperation : int32_t
	{
		IgnoreAndFetch = 0,
		Move = 1,
		MoveAndSetMethod = 2,
		FetchAndSend = 3,
		MoveAndSend = 4,
		FetchAndSetMethod = 5,
		MoveAndSetMethodFetchAndSend = 6,
		MoveAndSetMethodSend = 7
	};

	enum class BranchCondition : int32_t
	{
		Zero = 0,
		NotZero = 1,
		ZeroAnnul = 2,
		NotZeroAnnul = 3,
	};

	constexpr int32_t MakeImmediate(int32_t imm)
	{
		return ((imm & 0x3FFFF) << 14);
	}

	constexpr int32_t MakeAlu(AluOperation op, int32_t rA, int32_t rB)
	{
		return int32_t(Operation::Alu) | (rA << 11) | (rB << 14) | (int32_t(op) << 17);
	}

	constexpr int32_t MakeAddImmediate(int32_t rA, int32_t imm)
	{
		return int32_t(Operation::AddImmediate) | (rA << 11) | MakeImmediate(imm);
	}

	constexpr int32_t MakeExtractInsert(int32_t bA, int32_t rA, int32_t bB, int32_t rB, int32_t size)
	{
		return int32_t(Operation::ExtractInsert) | (rA << 11) | (rB << 14) | (bB << 17) | (size << 22) | (bA << 27);
	}

	constexpr int32_t MakeExtractShiftLeftImmediate(int32_t bA, int32_t rA, int32_t rB, int32_t size)
	{
		return int32_t(Operation::ExtractShiftLeftImmediate) | (rA << 11) | (rB << 14) | (size << 22) | (bA << 27);
	}

	constexpr int32_t MakeExtractShiftLeftRegister(int32_t rA, int32_t bB, int32_t rB, int32_t size)
	{
		return int32_t(Operation::ExtractShiftLeftRegister) | (rA << 11) | (rB << 14) | (bB << 17) | (size << 22);
	}

	constexpr int32_t MakeRead(int32_t rA, int32_t imm)
	{
		return int32_t(Operation::Read) | (rA << 11) | MakeImmediate(imm);
	}

	constexpr int32_t MakeBranch(BranchCondition cond, int32_t rA)
	{
		return int32_t(Operation::Branch) | (int32_t(cond) << 4) | (rA << 11);
	}

	constexpr int32_t MakeResult(ResultOperation op, int32_t rDst)
	{
		return (int32_t(op) << 4) | (rDst << 8);
	}

	constexpr int32_t MakeNopSource()
	{
		return MakeAddImmediate(0, 0);
	}

	constexpr int32_t MakeNopResult()
	{
		return MakeResult(ResultOperation::Move, 0);
	}

	constexpr int32_t MakeNop()
	{
		return MakeNopSource() | MakeNopResult();
	}

	constexpr int32_t ExitBit = 1 << 7;
	constexpr int32_t ImmediateMask = MakeImmediate(0x3FFFF);
}
