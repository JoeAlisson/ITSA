//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see http://www.gnu.org/licenses/.
//

/*
 *
 *  Created on: Nov 01, 2014
 *      @author: Alisson Oliveira
 *
 *  Updated on: Jan 03, 2015
 */

#ifndef CONNECTION_HPP_
#define CONNECTION_HPP_

#include <unistd.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/rfcomm.h>
#include <netinet/in.h>
#include <ByteBuffer.h>
#include <pthread.h>
#include <queue>

#define HEADER_SIZE 2
#define BUFFER_SIZE 64*1024

class WritablePacket;
class ReadablePacket;
class PacketReader;
class Manager;

class Client {
protected:
    int socket;
    ByteBuffer *writerBuffer, *readerBuffer;
    unsigned int lengthAddress;
    pthread_mutex_t mutex;
    virtual void write();
public:
    virtual ~Client();
    Client(int socket, unsigned lengthAddress);
    virtual bool isConnected();
    virtual ByteBuffer* read();
    virtual std::string getAddress() = 0;
    virtual void closeConnection();
    virtual void sendPacket(WritablePacket*);
};

class NetClient: public Client {
    struct sockaddr_in address;
public:
    NetClient(int socket, sockaddr_in address, unsigned int lengthAddress);
    virtual ~NetClient(){}
    virtual std::string getAddress();
};

class BluetoothClient: public Client {
    struct sockaddr_rc address;
public:
    BluetoothClient(int socket, sockaddr_rc address, unsigned int lengthAddress);
    virtual ~BluetoothClient(){}
    virtual std::string getAddress();
};

class Server {
protected:
    friend class PacketReader;
    PacketReader* reader;
    bool waiting;
    int mSocket;
public:
    virtual ~Server();
    virtual void acceptConnection();
    virtual void setPacketReader(PacketReader*);
    virtual Client* waitConnection() = 0;
};

class NetServer: public Server {
    struct sockaddr_in local;
public:
    NetServer(int port = 3528, int connections = 5);
    virtual ~NetServer() {
    }
    virtual Client* waitConnection();
};

class BluetoothServer: public Server {
    struct sockaddr_rc local;
public:
    ~BluetoothServer() {
    }
    BluetoothServer(const char*, int = 1, int = 5);
    virtual Client* waitConnection();
};

class Manager {
public:
    virtual void handleConnection(Client* connection) = 0;
    virtual void onDisconnection(Client* connection) = 0;
};

class WritablePacket {
public:
    static const short OPCODE = -1;
    virtual ~WritablePacket() { }
    virtual void write(Client* connection, ByteBuffer* buf) = 0;
    virtual short getOpcode() = 0;
protected:
    void writeString(std::string, ByteBuffer*);

};

class ReadablePacket {
    friend class PacketReader;
public:
    static const short OPCODE = -1;
    virtual ~ReadablePacket() {}
    virtual void read(ByteBuffer* buf)=0;
    virtual void process(Manager* manager)=0;
    void setConnection(Client* con) { connection = con;}
protected:
    std::string readString(ByteBuffer* buf);
    Client* connection;
};

class PacketListener {
public:
    virtual void processPacket(ReadablePacket* packet, Manager* manager) = 0;
};

class PacketReader {
    PacketListener* listener;
    bool running;
    void handlerConnections();
protected:
    Server* server;
    Manager* manager;
    void setConnection(Server*);
public:
    PacketReader(Server* = NULL, Manager* = NULL, PacketListener* = NULL);
    virtual ~PacketReader();
    static void* handler(void*);
    virtual ReadablePacket* createPacket(uint16_t, Client*) = 0;
    void setPacketListener(PacketListener*);
};

#endif /* CONNECTION_HPP_ */
