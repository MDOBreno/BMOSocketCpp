//
//  main.cpp
//  BMOSocketCpp
//
//  Created by Breno Medeiros on 18/05/20.
//  Copyright © 2020 ProgramasBMO. All rights reserved.
//


#include <sys/types.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <string.h>
#include <string>

#include <stdio.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

#include <CoreFoundation/CoreFoundation.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <iostream>
#include <sstream>

using namespace std;
 
#define PORT 37716

class Communications{
    public:
        int socketServer, socketCliente, valread;
        struct sockaddr_in saddr;
        int opcao = 1;
        int saddrSize = sizeof(saddr);
        char buffer[1024] = {0};

    Communications(){
        socketServer = socket(AF_INET, SOCK_STREAM, 0);
        setsockopt(socketServer, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opcao, sizeof(opcao));

        saddr.sin_family = AF_INET;
        saddr.sin_addr.s_addr = INADDR_ANY;
        saddr.sin_port = htons(PORT);

        bind(socketServer, (struct sockaddr *)&saddr, sizeof(saddr));
        listen(socketServer, 3);
        
        std::stringstream ss;
        ss << PORT;
        std::cout << "[Server] Listening in port " << ss.str() << std::endl;

        socketCliente = accept(socketServer, (struct sockaddr *)&saddr, (socklen_t*)&saddrSize);
        std::cout << "[Server] Client has successfully connected." << std::endl;
    }
};


int main(int argc, const char * argv[]) {
    // insert code here...
    Communications CommunicationsObj = Communications();
    return 0;
}
/*
  //ABRIR/Criar o Socket
    struct sockaddr_in saddr;                                  //Informações do Servidor [Como Endereço e Porta ultilizadas pelo Cliente]
    int opcao = 1;
    int saddrSize = sizeof(saddr);
    // AF_INET=ipv4      e   AF_INET6=ipv6
    // SOCK_STREAM=TCP   e   SOCK_DGRAM=UDP
    // O terceiro argumento é para se desejassemos especificar algum protocolo
    int socketServer = socket(AF_INET, SOCK_STREAM, 0); //Cria e retorna o Número de socket desse servidor
    setsockopt(socketServer, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opcao, sizeof(opcao));
    if (socketServer == -1) {
      cerr << "Não foi possível criar o Socket! Saindo" << endl;
      return -1;
    }
    
    
  //VINCULAR(bind) o socket do SERVIDOR a um endereço+porta
    saddr.sin_family = AF_INET;                             //Familia de Protocolo(ipv4 ou ipv6)
    
    //inet_pton(AF_INET, "0.0.0.0", &saddr.sin_addr);         //PointerTOaNumber: Insere o atual endereço de rede do SERVIDOR, seja lá qual for
    //saddr.sin_addr.s_addr = htonl(INADDR_ANY);              //Endereço na rede local [nesse caso htonl(INADDR_ANY)=definidoPelaRede]
    saddr.sin_addr.s_addr = INADDR_ANY;                       //Endereço na rede local [nesse caso htonl(INADDR_ANY)=definidoPelaRede]
    
    saddr.sin_port = htons(PORT);   //HostToNetworkShort      //Porta padrão para o Airport. Caso n funcione some +1 até que funcione
      
      
  //ACEITAR/Aguardar coneções
    struct sockaddr_in caddr;                                   //Informações do Cliente [Como Endereço e Porta ultilizadas pelo Cliente]
    socklen_t caddrSize = sizeof caddr;                             // Tamanho em bytes das informações do cliente
    int socketCliente;
    // A função bind() serve para alocar um endereço+porta na rede local(ou Socket ao qual deseja associar esse endereço)
    bind(socketServer, (struct sockaddr*)&saddr, sizeof(saddr));    //O terceiro argumento(saddr) é o tamnho em bytes do segundo arg.


  //COLOCAR/Avisar ao Winsock que o socket ficará no modo de escuta(listening)
    listen(socketServer, 3); //..,SOMAXCONN);    // O segundo argumento define o numero de conexões permitidas neste socket. Nesse caso = 128
    std::stringstream ss;
    ss << PORT;
    std::cout << "[Server] Listening in port " << ss.str() << std::endl;

    socketCliente = accept(socketServer, (struct sockaddr*)&saddr, (socklen_t*)&saddrSize);
    std::cout << "[Server] Client has successfully connected." << std::endl;
    
    // Evitar que o programa se encerre, mantendo o programa em listening
  //ENQUANTO(while) está recebendo mensagem, exibir o eco 'echo mensage' de volta pro cliente
    char buf[4096];
    while (true) {
        // Aceitar conexões feitas, retornando um novo socket em que ha a nova conexao do servidor com o cliente
        socketCliente = accept(socketServer, (struct sockaddr*)&saddr, (socklen_t*)&saddrSize);
        std::cout << "[Server] Client has successfully connected." << std::endl;
        char hostCliente[NI_MAXHOST];               // Host: Nome/Endereço remoto do cliente.           Neste caso com tamanho = 1025
        char portaCliente[NI_MAXSERV];              // Serviço: Porta ao qual o cliente está conectado. Neste caso com tamanho = 32

        memset(hostCliente, 0, NI_MAXHOST); // Como não sabemos o que havia na memoria antes da instacia do 'endereço', botamos zeros na memoria
        memset(portaCliente, 0, NI_MAXSERV);
        //Tentando abaixo conseguir o nome da maquina do cliente. Caso não consiga(else) então trapaciamos! :D
        if (getnameinfo((sockaddr*)&caddr, sizeof(caddr), hostCliente, NI_MAXHOST, portaCliente, NI_MAXSERV, 0) == 0) {
           cout << hostCliente << " conectado na porta " << portaCliente << endl;
        } else {
           inet_ntop(AF_INET, &caddr.sin_addr, hostCliente, NI_MAXHOST);    //NumericTOaString: Faz o contrario da inet_pton() que usamos antes
           cout << hostCliente << " conectado na porta " << ntohs(caddr.sin_port) << endl;
        }
        
        //Limpar o Buffer com seja lá o que havia na memoria anteriormente (ainda mais que estamos em um loop)
        memset(buf, 0, 4096);

        // Aguardando o cliente enviar a mensagem
        int tamanhoBytesDadosRecebidos = recv(socketCliente, buf, 4096, 0); //O ultimo argumento define como recv() trabalha para retornar os dados
        if (tamanhoBytesDadosRecebidos == -1) {
           cerr << "Erro em receber mensagem. Saindo.." << endl;
           break;
        }
        if (tamanhoBytesDadosRecebidos == 0) {
           cout << "Cliente desconectado " << endl;
           break;
        }
        cout << string(buf, 0, tamanhoBytesDadosRecebidos) << endl; //O segundo argumento representa a posição inicial que se deve iniciar.

        // Ecoa mensagem de volta para o cliente   [Faz o Inverso do recv()]
        send(socketCliente, buf, tamanhoBytesDadosRecebidos + 1, 0);    //O +1 é porque necessitamos ter um zero no final
        
      //FECHAR/destruir o socket
        close(socketCliente);
    }
    
    
    return 0;
}*/
