/*
 * Rotas.h
 *
 *  Created on: Jan 18, 2015
 *      Author: Alisson Oliveira
 */

#ifndef ROUTES_H_
#define ROUTES_H_
#include <map>

class Routes {
    std::map<std::string, std::string> rotas;
public:
    Routes();
    virtual ~Routes() {}
    std::string getRota(std::string rotaId) {
        if (rotas.count(rotaId))
            return rotas[rotaId];
        return "";
    }
};

class RoutesAccess {
    static Routes* routes;
public:
    Routes* getAll() const;
    std::string get(std::string routeId);
};

#endif /* ROTAS_H_ */
