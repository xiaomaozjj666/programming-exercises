#include<stdio.h>
#include<ctype.h>
int main(void) {
    char str[101];
    fgets(str,101,stdin);
    int u=0,l=0,s=0,d=0,o=0;
    int i=0;
    while(str[i]!='\0'&&str[i]!='\n') {
        /* ctype 函数的入参必须是 0~255：char 为有符号时负值是未定义行为，
           教学示例直接示范 (unsigned char) 转换的正确写法 */
        unsigned char c=(unsigned char)str[i];
        if(isupper(c)) {
            u++;
        } else if(islower(c)) {
            l++;
        } else if(isspace(c)) {
            s++;
        } else if(isdigit(c)) {
            d++;
        } else {
            o++;
        }
        i++;
    }
    printf("%d %d %d %d %d\n",u,l,s,d,o);
    return 0;
}
