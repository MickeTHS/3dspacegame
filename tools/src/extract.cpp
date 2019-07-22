//
//  extract.cpp
//
//  Created by macbookpro on 2016-03-22.
//  Copyright © 2016 macbookpro. All rights reserved.
//

#include <string>
#include <iostream>
#include <stdio.h>
#include <vector>

using namespace std;

struct MDLTOC {
    char filename[32];
    unsigned int size;
    unsigned int offset;
};

int main (int argc, const char * argv[]) {
    string filename = string(argv[1]);
    //string filename = "/Users/macbookpro/modelling/test.pkg";
    cout << "PKG extractor" << endl;
    
    FILE* f = fopen(filename.c_str(), "rb");
    
    string pkgbuffer;
    string toc;
    
    unsigned int num_files = 0;
    
    fread(&num_files, sizeof(unsigned int), 1, f);
    
    cout << "num files in pkg: " << num_files << endl;
    
    std::vector<MDLTOC*> tocs;
    
    for (int i = 0; i < num_files; ++i) {
        MDLTOC* toc = new MDLTOC;
        fread(toc, sizeof(MDLTOC), 1, f);
        cout << "toc: " << toc->filename << ", size: " << toc->size << ", off: " << toc->offset << endl;
        tocs.push_back(toc);
    }
    
    
    for (auto t : tocs) {
        char* buffer = new char[t->size];
        
        fread(&buffer[0], sizeof(char), t->size, f);
        
        //FILE* fw = fopen(string("/Users/macbookpro/modelling/" + string(t->filename)).c_str(), "w");
        FILE* fw = fopen(string(t->filename).c_str(), "w");
        
        fwrite(&buffer[0], sizeof(char), t->size, fw);
        
        fclose(fw);
        
        delete [] buffer;
    }
    
    
    fclose(f);
    
    return 0;
}

