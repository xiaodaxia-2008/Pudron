#include"parse.h"
Token nextToken(Parser*parser){
    char msgt[50];
    Token token;
    char c=parser->code[parser->ptr];
    while(c==' ' || c=='	' || c=='\n' || c=='\''){
        if(c=='\n'){
            parser->line++;
        }
        if(c=='\''){
            /*跳过注释*/
            c=parser->code[++parser->ptr];
            while(c!='\''){
                if(c=='\0'){
                    reportError(parser,"unterminated comment.");
                }
                if(c=='\n'){
                    parser->line++;
                }
                c=parser->code[++parser->ptr];
            }
            c=parser->code[++parser->ptr];
        }
        c=parser->code[++parser->ptr];
    }
    if(c=='\0'){
        token.type=TOKEN_END;
        return token;
    }
    if(c>='0' && c<='9'){
        int num=0,dat=0;
        while(c>='0' && c<='9'){
            num=num*10+c-'0';
            c=parser->code[++parser->ptr];
        }
        token.type=TOKEN_INTEGER;
        token.num=num;
        if(c=='.'){
            c=parser->code[++parser->ptr];
            while(c>='0' && c<='9'){
                dat++;
                num=num*10+c-'0';
                c=parser->code[++parser->ptr];
            }
            if(dat==0){
                sprintf(msgt,"\n    %d.\nexpected decimal.",num);
                reportError(parser,msgt);
            }
            if(dat>7){
                reportError(parser,"the length of the decimal is too long.");
            }
            token.type=TOKEN_FLOAT;
            token.num=(num&0x1FFFFFFF)|(dat<<29);
        }
        return token;
    }else if((c>='a' && c<='z') || (c>='A' && c<='Z') || c=='_' || (c>='0' && c<='9')){
        int i=0;
        for(i=0;(c>='a' && c<='z') || (c>='A' && c<='Z') || c=='_' || (c>='0' && c<='9');i++){
            if(i>=WORD_MAX-1){
                token.word[i]='\0';
                token.type=TOKEN_UNKNOWN;
                sprintf(msgt,"\n    %s...\ntoo long word.",token.word);
                reportError(parser,msgt);
            }
            token.word[i]=c;
            c=parser->code[++parser->ptr];
        }
        token.word[i]='\0';
        if(strcmp(token.word,"int")==0){
            token.type=TOKEN_INT;
        }else if(strcmp(token.word,"float")==0){
            token.type=TOKEN_FLOAT_CLASS;
        }else if(strcmp(token.word,"func")==0){
            token.type=TOKEN_FUNC;
        }else if(strcmp(token.word,"while")==0){
            token.type=TOKEN_WHILE;
        }else if(strcmp(token.word,"if")==0){
            token.type=TOKEN_IF;
        }else if(strcmp(token.word,"elif")==0){
            token.type=TOKEN_ELIF;
        }else if(strcmp(token.word,"else")==0){
            token.type=TOKEN_ELSE;
        }else if(strcmp(token.word,"and")==0){
            token.type=TOKEN_CAND;
        }else if(strcmp(token.word,"or")==0){
            token.type=TOKEN_COR;
        }else if(strcmp(token.word,"break")==0){
            token.type=TOKEN_BREAK;
        }else if(strcmp(token.word,"putc")==0){
            token.type=TOKEN_PUTC;
        }else if(strcmp(token.word,"return")==0){
            token.type=TOKEN_RETURN;
        }else if(strcmp(token.word,"null")==0){
            token.type=TOKEN_NULL;
        }else{
            token.type=TOKEN_WORD;
        }
        return token;
    }else if(c=='+'){
        if(parser->code[parser->ptr+1]=='+'){
            token.type=TOKEN_DOUBLE_ADD;
            parser->ptr+=2;
            return token;
        }
        if(parser->code[parser->ptr+1]=='='){
            token.type=TOKEN_ADD_EQUAL;
            parser->ptr+=2;
            return token;
        }
        token.type=TOKEN_ADD;
        parser->ptr++;
        return token;
    }else if(c=='-'){
        if(parser->code[parser->ptr+1]=='='){
            token.type=TOKEN_SUB_EQUAL;
            parser->ptr+=2;
            return token;
        }
        if(parser->code[parser->ptr+1]=='-'){
            token.type=TOKEN_DOUBLE_SUB;
            parser->ptr+=2;
            return token;
        }
        token.type=TOKEN_SUB;
        parser->ptr++;
        return token;
    }else if(c=='*'){
        if(parser->code[parser->ptr+1]=='='){
            token.type=TOKEN_MUL_EQUAL;
            parser->ptr+=2;
            return token;
        }
        token.type=TOKEN_MUL;
        parser->ptr++;
        return token;
    }else if(c=='/'){
        if(parser->code[parser->ptr+1]=='='){
            token.type=TOKEN_DIV_EQUAL;
            parser->ptr+=2;
            return token;
        }
        token.type=TOKEN_DIV;
        parser->ptr++;
        return token;
    }else if(c=='('){
        token.type=TOKEN_PARE1;
        parser->ptr++;
        return token;
    }else if(c==')'){
        token.type=TOKEN_PARE2;
        parser->ptr++;
        return token;
    }else if(c=='{'){
        token.type=TOKEN_BRACE1;
        parser->ptr++;
        return token;
    }else if(c=='}'){
        token.type=TOKEN_BRACE2;
        parser->ptr++;
        return token;
    }else if(c==','){
        token.type=TOKEN_COMMA;
        parser->ptr++;
        return token;
    }else if(c=='.'){
        token.type=TOKEN_POINT;
        parser->ptr++;
        return token;
    }else if(c==';'){
        token.type=TOKEN_SEMI;
        parser->ptr++;
        return token;
    }else if(c=='='){
        token.type=TOKEN_EQUAL;
        parser->ptr++;
        return token;
    }else if(c=='~'){
        token.type=TOKEN_INVERT;
        parser->ptr++;
        return token;
    }else if(c=='['){
        token.type=TOKEN_BRACKET1;
        parser->ptr++;
        return token;
    }else if(c==']'){
        token.type=TOKEN_BRACKET2;
        parser->ptr++;
        return token;
    }else if(c=='!'){
        if(parser->code[parser->ptr+1]=='='){
            token.type=TOKEN_NOT_EQUAL;
            parser->ptr+=2;
            return token;
        }
        token.type=TOKEN_EXCL;
        parser->ptr++;
        return token;
    }else if(c=='>'){
        if(parser->code[parser->ptr+1]=='='){
            token.type=TOKEN_GTHAN_EQUAL;
            parser->ptr+=2;
            return token;
        }
        if(parser->code[parser->ptr+1]=='>'){
            if(parser->code[parser->ptr+2]=='='){
                token.type=TOKEN_RIGHT_EQUAL;
                parser->ptr+=3;
                return token;
            }
            token.type=TOKEN_RIGHT;
            parser->ptr+=2;
            return token;
        }
        token.type=TOKEN_GTHAN;
        parser->ptr++;
        return token;
    }else if(c=='<'){
        if(parser->code[parser->ptr+1]=='='){
            token.type=TOKEN_LTHAN_EQUAL;
            parser->ptr+=2;
            return token;
        }
        if(parser->code[parser->ptr+1]=='<'){
            if(parser->code[parser->ptr+2]=='='){
                token.type=TOKEN_LEFT_EQUAL;
                parser->ptr+=3;
                return token;
            }
            token.type=TOKEN_LEFT;
            parser->ptr+=2;
            return token;
        }
        token.type=TOKEN_LTHAN;
        parser->ptr++;
        return token;
    }else if(c=='&'){
        if(parser->code[parser->ptr+1]=='='){
            token.type=TOKEN_AND_EQUAL;
            parser->ptr+=2;
            return token;
        }
        token.type=TOKEN_AND;
        parser->ptr++;
        return token;
    }else if(c=='|'){
        if(parser->code[parser->ptr+1]=='='){
            token.type=TOKEN_OR_EQUAL;
            parser->ptr+=2;
            return token;
        }
        token.type=TOKEN_OR;
        parser->ptr++;
        return token;
    }else if(c=='%'){
        if(parser->code[parser->ptr+1]=='='){
            token.type=TOKEN_PERCENT_EQUAL;
            parser->ptr+=2;
            return token;
        }
        token.type=TOKEN_PERCENT;
        parser->ptr++;
        return token;
    }else{
        sprintf(msgt,"unknown charactor \"%c\".",c);
        reportError(parser,msgt);
        parser->ptr++;
    }
    return token;
}
HandleType handleFloat(HandleType ht){
    switch (ht)
    {
    case HANDLE_ADD:
        return HANDLE_FADD;
        break;
    case HANDLE_SUB:
        return HANDLE_FSUB;
        break;
    case HANDLE_MUL:
        return HANDLE_FMUL;
        break;
    case HANDLE_DIV:
        return HANDLE_FDIV;
        break;
    default:
        return ht;
        break;
    }
    return ht;
}
bool getClass(Parser*parser,int*class){
    Token token;
    ORI_DEF();
    ORI_ASI();
    token=nextToken(parser);
    if(token.type==TOKEN_INT){
        *class=TYPE_INTEGER;
    }else if(token.type==TOKEN_FLOAT_CLASS){
        *class=TYPE_FLOAT;
    }else if(token.type==TOKEN_WORD){
        bool isFound=false;
        for(int i=2;i<parser->classList.count;i++){
            if(strcmp(token.word,parser->classList.vals[i].name)==0){
                *class=i;
                isFound=true;
            }
        }
        if(!isFound){
            ORI_RET();
            return false;  
        }
    }else{
        ORI_RET();
        return false;
    }
    return true;
}
bool getExpression(Parser*parser,CmdList*clist,ReturnType*rtype,Environment envirn){
    Token token;
    OperatList olist;
    Operat operat;
    char msg[100];
    Variable var;
    ORI_DEF();
    bool isEnd=false;
    DataType dt;
    LIST_INIT(olist,Operat);
    while(1){
        ORI_ASI();
        token=nextToken(parser);
        /*处理前缀运算*/
        if(token.type==TOKEN_SUB){
            operat.handle_prefix=HANDLE_SUBS;
            token=nextToken(parser);
        }else if(token.type==TOKEN_EXCL){
            operat.handle_prefix=HANDLE_INVERT;
            token=nextToken(parser);
        }else if(token.type==TOKEN_INVERT){
            operat.handle_prefix=HANDLE_INVERT2;
            token=nextToken(parser);
        }else{
            operat.handle_prefix=HANDLE_NOP;
        }
        operat.rtype.isVar=false;
        operat.rtype.isStack=false;
        operat.rtype.dim=0;
        if(token.type==TOKEN_PARE1){
            if(!getExpression(parser,clist,&operat.rtype,envirn)){
                reportWarning(parser,"expected an expression in the \"( )\".");
            }
            token=nextToken(parser);
            if(token.type!=TOKEN_PARE2){
                reportError(parser,"expected \")\".");
            }
        }else if(token.type==TOKEN_INTEGER || token.type==TOKEN_FLOAT){
            addCmd2(clist,HANDLE_MOV,DATA_REG,DATA_INTEGER,REG_AX,token.num);
            operat.rtype.class=(token.type==TOKEN_INTEGER)?TYPE_INTEGER:TYPE_FLOAT;
        }else if(token.type==TOKEN_WORD){
            if(!getVarRef(parser,token.word,clist,&var,envirn)){
                sprintf(msg,"unfound variable \"%s\".",token.word);
                reportError(parser,msg);
            }
            operat.rtype.isVar=true;
            operat.rtype.class=var.class;
            operat.rtype.dim=var.dim;
            if(var.vtype==VAR_PART){
                operat.rtype.isStack=true;
            }else{
                operat.rtype.isStack=false;
            }
        }else if(token.type==TOKEN_NULL){
            addCmd2(clist,HANDLE_MOV,DATA_REG,DATA_INTEGER,REG_AX,TYPE_NULL);
            operat.rtype.class=TYPE_NULL;
        }else{
            ORI_RET();
            return false;
        }
        ORI_ASI();
        token=nextToken(parser);
        /*处理后缀运算*/
        if(token.type==TOKEN_DOUBLE_ADD){
            ORI_ASI();
            operat.handle_postfix=HANDLE_ADD;
            token=nextToken(parser);
        }else if(token.type==TOKEN_DOUBLE_SUB){
            ORI_ASI();
            operat.handle_postfix=HANDLE_SUB;
            token=nextToken(parser);
        }else{
            operat.handle_postfix=HANDLE_NOP;
        }
        /*处理中间运算*/
        if(token.type==TOKEN_ADD){
            operat.handle_infix=HANDLE_ADD;
            operat.power=50;
        }else if(token.type==TOKEN_SUB){
            operat.handle_infix=HANDLE_SUB;
            operat.power=50;
        }else if(token.type==TOKEN_MUL){
            operat.handle_infix=HANDLE_MUL;
            operat.power=80;
        }else if(token.type==TOKEN_DIV){
            operat.handle_infix=HANDLE_DIV;
            operat.power=80;
        }else if(token.type==TOKEN_PERCENT){
            operat.handle_infix=HANDLE_REM;
            operat.power=80;
        }else if(token.type==TOKEN_EQUAL){
            operat.handle_infix=HANDLE_EQUAL;
            operat.power=30;
        }else if(token.type==TOKEN_CAND){
            operat.handle_infix=HANDLE_CAND;
            operat.power=10;
        }else if(token.type==TOKEN_COR){
            operat.handle_infix=HANDLE_COR;
            operat.power=10;
        }else if(token.type==TOKEN_AND){
            operat.handle_infix=HANDLE_AND;
            operat.power=100;
        }else if(token.type==TOKEN_OR){
            operat.handle_infix=HANDLE_OR;
            operat.power=100;
        }else if(token.type==TOKEN_GTHAN){
            operat.handle_infix=HANDLE_GTHAN;
            operat.power=30;
        }else if(token.type==TOKEN_GTHAN_EQUAL){
            operat.handle_infix=HANDLE_GTHAN_EQUAL;
            operat.power=30;
        }else if(token.type==TOKEN_LTHAN){
            operat.handle_infix=HANDLE_LTHAN;
            operat.power=30;
        }else if(token.type==TOKEN_LTHAN_EQUAL){
            operat.handle_infix=HANDLE_LTHAN_EQUAL;
            operat.power=30;
        }else if(token.type==TOKEN_NOT_EQUAL){
            operat.handle_infix=HANDLE_NOT_EQUAL;
            operat.power=30;
        }else if(token.type==TOKEN_LEFT){
            operat.handle_infix=HANDLE_LEFT;
            operat.power=110;
        }else if(token.type==TOKEN_RIGHT){
            operat.handle_infix=HANDLE_RIGHT;
            operat.power=110;
        }else{
            /*表达式结束*/
            ORI_RET();
            isEnd=1;
        }
        bool isRight=false;
        if(olist.count>=1){
            if(olist.vals[olist.count-1].power>=operat.power){
                isRight=true;
            }
        }
        if(isEnd){
            isRight=true;
        }
        if(isRight){
            if(operat.rtype.isVar){
                if(operat.rtype.isStack){
                    dt=DATA_REG_STACK;
                }else{
                    dt=DATA_REG_POINTER;
                }
            }else{
                dt=DATA_REG;
            }
            if(operat.handle_postfix==HANDLE_ADD || operat.handle_postfix==HANDLE_SUB){
                if(operat.rtype.class==TYPE_INTEGER){
                    addCmd2(clist,operat.handle_postfix,dt,DATA_INTEGER,REG_AX,1);
                }else if(operat.rtype.class==TYPE_FLOAT){
                    addCmd2(clist,handleFloat(operat.handle_postfix),dt,DATA_INTEGER,REG_AX,1);
                }else if(operat.rtype.class==TYPE_NULL){
                    reportWarning(parser,"the null does not support postfix calculation.");
                }else{
                    reportError(parser,"unknown constant.");
                }
                operat.handle_postfix=HANDLE_NOP;
            }
            if(operat.rtype.isVar && (operat.rtype.class==TYPE_FLOAT || operat.rtype.class==TYPE_INTEGER)){
                addCmd2(clist,HANDLE_MOV,DATA_REG,dt,REG_AX,REG_AX);
                operat.rtype.isVar=false;
                operat.rtype.dim=0;
            }
            if(operat.handle_prefix!=HANDLE_NOP){
                if(operat.rtype.class==TYPE_FLOAT || operat.rtype.class==TYPE_INTEGER){
                    addCmd1(clist,operat.handle_prefix,DATA_REG,REG_AX);
                    operat.handle_prefix=HANDLE_NOP;
                }else{
                    /*处理对象*/
                }
            }
            while(olist.count>=1){
                Operat opt=olist.vals[olist.count-1];
                if(operat.power>opt.power && !isEnd){
                    break;
                }
                if(opt.rtype.isVar){
                    if(opt.rtype.isStack){
                        dt=DATA_REG_STACK;
                    }else{
                        dt=DATA_REG_POINTER;
                    }
                }else{
                    dt=DATA_REG;
                }
                addCmd2(clist,HANDLE_MOV,DATA_REG,DATA_REG,REG_BX,REG_AX);
                addCmd1(clist,HANDLE_POP,DATA_REG,REG_AX);
                if(opt.handle_postfix==HANDLE_ADD || opt.handle_postfix==HANDLE_SUB){
                    if(opt.rtype.class==TYPE_INTEGER){
                        addCmd2(clist,opt.handle_postfix,dt,DATA_INTEGER,REG_AX,1);
                    }else if(opt.rtype.class==TYPE_FLOAT){
                        addCmd2(clist,handleFloat(opt.handle_postfix),dt,DATA_INTEGER,REG_AX,1);
                    }else if(opt.rtype.class==TYPE_NULL){
                        reportWarning(parser,"the null does not support postfix calculation.");
                    }else{
                        sprintf(msg,"the type \"%s\" does not support postfix calculation.",parser->classList.vals[operat.rtype.class].name);
                        reportWarning(parser,msg);
                    }
                    opt.handle_postfix=HANDLE_NOP;
                }
                if(opt.rtype.isVar && (opt.rtype.class==TYPE_FLOAT || opt.rtype.class==TYPE_INTEGER)){
                    addCmd2(clist,HANDLE_MOV,DATA_REG,dt,REG_AX,REG_AX);
                }
                if(opt.handle_prefix!=HANDLE_NOP){
                    if(opt.rtype.class==TYPE_FLOAT || opt.rtype.class==TYPE_INTEGER){
                        addCmd1(clist,opt.handle_prefix,DATA_REG,REG_AX);
                    }else{
                        /*处理对象*/
                    }
                }
                if(opt.rtype.class==TYPE_FLOAT){
                    if(operat.rtype.class!=TYPE_FLOAT && operat.rtype.class!=TYPE_INTEGER && operat.rtype.class!=TYPE_NULL){
                        sprintf(msg,"can not cover type \"%s\" to \"%s\".",parser->classList.vals[TYPE_FLOAT].name,parser->classList.vals[operat.rtype.class].name);
                        reportError(parser,msg);
                    }
                    if(operat.rtype.class!=TYPE_NULL){
                        addCmd2(clist,handleFloat(opt.handle_infix),DATA_REG,DATA_REG,REG_AX,REG_BX);
                    }
                }else if(opt.rtype.class==TYPE_INTEGER){
                    if(operat.rtype.class==TYPE_FLOAT){
                        opt.handle_infix=handleFloat(opt.handle_infix);
                    }else if(operat.rtype.class!=TYPE_INTEGER && operat.rtype.class!=TYPE_NULL){
                        sprintf(msg,"can not cover type \"%s\" to \"%s\".",parser->classList.vals[TYPE_INTEGER].name,parser->classList.vals[operat.rtype.class].name);
                        reportError(parser,msg);
                    }
                    if(operat.rtype.class!=TYPE_NULL){
                        addCmd2(clist,opt.handle_infix,DATA_REG,DATA_REG,REG_AX,REG_BX);
                    }
                }else if(opt.rtype.class==TYPE_NULL && (operat.rtype.class==TYPE_INTEGER || operat.rtype.class==TYPE_FLOAT)){
                    addCmd2(clist,HANDLE_MOV,DATA_REG,DATA_INTEGER,REG_AX,0);
                    addCmd2(clist,opt.handle_infix,DATA_REG,DATA_REG,REG_AX,REG_BX);
                }else{
                    /*处理对象*/
                }
                operat.rtype.class=opt.rtype.class;
                LIST_SUB(olist,Operat);
            }
        }
        if(isEnd){
            *rtype=operat.rtype;
            break;
        }
        addCmd1(clist,HANDLE_PUSH,DATA_REG,REG_AX);
        LIST_ADD(olist,Operat,operat);
    }
    LIST_DELETE(olist);
    return true;
}
bool getVariableDef(Parser*parser,VariableList*vlist,CmdList*clist,bool isPart,int*partSize,Environment envirn){
    Token token;
    Variable var;
    char msg[100];
    intList arraySize;
    ReturnType rtype;
    if(!getClass(parser,&var.class)){
        return false;
    }
    var.vtype=isPart?VAR_PART:VAR_GLOBAL;
    while(1){
        token=nextToken(parser);
        if(token.type!=TOKEN_WORD){
            reportError(parser,"expected a variable name.");
        }
        if(envirn.pvlist!=NULL){
            for(int i=0;i<envirn.pvlist->count;i++){
                if(strcmp(token.word,envirn.pvlist->vals[i].name)==0){
                    sprintf(msg,"the variable \"%s\" has already exist.",token.word);
                    reportError(parser,msg);
                }
            }
        }
        for(int i=0;i<parser->varlist.count;i++){
            if(strcmp(token.word,parser->varlist.vals[i].name)==0){
                sprintf(msg,"the variable \"%s\" has already exist.",token.word);
                reportError(parser,msg);
            }
        }
        var.subVar=NULL;
        var.dim=0;
        var.unitSize=0;
        var.arrayCount=0;
        strcpy(var.name,token.word);
        var.ptr=isPart?*partSize:parser->dataSize;
        token=nextToken(parser);
        /*处理数组 */
        int dim=0;
        Variable*var2;
        for(int i=0;token.type==TOKEN_BRACKET1;i++){
            if(i==0){
                LIST_INIT(arraySize,int);
            }
            token=nextToken(parser);
            if(token.type!=TOKEN_INTEGER){
                reportError(parser,"expected integer when define array.");
            }
            token=nextToken(parser);
            if(token.type!=TOKEN_BRACKET2){
                reportError(parser,"expected \"]\" when define array.");
            }
            LIST_ADD(arraySize,int,token.num);
            token=nextToken(parser);
            dim++;
        }
        if(dim){
            LIST_ADD(arraySize,int,parser->classList.vals[var.class].size);
            for(int i=arraySize.count-2;i>=0;i--){
                arraySize.vals[i]=arraySize.vals[i+1]*arraySize.vals[i];
            }
            if(var.vtype==VAR_GLOBAL){
                parser->dataSize+=arraySize.vals[0];
            }else{
                (*partSize)+=arraySize.vals[0];
            }
            var2=&var;
            for(int i=0;i<dim;i++){
                var2->vtype=var.vtype;
                var2->ptr=var.ptr;
                var2->unitSize=arraySize.vals[i+1];
                var2->dim=dim-i;
                var2->class=var.class;
                var2->arrayCount=arraySize.vals[i]/arraySize.vals[i+1];
                var2->subVar=(Variable*)malloc(sizeof(Variable));
                var2=var2->subVar;
            }
            var2->dim=0;
            var2->unitSize=0;
            var2->class=var.class;
            LIST_DELETE(arraySize);
        }else{
            if(var.vtype==VAR_GLOBAL){
                parser->dataSize+=parser->classList.vals[var.class].size;
            }else{
                (*partSize)+=parser->classList.vals[var.class].size;
            }
        }
        LIST_ADD((*vlist),Variable,var);
        if(token.type==TOKEN_EQUAL){
            if(var.dim){
                addCmd2(clist,HANDLE_MOV,DATA_REG,DATA_INTEGER,REG_AX,var.ptr);
                if(!getArray(parser,clist,var,envirn)){
                    reportError(parser,"expected an array when initializing variable.");
                }
            }else{
                if(!getExpression(parser,clist,&rtype,envirn)){
                    reportError(parser,"expected an expression when initializing variable.");
                }
                if((var.class==TYPE_FLOAT && rtype.class==TYPE_INTEGER) || (var.class==TYPE_INTEGER && rtype.class==TYPE_INTEGER) || (var.class==TYPE_FLOAT && rtype.class==TYPE_FLOAT)){
                    if(isPart){
                        addCmd1(clist,HANDLE_PUSH,DATA_REG,REG_AX);
                    }else{
                        addCmd2(clist,HANDLE_MOV,DATA_POINTER,DATA_REG,var.ptr,REG_AX);
                    }
                }else if(var.class==TYPE_INTEGER && rtype.class==TYPE_FLOAT){
                    //addCmd1(clist,HANDLE_POP,DATA_REG,REG_AX);
                    addCmd2(clist,HANDLE_FTOI,DATA_REG,DATA_REG,REG_AX,REG_AX);
                    if(isPart){
                        addCmd1(clist,HANDLE_PUSH,DATA_REG,REG_AX);
                    }else{
                        addCmd2(clist,HANDLE_MOV,DATA_POINTER,DATA_REG,var.ptr,REG_AX);
                    }
                }else if(rtype.class==TYPE_NULL){
                    if(var.class==TYPE_INTEGER || var.class==TYPE_FLOAT){
                        if(isPart){
                            addCmd1(clist,HANDLE_PUSH,DATA_INTEGER,0);
                        }else{
                            addCmd2(clist,HANDLE_MOV,DATA_POINTER,DATA_INTEGER,var.ptr,0);
                        }
                    }else{
                        if(isPart){
                            addCmd1(clist,HANDLE_PUSH,DATA_INTEGER,TYPE_NULL);
                        }else{
                            addCmd2(clist,HANDLE_MOV,DATA_POINTER,DATA_INTEGER,var.ptr,TYPE_NULL);
                        }
                    }
                }else if(var.class!=rtype.class){
                    sprintf(msg,"incompatible types when initializing to type \"%s\" from type \"%s\".",parser->classList.vals[var.class].name,parser->classList.vals[rtype.class].name);
                    reportError(parser,msg);
                }else{
                    /*对象赋值，有待改进 */
                    addCmd2(clist,HANDLE_MOV,DATA_POINTER,DATA_REG,var.ptr,REG_AX);
                }
            }
            token=nextToken(parser);
        }else if(isPart){
            if(var.dim){
                addCmd2(clist,HANDLE_PUSHB,DATA_INTEGER,DATA_INTEGER,0,var.arrayCount*var.unitSize);
            }else{
                addCmd2(clist,HANDLE_PUSHB,DATA_INTEGER,DATA_INTEGER,0,parser->classList.vals[var.class].size);
            }
        }
        if(token.type==TOKEN_COMMA){
            continue;
        }else if(token.type==TOKEN_SEMI){
            break;
        }else{
            reportError(parser,"expected \",\" or \";\" after initializing variable");
        }
    }
    return true;
}
bool getAssignment(Parser*parser,CmdList*clist,Environment envirn){
    Token token;
    ReturnType rtype;
    Variable var;
    VariableList  varList;
    ORI_DEF();
    HandleType ht;
    DataType dt;
    char msg[100];
    int stackPtr=-1;
    ORI_ASI();
    LIST_INIT(varList,Variable);
    while(1){
        token=nextToken(parser);
        if(token.type!=TOKEN_WORD){
            ORI_RET();
            return false;
        }
        if(!getVarRef(parser,token.word,clist,&var,envirn)){
            ORI_RET();
            return false;
        }
        addCmd1(clist,HANDLE_PUSH,DATA_REG,REG_AX);
        token=nextToken(parser);
        stackPtr++;
        LIST_ADD(varList,Variable,var);
        if(token.type==TOKEN_COMMA){
            continue;
        }else if(token.type==TOKEN_EQUAL){
            ht=HANDLE_MOV;
            break;
        }else if(token.type==TOKEN_ADD_EQUAL){
            ht=HANDLE_ADD;
            break;
        }else if(token.type==TOKEN_SUB_EQUAL){
            ht=HANDLE_SUB;
            break;
        }else if(token.type==TOKEN_MUL_EQUAL){
            ht=HANDLE_MUL;
            break;
        }else if(token.type==TOKEN_DIV_EQUAL){
            ht=HANDLE_DIV;
            break;
        }else if(token.type==TOKEN_AND_EQUAL){
            ht=HANDLE_AND;
            break;
        }else if(token.type==TOKEN_OR_EQUAL){
            ht=HANDLE_OR;
            break;
        }else if(token.type==TOKEN_LEFT_EQUAL){
            ht=HANDLE_LEFT;
            break;
        }else if(token.type==TOKEN_RIGHT_EQUAL){
            ht=HANDLE_RIGHT;
            break;
        }else if(token.type==TOKEN_PERCENT_EQUAL){
            ht=HANDLE_REM;
            break;
        }else{
            ORI_RET();
            return false;
        }
    }
    int sCount=stackPtr+1;
    int i=0;
    while(1){
        if(stackPtr<0){
            reportError(parser,"too many assignment.");
        }
        var=varList.vals[i];
        dt=(var.vtype==VAR_PART)?DATA_REG_STACK:DATA_REG_POINTER;
        if(var.dim){
            addCmd2(clist,HANDLE_POPT,DATA_REG,DATA_INTEGER,REG_AX,stackPtr);
            if(!getArray(parser,clist,var,envirn)){
                reportError(parser,"expected an array in assignment.");
            }
        }else{
            if(!getExpression(parser,clist,&rtype,envirn)){
                reportError(parser,"expected an expression in assignment.");
            }
            addCmd2(clist,HANDLE_MOV,DATA_REG,DATA_REG,REG_BX,REG_AX);
            addCmd2(clist,HANDLE_POPT,DATA_REG,DATA_INTEGER,REG_AX,stackPtr);
            if((var.class==TYPE_FLOAT && rtype.class==TYPE_INTEGER) || (var.class==TYPE_INTEGER && rtype.class==TYPE_INTEGER) || (var.class==TYPE_FLOAT && rtype.class==TYPE_FLOAT)){
                addCmd2(clist,ht,dt,DATA_REG,REG_AX,REG_BX);
            }else if(var.class==TYPE_INTEGER && rtype.class==TYPE_FLOAT){
                addCmd2(clist,HANDLE_FTOI,DATA_REG,DATA_REG,REG_BX,REG_BX);
                addCmd2(clist,ht,dt,DATA_REG,REG_AX,REG_BX);
            }else if((var.class==TYPE_FLOAT || var.class==TYPE_INTEGER) && rtype.class==TYPE_NULL){
                addCmd2(clist,ht,dt,DATA_INTEGER,REG_AX,0);
            }else if(rtype.class==TYPE_NULL){
                if(ht==HANDLE_MOV){
                    addCmd2(clist,HANDLE_MOV,dt,DATA_INTEGER,REG_AX,-1);
                }
            }else if(var.class!=rtype.class){
                sprintf(msg,"incompatible types when assigning to type \"%s\" from type \"%s\".",parser->classList.vals[var.class].name,parser->classList.vals[rtype.class].name);
                reportError(parser,msg);
            }else{
                /*对象赋值 */
                if(ht!=HANDLE_MOV){
                    sprintf(msg,"the type \"%s\" only support \"=\" to assign.",parser->classList.vals[var.class].name);
                    reportError(parser,msg);
                }
                addCmd2(clist,ht,dt,DATA_REG,REG_AX,REG_BX);
            }
        }
        stackPtr--;
        token=nextToken(parser);
        if(token.type==TOKEN_COMMA){
            continue;
            i++;
        }else if(token.type==TOKEN_SEMI){
            while(stackPtr>=0){
                var=varList.vals[i];
                dt=(var.vtype==VAR_PART)?DATA_REG_STACK:DATA_REG_POINTER;
                if(var.dim){
                    reportError(parser,"too many array in assignment.");
                }else{
                    addCmd2(clist,HANDLE_POPT,DATA_REG,DATA_INTEGER,REG_AX,stackPtr);
                    if((var.class==TYPE_FLOAT && rtype.class==TYPE_INTEGER) || (var.class==TYPE_INTEGER && rtype.class==TYPE_INTEGER) || (var.class==TYPE_FLOAT && rtype.class==TYPE_FLOAT)){
                        addCmd2(clist,ht,dt,DATA_REG,REG_AX,REG_BX);
                    }else if(var.class==TYPE_INTEGER && rtype.class==TYPE_FLOAT){
                        addCmd2(clist,HANDLE_FTOI,DATA_REG,DATA_REG,REG_BX,REG_BX);
                        addCmd2(clist,ht,dt,DATA_REG,REG_AX,REG_BX);
                    }else if((var.class==TYPE_FLOAT || var.class==TYPE_INTEGER) && rtype.class==TYPE_NULL){
                        addCmd2(clist,ht,dt,DATA_INTEGER,REG_AX,0);
                    }else if(rtype.class==TYPE_NULL){
                        if(ht==HANDLE_MOV){
                            addCmd2(clist,HANDLE_MOV,dt,DATA_INTEGER,REG_AX,-1);
                        }
                    }else if(var.class!=rtype.class){
                        sprintf(msg,"incompatible types when assigning to type \"%s\" from type \"%s\".",parser->classList.vals[var.class].name,parser->classList.vals[rtype.class].name);
                        reportError(parser,msg);
                    }else{
                        /*对象赋值 */
                        if(ht!=HANDLE_MOV){
                            sprintf(msg,"the type \"%s\" only support \"=\" to assign.",parser->classList.vals[var.class].name);
                            reportError(parser,msg);
                        }
                        addCmd2(clist,ht,dt,DATA_REG,REG_AX,REG_BX);
                    }
                }
                stackPtr--;
            }
            break;
        }else{
            reportError(parser,"expected \",\" or \";\" after the assignment.");
        }
    }
    addCmd1(clist,HANDLE_SFREE,DATA_INTEGER,sCount);
    LIST_DELETE(varList);
    return true;
}
bool getVarRef(Parser*parser,char*varName,CmdList*clist,Variable*var,Environment envirn){
    bool isFound=false;
    Token token;
    ReturnType rtype;
    ORI_DEF();
    ORI_ASI();
    if(envirn.pvlist!=NULL){
        for(int i=0;i<envirn.pvlist->count;i++){
            if(strcmp(varName,envirn.pvlist->vals[i].name)==0){
                *var=envirn.pvlist->vals[i];
                if(var->vtype==VAR_ARGUMENT){
                    addCmd2(clist,HANDLE_MOV,DATA_REG,DATA_INTEGER,REG_AX,envirn.pvlist->vals[i].ptr);
                    addCmd2(clist,HANDLE_ADD,DATA_REG,DATA_REG,REG_AX,REG_SP);
                }else{
                    addCmd2(clist,HANDLE_MOV,DATA_REG,DATA_INTEGER,REG_AX,envirn.pvlist->vals[i].ptr);
                    addCmd2(clist,HANDLE_ADD,DATA_REG,DATA_REG,REG_AX,REG_SP);
                }
                isFound=true;
                break;
            }
        }
    }
    for(int i=0;i<parser->varlist.count && !isFound;i++){
        if(strcmp(varName,parser->varlist.vals[i].name)==0){
            *var=parser->varlist.vals[i];
            addCmd2(clist,HANDLE_MOV,DATA_REG,DATA_INTEGER,REG_AX,parser->varlist.vals[i].ptr);
            addCmd2(clist,HANDLE_ADD,DATA_REG,DATA_REG,REG_AX,REG_SP);
            isFound=true;
            break;
        }
    }
    if(!isFound){
        return false;
    }
    token=nextToken(parser);
    if(token.type==TOKEN_BRACKET1){
        if(var->vtype==VAR_ARGUMENT){
            addCmd1(clist,HANDLE_PUSH,DATA_REG,REG_AX);
            addCmd1(clist,HANDLE_PUSH,DATA_REG_STACK,REG_AX);
        }else{
            addCmd1(clist,HANDLE_PUSH,DATA_REG,REG_AX);
        }
        for(int i=0;token.type==TOKEN_BRACKET1;i++){
            if(var->dim<=0){
                reportError(parser,"too many index in the array.");
            }
            if(!getExpression(parser,clist,&rtype,envirn)){
                reportError(parser,"expected an expression in the index of array.");
            }
            token=nextToken(parser);
            if(token.type!=TOKEN_BRACKET2){
                reportError(parser,"expected \"]\" in the index of array.");
            }
            if(var->vtype==VAR_ARGUMENT){
                addCmd2(clist,HANDLE_POPT,DATA_REG,DATA_INTEGER,REG_BX,1);
                addCmd2(clist,HANDLE_ADD,DATA_REG,DATA_INTEGER,REG_BX,i);
                addCmd2(clist,HANDLE_MUL,DATA_REG,DATA_REG_STACK,REG_AX,REG_BX);
            }else{
                addCmd2(clist,HANDLE_MUL,DATA_REG,DATA_INTEGER,REG_AX,var->unitSize);
            }
            addCmd1(clist,HANDLE_POP,DATA_REG,REG_BX);
            addCmd2(clist,HANDLE_ADD,DATA_REG,DATA_REG,REG_BX,REG_AX);
            addCmd1(clist,HANDLE_PUSH,DATA_REG,REG_BX);
            *var=*(var->subVar);
            ORI_ASI();
            token=nextToken(parser);
        }
        addCmd1(clist,HANDLE_POP,DATA_REG,REG_AX);
        if(var->vtype==VAR_ARGUMENT){
            addCmd1(clist,HANDLE_SFREE,DATA_INTEGER,1);
        }
    }else if(var->vtype==VAR_ARGUMENT){
        addCmd2(clist,HANDLE_MOV,DATA_REG,DATA_REG_STACK,REG_AX,REG_AX);
    }
    ORI_RET();
    return true;
}
void getBlock(Parser*parser,CmdList*clist,Environment envirn){
    Token token;
    ORI_DEF();
    char msg[100],temp[50];
    ReturnType rtype;
    VariableList pvlist;
    int rSize=envirn.partSize;/*用于计算本层局部变量大小*/
    token=nextToken(parser);
    if(token.type!=TOKEN_BRACE1){
        reportError(parser,"expected \"{\".");
    }
    LIST_INIT(pvlist,Variable);
    if(envirn.pvlist!=NULL){
        LIST_CONNECT(pvlist,(*(envirn.pvlist)),Variable);
    }
    envirn.pvlist=&pvlist;
    while(1){
        ORI_ASI();
        token=nextToken(parser);
        if(token.type==TOKEN_BRACE2){
            break;
        }else if(token.type==TOKEN_BREAK){
            token=nextToken(parser);
            if(token.type!=TOKEN_SEMI){
                reportError(parser,"expected \";\" after \"break\".");
            }
            if(envirn.breakList==NULL){
                reportError(parser,"unuseful break.");
            }else{
                addCmd1(clist,HANDLE_JMP,DATA_INTEGER,1);
                LIST_ADD((*envirn.breakList),int,clist->count-1);
            }
            continue;
        }else if(token.type==TOKEN_RETURN){
            if(envirn.func==NULL){
                reportError(parser,"unuseful return.");
            }
            if(!getExpression(parser,clist,&rtype,envirn)){
                if(envirn.func->class!=TYPE_INTEGER){
                    getFuncName(parser,*envirn.func,temp);
                    sprintf(msg,"expected a return type in function \"%s\".",temp);
                    reportError(parser,msg);
                }
            }
            token=nextToken(parser);
            if(token.type!=TOKEN_SEMI){
                reportError(parser,"expected \";\" after \"return\".");
            }
            if(envirn.partSize-rSize+envirn.func->partSize!=0){
                addCmd1(clist,HANDLE_SFREE,DATA_INTEGER,envirn.partSize-rSize+envirn.func->partSize);
            }
            addCmd1(clist,HANDLE_POP,DATA_REG,REG_BX);
            addCmd1(clist,HANDLE_JMPS,DATA_REG,REG_BX);
            continue;
        }else if(token.type==TOKEN_END){
            reportError(parser,"expected \"}\".");
        }
        ORI_RET();
        if(getVariableDef(parser,&pvlist,clist,true,&envirn.partSize,envirn)){

        }else if(getAssignment(parser,clist,envirn)){

        }else if(getConditionState(parser,clist,envirn)){
            
        }else if(getWhileLoop(parser,clist,envirn)){
            
        }else if(getInsideSub(parser,clist,envirn)){

        }else if(getExpression(parser,clist,&rtype,envirn)){
            token=nextToken(parser);
            if(token.type!=TOKEN_SEMI){
                reportError(parser,"expected \";\" after an expression");
            }
        }else{
            reportError(parser,"unknown expression.");
        }
    }
    if(envirn.partSize-rSize!=0){
        addCmd1(clist,HANDLE_SFREE,DATA_INTEGER,envirn.partSize-rSize);
    }
    LIST_DELETE(pvlist);
}
bool getConditionState(Parser*parser,CmdList*clist,Environment envirn){
    Token token;
    int rptr,rline;
    intList ilist;
    int jptr;
    ReturnType rtype;
    rptr=parser->ptr;
    rline=parser->line;
    LIST_INIT(ilist,int);
    token=nextToken(parser);
    if(token.type!=TOKEN_IF){
        parser->ptr=rptr;
        parser->line=rline;
        return false;
    }
    token=nextToken(parser);
    if(token.type!=TOKEN_PARE1){
        reportError(parser,"expected \"(\" after \"if\".");
    }
    if(!getExpression(parser,clist,&rtype,envirn)){
        reportError(parser,"expected an expression in the conditional statement.");
    }
    token=nextToken(parser);
    if(token.type!=TOKEN_PARE2){
        reportError(parser,"expected \")\" after \"if(expression...\".");
    }
    addCmd2(clist,HANDLE_MOV,DATA_REG,DATA_REG,REG_CF,REG_AX);
    addCmd1(clist,HANDLE_JMPC,DATA_INTEGER,1);
    jptr=clist->count-1;
    getBlock(parser,clist,envirn);
    rptr=parser->ptr;
    rline=parser->line;
    token=nextToken(parser);
    while(token.type==TOKEN_ELIF){
        addCmd1(clist,HANDLE_JMP,DATA_INTEGER,1);
        LIST_ADD(ilist,int,clist->count-1);
        clist->vals[jptr].a=clist->count-jptr;
        token=nextToken(parser);
        if(token.type!=TOKEN_PARE1){
            reportError(parser,"expected \"(\" after \"if(expression...\".");
        }
        if(!getExpression(parser,clist,&rtype,envirn)){
            reportError(parser,"expected an expression in the conditional statement.");
        }
        token=nextToken(parser);
        if(token.type!=TOKEN_PARE2){
            reportError(parser,"expected \")\" after \"if\".");
        }
        addCmd2(clist,HANDLE_MOV,DATA_REG,DATA_REG,REG_CF,REG_AX);
        addCmd1(clist,HANDLE_JMPC,DATA_INTEGER,1);
        jptr=clist->count-1;
        getBlock(parser,clist,envirn);
        ORI_ASI();
        token=nextToken(parser);
    }
    if(token.type==TOKEN_ELSE){
        addCmd1(clist,HANDLE_JMP,DATA_INTEGER,1);
        LIST_ADD(ilist,int,clist->count-1);
        getBlock(parser,clist,envirn);
    }else{
        ORI_RET();
    }
    clist->vals[jptr].a=clist->count-jptr;
    for(int i=0;i<ilist.count;i++){
        clist->vals[ilist.vals[i]].a=clist->count-ilist.vals[i];
    }
    LIST_DELETE(ilist)
    return true;
}
bool getWhileLoop(Parser*parser,CmdList*clist,Environment envirn){
    Token token;
    ORI_DEF();
    int jptr,wptr;
    ReturnType rtype;
    ORI_ASI();
    token=nextToken(parser);
    if(token.type!=TOKEN_WHILE){
        ORI_RET();
        return false;
    }
    token=nextToken(parser);
    if(token.type!=TOKEN_PARE1){
        reportError(parser,"expected \"(\" after \"while\".");
    }
    wptr=clist->count;
    if(!getExpression(parser,clist,&rtype,envirn)){
        reportError(parser,"expected an expression in the loop statement.");
    }
    token=nextToken(parser);
    if(token.type!=TOKEN_PARE2){
        reportError(parser,"expected \")\" after \"while\".");
    }
    intList breakList;
    LIST_INIT(breakList,int);
    envirn.breakList=&breakList;
    addCmd2(clist,HANDLE_MOV,DATA_REG,DATA_REG,REG_CF,REG_AX);
    addCmd1(clist,HANDLE_JMPC,DATA_INTEGER,1);
    jptr=clist->count-1;
    ORI_ASI();
    token=nextToken(parser);
    if(token.type!=TOKEN_SEMI){
        ORI_RET();
        getBlock(parser,clist,envirn);
    }
    addCmd1(clist,HANDLE_JMP,DATA_INTEGER,-(clist->count-wptr));
    clist->vals[jptr].a=clist->count-jptr;
    for(int i=0;i<breakList.count;i++){
        clist->vals[breakList.vals[i]].a=clist->count-breakList.vals[i];
    }
    LIST_DELETE(breakList);
    return true;
}
bool getInsideSub(Parser*parser,CmdList*clist,Environment envirn){
    Token token;
    ORI_DEF();
    ReturnType rtype;
    ORI_ASI();
    token=nextToken(parser);
    if(token.type==TOKEN_PUTC){
        token=nextToken(parser);
        if(token.type!=TOKEN_PARE1){
            reportError(parser,"expected \"(\" after putc.");
        }
        if(!getExpression(parser,clist,&rtype,envirn)){
            reportError(parser,"expected an expression in putc.");
        }
        token=nextToken(parser);
        if(token.type!=TOKEN_PARE2){
            reportError(parser,"expected \")\" after putc.");
        }
        token=nextToken(parser);
        if(token.type!=TOKEN_SEMI){
            reportError(parser,"expected \";\" after putc.");
        }
        if(rtype.class!=TYPE_INTEGER){
            reportError(parser,"putc only can output integer.");
        }
        addCmd1(clist,HANDLE_PUTC,DATA_REG,REG_AX);
    }else{
        ORI_RET();
        return false;
    }
    return true;
}
bool getArray(Parser*parser,CmdList*clist,Variable var,Environment envirn){
    Token token;
    ORI_DEF();
    ReturnType rtype;
    char msg[100];
    int count=0;
    ORI_ASI();
    token=nextToken(parser);
    if(token.type!=TOKEN_BRACE1){
        ORI_RET();
        return false;
    }
    while(1){
        count++;
        if(count>var.arrayCount){
            reportError(parser,"too many arrays.");
        }
        if(var.dim-1>0){
            if(!getArray(parser,clist,*var.subVar,envirn)){
                reportError(parser,"expected an array in the array.");
            }
        }else{
            addCmd1(clist,HANDLE_PUSH,DATA_REG,REG_AX);
            if(!getExpression(parser,clist,&rtype,envirn)){
                reportError(parser,"expected an expression in the array.");
            }
            if(var.class==TYPE_INTEGER && rtype.class==TYPE_FLOAT){
                addCmd1(clist,HANDLE_FTOI,DATA_REG,REG_AX);
            }else if(!(var.class==TYPE_FLOAT && rtype.class==TYPE_INTEGER) && var.class!=rtype.class){
                sprintf(msg,"can not cover the type \"%s\" from \"%s\".",parser->classList.vals[var.class].name,parser->classList.vals[var.class].name);
                reportError(parser,msg);
            }
            if(var.class==TYPE_FLOAT || var.class==TYPE_INTEGER){
                addCmd1(clist,HANDLE_POP,DATA_REG,REG_BX);
                if(var.vtype==VAR_PART){
                    addCmd2(clist,HANDLE_MOV,DATA_REG_STACK,DATA_REG,REG_BX,REG_AX);
                }else{
                    addCmd2(clist,HANDLE_MOV,DATA_REG_POINTER,DATA_REG,REG_BX,REG_AX);
                }
                addCmd2(clist,HANDLE_ADD,DATA_REG,DATA_INTEGER,REG_BX,1);
                addCmd1(clist,HANDLE_PUSH,DATA_REG,REG_BX);
            }else{
                /*对象赋值*/
            }
            addCmd1(clist,HANDLE_POP,DATA_REG,REG_AX);
        }
        token=nextToken(parser);
        if(token.type==TOKEN_COMMA){
            continue;
        }else if(token.type==TOKEN_BRACE2){
            break;
        }else{
            reportError(parser,"expected \",\" or \"}\" in the defination of the array.");
        }
    }
    if(count<var.arrayCount){
        reportError(parser,"too few arrays.");
    }
    return true;
}
bool getFunctionDef(Parser*parser,FunctionList*funcList){
    Token token;
    Function func;
    Variable var;
    Variable*var2;
    Environment envirn;
    bool isEmpty;
    char msg[100],temp[50];
    ORI_DEF();
    ORI_ASI();
    if(!getClass(parser,&func.class)){
        return false;
    }
    token=nextToken(parser);
    if(token.type!=TOKEN_WORD){
        ORI_RET();
        return false;
    }
    strcpy(func.name,token.word);
    token=nextToken(parser);
    if(token.type!=TOKEN_PARE1){
        ORI_RET();
        return false;
    }
    func.partSize=0;
    func.ptr=parser->funcClist.count;
    LIST_INIT(func.argument,Variable);
    if(!getClass(parser,&var.class)){
        isEmpty=true;
        token=nextToken(parser);
        if(token.type!=TOKEN_PARE2){
            sprintf(msg,"expect \")\" in function \"%s()\".",func.name);
            reportError(parser,msg);
        }
    }else{
        isEmpty=false;
    }
    var.vtype=VAR_ARGUMENT;
    while(!isEmpty){
        token=nextToken(parser);
        if(token.type!=TOKEN_WORD){
            sprintf(msg,"expect a argument name in function \"%s\".",func.name);
            reportError(parser,msg);
        }
        strcpy(var.name,token.word);
        /*检查参数名称重复*/
        for(int i=0;i<func.argument.count;i++){
            if(strcmp(var.name,func.argument.vals[i].name)==0){
                sprintf(msg,"the argument \"%s\" has already existed in function \"%s()\".",var.name,func.name);
                reportError(parser,msg);
            }
        }
        var.dim=0;
        token=nextToken(parser);
        var2=&var;
        while(token.type==TOKEN_BRACKET1){
            token=nextToken(parser);
            if(token.type!=TOKEN_BRACKET2){
                sprintf(msg,"expect \"]\" while declaring the argument \"%s\" in function \"%s\".",var.name,func.name);
                reportError(parser,msg);
            }
            var2->class=var.class;
            var2->ptr=func.partSize;
            var2->vtype=VAR_ARGUMENT;
            var2->subVar=(Variable*)malloc(sizeof(Variable));
            var2=var2->subVar;
            var.dim+=1;
            token=nextToken(parser);
        }
        var2=var.subVar;
        for(int i=0;i<var.dim;i++){
            var2->dim=var.dim-i-1;
            var2=var2->subVar;
        }
        func.partSize+=var.dim+1;
        LIST_ADD(func.argument,Variable,var);
        if(token.type==TOKEN_COMMA){
            getClass(parser,&var.class);
            continue;
        }else if(token.type==TOKEN_PARE2){
            break;
        }else{
            sprintf(msg,"expect \",\" or \")\" in function \"%s\".",func.name);
            reportError(parser,msg);
        }
    }
    /*检查重复或者未定义函数*/
    int ind;
    Function func2;
    bool isFound=false;
    bool isSame;
    for(int i=0;i<funcList->count;i++){
        func2=funcList->vals[i];
        if(strcmp(func.name,func2.name)==0 && func.argument.count==func2.argument.count){
            isSame=true;
            for(int i2=0;i2<func.argument.count;i2++){
                if(func.argument.vals[i2].class!=func2.argument.vals[i2].class || func.argument.vals[i2].dim!=func2.argument.vals[i2].dim){
                    isSame=false;
                }
            }
            if(isSame && !func2.isDef){
                isFound=true;
                ind=i;
                break;
                
            }else if(isSame && func2.isDef){
                getFuncName(parser,func2,temp);
                sprintf(msg,"the function \"%s\" has already existed.",temp);
                reportError(parser,msg);
            }
        }
    }
    if(isFound){
        funcList->vals[ind].isDef=true;
        funcList->vals[ind].ptr=func.ptr;
    }else{
        func.isDef=true;
        LIST_ADD((*funcList),Function,func);
        ind=funcList->count-1;
    }
    envirn.pvlist=&funcList->vals[ind].argument;
    envirn.breakList=NULL;
    envirn.partSize=funcList->vals[ind].partSize;
    envirn.func=&funcList->vals[ind];
    getBlock(parser,&parser->funcClist,envirn);
    if(funcList->vals[ind].partSize!=0){
        addCmd1(&parser->funcClist,HANDLE_SFREE,DATA_INTEGER,funcList->vals[ind].partSize);
    }
    /*返回*/
    addCmd2(&parser->funcClist,HANDLE_MOV,DATA_REG,DATA_INTEGER,REG_AX,0);
    addCmd1(&parser->funcClist,HANDLE_POP,DATA_REG,REG_BX);
    addCmd1(&parser->funcClist,HANDLE_JMPS,DATA_REG,REG_BX);
    return true;
}