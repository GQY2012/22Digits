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
	int(*state)[N]; //当前结点状态
	struct searchTree *parent;	 //指向父节点
	char action;	 //生成该节点的动作
	int pathcost;	 //到达此节点已花费的代价g(n)
	int h;			 //当前状态不在位的棋子数h(n)
}Node;

typedef struct actionBool {
	boolean u;
	boolean d;
	boolean l;
	boolean r;
}actionBool;

boolean operator<(Node n1, Node n2) { //重载优先队列的比较符号
	if ((n1.pathcost + n1.h) == (n2.pathcost + n2.h))
		return n1.pathcost < n2.pathcost;
	return (n1.pathcost + n1.h) >(n2.pathcost + n2.h);
}

//clock_t t1, t2;
LARGE_INTEGER nFreq;//cpu frequency  
LARGE_INTEGER t1;//begin
LARGE_INTEGER t2;//end

int(*start)[N], (*goal)[N];
int movecount = 0; //移动总次数
char ac[] = { 'u','d','l','r' };
int limit, next_limit = INT_MAX;
Node *r;

int(*filesInput(const char*))[N];	 //读文件
Node *getNode(Node*, int(*)[N], char);//返回子节点
int *getPosition(int(*)[N]);		 //返回空位
int(*doAction(int(*)[N], char))[N]; //移动
int geth(int(*)[N], int(*)[N]);      //返回当前状态不在位的棋子数
actionBool getActionBool(int(*)[N]); //返回当前状态可进行的动作
void printAction(Node*);			 //打印移动路径
boolean dfs(Node*);


int main() {
	const char *pathInput = "C:/Users/95850/Desktop/Mine/作业/人工智能基础/PB15111650_郭秋洋_实验一/22数码/input.txt";
	const char *pathTarget = "C:/Users/95850/Desktop/Mine/作业/人工智能基础/PB15111650_郭秋洋_实验一/22数码/target.txt";

	start = filesInput(pathInput);
	goal = filesInput(pathTarget);

	priority_queue<Node> q;//优先队列
	/*初始结点*/
	Node root;
	root.state = start;
	root.action = NULL;
	root.parent = NULL;
	root.pathcost = 0;
	root.h = geth(start, goal);

	QueryPerformanceFrequency(&nFreq);//获取cpu频率
	QueryPerformanceCounter(&t1);//算法开始时间

	Node *n = &root;
	limit = root.h;
	while (!dfs(n)) {
		limit = next_limit;
		next_limit = INT_MAX;
	}
	//	t2 = clock();
	QueryPerformanceCounter(&t2);//算法结束时间
	freopen("C:/Users/95850/Desktop/Mine/作业/人工智能基础/PB15111650_郭秋洋_实验一/22数码/output_IDAh1.txt", "w", stdout);
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

Node *getNode(Node *currentNode, int(*goal)[N], char action) {
	Node *childNode = new Node;
	actionBool aB = getActionBool(currentNode->state);
	if (aB.d == false && action == 'd'
		|| aB.u == false && action == 'u'
		|| aB.r == false && action == 'r'
		|| aB.l == false && action == 'l') {
		return currentNode;
	}

	childNode->action = action;
	childNode->parent = currentNode;
	childNode->state = doAction(currentNode->state, action);
	childNode->h = geth(childNode->state, goal);
	childNode->pathcost = currentNode->pathcost + 1;

	return childNode;
}

int *getPosition(int(*state)[N]) {
	int *position = new int[2];
	for (int i = 0; i < N; i++) {
		for (int j = 0; j < N; j++) {
			if (*(*(state + i) + j) == 0) {
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
	case 'd': {//下移
		*(*(nextstate + i) + j) = *(*(nextstate + i + 1) + j);
		*(*(nextstate + i + 1) + j) = 0;
		break;
	}
	case 'u': {//上移
		*(*(nextstate + i) + j) = *(*(nextstate + i - 1) + j);
		*(*(nextstate + i - 1) + j) = 0;
		break;
	}
	case 'r': {//右移
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
	case 'l': {//左移
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
	int h1 = 0;
	for (int i = 0; i < N; i++) {
		for (int j = 0; j < N; j++) {
			if (*(*(state + i) + j) != *(*(goal + i) + j)) {
				h1++;
			}
		}
	}
	return h1;
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
	else if (i == 0 || i == 3 && (j == 1 || j == 3)) { //考虑半透明障碍位
		action.d = true;
		action.u = false;
		action.r = true;
		action.l = true;
	}
	else if (i == 4 || i == 1 && (j == 1 || j == 3)) { //考虑半透明障碍位
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

boolean dfs(Node *n) {
	if (n->h == 0)
		return true;
	Node *t;
	for (int i = 0;i < 4;i++) {
		if (ac[i] == 'u' && n->action == 'd' || ac[i] == 'd' && n->action == 'u'
			|| ac[i] == 'l' && n->action == 'r' || ac[i] == 'r' && n->action == 'l')
			continue;
		t = getNode(n, goal, ac[i]);
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