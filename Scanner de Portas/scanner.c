#include <stdio.h>
#include <string.h>

#ifdef _WIN32
    #include <winsock2.h>
    #include <ws2tcpip.h>
    #pragma comment(lib, "ws2_32.lib")
#else
    #include <sys/socket.h>
    #include <arpa/inet.h>
    #include <netdb.h>
    #include <unistd.h>
    #define INVALID_SOCKET -1
    #define SOCKET_ERROR -1
    #define closesocket close
    typedef int SOCKET;
#endif

// Função para escanear uma porta específica
int scan_port(const char *ip, int port) {
    SOCKET sock;
    struct sockaddr_in server;

    // Cria um socket
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == INVALID_SOCKET) {
        printf("Não foi possível criar o socket\n");
        return 0;
    }

    // Configurações do servidor
    server.sin_addr.s_addr = inet_addr(ip);
    server.sin_family = AF_INET;
    server.sin_port = htons(port);

    // Tenta conectar na porta
    int connect_status = connect(sock, (struct sockaddr *)&server, sizeof(server));
    if (connect_status == 0) {
        // Porta está aberta
        closesocket(sock);  // Fecha o socket após a conexão bem-sucedida
        return 1;  // Retorna 1 se a porta estiver aberta
    } else {
        // Porta está fechada
        printf("Houve um erro ao se conectar na porta %d: %d\n", port, WSAGetLastError);
        closesocket(sock);
        return 0;  // Retorna 0 se a porta estiver fechada
    }
}

int main() {
    char host[100];
    int initial_port, final_port, port;
    struct hostent *he;
    struct in_addr **addr_list;

    #ifdef _WIN32
        WSADATA wsa;
        // Inicializa o Winsock
        if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
            printf("Falha ao inicializar o Winsock. Código de erro: %d\n", WSAGetLastError());
            return 1;
        }
        printf("Winsock inicializado.\n");
    #endif

    // Entrada do usuário
    printf("Digite o endereço IP ou o domínio a ser escaneado: ");
    scanf("%s", host);

    // Resolve o nome do host para obter o IP
    if ((he = gethostbyname(host)) == NULL) {
        printf("Erro ao resolver o nome do host\n");
        #ifdef _WIN32
            WSACleanup();
        #endif
        return 1;
    }

    addr_list = (struct in_addr **)he->h_addr_list;
    char *ip = inet_ntoa(*addr_list[0]);
    printf("Escaneando IP: %s\n", ip);

    // Intervalo de portas para escanear
    printf("Digite a porta inicial: ");
    scanf("%d", &initial_port);
    printf("Digite a porta final: ");
    scanf("%d", &final_port);

    // Escaneia portas no intervalo especificado
    for (port = initial_port; port <= final_port; port++) {
        if (scan_port(ip, port)) {
            printf("Porta %d está aberta\n", port);
        } else {
            printf("Porta %d está fechada\n", port);
        }
    }

    #ifdef _WIN32
        // Finaliza o Winsock
        WSACleanup();
    #endif

    return 0;
}
