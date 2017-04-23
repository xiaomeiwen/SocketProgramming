#include <iostream>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
using namespace std;
const int BUF_SIZE = 4000;
const int LINE_SIZE = 30;
const int ROW_SIZE = 100;

int main(int argc, char const **argv){
    int socket_edge = socket(AF_INET, SOCK_STREAM, 0);
    //向服务器（特定的IP和端口）发起请求
    struct sockaddr_in edge_addr;
    memset(&edge_addr, 0, sizeof(edge_addr));  //每个字节都用0填充
    edge_addr.sin_family = AF_INET;  //使用IPv4地址
    edge_addr.sin_addr.s_addr = inet_addr("127.0.0.1");  //具体的IP地址
    edge_addr.sin_port = htons(23251);  //端口
    connect(socket_edge, (struct sockaddr*)&edge_addr, sizeof(edge_addr));
    cout << "The client is up and running."<< endl;
    

    //read file
    FILE *fp = fopen(argv[1], "rb");
    //FILE *fp = fopen("./job.txt", "rb");
    if (fp == NULL){
        printf("Can not open the file\n");
    }else{
        //printf("Open Success.\n");
    }

    char bufSendToServ[BUF_SIZE] = {'\0'};
    int numCount = 0;
    while(!feof(fp)){
        fscanf(fp, "%s\n", &bufSendToServ[numCount * LINE_SIZE]);
        numCount += 1;
    }
    cout << "Read file."<<numCount<< endl;
    //cout << numCount << endl;
    send(socket_edge, &numCount, sizeof(int), 0);
    send(socket_edge, bufSendToServ, numCount * LINE_SIZE, 0);
    cout << "connect socket."<< endl;
    memset(bufSendToServ, 0, BUF_SIZE);
    fclose(fp);
    printf("The client has successfully finished sending %d lines to the edge server.\n",numCount);
    //读取服务器传回的数据
    /*
    
    
    for (int i = 0; i < 60; i++) {
        cout << buffer[i];
     //关闭套接字
    }
    */
    cout << "The client has successfully finished receiving all computation results from the edge server."<< endl;
    cout << "The final results are:" << endl;
    char buffer[BUF_SIZE] =  {'\0'};
    //for (int i = 0; i < 2; i++) {
    recv(socket_edge, buffer, numCount * LINE_SIZE, 0);
    for (int i = 0; i < numCount; i++) {
        for (int j = 0; j < 30; j++) {
            cout << buffer[i * 30 + j] ;
        }
        cout << "" << endl;
    }
    close(socket_edge);
    return 0;
}