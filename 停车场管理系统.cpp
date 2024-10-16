#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> // for usleep

#define stacksize 10

int loc[50] = {-1}; //���ڼ�¼λ�ã�loc[i]��ʾ���Ϊi�ĳ����Ĵ洢λ��,��ʼʱʱ0����ʾ��ǰû�г��� 
// ��loc[i] mod 2 == 0����iλ��ջ�У���ʱloc[i]/2��Ϊi��ջ�е�λ��
// ��loc[i] mod 2 == 1����iλ�ڵȺ�����У���ʱloc[i]/2���൱��(loc[i]-1)/2����Ϊi�ڶ����е�λ��

typedef struct sqstack {
    int data[stacksize];
    int top;
} SqStackTp; // ˳��ջ  
typedef struct linked_queue {
    int data;
    struct linked_queue *next;
} LqueueTp;

typedef struct {
	int queue_car_num;
    LqueueTp *front, *rear;
} QueptrTp; // �ȴ������� 

void init_stack(SqStackTp* &s) {  //��ʼ��ջ 
    s = (SqStackTp*)malloc(sizeof(SqStackTp));
    s->top = -1;
}

void destroy_stack(SqStackTp* &s) { //����ջ 
    free(s);
}

bool stack_is_empty(SqStackTp* &s) { //�ж�ջ�� 
    return (s->top == -1);
}

bool stack_is_full(SqStackTp* &s) { //�ж�ջ�� 
    return (s->top == stacksize - 1);
}

bool push_stack(SqStackTp* s, int e) { //��ջ 
    if(stack_is_full(s)) return false;
    s->top++;
    s->data[s->top] = e;
    return true;
}

int pop_stack(SqStackTp* s) { // ��ջ����ȡջ��Ԫ�� 
    int e = s->data[s->top];
    s->top--;
    return e;
}

int get_stack_top(SqStackTp* s) { // ȡջ��Ԫ�� 
    if(s->top == -1) return 0;
    return s->data[s->top];
}

void init_queue(QueptrTp* &q) { // ��ʼ������ 
    q = (QueptrTp*)malloc(sizeof(QueptrTp));
    q->queue_car_num = 0;
    q->front = q->rear = NULL;
}

void destroy_queue(QueptrTp* &q) { // ���ٶ��� 
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

bool queue_is_empty(QueptrTp* &q) { // �ж϶��п� 
    return (q->rear == NULL);
}

bool en_queue(QueptrTp* &q, int e) { // ������ 
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
    q->queue_car_num++; // ������ʱ���Ӽ���
    return true;
}

int de_queue(QueptrTp* &q) { // �����У�ȡ�����е�Ԫ�� 
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
    q->queue_car_num--; // ������ʱ���ټ���
    return e;
}

void clear_screen() { // �������ﵽ����Ч�� 
    printf("\033[H\033[J");
}

void show(SqStackTp* parking_lot, SqStackTp* temp_lot, QueptrTp* waiting_queue) { // չʾ�������� 
    clear_screen();
    printf("��ǰͣ�����еĳ������: ");
    for(int i = 0; i <= parking_lot->top; i++) {
        printf("%d ", parking_lot->data[i]);
    }
    printf("\n");
    
    printf("��ʱջ�еĳ������: ");
    for(int i = 0; i <= temp_lot->top; i++) {
        printf("%d ", temp_lot->data[i]);
    }
    printf("\n");
    
    printf("�ȴ������еĳ������: ");
    LqueueTp* p = waiting_queue->front;
    while(p != NULL) {
        printf("%d ", p->data);
        p = p->next;
    }
    printf("\n");
}

bool prompt_user_decision(const char* message) { // ���û��ж��Ƿ���Ҫ��������ͣ���� 
    char decision;
    printf("%s (y/n): ", message);
    scanf(" %c", &decision);
    return decision == 'y' || decision == 'Y';
}

int del_num = 0; // ��������ɾ���ĳ������� 

void enpark(SqStackTp* parking_lot, SqStackTp* temp_lot, QueptrTp* waiting_queue, int car_num) { //����ͣ���� 
    if(stack_is_full(parking_lot)) { //��ͣ������������Ҫ���������е�Ԫ������ 
        int waiting_count = waiting_queue -> queue_car_num;
        printf("ͣ�������������ƺ� %d ��Ҫ�Ŷӣ�ǰ�滹�� %d ����\n", car_num, waiting_count);
        if(prompt_user_decision("�Ƿ����ȴ�����")) {
            en_queue(waiting_queue, car_num);
            loc[car_num] = (waiting_count + del_num + 1) * 2 + 1; // ����car_num��λ�ã�loc����洢�����е�λ����ʵ��λ��*2+1���ֵ 
            printf("���ƺ� %d �ѽ���ȴ�����\n", car_num);
        } else {
            printf("���ƺ� %d ��������ͣ����\n", car_num);
        }
    } else {
        printf("ͣ����δ����Ŀǰ�� %d ����\n", parking_lot->top + 1);
        if(prompt_user_decision("�Ƿ����ͣ����")) {
            push_stack(parking_lot, car_num);
            loc[car_num] = (parking_lot->top + 1) * 2;//  ����car_num��λ�ã�loc����洢ջ�е�λ����ʵ��λ��*2���ֵ 
            printf("���ƺ� %d �ѽ���ͣ����\n", car_num);
        } else {
            printf("���ƺ� %d ��������ͣ����\n", car_num);
        }
    }
    show(parking_lot, temp_lot, waiting_queue);
    usleep(500000); // ģ�⶯��Ч������ͣ0.5��
}

void depark(SqStackTp* parking_lot, SqStackTp* temp_lot, QueptrTp* waiting_queue, int car_num) { // �뿪ͣ���� 
    int num, temp_car_num;
    while(get_stack_top(parking_lot) != car_num) {
        // push_stack(temp_lot, pop_stack(parking_lot)); //��ͣ������ջ����������ʱջ 
        temp_car_num = pop_stack(parking_lot);
        loc[temp_car_num] = loc[temp_car_num] - 2; // �ڳ�ջ��ͬʱ������loc���� 
        push_stack(temp_lot,temp_car_num); 

        show(parking_lot, temp_lot, waiting_queue);
        usleep(500000); // ģ�⶯��Ч������ͣ0.5��
    }
    
    pop_stack(parking_lot); // ָ����������ջ
    loc[car_num] = -1; // ָ�������뿪ͣ��������locֵӦ������Ϊ-1 
    show(parking_lot, temp_lot, waiting_queue); 
    usleep(500000); // ģ�⶯��Ч������ͣ0.5��
    
    while(!stack_is_empty(temp_lot)) {
        push_stack(parking_lot, pop_stack(temp_lot)); // ����ʱջ��ջ��������ͣ����
        show(parking_lot, temp_lot, waiting_queue);
        usleep(500000); // ģ�⶯��Ч������ͣ0.5��
    }
 
    if(!queue_is_empty(waiting_queue)) {
    	// push_stack(parking_lot, de_queue(waiting_queue)); // ���в�����ȡ����ͷ����������ͣ����
    	del_num++; // ��ɾ���ĳ�����������һ�� 
    	temp_car_num = de_queue(waiting_queue);
    	push_stack(parking_lot, temp_car_num);
    	loc[temp_car_num] = (parking_lot->top + 1) * 2; // ����car_num��λ��
        show(parking_lot, temp_lot, waiting_queue);
        usleep(500000); // ģ�⶯��Ч������ͣ0.5��
    }
}

void find_car(int car_num)
{
	if(loc[car_num] % 2 == 0) // ��ջ��
		printf("\n%d��ջ�У����ڵ�%d��λ��", car_num, loc[car_num]/2);
	else
		printf("\n%d�ڵȺ�����У����ڵ�%d��λ��", car_num, (loc[car_num]-del_num*2)/2);
		
	// �ȴ��û�ȷ��
    char confirm;
    printf("\n��ȷ�ϲ��ҽ�� (����yȷ��): ");
    scanf(" %c", &confirm);
    if (confirm != 'Y' && confirm != 'y') {
        printf("�û�δȷ�ϡ�\n");
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
    printf("������ָ��ͳ��ƺţ��м��ÿո����\n");
    printf("1��ʾ����ͣ������2��ʾ�뿪ͣ������3��ʾ���ҳ���λ�ã�0��ʾ�˳�����\n");
    scanf("%d %d", &command, &car_num);
    while(1) {
        if(command == 1) {enpark(parking_lot, temp_lot, waiting_queue, car_num);}
        else if(command == 2) {depark(parking_lot, temp_lot, waiting_queue, car_num);}
        else if(command == 0) {printf("---------------------------------------------------���˳�---------------------------------------------------\n"); break;}
        else if(command == 3) 
		{
            find_car(car_num);
            clear_screen(); // ����

		}
        else printf("��������ȷ��ָ��\n");
        show(parking_lot, temp_lot, waiting_queue);
        printf("������ָ��ͳ��ƺţ��м��ÿո����\n");
        printf("1��ʾ����ͣ������2��ʾ�뿪ͣ������3��ʾ���ҳ���λ�ã�0��ʾ�˳�����\n");
        scanf("%d %d", &command, &car_num);
    }

    destroy_stack(parking_lot);
    destroy_stack(temp_lot);
    destroy_queue(waiting_queue);
}

