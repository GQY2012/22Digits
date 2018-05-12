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
	char (*state)[N]; //当前结点状态
	struct searchTree *parent;	 //指向父节点
	char action;	 //生成该节点的动作
	short pathcost;	 //到达此节点已花费的代价g(n)
	short h;			 //当前状态不在位的棋子数h(n)
	boolean *move;	 //能移动的方向
	char *position;	 //0所在的位置
}Node;

struct cmp {
	boolean operator() (Node const *n1, Node const *n2) { //重载优先队列的比较符号
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
int movecount = 0; //移动总次数
int tries;			//扩展次数
int repeat;			//查重次数

int hashValue(char(*)[N]);			 //hash函数
int(*filesInput(const char*))[N];	 //读文件
Node *getNode(Node*, char(*)[N], char);//返回子节点
void getPosition(Node*, char n = '0');  //返回数字n所在位置
char (*doAction(Node*, char))[N];	 //移动
short geth(char(*)[N], char(*)[N]);      //返回当前状态不在位的棋子数
void getActionBool(Node*);			 //返回当前状态可进行的动作
void printAction(Node*);			 //打印移动路径


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

	priority_queue<Node*,vector<Node*>,cmp> q;//优先队列
	hash_map<int, boolean> hashmap;
	/*初始结点*/
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

	QueryPerformanceFrequency(&nFreq);//获取cpu频率
	QueryPerformanceCounter(&t1);//算法开始时间
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
	QueryPerformanceCounter(&t2);//算法结束时间
	freopen("output_Ah1.txt", "w", stdout);
	//重定向输出流到指定文件
	printf("%fs\n", (t2.QuadPart - t1.QuadPart) / (double)nFreq.QuadPart);	
	printAction(t);
	printf("\n%d\n", movecount);
	freopen("CON", "w", stdout);
	//重定向输出流到windows控制台

//	cout << (double)(t2 - t1) / CLOCKS_PER_SEC << endl;
	cout << (t2.QuadPart - t1.QuadPart) / (double)nFreq.QuadPart << "s" << endl;
	movecount = 0;
	printAction(t);

	cout << endl << "移动次数:" << movecount << endl;
	cout << "扩展次数:" << tries << endl;
	cout << "查重次数:" << repeat << endl;
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
		case 'D': {//下移
			*(*(nextstate + i) + j) = *(*(nextstate + i + 1) + j);
			*(*(nextstate + i + 1) + j) = '0';
			break;
		}
		case 'U': {//上移
			*(*(nextstate + i) + j) = *(*(nextstate + i - 1) + j);
			*(*(nextstate + i - 1) + j) = '0';
			break;
		}
		case 'R': {//右移
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
		case 'L': {//左移
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