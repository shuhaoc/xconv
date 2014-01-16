// chkconv.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <mrl/utility/CodeConv.h>

int _tmain(int argc, _TCHAR* argv[]) {
	if (argc == 2) {
		fstream file(argv[1], ios_base::in | ios_base::out | ios_base::binary);
		if (file.good()) {
			file.seekg(0, ios_base::end);
			unsigned size = file.tellg();
			file.seekg(0);
			if (size >= 3) {
				string fileHeader(3, '\0');
				file.read(const_cast<char*>(fileHeader.data()), 3);
				if (file.gcount() == 3) {
					string utf8Header = "\xEF\xBB\xBF";
					if (fileHeader != utf8Header) {
						file.seekg(0);
						string asciiData(size, '\0');
						file.read(const_cast<char*>(asciiData.data()), size);
						if (file.gcount() == size) {
							wstring utf16Data = mrl::utility::codeconv::asciiToUnicode(asciiData);
							string utf8Data = mrl::utility::codeconv::unicodeToUtf8(utf16Data);
							file.seekp(ios_base::beg);
							file.write(utf8Header.data(), utf8Header.size());
							file.write(utf8Data.data(), utf8Data.size());
							return 0;
						}
						return -6; // 读文件出错
					}
					return -5; // 文件头不是UTF-8
				}
				return -4; // 读文件出错
			}
			return -3; // 文件大小不足3
		}
		return -2; // 文件打开错误
	}
	return -1;	// 参数错误
}