/*
txt2nsb.cpp 2011/07/19
by 7k
email: blxode [at] gmail.com 
merge the *.nsb files and the translated subtitles which were extracted by nsb2txt.exe 
(undone)and modify the pointers in the corresponding *.map files

for STEINS;GATE PC
*/

#include <iostream>
#include <fstream>
#include <cmath>
#include <cstdlib>
#include <string>
#include <io.h>

using namespace std;


int main() {
	cerr<<"txt2nsb by 7k"<<endl;
	system("dir /B sg*.nsb >flist.txt");

	ifstream flist("flist.txt");
	string nsbfn;
	
	while(flist>>nsbfn){
		string txtfn = nsbfn+".txt";
		/* Converting the files to Unix format */
		string cmd = "dos2unix.exe -q " + txtfn;
		system(cmd.c_str());
		
		fstream nsb,txt;
		nsb.open(nsbfn.c_str(),ios::in | ios::binary);
		if(!nsb.is_open()){
			cerr<<"Could not open "<<nsbfn<<" (No such file or directory)"<<endl;
			return 1;
		}
		txt.open(txtfn.c_str(),ios::in | ios::binary);
		if(!txt.is_open()){
			cerr<<"Could not open "<<txtfn<<" (No such file or directory)"<<endl;
			return 1;
		}
		/* Copy the *.nsb files to memory */
		nsb.seekg(0,ios::end);
		unsigned long len1 = nsb.tellg();
		nsb.seekg(0,ios::beg);
		unsigned char* buff1 = new unsigned char[len1];
		nsb.read((char *)buff1,len1);
		
		/* Copy the *.txt files to memory */
		txt.seekg(0,ios::end);
		unsigned long len2 = txt.tellg();
		txt.seekg(0,ios::beg);
		unsigned char* buff2 = new unsigned char[len2];
		txt.read((char *)buff2,len2);
		
		/* file exist,terminate */
		string outfn = "new_" + nsbfn;
		if(access(outfn.c_str(),0) != -1) { 
			cout<<"file "<<outfn<<" exist"<<endl;
			remove("flist.txt");
			return 1;
		}
		ofstream outfile(outfn.c_str(),ios::binary | ios::app);
		
		unsigned long i, m=0, t=0, oldboxlen=0;
		
		struct NSB{
			unsigned long boxlen;
		} nsb0;
		nsb0.boxlen=0;

		/* Merge files */
		cerr<<"txt2nsb: merging "<<nsbfn<<" and "<<nsbfn<<" ..."<<endl;
		for (i = 0; i < len1; i++){
			if(buff1[i] == '<' && buff1[i+1] == 'P'){
				for(unsigned long j=t+oldboxlen;j<i-4;j++){
					outfile<<buff1[j];
				}
				oldboxlen = buff1[i-1]*pow(16,6)+buff1[i-2]*pow(16,4)+buff1[i-3]*pow(16,2)+buff1[i-4];

				int flag=0;
				for (; m < len2; m++){
					if(flag==1) break;
					if(buff2[m] == '<' && buff2[m+1] == 'P'){
						flag=1;
						nsb0.boxlen = 0;
						for (unsigned long n = m; n < len2; n++){
							nsb0.boxlen++;
							if(buff2[n] == '>' && buff2[n-1] == 'E') break;
						}
						outfile.write((const char*)&nsb0,4);
						for (unsigned long n = m; n < len2; n++){
							outfile<<buff2[n];
							if(buff2[n] == '>' && buff2[n-1] == 'E') break;
						}
						m+=nsb0.boxlen;
					}
				}	
				t=i;
				i+=oldboxlen;
			}
		}
		for(unsigned long j=t+oldboxlen;j<i;j++){
			outfile<<buff1[j];
		}
		nsb.close();
		txt.close();
		delete []buff1;
		delete []buff2;	
	}
	flist.close();
	remove("flist.txt");
	
	return 0;
}
