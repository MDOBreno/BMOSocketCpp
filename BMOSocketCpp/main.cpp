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

#include <unistd.h>

#include <sys/socket.h>
#include <iostream>
#include <sstream>


#define PORTA 5000



int main(int argc, const char * argv[]) {
    // insert code here...
    
    
    
    // SERVIDOR
    struct sockaddr_in saddr = {                        //Informações do Servidor [Como Endereço e Porta ultilizadas pelo Cliente]
        .sin_family = AF_INET,                              //Familia de Protocolo(ipv4 ou ipv6)
        
        //inet_pton(AF_INET, "0.0.0.0", &saddr.sin_addr),   //PointerTOaNumber: Insere o atual endereço de rede do SERVIDOR, seja lá qual for
        //.sin_addr.s_addr = htonl(INADDR_ANY),             //Endereço na rede local [nesse caso htonl(INADDR_ANY)=definidoPelaRede]
        .sin_addr.s_addr = INADDR_ANY,                      //Endereço na rede local [nesse caso htonl(INADDR_ANY)=definidoPelaRede]
        
        .sin_port = htons(PORTA)   //HostToNetworkShort      //Porta padrão para o Airport. Caso n funcione some +1 até que funcione
    };
    int opcao = 1;
    int saddrSize = sizeof(saddr);
    // AF_INET=ipv4      e   AF_INET6=ipv6
    // SOCK_STREAM=TCP   e   SOCK_DGRAM=UDP
    // O terceiro argumento é para se desejassemos especificar algum protocolo
    int socketServer = socket(AF_INET, SOCK_STREAM, 0); //Cria e retorna o Número de socket desse servidor
    setsockopt(socketServer, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opcao, sizeof(opcao));
    if (socketServer == -1) {
      std::cerr << "Não foi possível criar o Socket! Saindo" << std::endl;
      return -1;
    }
    
    

    // CLIENTE
    struct sockaddr_in caddr;                           //Informações do Cliente [Como Endereço e Porta ultilizadas pelo Cliente]
    socklen_t caddrSize = sizeof caddr;                 // Tamanho em bytes das informações do cliente
    int socketCliente;                                  // Número de socket do cliente
    
    
    
  //VINCULAR(bind) o socket do SERVIDOR a um endereço+porta
    // A função bind() serve para alocar um endereço+porta da rede a um Socket
    bind(socketServer, (struct sockaddr*)&saddr, sizeof(saddr));    //O terceiro argumento(saddr) é o tamnho em bytes do segundo arg.


    
  //COLOCAR/Avisar ao Winsock que o socket ficará no modo de escuta(listening)
    listen(socketServer, SOMAXCONN);    // O segundo argumento define o numero de conexões permitidas neste socket. Nesse caso = 128
    //Print da escuta
    std::stringstream ss;
    ss << PORTA;
    std::cout << "[Servidor] Escutando na porta " << ss.str() << std::endl;
    
    
    
    char buff[4096];
    int tamanhoBytesDadosRecebidos;
    
  //ENQUANTO(while) está recebendo mensagem, exibir o eco 'echo mensage' de volta pro cliente
    // Evitar que o programa se encerre, mantendo o programa em listening
    while (true) {
        // Aceitar conexões feitas, retornando um novo socket em que ha a nova conexao do servidor com o cliente
        socketCliente = accept(socketServer, (struct sockaddr*)&caddr, (socklen_t*)&caddrSize);
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
        memset(buff, 0, 4096);
        

        //Insere os dados(no buff) e Retorna o tamanho desses dados enviados pelo Cliente
        tamanhoBytesDadosRecebidos = recv(socketCliente, buff, 4096, 0); //O ultimo argumento define como recv() trabalha para retornar os dados
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
    
    
    return 0;
}
