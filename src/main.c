#include"common.h"
#include"compiler.h"
void testToken(Parser*parser){
    Token token;
    token=nextToken(parser);
    while(token.type!=TOKEN_END){
        if(token.type==TOKEN_WORD || token.type==TOKEN_STRING){
            printf("%d:%s\n",token.type,token.word);
        }else if(token.type==TOKEN_INTEGER){
            printf("%d:%d\n",token.type,token.num);
        }else if(token.type==TOKEN_FLOAT){
            printf("%d:%f\n",token.type,token.numf);
        }else{
            printf("%d\n",token.type);
        }
        token=nextToken(parser);
    }
}
int main(int argc,char**argv){
    if(argc==1){
        puts("Welcome to use Pudron Program Language\nusage:pd [file name]\n");
        return 0;
    }
    bool isLib=false;
    char*fileName=NULL;
    for(int i=1;i<argc;i++){
        if(argv[i][0]=='-'){
            if(argv[i][1]=='l'){
                isLib=true;
            }
        }else{
            if(fileName!=NULL){
                printf("error:can only compile one file.");
                return -1;
            }
            fileName=argv[i];
        }
    }
    run(fileName,isLib);
    return 0;
}