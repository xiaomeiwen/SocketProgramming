#include <iostream>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
using namespace std;
const int BUF_SIZE = 4000;
const int LINE_SIZE = 30;
const int ROW_SIZE = 100;

int main(){

	int tcp_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    struct sockaddr_in serv_addr;
    memset(&serv_addr, 0, sizeof(serv_addr));  //每个字节都用0填充
    serv_addr.sin_family = AF_INET;  //使用IPv4地址
    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");  //具体的IP地址
    serv_addr.sin_port = htons(23251);  //端口
    
    
    bind(tcp_sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr));
    
    if (listen(tcp_sock, 50) > -1) {
        printf("The edge sever is up and running\n");
    } else {
        printf("not connect");
    }

    int socket_UDP = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
    struct sockaddr_in server_add;
    memset(&server_add, 0, sizeof(server_add));
    server_add.sin_family = PF_INET;
    server_add.sin_addr.s_addr = inet_addr("127.0.0.1"); //???
    server_add.sin_port = htons(24251);
    int bindUDP = bind(socket_UDP, (struct sockaddr*)&server_add, sizeof(server_add));
	if (bindUDP < 0) {
		cout << "bind UDP error" << endl;
	} else {
		//cout << "edge UDP booting up" << endl;
	}

    while(1) {

    //进入监听状态，等待用户发起请求
    listen(tcp_sock, 20);
    //接收客户端请求
    
    struct sockaddr_in clnt_addr;
    socklen_t clnt_addr_size = sizeof(clnt_addr);
    int clnt_sock = accept(tcp_sock, (struct sockaddr*)&clnt_addr, &clnt_addr_size);
    //cout << clnt_sock << endl;
    int numCount;

    recv(clnt_sock, &numCount, sizeof(int), 0);
    printf("The edge server has received %d lines from the client using TCP over port 23251\n", numCount);
    
    
    int countA = 0, countB = 0;
    string buf[100];
    string bufA[100];
    string bufB[100];
    int flag[100] = {0}; //save A or B flag
	char bufRecvFromClnt[LINE_SIZE] = {'\0'};
    for (int i = 0; i < numCount; i++) {
        recv(clnt_sock, bufRecvFromClnt, LINE_SIZE, 0);
        buf[i] = bufRecvFromClnt;
        if (buf[i].at(0) == 'a') {
            //cout << buf[i] << endl;
            flag[i] = 1;
            bufA[countA] = buf[i];
            countA++;
        }
        if (buf[i].at(0) == 'o') {
            //cout << buf[i] << endl;
            flag[i] = 2;
            bufB[countB] = buf[i];
            countB ++;
        }
    }

    //memset() buf to 0
    memset(buf, 0, sizeof(buf));

    //create the UDP socket with port num 24251
    
	
	///////////////////////////////////////////////////////////////////////////
    //send to AND
    
	int sockAND = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
    struct sockaddr_in servAND_addr;
    struct sockaddr server_AND_addr;
    socklen_t servAND_addr_size = sizeof(server_AND_addr);

    memset(&servAND_addr, 0, sizeof(servAND_addr));
    servAND_addr.sin_family = PF_INET;
    servAND_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    servAND_addr.sin_port = htons(22251);
    
    sendto(sockAND, &countA, sizeof(int), 0, (struct sockaddr*)&servAND_addr, sizeof(servAND_addr));
    
    char sendbufA[BUF_SIZE] = {'\0'};
    for (int i = 0; i < countA; i++) {
        for (int j = 0; j < bufA[i].size(); j++) {
            sendbufA[LINE_SIZE * i + j] = bufA[i].at(j);
           // cout << sendbufA[LINE_SIZE * i + j] << endl;
        }
        
	}
    sendto(sockAND, sendbufA, countA * LINE_SIZE, 0, (struct sockaddr*)&servAND_addr, sizeof(servAND_addr));

    memset(sendbufA, 0, sizeof(sendbufA));
    
    cout << "The edge server has successfully sent "<< countA;
    cout << " lines to Backend-Server AND." << endl;
    ////////////////////////////////////////////////////////////////////////////
	//send to OR
	
	int sockOR = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
    struct sockaddr_in servOR_addr;

    struct sockaddr server_OR_addr;
    socklen_t servOR_addr_size = sizeof(server_OR_addr);

    memset(&servOR_addr, 0, sizeof(servOR_addr));
    servOR_addr.sin_family = PF_INET;
    servOR_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    servOR_addr.sin_port = htons(21251);

	
    sendto(sockOR, &countB, sizeof(int), 0, (struct sockaddr*)&servOR_addr, sizeof(servOR_addr));
    
    char sendbufB[BUF_SIZE] = {'\0'};
    for (int i = 0; i < countB; i++) {
        for (int j = 0; j < bufB[i].size(); j++) {
            sendbufB[LINE_SIZE * i + j] = bufB[i].at(j);
           // cout << sendbufB[LINE_SIZE * i + j] << endl;
        }
        
	}
	sendto(sockOR, sendbufB, countB * LINE_SIZE, 0, (struct sockaddr*)&servOR_addr, sizeof(servOR_addr));

    memset(sendbufB, 0, sizeof(sendbufB));
    
    cout << "The edge server has successfully sent "<< countB;
    cout << " lines to Backend-Server OR." << endl;


    char recbufA[BUF_SIZE] = {'\0'};
    
    
    recvfrom(socket_UDP, recbufA, countA * LINE_SIZE, 0, &server_AND_addr, &servAND_addr_size);

    //cout << sizeof(recbufA)<< endl;
    for (int i = 0; i < countA; i++) {
    	cout << bufA[i] + " = ";
    	for (int j = 0; j < 30; j++) {
    		if (recbufA[i * 30 + j] == '0') {
    			cout << recbufA[i * 30 + j];
    		}
    		if (recbufA[i * 30 + j] == '1') {
    			cout << recbufA[i * 30 + j];
    		}
    		
    	}
    	cout << " "<< endl;
    	//cout << recbufA[i] << endl;
    }	
    
    
    //memset(bufA, 0 , sizeof(bufA));

    
    char recbufB[BUF_SIZE] = {'\0'};
    
    recvfrom(socket_UDP, recbufB, countB * LINE_SIZE, 0, &server_OR_addr, &servOR_addr_size);

    //cout << sizeof(recbufB)<< endl;
    for (int i = 0; i < countB; i++) {
    	cout << bufB[i] + " = ";
    	for (int j = 0; j < 30; j++) {
    		if (recbufB[i * 30 + j] == '0') {
    			cout << recbufB[i * 30 + j];
    		}
    		if (recbufB[i * 30 + j] == '1') {
    			cout << recbufB[i * 30 + j];
    		}
    		
    	}
    	cout << " "<< endl;
    	//cout << recbufA[i] << endl;
    }	

    cout << "The edge server has successfully finished receiving all computation results from the Backend-Server OR and Backend-Server AND." << endl;


    //send(clnt_sock, recbufA, BUF_SIZE, 0);
    
    for (int i = 0, j = 0, k = 0; i < numCount; i++) {
        if (flag[i] == 1) {
            send(clnt_sock, &recbufA[j * 30], 30, 0);
            j++;
            //send(resultA[j]) j++
        } 
        if (flag[i] == 2) {
            send(clnt_sock, &recbufB[k * 30], 30, 0);
            k++;
            //send(resultA[j]) j++
        }
        
    }
    cout << "The edge server has successfully finished sending all computation results to the client." << endl;
    //close(socket_edge);
    //close(clnt_sock);
   // close(tcp_sock);
    //close(socket_UDP);
}
	return 0;
	
}

    