#include "./include/scanner.h"
#include "cstring"
#include <iostream>


scanner::scanner(std::string str)
{
    memset(buf,'\0',1024); 
    strcpy(buf,str.c_str()); 
    strcat(buf,"#"); 
    len = strlen(buf);
}

scanner::~scanner()
{ 

}

bool scanner::is_done() 
{
    if(pos == len-1)
    return true;
    return false;    
}

char scanner::scan()
{
    pos++;
    return buf[pos];   
}

