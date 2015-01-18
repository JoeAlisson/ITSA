/*
 * Rotas.cc
 *
 *  Created on: Jan 18, 2015
 *      Author: Alisson Oliveira
 */

#include <Routes.h>

Routes::Routes() {

    rotas["rota1"] = "Centro,Novo horizonte,Santa Esmeralda,Alto do Cruzeiro,Caititus,Santa Edwiges,Planalto,Bom Sucesso";

    rotas["rota2"] = "Bom Sucesso,Cavaco,Baixa Grande,Eldorado,Baixão,Manoel Teles,Centro,Caititus,Alto do Cruzeiro,Santa Esmeralda,Brasília,Itapuã,Nova Esperança,Canafistula,Boa Vista";

    rotas["rota3"] = "Centro,Ouro Preto,Jardim Tropical,São Luiz,Primavera,Olho D'Água dos Cazuzinhos,Manoel Teles,Baixão,Eldorado,Capiatã,Senador Teotônio Vilela,Caititus,Jardim de Maria,Planalto,Massaranduba,Bom Sucesso";

    rotas["itn"] = "Alto do Cruzeiro,Baixa Grande,Baixão,Boa Vista,Bom Sucesso,Brasiliana,Brasília,Cacimbas,Caititus,Canafistula,Capiatã,Cavaco,Centro,Eldorado,Guaribas,Itapuã,Jardim de Maria,Jardim Esperança,Jardim Tropical,João Paulo Segundo,Manoel Teles,Massaranduba,Nova Esperança,Novo horizonte,Olho D'Água dos Cazuzinhos,Ouro Preto,Planalto,Primavera,Santa Edwiges,Santa Esmeralda,São Luiz,Senador Arnon de Melo,Senador Teotônio Vilela,Senador Nilo Coelho,Verdes Campos,Zélia Barbosa Rocha";
}



Routes* RoutesAccess::routes = new Routes();

Routes* RoutesAccess::getAll() const {
    if (routes == NULL) {
        routes = new Routes();
    }
    return routes;
}

std::string RoutesAccess::get(std::string routeId) {
    getAll();
    return routes->getRota(routeId);
}
