#include "./include/fun.h"
#include <iostream>

char compare(char a, char b)   
{
    char ch;
    if ((a == '+' || a=='-')  && (b=='+' ||b == '-'))
    ch =  '>';
    else if((a == '*' || a=='/') && (b == '/' || b=='*'))
    ch =  '>';
    else if((a == '+' || a == '-') &&( b =='*' || b== '/'))
    ch =  '<';
    else if((a == '/' || a == '*' ) && ( b == '+' || b == '-'))
    ch =  '>';
    else if (b=='#' && a!='#')
    ch =  '>';
    else if (a == '(' && b == ')')
    ch =  '=';
    else if (a == '(' && b != ')')
    ch =  '<';
    else if (a !='(' && b == '(')
    ch =  '<';
    else if (a == '#' && b != '#')
    ch =  '<';
    else if (a =='(' && b ==')')
    ch = '<';
    else if (a == ')' && b ==')')
    ch =  '>';
    else if (a !='(' && b == ')')
    ch = '>';
    return ch;

}
double op(double a, double b ,char c)
{
    double data;
    if (c == '+')
    data =  a+b;
    else if ( c == '-' )
    data =  b-a;
    else if (c == '*')
    data =  a * b;
    else if (c == '/')
    data =  (b / a);
    return data;
}
void help()
{
        printf("input \"quit\" to exit the counter!\n"); 
        printf("input \"expression\" to calculate the expression!\n");
}

void shoutdown(void) {
    printf("I'm looking forward to seeing you next time\n");
}