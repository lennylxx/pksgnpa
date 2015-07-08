/*
  nsb2txt.cpp 2011/06/12
  by 7k
  email: blxode [at] gmail.com 
  extract the subtitle from *.nsb used by STEINS;GATE PC
*/

#include <iostream>
#include <fstream>
#include <cstdlib>
#include <string>

using namespace std;

int main() {
	cerr<<"nsb2txt by 7k"<<endl;
	system("dir /B sg*.nsb >filename.txt");
	
	ifstream flist("filename.txt");
	string filename;
	while(flist>>filename){
		string ofn=filename+".txt";
		
		ofstream fout(ofn.c_str());
		const char *fn = filename.c_str();
		fstream file;
		file.open(fn,ios::in);
		if(!file.is_open()){
			cerr<<"Could not open "<<fn<<" (No such file or directory)"<<endl;
			return 1;
		}
		file.seekg(0,ios::end);
		unsigned long len = file.tellg();
		file.seekg(0,ios::beg);
		unsigned char* buff = new unsigned char[len];
		file.read((char *)buff,len);
		
		for (unsigned long i = 0; i < len; i++){
			if(buff[i] == '<' && buff[i+1] == 'P'){
				for (unsigned long j = i; j < len; j++){
					fout<<buff[j];
					if(buff[j] == '>' && buff[j-1] == 'E') break;
				}
				fout<<endl<<endl;
			}
		}
		file.close();
	}
	flist.close();
	system("del filename.txt");
	return 0;
}