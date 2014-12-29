/*
 * handler.hpp
 *
 *  Created on: 20/11/2014
 *      Author: alisson
 */

#ifndef HANDLER_HPP_
#define HANDLER_HPP_

#include <BluetoothConnection.hpp>
#include <Packets.hpp>

class PacketHandler : public PacketReader, public PacketListener {
public:
    PacketHandler(BluetoothConnection*);
    virtual ReadablePacket* createPacket(short);
    virtual void processPacket(ReadablePacket*);
};

#endif /* HANDLER_HPP_ */
