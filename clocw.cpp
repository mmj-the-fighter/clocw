/*Attributions:
https://stackoverflow.com/users/13566062/song-zhu has written major parts of ProcessFiles function
Stackoverflow Post : https://stackoverflow.com/questions/63386581/how-can-you-iterate-though-directories-without-using-any-recursive-functions-in
*/

/*
Program for counting lines of code in C++ projects.
It can be modified to work for any type of project.
Just populate the std::unordered_set<std::wstring> sourcefile_extensions with the desired extensions.
*/

/*
Its a command line utility. For smooth workflow follow these steps.
After building this file into an exe, store the exe into a folder and add that folder path to 
Windows environment variables
How to do that: https://learn.microsoft.com/en-us/previous-versions/office/developer/sharepoint-2010/ee537574(v=office.14)
*/

/*
Usage: go to the directory you want to count lines of code recursively. type clocw and hit enter.
or 
Make a .bat file with the command "clocw > log.txt" in the source code directory and execute that .bat file.
log.txt will contain the number of lines in the project
*/



#include <Windows.h>
#include <tchar.h>
#include <iostream>
#include <string>
#include <queue>
#include <vector>
#include <unordered_set>
#include <chrono>


class QuickProfiler
{
public:
	std::chrono::system_clock::time_point t0;
	long long millis;
	double seconds;
	long long diff;

	inline void Start() 
	{
		t0 = std::chrono::high_resolution_clock::now();
	}

	inline void Stop() 
	{
		using namespace std::chrono;
		system_clock::time_point t1 = high_resolution_clock::now();
		millis = duration_cast<milliseconds>(t1 - t0).count();
		diff = (t1 - t0).count();
		seconds = (double)millis / 1000.0;
	}
};

int ReadExtensionFromPath(TCHAR* path, TCHAR* extension)
{
	TCHAR c;
	TCHAR ext[MAX_PATH];
	int i = 0;
	int j = 0;

	//find the length
	while (*(path + i) != '\0') {
		++i;
	}

	if (i == 0) {
		return -1;
	}
		

	//read from last char till a dot
	--i;
	while (i >= 0) {
		c = *(path + i);
		if (c != '.') {
			ext[j++] = c;
		}
		else{
			break;
		}
		--i;
	}
	ext[j] = '\0';

	// reverse buffer
	i = 0;
	j = j - 1;
	while (i < j)
	{
		c = ext[i];
		ext[i] = ext[j];
		ext[j] = c;
		++i;
		--j;
	}

	//copy ext
	i = 0;
	while ((c = *(ext + i)) != '\0')
	{
		*(extension + i) = c;
		++i;
	}
	*(extension + i) = '\0';

	return 0;
}

bool IsDesiredFileType(TCHAR* filename, std::unordered_set<std::wstring> sourcefile_extensions)
{
	TCHAR extension[MAX_PATH];
	ReadExtensionFromPath(filename, extension);
	std::wstring ext(extension);

	if (sourcefile_extensions.find(ext) != sourcefile_extensions.end())
		return true;
	else
		return false;
}

unsigned int CountLines(const wchar_t *filename)
{
	unsigned int lc = 0;
	int ch;
	FILE* f;
	_wfopen_s(&f, filename, L"r");
	while ((ch = fgetc(f)) != EOF)
		if (ch == '\n')
			++lc;
	fclose(f);
	return lc;
}

unsigned int ProcessFiles(std::wstring path, std::unordered_set<std::wstring> sourcefile_extensions)
{
	unsigned int linecount = 0;
	std::queue<std::wstring> qFolders;
	qFolders.push(path);

	WIN32_FIND_DATA findResult;
	HANDLE handle = NULL;

	while (qFolders.size() > 0)
	{
		std::wstring temp = qFolders.front();
		temp.append(_T("\\*"));
		handle = FindFirstFile(temp.c_str(), &findResult);
		do
		{
			if (findResult.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			{
				if (lstrcmp(_T("."), findResult.cFileName) == 0 || lstrcmp(_T(".."), findResult.cFileName) == 0)
				{
					continue;
				}
				temp = qFolders.front();
				temp.append(_T("\\")).append(findResult.cFileName);
				qFolders.push(temp);
			}
			else {
				//wcout << findResult.cFileName << endl;
				if (IsDesiredFileType(findResult.cFileName, sourcefile_extensions))
				{
					temp = qFolders.front();
					temp.append(_T("\\")).append(findResult.cFileName);
					//wcout << temp << endl;
					linecount += CountLines(temp.c_str());
					//cout << linecount << endl;
				}

			}
		} while (FindNextFile(handle, &findResult));
		qFolders.pop();
	}
	if (handle)
	{
		FindClose(handle);
		handle = NULL;
	}
	return linecount;
}

int main()
{
	QuickProfiler profiler;
	TCHAR currentDirectory[MAX_PATH];
	DWORD result;
	profiler.Start();
	result = GetCurrentDirectory(MAX_PATH, currentDirectory);
	if (result == 0)
	{
		return 1;
	}

	std::unordered_set<std::wstring> sourcefile_extensions = { 
		_T("c"), 
		_T("cpp"), 
		_T("cxx"), 
		_T("cc"), 
		_T("hpp"), 
		_T("h") };
	///ProcessFiles(_T("D:\\sample"));
	unsigned int linecount = ProcessFiles(currentDirectory,sourcefile_extensions);
	profiler.Stop();
	std::cout << "Lines Of C++ Code: " << linecount << std::endl;
	std::cout << "Processing time: " << profiler.seconds << " Seconds"
		<< "(" << profiler.millis << " Milliseconds)" << std::endl;
	return 0;
}

