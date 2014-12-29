/*
 * bluetoothConnection.hpp
 *
 *  Created on: 01/11/2014
 *      Author: Alisson Oliveira
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

class WriteblePacket;
class ReadablePacket;
class PacketReader;

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

class BluetoothConnection {
    friend class PacketReader;
    PacketReader* reader;
    pthread_mutex_t mutex;
    ByteBuffer* writerBuffer;
    ByteBuffer* readerBuffer;
    void write();
    void handlerDisconnection();
public:
    bool waiting;
    struct sockaddr_rc local, remote;
    int mSocket, client;
    unsigned int opt;
    ~BluetoothConnection();
    BluetoothConnection(const char*, int = 1);
    void accept();
    void sendPacket(WriteblePacket*);
    ByteBuffer* read();
    bool isConnected();
    void setPacketReader(PacketReader*);
};

class WriteblePacket {
public:
    static const short OPCODE = -1;
    virtual ~WriteblePacket() {
    }
    virtual void write(BluetoothConnection*, ByteBuffer*) = 0;
    virtual short getOpcode() = 0;
protected:
    void writeString(std::string, ByteBuffer*);

};

class ReadablePacket {
public:
    static const short OPCODE =-1;
    virtual ~ReadablePacket() {
    }
    virtual void read(ByteBuffer*)=0;
    virtual void process(BluetoothConnection*)=0;
protected:
    std::string readString(ByteBuffer*);
};

class PacketListener {
public:
    virtual void processPacket(ReadablePacket*) = 0;
};

class PacketReader {
    PacketListener* listener;
    pthread_mutex_t mutex;
    BlockingQueue* packets;
    void handlerWrapped();
    void handlerPackets();
    static void* handlerPacketsWrapped(void*);
protected:
    BluetoothConnection* con;
    void setConnection(BluetoothConnection*);
public:
    PacketReader(BluetoothConnection* = NULL);
    virtual ~PacketReader();
    static void* handler(void*);
    virtual ReadablePacket* createPacket(short) = 0;
    void setPacketListener(PacketListener*);
};

#endif /* BLUETOOTHCONNECTION_HPP_ */
