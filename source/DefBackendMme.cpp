#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "DefDocument.h"

void DefDocument::EmitMme(FILE* f)
{
	auto emitValue = [&](const char* name, const char* trail, int32_t value)
	{
		fprintf(f, "%%equ %s%s %d\n", name, trail, value);
	};

	auto emitHex = [&](const char* name, const char* trail, uint32_t value)
	{
		fprintf(f, "%%equ %s%s 0x%08x\n", name, trail, value);
	};

	auto emitReg = [&](const char* name, const char* trail, uint32_t value)
	{
		fprintf(f, "%%equ %s%s 0x%03x\n", name, trail, value);
	};

	auto emitEnum = [&](const char* name, int32_t body)
	{
		for (auto& f : m_enumBodies[body])
			emitValue(name, f.name.c_str(), f.value);
	};

	auto emitBits = [&](const char* basename, int32_t body)
	{
		for (auto& bit : m_bitsBodies[body])
		{
			std::string name = std::string{basename} + bit.name;
			uint32_t numbits = 1 + bit.bits.second - bit.bits.first;
			emitHex(name.c_str(), "", ((1U<<numbits)-1)<<bit.bits.first);
			if (bit.type.first == Field::Enum)
				for (auto& val : m_enumBodies[body])
					emitHex(name.c_str(), val.name.c_str(), val.value<<bit.bits.first);
			emitValue(name.c_str(), "_Size", numbits);
			emitValue(name.c_str(), "_Shift", bit.bits.first);
		}
	};

	auto emitField = [&](Field const& field)
	{
		auto impl = [&](const char* basename, uint32_t baseindex, Field const& field, auto& self) -> void
		{
			std::string compound_name;
			const char* name = field.name.c_str();
			if (basename)
			{
				compound_name = std::string{basename} + 'N' + name;
				name = compound_name.c_str();
			}
			if (field.type.first == Field::Array)
			{
				for (auto& subfield : m_arrayBodies[field.type.second])
					self(name, field.index, subfield, self);
				emitValue(name, "_Count", field.arraySize);
				emitValue(name, "_Shift", __builtin_ffs(field.arrayNext)-1);
				emitReg(name, "_Size", field.arrayNext);
				return;
			}
			emitReg(name, "", baseindex+field.index);
			if (field.arraySize > 1)
				emitValue(name, "_Count", field.arraySize);
			switch (field.type.first)
			{
				case Field::Bits:
					emitBits(name, field.type.second);
					break;
				case Field::Enum:
					emitEnum(name, field.type.second);
					break;
				case Field::Iova:
					emitReg(name, "_High", baseindex+field.index);
					emitReg(name, "_Low", baseindex+field.index+1);
					break;
			}
		};
		return impl(NULL, 0, field, impl);
	};

	for (auto& e : m_namedEnums)
		emitEnum(e.first.c_str(), e.second.body);

	for (auto& field : m_fields)
		emitField(field);
}
