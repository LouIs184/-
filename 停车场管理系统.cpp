#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> // for usleep

#define stacksize 10

int loc[50] = {-1}; //用于记录位置，loc[i]表示编号为i的车辆的存储位置,初始时时0，表示当前没有车辆 
// 若loc[i] mod 2 == 0，则i位于栈中，此时loc[i]/2即为i在栈中的位置
// 若loc[i] mod 2 == 1，则i位于等候队列中，此时loc[i]/2（相当于(loc[i]-1)/2）即为i在队列中的位置

typedef struct sqstack {
    int data[stacksize];
    int top;
} SqStackTp; // 顺序栈  
typedef struct linked_queue {
    int data;
    struct linked_queue *next;
} LqueueTp;

typedef struct {
	int queue_car_num;
    LqueueTp *front, *rear;
} QueptrTp; // 等待链队列 

void init_stack(SqStackTp* &s) {  //初始化栈 
    s = (SqStackTp*)malloc(sizeof(SqStackTp));
    s->top = -1;
}

void destroy_stack(SqStackTp* &s) { //销毁栈 
    free(s);
}

bool stack_is_empty(SqStackTp* &s) { //判断栈空 
    return (s->top == -1);
}

bool stack_is_full(SqStackTp* &s) { //判断栈满 
    return (s->top == stacksize - 1);
}

bool push_stack(SqStackTp* s, int e) { //进栈 
    if(stack_is_full(s)) return false;
    s->top++;
    s->data[s->top] = e;
    return true;
}

int pop_stack(SqStackTp* s) { // 出栈，并取栈顶元素 
    int e = s->data[s->top];
    s->top--;
    return e;
}

int get_stack_top(SqStackTp* s) { // 取栈顶元素 
    if(s->top == -1) return 0;
    return s->data[s->top];
}

void init_queue(QueptrTp* &q) { // 初始化队列 
    q = (QueptrTp*)malloc(sizeof(QueptrTp));
    q->queue_car_num = 0;
    q->front = q->rear = NULL;
}

void destroy_queue(QueptrTp* &q) { // 销毁队列 
    LqueueTp* pre = q->front, *p;
    if(pre == NULL) return;
    p = pre->next;
    while(p != NULL) {
        free(pre);
        pre = p;
        p = p->next;
    }
    free(pre);
    free(q);
}

bool queue_is_empty(QueptrTp* &q) { // 判断队列空 
    return (q->rear == NULL);
}

bool en_queue(QueptrTp* &q, int e) { // 进队列 
    LqueueTp* p;
    p = (LqueueTp*)malloc(sizeof(LqueueTp));
    p->data = e;
    p->next = NULL;
    if(q->rear == NULL) {
        q->front = q->rear = p;
    } else {
        q->rear->next = p;
        q->rear = p;
    }
    q->queue_car_num++; // 进队列时增加计数
    return true;
}

int de_queue(QueptrTp* &q) { // 出队列，取出队列的元素 
    int e;
    if(q->rear == q->front) {
        e = q->rear->data; 
        q->rear = q->front = NULL; 
    } else {
        LqueueTp* p = q->front;
        e = p->data;
        q->front = q->front->next;
        free(p);
    }
    q->queue_car_num--; // 出队列时减少计数
    return e;
}

void clear_screen() { // 清屏，达到动画效果 
    printf("\033[H\033[J");
}

void show(SqStackTp* parking_lot, SqStackTp* temp_lot, QueptrTp* waiting_queue) { // 展示基本界面 
    clear_screen();
    printf("当前停车场中的车辆序号: ");
    for(int i = 0; i <= parking_lot->top; i++) {
        printf("%d ", parking_lot->data[i]);
    }
    printf("\n");
    
    printf("临时栈中的车辆序号: ");
    for(int i = 0; i <= temp_lot->top; i++) {
        printf("%d ", temp_lot->data[i]);
    }
    printf("\n");
    
    printf("等待队列中的车辆序号: ");
    LqueueTp* p = waiting_queue->front;
    while(p != NULL) {
        printf("%d ", p->data);
        p = p->next;
    }
    printf("\n");
}

bool prompt_user_decision(const char* message) { // 让用户判断是否需要继续进入停车场 
    char decision;
    printf("%s (y/n): ", message);
    scanf(" %c", &decision);
    return decision == 'y' || decision == 'Y';
}

int del_num = 0; // 队列中已删除的车辆数量 

void enpark(SqStackTp* parking_lot, SqStackTp* temp_lot, QueptrTp* waiting_queue, int car_num) { //进入停车场 
    if(stack_is_full(parking_lot)) { //若停车场已满，需要计算链队中的元素数量 
        int waiting_count = waiting_queue -> queue_car_num;
        printf("停车场已满，车牌号 %d 需要排队，前面还有 %d 辆车\n", car_num, waiting_count);
        if(prompt_user_decision("是否进入等待队列")) {
            en_queue(waiting_queue, car_num);
            loc[car_num] = (waiting_count + del_num + 1) * 2 + 1; // 更新car_num的位置，loc数组存储队列中的位置是实际位置*2+1后的值 
            printf("车牌号 %d 已进入等待队列\n", car_num);
        } else {
            printf("车牌号 %d 放弃进入停车场\n", car_num);
        }
    } else {
        printf("停车场未满，目前有 %d 辆车\n", parking_lot->top + 1);
        if(prompt_user_decision("是否进入停车场")) {
            push_stack(parking_lot, car_num);
            loc[car_num] = (parking_lot->top + 1) * 2;//  更新car_num的位置，loc数组存储栈中的位置是实际位置*2后的值 
            printf("车牌号 %d 已进入停车场\n", car_num);
        } else {
            printf("车牌号 %d 放弃进入停车场\n", car_num);
        }
    }
    show(parking_lot, temp_lot, waiting_queue);
    usleep(500000); // 模拟动画效果，暂停0.5秒
}

void depark(SqStackTp* parking_lot, SqStackTp* temp_lot, QueptrTp* waiting_queue, int car_num) { // 离开停车场 
    int num, temp_car_num;
    while(get_stack_top(parking_lot) != car_num) {
        // push_stack(temp_lot, pop_stack(parking_lot)); //从停车场出栈，并进入临时栈 
        temp_car_num = pop_stack(parking_lot);
        loc[temp_car_num] = loc[temp_car_num] - 2; // 在出栈的同时更新了loc数组 
        push_stack(temp_lot,temp_car_num); 

        show(parking_lot, temp_lot, waiting_queue);
        usleep(500000); // 模拟动画效果，暂停0.5秒
    }
    
    pop_stack(parking_lot); // 指定的汽车出栈
    loc[car_num] = -1; // 指定汽车离开停车场，其loc值应当更新为-1 
    show(parking_lot, temp_lot, waiting_queue); 
    usleep(500000); // 模拟动画效果，暂停0.5秒
    
    while(!stack_is_empty(temp_lot)) {
        push_stack(parking_lot, pop_stack(temp_lot)); // 从临时栈出栈，并进入停车场
        show(parking_lot, temp_lot, waiting_queue);
        usleep(500000); // 模拟动画效果，暂停0.5秒
    }
 
    if(!queue_is_empty(waiting_queue)) {
    	// push_stack(parking_lot, de_queue(waiting_queue)); // 队列不空则取出队头车辆，进入停车场
    	del_num++; // 已删除的车的数量增加一个 
    	temp_car_num = de_queue(waiting_queue);
    	push_stack(parking_lot, temp_car_num);
    	loc[temp_car_num] = (parking_lot->top + 1) * 2; // 更新car_num的位置
        show(parking_lot, temp_lot, waiting_queue);
        usleep(500000); // 模拟动画效果，暂停0.5秒
    }
}

void find_car(int car_num)
{
	if(loc[car_num] % 2 == 0) // 在栈里
		printf("\n%d在栈中，处于第%d个位置", car_num, loc[car_num]/2);
	else
		printf("\n%d在等候队列中，处于第%d个位置", car_num, (loc[car_num]-del_num*2)/2);
		
	// 等待用户确认
    char confirm;
    printf("\n请确认查找结果 (输入y确认): ");
    scanf(" %c", &confirm);
    if (confirm != 'Y' && confirm != 'y') {
        printf("用户未确认。\n");
    }
} 

int main() {
    SqStackTp* parking_lot, *temp_lot;
    QueptrTp* waiting_queue;
    init_stack(parking_lot);
    init_stack(temp_lot);
    init_queue(waiting_queue);

    int command, car_num, count = 1;
    int find_car_num;
    printf("请输入指令和车牌号，中间用空格隔开\n");
    printf("1表示进入停车场，2表示离开停车场，3表示查找车辆位置，0表示退出程序\n");
    scanf("%d %d", &command, &car_num);
    while(1) {
        if(command == 1) {enpark(parking_lot, temp_lot, waiting_queue, car_num);}
        else if(command == 2) {depark(parking_lot, temp_lot, waiting_queue, car_num);}
        else if(command == 0) {printf("---------------------------------------------------已退出---------------------------------------------------\n"); break;}
        else if(command == 3) 
		{
            find_car(car_num);
            clear_screen(); // 清屏

		}
        else printf("请输入正确的指令\n");
        show(parking_lot, temp_lot, waiting_queue);
        printf("请输入指令和车牌号，中间用空格隔开\n");
        printf("1表示进入停车场，2表示离开停车场，3表示查找车辆位置，0表示退出程序\n");
        scanf("%d %d", &command, &car_num);
    }

    destroy_stack(parking_lot);
    destroy_stack(temp_lot);
    destroy_queue(waiting_queue);
}

