#include <stdio.h>

/*           int strcmp(cs,ct) - compare string cs to string ct; return <0 if cs<ct , 0 if cs==ct and >0 if cs>ct.                  */
/*           int strncmp(cs,ct,n) - compare at most n charactets of string cs to string ct; return <0 if cs<ct , 0 if cs==ct and >0 if cs>ct.         */
/*           char strchr(cs,c) - return pointer to first occurrence of c in cs or NULL if not present                         */

char s1[80],s2[80];

int main(){
    printf("Please enter the first word:");
    scanf("%79s", s1);
    printf("Please enter the second word:");
    scanf("%79s", s2);
    my_strcmp(s1,s2);
    my_strsncmp(s1,s2,1);
    my_strchr(s1,s2);

}


int my_strcmp(cs,ct){
/* The goal is to run a loop and to compare cs[i] to ct[i] with ascii and return the value */
int i=0,diff=0;
int ascii_cs,ascii_ct;


ascii_cs = ascii_ct = 0;

 
}
int my_strsncmp(cs,ct,n){

}
char my_strchr(cs,c){
int MAX_INPUT=80;
int my_strchr=-1,i=0; /*this will be the returned result*/

}

