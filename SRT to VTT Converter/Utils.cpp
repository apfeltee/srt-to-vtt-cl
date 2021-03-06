/**
 * @file Utils.cpp
 * Implementation for the Utils (utilities) class.
 *
 * @author Nathan Woltman
 * @copyright 2014-2015 Nathan Woltman
 * @license MIT https://github.com/woollybogger/srt-to-vtt-cl/blob/master/LICENSE.txt
 */

#if defined(_WIN32) || defined(WIN32)
#include <Windows.h>
#else
#include <sys/stat.h>
#endif

#include <codecvt>
#include <dirent.h>
#include "Utils.h"
#include "text_encoding_detect.h"

using namespace std;


void Utils::openFile(const string& filepath, wifstream& stream)
{
	// Read in 2 kb of the file
	const size_t bytes = 2048;
	unsigned char buffer[bytes];
	ifstream fin(filepath);
	fin.read((char*)buffer, bytes);
	fin.close();

	AutoIt::TextEncodingDetect encodingDetector;
	switch (encodingDetector.DetectEncoding(buffer, bytes)) {
		// UTF-16
		case AutoIt::TextEncodingDetect::UTF16_LE_BOM:
		case AutoIt::TextEncodingDetect::UTF16_LE_NOBOM:
		case AutoIt::TextEncodingDetect::UTF16_BE_BOM:
		case AutoIt::TextEncodingDetect::UTF16_BE_NOBOM:
		#if defined(_WIN32) || defined(WIN32) // Windows
			stream.open(filepath, ios::binary);
			stream.imbue(locale(fin.getloc(), new codecvt_utf16<wchar_t, 0x10ffff, consume_header>));
			break;
		#else
			throw runtime_error("Converting UTF-16 encoded files is not supported on your platform.");
		#endif

		// ASCII, ANSI, UTF-8, none (treat as UTF-8)
		default:
			stream.open(filepath);
			stream.imbue(locale(fin.getloc(), new codecvt_utf8<wchar_t, 0x10ffff, consume_header>));
			break;
	}
}

bool Utils::isDir(const string& path)
{
#if defined(_WIN32) || defined(WIN32)
	DWORD ftype = GetFileAttributesA(path.c_str());
	if (ftype == FILE_ATTRIBUTE_DIRECTORY)
		return true;
#else
	struct stat st;
	if (stat(path.c_str(), &st) == 0 && (st.st_mode & S_IFDIR))
		return true;
#endif

	return false;
}

bool Utils::pathExists(const string& path)
{
#if defined(_WIN32) || defined(WIN32)
	DWORD ftype = GetFileAttributesA(path.c_str());
	if (ftype != INVALID_FILE_ATTRIBUTES)
		return true;
#else
	struct stat st;
	if (stat(path.c_str(), &st) == 0 && (st.st_mode & S_IFDIR || st.st_mode & S_IFREG))
		return true;
#endif

	return false;
}

void Utils::rtrim(string& s, const char c)
{
	while (!s.empty() && s.back() == c)
	{
		s.pop_back();
	}
}

void Utils::rtrim(wstring& s, const wchar_t c)
{
	while (!s.empty() && s.back() == c)
	{
		s.pop_back();
	}
}

wstring& Utils::wstr_replace(wstring& subject, wstring search, wstring replace)
{
	for (;;)
	{
		size_t index = subject.find_first_of(search);
		if (index == wstring::npos) break;
		subject.replace(index, search.length(), replace);
	}

	return subject;
}
