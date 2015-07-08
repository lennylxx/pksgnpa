/*
  pksgnpa.cpp 2011/06/16
  by 7k
  email: blxode [at] gmail.com
  pack the *.npa used by STEINS;GATE PC
*/

#include <iostream>
#include <fstream>
#include <cstdlib>
#include <string>
#include <cstring>
#include <io.h>
#include <sys/stat.h>
#include "dirent.h"

using namespace std;

struct NPAHDR1 {
  unsigned long toc_len;
} hdr1;

struct NPAHDR2 {
  unsigned long entry_count;
} *hdr2;

struct NPAENTRY1 {
  unsigned long filename_length;
} *entry1;

struct NPAENTRY2 {
  unsigned long length;
  unsigned long offset;
  unsigned long null;
} *entry2;

void obfuscate(unsigned char* buff, unsigned long len) {//encrypt function from exsgnpa.cpp
	unsigned char KEY[] = { 0xBD, 0xAA, 0xBC, 0xB4, 0xAB, 0xB6, 0xBC, 0xB4 };
	for (unsigned long i = 0; i < len; i++) {
		buff[i] ^= KEY[i % sizeof(KEY)];
	}
}


void list_file(string path){//make the file name list in ASCII order
	ofstream flistout("flist.tmp", ios::app);
	struct dirent **namelist;
	struct stat statbuf;
	string curr = path;
	int i,total;
	total = scandir(curr.c_str(),&namelist,0,alphasort);
	if(total<0)  return; 
	else{
		for(i=0;i<total;i++){
			if(strcmp(".",namelist[i]->d_name) == 0 || strcmp("..",namelist[i]->d_name) == 0) continue;
			curr=path+'/'+namelist[i]->d_name;
			stat(curr.c_str(),&statbuf);
 			if(S_ISDIR(statbuf.st_mode))
			{
				list_file(curr);
 		 	}
			else
			{
				flistout<<path.substr(0,string::npos)<<'/';
				flistout<<namelist[i]->d_name<<endl;
			}
		}
	}
	//flistout.close();
}

void init1()
{
	ifstream flist("flist.tmp");
	string filename;
	unsigned long total_filename_length = 0;
	hdr2->entry_count = 0;
	while(flist>>filename){
		total_filename_length += filename.length();
		hdr2->entry_count++;
	}
	hdr1.toc_len = 4 + hdr2->entry_count * 16 + total_filename_length;
	flist.close();
}

void init2(){
	ifstream flist("flist.tmp");
	string filename;
	unsigned long i,temp = 0;
	for(i = 0; i < hdr2->entry_count; i++)
	{
		flist>>filename;
		entry1[i].filename_length =  filename.length();
		struct _stat buf;
		_stat(filename.c_str(), &buf);
		entry2[i].length = buf.st_size;
		entry2[i].offset = 4 + temp + hdr1.toc_len;
		entry2[i].null = 0x0;
		temp += entry2[i].length;
	}
	flist.close();
}

void wrdata(string path){
	ofstream outfile(path.c_str() , ios::binary | ios::app);
	ifstream flist("flist.tmp");
	unsigned long i;
	string filename;
	for(i = 0; i < hdr2->entry_count; i++)
	{
		flist>>filename;
		fstream file;
		const char *fn = filename.c_str();
		file.open(fn, ios::in | ios::binary);
		if(!file.is_open()){
			cerr<<"Could not open "<<fn<<" (No such file or directory)"<<endl;
			exit(1);
		}
		struct _stat buf;
		_stat(filename.c_str(), &buf);
		unsigned long len_i = buf.st_size;
		unsigned char* buff = new unsigned char[len_i];
		file.read((char *)buff,len_i);
		obfuscate(buff, len_i);
		outfile.write((const char *)buff,len_i);
		file.close();
		delete [] buff;
	}
	flist.close();
	outfile.close();
}

/*             >>d(' _ ')b<<             */
int main(int argc, char** argv) {
   	if (argc != 2) {
		cerr<<"pksgnpa by 7k\n\n";
		cerr<<"usage: "<<argv[0]<<" [directory]\n\n";
		return -1;
	}
	string path(argv[1]);
	/* Make the file list */
	list_file(path);

	/* File exist, terminate */
	path += ".npa";
	if(access(path.c_str(),0) != -1) { 
		cout<<"file "<<path<<" exist"<<endl;
		remove("flist.tmp");
		return 1;
	}
	
	/* Initialize */
	hdr2= (NPAHDR2 *)new unsigned char[sizeof(*hdr2)];
	init1();
	entry1 = (NPAENTRY1 *)new unsigned char[hdr2->entry_count * sizeof(*entry1)];
	entry2 = (NPAENTRY2 *)new unsigned char[hdr2->entry_count * sizeof(*entry2)];
	init2();
	
	/* Build the output *npa file */
	ofstream outfile(path.c_str() , ios::binary | ios::app);
	
	/* Write the header */
	outfile.write((const char*)&hdr1,4);

	/* Write the file table */
	unsigned long i;
	unsigned char* table_buff = new unsigned char[hdr1.toc_len];//prepare for encryption
	unsigned char *p = table_buff;
	memcpy(p, hdr2, 4);
 	p += sizeof(*hdr2);
	ifstream flist("flist.tmp");
	string filename;
  	for (i = 0;i < hdr2->entry_count;i++)
	{//copy the data
		flist>>filename;
		memcpy(p, entry1, sizeof(*entry1));
		p += sizeof(*entry1);
		entry1++;
		memcpy(p, (unsigned char *)filename.c_str(),filename.length());
		p += filename.length();
		memcpy(p, entry2, sizeof(*entry2));
		p += sizeof(*entry2);
		entry2++;
	}
	obfuscate(table_buff, hdr1.toc_len);
	outfile.write((const char*)table_buff,hdr1.toc_len);
	delete [] table_buff;
	flist.close();
	outfile.close();
	
	/* Write the file data  */
	wrdata(path);
	remove("flist.tmp");
	return 0;
}

