/**
 * \file 	http.h
 * 
 * \author 	Jorismar Barbosa Meira <jorismar.barbosa@cc.ci.ufpb.br>
 */

#ifndef JB_HTTP_H
#define JB_HTTP_H

#include <string>
#include <cstring>
#include <stdexcept>
#include "util.h"

/**
 * \brief   This class implements the essentials operations for HTTP protocol.
 * 
 * \headerfile http.h
 */

class Http {
    private:
    /**************************************************************************************/
        int         begin_range;    // Requested content begin range.
        int         end_range;      // Requested content end range.
        size_t      buffer_size;    // Buffer size
        t_byte *    buffer;         // Buffer for binary packet generated.
        std::string reqst_file;     // Requested filename.
        std::string reqst_filetype; // Requested file type.
        std::string server_name;    // Server name
        std::string header;         // Container generated header.

    /**************************************************************************************/

        /**
         * \brief   Takes a string, searching for a particular brand and collecting a
         *          substring starting after the last marker character found even to the
         *          termination character.
         *
         * \param   src     Origin source.
         * \param   mark    Mark to be find.
         * \param   end     End character
         *
         * \return  Return a string containing the substring found.
         */
        std::string getfield(std::string src, std::string mark, char end);

    public:

        /**
         * \enum    Status
         * 
         * \brief   Constants of the supported responses status.
         */
        enum Status {
            OK = 200,               /*!< Status code 200 OK */
            PARTIAL_CONTENT = 206,  /*!< Status code 206 Partial Content */
            NOT_FOUND = 404,        /*!< Status code 404 Not Found */
        };

        /**
         * \enum    BufferSize
         * 
         * \brief   Constants of the buffer sizes.
         */
        enum BufferSize {
            DEFAULT = 1024,     /*!< Default value of HTTP buffer size 1 KB */
            MAX = 1024000       /*!< Max value of HTTP buffer size 1 MB */
        };
        
        /**
         * \brief   Constructor
         */
        Http();

        /**
         * \brief   Destructor
         */
        virtual ~Http();
        
        /**
         * \brief   Get the file requested by the client.
         *
         * \return  Return the requested file.
         */
        std::string getReqstedFile();

        /**
         * \brief   Set the server name.
         *
         * \param   name    New name of server.
         */
        void setServerName(std::string name);

        /**
         * \brief   Get the generated binary packet size.
         *
         * \return  Return the binary packet size.
         */
        size_t getBinarySize();

        /**
         * \brief   Get the generated binary packet pointer.
         *
         * \return  Return the binary packet pointer.
         */
        t_byte * getBinaryPacket();

        /**
         * \brief   Get the generated header.
         *
         * \return  Return a string of the generated header.
         */
        std::string getHeader();

        /**
         * \brief   Processes a request header and collects the data contained therein.
         *
         * \param   header  Byte pointer of binary packet from a HTTP/TCP connection.
         */
        void processRequest(t_byte* header);

        /**
         * \brief   Create a response header.
         *
         * \param   filelen         Size of file to be send with the header.
         * \param   status_code          HTTP status code value.
         *
         * \return  Return a string containing the generated header.
         */
        std::string createResponseHeader(size_t filelen, int status_code);

        /**
         * \brief   Create a binary package with the generated header and the binary of the file to be sent.
         *
         * \param   file_bin    Pointer to the binary file buffer.
         * \param   file_size   Size of file buffer.
         *
         * \return  Return a pointer of the binary packet containig the generated header and the binary data of the file.
         */
        t_byte* createBinaryPacket(t_byte * file_bin, size_t file_size);
};

#endif
