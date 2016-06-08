/**
 * \file 	util.h
 * 
 * \author 	Jorismar Barbosa Meira <jorismar.barbosa@cc.ci.ufpb.br>
 */

#ifndef JB_UTIL_H
#define JB_UTIL_H

#include <ctime>
#include <string>
#include <iostream>
#include <fstream>

/**
 * \brief This header implement essential functions and types for certain classes.
 * 
 * \headerfile util.h
 */

/**
 * \brief   Macro function used for immediate return if the condition is satisfied, and show a custom mensage.
 * \def     EXIT_IF(cond, msg, ret)
 * \param   cond    Condition
 * \param   msg     Message to be displayed before leaving.
 * \param	ret		Return value.
 */
#define CHECK_ERR(cond, msg, ret) { 					\
	if(cond) {											\
		std::string file(__FILE__);						\
		size_t pos = file.find_last_of("\\/");			\
		std::cout << "[ERROR] " << msg << std::endl;	\
		return ret;										\
	}													\
}

//[" << (std::string::npos == pos ? "" : file.substr(pos + 1, file.length() - 1)) << ":" << __LINE__ << "]

/** Define char in a new type for manipulating bytes. */
typedef char t_byte;

/** Define int in a new type for socket opearations. */
typedef int t_socket;

/**
 * \brief   Get the current date and time in the specific format.
 *
 * \param   format  Format of date and time. (See the strftime function documentation from ctime lib for format symbols table).
 *
 * \return  Return a string of the current date/time.
 */
static std::string getDate(std::string format) {
    time_t rawtime;
    struct tm * timeinfo;
    char buffer [80];
    
    time (&rawtime);
    timeinfo = localtime (&rawtime);
    
    strftime (buffer, 80, format.c_str(), timeinfo);
    
    return std::string(buffer);
}

/**
 * \brief   This method read a file on the computer storage disc.
 *
 * \param   path        Requested file location.
 * \param   buffer    	Buffer to store the bytes.
 *
 * \return  Returns the number of bytes readed.
 */
static int readFile(std::string path, t_byte ** buffer) {
/*	int count;
	char * bin;
	FILE * pfile;
    
	pfile = fopen(path.c_str(), "rb");
	
    if(pfile == NULL) return -1;

    fseek(pfile, 0, SEEK_END);
    long fsize = ftell(pfile);
    rewind(pfile);
    
    *buffer = (t_byte*) malloc (sizeof(t_byte) * fsize);

    count = fread(*buffer, 1, fsize, pfile);

    fclose(pfile);

    return count;	*/
	std::streampos size;
	
	std::ifstream file (path.c_str(), std::ios::in | std::ios::binary | std::ios::ate);
	
	CHECK_ERR(file.fail(), "The requested file " + path + " not found.", -1);
	
	CHECK_ERR(!file.is_open(), "Unable to open the file.", -1);
	
	size = file.tellg();
	*buffer = (t_byte*) malloc (sizeof(t_byte) * size);
	
	file.seekg (0, std::ios::beg);
	file.read (*buffer, size);
	
	file.close();
	
	return size;
}

#endif
