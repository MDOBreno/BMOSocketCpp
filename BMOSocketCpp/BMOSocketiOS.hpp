//
//  BMOSocketiOS.hpp
//  BMOSocketCpp
//
//  Created by Breno Medeiros on 08/03/21.
//  Copyright © 2021 ProgramasBMO. All rights reserved.
//

#ifndef BMOSocketiOS_hpp
#define BMOSocketiOS_hpp

#include <stdio.h>

#include <sys/types.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <string.h>
#include <string>

#include <unistd.h>

#include <sys/socket.h>
#include <sstream>

#include <iostream>
#define PORTA 5000


class BMOSocketiOS {
    public:
        BMOSocketiOS(struct sockaddr_in saddrSizeInicial, int opcaoInicial);
        void setOpcao(int x);
        int getOpcao();
        void setSaddr(struct sockaddr_in saddrInicial);
        struct sockaddr_in getSaddr();
        int saddrSize();
        int criarSocketServidor(int ipv4ou6, int TCPouUDP, int protocolo);
        void setSocketServidor(int x);
        int getSocketServidor();
        void setPorta(int x);
        int getPorta();
        void setMaxDeConexoes(int x);
        int getMaxDeConexoes();
        void setTamanhoBytesDoBuffer(int x);
        int getTamanhoBytesDoBuffer();
        void startSocketServidor(int porta, int maxDeConexoes, int tamanhoBytesDoBuffer);
        void setSs(std::string x);
        std::string getSs();
        void imprimir(std::string x, bool adicionarQuebraDeLinha);
        std::string popSs();
    private:
        int opcao;
        struct sockaddr_in saddr;
        int socketServidor;
        int porta;
        int maxDeConexoes;
        int tamanhoBytesDoBuffer;
        std::stringstream ss;
};


#endif /* BMOSocketiOS_hpp */
