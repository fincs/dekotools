#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "DefDocument.h"

void DefDocument::EmitCppHeader(FILE* f)
{
	std::string engineName = "Engine";
	if (m_engineName[0] != '_')
		engineName += m_engineName;
	else
		engineName += m_engineName.c_str()+1;

	auto emitBits = [&](const char* indent, int32_t body)
	{
		for (auto& bit : m_bitsBodies[body])
		{
			uint32_t numbits = 1 + bit.bits.second - bit.bits.first;
			fprintf(f, "%s\tstruct %s : __bit<%u,%u> {\n", indent,
				bit.name.c_str(), bit.bits.first, numbits);
			fprintf(f, "%s\t\tusing __bit::__bit;\n", indent);
			if (bit.type.first == Field::Enum)
				for (auto& val : m_enumBodies[bit.type.second])
					fprintf(f, "%s\t\tstatic constexpr uint32_t %s = 0x%08X;\n", indent,
						val.name.c_str(), val.value<<bit.bits.first);
			fprintf(f, "%s\t};\n", indent);
		};
	};

	auto emitField = [&](Field const& field)
	{
		auto impl = [&](Field const& field, bool nested, auto& self) -> void
		{
			const char* indent = nested ? "\t\t\t" : "\t\t";
			if (field.type.first == Field::Array)
			{
				fprintf(f, "%sstruct %s : __array<__self, 0x%03x, %u, %u> {\n", indent,
					field.name.c_str(), field.index, field.arraySize, __builtin_ffs(field.arrayNext)-1);
				fprintf(f, "%s\tusing __self = %s;\n", indent, field.name.c_str());
				for (auto& subfield : m_arrayBodies[field.type.second])
					self(subfield, true, self);
				fprintf(f, "%s};\n", indent);
				return;
			}
			uint32_t regsize = field.arraySize;
			if (field.type.first == Field::Iova)
				regsize *= 2;
			fprintf(f, "%sstruct %s : __reg<__self, 0x%03x, %u> {\n", indent,
				field.name.c_str(), field.index, regsize);
			fprintf(f, "%s\tusing __reg::__reg;\n", indent);
			switch (field.type.first)
			{
				case Field::Bits:
					emitBits(indent, field.type.second);
					break;
				case Field::Enum:
					for (auto& val : m_enumBodies[field.type.second])
						fprintf(f, "%s\tstatic constexpr uint32_t %s = %u;\n", indent,
							val.name.c_str(), val.value);
					break;
			}
			fprintf(f, "%s};\n", indent);
		};
		return impl(field, false, impl);
	};

	fprintf(f, "#pragma once\n");
	fprintf(f, "#include <stdint.h>\n\n");
	fprintf(f, "namespace maxwell {\n\n");
	fprintf(f, "#ifndef __dekodef_helpers\n");
	fprintf(f, "#define __dekodef_helpers\n");
	fprintf(f, "\ttemplate <typename T> constexpr auto ClassId = T::__classid;\n");
	fprintf(f, "\ttemplate <typename T, uint16_t index, uint16_t size>\n");
	fprintf(f, "\tstruct __reg {\n");
	fprintf(f, "\t\tusing Parent = T;\n");
	fprintf(f, "\t\tstatic constexpr auto Index = index;\n");
	fprintf(f, "\t\tstatic constexpr auto Size = size;\n");
	fprintf(f, "\t\tconst uint16_t __idx;\n");
	fprintf(f, "\t\tconstexpr operator uint16_t() const noexcept { return __idx; }\n");
	fprintf(f, "\t\tconstexpr __reg() noexcept : __idx{Index} {}\n");
	fprintf(f, "\t\tconstexpr __reg(uint16_t i) noexcept : __idx{ Parent::Index + i*Parent::Size + Index } {}\n");
	fprintf(f, "\t};\n");
	fprintf(f, "\ttemplate <typename T, uint16_t index, uint16_t count, unsigned shift>\n");
	fprintf(f, "\tstruct __array : __reg<T,index,(1U<<shift)> {\n");
	fprintf(f, "\t\tusing Parent = T;\n");
	fprintf(f, "\t\tstatic constexpr bool __isarray = true;\n");
	fprintf(f, "\t\tstatic constexpr auto Count = count;\n");
	fprintf(f, "\t\tstatic constexpr auto Shift = shift;\n");
	fprintf(f, "\t};\n");
	fprintf(f, "\ttemplate <unsigned shift, unsigned size>\n");
	fprintf(f, "\tstruct __bit {\n");
	fprintf(f, "\t\tstatic constexpr auto Shift = shift;\n");
	fprintf(f, "\t\tstatic constexpr auto Size = size;\n");
	fprintf(f, "\t\tstatic constexpr uint32_t Mask = ((1U<<Size)-1)<<Shift;\n");
	fprintf(f, "\t\tconst uint32_t __value;\n");
	fprintf(f, "\t\tconstexpr operator uint32_t() const noexcept { return __value; }\n");
	fprintf(f, "\t\tconstexpr __bit(uint32_t x = UINT32_MAX) noexcept : __value{ (x<<Shift) & Mask } {}\n");
	fprintf(f, "\t};\n");
	fprintf(f, "#endif\n\n");
	fprintf(f, "\tstruct %s {\n", engineName.c_str());
	fprintf(f, "\t\tusing __self = %s;\n", engineName.c_str());
	fprintf(f, "\t\tstatic constexpr auto __classid = 0x%04x;\n", m_engineClass);

	for (auto& field : m_fields)
		emitField(field);

	fprintf(f, "\t};\n\n");
	fprintf(f, "}\n");
}
