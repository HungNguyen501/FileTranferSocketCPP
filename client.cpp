#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <cstdio>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>

using namespace std;

#define PORT 1010
#define IP "192.168.1.9" //192.168.1.9    192.168.1.112

SOCKET s;
char buf[1024];
char jsonString[1024] = "{\"name\":\"VangAnh\",\"sex\":\"femal\",\"age\":18}";

void sendFile(SOCKET s, const char *fileName) {
    char bufferSend[1024];
    char header[256]; // header = fileName*fileSize
    char fileSize[256];

    strcpy(header, fileName); // Gan ten file vao header
    strcat(header, "*");    
    
    FILE *fp = fopen(fileName,"rb");
    
    if(fp == NULL)
    {
        printf("Loi mo file.\n");  
        return;
    }   

    // Lay kich co cua file
    fseek(fp, 0, SEEK_END); // Dua con tro den cuoi file
    long long size = ftell(fp); // Lay kich co cua file
    fseek(fp, 0, SEEK_SET); // Dua con tro ve dau file
    sprintf(fileSize, "%lld", size); // Convert tu Long Long sang string
    
    // header = fileName*fileSize
    strcat(header, fileSize);
    strcat(header, "\0");

    // Gui header
    send(s, header , strlen(header), 0);
    sleep(1);

    while(!feof(fp))
    {
        // Doc file
        int len;
        len = fread(bufferSend,sizeof(char), 1023,fp);
        bufferSend[len] = '\0';

        // Gui du lieu toi client
        send(s, bufferSend, len+1, 0);
    }
    
    // Ket thuc gui file
    fclose(fp);
}

void receiveFile(SOCKET soc) {
    char fileName[256];
    char charFileSize[256];
    
    strcpy(fileName, "fromclient_");
    strcpy(charFileSize, "0");
    
    recv(soc, buf, 1024, 0); // Lay header=fileName*charFileSize
    //cout << buf << "***" << strlen(buf) << endl;;
    
    bool check = false;
    for (int i = 0; i < strlen(buf); i++) {
        if (buf[i] == '*') {
            check = true;
            continue;
        }
        if (!check) {
            int length = strlen(fileName);
            fileName[length] = buf[i];
            fileName[length + 1] = '\0';
        }
        else {
            int length = strlen(charFileSize);
            charFileSize[length] = buf[i];
            charFileSize[length + 1] = '\0';
        }
    }
    
    //cout << "file name: " << fileName << endl;
    //cout << "file size: " << charFileSize << endl;
    
    long long fileSize;
    fileSize = atoll(charFileSize); // Convert tu string sang long long
    
    FILE *fp = fopen(fileName, "wb"); 
    if(NULL == fp){
     printf("Tao file bi loi.\n");
    }
    
    int bytesReceived;
    // Nhan du lieu
    while(bytesReceived = recv(soc, buf, 1024, 0))
    {     
        fwrite(buf, 1,bytesReceived-1,fp);       
        if (fileSize == ftell(fp)) // Kiem tra xem da nhan du du lieu hay chua
            break;
    }
    
    // Ket thuc nhan file
    fclose(fp);
}

int main() {
    // Khoi tao thu vien Winsock 2.2
    WSADATA wsa;
    WSAStartup(MAKEWORD(2, 2), &wsa); //set up version 2.2
    
    // Tao socket
    s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    
    // Khai bao dia chi server la IP va cong la PORT
    SOCKADDR_IN serAddr;
    serAddr.sin_family = AF_INET;
    serAddr.sin_addr.s_addr = inet_addr(IP);
    serAddr.sin_port = htons(PORT);
    
    if ( connect(s, (SOCKADDR*) &serAddr, sizeof(serAddr)) != 0) {
        printf("Ket noi that bai...\n");
        return 0;
    }
    
    // Gui mot chuoi json
    /*
    send(s, jsonString, strlen(jsonString), 0);
    */
    
    // Nhan file tu server gui toi   
    receiveFile(s);
    
    // Gui file toi server
    sendFile(s, "t1.txt");
    
    //Gui du lieu
    while(1) {
        printf("Nhap chuoi ky tu: ");
        gets(buf);
        if (strcmp(buf, "exit") == 0) {
            break;
        }
        // Gui xau ky tu sang client
        send(s, buf, strlen(buf), 0);
    }
    
    /*
    // Nhan du lieu tu server
    int ret = recv(client, buf, sizeof(buf), 0);

    if (ret <= 0)
    {
        ret = WSAGetLastError();
        printf("Ket noi bi ngat");
        //system("pause");
        //return 1;
    }

    // Them ky tu ket thuc xau
    if (ret < sizeof(buf)) buf[ret] = 0;
    // In ra man hinh du lieu nhan duoc
    printf("\nReceived: %s\n", buf);
    */
    
    // Ket thuc gui nhan du lieu va dong socket
    closesocket(s);
    
    // Giai phong Winsock
    WSACleanup();
    
    system("pause");
    return 0;
}








