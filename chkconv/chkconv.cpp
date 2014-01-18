// chkconv.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <mrl/utility/CodeConv.h>

#ifdef _UNICODE
#define tcout std::wcout
#else
#define tcout std::cout
#endif

enum {
	Error_ArgumentCount = 1,
	Error_OpenFile,
	Error_ReadFile,
	Error_WriteFile,
};

enum {
	FileEncoding_ASCII = -3,
	FileEncoding_UTF16LE,
	FileEncoding_UTF8,
};

string utf16LeHeader = "\xFF\xFE";
string utf8Header = "\xEF\xBB\xBF";

unsigned tellFileSize(fstream& fs) {
	assert(fs.good());

	streamsize orignPos = fs.tellg();
	fs.seekg(0, ios_base::end);
	// NOTICE: 不考虑4GB以上文件
	unsigned size = static_cast<unsigned>(fs.tellg());
	fs.seekg(orignPos);
	return size;
}

int recognizeFileEncoding(fstream& fs) {
	if (fs.fail()) {
		return Error_OpenFile; // 文件打开错误
	}
	unsigned size = tellFileSize(fs);
	if (size < 0) {
		return Error_ReadFile;
	}
	if (size >= 2) {
		string fileHeader(2, '\0');
		fs.read(const_cast<char*>(fileHeader.data()), 2);
		if (fs.gcount() == 2) {
			fs.seekg(-2, ios_base::cur);
			if (fileHeader == utf16LeHeader) {
				return FileEncoding_UTF16LE;
			} else {
				// 继续判断是否为UTF-8
			}
		} else {
			return Error_ReadFile; // 读文件出错
		}
	}
	if (size >= 3) {
		string fileHeader(3, '\0');
		fs.read(const_cast<char*>(fileHeader.data()), 3);
		if (fs.gcount() == 3) {
			fs.seekg(-3, ios_base::cur);
			if (fileHeader == utf8Header) {
				return FileEncoding_UTF8;
			} else {
				return FileEncoding_ASCII; // 文件头不是UTF-8，也不是UTF-16LE
			}
		}
		return Error_ReadFile; // 读文件出错
	}
	// 文件大小不足2，认为是ascii
	return FileEncoding_ASCII;
}

int _tmain(int argc, _TCHAR* argv[]) {
	if (argc != 2) {
		return Error_ArgumentCount;	// 参数错误
	}
	fstream file(argv[1], ios_base::in | ios_base::out | ios_base::binary);
	if (file.fail()) {
		return Error_OpenFile; // 文件打开错误
	}
	unsigned size = tellFileSize(file);
	int fileEncoding = recognizeFileEncoding(file);
	if (fileEncoding == FileEncoding_ASCII) {
		string asciiData(size, '\0');
		file.read(const_cast<char*>(asciiData.data()), size);
		if (file.gcount() == size) {
			file.seekg(0);
			wstring utf16Data = mrl::utility::codeconv::asciiToUnicode(asciiData);
			string utf8Data = mrl::utility::codeconv::unicodeToUtf8(utf16Data);
			// UTF8数据流一定比ASCII大，不用考虑文件大小
			file.write(utf8Header.data(), utf8Header.size());
			file.write(utf8Data.data(), utf8Data.size());
			tcout << argv[1] << _T("\t ASCII => UTF8") << endl;
			return 0;
		}
		return Error_ReadFile; // 读文件出错
	}
	else if (fileEncoding == FileEncoding_UTF16LE) {
		// UNDONE: UTF16LE尚未实现转换因为文件有可能变小
		tcout << argv[1] << _T("\t UTF16LE skipped") << endl;
		return 0;
	}
	else if (fileEncoding == FileEncoding_UTF8) {
		return 0;
	}
	return fileEncoding; // 错误码
}