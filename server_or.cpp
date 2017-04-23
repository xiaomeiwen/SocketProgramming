#include <iostream>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <signal.h>
using namespace std;
const int BUF_SIZE = 4000;
const int LINE_SIZE = 30;
const int ROW_SIZE = 100;

string nozero(string s1) {
    int size = (int)s1.size();
    string s;
    int flag = 0;
    for (int i = 0; i < size; i++) {
        //int c1 = s1.at(i) - '0';
        if (flag == 0 && s1.at(i) == '0') {
            continue;
        }
        if (s1.at(i) == '1') {
            flag = 1;
        }
        if (s1.at(i) == '0') {
            s += '0';
        }
        if (s1.at(i) == '1') {
            s += '1';
        }
    }
    if (s.size() == 0) {
        s += '0';
    }
    return s;
}

string reverses (string s1) {
    int size = (int)s1.size();
    string s = "";
    //cout << "s" + s << endl;
    for (int i = size - 1; i >= 0; i--) {
        //int c1 = s1.at(i) - '0';
        if (s1.at(i)  == '0') {
            s += '0';
        }
        if (s1.at(i)  == '1') {
            s += '1';
        }
       // cout << s1.at(i) << endl;
       // cout << "s" + s << endl;
    }
    //cout << "s" + s << endl;
    return s;
}

string doOR(string s1, string s2) {
    s1 = reverses(s1);
    s2 = reverses(s2);
    if (s1.size() < s2.size()) {
        string temp;
        temp = s1;
        s1 = s2;
        s2 = temp;
    }
    for (int i = (int)s2.size(); i < s1.size(); i++) {
        s2 += '0';
    }
    int size = (int)max(s1.size(), s2.size());
    string result;
    
    for (int i = 0; i < size; i ++) {
        int c1 = s1.at(i) - '0';
        int c2 = s2.at(i) - '0';
        if ((c1 | c2) == 0) {
            result += '0';
        }
        if ((c1 | c2) == 1) {
            result += '1';
        }

    }
    result = reverses(result);
    result = nozero(result);
    return result;
}

int main(int argc, const char * argv[]) {
    
	int sockAND = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
    struct sockaddr_in servAND_addr;
    memset(&servAND_addr, 0, sizeof(servAND_addr));
    servAND_addr.sin_family = PF_INET;
    servAND_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    servAND_addr.sin_port = htons(21251);
    bind(sockAND, (struct sockaddr*)&servAND_addr, sizeof(servAND_addr));
    printf("The Server OR is up and running using UDP on port 21251.\n");
    
    struct sockaddr clnt_addr;

    socklen_t clnt_addr_size = sizeof(clnt_addr);
    while (1) {
    int count = 0;
    recvfrom(sockAND, &count, sizeof(int), 0, &clnt_addr, &clnt_addr_size);
    //cout << count << endl;
    cout << "The Server OR start receiving lines from the edge server for OR computation." << endl;
    
    char bufRecvFromClnt[BUF_SIZE] = {'\0'};
    
    	//recv(clnt_sock, bufRecvFromClnt, LINE_SIZE, 0);
    recvfrom(sockAND, bufRecvFromClnt, count * LINE_SIZE, 0, &clnt_addr, &clnt_addr_size);
    cout << "The Server OR start receiving lines from the edge server for OR computation. The computation results are: " << endl;
    
    	//cout << bufRecvFromClnt << endl;
    	//buf[i] = bufRecvFromClnt;

    //cout << sizeof(bufRecvFromClnt) <<endl;

    string buf[100];
    for (int i = 0; i < count; i++) {
    	int j = 0;
    	while (j < 30) {
    		buf[i] += bufRecvFromClnt[i * LINE_SIZE + j];
    		j++;
    	}
    	//cout << buf[i] << endl;
    }
    //string s1
    memset(bufRecvFromClnt, 0, BUF_SIZE);

    string *s1buf = new string[count];
    string *s2buf = new string[count];
    for (int i = 0; i < count; i++) {
    	string s1, s2;
    	int commet = 0;
    	for (int j = 0; j < LINE_SIZE; j++) {
    		if (buf[i][j] == ',') {
    			commet++;
    			//cout << commet;
    			continue;
    		}
    		if (commet == 1) {
    			//int c1 = buf[i][j] - '0';
    			if (buf[i][j] == '0') {
                    s1 += '0';
                }
                if (buf[i][j] == '1') {
                    s1 += '1';
                }
    		}
    		if (commet == 2) {
    			if (buf[i][j] == '0') {
                    s2 += '0';
                }
                if (buf[i][j] == '1') {
                    s2 += '1';
                }
    		}
    	}
    	s1buf[i] = s1;
    	s2buf[i] = s2;
    }

    string *result = new string[count];
    for (int i = 0; i < count; i++) {
    	result[i] = doOR(s1buf[i], s2buf[i]);
        cout << s1buf[i] + " or " + s2buf[i] + " = " + result[i]<< endl;
        
    	//result[i] = "11011";
    }
    cout <<"The Server OR has successfully received "<< count;
    cout <<" lines from the edge server and finished all OR computations." << endl;
    
    memset(s1buf, 0, sizeof(s1buf));
    memset(s1buf, 0, sizeof(s2buf));
    memset(buf, 0, sizeof(buf));

    char bufSendClnt[BUF_SIZE] = {'\0'};

    for (int i = 0; i < count; i++) {
    	//cout << result[i].size() <<endl;
    	for (int j = 0; j < result[i].size(); j++) {
    		bufSendClnt[i * LINE_SIZE + j] = result[i][j];
    		//cout << bufSendClnt[i * LINE_SIZE + j];
    	}
    }
    memset(result, 0, sizeof(result));

    int sock_edge = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
    struct sockaddr_in server_edge_addr;
    server_edge_addr.sin_family = PF_INET;
    server_edge_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    server_edge_addr.sin_port = htons(24251);

    sendto(sock_edge, bufSendClnt, count * LINE_SIZE, 0, (struct sockaddr*)&server_edge_addr, sizeof(server_edge_addr));
    cout << "The Server OR has successfully finished sending all computation results to the edge server." << endl;
    
    //close(sock_edge);
    //close(sockAND);
    }
    return 0;
}