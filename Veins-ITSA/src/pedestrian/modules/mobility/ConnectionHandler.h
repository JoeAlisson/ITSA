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
 *  Created on: Dec 31, 2014
 *      @author: Alisson Oliveira
 *
 *  Updated on: 02/01/2015
 */

#ifndef CONNECTIONHANDLER_H_
#define CONNECTIONHANDLER_H_

#include <Connection.hpp>
#include <Packets.hpp>

class ConnectionHandler : public PacketReader, public PacketListener {
    int activeConnections;
    int maxConnections;
public:
    ConnectionHandler(Server*, Manager*, int=7);
    virtual ~ConnectionHandler();
    virtual ReadablePacket* createPacket(uint16_t, Client*);
    virtual void processPacket(ReadablePacket*, Manager*);
    void listen();
    void onConnected();
    void onDisconnected();
};

#endif /* CONNECTIONHANDLER_H_ */
