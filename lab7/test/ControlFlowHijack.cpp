#include <cstdio>
#include <cstring>
#include <cstdlib>

struct Memory{
	char buffer[8]; 
	int data = 0; 
};

bool check_secret(int secret){
	return secret == 97;
}

int main() {
    Memory mem;
    int secret_value=0;

    printf("input:");
    
    //To simulate unsafe gets
    char* ptr = mem.buffer;
    int ch;
    while ((ch = getchar()) != '\n') {
        *ptr++ = ch; 
    }
    *ptr = '\0';
    
    secret_value=mem.data+32;
    if (check_secret(secret_value)) {
        printf("You've discovered the secret value!\n");
    } else {
        printf("Secret value: %d\n", secret_value);
    }
}

