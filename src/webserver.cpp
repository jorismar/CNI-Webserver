#include "webserver.h"

/******************************************************************************************/

Webserver::Webserver(int port, std::string path) {
    this->port = port;
    this->path = path;
}

/******************************************************************************************/

Webserver::~Webserver() {
    this->alive = false;
    delete[] this->socket;
}

/******************************************************************************************/

void Webserver::start() {
	// Instantiate the socket
    this->socket = new Socket(this->port);
    
	// Bind the port
    if(this->socket->Bind() >= 0) {
		// Listen the port
        if(this->socket->Listen(10) >= 0) {
			// Socket to capture the client connection
			t_socket client;
			
			this->alive = true;
		
			std::cout << "[INFO] Server running on port: " << this->port << std::endl;
		
			while(alive) {
				// Wait for client connection
				client = this->socket->Accept();
				
				if(client >= 0) {
					// Start the client thread
					std::thread cl([=](){ Webserver::startClient(client); });
					cl.detach();
				}
			}
			
			std::cout << "[INFO] Session closed!" << std::endl;
		} else std::cout << "[ERROR] Webserver listen error" << std::endl;
    } else std::cout << "[ERROR] Webserver bind error" << std::endl;
	
	// Close server socket
    this->socket->CloseServer();
}

/******************************************************************************************/

void Webserver::stop() {
	// Break server loop
    this->alive = false;
    
	// Open a new socket to connect to webserver and unlock accept function.
    Socket * unlock = new Socket();
    
	// Self-connect
    while(unlock->Connect("localhost", this->port) < 0); // Unlock Accept();
    
	// Close socket
    unlock->Close();
	
	// Release memory
	delete[] unlock;
}

/******************************************************************************************/

void Webserver::startClient(t_socket cl) {
    Http   * http = new Http();
    t_byte * buffer = new t_byte[Http::BufferSize::MAX];
	t_byte * file	= NULL;
    
	// Receive request
	std::cout << "[INFO] Client: " << cl << " - Request received." << std::endl;
    Socket::readFrom(cl, buffer, Http::BufferSize::MAX, 5);

	// Process request
	http->processRequest(buffer);
	
	// Read file
	int size = readFile(this->path + http->getReqstedFile(), &file);
	    
	// Generates the binary packet (header + data)
	if(size > 0) {
		http->createResponseHeader(size, Http::Status::OK);		// Generates the response header
		http->createBinaryPacket(file, size);					// Generates the binary packet
	} else {
		http->createResponseHeader(0, Http::Status::NOT_FOUND);
		http->createBinaryPacket(NULL, 0);
	}
	
	// Send response
	if(size > 0)
		std::cout << "[INFO] Client: " << cl << " - Sending the request file: " + http->getReqstedFile() + "..." << std::endl;
	Socket::sendTo(cl, http->getBinaryPacket(), http->getBinarySize());
	if(size > 0)
		std::cout << "[INFO] Client: " << cl << " - Request file: " + http->getReqstedFile() + " sended." << std::endl;

	// Release memory
    delete[] buffer;
	if(file != NULL)
		free(file);

	// Close socket
    Socket::Close(cl);
}

/******************************************************************************************/

void Webserver::setPort(int port) {
    this->port = port;
}

/******************************************************************************************/

int Webserver::getPort() {
    return this->port;
}
