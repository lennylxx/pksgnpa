/*
  as-util.h 2011/06/12
  by 7k
  email: blxode [at] gmail.com 
*/


#ifndef _AS_UTIL_H_
#define _AS_UTIL_H_

#include <iostream>
#include <fstream>
#include <cstdlib>
#include <string>
#include <fcntl.h>

using namespace std;

namespace as{
	int open_or_die(string in_filename, const int type){
		const char *fn = in_filename.c_str();
		int fd = open(fn, type);
		if(fd == -1){
			cerr<<"Could not open "<<fn<<" (No such file or directory)"<<endl;
			exit(1);
		}
		return fd;
	}
	
	
 	bool mkdirEx(const char* lpPath)
	{
		string pathname = lpPath;
		if(pathname[pathname.length()-1] != '/')	pathname += '/' ;
		int end = pathname.rfind('/');
		int pt = pathname.find('/');
		if (pathname[pt-1] == ':')	pt = pathname.find('/', pt+1);
		const char * path;
		while(pt != -1 && pt<=end)
		{
			path = pathname.substr(0,pt+1).c_str();
			if(access(path, 0) == -1)
				mkdir(path);
			pt = pathname.find('/', pt+1);
		}
		return true;
	}

	void make_path(string filename){
		unsigned long i;
		for(i = filename.length(); i >0 ; i--){
			if(filename[i] == '/')	break;
		}
		string path=filename.substr(0,i);
		mkdirEx(path.c_str());
	}
	
	void write_file(string filename, unsigned char *buff, unsigned long len){
		const char *fn = filename.c_str();
		fstream file;
		file.open(fn,ios::binary | ios::out);
		if(!file.is_open()){
			cerr<<"Could not open "<<fn<<" (Permission denied)"<<endl;
			exit(1);
		}
		file.write((const char *)buff,len);
		file.close();
	}
};

#endif