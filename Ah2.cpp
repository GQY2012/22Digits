#pragma warning(disable:4996)

#include <iostream>
#include <cstdio>
#include <ctime>
#include <cstring>
#include <queue>
#include <Windows.h>

#define N 5
using namespace std;

typedef struct searchTree {
	int(*state)[N]; //��ǰ���״̬
	struct searchTree *parent;	 //ָ�򸸽ڵ�
	char action;	 //���ɸýڵ�Ķ���
	int pathcost;	 //����˽ڵ��ѻ��ѵĴ���g(n)
	int h;			 //��ǰ�����پ����һ������ȡ��h(n)
}Node;

typedef struct actionBool {
	boolean u;
	boolean d;
	boolean l;
	boolean r;
}actionBool;

boolean operator<(Node n1, Node n2) { //�������ȶ��еıȽϷ���
	if ((n1.pathcost + n1.h) == (n2.pathcost + n2.h))
		return n1.pathcost < n2.pathcost;
	return (n1.pathcost + n1.h) >(n2.pathcost + n2.h);
}

//clock_t t1, t2;
LARGE_INTEGER nFreq;//cpu frequency  
LARGE_INTEGER t1;//begin
LARGE_INTEGER t2;//end
int movecount = 0; //�ƶ��ܴ���

int(*filesInput(const char*))[N];	 //���ļ�
Node *getNode(Node*, int(*)[N], char);//�����ӽڵ�
int *getPosition(int(*)[N],int n = 0);		 //���ؿ�λ
int(*doAction(int(*)[N], char))[N]; //�ƶ�
int geth(int(*)[N], int(*)[N]);      //���������پ����һ������ȡ��
actionBool getActionBool(int(*)[N]); //���ص�ǰ״̬�ɽ��еĶ���
void printAction(Node*);			 //��ӡ�ƶ�·��


int main() {
	const char *pathInput = "C:/Users/95850/Desktop/Mine/��ҵ/�˹����ܻ���/PB15111650_������_ʵ��һ/22����/input.txt";
	const char *pathTarget = "C:/Users/95850/Desktop/Mine/��ҵ/�˹����ܻ���/PB15111650_������_ʵ��һ/22����/target.txt";

	int(*start)[N] = filesInput(pathInput);
	int(*goal)[N] = filesInput(pathTarget);

	priority_queue<Node> q;//���ȶ���
	/*��ʼ���*/
	Node root;
	root.state = start;
	root.action = NULL;
	root.parent = NULL;
	root.pathcost = 0;
	root.h = geth(start, goal);

	Node *t = NULL;
	q.push(root);

	QueryPerformanceFrequency(&nFreq);//��ȡcpuƵ��
	QueryPerformanceCounter(&t1);//�㷨��ʼʱ��
								 //	t1 = clock();
	while (!q.empty()) {
		Node *n = new Node;
		*n = q.top();
		q.pop();
		char ac[] = { 'u','d','l','r' };
		for (int i = 0;i < 4;i++) {
			if (ac[i] == 'u' && n->action == 'd' || ac[i] == 'd' && n->action == 'u'
				|| ac[i] == 'l' && n->action == 'r' || ac[i] == 'r' && n->action == 'l')
				continue;
			t = getNode(n, goal, ac[i]);
			if (t->state != NULL) {
				q.push(*t);
			}
			if (t->h == 0)
				goto end;
		}
	//	delete n;
	}
end:
	//	t2 = clock();
	QueryPerformanceCounter(&t2);//�㷨����ʱ��
	freopen("C:/Users/95850/Desktop/Mine/��ҵ/�˹����ܻ���/PB15111650_������_ʵ��һ/22����/output_Ah2.txt", "w", stdout);
	//�ض����������ָ���ļ�
	printf("%f����\n", (t2.QuadPart - t1.QuadPart) / (double)nFreq.QuadPart * 1000);
	printAction(t);
	printf("\n%d\n", movecount);
	freopen("CON", "w", stdout);
	//�ض����������windows����̨

	//	cout << (double)(t2 - t1) / CLOCKS_PER_SEC << endl;
	cout << (t2.QuadPart - t1.QuadPart) / (double)nFreq.QuadPart * 1000 << "����" << endl;
	movecount = 0;
	printAction(t);
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

Node *getNode(Node *currentNode, int(*goal)[N], char action) {
	Node *childNode = new Node;
	actionBool aB = getActionBool(currentNode->state);
	if (aB.d == false && action == 'd'
		|| aB.u == false && action == 'u'
		|| aB.r == false && action == 'r'
		|| aB.l == false && action == 'l') {
		childNode->state = NULL;
		return childNode;
	}

	childNode->action = action;
	childNode->parent = currentNode;
	childNode->state = doAction(currentNode->state, action);
	childNode->h = geth(childNode->state, goal);
	childNode->pathcost = currentNode->pathcost + 1;

	return childNode;
}

int *getPosition(int(*state)[N],int n) {
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

int(*doAction(int(*state)[N], char action))[N] {
	int i, j;
	int *position = getPosition(state);
	i = position[0];
	j = position[1];

	int(*nextstate)[N] = new int[N][N];
	for (int i = 0; i < N; i++) {
		for (int j = 0; j < N; j++) {
			*(*(nextstate + i) + j) = *(*(state + i) + j);
		}
	}

	switch (action) {
	case 'd': {//����
		*(*(nextstate + i) + j) = *(*(nextstate + i + 1) + j);
		*(*(nextstate + i + 1) + j) = 0;
		break;
	}
	case 'u': {//����
		*(*(nextstate + i) + j) = *(*(nextstate + i - 1) + j);
		*(*(nextstate + i - 1) + j) = 0;
		break;
	}
	case 'r': {//����
		if (i == 2 && (j == 0 || j == 2)) {
			*(*(nextstate + i) + j) = *(*(nextstate + i) + j + 2);
			*(*(nextstate + i) + j + 2) = 0;
		}
		else {
			*(*(nextstate + i) + j) = *(*(nextstate + i) + j + 1);
			*(*(nextstate + i) + j + 1) = 0;
		}
		break;
	}
	case 'l': {//����
		if (i == 2 && (j == 2 || j == 4)) {
			*(*(nextstate + i) + j) = *(*(nextstate + i) + j - 2);
			*(*(nextstate + i) + j - 2) = 0;
		}
		else {
			*(*(nextstate + i) + j) = *(*(nextstate + i) + j - 1);
			*(*(nextstate + i) + j - 1) = 0;
		}
		break;
	}
	}
	return nextstate;
}

int geth(int(*state)[N], int(*goal)[N]) {
	int h2 = 0;
	for (int i = 0; i < N; i++) {
		for (int j = 0; j < N; j++) {
			if (state[i][j] <= 0)
				continue;
			int *p = getPosition(goal, state[i][j]);
			h2 = h2 + abs(i - p[0]) + abs(j - p[1]);
		}
	}
	return ceil((double) h2 / 2);
}

actionBool getActionBool(int(*state)[N]) {
	int i, j;
	int *position = getPosition(state);
	actionBool action;
	i = position[0];
	j = position[1];
	if (i == 0 && j == 0) {
		action.d = true;
		action.u = false;
		action.r = true;
		action.l = false;
	}
	else if (i == 4 && j == 0) {
		action.d = false;
		action.u = true;
		action.r = true;
		action.l = false;
	}
	else if (i == 0 && j == 4) {
		action.d = true;
		action.u = false;
		action.r = false;
		action.l = true;
	}
	else if (i == 4 && j == 4) {
		action.d = false;
		action.u = true;
		action.r = false;
		action.l = true;
	}
	else if (i == 0 || i == 3 && (j == 1 || j == 3)) { //���ǰ�͸���ϰ�λ
		action.d = true;
		action.u = false;
		action.r = true;
		action.l = true;
	}
	else if (i == 4 || i == 1 && (j == 1 || j == 3)) { //���ǰ�͸���ϰ�λ
		action.d = false;
		action.u = true;
		action.r = true;
		action.l = true;
	}
	else if (j == 0) {
		action.d = true;
		action.u = true;
		action.r = true;
		action.l = false;
	}
	else if (j == 4) {
		action.d = true;
		action.u = true;
		action.r = false;
		action.l = true;
	}
	else {
		action.d = true;
		action.u = true;
		action.r = true;
		action.l = true;
	}
	return action;
}

void printAction(Node *p) {
	if (p->parent != NULL) {
		printAction(p->parent);
		//	cout << p->action;
		printf("%c", p->action);
		movecount++;
	}

}