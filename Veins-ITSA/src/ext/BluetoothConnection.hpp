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
 *  Updated on: Jan 02, 2015
 */

#ifndef BLUETOOTHCONNECTION_HPP_
#define BLUETOOTHCONNECTION_HPP_

#include <unistd.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/rfcomm.h>
#include <ByteBuffer.h>
#include <pthread.h>
#include <queue>

#define HEADER_SIZE 2
#define BUFFER_SIZE 64*1024

class WritablePacket;
class ReadablePacket;
class PacketReader;
class Manager;

class BlockingQueue {
    std::queue<ReadablePacket*>* queue;
    pthread_mutex_t mutex;
public:
    BlockingQueue();
    ~BlockingQueue();
    bool empty();
    int size();
    void push(ReadablePacket*);
    ReadablePacket* pop();
};

class BluetoothConnectionClient {
    int socket;
    ByteBuffer *writerBuffer, *readerBuffer;
    struct sockaddr_rc address;
    unsigned int lengthAddress;
    pthread_mutex_t mutex;
    Manager* manager;
    void write();
    virtual void handleDisconnection();
public:
    BluetoothConnectionClient(int socket, sockaddr_rc address, unsigned int lengthAddress, Manager* manager);
    virtual ~BluetoothConnectionClient();
    virtual bool isConnected();
    virtual void sendPacket(WritablePacket*);
    virtual ByteBuffer* read();
    std::string getAddress();
    virtual void closeConnection();
};

class BluetoothConnectionServer {
    friend class PacketReader;
    PacketReader* reader;
    bool waiting;
    struct sockaddr_rc local;
    int mSocket;
public:
    ~BluetoothConnectionServer();
    BluetoothConnectionServer(const char*, int = 1, int = 7);
    void accept();
    void setPacketReader(PacketReader*);
};

class Manager {
public:
    virtual void onForcedDisconnection(BluetoothConnectionClient* connection) = 0;
    virtual void handleConnection(BluetoothConnectionClient* connection) = 0;
    virtual void onDisconnection(BluetoothConnectionClient* connection) = 0;
};

class WritablePacket {
public:
    static const short OPCODE = -1;
    virtual ~WritablePacket() {
    }
    virtual void write(BluetoothConnectionClient* connection, ByteBuffer* buf) = 0;
    virtual short getOpcode() = 0;
protected:
    void writeString(std::string, ByteBuffer*);

};


class ReadablePacket {
    friend class PacketReader;
public:
    static const short OPCODE =-1;
    virtual ~ReadablePacket() {
    }
    virtual void read(ByteBuffer* buf)=0;
    virtual void process(Manager* manager)=0;
protected:
    std::string readString(ByteBuffer* buf);
    BluetoothConnectionClient* connection;
};

class PacketListener {
public:
    virtual void processPacket(ReadablePacket* packet, Manager* manager ) = 0;
};

class PacketReader {
    PacketListener* listener;
    pthread_mutex_t mutex;
    BlockingQueue* packets;
    bool running;
    void handlerWrapped();
    void handlerPackets();
    static void* handlerPacketsWrapped(void*);
protected:
    BluetoothConnectionServer* server;
    Manager* manager;
    void setConnection(BluetoothConnectionServer*);
public:
    PacketReader(BluetoothConnectionServer* = NULL, Manager* = NULL);
    virtual ~PacketReader();
    static void* handler(void*);
    virtual ReadablePacket* createPacket(uint16_t) = 0;
    void setPacketListener(PacketListener*);
};

#endif /* BLUETOOTHCONNECTION_HPP_ */
