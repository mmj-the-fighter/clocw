/*
Attribution:
https://stackoverflow.com/users/13566062/song-zhu has written major parts of ProcessFiles function.
Stackoverflow Post : https://stackoverflow.com/questions/63386581/how-can-you-iterate-though-directories-without-using-any-recursive-functions-in
*/
#include <Windows.h>
#include <tchar.h>
#include <iostream>
#include <string>
#include <queue>
#include <vector>
#include <unordered_set>
#include <chrono>
#include <stdio.h>



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

//For analyzing C, C++, Java, C# code comments and source poritions
void CountLines(const wchar_t *filename, unsigned int* pploc, unsigned int *psloc)
{
	unsigned int loc = 0;
	unsigned int sloc = 0;
	int ch, prevChar = 0;
	int foundCode = 0;
	int foundSLComment = 0;
	int foundMLComment = 0;
	int fwslashcount = 0;

#define BUFFERSIZE_CLOCW 4096
	static char buffer[BUFFERSIZE_CLOCW];
	FILE* f;
	_wfopen_s(&f, filename, L"r");
	if (f == NULL) {
		std::wcout << "Cannot Open : " << filename << std::endl;
		return;
	}
		
	if (setvbuf(f, buffer, _IOFBF, BUFFERSIZE_CLOCW) != 0)
	{
		std::cout << "setvbuf failed" << std::endl;
		exit(EXIT_FAILURE);
	}

	while ((ch = fgetc(f)) != EOF)
	{
		//std::cout << (char)ch;
		if (foundSLComment == 0 && foundMLComment == 0 && foundCode == 0){
			foundCode = isalnum(ch);
		}
		if (foundMLComment == 0){
			if (ch == '/')
			{
				if (prevChar == '/'){
					foundSLComment = 1;
				}
			}
			else if (ch == '*'){
				if (prevChar == '/' && foundSLComment == 0){
					foundMLComment = 1;
				}
			}
		}
		else {
			if (ch == '/' && prevChar == '*'){
				foundMLComment = 0;
			}
		}

		if (ch == '\n'){
			++loc;
			if (foundCode) {
				++sloc;
			}
			foundCode = 0;
			foundSLComment = 0;
		}
		prevChar = ch;
	}
	++loc;
	if (foundCode) {
		++sloc;
	}
	fclose(f);
	*pploc += loc;
	*psloc += sloc;
#undef BUFFERSIZE_CLOCW
}

//void CountLinesSkelton(const wchar_t *filename, unsigned int* pploc, unsigned int *psloc)
//{
//	unsigned int loc = 0;
//	unsigned int sloc = 0;
//	int ch;
//	int foundCode = 0;
//
//	FILE* f;
//	_wfopen_s(&f, filename, L"r");
//	if (f == NULL) {
//		std::wcout << "Cannot Open : " << filename << std::endl;
//		return;
//	}
//
//	while ((ch = fgetc(f)) != EOF)
//	{
//		if (foundCode == 0){
//			foundCode = isalnum(ch);
//		}
//		if (ch == '\n'){
//			++loc;
//			if (foundCode) {
//				++sloc;
//			}
//			foundCode = 0;
//		}
//	}
//	++loc;
//	if (foundCode) {
//		++sloc;
//	}
//	fclose(f);
//	*pploc = loc;
//	*psloc = sloc;
//}

void ProcessFiles(std::wstring sourcepath, bool isRecursive, std::unordered_set<std::wstring> sourcefile_extensions, unsigned int* pphysicalLinecount, unsigned int *psourceLinecount)
{
	bool isAllFilesToBeProcessed = false;
	std::wstring allfilesext = L"*";
	if (sourcefile_extensions.find(allfilesext) == sourcefile_extensions.end())
		isAllFilesToBeProcessed = false;
	else
		isAllFilesToBeProcessed = true;

	std::queue<std::wstring> qFolders;
	qFolders.push(sourcepath);

	WIN32_FIND_DATA findResult;
	HANDLE handle = NULL;

	while (qFolders.size() > 0)
	{
		std::wstring path = qFolders.front();
		path.append(_T("\\*"));
		handle = FindFirstFile(path.c_str(), &findResult);
		do
		{
			if (findResult.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			{
				if (lstrcmp(_T("."), findResult.cFileName) == 0 || lstrcmp(_T(".."), findResult.cFileName) == 0)
				{
					continue;
				}
				if (isRecursive)
				{
					path = qFolders.front();
					path.append(_T("\\")).append(findResult.cFileName);
					qFolders.push(path);
				}
			}
			else {
				//wcout << findResult.cFileName << endl;
				if (isAllFilesToBeProcessed || 
					IsDesiredFileType(findResult.cFileName, sourcefile_extensions))
				{
					path = qFolders.front();
					path.append(_T("\\")).append(findResult.cFileName);
					//wcout << path << endl;
					CountLines(path.c_str(), pphysicalLinecount, psourceLinecount);
					//CountLinesSkelton(path.c_str(), pphysicalLinecount, psourceLinecount);
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
}

int _tmain(int argc, _TCHAR* argv[])
{
	unsigned int physicalLinecount = 0;
	unsigned int sourceLinecount = 0;
	QuickProfiler profiler;
	std::unordered_set<std::wstring> sourcefile_extensions;
	TCHAR currentDirectory[MAX_PATH];
	DWORD result;
	profiler.Start();
	result = GetCurrentDirectory(MAX_PATH, currentDirectory);
	if (result == 0)
	{
		return 1;
	}

	bool isRecursive = true;
	std::wstring nonRecursiveOption = L"-nr";
	std::wstring recursiveOption = L"-r";
	if (argc == 1)
	{
		sourcefile_extensions.insert(_T("cpp"));
		sourcefile_extensions.insert(_T("h"));
		sourcefile_extensions.insert(_T("hpp"));
	}
	else if (argc == 2)
	{
		std::wstring option = std::wstring(argv[1]);
		if (option == nonRecursiveOption){
			isRecursive = false;
			sourcefile_extensions.insert(_T("cpp"));
			sourcefile_extensions.insert(_T("h"));
			sourcefile_extensions.insert(_T("hpp"));
		}
		else if (option == recursiveOption){
			isRecursive = true;
			sourcefile_extensions.insert(_T("cpp"));
			sourcefile_extensions.insert(_T("h"));
			sourcefile_extensions.insert(_T("hpp"));
		}
		else{
			sourcefile_extensions.insert(option);
		}
	}
	else
	{
		std::wstring option = std::wstring(argv[1]);
		if (option == nonRecursiveOption){
			isRecursive = false;
		}
		else if (option == recursiveOption){
			isRecursive = true;
		}
		else{
			sourcefile_extensions.insert(option);
		}

		for (int i = 2; i < argc; i++)
		{
			std::wstring option = std::wstring(argv[i]);
			sourcefile_extensions.insert(option);
		}
	}
	 
	//ProcessFiles(_T("D:\\sample1"), false, sourcefile_extensions, &physicalLinecount, &sourceLinecount);
	ProcessFiles(currentDirectory, isRecursive, sourcefile_extensions, &physicalLinecount, &sourceLinecount);
	profiler.Stop();
	std::cout << "Physical Lines Of Code (loc): " << physicalLinecount << std::endl;
	std::cout << "Logical Lines Of Code (sloc): " << sourceLinecount << std::endl;
	std::cout << "Processing time: " << profiler.seconds << " Seconds"
		<< "(" << profiler.millis << " Milliseconds)" << std::endl;
	return 0;
}
