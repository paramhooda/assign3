#include "server.h"
#include "tands.h"
int main(int argc,char* argv[]) {
    double divider = 1000000;
    validateInputServer(argc,argv);

    int fd = socket(AF_INET, // an Internet socket
                    SOCK_STREAM, // reliable stream-like socket
                    0); // OS determine the protocol (TCP)

    if (fd < 0)
        return 1; // something went wrong

    struct sockaddr_in serv_addr;

    int portNum = atoi(argv[1]);

    // setup server address
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portNum); // port
    struct timeval serverWait;
    if (bind(fd, (struct sockaddr*) &serv_addr, sizeof(serv_addr)) < 0) // bind socket to the server address
        return 1;

    std::cout<<"Listening on port" <<portNum<<std::endl;
    if(bind(fd,(struct sockaddr*) &serv_addr,sizeof(serv_addr))<0)
        exit(0);
    if (listen(fd, 25) < 0) // wait for clients, only 1 is allowed.
        return 1;

    char clientId[25][100];
    int clientTrans[25];
    int a=0;
    struct timeval start;
    gettimeofday(&start,NULL);
    int transactions = 0;
    while(1) {

        struct sockaddr_in client_address; // client address
        socklen_t len = sizeof(client_address);
        fd_set setval;
        FD_ZERO(&setval);
        FD_SET(fd,&setval);
        std::cout<<"Waiting for clients...\n";
        int returnFromSelect;
        serverWait.tv_sec=30;
        serverWait.tv_usec=0;
         returnFromSelect = select(fd+1,&setval,NULL,NULL,&serverWait);
         if(returnFromSelect==-1)
             perror("Wait Time Error");
         else if(returnFromSelect==0)
             break;
         else if(returnFromSelect==1) {
            int client_fd = accept(fd, (struct sockaddr *) &client_address, &len);  // accept connection

            if (client_fd < 0) // bad connection
                continue; // discard

            printf("Client connected.\n");
            char buffer[1024];
            int count;
            do {
                char parsedString[3][20];
                count = read(client_fd, buffer, sizeof(buffer));
                if (count > 0) {
                    buffer[count] = '\0';
                    printf("%s\n", buffer);

                    int j = 0;
                    int k = 0;
                    for (int i = 0; i < strlen(buffer); i++) {
                        if (buffer[i] == '_') {
                            j++;
                            k = 0;
                        } else if (buffer[i] == '\0')
                            break;
                        else {
                            parsedString[j][k] = buffer[i];
                            k++;
                        }

                    }
                    int argnum = atoi(parsedString[0]);
                    int totTrans = atoi(parsedString[2]);
                    strcpy(clientId[a], parsedString[1]);
                    clientTrans[a] = totTrans;
                    ++transactions;
                    a++;
                    struct timeval t1;
                    gettimeofday(&t1,NULL);
                    float timet1 = t1.tv_sec+t1.tv_usec/divider;
                    char* taskRec = new char[100];
                    snprintf(taskRec,100,"%.2f # %3d (T%3d) from %s",timet1,transactions,argnum,clientId[a-1]);
                    std::cout<<taskRec<<std::endl;
                    Trans(argnum);
                    char sendToClient[1024];
                    int sendSize;
                    snprintf(sendToClient,1024,"%d",totTrans);
                    int sizeOfSend = strlen(sendToClient);
                    sendSize = write(client_fd,sendToClient,sizeOfSend);

                    if(sendSize<0)
                        perror("Write Error");

                    struct timeval t2;
                    gettimeofday(&t2,NULL);
                    float timet2 = t1.tv_sec+t1.tv_usec/divider;
                    char* taskDone = new char[100];
                    snprintf(taskDone,100,"%.2f # %3d (Done) from %s",timet1,transactions,clientId[a-1]);
                    std::cout<<taskRec<<std::endl;

                }} while (count > 0);
             if(count<0)
                 perror("Read Error");
             close(client_fd);
        }

         printf("Client disconnected.\n");
    }
    struct timeval end;
    gettimeofday(&end,NULL);
    float totalTime = end.tv_sec-start.tv_sec+(end.tv_usec-start.tv_usec)/divider;
    std::cout<<"SUMMARY :/n"<<std::endl;
    for(int i=0;i<a;i++){
        std::cout<<clientTrans[i]<<" transactions from "<<clientId[i];
    }
    float transPerSec = totalTime/transactions;
    std::cout<<transPerSec<<" transactions/sec"<<std::endl;
    return 0;
}

void validateInputServer(int argc,char* argv[]){
    if(argc!=2){
        std::cout<<"Server takes 1 argument Port Number."<<std::endl;
        exit(0);
    }

    if(atoi(argv[1])>64000 || atoi(argv[1])<5000){
        std::cout<<"Port number must be in the range 5000-64000"<<std::endl;
        exit(0);
    }
}