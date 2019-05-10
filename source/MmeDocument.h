#pragma once
#include <map>
#include <set>
#include <vector>
#include <memory>
#include <string>
#include <utility>
#include <algorithm>

#include "MmeDefinitions.h"

template <typename U, typename V>
struct TrivialPair
{
	U first;
	V second;
};

struct StringPair
{
	typedef std::shared_ptr<StringPair> Ptr;
	std::string first, second;

	StringPair(std::string&& first, std::string&& second)
		: first(first), second(second) { }
};

class MmeDocument
{
	std::vector<int32_t> m_code;
	std::map<std::string, int32_t> m_equs;
	std::map<std::string, size_t> m_localLabels;
	std::map<std::string, size_t> m_labels;
	std::vector<decltype(m_labels)::iterator> m_exports;
	std::map<size_t, std::string> m_localLabelRefs;
	std::map<size_t, std::string> m_labelRefs;

	bool ResolveLocalLabels();
	bool ResolveLabels();

public:
	bool Load(const char* fileName);
	void Print();

	bool VisitEqu(std::string&& ident, int32_t value);
	bool VisitLocalLabel(std::string&& ident);
	bool VisitLabel(std::string&& ident, bool exported);
	void VisitInstruction(int32_t inst);
	void VisitLabelRef(std::string&& ident, bool local);
	bool VisitExprIdent(std::string&& ident, int32_t& value);
};
