#include <stdio.h>

/*           int strcmp(cs,ct) - compare string cs to string ct; return <0 if cs<ct , 0 if cs==ct and >0 if cs>ct.                  */
/*           int strncmp(cs,ct,n) - compare at most n charactets of string cs to string ct; return <0 if cs<ct , 0 if cs==ct and >0 if cs>ct.         */
/*           char strchr(cs,c) - return pointer to first occurrence of c in cs or NULL if not present                        */




int my_strcmp(char* cs,char* ct){
    int i;
    for (i=0; i<80; i++){
        if(cs[i]>ct[i]){
            return 1;
        }
        else if (cs[i]<ct[i]){
            return -1;
        }
    }
    return 0;
}
int my_strncmp(char* cs,char* ct,int n){
    int i;
        for (i=0; i<80 && i<n; i++){
            if(cs[i]>ct[i]){
                return 1;
            }
            else if (cs[i]<ct[i]){
                return -1;
            }
    }
    return 0;
}

int main(){
    char s1[80]={0},s2[80]={0};
    int menu,ans;
    printf("Please choose what you would like to do: \n1.strcmp\n2.strncmp\n3.strchr\n");
    ans = scanf("%d",&menu);
    while(ans!=EOF){
        if(menu==1){
            printf("Please enter the first word:\n");
            scanf("%79s", s1);
            printf("Please enter the second word:\n");
            scanf("%79s", s2);
            printf("The result of strcmp on %s and %s is %d.\n",s1,s2,my_strcmp(s1,s2));
        }
        if(menu==2){
            char num;
            printf("Please enter the first word:\n");
            scanf("%79s", s1);
            printf("Please enter the second word:\n");
            scanf("%79s", s2);
            printf("Please enter a number:\n");
            scanf("%d", &num);
            printf("The result of strncmp of %s and %s and %d is %d:\n",s1,s2,num,my_strncmp(s1,s2,num));
        }
    }
    return 0;
}