#include "other/Print_.h"
void Printf_green(std::string str){
    std::string str1;
    str1="\033[32m"+str+"\033[0m";
    std::cout<<str1<<std::endl;
}

void Printf_red(std::string str){
    std::string str1;
    str1="\033[31m"+str+"\033[0m";
    std::cout<<str1<<std::endl;
}