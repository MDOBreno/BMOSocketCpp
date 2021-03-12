//
//  BMOSocketiOS.cpp
//  BMOSocketCpp
//
//  Created by Breno Medeiros on 08/03/21.
//  Copyright © 2021 ProgramasBMO. All rights reserved.
//

#include "BMOSocketiOS.hpp"

BMOSocketiOS::BMOSocketiOS(struct sockaddr_in saddrInicial, int opcaoInicial) {
    saddr = saddrInicial;
    opcao = opcaoInicial;
}

void BMOSocketiOS::setOpcao(int x) {
    opcao = x;
}
 
int BMOSocketiOS::getOpcao() {
    return opcao;
}

void BMOSocketiOS::setSaddr(struct sockaddr_in saddrInicial) {
    saddr = saddrInicial;
}

struct sockaddr_in BMOSocketiOS::getSaddr() {
    return saddr;
}

int BMOSocketiOS::saddrSize() {
    int saddrSize = sizeof(saddr);
    return saddrSize;
}

int BMOSocketiOS::criarSocketServidor(int ipv4ou6, int TCPouUDP, int protocolo) {
    socketServidor = socket(ipv4ou6, TCPouUDP, protocolo); //Cria e retorna o Número de socket desse servidor
    setsockopt(socketServidor, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opcao, sizeof(opcao));
    return socketServidor;
}

void BMOSocketiOS::setSocketServidor(int x) {
    socketServidor = x;
}
 
int BMOSocketiOS::getSocketServidor() {
    return socketServidor;
}

void BMOSocketiOS::setPorta(int x) {
    porta = x;
}
 
int BMOSocketiOS::getPorta() {
    return porta;
}

void BMOSocketiOS::setMaxDeConexoes(int x) {
    maxDeConexoes = x;
}
 
int BMOSocketiOS::getMaxDeConexoes() {
    return maxDeConexoes;
}

void BMOSocketiOS::setTamanhoBytesDoBuffer(int x) {
    tamanhoBytesDoBuffer = x;
}
 
int BMOSocketiOS::getTamanhoBytesDoBuffer() {
    return tamanhoBytesDoBuffer;
}

void BMOSocketiOS::startSocketServidor(int porta, int maxDeConexoes, int tamanhoBytesDoBuffer){
    
    // CLIENTE
    struct sockaddr_in caddr;                           //Informações do Cliente [Como Endereço e Porta ultilizadas pelo Cliente]
    socklen_t caddrSize = sizeof caddr;                 // Tamanho em bytes das informações do cliente
    int socketCliente;                                  // Número de socket do cliente
    
    
    
  //VINCULAR(bind) o socket do SERVIDOR a um endereço+porta
    // A função bind() serve para alocar um endereço+porta da rede a um Socket
    bind(socketServidor, (struct sockaddr*)&saddr, sizeof(saddr));    //O terceiro argumento(saddr) é o tamnho em bytes do segundo arg.


    
  //COLOCAR/Avisar ao Winsock que o socket ficará no modo de escuta(listening)
    listen(socketServidor, maxDeConexoes);    // O segundo argumento define o numero de conexões permitidas neste socket. Nesse caso = 128
    //Print da escuta
    std::stringstream ss;
    ss << porta;
    std::cout << "[Servidor] Escutando na porta " << ss.str() << std::endl;
    
    
    
    char buff[tamanhoBytesDoBuffer];
    int tamanhoBytesDadosRecebidos;
    
  //ENQUANTO(while) está recebendo mensagem, exibir o eco 'echo mensage' de volta pro cliente
    // Evitar que o programa se encerre, mantendo o programa em listening
    while (true) {
        // Aceitar conexões feitas, retornando um novo socket em que ha a nova conexao do servidor com o cliente
        socketCliente = accept(socketServidor, (struct sockaddr*)&caddr, (socklen_t*)&caddrSize);
        std::cout << "[Servidor] Cliente conectado com sucesso." << std::endl;
        
        
        // Tentando descobrir o nome do Host-cliente
        char hostCliente[NI_MAXHOST];       // Host: Nome/Endereço remoto do cliente.           Neste caso com tamanho = 1025
        char portaCliente[NI_MAXSERV];      // Serviço: Porta ao qual o cliente está conectado. Neste caso com tamanho = 32
        memset(hostCliente, 0, NI_MAXHOST); // Como não sabemos o que havia na memoria antes da instacia do 'endereço', botamos zeros na memoria
        memset(portaCliente, 0, NI_MAXSERV);
        //Tentando abaixo conseguir o nome da maquina do cliente. Caso não consiga(else) então trapaciamos! :D
        if (getnameinfo((sockaddr*)&caddr, sizeof(caddr), hostCliente, NI_MAXHOST, portaCliente, NI_MAXSERV, 0) == 0) {
           std::cout << " --> " << hostCliente << " conectado na porta " << portaCliente << std::endl;
        } else {
           inet_ntop(AF_INET, &caddr.sin_addr, hostCliente, NI_MAXHOST);    //NumericTOaString: Faz o contrario da inet_pton() que usamos antes
           std::cout << " --> " << hostCliente << " conectado na porta " << ntohs(caddr.sin_port) << std::endl;
        }
        //Limpar o Buffer com seja lá o que havia na memoria anteriormente (ainda mais que estamos em um loop)
        memset(buff, 0, tamanhoBytesDoBuffer);
        

        //Insere os dados(no buff) e Retorna o tamanho desses dados enviados pelo Cliente
        tamanhoBytesDadosRecebidos = recv(socketCliente, buff, tamanhoBytesDoBuffer, 0); //O ultimo argumento define como recv() trabalha para retornar os dados
        if (tamanhoBytesDadosRecebidos == -1) {
           std::cerr << "Erro em receber mensagem. Saindo.." << std::endl;
           break;
        }
        if (tamanhoBytesDadosRecebidos == 0) {
           std::cout << "Cliente desconectado " << std::endl;
           break;
        }
        // Ecoa mensagem de volta para o cliente   [Faz o Inverso do recv()]
        send(socketCliente, buff, tamanhoBytesDadosRecebidos + 1, 0);    //O +1 é porque necessitamos ter um zero no final
        
        
        //'Print' no terminal
        std::cout << std::string(buff, 0, tamanhoBytesDadosRecebidos) << std::endl; //O segundo argumento é a posição inicial que se deve comecar.
        
        
        //FECHAR/destruir o socket
        close(socketCliente);
    }
}

