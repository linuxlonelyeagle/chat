#include "./include/include.h"
int main(void) {
    signal(SIGINT,SIG_IGN);
    atexit(shoutdown);
    std::stack<double> stack_num;
    std::stack<char> stack_ch;
    std::string str;
    std::cout << "you can input \"help\" to get help" << std::endl;
    std::cout << "$:" << std::ends;
    std::cin >> str;
    while (str != std::string("quit")) {
        if (std::string("help") == str) {
            help();
            std::cout << "$:" << std::ends;
            std::cin >> str;
            continue;
        }
        scanner scan(str); 
        stack_ch.push('#');
        char ch = scan.scan();
        while (ch != '#' || stack_ch.top() != '#') {
            std::string token;
            if (isdigit(ch)) {
                    token.push_back(ch);
                    if (!scan.is_done()) {
                    ch = scan.scan();
                } 
                while (isdigit(ch) || ch == '.')  {
                    token.push_back(ch);
                    ch = scan.scan();
                }
                stack_num.push(std::stod(token));
            } else  if (is_op(ch)) {
                    switch(compare(stack_ch.top(),ch)) {
                    case '<':
                    stack_ch.push(ch);
                    ch = scan.scan();
                    break;
                    case '=':
                    stack_ch.pop();
                    ch = scan.scan();
                    break;
                    case '>':
                    char  data = stack_ch.top();
                    stack_ch.pop();
                    double pt1 = stack_num.top();
                    stack_num.pop();
                    double pt2 = stack_num.top();
                    stack_num.pop();
                    pt1 = op(pt1,pt2,data);
                    stack_num.push(pt1);
                    break;  
                } 
            } 
        }
        double data = stack_num.top();
        std::cout << "result: " << data << std::endl;
        std::cout << "$:" << std::ends;
        std::cin >> str;
    } return 0;    
}
    
  
