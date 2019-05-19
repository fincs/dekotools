#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "DefDocument.h"

#include "DefParser.hpp"
#define yyscan_t def_scan_t
#define YYSTYPE DEF_STYPE
#define YYLTYPE DEF_LTYPE
#include "DefLexer.hpp"

void def_error(DEF_LTYPE *llocp, def_scan_t scanner, DefDocument& doc, char const* curFile, char const *msg)
{
	//curFile = AbbreviatePath(doc.firstDir, curFile);
	fprintf(stderr, "%s:%d:%d: %s\n", curFile, llocp->first_line, llocp->first_column, msg);
	fprintf(stderr, "%s:%d:%d: (error ends here)\n", curFile, llocp->last_line, llocp->last_column);
}

bool DefDocument::Load(const char* fileName)
{
	FILE* fin = fopen(fileName, "r");
	if (!fin)
	{
		fprintf(stderr, "%s: couldn't open file\n", fileName);
		return false;
	}

	def_scan_t scanner;
	def_lex_init(&scanner);
	def_set_in(fin, scanner);
	int ret = def_parse(scanner, *this, fileName);
	def_lex_destroy(scanner);
	fclose(fin);

	if (ret==0)
		return true;

	return false;
}

int32_t DefDocument::VisitDoc(std::string&& str)
{
	m_docStrings.emplace_back(std::move(str));
	return m_docStrings.size()-1;
}

const char* DefDocument::VisitEngine(std::string&& name, uint32_t _class, int32_t doc)
{
	m_engineName = std::move(name);
	m_engineClass = _class;
	m_engineDoc = doc;

	if (m_engineClass &~ 0xFFFF)
		return "Class ID out of bounds";

	return nullptr;
}

const char* DefDocument::VisitReg(int32_t reg, std::string&& name, int32_t doc, int32_t size, Field::Type type)
{
	auto& v = m_inArray ? m_curArray : m_fields;
	for (auto& f : v)
		if (f.name == name)
			return "Name already in use";
	v.emplace_back(Field{ std::move(name), doc, reg, size, 0, type });
	return nullptr;
}

const char* DefDocument::VisitArray(int32_t base, std::string&& name, int32_t doc, int32_t size, int32_t body, int32_t next)
{
	for (auto& f : m_fields)
		if (f.name == name)
			return "Name already in use";
	m_fields.emplace_back(Field{ std::move(name), doc, base, size, next, MakeTypeArray(body) });
	return nullptr;
}

const char* DefDocument::VisitEnum(std::string&& name, int32_t doc, int32_t body)
{
	auto it = m_namedEnums.find(name);
	if (it != m_namedEnums.end())
		return "Duplicate enum name";
	m_namedEnums.emplace(std::move(name), NamedEnum{body, doc});
	return nullptr;
}

const char* DefDocument::VisitEnumField(int32_t value, std::string&& name, int32_t doc)
{
	for (auto& f : m_curEnum)
		if (f.name == name)
			return "Duplicate enum value";
	m_curEnum.emplace_back(EnumField{ std::move(name), value, doc });
	return nullptr;
}

const char* DefDocument::VisitBitsField(BitsField::Range range, std::string&& name, int32_t doc, Field::Type type)
{
	for (auto& f : m_curBits)
		if (f.name == name)
			return "Duplicate bitfield";
	m_curBits.emplace_back(BitsField{ std::move(name), doc, range, type });
	return nullptr;
}
