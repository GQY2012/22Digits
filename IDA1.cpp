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
	char(*state)[N]; //当前结点状态
	struct searchTree *parent;	 //指向父节点
	char action;	 //生成该节点的动作
	short pathcost;	 //到达此节点已花费的代价g(n)
	int h;			 //当前状态不在位的棋子数h(n)
	boolean *move;	 //能移动的方向
	char *position;	 //0所在的位置
}Node;

struct cmp {
	boolean operator() (Node const *n1, Node const *n2) { //重载优先队列的比较符号
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
int movecount = 0; //移动总次数
int limit, next_limit = INT_MAX;
Node *r;

int(*filesInput(const char*))[N];	 //读文件
Node *getNode(Node*, char(*)[N], char);//返回子节点
void getPosition(Node*, char n = '0');  //返回数字n所在位置
int *getPosition(char(*)[N], char n);		//返回数字n所在位置
char(*doAction(Node*, char))[N];	 //移动
int geth(char(*)[N], char(*)[N]);      //返回当前状态不在位的棋子数
void getActionBool(Node*);			 //返回当前状态可进行的动作
void printAction(Node*);			 //打印移动路径
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

	/*初始结点*/
	Node root;
	Node *n = &root;
	n->state = start;
	n->action = NULL;
	n->parent = NULL;
	n->pathcost = 0;
	getPosition(n);
	getActionBool(n);
	n->h = geth(n->state, goal);
	
	QueryPerformanceFrequency(&nFreq);//获取cpu频率
	QueryPerformanceCounter(&t1);//算法开始时间
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
	QueryPerformanceCounter(&t2);//算法结束时间
	freopen("output_IDAh1.txt", "w", stdout);
	//重定向输出流到指定文件
	printf("%f毫秒\n", (t2.QuadPart - t1.QuadPart) / (double)nFreq.QuadPart * 1000);
	printAction(r);
	printf("\n%d\n", movecount);
	freopen("CON", "w", stdout);
	//重定向输出流到windows控制台

	//	cout << (double)(t2 - t1) / CLOCKS_PER_SEC << endl;
	cout << (t2.QuadPart - t1.QuadPart) / (double)nFreq.QuadPart * 1000 << "毫秒" << endl;
	movecount = 0;
	printAction(r);

	cout << endl << "移动次数:" << movecount << endl;
	system("pause");
	return 0;
}

int(*filesInput(const char* filePath))[N] {
	freopen(filePath, "r", stdin);//重定向输入流到指定文件
	int(*p)[N] = new int[N][N];
	for (int i = 0;i < N;i++) {
		for (int j = 0;j < N;j++) {
			scanf("%d", &p[i][j]);
		}
	}
	freopen("CON", "r", stdin);//重定向输入流到windows控制台
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
	case 'd': {//下移
		*(*(nextstate + i) + j) = *(*(nextstate + i + 1) + j);
		*(*(nextstate + i + 1) + j) = '0';
		break;
	}
	case 'u': {//上移
		*(*(nextstate + i) + j) = *(*(nextstate + i - 1) + j);
		*(*(nextstate + i - 1) + j) = '0';
		break;
	}
	case 'r': {//右移
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
	case 'l': {//左移
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
		p->move[0] = false;			//上
		p->move[1] = true;			//下
		p->move[2] = false;			//左
		p->move[3] = true;			//右
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
	else if (i == 0 || i == 3 && (j == 1 || j == 3)) { //考虑半透明障碍位
		p->move[0] = false;
		p->move[1] = true;
		p->move[2] = true;
		p->move[3] = true;
	}
	else if (i == 4 || i == 1 && (j == 1 || j == 3)) { //考虑半透明障碍位
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