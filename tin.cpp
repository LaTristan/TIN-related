#include <graphics.h>
#include<math.h>
#include<stdio.h>
#include<conio.h>
#include<time.h>
#define WIDTH 640										// 生成窗口的大小
#define HEIGHT 480
#define NUMBER 50										// 随机点的个数
int X[NUMBER];											// 储存 X 方向产生的随机数
int Y[NUMBER];											// 储存 Y 方向产生的随机数
bool LineXY[(1 + NUMBER) * NUMBER / 2] = { false };		// 为了判断两点是否连线定义的一维数组


//判断这两个点是否连线
bool IsLinked(int p1, int p2)
{
	if (p1 >= p2)
		return LineXY[(1 + p1) * p1 / 2 + p2];
	else
		return LineXY[(1 + p2) * p2 / 2 + p1];
}


//储存已经绘制过的线
void Link(int p1, int p2)
{
	if (p1>=p2)
		LineXY[(1 + p1) * p1 / 2 + p2] = true;
	else
		LineXY[(1 + p2) * p2 / 2 + p1] = true;
}


// 绘制随机点
void drawpoint(int x, int y, float z)
{
	float S = 1, L = 0.5;
	setfillcolor(HSLtoRGB(z, S, L));
	solidcircle(x, y, 4);
}


// 用于计算两点的距离
double distance(double x1, double y1, int x2, int y2)
{
	return sqrt((double)((x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2)));
}


// 用于产生随机点
void Randompoint(int width, int height, int *lx, int *ly, int n)
{
	int i;
	bool T;
	while (true)
	{
		i = 0;
		*lx = (rand() % (width - 60) + 30);
		*ly = (rand() % (height - 60) + 30);
		while (true)
		{
			if (i >= n)
			{
				T = true;
				break;
			}
			if (distance(*lx, *ly, X[i], Y[i]) < 20)			// 用于控制随机点之间的距离
			{
				T = false;
				break;
			}
			i++;
		}
		if (T == false)
			continue;
		else
			break;
	}
}


// 根据三个点坐标算出他们三个形成外接圆的圆心和半径
double CircleCenter(int x1, int y1, int x2, int y2, int x3, int y3, double *x, double *y, double *r)
{
	if ((2.0 * (x1 * (y2 - y3) - y1 * (x2 - x3) + x2 * y3 - x3 * y2)) == 0)
	{
		*x = 0;
		*y = 0;
		*r = 0;
		return *r;
	}
	*x = ((x1 * x1 + y1 * y1) * (y2 - y3) + (x2 * x2 + y2 * y2) * (y3 - y1) + (x3 * x3 + y3 * y3) * (y1 - y2)) / (2.0 * (x1 * (y2 - y3) - y1 * (x2 - x3) + x2 * y3 - x3 * y2));
	*y = ((x1 * x1 + y1 * y1) * (x3 - x2) + (x2 * x2 + y2 * y2) * (x1 - x3) + (x3 * x3 + y3 * y3) * (x2 - x1)) / (2.0 * (x1 * (y2 - y3) - y1 * (x2 - x3) + x2 * y3 - x3 * y2));
	*r = sqrt((*x - x1) * (*x - x1) + (*y - y1) * (*y - y1));
	return *r;
}


int main()
{
	initgraph(WIDTH, HEIGHT);
	float G[NUMBER];								// 储存不同的颜色（后期可以用来储存高程
	int Z[NUMBER];									// 储存判断过的点的数组
	double max = WIDTH * HEIGHT * WIDTH;			// 用窗口的长和宽来定义一个相对大的数
	int lx;											// 储存临时 X 方向产生的变量
	int ly;											// 储存临时 Y 方向产生的变量
	int li;											// 用于储存临时判断过的点的下标数

	srand((unsigned)time(NULL));					// 初始化随机数
	for (int i = 0; i < NUMBER; i++)
	{
		Randompoint(WIDTH, HEIGHT, &lx, &ly, i);
		X[i] = lx;									// 产生 X 方向的随机数
		Y[i] = ly;									// 产生 Y 方向的随机数
		G[i] = float(rand() % 240);					// 产生不同的颜色的随机数（后期可以用来表示高程）
		drawpoint(X[i], Y[i], G[i]);				// 绘制随机点
	}

	float H = 60, S = 1, L = 0.5;
	setlinecolor(HSLtoRGB(H, S, L));

	// 在随机生成的点中依次判断，找出距离最短的两个点
	for (int i = 0; i < NUMBER; i++)
	{
		for (int j = i+1; j < NUMBER; j++)
		{
			if (max > distance(X[i], Y[i], X[j], Y[j]))
			{
				lx = i;
				ly = j;
				max = distance(X[i], Y[i], X[j], Y[j]);
			}
		}
	}

	line(X[lx], Y[lx], X[ly], Y[ly]);
	Link(lx, ly);
	Z[0] = lx;
	Z[1] = ly;

	int n = 2;
	while (true)
	{
		if (n >= NUMBER)
			break;

		int m = 0;
		double rad, Xd, Yd, Rd;
		bool OK = false;
		max = WIDTH * HEIGHT * WIDTH;

		// 开始判断随机生成的每一个点
		for (int i = 0; i < NUMBER; i++)
		{
			m = 0;
			OK = false;

			// 判断这个点是否已经判断过，如果已经判断过，返回判断下一个点，如果不在，继续程序
			while (true)
			{
				if (m >= n)
				{
					m = 0;
					break;
				}
				if (i == Z[m])
				{
					OK = true;
					break;
				}
				m++;
			}
			if (OK == true)
				continue;

			// 在已经确定的两个点和未确定的点进行计算它们形成三角形的的半径，并判断形成的圆内有无其它的点
			// 若无其它的点，则可以连线，如有其它的点，则进行判断下一个点
			for (int j = 0; j < n; j++)
			{
				for (int k = 0; k < n; k++)
				{
					rad = CircleCenter(X[Z[j]], Y[Z[j]], X[Z[k]], Y[Z[k]], X[i], Y[i], &Xd, &Yd, &Rd);
					int cc = 0;
					OK = false;
					while (true)
					{
						if (cc >= NUMBER)
							break;

						// 判断圆内有无其它点，并且这个被判断的点不能为形成这个圆的这个三个点，如果有其它点，就跳出该循环
						if (distance(Xd, Yd, X[cc], Y[cc]) <= Rd && cc != Z[k] && cc != Z[j] && cc != i)
						{
							OK = true;
							break;
						}
						cc++;
					}

					// 因为圆内有其它点，结束本次循环
					if (OK == true)
						continue;

					if (max >= rad && rad !=0)					// 在三个点围成圆内没有点找到半径最小的
					{
						lx = Z[j];
						ly = Z[k];

						if (rad >= WIDTH)
							continue;
						else
						{
							if (IsLinked(i, lx) == false)		// 绘制线段，首先判断这个线段是否已经绘制过
							{
								Link(i, lx);
								line(X[i], Y[i], X[lx], Y[lx]);
								/*Sleep(100);*/					// 如果需要查看绘制过程，去掉注释
							}
							if (IsLinked(i, ly) == false)
							{
								Link(i, ly);
								line(X[i], Y[i], X[ly], Y[ly]);
								/*Sleep(100);*/					// 如果需要查看绘制过程，去掉注释
							}
							if (IsLinked(lx, ly) == false)
							{
								Link(lx, ly);
								line(X[lx], Y[lx], X[ly], Y[ly]);
								/*Sleep(100);*/					// 如果需要查看绘制过程，去掉注释
							}
							li = i;
						}
					}
				}
			}
		}
		Z[n] = li;
		n++;
	}

	//重新描点
	for (int i = 0; i < NUMBER; i++)
	{
		drawpoint(X[i], Y[i], G[i]);
	}

	_getch();
	return 0;
}
