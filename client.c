#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <arpa/inet.h>

#define PORT 1111
// localhost
#define SERVER_IP "127.0.0.1"

// 서버 소켓
int sock_fd;

// 실시간 수신 스레드 함수
void* receive_thread(void* arg) {
    char buffer[1024];
    while (1) {
        memset(buffer, 0, sizeof(buffer));
        int len = read(sock_fd, buffer, sizeof(buffer));
        if (len <= 0) break;

        printf(buffer, "\n");
        fflush(stdout);

        if (strncmp(buffer, "exit", 4) == 0) break;
    }
    return NULL;
}

int main() {
    struct sockaddr_in server_addr;
    char buffer[1024];

    // 소켓 생성 (IPv4, TCP)
    sock_fd = socket(AF_INET, SOCK_STREAM, 0);

    // 서버 주소 설정
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    // IP주소를 네트워크 이진값으로 변환 후 저장
    inet_pton(AF_INET, SERVER_IP, &server_addr.sin_addr);

    // 서버에 연결
    if (connect(sock_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        // 연결 실패 시 에러 출력
        perror("Connection Failed");
        exit(1);
    }

    printf("Connected\n");

    // 수신용 스레드 생성
    pthread_t recv_thread;
    pthread_create(&recv_thread, NULL, receive_thread, NULL);

    while (1) {
        prinf("Client: ");
        fgets(buffer, sizeof(buffer), stdin);
        write(sock_fd, buffer, strlen(buffer));

        if (strncmp(buffer, "exit", 4) == 0) break;
    }

    // 스레드 종료 대기
    pthread_join(recv_thread, NULL);
    close(sock_fd);
    return 0;
}   