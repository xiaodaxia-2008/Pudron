#include"vm.h"
#ifdef DEBUG
#define ASSERT(condition,message,...) \
    if(condition){\
        printf(message,##__VA_ARGS__);\
        getchar();\
    }
#else
#define ASSERT(condition,message,...)
#endif
#define checkStack(num) ASSERT(vm->stackCount<num,"%d:expected %d slots but only %d slots in stack.",vm->ptr,num,vm->stackCount)
VM newVM(char*fileName,PdSTD pstd){
    VM vm;
    vm.stackCount=0;
    vm.ptr=0;
    vm.part.fileName=fileName;
    vm.part.code=NULL;
    vm.part.line=0;
    vm.part.column=0;
    vm.part.start=0;
    vm.part.end=0;
    LIST_INIT(vm.plist)
    vm.this=NULL;
    vm.pstd=pstd;
    return vm;
}
void popStack(VM*vm,Unit*unit,int num){
    for(int i=0;i<num;i++){
        reduceRef(vm,unit,POP());
    }
}
void invertOrder(VM*vm,int count){
    checkStack(count);
    Object*st;
    int x=count/2;
    for(int i=0;i<x;i++){
        st=vm->stack[vm->stackCount-1-i];
        vm->stack[vm->stackCount-1-i]=vm->stack[vm->stackCount-count+i];
        vm->stack[vm->stackCount-count+i]=st;
    }
}
Object copyObject(VM*vm,Unit*unit,Object*obj){
    Object val=*obj;
    /*refCount与前面保持一致，可能别的地方还在引用(不知何用)
    val.refCount=refCount;*/
    val.refCount=1;
    val.isInit=false;
    LIST_INIT(val.classNameList);
    LIST_CONNECT(val.classNameList,obj->classNameList,Name)
    val.member=hashCopy(obj->member);
    HashSlot hs;
    for(int i=0;i<val.member.capacity;i++){
        hs=val.member.slot[i];
        if(hs.isUsed && hs.obj!=NULL){
            val.member.slot[i].obj=(Object*)memManage(NULL,sizeof(Object));
            *(val.member.slot[i].obj)=copyObject(vm,unit,hs.obj);
        }
    }
    if(obj->type==OBJECT_STRING){
        val.str=(char*)memManage(NULL,strlen(obj->str)+1);
        strcpy(val.str,obj->str);
    }else if(obj->type==OBJECT_LIST){
        Object*cnt=loadMember(vm,obj,"count",true);
        val.subObj=(Object**)memManage(NULL,cnt->num*sizeof(Object*));
        for(int i=0;i<cnt->num;i++){
            val.subObj[i]=obj->subObj[i];
        }
        reduceRef(vm,unit,cnt);
    }
    return val;
}
/*若argList为NULL,则不执行opInit()方法*/
Object*createObject(VM*vm,Unit*unit,Class class,ArgList*argList,int opcode){
    Object*this=newObject(OBJECT_OTHERS);
    this->member=hashCopy(class.memberList);
    LIST_ADD(this->classNameList,Name,class.name)
    /*继承父类*/
    Object*parent;
    Class classp;
    for(int i=0;i<class.parentList.count;i++){
        classp=class.parentList.vals[i];
        LIST_ADD(this->classNameList,Name,classp.name)
        parent=createObject(vm,unit,classp,NULL,opcode);
        setHash(vm,&this->member,classp.name,parent);
    }
    /*执行initFunc*/
    Unit funit=getFuncUnit(class.initFunc);
    funit.gvlist=hashMerge(unit->gvlist,unit->lvlist);
    funit.lvlist=hashCopy(funit.lvlist);
    setHash(vm,&funit.lvlist,"this",this);
    if(class.initFunc.exe!=NULL){
        class.initFunc.exe(vm,&funit);
    }else{
        execute(vm,&funit);
    }
    freeHashList(vm,unit,&funit.lvlist);
    free(funit.gvlist.slot);
    invertOrder(vm,class.varList.count);
    for(int i=0;i<class.varList.count;i++){
        setHash(vm,&this->member,class.varList.vals[i],POP());
    }
    /*执行opInit()方法*/
    if(argList!=NULL){
        Object*iobj=loadMember(vm,this,METHOD_NAME_INIT,false);
        if(iobj!=NULL){
            confirmObjectType(vm,iobj,OBJECT_FUNCTION);
            if(opcode==OPCODE_CALL_METHOD){
                argList->vals[0]=this;
            }else{
                LIST_INSERT((*argList),Arg,0,this)
            }
            this->refCount++;
            callFunction(vm,unit,iobj->func,-1,*argList);
            reduceRef(vm,unit,POP());
        }
        reduceRef(vm,unit,iobj);
    }
    return this;
}
#define EXE_OPT_BIT(opt,methodName) \
    if(a->type==OBJECT_INT){\
        confirmObjectType(vm,b,OBJECT_INT);\
        c=newIntObject(a->num opt b->num);\
        PUSH(c);\
        reduceRef(vm,unit,a);\
        reduceRef(vm,unit,b);\
    }else{\
        mName=methodName;\
    }

#define EXE_OPT(opt,methodName) \
    switch(a->type){\
        case OBJECT_INT:\
            switch(b->type){\
                case OBJECT_INT:\
                    c=newIntObject(a->num opt b->num);\
                    break;\
                case OBJECT_DOUBLE:\
                    c=newDoubleObject((double)a->num opt b->numd);\
                    break;\
                default:\
                    vmError(vm,"unsupported type for int operation");\
                    break;\
            }\
            reduceRef(vm,unit,a);\
            reduceRef(vm,unit,b);\
            PUSH(c);\
            break;\
        case OBJECT_DOUBLE:\
            switch(b->type){\
                case OBJECT_INT:\
                    c=newDoubleObject(a->numd opt b->num);\
                    break;\
                case OBJECT_DOUBLE:\
                    c=newDoubleObject(a->numd opt b->numd);\
                    break;\
                default:\
                    vmError(vm,"unsupported type for double operation");\
                    break;\
            }\
            reduceRef(vm,unit,a);\
            reduceRef(vm,unit,b);\
            PUSH(c);\
            break;\
        default:\
            mName=methodName;\
            break;\
    }

void exeOpt(VM*vm,Unit*unit,int opcode){
    checkStack(2);
    Object*a,*b,*c=NULL;
    char*mName=NULL;
    b=POP();
    a=POP();
    switch(opcode){
        case OPCODE_ADD:
            EXE_OPT(+,METHOD_NAME_ADD)
            break;
        case OPCODE_SUB:
            EXE_OPT(-,METHOD_NAME_SUB)
            break;
        case OPCODE_MUL:
            EXE_OPT(*,METHOD_NAME_MUL)
            break;
        case OPCODE_DIV:
            EXE_OPT(/,METHOD_NAME_DIV)
            break;
        case OPCODE_AND:
            EXE_OPT_BIT(&,METHOD_NAME_AND)
            break;
        case OPCODE_OR:
            EXE_OPT_BIT(|,METHOD_NAME_OR)
            break;
        case OPCODE_CAND:
            EXE_OPT(&&,METHOD_NAME_CAND)
            break;
        case OPCODE_COR:
            EXE_OPT(||,METHOD_NAME_COR)
            break;
        case OPCODE_LEFT:
            EXE_OPT_BIT(<<,METHOD_NAME_LEFT)
            break;
        case OPCODE_RIGHT:
            EXE_OPT_BIT(>>,METHOD_NAME_RIGHT)
            break;
        case OPCODE_EQUAL:
            EXE_OPT(==,METHOD_NAME_EQUAL)
            break;
        case OPCODE_GTHAN:
            EXE_OPT(>,METHOD_NAME_GTHAN)
            break;
        case OPCODE_LTHAN:
            EXE_OPT(<,METHOD_NAME_LTHAN)
            break;
        case OPCODE_REM:
            EXE_OPT_BIT(%,METHOD_NAME_REM)
            break;
        default:
            break;
    }
    if(mName!=NULL){
        Object*mobj=loadMember(vm,a,mName,true);
        confirmObjectType(vm,mobj,OBJECT_FUNCTION);
        if(mobj->func.argCount!=2){
            vmError(vm,"the count of operator method must be 1 but %d is here.",mobj->func.argCount);
        }
        callFunction(vm,unit,mobj->func,2,a,b);
    }
}
void assign(VM*vm,Unit*unit,int astype,int asc){
    checkStack(asc+1);
    Object*obj=POP();
    Object*val,*obj2;
    for(int i=0;i<asc;i++){
        if(astype==-1){
            val=POP();
            delObj(vm,unit,val);
            if(obj->isInit){
                *val=*obj;
                val->isInit=false;
            }else{
                *val=copyObject(vm,unit,obj);
            }
        }else{
            obj->refCount++;
            val=vm->stack[vm->stackCount-1];
            val->refCount++;
            PUSH(obj);
            exeOpt(vm,unit,astype);
            delObj(vm,unit,val);
            obj2=POP();
            *val=copyObject(vm,unit,obj2);
            reduceRef(vm,unit,obj2);
        }
    }
    if(astype==-1 && obj->isInit){
        free(obj);
    }else{
        reduceRef(vm,unit,obj);
    }
}
#define EXE_OPT_PREFIX(opt) \
    this=POP();\
    if(this->type==OBJECT_INT){\
        obj=newIntObject(opt this->num);\
    }else if(this->type==OBJECT_DOUBLE){\
        obj=newDoubleObject(opt this->numd);\
    }else{\
        vmError(vm,"unsupported prefix operation.");\
    }\
    PUSH(obj);\
    reduceRef(vm,unit,this);
void execute(VM*vm,Unit*unit){
    int c=0;
    ArgList argList;
    Object*obj=NULL,*this=NULL;
    int asc=1;
    char*name=NULL;
    for(int i=0;i<unit->clist.count;i++){
        c=unit->clist.vals[i];
        if(c>=0){
            vm->part=unit->plist.vals[c];
        }
        vm->ptr=i;
        //printf("cmd:%d\n",i);
        c=unit->clist.vals[++i];
        switch(c){
            case OPCODE_NOP:
                break;
            case OPCODE_ADD:
            case OPCODE_SUB:
            case OPCODE_MUL:
            case OPCODE_DIV:
            case OPCODE_AND:
            case OPCODE_OR:
            case OPCODE_CAND:
            case OPCODE_COR:
            case OPCODE_LEFT:
            case OPCODE_RIGHT:
            case OPCODE_EQUAL:
            case OPCODE_GTHAN:
            case OPCODE_LTHAN:
            case OPCODE_NOT_EQUAL:
            case OPCODE_GTHAN_EQUAL:
            case OPCODE_LTHAN_EQUAL:
            case OPCODE_REM:
                exeOpt(vm,unit,c);
                break;
            case OPCODE_INVERT:
                this=POP();
                confirmObjectType(vm,this,OBJECT_INT);
                obj=newIntObject(~this->num);
                PUSH(obj);
                reduceRef(vm,unit,this);
                break;
            case OPCODE_NOT:
                EXE_OPT_PREFIX(!)
                break;
            case OPCODE_SUBS:
                EXE_OPT_PREFIX(-)
                break;
            case OPCODE_LOAD_CONST:
                PUSH(loadConst(vm,unit,unit->clist.vals[++i]));
                break;
            case OPCODE_LOAD_VAR:
                name=unit->nlist.vals[unit->clist.vals[++i]];
                obj=NULL;
                if(vm->this!=NULL){
                    obj=loadMember(vm,vm->this,name,false);
                }
                if(obj==NULL){
                    obj=loadVar(vm,unit,name);
                }
                PUSH(obj);
                break;
            case OPCODE_LOAD_MEMBER:
            case OPCODE_LOAD_METHOD:
                this=POP();
                obj=loadMember(vm,this,unit->nlist.vals[unit->clist.vals[++i]],true);
                if(c==OPCODE_LOAD_METHOD){
                    PUSH(this);
                }else{
                    reduceRef(vm,unit,this);
                }
                PUSH(obj);
                break;
            case OPCODE_LOAD_SUBSCRIPT:
                c=unit->clist.vals[++i]+1;/*include this*/
                LIST_INIT(argList)
                invertOrder(vm,c);
                for(int i2=0;i2<c;i2++){
                    LIST_ADD(argList,Arg,POP())
                }
                obj=loadMember(vm,this,METHOD_NAME_SUBSCRIPT,true);
                confirmObjectType(vm,obj,OBJECT_FUNCTION);
                callFunction(vm,unit,obj->func,-1,argList);
                break;
            case OPCODE_STACK_COPY:
                obj=vm->stack[vm->stackCount-unit->clist.vals[++i]-1];
                obj->refCount++;
                PUSH(obj);
                break;
            case OPCODE_POP_STACK:
                popStack(vm,unit,unit->clist.vals[++i]);
                break;
            case OPCODE_JUMP:
                i++;
                i=unit->clist.vals[i]-1;
                break;
            case OPCODE_JUMP_IF_FALSE:
                obj=POP();
                confirmObjectType(vm,obj,OBJECT_INT);
                i++;
                if(!obj->num){
                    i=unit->clist.vals[i]-1;
                }
                reduceRef(vm,unit,obj);
                break;
            case OPCODE_CALL_METHOD:
            case OPCODE_CALL_FUNCTION:{
                Object*oldThis;
                LIST_ADD(vm->plist,Part,vm->part)
                int argc=unit->clist.vals[++i];
                invertOrder(vm,argc);
                LIST_INIT(argList)
                if(c==OPCODE_CALL_METHOD){
                    this=vm->stack[vm->stackCount-argc-2];
                    LIST_ADD(argList,Arg,this);/*add this*/
                    oldThis=vm->this;
                    vm->this=this;
                }
                for(int i2=0;i2<argc;i2++){
                    LIST_ADD(argList,Arg,POP())
                }
                obj=POP();/*pop func*/
                if(c==OPCODE_CALL_METHOD){
                    this=POP();/*pop this*/
                }
                if(obj->type==OBJECT_FUNCTION){
                    callFunction(vm,unit,obj->func,-1,argList);
                }else if(obj->type==OBJECT_CLASS){
                    PUSH(createObject(vm,unit,obj->class,&argList,c));
                }else{
                    vmError(vm,"expected class or function when calling.");
                }
                if(c==OPCODE_CALL_METHOD){
                    vm->this=oldThis;
                }
                LIST_SUB(vm->plist,Part)
                reduceRef(vm,unit,obj);
                break;
            }
            case OPCODE_RETURN:
                return;
            case OPCODE_INVERT_ORDER:
                invertOrder(vm,unit->clist.vals[++i]);
                break;
            case OPCODE_SET_ASSIGN_COUNT:
                asc=unit->clist.vals[++i];
                break;
            case OPCODE_ASSIGN:
                assign(vm,unit,unit->clist.vals[++i],asc);
                asc=1;
                break;
            case OPCODE_MAKE_ARRAY:{
                c=unit->clist.vals[++i];
                this=newListObject(vm,c);
                invertOrder(vm,c);
                for(int i2=0;i2<c;i2++){
                    this->subObj[i2]=POP();
                }
                PUSH(this);
                break;
            }
            case OPCODE_GET_FOR_INDEX:{
                checkStack(3);
                Object*array=vm->stack[vm->stackCount-2];
                Object*sum=vm->stack[vm->stackCount-1];
                confirmObjectType(vm,array,OBJECT_LIST);
                Object*cnt=loadMember(vm,array,"count",true);
                Object*rt=newIntObject(0);
                if(sum->num<cnt->num){
                    vm->stack[vm->stackCount-3]=array->subObj[sum->num++];
                    reduceRef(vm,unit,vm->stack[vm->stackCount-3]);
                    vm->stack[vm->stackCount-3]->refCount++;
                    rt->num=true;
                }else{
                    popStack(vm,unit,3);
                    rt->num=false;
                }
                reduceRef(vm,unit,cnt);
                PUSH(rt);
                break;
            }
            case OPCODE_CLASS_EXTEND:
                name=unit->nlist.vals[unit->clist.vals[++i]];
                this=vm->stack[vm->stackCount-1];
                obj=loadVar(vm,unit,name);
                confirmObjectType(vm,obj,OBJECT_CLASS);
                LIST_ADD(this->class.parentList,Class,obj->class)
                reduceRef(vm,unit,obj);
                break;
            default:
                vmError(vm,"unknown opcode %d.",c);
                break;
        }
    }
}