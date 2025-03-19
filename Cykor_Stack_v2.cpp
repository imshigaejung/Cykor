
/*  call_stack
    
    실제 시스템에서는 스택이 메모리에 저장되지만, 본 과제에서는 `int` 배열을 이용하여 메모리를 구현합니다.
    원래는 SFP와 Return Address에 실제 가상 메모리 주소가 들어가겠지만, 이번 과제에서는 -1로 대체합니다.
    
    int call_stack[]      : 실제 데이터(`int 값`) 또는 `-1` (메타데이터 구분용)을 저장하는 int 배열 
    char stack_info[][]   : call_stack[]과 같은 위치(index)에 대한 설명을 저장하는 문자열 배열

    ==========================call_stack 저장 규칙==========================
    매개 변수 / 지역 변수를 push할 경우   : int 값 그대로
    Saved Frame Pointer 를 push할 경우  : call_stack에서의 index
    반환 주소값을 push할 경우       : -1
    =======================================================================
    

    ==========================stack_info 저장 규칙==========================
    매개 변수 / 지역 변수를 push할 경우        : 변수에 대한 설명
    Saved Frame Pointer 를 push할 경우  : 어떤 함수의 SFP인지
    반환 주소값을 push할 경우                 : "Return Address"
    ========================================================================
*/
#include <stdio.h>
#include <string.h>
#define STACK_SIZE 50 // 최대 스택 크기

int     call_stack[STACK_SIZE];         // Call Stack을 저장하는 배열
char    stack_info[STACK_SIZE][20];     // Call Stack 요소에 대한 설명을 저장하는 배열

/*  SP (Stack Pointer), FP (Frame Pointer)

    SP는 현재 스택의 최상단 인덱스를 가리킵니다.
    스택이 비어있을 때 SP = -1, 하나가 쌓이면 `call_stack[0]` -> SP = 0, `call_stack[1]` -> SP = 1, ...

    FP는 현재 함수의 스택 프레임 포인터입니다.
    실행 중인 함수 스택 프레임의 sfp를 가리킵니다.
*/
int SP = -1; 
int FP = -1;

void func1(int arg1, int arg2, int arg3);
void func2(int arg1, int arg2);
void func3(int arg1);

/*  
    현재 call_stack 전체를 출력합니다.
    해당 함수의 출력 결과들을 바탕으로 구현 완성도를 평가할 예정입니다.
*/
void push(int push_list[], int len, char type) {

    //변수, 함수 이름 뒤에 숫자를 붙이기 위해 임시로 이름 저장하는 버퍼
    char temp[10];

    //지역 변수와 함수 이름 뒤의 숫자는 push 함수 종료 시에도 초기화 X
    static int local = 1;
    static int func = 1;
        

    //stack overflow 방지
    if (SP-1 >= STACK_SIZE) {
	return;
    }

    //매개 변수, 지역 변수 구분해서 call_stack과 stack_info 설정

    if (type == 'p') {
        for (int i = len; i > 0; i--) {
            //매개변수에 값을 할당하면서 SP값을 올림
            SP++;
            call_stack[SP] = push_list[i - 1];
            sprintf_s(temp, "arg%d", i);
            strcpy_s(stack_info[SP], temp);
        }
    }
    else if (type == 'l') {
        for (int i = 0; i < len; i++) {
            //지역변수에 값을 할당하면서 SP값을 올림
            SP++;
            call_stack[SP] = push_list[i];
            sprintf_s(temp, "var_%d", local);
            strcpy_s(stack_info[SP], temp);
            local++;
        }
    }
    else if (type == 'f'){

        for (int i = len; i > 0; i--) {
            //sfp에 값을 할당하면서 SP값을 올림
            SP++;
            call_stack[SP] = FP;
            sprintf_s(temp, "func%d SFP", func);
            strcpy_s(stack_info[SP], temp);
            func++;
            FP = SP;

        }
    }
    else if (type == 's') {
        //귀환 주소의 call_stack값인 -1 할당, stack_info에 Return Address 할당
        SP++;
        call_stack[SP] = -1; 
        strcpy_s(stack_info[SP], "Return Address");
    }
}

void pop(int len, char type) {

    if (type == 'l') {
        while (SP != FP) {
            //지역변수에 값을 할당하면서 SP값을 내림
            call_stack[SP] = 0;
            strcpy_s(stack_info[SP], "");
            SP--;
        }
    }
    else if (type == 'f') {
        FP = call_stack[SP];
        call_stack[SP] = 0;
        strcpy_s(stack_info[SP], "");
        SP--;
    }
    else if (type == 's') {
        call_stack[SP] = 0;
        strcpy_s(stack_info[SP], "");
        SP--;
    }
    else if (type == 'p') {

        for (int i = len; i > 0; i--) {
            //매개변수에 값을 할당하면서 SP값을 내림
            call_stack[SP] = 0;
            strcpy_s(stack_info[SP], "");
            SP--;
        }

    }

}

void prologue(int param_list[], int local_list[], int par_len, int loc_len) {
    //push 함수에 형식에 맞춰 집어넣기 위해 딱히 리스트를 제공받지 않는 sfp와 반환 주소를 위한 공백 리스트
    int NULL_list[1] = {};
    //매개 변수 push
    push(param_list, par_len, 'p');
    //반환 주소 push
    push(NULL_list, 1, 's');
    //SFP push
    push(NULL_list , 1, 'f');
    //지역 변수 push
    push(local_list, loc_len, 'l');
}

void epilogue(int par_len) {
   
    //지역 변수 pop
    pop(0,'l');
    //SFP pop
    pop(0,'f');
    //반환 주소 pop
    pop(0,'s');
    //매개 변수 pop
    pop(par_len, 'p');
}

void print_stack()
{
    if (SP == -1)
    {
        printf("======Stack is empty.======\n");
        return;
    }
    else if (SP-1 >= STACK_SIZE) {
	printf("======Stack Overflow!!!======\n");
	return;
	}

    printf("====== Current Call Stack ======\n");
    
    for (int i = SP; i >= 0; i--)
    {
        if (call_stack[i] != -1)
            printf("%d : %s = %d", i ,stack_info[i], call_stack[i]);
        else
            printf("%d : %s", i, stack_info[i]);

        if (i == SP)
            printf("    <=== [esp]\n");
        else if (i == FP)
            printf("    <=== [ebp]\n");
        else
            printf("\n");
    }
    printf("================================\n\n");
}


//func 내부는 자유롭게 추가해도 괜찮으나, 아래의 구조를 바꾸지는 마세요
void func1(int arg1, int arg2, int arg3)
{
    int var_1 = 100;
    // 직접 제작 시작
    int param_list[3] = { arg1,arg2,arg3 };
    int local_list[1] = { var_1 };
    
    // func1의 스택 프레임 형성 (함수 프롤로그 + push)
    prologue(param_list, local_list, 3, 1);
    print_stack();

    
    func2(11, 13);

    // func2의 스택 프레임 제거 (함수 에필로그 + pop)
    epilogue(2);
    print_stack();
}


void func2(int arg1, int arg2)
{
    int var_2 = 200;
    //
    int param_list[2] = { arg1,arg2,};
    int local_list[1] = { var_2 };
    
    // func2의 스택 프레임 형성 (함수 프롤로그 + push)
    prologue(param_list, local_list, 2, 1);
    print_stack();


    func3(77);

    
    // func3의 스택 프레임 제거 (함수 에필로그 + pop)
    epilogue(1);
    print_stack();
}


void func3(int arg1)
{
    int var_3 = 300;
    int var_4 = 400;

    int param_list[1] = { arg1 };
    int local_list[2] = { var_3,var_4 };
  
    // func3의 스택 프레임 형성 (함수 프롤로그 + push)
    prologue(param_list, local_list, 1, 2);
    print_stack();

}



//main 함수에 관련된 stack frame은 구현하지 않아도 됩니다.
int main()
{
    func1(1, 2, 3);
    
    // func1의 스택 프레임 제거 (함수 에필로그 + pop)
    epilogue(3);
    print_stack();
    return 0;
}
