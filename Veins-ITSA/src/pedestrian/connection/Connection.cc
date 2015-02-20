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

#include <Connection.hpp>

Client::Client(int socket, unsigned int lengthAddress) {
    this->socket = socket;
    this->lengthAddress = lengthAddress;
    writerBuffer = new ByteBuffer(BUFFER_SIZE);
    readerBuffer = new ByteBuffer(BUFFER_SIZE);
    mutex = PTHREAD_MUTEX_INITIALIZER;
}

Client::~Client() {
    delete writerBuffer;
    delete readerBuffer;
    pthread_mutex_destroy(&mutex);
}

void Client::closeConnection() {
    socket = -1;
    close(socket);
}

bool Client::isConnected() {
    return socket != -1;
}

void Client::sendPacket(WritablePacket* packet) {
    if (packet == NULL) return;

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

ByteBuffer* Client::read() {
    try {
        readerBuffer->clear();
        uint8_t buf[4096];
        ssize_t received = 0;
        uint8_t * tmp = buf;
        while (received < HEADER_SIZE) {
            int r = recv(socket, tmp, HEADER_SIZE - received, 0);
            if (r <= 0) {
                std::cout << "error receiving header " << std::endl;
                return 0;
            }else {
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
                return 0;
            }
        } while (readerBuffer->getWritePos() < datasize);

        return readerBuffer->clone();
    } catch (...) {
        std::cout << "error on read " << std::endl;
        return 0;
    }
}

void Client::write() {
    short size = writerBuffer->getShort() + HEADER_SIZE;
    uint8_t * array = new uint8_t[size];
    writerBuffer->setReadPos(0);
    writerBuffer->getBytes(array, size);

    int f = send(socket, array, size, 0);
    if (f < 0) {
        std::cout << "error on writing" << std::endl;
    }
    delete array;
}

NetClient::NetClient(int socket, sockaddr_in address, unsigned int lengthAddress) :
    Client(socket, lengthAddress){
    this->address = address;
}

std::string NetClient::getAddress() {
    return "";
}

BluetoothClient::BluetoothClient(int socket, sockaddr_rc address, unsigned int lengthAddress) :
    Client(socket, lengthAddress){
    this->address = address;
}

std::string BluetoothClient::getAddress() {
    return batostr(&address.rc_bdaddr);
}

Server::~Server() {
    delete reader;
}

void Server::acceptConnection() {
    if (waiting) return;
    pthread_t thread;
    if (reader != NULL)
        pthread_create(&thread, NULL, PacketReader::handler, (void*) reader);
}

void Server::setPacketReader(PacketReader* reader) {
    this->reader = reader;
}

NetServer::NetServer(int port, int connections) {
    mSocket = socket(AF_INET, SOCK_STREAM, 0);
    local.sin_family = AF_INET;
    local.sin_addr.s_addr = INADDR_ANY;
    local.sin_port = htons(port);
    bind(mSocket, (struct sockaddr*) &local, sizeof(local));
    listen(mSocket, connections);
    waiting = false;
}
Client* NetServer::waitConnection() {
    waiting = true;
    struct sockaddr_in clientAddress;
    unsigned int length = sizeof(clientAddress);
    int clientSocket = accept(mSocket, (struct sockaddr*) &clientAddress, &length);
    waiting = false;
    Client* client = NULL;
    if (clientSocket != -1) {
        client = new NetClient(clientSocket, clientAddress, length);
    }
    return client;
}

BluetoothServer::BluetoothServer(const char* mac, int port, int connections) {
    mSocket = socket(AF_BLUETOOTH, SOCK_STREAM, BTPROTO_RFCOMM);
    local.rc_family = AF_BLUETOOTH;
    str2ba(mac, &local.rc_bdaddr);
    local.rc_channel = (u_int8_t) port;
    bind(mSocket, (struct sockaddr*) &local, sizeof(local));
    listen(mSocket, connections);
    waiting = false;
}

Client* BluetoothServer::waitConnection() {
    waiting = true;
    struct sockaddr_rc clientAddress;
    unsigned int length = sizeof(clientAddress);
    int clientSocket = accept(mSocket, (struct sockaddr*) &clientAddress, &length);
    waiting = false;
    Client* client = NULL;
    if (clientSocket != -1) {
       client = new BluetoothClient(clientSocket, clientAddress, length);
    }
    return client;
}

// PacketReader
PacketReader::PacketReader(Server* con, Manager* manager) {
    this->server = con;
    this->manager = manager;
    listener = NULL;
}

PacketReader::~PacketReader() { }

void PacketReader::setConnection(Server* con) {
    this->server = con;
}

void PacketReader::setPacketListener(PacketListener* listener) {
    this->listener = listener;
}

void PacketReader::handlerWrapped() {
    Client* client = server->waitConnection();
    if(client == NULL) {
        server->acceptConnection();
        return;
    }
    std::cout << "connection successful, client address: " << client->getAddress() << std::endl;
    manager->handleConnection(client);
    try {
        do {
            std::cout << "Waiting for packet " << std::endl;
            ByteBuffer* buf = client->read();
            if (buf == NULL) {
                // something wrong happened, probably the connection is already closed.
                std::cout << "Error on client->read " << std::endl;
                break;
            }
            uint16_t opcode = buf->getShort();
            std::cout << "OPCODE " << opcode << std::endl;
            ReadablePacket * packet = createPacket(opcode, client);
            if (packet != NULL && listener != NULL) {
                packet->read(buf);
                listener->processPacket(packet, manager);
            }
            std::cout << "deleting buf " << std::endl;
            delete buf;
            std::cout << "deleting packet " << std::endl;
            delete packet;
            std::cout << "checking client" << std::endl;
        } while (client->isConnected());
    } catch (...) {
        std::cout << "error in handler " << std::endl;
    }
    manager->onDisconnection(client);
    delete client;
}

void* PacketReader::handler(void* context) {
    ((PacketReader*) context)->handlerWrapped();
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
