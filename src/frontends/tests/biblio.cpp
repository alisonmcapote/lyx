#include <config.h>

#include <iostream>
#include <map>

#include <boost/regex.hpp>

using namespace std;

// Escape special chars.
// All characters are literals except: '.|*?+(){}[]^$\'
// These characters are literals when preceded by a "\", which is done here
// This function is unfortunately copied from ../frontend_helpers.cpp, so we should
// try to make sure to keep the two in sync.
string const escape_special_chars(string const & expr)
{
	// Search for all chars '.|*?+(){}[^$]\'
	// Note that '[' and '\' must be escaped.
	// This is a limitation of boost::regex, but all other chars in BREs
	// are assumed literal.
	boost::regex reg("[].|*?+(){}^$\\[\\\\]");

	// $& is a perl-like expression that expands to all of the current match
	// The '$' must be prefixed with the escape character '\' for
	// boost to treat it as a literal.
	// Thus, to prefix a matched expression with '\', we use:
	return boost::regex_replace(expr, reg, "\\\\$&");
}


typedef map<string, string> InfoMap;

// A functor for use with find_if, used to ascertain whether a
// data entry matches the required regex_
// This class is unfortunately copied from ../frontend_helpers.cpp, so we should
// try to make sure to keep the two in sync.
class RegexMatch : public unary_function<string, bool>
{
public:
	// re and icase are used to construct an instance of boost::RegEx.
	// if icase is true, then matching is insensitive to case
	RegexMatch(InfoMap const & m, string const & re, bool icase)
		: map_(m), regex_(re, icase) {}

	bool operator()(string const & key) const {
		// the data searched is the key + its associated BibTeX/biblio
		// fields
		string data = key;
		InfoMap::const_iterator info = map_.find(key);
		if (info != map_.end())
			data += ' ' + info->second;

		// Attempts to find a match for the current RE
		// somewhere in data.
		return boost::regex_search(data, regex_);
	}
private:
	InfoMap const map_;
	mutable boost::regex regex_;
};


void test_escape_special_chars()
{
	cout << escape_special_chars("abcd") << endl;
	cout << escape_special_chars("ab&cd") << endl;
	cout << escape_special_chars(".|*?+(){}[]^$\"") << endl;
	cout << escape_special_chars("..||**??++(()){{}}[[]]^^$$\\\\") << endl;
}


void test_RegexMatch()
{
	InfoMap im;
	im["hello"] = "hei";

	try {
		RegexMatch rm(im, "h.*o", false);

		cout << rm("hello") << endl;
		cout << rm("hei") << endl;
	}
	catch (boost::regex_error & regerr) {
		cout << regerr.what() << endl;
	}
}


int main()
{
	test_escape_special_chars();
	test_RegexMatch();
}
