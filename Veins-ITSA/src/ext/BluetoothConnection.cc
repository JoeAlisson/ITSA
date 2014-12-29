/*
 * BluetoothConnection.cpp
 *
 *  Created on: 01/11/2014
 *      Author: Alisson Oliveira
 */

#include <BluetoothConnection.hpp>

BlockingQueue::BlockingQueue() {
    queue = new std::queue<ReadablePacket*>();
    mutex = PTHREAD_MUTEX_INITIALIZER;
}

BlockingQueue::~BlockingQueue() {
    delete queue;
    pthread_mutex_destroy(&mutex);
}

bool BlockingQueue::empty() {
    return queue->empty();
}

int BlockingQueue::size() {
    return queue->size();
}

void BlockingQueue::push(ReadablePacket* item) {
    pthread_mutex_lock(&mutex);
    {
        queue->push(item);
    }
    pthread_mutex_unlock(&mutex);
}

ReadablePacket* BlockingQueue::pop() {
    ReadablePacket* item = NULL;
    pthread_mutex_lock(&mutex);
    {
        item = queue->front();
        queue->pop();
    }
    pthread_mutex_unlock(&mutex);
    return item;
}

// BluetoothConnection
//

BluetoothConnection::BluetoothConnection(const char* mac, int porta) {
    client = -1;
    mutex = PTHREAD_MUTEX_INITIALIZER;
    opt = sizeof(remote);
    mSocket = socket(AF_BLUETOOTH, SOCK_STREAM, BTPROTO_RFCOMM);
    local.rc_family = AF_BLUETOOTH;
    str2ba(mac, &local.rc_bdaddr);
    local.rc_channel = (u_int8_t) porta;
    bind(mSocket, (struct sockaddr*) &local, sizeof(local));
    listen(mSocket, 1);
    writerBuffer = new ByteBuffer(BUFFER_SIZE);
    readerBuffer = new ByteBuffer(BUFFER_SIZE);
    waiting = false;
}

BluetoothConnection::~BluetoothConnection() {
    if(reader) {
        delete reader;
    }
    delete writerBuffer;
    delete readerBuffer;
}

void BluetoothConnection::accept() {
    pthread_t thread;
    if (reader != NULL)
        pthread_create(&thread, NULL, PacketReader::handler, (void*) reader);
}

void BluetoothConnection::handlerDisconnection() {
    close(client);
    client = -1;
    accept();
}

bool BluetoothConnection::isConnected() {
    return client != -1;
}

void BluetoothConnection::sendPacket(WriteblePacket* packet) {
    if (packet == NULL)
        return;

    pthread_mutex_lock(&mutex);
    {
        writerBuffer->clear();
        writerBuffer->setWritePos(HEADER_SIZE);
        writerBuffer->putShort(packet->getOpcode());
        packet->write(this, writerBuffer);
        short dataSize = writerBuffer->getWritePos() - HEADER_SIZE;
        writerBuffer->setWritePos(0);
        writerBuffer->putShort(dataSize);
        writerBuffer->setWritePos(dataSize);
        write();
    }
    pthread_mutex_unlock(&mutex);
    delete packet;
}

void BluetoothConnection::write() {
    short size = writerBuffer->getShort() + HEADER_SIZE;
    uint8_t * array = new uint8_t[size];
    writerBuffer->setReadPos(0);
    writerBuffer->getBytes(array, size);
    try {
        int f = send(client, array, size, 0);
        if (f < 0) {
            handlerDisconnection();
        }
    } catch (...) {
        std::cout << "error on write " << std::endl;
    }
    delete array;
}

ByteBuffer* BluetoothConnection::read() {
    readerBuffer->clear();
    uint8_t buf[4096];
    ssize_t received;
    // read header
    short count = 0;
    while (count < HEADER_SIZE) {
        int i = ioctl(client, FIONREAD, &count);
        usleep(300);
        if (i == -1) {
            handlerDisconnection();
            return 0;
        }
    }

    received = recv(client, buf, HEADER_SIZE, 0);
    if (received == -1) {
        handlerDisconnection();
        return 0;
    }

    readerBuffer->putBytes(buf, received);

    unsigned short datasize = readerBuffer->getShort() + HEADER_SIZE;
    do {
        received = recv(client, buf, datasize - readerBuffer->getWritePos(), 0);
        if (received > 0)
            readerBuffer->putBytes(buf, received);
        else if (received < 0) {
            handlerDisconnection();
            return 0;
        }
    } while (readerBuffer->getWritePos() < datasize);

    return readerBuffer->clone();
}

void BluetoothConnection::setPacketReader(PacketReader* reader) {
    this->reader = reader;
}

// PacketReader
//

PacketReader::PacketReader(BluetoothConnection* con) {
    this->con = con;
    packets = new BlockingQueue();
    listener = NULL;
    mutex = PTHREAD_MUTEX_INITIALIZER;
}

PacketReader::~PacketReader() {
    delete packets;
    pthread_mutex_destroy(&mutex);
}

void PacketReader::setConnection(BluetoothConnection* con){
    this->con = con;
}
void PacketReader::setPacketListener(PacketListener* listener) {
    this->listener = listener;
    handlerPackets();
}

void PacketReader::handlerPackets() {
    pthread_t thread;
    pthread_create(&thread, NULL, PacketReader::handlerPacketsWrapped,
            (void*) this);
}

void PacketReader::handlerWrapped() {
    con->waiting = true;
    con->client = accept(con->mSocket, (struct sockaddr*) &con->remote, &con->opt);
    con->waiting = false;
    if (!con->isConnected())
        con->handlerDisconnection();
        return;
    do {
        try {
            ByteBuffer* buf = con->read();
            if (buf == NULL) {
                return;
            }
            short opcode = buf->getShort();
            ReadablePacket * packet = createPacket(opcode);
            if (packet != NULL) {
                packet->read(buf);
                packets->push(packet);
                if (listener != NULL) {
                    handlerPackets();
                }
            }
        } catch (...) {
            std::cout << "error in handler " << std::endl;
            break;
        }

    } while (true);
}

void* PacketReader::handler(void* context) {
    ((PacketReader*) context)->handlerWrapped();
    return context;
}

void* PacketReader::handlerPacketsWrapped(void* context) {
    PacketReader* self = (PacketReader*) context;
    pthread_mutex_lock(&self->mutex);
    {
        while (!self->packets->empty() && self->listener != NULL) {
            self->listener->processPacket(self->packets->pop());
        }
    }
    pthread_mutex_unlock(&self->mutex);
    return context;
}

// Packets base
void WriteblePacket::writeString(std::string msg, ByteBuffer* buf) {
    for (unsigned short i = 0; i < msg.size(); ++i) {
        buf->putChar(msg[i]);
    }
    buf->putChar('\000');
}


std::string ReadablePacket::readString(ByteBuffer * buf) {
    std::string str = "";
    char c;
    while ((c = buf->getChar()) != '\000') {
        str += c;
        std::cout << c;
    }
    std::cout << c << std::endl;

    return str;
}
