#pragma once
#include <map>
#include <set>
#include <vector>
#include <memory>
#include <string>
#include <utility>
#include <algorithm>

template <typename U, typename V>
struct TrivialPair
{
	U first;
	V second;
};

class DefDocument
{
	std::vector<std::string> m_docStrings;

	std::string m_engineName;
	uint32_t m_engineClass;
	int32_t m_engineDoc;

	struct Field
	{
		enum
		{
			Uint,
			Bool,
			Float,
			Pipe,
			Iova,
			Enum,
			Bits,
			Array,
		};
		using Type = TrivialPair<int32_t,int32_t>;
		std::string name;
		int32_t doc;
		int32_t index;
		int32_t arraySize, arrayNext;
		Type type;
	};

	struct EnumField
	{
		std::string name;
		int32_t value;
		int32_t doc;
	};

	struct BitsField
	{
		using Range = TrivialPair<int32_t,int32_t>;
		std::string name;
		int32_t doc;
		Range bits;
		Field::Type type;
	};

	struct NamedEnum
	{
		int32_t body;
		int32_t doc;
	};

	std::vector<EnumField> m_curEnum;
	std::vector<std::vector<EnumField>> m_enumBodies;
	std::map<std::string, NamedEnum> m_namedEnums;

	std::vector<BitsField> m_curBits;
	std::vector<std::vector<BitsField>> m_bitsBodies;

	bool m_inArray = false;
	std::vector<Field> m_curArray;
	std::vector<std::vector<Field>> m_arrayBodies;

	std::vector<Field> m_fields;

public:
	bool Load(const char* fileName);
	void EmitCppHeader(FILE* f);
	void EmitMme(FILE* f);

	static constexpr auto MakeTypeUint() { return Field::Type{Field::Uint}; }
	static constexpr auto MakeTypeBool() { return Field::Type{Field::Bool}; }
	static constexpr auto MakeTypeFloat() { return Field::Type{Field::Float}; }
	static constexpr auto MakeTypePipe() { return Field::Type{Field::Pipe}; }
	static constexpr auto MakeTypeIova() { return Field::Type{Field::Iova}; }
	static constexpr auto MakeTypeEnum(int32_t i) { return Field::Type{Field::Enum,i}; }
	static constexpr auto MakeTypeBits(int32_t i) { return Field::Type{Field::Bits,i}; }
	static constexpr auto MakeTypeArray(int32_t i) { return Field::Type{Field::Array,i}; }

	int32_t PopEnumBody()
	{
		m_enumBodies.emplace_back(std::move(m_curEnum));
		return m_enumBodies.size()-1;
	}

	int32_t PopBitsBody()
	{
		m_bitsBodies.emplace_back(std::move(m_curBits));
		return m_bitsBodies.size()-1;
	}

	void PushArray()
	{
		m_inArray = true;
	}

	int32_t PopArray()
	{
		m_inArray = false;
		m_arrayBodies.emplace_back(std::move(m_curArray));
		return m_arrayBodies.size()-1;
	}

	int32_t VisitDoc(std::string&& str);
	const char* VisitEngine(std::string&& name, uint32_t _class, int32_t doc);
	const char* VisitReg(int32_t reg, std::string&& name, int32_t doc, int32_t size, Field::Type type);
	const char* VisitArray(int32_t base, std::string&& name, int32_t doc, int32_t size, int32_t body, int32_t next);
	const char* VisitEnum(std::string&& name, int32_t doc, int32_t body);
	const char* VisitEnumField(int32_t value, std::string&& name, int32_t doc);
	const char* VisitBitsField(BitsField::Range range, std::string&& name, int32_t doc, Field::Type type);
};
