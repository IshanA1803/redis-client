#include<iostream>

int main(int argc,char* argv[]){
    std::string host="127.0.0.1"; //default host
    int port=6379; //default port

    int i=1; //start from 1 to skip program name
    while(i<argc){
        std::string arg=argv[i];
        if(arg=="-h" && i+1<argc){
            host=argv[++i];
        }else if(arg=="-p" && i+1<argc){
            port=std::stoi(argv[++i]);
        }else{
            break; //stop parsing on unknown argument
        }
        i++;
    }
}