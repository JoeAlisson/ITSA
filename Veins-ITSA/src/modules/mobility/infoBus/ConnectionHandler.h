/*
 * ConnectionHandler.h
 *
 *  Created on: Dec 31, 2014
 *      Author: Alisson Oliveira
 */

#ifndef CONNECTIONHANDLER_H_
#define CONNECTIONHANDLER_H_

#include <BluetoothConnection.hpp>
#include <Packets.hpp>

class ConnectionHandler : public PacketReader, public PacketListener {
protected:
public:
    ConnectionHandler(const char*, Manager*);
    virtual ~ConnectionHandler();

    virtual ReadablePacket* createPacket(short);
    virtual void processPacket(ReadablePacket*, Manager*);
};

#endif /* CONNECTIONHANDLER_H_ */
