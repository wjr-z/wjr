#include "../include/mtool.h"
#ifndef __linux__
#include <io.h>
#include <direct.h>
#else
#include <sys/io.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#endif
#include <fstream>
#include <sstream>
#include <set>
#include <iostream>
#include <cassert>
#include "../include/thread_pool.h"

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

	bool dfs_get_files(String& path, std::vector<String>& file_path) {
	#ifndef __linux__
		intptr_t h_file;
		struct _finddata_t fileinfo {};
		if ((h_file = _findfirst(path.append("/*").c_str(), &fileinfo)) != -1) {
			path.pop_back();
			const size_t l = path.length();
			do {
				if ((fileinfo.attrib & _A_SUBDIR))
				{
					if (strcmp(fileinfo.name, ".") != 0 && strcmp(fileinfo.name, "..") != 0) {
						path.append(fileinfo.name);
						dfs_get_files(path, file_path);
					}
				}
				else
				{
					path.append(fileinfo.name);
					file_path.emplace_back(path);
				}
				path.resize(l);
			} while (_findnext(h_file, &fileinfo) == 0);
			_findclose(h_file);
			return true;
		}
		return false;
	#else
		struct dirent* ptr;
		DIR* dir = opendir(path.c_str());
		path.push_back('/');
		const size_t l = path.length();
		while ((ptr = readdir(dir)) != nullptr) {
			if(ptr->d_type == DT_DIR){
				if (strcmp(ptr->d_name, ".") != 0 && strcmp(ptr->d_name, "..") != 0) {
					path.append(ptr->d_name);
					dfs_get_files(path,file_path);
				}
			}else{
				path.append(ptr->d_name);
				file_path.emplace_back(path);
			}
			path.resize(l);
		}
		closedir(dir);
		return true;
	#endif
	}

	bool get_all_files(const String&path, std::vector<String>& filePath) {
		String cop(260, wjr::Reserved{});
		cop.assign(path);
		return dfs_get_files(cop, filePath);
	}

	std::vector<String> get_all_files(const String&path) {
		std::vector<String> file_path;
	#ifndef __linux__
		if (!_access(path.c_str(), 0)) {
			if (!get_all_files(path, file_path)) {
				file_path.push_back(path);
			}
		}
	#else
		if (!access(path.c_str(), F_OK)) {
			DIR* dir = opendir(path.c_str());
			if (dir == nullptr) {
				file_path.push_back((String)path);
			}
			else {
				closedir(dir);
				get_all_files(path, file_path);
			}
		}
	#endif
		return file_path;
	}

	String read_file(const String& filename) {
	#ifndef __linux
		if (_access(filename.c_str(), 0) == -1)
			return String();
	#else
		if (access(filename.c_str(), F_OK) == -1)
			return String();
	#endif
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

	void write_file(const String& filename, String_view str) {
		std::ofstream out(filename.c_str(), std::ios::binary);
		out.write(str.c_str(), static_cast<std::streamsize>(str.length()));
	}

	int create_file(const String& filename
	#ifdef __linux__
		,__mode_t __mode
	#endif
	) {
	#ifndef __linux__
		return mkdir(filename.c_str());
	#else
		return mkdir(filename.c_str(),__mode);
	#endif
	}

}
