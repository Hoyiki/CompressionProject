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


#include<time.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <pwd.h>
#include <grp.h>
#include <iostream>
#include <fcntl.h>
#include <errno.h>
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif /* HAVE_UNISTD_H */
#include <string.h>
#include <string>


typedef struct stat Stat;

static int do_mkdir(const char *path, mode_t mode){
    Stat st;
    int status = 0;
    if (stat(path, &st) != 0){
        /* Directory does not exist. EEXIST for race condition */
        if (mkdir(path, mode) != 0 && errno != EEXIST)
            status = -1;
    }
    else if (!S_ISDIR(st.st_mode)){
        errno = ENOTDIR;
        status = -1;
    }
    return(status);
}

int mkpath(const char *path, mode_t mode){
    char *pp;
    char *sp;
    int status;
    char *copypath = strdup(path);
    
    status = 0;
    pp = copypath;
    while (status == 0 && (sp = strchr(pp, '/')) != 0)
    {
        if (sp != pp)
        {
            /* Neither root nor double slash in path */
            *sp = '\0';
            status = do_mkdir(copypath, mode);
            *sp = '/';
        }
        pp = sp + 1;
    }
    if (status == 0)
        status = do_mkdir(path, mode);
    free(copypath);
    if (status != 0)
        cout<<stderr<<strerror(errno);
    return (status);
}

struct meta_file{
    mode_t mode;
    char path[200];
    char name[200];
    char owner[200];
    char group[200];
    int size;
    int inode;
    long long position;
};

void recreate_file(meta_file meta){
    int filedes;
    if ((filedes = open("second.txt",O_CREAT|O_RDWR)) == -1){perror("creating");exit(1);}
    chmod("second.txt", meta.mode);
}

meta_file meta_from_string(char* text){
    meta_file *meta2;
    meta2 = (meta_file*) text;
    return *meta2;
}

void print_meta(meta_file meta){
    printf("Information for %s/%s\n", meta.path, meta.name);
    printf("---------------------------\n");
    printf("File Owner Name: \t%s \n",meta.owner);
    printf("File Group Name: \t%s \n",meta.group);
    printf("File Size: \t\t%d bytes\n",meta.size);
    printf("File position: \t\t%lld bytes\n",meta.position);
    printf("File inode: \t\t%d\n",meta.inode);
    printf("File Permissions: \t");
    printf( (S_ISDIR(meta.mode)) ? "d" : "-");
    printf( (meta.mode & S_IRUSR) ? "r" : "-");
    printf( (meta.mode & S_IWUSR) ? "w" : "-");
    printf( (meta.mode & S_IXUSR) ? "x" : "-");
    printf( (meta.mode & S_IRGRP) ? "r" : "-");
    printf( (meta.mode & S_IWGRP) ? "w" : "-");
    printf( (meta.mode & S_IXGRP) ? "x" : "-");
    printf( (meta.mode & S_IROTH) ? "r" : "-");
    printf( (meta.mode & S_IWOTH) ? "w" : "-");
    printf( (meta.mode & S_IXOTH) ? "x" : "-");
    printf("\n\n");
    printf("The file %s a symbolic link\n", (S_ISLNK(meta.mode)) ? "is" : "is not");
}

void file_meta_str(char * abs, char *root, char * out, long long position){
    int abs_size = strlen(abs);
    int root_size = strlen(root);
    meta_file new_meta;
    
    //char rel[abs_size - root_size+1];
    int i, k, l = 0;
    for (k = abs_size; k >= 0; k--){
        if (abs[k] == '/')
            break;
    }
    //char name[abs_size - k+1];
    for (i = k+1; i < abs_size; i++){
        new_meta.name[l] = abs[i];
        l++;
    }
    new_meta.name[i+1] = '\0';
    for (i = 0; i < k - root_size; i++){
        new_meta.path[i] = abs[i+root_size];
    }
    new_meta.path[i] = '\0';
    
    
    struct stat fileStat;
    if(stat(abs,&fileStat) < 0)
        throw "Couldn't open file stat";
    new_meta.mode = fileStat.st_mode;
    strcpy(new_meta.owner, getpwuid(fileStat.st_uid)->pw_name);
    strcpy(new_meta.group, getgrgid(fileStat.st_gid)->gr_name);
    new_meta.size = fileStat.st_size;
    new_meta.inode = fileStat.st_ino;
    
    new_meta.position = position;
    
    memcpy(out, &new_meta, sizeof(meta_file));   
}



ofstream outputFile;
char *input;
string all_metadata;



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
            
            char charpointer[sizeof(meta_file)];
            file_meta_str(newname, input, charpointer, position);
            //print_meta(meta_from_string(charpointer));
            
            string a;
            a = string(charpointer);
            
            all_metadata += a;
            
        }

        free ( newname );
        newname = NULL ;
        
    }
    closedir ( dp );
}


int main(int argc, const char * argv[]) {
    if (argc < 2)
        return 0;

    char * archive = argv[2];
    char * input[argc - 2];

    if (!((string)argv[argcount]).compare("-c")){
        //compress
    } else if(!((string)argv[argcount]).compare("-a")){
        //append
    } else if(!((string)argv[argcount]).compare("-x")){
        //extract
    } else if(!((string)argv[argcount]).compare("-m")){
        //print meta
    } else if(!((string)argv[argcount]).compare("-p")){
        //print hierarchy
    }
    int i = 0;
    for (int argcount = 2; argcount < argc; argcount++){
        input[i] = argv[argcount];
    }

    input = "/Users/MotaWolley/Desktop/testingFolder";
    char *output = "/Users/MotaWolley/Desktop/haha";
    
    long long header = 0; //header saves the position of the starting point of metadata
    
    outputFile.open(output, fstream::binary);
    
    outputFile.write(reinterpret_cast<char*>(&header),sizeof(header));
    
    all_metadata = "";
    
    list(input);
    
    header = outputFile.tellp(); //tell position of where the metadata will be
    
    char all_meta_char[all_metadata.length()];
    strcpy(all_meta_char, all_metadata.c_str());
    outputFile.write(all_meta_char, sizeof(all_meta_char));
    
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


















