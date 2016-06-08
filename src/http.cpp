#include "http.h"

/******************************************************************************************/

Http::Http() {
    this->server_name       = "Jorismar";
}

/******************************************************************************************/

Http::~Http() {
    if(this->buffer != NULL)
        delete[] buffer;
}

/******************************************************************************************/

std::string Http::getReqstedFile() {
    return this->reqst_file;
}

/******************************************************************************************/

void Http::setServerName(std::string name) {
    this->server_name = name;
}

/******************************************************************************************/

size_t Http::getBinarySize() {
    return this->buffer_size;
}

/******************************************************************************************/

t_byte * Http::getBinaryPacket() {
    return this->buffer;
}

/******************************************************************************************/

std::string Http::getHeader() {
    return this->header;
}

/******************************************************************************************/
// Private Method
std::string Http::getfield(std::string src, std::string mark, char end) {
    int src_size = src.length(), mark_start_pos = src.find(mark);
    std::string value = "";
    
    if(mark_start_pos != std::string::npos) {
        for(int pos = mark_start_pos + mark.length(); pos < src_size && src.at(pos) != end; pos++) {
            value += src.at(pos);
        }
    }
    
    return value;    
}

/******************************************************************************************/

void Http::processRequest(t_byte * header) {
    // Convert bytes to string
    std::string msg(header);
    
    // Check if is a GET Request
    if(!(msg.find("GET") != std::string::npos))
        return;
    
    // Retrieve the request filename
    this->reqst_file    = this->getfield(msg, "GET ", ' ');
    
    // Retrieve the request file type
    if(!this->reqst_file.compare("/")) {
        this->reqst_file = "/index.html";
        this->reqst_filetype = "text/html; charset=UTF-8";
    } else {
        std::string filetype = this->reqst_file.substr(this->reqst_file.rfind(".", this->reqst_file.length() - 1) + 1, this->reqst_file.length() - 1);

        if(!filetype.compare("html") || !filetype.compare("htm"))
            this->reqst_filetype = "text/html; charset=UTF-8";
        else if(!filetype.compare("jpg"))
            this->reqst_filetype = "image/jpg";
        else if(!filetype.compare("png"))
            this->reqst_filetype = "image/png";
        else if(!filetype.compare("gif"))
            this->reqst_filetype = "image/gif";
        else if(!filetype.compare("ico"))
            this->reqst_filetype = "image/x-icon";
        else if(!filetype.compare("mp3"))
            this->reqst_filetype = "audio/mpeg";
        else if(!filetype.compare("wav"))
            this->reqst_filetype = "audio/wav";
        else if(!filetype.compare("mp4"))
            this->reqst_filetype = "video/mp4";
        else if(!filetype.compare("webm"))
            this->reqst_filetype = "video/webm";
        else if(!filetype.compare("ogg"))
            this->reqst_filetype = "video/ogg";
        else if(!filetype.compare("json"))
            this->reqst_filetype = "application/json; charset=UTF-8";
        else if(!filetype.compare("js"))
            this->reqst_filetype = "application/javascript; charset=UTF-8";
        else if(!filetype.compare("css"))
            this->reqst_filetype = "text/css";
        else
            this->reqst_filetype = "application/octet-stream";
    }

    // Gets the byte range
    this->begin_range = -1;
    this->end_range = -1;

    std::string aux = "";
    std::string::size_type sz;

    aux = this->getfield(msg, "Range: bytes=", '-');

    if(aux.length() > 0) {
        try {
            this->begin_range = std::stoi(aux, &sz);
        } catch(...) {
            this->begin_range = -1;
        }
    }
}

/******************************************************************************************/

std::string Http::createResponseHeader(size_t filelen, int status_code) {
    std::string status, connection;
    std::string filesize = std::to_string(filelen);
    std::string content_length   = filesize;
	std::string content_range = "";
    std::string custom_data = "";
	std::string accepted_ranges = "";
	std::string cache_control = "";
	std::string http_version = "HTTP/1.0";
	std::string EOL = "\r\n";
	std::string options = "";
	
    options += "Access-Control-Allow-Origin: *" + EOL;
	options	+= "Access-Control-Allow-Headers: X-Requested-With, Content-Type, X-Codingpedia, X-HTTP-Method-Override" + EOL;

	// Status code = 206
	if (status_code == Http::Status::OK && this->begin_range >= 0) {
		status_code 	= Http::Status::PARTIAL_CONTENT;
		status 			= "206 Partial Content";
		content_length 	= std::to_string(filelen - this->begin_range);
		content_range	= std::to_string(this->begin_range) + "-" + std::to_string(filelen - 1) + "/" + filesize;
		connection 		= "keep-alive";
	}
	
	if(this->begin_range < 0)
		this->begin_range = 0;
	
	// Status code = 200
	if (status_code == Http::Status::OK) {
		status 		= "200 OK";
		connection 	= "keep-alive";
	}
	
	// Status code = 304
	else if(status_code == Http::Status::NOT_MODIFIED) {
        status 		= "304 Not Modified";
        connection 	= "keep-alive";
        // implement time check
	}
	
	// Status code = 404
	else if(status_code == Http::Status::NOT_FOUND) {
		status 				 = "404 Not Found";
		options    			+= "x-content-type-options: nosniff" + EOL;
		connection 			 = "close";
		this->reqst_filetype = "text/html; charset=UTF-8";
		
		// Custom not found page.
		custom_data = "																					\
			<html>																						\
				<head>																					\
					<title>404 Not Found</title>														\
				</head>																					\
				<body>																					\
					<h1>Not Found</h1>																	\
					<p>The requested URL " + this->reqst_file + " was not found on this server.</p>		\
					<hr>																				\
					<address>Projeto de Redes de Computadores I - Servidor HTTP Multithread</address>	\
				</body>																					\
			</html>																						\
		";
		
		content_length 		 = custom_data.length();
	}
	
	
	this->header += http_version + " " + status + EOL;
	
	if(status_code != Http::Status::NOT_FOUND)
		this->header += "Accept-Ranges: bytes" + EOL;
	
	if(status_code == Http::Status::PARTIAL_CONTENT)
		this->header += "Content-Range: bytes " + content_range + EOL;
	
	this->header += "Cache-Control: public, max-age=0" + EOL;
	this->header += "Content-Type: " + this->reqst_filetype + EOL;
	this->header += "Content-Length: " + content_length + EOL;
	this->header += "Connection: " + connection + EOL;
	this->header += options;
	this->header += "Date: " + getDate("%a, %d %b %Y %T %Z") + EOL;
	this->header += "X-Powered-By: " + this->server_name + EOL;
	this->header += EOL;
	this->header += custom_data;
	
	/*
	// Status NOT MODIFIED
    if(status_code == Http::Status::NOT_MODIFIED) {
        status = "304 Not Modified";
        connection = "keep-alive";
        // implement time check
    } else {
        if(status_code >= 200 && status_code < 300) {
			// Status OK
            if(status_code == Http::Status::OK) {
                status = "200 OK";
			// Status PARTIAL CONTENT
            } else if(status_code == Http::Status::PARTIAL_CONTENT) {
                status = "206 Partial Content";
            }
            
            if(this->begin_range >= 0) {
                status = "206 Partial Content";
                length = std::to_string(filelen - this->begin_range);
            } else this->begin_range = 0;

            connection = "keep-alive";

            this->header = "Content-Range: bytes " + std::to_string(this->begin_range) + "-" + std::to_string(filelen - 1) + "/" + filesize + "\r\nAccept-Ranges: bytes\r\nCache-Control: public, max-age=0\r\n";
        }
		// Status NOT FOUND
		else if(status_code == Http::Status::NOT_FOUND) {
            status = "404 Not Found";
            this->header = "x-content-type-options: nosniff\r\n";
            connection = "close";
			
			// Custom not found page.
            custom_data = "<html><head><title>404 Not Found</title></head><body><h1>Not Found</h1><p>The requested URL " + this->reqst_file + " was not found on this server.</p><hr><address>Projeto/Redes Webserver</address></body></html>";
            length = custom_data.length();
        }
        
        this->header += "Content-Type: " + reqst_filetype + "\r\nContent-Length: " + length + "\r\n";
    }

    this->header += "Connection: " + connection + "\r\n";
    
    this->header += "Access-Control-Allow-Origin: *\r\nAccess-Control-Allow-Headers: X-Requested-With, Content-Type, X-Codingpedia, X-HTTP-Method-Override\r\nDate: " + getDate("%a, %d %b %Y %T %Z") + "\r\nX-Powered-By: " + this->server_name + "\r\n";
    
    this->header = "HTTP/1.0 " + status + "\r\n" + this->header + "\r\n" + custom_data;
    */
    return this->header;
}

/******************************************************************************************/

t_byte * Http::createBinaryPacket(t_byte * file_bin, size_t file_size) {
    // Check if the header has been created.
	if(this->header.length() < 1)
        return NULL;
    
    size_t data_size;
    
	// Check if have data
    if(file_bin == NULL) {
        file_size = 0;
        data_size = 0;
    } else data_size = file_size - this->begin_range;
    
	// Sets the buffer size
    this->buffer_size = this->header.length() + data_size;
    
    // Allocates memory to buffer
    this->buffer = new t_byte[this->buffer_size];
   
    // Copy the header bytes to buffer
    memcpy(this->buffer, this->header.c_str(), this->header.length());
    
    // Copy the file bytes to buffer
    if(file_size > 0)
        memcpy(this->buffer + this->header.length(), file_bin + this->begin_range, file_size - this->begin_range);
        
    return this->buffer;
}
