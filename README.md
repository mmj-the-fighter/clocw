# clocw
Windows command line utility for counting lines of code in C++ projects. 

## Modifying 
It can be modified to work for any type of project. 
Just populate the std::unordered_set<std::wstring> sourcefile_extensions with the desired extensions. 

## For better workflow 
After building this file into an exe, store the exe into a folder and add that folder path to  
Windows environment variables 


## Usage 
In the console go to the source code directory and type clocw and hit enter. 
or  
Make a .bat file with the command "clocw > log.txt" in the source code directory and execute that .bat file. 
log.txt will contain the number of lines in the project 
