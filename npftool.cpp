/*
  npftool.cpp 2011/08/11
  by 7k
  email: blxode [at] gmail.com
  decrypt and encrypt the *.npf files used by STEINS;GATE PC
  dir: C:\Users\[User Name]\AppData\Roaming\Nitroplus\STEINS;GATE\1.200(1.100)\
*/

#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>
#include <io.h>
#include <sys/stat.h>

using namespace std;

void obfuscate(unsigned char* buff, unsigned long len) {//encrypt function from exsgnpa.cpp
	unsigned char KEY[] = "BUCKTICK";
	int keylen = 8;
	for (unsigned long i = 0; i < len; i++) {
		buff[i] ^= ~KEY[i % keylen];
	}
}

int main(int argc, char** argv) {
   	if (argc != 3) {
		cerr<<"npftool by 7k\n\n";
		cerr<<"[usage]\ndecrypt: "<<argv[0]<<" example.npf example.txt\nencrypt: "<<argv[0]<<" example.txt example.npf\n";
		return -1;
	}
	string path(argv[1]);
	string ofn (argv[2]);
    ifstream infile(path.c_str());
	ofstream outfile(ofn.c_str());
	struct _stat buf;
	_stat(path.c_str(), &buf);
	unsigned char* table_buff = new unsigned char[buf.st_size];
	infile.read((char*)table_buff,buf.st_size);
	obfuscate(table_buff,buf.st_size);
	outfile.write((const char*)table_buff,buf.st_size);
    return 0;
}
