#include <iostream>

class scanner{
public:
    scanner(std::string str);
    ~scanner();
    char scan();
    bool is_done();
private:    
    char buf[1024];
    int  pos = -1;
    int len;
};