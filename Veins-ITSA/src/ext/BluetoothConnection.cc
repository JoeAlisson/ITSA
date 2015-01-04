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
 *  Created on: 01/11/2014
 *      Author: Alisson Oliveira
 *
 *  Updated on: 03/01/2015
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

BluetoothConnectionClient::BluetoothConnectionClient(int socket, sockaddr_rc address, unsigned int lengthAddress, Manager* manager) {
    this->socket = socket;
    this->lengthAddress = lengthAddress;
    this->address = address;
    this->manager = manager;
    writerBuffer = new ByteBuffer(BUFFER_SIZE);
    readerBuffer = new ByteBuffer(BUFFER_SIZE);
    mutex = PTHREAD_MUTEX_INITIALIZER;

}

BluetoothConnectionClient::~BluetoothConnectionClient() {
    std::cout << "Deleting Connection " << std::endl;
    delete writerBuffer;
    delete readerBuffer;
    std::cout << "Connection Deleted" << std::endl;
}

void BluetoothConnectionClient::handleDisconnection() {
    if(manager)
        manager->onDisconnection(this);
}

void BluetoothConnectionClient::closeConnection() {
    std::cout << "Closing Connection " << std::endl;
    socket = -1;
    close(socket);
    std::cout << "Connection Closed " << std::endl;
}

bool BluetoothConnectionClient::isConnected() {
    return socket != -1;
}

void BluetoothConnectionClient::sendPacket(WritablePacket* packet) {
    if (packet == NULL)
        return;
    std::cout << "Sending Packet " << packet->getOpcode() << std::endl;
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

void BluetoothConnectionClient::write() {
    short size = writerBuffer->getShort() + HEADER_SIZE;
    uint8_t * array = new uint8_t[size];
    writerBuffer->setReadPos(0);
    writerBuffer->getBytes(array, size);

    int f = send(socket, array, size, 0);
    if (f < 0) {
        std::cout << "error on write " << std::endl;
        handleDisconnection();
    }
    delete array;
}

ByteBuffer* BluetoothConnectionClient::read() {
    std::cout << "Waiting Packets " << std::endl;
    try {
        readerBuffer->clear();
        uint8_t buf[4096];
        ssize_t received = 0;
        // read header

        uint8_t * tmp = buf;
        while (received < HEADER_SIZE) {
            int r = recv(socket, tmp, HEADER_SIZE - received, 0);
            if (r == -1) {
                std::cout << "error receiving header " << std::endl;
                handleDisconnection();
                return 0;
            } else if (r == 0) {
                usleep(300);
            } else {
                tmp += r;
                received += r;
            }
        }
        tmp = NULL;

        readerBuffer->putBytes(buf, received);

        uint16_t datasize = readerBuffer->getShort();

        do {
            received = recv(socket, buf, datasize + HEADER_SIZE - readerBuffer->getWritePos(), 0);
            if (received > 0)
                readerBuffer->putBytes(buf, received);
            else if (received < 0) {
                std::cout << "error receiving data " << std::endl;
                handleDisconnection();
                return 0;
            }
        } while (readerBuffer->getWritePos() < datasize);

        return readerBuffer->clone();
    } catch (...) {
        std::cout << "error on read " << std::endl;
        handleDisconnection();
        return 0;
    }
}

std::string BluetoothConnectionClient::getAddress() {
    return batostr(&address.rc_bdaddr);
}

BluetoothConnectionServer::BluetoothConnectionServer(const char* mac, int porta, int connections) {
    mSocket = socket(AF_BLUETOOTH, SOCK_STREAM, BTPROTO_RFCOMM);
    local.rc_family = AF_BLUETOOTH;
    str2ba(mac, &local.rc_bdaddr);
    local.rc_channel = (u_int8_t) porta;
    bind(mSocket, (struct sockaddr*) &local, sizeof(local));
    listen(mSocket, connections);
    waiting = false;
}

BluetoothConnectionServer::~BluetoothConnectionServer() {
    if (reader) {
        delete reader;
    }
}

void BluetoothConnectionServer::accept() {
    if (waiting) return;
    pthread_t thread;
    if (reader != NULL)
        pthread_create(&thread, NULL, PacketReader::handler, (void*) reader);
}

void BluetoothConnectionServer::setPacketReader(PacketReader* reader) {
    this->reader = reader;
}

// PacketReader
PacketReader::PacketReader(BluetoothConnectionServer* con, Manager* manager) {
    this->server = con;
    this->manager = manager;
    packets = new BlockingQueue();
    listener = NULL;
    mutex = PTHREAD_MUTEX_INITIALIZER;
}

PacketReader::~PacketReader() {
    delete packets;
    pthread_mutex_destroy(&mutex);
}

void PacketReader::setConnection(BluetoothConnectionServer* con) {
    this->server = con;
}
void PacketReader::setPacketListener(PacketListener* listener) {
    this->listener = listener;
    handlerPackets();
}

void PacketReader::handlerPackets() {
    pthread_t thread;
    pthread_create(&thread, NULL, PacketReader::handlerPacketsWrapped, (void*) this);
}

void PacketReader::handlerWrapped() {
    server->waiting = true;
    std::cout << "Waiting Connections " << std::endl;
    struct sockaddr_rc clientAddress;
    unsigned int length = sizeof(clientAddress);
    int clientSocket = accept(server->mSocket, (struct sockaddr*) &clientAddress, &length);
    server->waiting = false;
    BluetoothConnectionClient* client = NULL;
    if (clientSocket != -1) {
        client = new BluetoothConnectionClient(clientSocket, clientAddress, length, manager);
    } else {
        server->accept();
        return;
    }
    std::cout << "connection successful, client: " << clientSocket << " address: " << client->getAddress() << std::endl;
    manager->handleConnection(client);
    try {
        do {
        std::cout << "start to try read" << std::endl;

        ByteBuffer* buf = client->read();
        if (buf == NULL) {
            // something wrong happened, probably the connection is already closed.
            std::cout << "Error on client->read " << std::endl;
            break;
        }
        uint16_t opcode = buf->getShort();
        std::cout << "OPCODE " << opcode << std::endl;
        ReadablePacket * packet = createPacket(opcode);
        if (packet != NULL) {
            packet->connection = client;
            packet->read(buf);
            packets->push(packet);
            if (listener != NULL) {
                handlerPackets();
            }
        }
        std::cout << "while check connected " << std::endl;
    } while (client->isConnected());
    } catch (...) {
        std::cout << "error in handler " << std::endl;
    }
    delete client;
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
            self->listener->processPacket(self->packets->pop(), self->manager);
        }
    }
    pthread_mutex_unlock(&self->mutex);
    return context;
}

// Packets base
void WritablePacket::writeString(std::string msg, ByteBuffer* buf) {
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
    }
    return str;
}
