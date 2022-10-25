# clocw
Windows command line utility for counting lines of code in C++ projects. 

## Building
Make an empty VisualStudio console application project and add the clocw.cpp file and build the project.

## Modifying 
It can be modified to work for any type of project.  
Just populate the std::unordered_set<std::wstring> sourcefile_extensions with the desired extensions. 

## For better workflow 
After building this file into an exe, store the exe into a folder and add that folder path to Windows environment variables 


## Usage 
In the console go to the source code directory and type clocw and hit enter. 
or  
Make a .bat file with the command "clocw > loc.txt" in the source code directory and execute that .bat file. 
loc.txt will contain the number of lines in the project 
