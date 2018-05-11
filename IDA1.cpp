#pragma warning(disable:4996)
#define _SILENCE_STDEXT_HASH_DEPRECATION_WARNINGS

#include <iostream>
#include <cstdio>
#include <ctime>
#include <cstring>
#include <list>
#include <Windows.h>

#define N 5
using namespace std;

typedef struct searchTree {
	char(*state)[N]; //��ǰ���״̬
	struct searchTree *parent;	 //ָ�򸸽ڵ�
	char action;	 //���ɸýڵ�Ķ���
	short pathcost;	 //����˽ڵ��ѻ��ѵĴ���g(n)
	int h;			 //��ǰ״̬����λ��������h(n)
	boolean *move;	 //���ƶ��ķ���
	char *position;	 //0���ڵ�λ��
}Node;

struct cmp {
	boolean operator() (Node const *n1, Node const *n2) { //�������ȶ��еıȽϷ���
		if ((n1->pathcost + n1->h) == (n2->pathcost + n2->h))
			return n1->pathcost < n2->pathcost;
		return (n1->pathcost + n1->h) >(n2->pathcost + n2->h);
	}
};

//clock_t t1, t2;
LARGE_INTEGER nFreq;//cpu frequency  
LARGE_INTEGER t1;//begin
LARGE_INTEGER t2;//end
char(*start)[N] = new char[N][N];
char(*goal)[N] = new char[N][N];
list<Node*> nodeList;
list<Node*> ::iterator iter;
char ac[] = { 'u','d','l','r' };
int movecount = 0; //�ƶ��ܴ���
int limit, next_limit = INT_MAX;
Node *r;

int(*filesInput(const char*))[N];	 //���ļ�
Node *getNode(Node*, char(*)[N], char);//�����ӽڵ�
void getPosition(Node*, char n = '0');  //��������n����λ��
int *getPosition(char(*)[N], char n);		//��������n����λ��
char(*doAction(Node*, char))[N];	 //�ƶ�
int geth(char(*)[N], char(*)[N]);      //���ص�ǰ״̬����λ��������
void getActionBool(Node*);			 //���ص�ǰ״̬�ɽ��еĶ���
void printAction(Node*);			 //��ӡ�ƶ�·��
boolean dfs(Node*);

int main() {
	const char *pathInput = "input.txt";
	const char *pathTarget = "target.txt";

	int(*startint)[N] = filesInput(pathInput);
	int(*goalint)[N] = filesInput(pathTarget);
	for (int i = 0;i < N;i++) {
		for (int j = 0;j < N;j++) {
			start[i][j] = startint[i][j] + '0';
			goal[i][j] = goalint[i][j] + '0';
		}
	}
	delete[] startint, goalint;

	/*��ʼ���*/
	Node root;
	Node *n = &root;
	n->state = start;
	n->action = NULL;
	n->parent = NULL;
	n->pathcost = 0;
	getPosition(n);
	getActionBool(n);
	n->h = geth(n->state, goal);
	
	QueryPerformanceFrequency(&nFreq);//��ȡcpuƵ��
	QueryPerformanceCounter(&t1);//�㷨��ʼʱ��
	//	t1 = clock();
	
	limit = n->h;
	while (!dfs(n)) {
		limit = next_limit;
		next_limit = INT_MAX;
		for (iter = nodeList.begin(); iter != nodeList.end(); ++iter) {
			delete *iter;
		}
		nodeList.clear();
	}

	//	t2 = clock();
	QueryPerformanceCounter(&t2);//�㷨����ʱ��
	freopen("output_IDAh1.txt", "w", stdout);
	//�ض����������ָ���ļ�
	printf("%f����\n", (t2.QuadPart - t1.QuadPart) / (double)nFreq.QuadPart * 1000);
	printAction(r);
	printf("\n%d\n", movecount);
	freopen("CON", "w", stdout);
	//�ض����������windows����̨

	//	cout << (double)(t2 - t1) / CLOCKS_PER_SEC << endl;
	cout << (t2.QuadPart - t1.QuadPart) / (double)nFreq.QuadPart * 1000 << "����" << endl;
	movecount = 0;
	printAction(r);

	cout << endl << "�ƶ�����:" << movecount << endl;
	system("pause");
	return 0;
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
	getPosition(childNode);
	getActionBool(childNode);
	childNode->h = geth(childNode->state, goal);
	childNode->pathcost = currentNode->pathcost + 1;
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

int *getPosition(char(*state)[N], char n) {
	int *position = new int[2];
	for (int i = 0; i < N; i++) {
		for (int j = 0; j < N; j++) {
			if (*(*(state + i) + j) == n) {
				*position = i;
				*(position + 1) = j;
				break;
			}
		}
	}
	return position;
}

char(*doAction(Node *p, char action))[N] {
	int i, j;
	i = p->position[0] - '0';
	j = p->position[1] - '0';
	char(*nextstate)[N] = new char[N][N];
	for (int i = 0; i < N; i++) {
		for (int j = 0; j < N; j++) {
			*(*(nextstate + i) + j) = *(*(p->state + i) + j);
		}
	}

	switch (action) {
	case 'd': {//����
		*(*(nextstate + i) + j) = *(*(nextstate + i + 1) + j);
		*(*(nextstate + i + 1) + j) = '0';
		break;
	}
	case 'u': {//����
		*(*(nextstate + i) + j) = *(*(nextstate + i - 1) + j);
		*(*(nextstate + i - 1) + j) = '0';
		break;
	}
	case 'r': {//����
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
	case 'l': {//����
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


int geth(char(*state)[N], char(*goal)[N]) {
	short h1 = 0;
	for (int i = 0; i < N; i++) {
		for (int j = 0; j < N; j++) {
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

	if (p->action == 'u') {
		p->move[1] = false;
	}
	else if (p->action == 'd') {
		p->move[0] = false;
	}
	else if (p->action == 'l') {
		p->move[3] = false;
	}
	else if (p->action == 'r') {
		p->move[2] = false;
	}
	else {
		//do nothing
	}
}

boolean dfs(Node *n) {
	if (n->h == 0)
		return true;
	Node *t;
	for (int i = 0;i < 4;i++) {
		if (n->move[0] == false && ac[i] == 'u'
			|| n->move[1] == false && ac[i] == 'd'
			|| n->move[2] == false && ac[i] == 'l'
			|| n->move[3] == false && ac[i] == 'r')
			continue;
		t = getNode(n, goal, ac[i]);
		nodeList.push_back(t);
		if (t->h == 0) {
			r = t;
			return true;
		}
		if (t != n) {
			//			cout << t->pathcost << endl;
			if (t->h + t->pathcost > limit) {
				next_limit = (next_limit < t->h + t->pathcost) ? next_limit : t->h + t->pathcost;
				//				cout << "break" << endl;
			}
			else {
				if (dfs(t))
					return true;
			}
		}
	}
	return false;
}

void printAction(Node *p) {
	if (p->parent != NULL) {
		printAction(p->parent);
		//	cout << p->action;
		printf("%c", p->action);
		movecount++;
	}
}