#include "FileLogger.h"

FileLogger::FileLogger():fp(NULL)
{
}

FileLogger::~FileLogger()
{
	close();
}

#pragma warning (disable: 4996)
bool FileLogger::open(const char *filename)
{
	fp =  fopen(filename, "w"); 
	return (fp!=NULL);
}

void FileLogger::print(const char * message,...)
{
	if(fp==NULL)
		return;

	char Buffer[2048];  //Log File Buffer    
	va_list args; //args list 
	va_start(args, message); 
	vsprintf(Buffer, message, args); //copy it to buffer 
	va_end(args); 
	fprintf(fp, "%s", Buffer); // print it 
}

void FileLogger::close()
{
	if(fp!=NULL)
		fclose(fp);
}

