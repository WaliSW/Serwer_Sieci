#include <iostream>
#include <sys/types.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <string>
#include <string.h>
#include <stdio.h>
#include <netinet/in.h>

using namespace std;

int main(int argc, char const *argv[])
{
    // Stwórz socket
    int listening = socket(AF_INET, SOCK_STREAM, 0);
    if(listening == -1)
    {
        cerr << "Nie można utworzyć gniazda";
        return -1;
    }
    // Zbinduj socket do ip / portu
    sockaddr_in hint;
    hint.sin_family = AF_INET;
    hint.sin_port = htons(54000);
    inet_pton(AF_INET,"192.168.1.100", &hint.sin_addr);

    if(bind(listening, (sockaddr*)&hint, sizeof(hint)))
    {
        cout << "Nie mozna zbindować ";
        return -2;
    }
    // Nasłuchiwanie
    if(listen(listening, SOMAXCONN) == -1)
    {
        cout << "Nie można słuchać";
        return -3;
    }
    sockaddr_in client;
    socklen_t clientSize = sizeof(client);
    char host[NI_MAXHOST];
    char svc[NI_MAXSERV];

    int clientSocket = accept(listening, (sockaddr*)&client, &clientSize);

    if (clientSocket == -1)
    {
        cout << "Problem z połączeniem klienta";
        return -4;
    }

    //Zamknięcie nasłuchiwania
    close(listening);

    memset(host, 0, NI_MAXHOST);
    memset(svc, 0, NI_MAXSERV);

    int result = getnameinfo((sockaddr*)&client,sizeof(client),host,NI_MAXHOST,svc,NI_MAXSERV,0);

    if(result)
    {
        cout << host << "connected on" << svc << endl;
    }else
    {
        inet_ntop(AF_INET,&client.sin_addr, host, NI_MAXHOST);
        cout << host << "connected on" << ntohs(client.sin_port);
    }

    
    // while odbiera, wyświetl wiadomość, echo wiadomość
    char buf[4096];
    while(true)
    {
        //wyczyszczenie buffera
        memset(buf,0,4096);
        int bytesRecv = recv(clientSocket, buf, 4096,0);
        if(bytesRecv == -1)
        {
            cout << "Problem z łącznością" <<endl;
            break;
        }

        if(bytesRecv == 0)
        {
            cout<<"Klient odłączył się" << endl;
            break;
        }
        //wyświetl wiadomość
        cout << "Odebrane: "<<string(buf,0,bytesRecv) << endl;
        //wyślij wiadomość
        send(clientSocket, buf, bytesRecv + 1 , 0);
    }

    // zamknięcie socketu
    close(clientSocket);
}
