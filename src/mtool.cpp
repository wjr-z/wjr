#include "mtool.h"

#include <io.h>
#include <fstream>
#include <sstream>
#include <set>
#include <iostream>
#include <cassert>
#include <direct.h>

namespace wjr {

	time_ref mtime() { return time_ref::now(); }

	void mtime(time_ref* x) { *x = mtime(); }

	time_ref::m_clock::time_point time_ref::now() {
		return m_clock::now();
	}
	double time_ref::testTime(const m_clock::time_point& start, const m_clock::time_point& end) {
		return static_cast<double>(std::chrono::duration_cast<m_mis>(end - start).count()) * 0.001;
	}

	time_ref::time_ref(const m_clock::time_point& TimePoint) :TimePoint(TimePoint) {}

	double operator-(const time_ref& lhs, const time_ref& rhs) {
		return time_ref::testTime(rhs.TimePoint, lhs.TimePoint);
	}

	void dfs_get_files(String& path, std::vector<String>& file_path) {
		intptr_t h_file;
		struct _finddata_t fileinfo {};
		if ((h_file = _findfirst(path.append("/*").c_str(), &fileinfo)) != -1) {
			path.pop_back();
			path.push_back('\\');
			const size_t l = path.length();
			do {
				path.append(fileinfo.name);
				if ((fileinfo.attrib & _A_SUBDIR))
				{
					if (strcmp(fileinfo.name, ".") != 0 && strcmp(fileinfo.name, "..") != 0)
						dfs_get_files(path, file_path);
				}
				else
				{
					file_path.emplace_back(path);
				}
				path.resize(l);
			} while (_findnext(h_file, &fileinfo) == 0);
			_findclose(h_file);
		}
	}

	void get_all_files(String_view path, std::vector<String>& filePath) {
		String cop(path);
		dfs_get_files(cop, filePath);
	}

	std::vector<String> get_all_files(String_view path) {
		std::vector<String> file_path;
		if (!_access(path.c_str(), 0))
			get_all_files(path, file_path);
		struct _finddata_t fileinfo {};
		if ((_findfirst(path.c_str(), &fileinfo)) != -1) {
			if (!(fileinfo.attrib & _A_SUBDIR)) {
				if (strcmp(fileinfo.name, ".") != 0 && strcmp(fileinfo.name, "..") != 0) {
					file_path.push_back(path);
				}
			}
		}
		return file_path;
	}

	String read_file(String_view filename) {
		if (_access(filename.c_str(), 0) == -1)
			return String();
		FILE* fp = fopen(filename.c_str(), "rb");
		if (fp == nullptr)return String();
		fseek(fp, 0, SEEK_END);
		size_t size = ftell(fp);
		String buffer(size, Uninitialized{});
		fseek(fp, 0, SEEK_SET);
		fread((void*)buffer.c_str(), sizeof(char), size, fp);
		fclose(fp);
		return buffer;
	}

	void write_file(String_view filename, String_view str) {
		std::ofstream out(filename.c_str(), std::ios::binary);
		out.write(str.c_str(), static_cast<std::streamsize>(str.length()));
	}

}
