/*
	string_utils.hpp

	The MIT License (MIT)

	Copyright (c) 2013 Maxime Alvarez

	Permission is hereby granted, free of charge, to any person obtaining a copy of
	this software and associated documentation files (the "Software"), to deal in
	the Software without restriction, including without limitation the rights to
	use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
	the Software, and to permit persons to whom the Software is furnished to do so,
	subject to the following conditions:

	The above copyright notice and this permission notice shall be included in all
	copies or substantial portions of the Software.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
	IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
	FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
	COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
	IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
	CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

	string_utils is a class which defines some useful static methods to use with std::string.
*/

#ifndef STRING_UTILS_HPP
#define STRING_UTILS_HPP

#include <algorithm>
#include <functional>
#include <locale>
#include <sstream>

class string_utils
{
	public:
		// Trim left and right side of the string.
		static std::string trim(std::string str)
		{
			str.erase(str.begin(), std::find_if(str.begin(), str.end(), std::not1(std::ptr_fun<int, int>(std::isspace))));
			str.erase(std::find_if(str.rbegin(), str.rend(), std::not1(std::ptr_fun<int, int>(std::isspace))).base(), str.end());

			return str;
		}

		// Convert std::string to template element.
		template <typename T>
		static T to(std::string str)
		{
			T obj;

			std::stringstream stream(str);
			stream >> obj;

			return obj;
		}

		// Convert template element to std::string.
		template <typename T>
		static std::string from(T element)
		{
			std::stringstream stream;
			stream << element;

			return stream.str();
		}

        // Convert a std::string to lowercase.
        static std::string lowercase(std::string data)
        {
            std::transform(data.begin(), data.end(), data.begin(), ::tolower);
            return data;
        }
};

#endif // STRING_UTILS_HPP
