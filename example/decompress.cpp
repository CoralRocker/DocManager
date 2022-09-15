#include <iostream>
#include <stdio.h>
#include "zlib.h"
#include <string>
#include <cstring>
#include <filesystem>
#include <errno.h>

namespace fs = std::filesystem;

using namespace std;

const string fpath = "test.zip";

int main() {
  FILE* input = fopen(fpath.c_str(), "rb");
  FILE* output = fopen("unzipped", "wb");

  z_stream infstream; // inflate zlib stream
  infstream.zalloc = Z_NULL;
  infstream.zfree = Z_NULL;
  infstream.opaque = Z_NULL;
  
  int ret = inflateInit(&infstream);
  switch(ret) {
    case Z_OK:
      cout << "Inflate Init OK" << endl;
      break;
    case Z_MEM_ERROR:
      cerr << "Inflate Init: Mem error." << endl;
      return -1;
    case Z_VERSION_ERROR:
      cerr << "Inflate Init: Version error" << endl;
      return -1;
    case Z_STREAM_ERROR:
      cerr << "Inflate Init: stream error. invalid params" << endl;
      return -1;
  }


  size_t fsize = fs::file_size(fpath);
  cout << "File size: " << fsize << endl;
  uint8_t* f = new uint8_t[fsize];
  size_t nread; // Amount read at last read
  size_t readamt = 1024; // Size of block to read in.
  do {
    nread = fread(f, 1, readamt, input); // Load file into memory;
    if( nread < readamt && !feof(input) ){
      cerr << "Error reading input file!" << endl;
      fclose(input);
      delete[] f;
      fclose(output);
      inflateEnd(&infstream);
      return -1;
      
    } 
  } while( !feof(input) );

  fclose(input);

  infstream.next_in = f;
  infstream.avail_in = fsize;

  // Begin inflate section
  uint8_t* buf = new uint8_t[readamt];
  
  infstream.next_out = buf;
  infstream.avail_out = readamt;

  int infresult;

  do {
    infresult = inflate(&infstream, Z_SYNC_FLUSH); 
    switch(infresult) {
      case Z_OK:
        // all good
        break;
      case Z_NEED_DICT:
        cerr << "Zlib inflate needs a dictionary?" << endl;
        return -1;
      case Z_MEM_ERROR:
        cerr << "Inflate Mem Error" << endl;
        return -1;
      case Z_STREAM_ERROR:
        cerr << "Inflate stream error" << endl;
        return -1;
      case Z_BUF_ERROR:
        cerr << "Inflate buffer error" << endl;
        return -1;
      case Z_DATA_ERROR:
        cerr << "Inflate data error" << endl;
        cerr << infstream.msg;
        return -1;
    }
    
    // Save to file
    if( infstream.avail_out == 0 ){
      fwrite(buf, 1, readamt, output);
      memset(buf, 0, readamt);
      infstream.avail_out = readamt;
      infstream.next_out = buf;
    }

  }while(infresult != Z_STREAM_END);

  if( infstream.avail_out != 0 ){
    fwrite(buf, 1, readamt - infstream.avail_out, output);
    memset(buf, 0, readamt);
    infstream.avail_out = readamt;
    infstream.next_out = buf;
  }


  delete[] buf;
  delete[] f;
  fclose(output);
  inflateEnd(&infstream);

}
