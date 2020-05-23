//
//  main.cpp
//  BMOSocketCpp
//
//  Created by Breno Medeiros on 18/05/20.
//  Copyright © 2020 ProgramasBMO. All rights reserved.
//

#include <unistd.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include <iostream>
#include <netdb.h>
#include <string.h>
#include <string>
 
using namespace std;

int main(int argc, const char * argv[]) {
    // insert code here...
      
      
  //VINCULAR(bind) o socket do SERVIDOR a um endereço+porta
    struct sockaddr_in saddr;                                  //Informações do Servidor [Como Endereço e Porta ultilizadas pelo Cliente]
    saddr.sin_family = AF_INET;                             //Familia de Protocolo(ipv4 ou ipv6)
    saddr.sin_port = htons(5000); /*HostToNetworkShort*/    //Porta padrão para o Airport. Caso n funcione some +1 até que funcione
    
    saddr.sin_addr.s_addr = htonl(INADDR_ANY);              //Endereço na rede local [nesse caso htonl(INADDR_ANY)=definidoPelaRede]
    //inet_pton(AF_INET, "0.0.0.0", &saddr.sin_addr);         //PointerTOaNumber: Insere o atual endereço de rede do SERVIDOR, seja lá qual for
    
    
  //ABRIR/Cria o Socket
    // AF_INET=ipv4      e   AF_INET6=ipv6
    // SOCK_STREAM=TCP   e   SOCK_DGRAM=UDP
    // O terceiro argumento é para se desejassemos especificar algum protocolo
    int socketServer = socket(AF_INET, SOCK_STREAM, 0); //Cria e retorna o Número de socket desse servidor
    if (socketServer == -1) {
        cerr << "Não foi possível criar o Socket! Saindo" << endl;
        return -1;
    }
      
      
  //ACEITAR/Aguardar coneções
    struct sockaddr_in caddr;                                   //Informações do Cliente [Como Endereço e Porta ultilizadas pelo Cliente]
    socklen_t caddrSize = sizeof caddr;                             // Tamanho em bytes das informações do cliente
    int socketCliente;
    // A função bind() serve para alocar um endereço+porta na rede local(ou Socket ao qual deseja associar esse endereço)
    bind(socketServer, (struct sockaddr*)&saddr, sizeof saddr);    //O terceiro argumento(saddr) é o tamnho em bytes do segundo arg.


  //COLOCAR/Avisar ao Winsock que o socket ficará no modo de escuta(listening)
    listen(socketServer, SOMAXCONN);    // O segundo argumento define o numero de conexões permitidas neste socket. Nesse caso = 128
    
    
    // Evitar que o programa se encerre, mantendo o programa em listening
  //ENQUANTO(while) está recebendo mensagem, exibir o eco 'echo mensage' de volta pro cliente
    char buf[4096];
    while (true) {
        // Aceitar conexões feitas, retornando um novo socket em que ha a nova conexao do servidor com o cliente
        socketCliente = accept(socketServer, (struct sockaddr*)&caddr, &caddrSize);
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
}
