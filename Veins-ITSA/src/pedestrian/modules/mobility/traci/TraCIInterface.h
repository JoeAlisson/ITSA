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
    TraCIConnection* con;
public:
    TraCIInterface(TraCIConnection* connection);
    virtual ~TraCIInterface();
    std::string getTypeId(std::string nodeId);
    TraCICoord positionConversionCoord(double longitude, double latitude, double altitude);
};

#endif /* TRACIINTERFACE_H_ */
