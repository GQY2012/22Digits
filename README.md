# 22 Digits
本问题包括一个5×5的表格，22个写有数字(1-22)的棋子以及一个空位（由0表示），两个半透明障碍位 （可以用-1表示）。 <br>
与空位上、下、左、右相邻的棋子可以移动到空位中。 <br>
• 半透明障碍位为：上下不可穿透，左右可穿透。 <br>
游戏的目的是要达到一个特定的目标状态。 <br>
启发函数：<br>
  h1(n) = number of misplaced tiles（错位的棋子数）   <br>
  h2(n) = total Euclidean distance（所有棋子到其目标位置的欧式距离和） <br>
  
实验要求：
1. 实现A*搜索
2. 实现迭代深入A*搜索
<br>