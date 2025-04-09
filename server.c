#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <arpa/inet.h>

#define PORT 1111

// 클라이언트 소켓
int client_fd;

// 실시간 메시지 수신을 위해 분리된 쓰레드
void* receve_thread(void* arg) {
    // 메세지 저장 공간
    char buffer[1024];
    while (1) {
        // 버퍼 초기화
        memset(buffer, 0, sizeof(buffer));
        // 클라이언트로 부터 읽은 메세지
        int len = read(client_fd, buffer, sizeof(buffer));
        if (len <= 0) break;

        // 메세지 출력
        printf("Client: %s", buffer);
        // 출력 메세지 즉시 출력 (출력 지연 방지)
        fflush(stdout);
        // 클라이언트 종료
        if (strncmp(buffer, "exit", 4) == 0) break;
    }
    return 0;
}

int main() {
    // 서버 소켓
    int server_fd;
    // 인터넷 주소 구조체
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_size;
    char buffer[1024];

    // IPv4, TCP
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    // 소켓을 주소에 연결
    bind(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr));
    listen(server_fd, 1);

    printf("Wait--\n");

    addr_size = sizeof(client_addr);
    // accept : client_addr에 클라이언트 IP, port 정보 리턴
    client_fd = accept(server_fd, (struct sockaddr*)&client_addr, &addr_size);
    printf("Connected!\n");

    //실시간 메시지 수신 Thread 생성
    pthread_t recv_thread;
    // 쓰레드 ID 저장 주소, 쓰레드 속성, 쓰레드 실행 함수, 함수에 전달할 인자
    pthread_create(&recv_thread, NULL, receve_thread, NULL);

    while (1) {
        fgets(buffer, sizeof(buffer), stdin);
        write(client_fd, buffer, strlen(buffer));
        
        // 종료 로직
        if (strncmp(buffer, "exit", 4) == 0) break;
    }
    
    // 응답 쓰레드 종료시까지 대기.
    pthread_join(recv_thread, NULL);
    // 소켓 종료
    close(client_fd);
    close(server_fd);

    return 0;
}