//
//  main.cpp
//  compression
//
//  Created by 黄梓原 on 19/12/2017.
//  Copyright © 2017 MotaWolley. All rights reserved.
//

#include <iostream>
#include <sstream>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <string.h>

#include <vector>
#include <fstream>

using namespace std;

ofstream outputFile;

void list ( char * name){
    DIR * dp ;
    struct dirent * dir ;
    char * newname ;
    
    if (( dp = opendir ( name )) == NULL ) {
        perror ( "opendir" );
        return ;
    }
    while (( dir = readdir ( dp )) != NULL ) {
        
        if ( dir -> d_ino == 0 or strcmp(dir -> d_name, ".") == 0 or strcmp(dir -> d_name, ".") == 0
            or strcmp(dir -> d_name, "..") == 0  or strcmp(dir -> d_name, ".DS_Store") == 0)
        
            continue ;
        
        newname =( char *) malloc ( strlen ( name )+ strlen ( dir -> d_name ) +2) ;
        strcpy ( newname , name );
        strcat ( newname ,"/");
        strcat ( newname , dir -> d_name );
//        printout ( newname );
        
        cout << newname << endl;
        
        struct stat mybuf;
        
        if ( stat (newname, &mybuf) < 0) {
            perror (newname);
            continue;
        }
        
        if ((mybuf.st_mode & S_IFDIR)){
            list(newname); /* directory encountered */

        }
        
        else if( mybuf.st_mode & S_IFREG )
        {
            //it's a file
            
            //save the metadata
            //save the begining position
            //input file into stream
            
            long long position = outputFile.tellp(); //beginning position of the file
            ifstream inputFile (newname, fstream::binary);
            outputFile << inputFile.rdbuf();
        }

        
        free ( newname );
        newname = NULL ;
        
    }
    closedir ( dp );
}


int main(int argc, const char * argv[]) {
    
    char *input = "/Users/MotaWolley/Desktop/testingFolder";
    char *output = "/Users/MotaWolley/Desktop/haha";
    
    long long header = 0; //header saves the position of the starting point of metadata
    
    //ofstream outputFile (output, fstream::binary);
    
    outputFile.open(output, fstream::binary);
    
    outputFile.write(reinterpret_cast<char*>(&header),sizeof(header));
    
    list(input);
    
    header = outputFile.tellp();
    
    cout << header << endl;
    
    char buffer [sizeof(long long)];
    sprintf(buffer, "%llx", header);
    
    int end = -1;
    for (int i = 0; i < sizeof(buffer); i++){
        if (buffer[i] == '\0'){
            end = i;
            break;
        }
    }
    if (end > 0){
        for (int i = 0; i < end; i++){
            buffer[i+8-end] = buffer[i];
        }
        for (int i = 0; i < 8-end; i++){
            buffer[i] = '0';
        }
    }
    outputFile.seekp(0);
    outputFile.write(buffer,8);
    
    cout << "------------" << endl;
    
//    //extracting 8 chars to a long value
//    unsigned long long value;
//    string b = buffer;
//    istringstream iss(b);
//    iss >> std::hex >> value;
//    std::cout << value << std::endl;
    
    
//    ifstream readingFile (output, fstream::binary);
//    cout << readingFile.rdbuf() << endl;
    
    return 0;
}


















