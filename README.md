# clocw
Windows command line utility for counting lines of code in C++ projects. 

## Building
Make an empty VisualStudio console application project and add the clocw.cpp file and build the project.


## For better workflow 
After building this file into an exe, store the exe into a folder and add that folder path to Windows environment variables 


## Usage 
In the console go to the source code directory and type clocw and hit enter. 
or  
Make a .bat file with the command "clocw > loc.txt" in the source code directory and execute that .bat file. 
loc.txt will contain the number of lines in the project 

## Advanced usage

- If no argument is given clocw will look for source file extensions with .cpp, .h and .hpp.    
e.g. "count lines in the current folder and subfolders recursively for .h, .hpp and .cpp files"  
**clocw**  
  
- The first argument is optional and can be -nr or -r.  
"-nr" means search only in the current folder.   
"-r" means recursively search current folder and all subfolders.(default case)   
  e.g.  "count lines in the current folder for .h, .hpp and .cpp files"  
  **clocw -nr**  
  "count lines the current folder for .txt files only"  
  **clocw -nr txt**  

- The first and other arguments can be file extensions which have to be considered for counting lines.   
e.g.  
**clocw js**  
**clocw py cpp h**  
  
- If * is added as a file extension in the argument, all filetypes will be considered for counting lines.  
e.g. "count lines in the current folder and subfolders recursively for all types of files"  
**clocw \***  
