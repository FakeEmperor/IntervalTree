#include "utils.h"

const char * utils::stristr(const char * str1, const char * str2)
{
	const char* p1 = str1;
	const char* p2 = str2;
	const char* r = *p2 == 0 ? str1 : 0;

	while (*p1 != 0 && *p2 != 0)
	{
		if (tolower(*p1) == tolower(*p2))
		{
			if (r == 0)
				r = p1;
			p2++;
		}
		else
		{
			p2 = str2;
			if (tolower(*p1) == tolower(*p2))
			{
				r = p1;
				p2++;
			}
			else
				r = 0;
		}
		p1++;
	}

	return *p2 == 0 ? r : 0;
}

const char * utils::SkipSpace(const char * str) {
	while (str&&isspace(*str))
		++str;
	return str;
}

int utils::my_strcmpi(const char * a, const char * b) {
#if defined(__ANDROID_API__) || defined(__LINUX__)
	return strcasecmp(a, b);
#else
	return _stricmp(a, b);
#endif
}
