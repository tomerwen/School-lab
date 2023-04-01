#include <stdio.h>

/*           int strcmp(cs,ct) - compare string cs to string ct; return <0 if cs<ct , 0 if cs==ct and >0 if cs>ct.                  */
/*           int strncmp(cs,ct,n) - compare at most n charactets of string cs to string ct; return <0 if cs<ct , 0 if cs==ct and >0 if cs>ct.         */
/*           char strchr(cs,c) - return pointer to first occurrence of c in cs or NULL if not present                        */

char s1[80]={0},s2[80]={0};

int main(){
    char menu;
    printf("Please choose what you would like to do: \n1.strcmp\n2.strncmp\n3.strchr");
    scanf("%1s",menu);
    if(menu==1){
        printf("Please enter the first word:\n");
        scanf("%79s", s1);
        printf("Please enter the second word:\n");
        scanf("%79s", s2);
        printf("The result of strcmp on %c and %c is %c.\n",s1,s2,my_strchr(s1,s2));
    }
    if(menu==2){
        char num;
        printf("Please enter the first word:\n");
        scanf("%79s", s1);
        printf("Please enter the second word:\n");
        scanf("%79s", s2);
        printf("Please enter a number:\n");
        scanf("%79s",num);
        printf("The result of strncmp of %c amd %c and %c is %c:\n",s1,s2,num,my_strncmp(s1,s2,num));
    }
    if(menu==3){
        char chr;
        printf("Please enter a word\n");
        scanf("%79s",s1);
        printf("Please enter a char");
        scanf("%79s",chr);
        printf("The result of strchr of %c with %c is %c",s1,chr,my_strchr(s1,chr));
    }

}


int my_strcmp(char* cs,char* ct){
    int i=0,diff=0;
    for (i; i<80; i++){
        if(cs[i]>ct[i]){
            return 1;
        }
        else if (cs[i]<ct[i]){
            return -1;
        }
    }
    return 0;
}

int my_strncmp(cs,ct,n){

}
char my_strchr(cs,c){
int MAX_INPUT=80;
int my_strchr=-1,i=0; /*this will be the returned result*/

}

