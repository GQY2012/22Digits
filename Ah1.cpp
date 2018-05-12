#pragma warning(disable:4996)
#define _SILENCE_STDEXT_HASH_DEPRECATION_WARNINGS

#include <iostream>
#include <cstdio>
#include <ctime>
#include <cstring>
#include <queue>
#include <hash_map>
#include <Windows.h>

#define N 5
using namespace std;

typedef struct searchTree {
	char (*state)[N]; //��ǰ���״̬
	struct searchTree *parent;	 //ָ�򸸽ڵ�
	char action;	 //���ɸýڵ�Ķ���
	short pathcost;	 //����˽ڵ��ѻ��ѵĴ���g(n)
	short h;			 //��ǰ״̬����λ��������h(n)
	boolean *move;	 //���ƶ��ķ���
	char *position;	 //0���ڵ�λ��
}Node;

struct cmp {
	boolean operator() (Node const *n1, Node const *n2) { //�������ȶ��еıȽϷ���
		if ((n1->pathcost + n1->h) == (n2->pathcost + n2->h))
			return n1->pathcost < n2->pathcost;
		return (n1->pathcost + n1->h) > (n2->pathcost + n2->h);
	}
};

//clock_t t1, t2;
LARGE_INTEGER nFreq;//cpu frequency  
LARGE_INTEGER t1;//begin
LARGE_INTEGER t2;//end
char ac[] = { 'U','D','L','R' };
int movecount = 0; //�ƶ��ܴ���
int tries;			//��չ����
int repeat;			//���ش���

int hashValue(char(*)[N]);			 //hash����
int(*filesInput(const char*))[N];	 //���ļ�
Node *getNode(Node*, char(*)[N], char);//�����ӽڵ�
void getPosition(Node*, char n = '0');  //��������n����λ��
char (*doAction(Node*, char))[N];	 //�ƶ�
short geth(char(*)[N], char(*)[N]);      //���ص�ǰ״̬����λ��������
void getActionBool(Node*);			 //���ص�ǰ״̬�ɽ��еĶ���
void printAction(Node*);			 //��ӡ�ƶ�·��


int main() {
	const char *pathInput = "input.txt";
	const char *pathTarget = "target.txt";
	
	int (*startint)[N] = filesInput(pathInput);
	int (*goalint)[N] = filesInput(pathTarget);
	char (*start)[N] = new char[N][N];
	char (*goal)[N] = new char[N][N];
	for (int i = 0;i < N;i++) {
		for (int j = 0;j < N;j++) {
			start[i][j] = startint[i][j] + '0';
			goal[i][j] = goalint[i][j] + '0';
		}
	}
	delete[] startint, goalint;

	priority_queue<Node*,vector<Node*>,cmp> q;//���ȶ���
	hash_map<int, boolean> hashmap;
	/*��ʼ���*/
	Node root;
	Node *t = &root;
	t->state = start;
	t->action = NULL;
	t->parent = NULL;
	t->pathcost = 0;
	t->h = geth(start, goal);
	getPosition(t);
	getActionBool(t);

	q.push(t);

	QueryPerformanceFrequency(&nFreq);//��ȡcpuƵ��
	QueryPerformanceCounter(&t1);//�㷨��ʼʱ��
//	t1 = clock();
	while (!q.empty()) {
		Node *n = NULL;
		n = q.top();
		q.pop();
		hashmap[hashValue(n->state)] = true;
		for (int i = 0;i < 4;i++) {
			if (n->move[0] == false && ac[i] == 'U'
				|| n->move[1] == false && ac[i] == 'D'
				|| n->move[2] == false && ac[i] == 'L'
				|| n->move[3] == false && ac[i] == 'R')
				continue;
			
			t = getNode(n, goal, ac[i]);
			tries++;
			if (t->h == 0)
				goto end;
			if (!hashmap[hashValue(t->state)]) {
					q.push(t);
			}
			else {
				repeat++;
				delete t;
			}			
		}
	}
end:
//	t2 = clock();
	QueryPerformanceCounter(&t2);//�㷨����ʱ��
	freopen("output_Ah1.txt", "w", stdout);
	//�ض����������ָ���ļ�
	printf("%fs\n", (t2.QuadPart - t1.QuadPart) / (double)nFreq.QuadPart);	
	printAction(t);
	printf("\n%d\n", movecount);
	freopen("CON", "w", stdout);
	//�ض����������windows����̨

//	cout << (double)(t2 - t1) / CLOCKS_PER_SEC << endl;
	cout << (t2.QuadPart - t1.QuadPart) / (double)nFreq.QuadPart << "s" << endl;
	movecount = 0;
	printAction(t);

	cout << endl << "�ƶ�����:" << movecount << endl;
	cout << "��չ����:" << tries << endl;
	cout << "���ش���:" << repeat << endl;
	system("pause");
	return 0;
}

int hashValue(char (*state)[N]) {
	int hash = 0;
	for (int i = 0;i < N;i++) {
		for (int j = 0;j < N;j++) {
			hash = hash*10 + state[i][j] - '0';
		}
	}
	return hash;
}

int(*filesInput(const char* filePath))[N] {
	freopen(filePath, "r", stdin);//�ض�����������ָ���ļ�
	int(*p)[N] = new int[N][N];
	for (int i = 0;i < N;i++) {
		for (int j = 0;j < N;j++) {
			scanf("%d", &p[i][j]);
		}
	}
	freopen("CON", "r", stdin);//�ض�����������windows����̨
	return p;
}

Node *getNode(Node *currentNode, char(*goal)[N], char action) {
	Node *childNode = new Node;	
	childNode->action = action;
	childNode->parent = currentNode;
	childNode->state = doAction(currentNode, action);
	childNode->h = geth(childNode->state,goal);
	childNode->pathcost = currentNode->pathcost + 1;
	getPosition(childNode);
	getActionBool(childNode);
	return childNode;
}

void getPosition(Node *p, char n) {
	p->position = new char[2];
	for (int i = 0; i < N; i++) {
		for (int j = 0; j < N; j++) {
			if (*(*(p->state + i) + j) == n) {
				*(p->position) = i + '0';
				*(p->position + 1) = j + '0';
				break;
			}
		}
	}
}

char (*doAction(Node *p,char action))[N] {
	int i, j;
	i = p->position[0] - '0';
	j = p->position[1] - '0';
	char (*nextstate)[N] = new char[N][N];
	for (int i = 0; i < N; i++) {
		for (int j = 0; j < N; j++) {
			*(*(nextstate + i) + j) = *(*(p->state + i) + j);
		}
	}

	switch (action) {
		case 'D': {//����
			*(*(nextstate + i) + j) = *(*(nextstate + i + 1) + j);
			*(*(nextstate + i + 1) + j) = '0';
			break;
		}
		case 'U': {//����
			*(*(nextstate + i) + j) = *(*(nextstate + i - 1) + j);
			*(*(nextstate + i - 1) + j) = '0';
			break;
		}
		case 'R': {//����
			if (i == 2 && (j == 0 || j == 2)) {
				*(*(nextstate + i) + j) = *(*(nextstate + i) + j + 2);
				*(*(nextstate + i) + j + 2) = '0';
			}
			else {
				*(*(nextstate + i) + j) = *(*(nextstate + i) + j + 1);
				*(*(nextstate + i) + j + 1) = '0';
			}
			break;
		}
		case 'L': {//����
			if (i == 2 && (j == 2 || j == 4)) {
				*(*(nextstate + i) + j) = *(*(nextstate + i) + j - 2);
				*(*(nextstate + i) + j - 2) = '0';
			}
			else {
				*(*(nextstate + i) + j) = *(*(nextstate + i) + j - 1);
				*(*(nextstate + i) + j - 1) = '0';
			}
			break;
		}
	}
	return nextstate;
}


short geth(char (*state)[N], char (*goal)[N]) {
	short h1 = 0;
	for (int i = 0; i < N; i++) {
		for (int j = 0; j < N; j++){
			if (*(*(state + i) + j) != *(*(goal + i) + j)) {
				h1++;
			}
		}
	}
	return h1;
}

void getActionBool(Node *p) {
	int i, j; 
	i = p->position[0] - '0';
	j = p->position[1] - '0';
	p->move = new boolean[4];
	if (i == 0 && j == 0) {
		p->move[0] = false;			//��
		p->move[1] = true;			//��
		p->move[2] = false;			//��
		p->move[3] = true;			//��
	}
	else if (i == 4 && j == 0) {
		p->move[0] = true;
		p->move[1] = false;			
		p->move[2] = false;			
		p->move[3] = true;
	}
	else if (i == 0 && j == 4) {
		p->move[0] = false;
		p->move[1] = true;
		p->move[2] = true;
		p->move[3] = false;
	}
	else if (i == 4 && j == 4) {
		p->move[0] = true;
		p->move[1] = false;
		p->move[2] = true;
		p->move[3] = false;
	}
	else if (i == 0 || i == 3 && (j == 1 || j == 3)) { //���ǰ�͸���ϰ�λ
		p->move[0] = false;
		p->move[1] = true;
		p->move[2] = true;
		p->move[3] = true;
	}
	else if (i == 4 || i == 1 && (j == 1 || j == 3)) { //���ǰ�͸���ϰ�λ
		p->move[0] = true;
		p->move[1] = false;
		p->move[2] = true;
		p->move[3] = true;
	}
	else if (j == 0) {
		p->move[0] = true;
		p->move[1] = true;
		p->move[2] = false;
		p->move[3] = true;
	}
	else if (j == 4) {
		p->move[0] = true;
		p->move[1] = true;
		p->move[2] = true;
		p->move[3] = false;
	}
	else {
		p->move[0] = true;
		p->move[1] = true;
		p->move[2] = true;
		p->move[3] = true;
	}

	if (p->action == 'U') {
		p->move[1] = false;
	}
	else if (p->action == 'D') {
		p->move[0] = false;
	}
	else if (p->action == 'L') {
		p->move[3] = false;
	}
	else if (p->action == 'R') {
		p->move[2] = false;
	}
	else {
		//do nothing
	}
}

void printAction(Node *p) {
	if (p->parent != NULL) {
		printAction(p->parent);
	//	cout << p->action;
		printf("%c", p->action);
		movecount++;
	}
}