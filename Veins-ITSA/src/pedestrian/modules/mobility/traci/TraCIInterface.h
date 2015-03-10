/*
 * TraCIInterface.h
 *
 *  Created on: Feb 16, 2015
 *      Author: Alisson Oliveira
 */

#ifndef TRACIINTERFACE_H_
#define TRACIINTERFACE_H_

#include <TraCICommandInterface.h>
#include <TraCIConnection.h>
#include <TraCICoord.h>

using Veins::TraCICommandInterface;
using Veins::TraCICoord;
using Veins::TraCIConnection;

class TraCIInterface : public TraCICommandInterface {
protected:
    std::string genericString(uint8_t commandId, std::string objectId, uint8_t variableId, uint8_t responseId);
    int32_t genericInt(uint8_t commandId, std::string objectId, uint8_t variableId, uint8_t responseId);
    std::list<std::string> genericStringList(uint8_t commandId, std::string objectId, uint8_t variableId, uint8_t responseId);
    TraCIConnection* con;
public:
    TraCIInterface(TraCIConnection* connection);
    virtual ~TraCIInterface() {}
    std::list<std::string> getTLIds();
    std::string getTypeId(std::string nodeId);
    std::string getTLState(std::string tlId);
    int32_t getCurrentTlState(std::string tlId);
    int32_t getTimeToNextTlState(std::string tlId);
    TraCICoord convertLonLatToTraCICoord(double longitude, double latitude, double altitude);
};

#endif /* TRACIINTERFACE_H_ */
