//
//  osfuncs.h
//  nut_to_mdl
//
//  Created by macbookpro on 2016-03-21.
//  Copyright © 2016 macbookpro. All rights reserved.
//

#ifndef osfuncs_h
#define osfuncs_h

#include <iostream>
#include <vector>
#include <string>

#include <sstream>
#include <algorithm>
#include <iterator>
#include <utility>

#include <fstream>

#include <fcntl.h>

#if defined(__APPLE__) || defined(__FreeBSD__)
#include <copyfile.h>
#else

#ifndef _WIN32
#include <unistd.h>
#include <sys/sendfile.h>
#include <sys/stat.h>
#endif

#endif


#ifdef _WIN32
#include <Windows.h>
#include <io.h>
#else
#include <unistd.h>
#endif


struct OSFuncs {
    static unsigned int get_filesize (const char* filename) {
        FILE* f = fopen(filename, "r");
        
        fseek(f, 0, SEEK_END); // seek to end of file
        long size = ftell(f); // get current file pointer
        fseek(f, 0, SEEK_SET);
        
        return static_cast<unsigned int>(size);
    }
    
    static std::string get_folder_char () {
#ifdef _WIN32
        return "\\";
#else
        return "/";
#endif
    }
    
    static int copy_file (const char* source, const char* destination) {
#ifndef _WIN32
		int input, output;
        if ((input = open(source, O_RDONLY)) == -1) {
            return -1;
        }
        if ((output = open(destination, O_RDWR | O_CREAT, S_IRWXU)) == -1) {
  	    printf("Failed create file %s errno;%d",destination,errno);
            close(input);
            return -1;
        }
#endif
        //Here we use kernel-space copying for performance reasons
#if defined(__APPLE__) || defined(__FreeBSD__)
        //fcopyfile works on FreeBSD and OS X 10.5+
        int result = fcopyfile(input, output, 0, COPYFILE_ALL);
#else
        //sendfile will work with non-socket output (i.e. regular file) on Linux 2.6.33+
        off_t bytesCopied = 0;
        struct stat fileinfo = {0};
#ifdef _WIN32
		
		/*wchar_t w_in[255];
		wchar_t w_out[255];
		mbstowcs(w_in, source, strlen(source) + 1);//Plus null
		mbstowcs(w_out, destination, strlen(destination) + 1);//Plus null
		LPWSTR lp_sourcefile = w_in;
		LPWSTR lp_destfile = w_out;

		if (!CopyFile(lp_sourcefile, lp_destfile, false)) {
			std::cout << "ERROR: unable to copy file " << source << std::endl;
			exit(0);
		}
		*/

		int filesize = OSFuncs::get_filesize(source);
		
		FILE* from = fopen(source, "rb");

		char * buffer = new char[filesize];
		
		fread(&buffer[0], 1, filesize, from);

		fclose(from);

		FILE* to = fopen(destination, "wb");


		fwrite(&buffer[0], 1, filesize, to);

		fclose(to);

		int result = 1;
#else
		fstat(input, &fileinfo);
		int result = sendfile(output, input, &bytesCopied, fileinfo.st_size);
#endif
        
#endif
#ifndef _WIN32
        close(input);
        close(output);
#endif        
        return result;
    }
};



#endif /* osfuncs_h */
