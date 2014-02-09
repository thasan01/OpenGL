#pragma once
#include <stdio.h>
#include <stdarg.h>	
#include "../TypeDef.h"


class FileLogger
{
	public:
	FileLogger();
	~FileLogger();
	bool open(const char *filename);
	void close();
	void print(const char * message,...); 

	private:
	FILE *fp; //Log File Pointer
};
