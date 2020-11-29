#include "client.h"
#include "tands.h"
int main(int argc,char* argv[]) {
    validateInputClient(argc,argv);
    int portNum = atoi(argv[1]);
    char ipAddress[20];
    strcpy(ipAddress,argv[2]);
    char hostChar[20];
    char* filename = new char[100];
    std::ofstream f;
    if(gethostname(hostChar,20)<0){
        perror("gethostname error");
        exit(0);
    }
    int hostId = getpid();
    snprintf(filename,100,"%s.%d",hostChar,hostId);

    f.open(filename,std::ios::out);
    f<<"Using Port "<<portNum<<"/nUsing Server "<<ipAddress<<"Host "<<filename<<"/n";

    std::string thisTask;//The task taken from input. Keep taking till EOF is reached
    int transactionCount=0;
    struct timeval t1;
    while ((std::cin>>thisTask)) {

        if (thisTask == "" || thisTask == "\0")  //EOF
            break;
        char *argNumStr = new char[10];    //To store the argument as a char array
        //Iterating through loop to get just the integer part of the input task
        for (int i = 1; thisTask[i] != '\0'; i++) {
            *(argNumStr + i - 1) = *(argNumStr + i - 1) + thisTask[i];
        }

        int argNum = atoi(argNumStr);

        char lineToPrint[1024];
        //Argtype will be either 'S' or 'T'
        char *argType = new char[2];
        *(argType) = thisTask[0];
        if (strcmp(argType, "S") == 0 || strcmp(argType, "s") == 0) {
            Sleep(argNum);
            snprintf(lineToPrint,100,"Sleep %d units/n",argNum);
        }
        else if(strcmp(argType, "T") == 0 || strcmp(argType, "t") == 0){
            int fd = socket(AF_INET, // an Internet socket
                            SOCK_STREAM, // reliable stream-like socket
                            0); // OS determine the protocol (TCP)

            if (fd < 0)
                return 1; // something went wrong

            struct sockaddr_in serv_addr;
            serv_addr.sin_family = AF_INET;
            serv_addr.sin_addr.s_addr = inet_addr(ipAddress);
            serv_addr.sin_port = htons(portNum); // port

            if (connect(fd, (struct sockaddr*) &serv_addr, sizeof(serv_addr)) < 0) // bind socket to the server address
                return 1;

            snprintf(lineToPrint,100,"%d_%s_%d",argNum,filename,++transactionCount);

            int writeLen = strlen(lineToPrint);
            write(fd,lineToPrint,writeLen);
            gettimeofday(&t1,NULL);
            double divider = 1000000;
            float time = t1.tv_sec+(t1.tv_usec)/divider;
            char* writePrint = new char[101];
            snprintf(writePrint,101,"%.2f: Send (T%3d)",time,argNum);
            delete [] writePrint;
            f<<writePrint<<std::endl;

            char readFromServer[1024];
            int readLen = strlen(readFromServer);
            int rd;
            if((rd = read(fd,readFromServer,readLen))<0)
                exit(0);
            readFromServer[rd] = '0/';

            gettimeofday(&t1,NULL);
            time = t1.tv_sec+(t1.tv_usec)/divider;
            char* readPrint = new char[101];
            snprintf(readPrint,101,"%.2f: Recv (D%3s)/n",time,readFromServer);
            delete [] readPrint;
            f<<readPrint<<std::endl;

            close(fd); // close connection

        }}

        f<<"Sent "<<transactionCount<<" transactions"<<std::endl;
        f.close();
        return 0;
    }

void validateInputClient(int argc,char* argv[]){
    if(argc!=3){
        std::cout<<"Client takes 2 arguments Port Number and IP Address"<<std::endl;
        exit(0);
    }

    if(atoi(argv[1])>64000 || atoi(argv[1])<5000){
        std::cout<<"Port number must be in the range 5000-64000"<<std::endl;
        exit(0);
    }
}



