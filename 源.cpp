#include <algorithm>
#include <vector>
#include <deque>
#include "src/mtool.h"
#include "src/json/json.h"
using namespace wjr;
using namespace std;
#include "src/string_helper.h"
int main(){

	//return 0;
	auto vec = get_all_files("test");
	auto s = mtime();
	for (auto& i : vec) {
		json::eval(read_file(i));
	}
	auto t = mtime();
	cout<<t-s<<'\n';
	/*locale it("");
	const auto& g = std::use_facet<std::ctype<char>>(it);

	int ans = 0;
	int n = 1e7;
	auto s = mtime();
	for (int i = 0; i < n; ++i) {
		ans += g.is(ctype_base::digit,static_cast<char>(i & 0xFF));
	}
	auto t = mtime();
	cout<<t-s<<'\n';
	cout<<ans<<'\n';
	ans = 0;
	s = mtime();
	for (int i = 0; i < n; ++i) {
		ans += quick_isdigit(static_cast<char>(i & 0xFF));
	}
	t = mtime();
	cout<< t-s <<'\n';
	cout<<ans<<'\n';
	return 0;*/
	/*using convert_facet = std::codecvt<wchar_t,char,mbstate_t>;
	String a("ÖÐ¹ú");
	wString b;
	b.reserve(5);
	std::locale sys_locale;
	locale::global(sys_locale);
	mbstate_t out_state;
	const char* next = nullptr;
	wchar_t* wnext = nullptr;
	auto result = std::use_facet<convert_facet>(sys_locale).in(
		out_state, a.begin(),a.end(),next,
		b.begin(),b.begin() + 5,wnext);
	b.resize(wnext - b.begin());
	cout<<wnext - b.begin()<<'\n';
	wcout<<b<<'\n';*/

	return 0;
}