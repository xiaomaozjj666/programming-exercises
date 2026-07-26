#include<stdio.h>
#include<ctype.h>
int main() {
    char str[101];
    fgets(str,101,stdin);
    int u=0,l=0,s=0,d=0,o=0;
    int i=0;
    while(str[i]!='\0'&&str[i]!='\n') {
        if(isupper(str[i])) {
            u++;
        } else if(islower(str[i])) {
            l++;
        } else if(isspace(str[i])) {
            s++;
        } else if(isdigit(str[i])) {
            d++;
        } else {
            o++;
        }
        i++;
    }
    printf("%d %d %d %d %d\n",u,l,s,d,o);
    return 0;
}