#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "MmeDocument.h"

#include "MmeParser.hpp"
#define yyscan_t mme_scan_t
#define YYSTYPE MME_STYPE
#define YYLTYPE MME_LTYPE
#include "MmeLexer.hpp"

#define TRACE() printf("%s\n", __func__)

void mme_error(MME_LTYPE *llocp, mme_scan_t scanner, MmeDocument& doc, char const* curFile, char const *msg)
{
	//curFile = AbbreviatePath(doc.firstDir, curFile);
	fprintf(stderr, "%s:%d:%d: %s\n", curFile, llocp->first_line, llocp->first_column, msg);
	fprintf(stderr, "%s:%d:%d: (error ends here)\n", curFile, llocp->last_line, llocp->last_column);
}

static bool resolveLabels(std::vector<int32_t>& code, std::map<std::string, size_t>& labels, std::map<size_t, std::string>& refs)
{
	for (auto& ref : refs)
	{
		auto it = labels.find(ref.second);
		if (it == labels.end())
		{
			fprintf(stderr, "Undefined reference to %s\n", ref.second.c_str());
			return false;
		}
		if (ref.first < code.size())
		{
			int32_t& inst = code[ref.first];
			int32_t target = it->second - int32_t(ref.first);
			inst = (inst &~ mme::ImmediateMask) | mme::MakeImmediate(target);
		}
	}
	refs.clear();
	return true;
}

bool MmeDocument::ResolveLocalLabels()
{
	bool ok = resolveLabels(m_code, m_localLabels, m_localLabelRefs);
	m_localLabels.clear();
	return ok;
}

bool MmeDocument::ResolveLabels()
{
	return resolveLabels(m_code, m_labels, m_labelRefs);
}

bool MmeDocument::Load(const char* fileName)
{
	FILE* fin = fopen(fileName, "r");
	if (!fin)
	{
		fprintf(stderr, "%s: couldn't open file\n", fileName);
		return false;
	}

	mme_scan_t scanner;
	mme_lex_init(&scanner);
	mme_set_in(fin, scanner);
	int ret = mme_parse(scanner, *this, fileName);
	mme_lex_destroy(scanner);
	fclose(fin);

	if (ret==0)
		return ResolveLabels();

	return false;
}

void MmeDocument::Print()
{
	for (size_t i = 0; i < m_exports.size(); i ++)
		printf("[%3u] %s: 0x%04X\n", int32_t(i), m_exports[i]->first.c_str(), int32_t(m_exports[i]->second));
	for (size_t i = 0; i < m_code.size(); i ++)
		printf("[%04X] %08X\n", uint32_t(i), m_code[i]);
}

bool MmeDocument::VisitEqu(std::string&& ident, int32_t value)
{
	auto it = m_equs.find(ident);
	if (it == m_equs.end())
	{
		m_equs.emplace( std::move(ident), value );
		return true;
	}
	return false;
}

bool MmeDocument::VisitLocalLabel(std::string&& ident)
{
	auto it = m_localLabels.find(ident);
	if (it == m_localLabels.end())
	{
		m_localLabels.emplace( std::move(ident), m_code.size() );
		return true;
	}
	return true;
}

bool MmeDocument::VisitLabel(std::string&& ident, bool exported)
{
	if (!ResolveLocalLabels())
		return false;
	auto it = m_labels.find(ident);
	if (it == m_labels.end())
	{
		auto ret = m_labels.emplace( std::move(ident), m_code.size() );
		if (exported)
			m_exports.push_back(ret.first);
		return true;
	}
	return true;
}

void MmeDocument::VisitInstruction(int32_t inst)
{
	m_code.push_back(inst);
}

void MmeDocument::VisitLabelRef(std::string&& ident, bool local)
{
	if (local)
		m_localLabelRefs.emplace( m_code.size(), std::move(ident) );
	else
		m_labelRefs.emplace( m_code.size(), std::move(ident) );
}

bool MmeDocument::VisitExprIdent(std::string&& ident, int32_t& value)
{
	auto it = m_equs.find(ident);
	if (it != m_equs.end())
	{
		value = it->second;
		return true;
	}
	return false;
}
