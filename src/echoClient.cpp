#include <stdlib.h>
#include <boost/locale.hpp>
#include <bits/stl_bvector.h>
#include "../include/ConnectionHandler.h"
#include <boost/thread.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
#include <queue>
using namespace std;

enum pendingRequest{NONE,NICK,JOIN,MSG,LISTGAMES,STARTGAME,TXTRESP,SELECTRESP,ASKCHOICES,ASKTXT,SYSMSG,GAMEMSG,USRMSG,QUIT};


string enumToString(pendingRequest p){
    switch(p){
        case NONE:
            return "NONE";
        case NICK:
            return "NICK";
        case JOIN:
            return "JOIN";
        case LISTGAMES:
            return "LISTGAMES";
        case TXTRESP:
            return "TXTRESP";
        case SELECTRESP:
            return "SELECTRESP";
        case MSG:
            return "MSG";
        case STARTGAME:
            return "STARTGAME";
        case ASKCHOICES:
            return "ASKCHOICES";
        case ASKTXT:
            return "ASKTXT";
        case SYSMSG:
            return "SYSMSG";
        case USRMSG:
            return "USRMSG";
        case GAMEMSG:
            return "GAMEMSG";
        case QUIT:
            return "QUIT";
    }
    return "";
}
pendingRequest stringToEnum(string s){
    for (int i = NONE; i <= QUIT; i++) {
        if(!enumToString(static_cast<pendingRequest >(i)).compare(s))
        {
            return static_cast<pendingRequest >(i);
        }
    }
    return NONE;
}


queue<pendingRequest> currentPendingRequests;
int isNotTerminated=1;
ConnectionHandler *connectionHandlerPtr;


void handleInput(){
    string line;
    pendingRequest currentPendingRequest;
    while (isNotTerminated){
        if(currentPendingRequests.empty())
            continue;
        currentPendingRequest=currentPendingRequests.front();
        currentPendingRequests.pop();
        switch(currentPendingRequest){
            case NICK:
                cout<<"Enter your name"<<endl;
                cin>>line;
                line=enumToString(currentPendingRequest)+" "+line;
                connectionHandlerPtr->sendLine(line);
                break;

            case JOIN:
                cout<<"Enter the room you want to join"<<endl;
                cin>>line;
                line=enumToString(currentPendingRequest)+" "+line;
                connectionHandlerPtr->sendLine(line);
                break;

            case LISTGAMES:
                cout<<"Would you like to see the list of available games?(y/n)"<<endl;
                cin>>line;
                if(!line.compare("y"))
                {
                    line=enumToString(currentPendingRequest);
                    connectionHandlerPtr->sendLine(line);
                }
                else
                {
                    currentPendingRequests.push(STARTGAME);
                }
                break;
            case TXTRESP:
                cout<<"Enter your response"<<endl;
                cin>>line;
                line=enumToString(currentPendingRequest)+" "+line;
                connectionHandlerPtr->sendLine(line);
                break;
            case SELECTRESP:
                cout<<"Enter your selection"<<endl;
                cin>>line;
                line=enumToString(currentPendingRequest)+" "+line;
                connectionHandlerPtr->sendLine(line);
                break;
            case MSG:
                cout<<"Enter your message"<<endl;
                cin>>line;
                line=enumToString(currentPendingRequest)+" "+line;
                connectionHandlerPtr->sendLine(line);
                break;
            case STARTGAME:
                cout<<"What game whould you like to play?"<<endl;
                cin>>line;
                line=enumToString(currentPendingRequest)+" "+line;
                connectionHandlerPtr->sendLine(line);
                break;
            case QUIT:
                cout<<"Would you like to quit the game?(y/n)"<<endl;
                cin>>line;
                if(!line.compare("y"))
                {
                    line=enumToString(currentPendingRequest);
                    connectionHandlerPtr->sendLine(line);
                    isNotTerminated=0;
                    connectionHandlerPtr->close();
                }
                else{
                    currentPendingRequests.push(JOIN);
                }
                break;
            default:
                break;
        }
        currentPendingRequest=NONE;
    }

}

void handleSocket(){
    while (isNotTerminated){
        std::string answer;
        // Get back an answer: by using the expected number of bytes (len bytes + newline delimiter)
        // We could also use: connectionHandler.getline(answer) and then get the answer without the newline char at the end
        if (!connectionHandlerPtr->getLine(answer)) {
            cout << "Disconnected. Exiting...\n" << endl;
            isNotTerminated=0;
            while (!currentPendingRequests.empty())
            {
                currentPendingRequests.pop();
            }
            connectionHandlerPtr->close();
            break;
        }
        std::size_t pos = answer.find(" ");
        string commandStr=answer.substr(0,pos);
        pendingRequest command=stringToEnum(commandStr);
        string param=answer.substr(1+pos);
        if (command==QUIT){
            cout<<"Quiting..."<<endl;
            isNotTerminated=0;
            connectionHandlerPtr->close();
        }
        if(command==ASKTXT){
            cout<<"You were asked :"<<param<<endl;
            currentPendingRequests.push(TXTRESP);
        }
        if(command==ASKCHOICES){
            cout<<"You were asked :"<<param<<endl;
            currentPendingRequests.push(SELECTRESP);
        }
        if(command==SYSMSG){

            std::size_t pos = param.find(" ");
            string originalCommandStr=param.substr(0,pos);
            cout<<"result for "<<originalCommandStr<<": "<<param.substr(pos+1)<<endl;
            pendingRequest originalCommand=stringToEnum(originalCommandStr);
            int wasRejected=boost::starts_with(param.substr(pos+1),"REJECTED");
            if(wasRejected){
                currentPendingRequests.push(originalCommand);
            }
            else
            {
                switch (originalCommand){
                    case NICK:
                        currentPendingRequests.push(JOIN);
                        break;
                    case JOIN:
                        currentPendingRequests.push(LISTGAMES);
                        break;
                    case LISTGAMES:
                        currentPendingRequests.push(STARTGAME);
                        break;
                    default:
                        break;
                }
            }

        }
        if(command==GAMEMSG){
            cout<<"You got game message :"<<param<<endl;
            if(param.find("won")!= std::string::npos||param.find("lose")!= std::string::npos)
                currentPendingRequests.push(QUIT);
        }
        if(command==USRMSG) {
            cout << "You were message from user :" << param << endl;
            currentPendingRequests.push(TXTRESP);
        }
    }
}

int main (int argc, char *argv[]) {
    if (argc < 3) {
        std::cerr << "Usage: " << argv[0] << " host port" << std::endl << std::endl;
        return -1;
    }
    std::string host = argv[1];
    unsigned short port = atoi(argv[2]);

    ConnectionHandler connectionHandler(host, port);
    connectionHandlerPtr=&connectionHandler;
    if (!connectionHandler.connect()) {
        std::cerr << "Cannot connect to " << host << ":" << port << std::endl;
        return 1;
    }

    boost::thread thread_1(handleInput);
    boost::thread thread_2(handleSocket);
    currentPendingRequests.push(NICK);
    thread_1.join();
    thread_2.join();


    return 0;
}