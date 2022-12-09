#include <string>
#include <fstream>
#include <regex>

int main(int argc, char** argv) {
	auto file1 = argv[1];
	auto file2 = argv[2];
	// 读入file1 到 s1
	// 读入file2 到 s2
	// 将 s1 中的 wjr/ 和 wjr:: 去除
	// 将 s2 中的 std/ 和 std:: 去除
	// 最后全部写回文件
	std::string s1, s2;
	std::ifstream ifs1(file1);
	std::ifstream ifs2(file2);
	std::ofstream ofs1(file1);
	std::ofstream ofs2(file2);
	ifs1 >> s1;
	ifs2 >> s2;

	std::regex reg1("wjr::");
	std::regex reg2("wjr/");
	std::regex reg3("std::");
	std::regex reg4("std/");
	s1 = std::regex_replace(s1, reg1, "");
	s1 = std::regex_replace(s1, reg2, "");
	s2 = std::regex_replace(s2, reg3, "");
	s2 = std::regex_replace(s2, reg4, "");
	
	ofs1 << s1;
	ofs2 << s2;
	
	std::string cmd("/usr/local/wjr/compare/tools/compare.py benchmarks ");
	cmd.append(file1).push_back(' ');
	cmd.append(file2);
	system(cmd.data());
	
	return 0;
}