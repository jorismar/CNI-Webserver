/**
 * \file 	webserver.h
 * 
 * \author 	Jorismar Barbosa Meira <jorismar.barbosa@cc.ci.ufpb.br>
 */

#ifndef JB_WEBSERVER_H
#define JB_WEBSERVER_H

#include <string>
#include <thread>
#include "socket.h"
#include "http.h"
#include "util.h"

/**
 * \brief This class manages and controls the server for web (http) clients.
 * 
 * \headerfile webserver.h
 */

class Webserver {
    private:
    /******************************************************************************************/

        Socket*     socket;         // TCP Socket for server
        std::string path;           // Path of the Website files
        int         port;           // Port used for the server
        bool        alive;          // Flag to keep the server alive.

    /******************************************************************************************/
        
    public:
        
        /**
         * \brief   Constructor
         * 
         * \param   port            TCP port for HTTP server
         * \param   path            Website files path
         */
        Webserver(int port, std::string path);

        /**
         * \brief   Destructor
         */
        virtual ~Webserver();

        /**
         * \brief   This method start the HTTP server
         */
        void start();

        /**
         * \brief   This method stop the HTTP server
         */
        void stop();

        /**
         * \brief   This method start a thread to meet a specific client.
         *
         * \param   cl  Socket of a connected client.
         */
        void startClient(t_socket cl);

        /**
         * \brief   Set the TCP port
         *
         * \param   port    TCP port for the HTTP connections
         */
        void setPort(int port);

        /**
         * \brief   Get the TCP port
         *
         * \return   Return the TCP port for the HTTP connections
         */
        int getPort();
};

#endif
