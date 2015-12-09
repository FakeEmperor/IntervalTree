#pragma once
/*
*	Utils.h – this file used for storing some useful info	
*
*
*/

#include <string>
#include <exception>

namespace utils {
	class debug_exception :public std::exception {
	protected:
		static void make_throw_generic(std::string msg_append) {
			std::string msg = "Debug exception";
			msg += msg_append;
			throw std::exception(msg.c_str());
		}
	public:
		static void make_throw(char* msg = nullptr) {
			if (_DEBUG)
				make_throw_generic(std::string(msg));
		}

	};

	const char* stristr(const char* str1, const char* str2);

	const char* SkipSpace(const char* str);

	int my_strcmpi(const char* a, const char* b);
}