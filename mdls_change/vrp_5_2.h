#include<vector>
using namespace std;

#ifndef VRPTW_H
#define VRPTW_H
#define maxpop   500  /*Max population */

const int MAX = 300;
const int FUNC_NUM = 5;
const int NEIGH_NUM = 3;
const int INF = 9999999;


//�ͻ���
static struct Customer {
	int id;					//���
	double x;				//x�������
	double y;				//y������
	double s_time;			//����ʱ��	
	double b_time;			//�������ʱ��	
	double e_time;			//�������ʱ��
	double demand;			//������
}customer[MAX];				//�ͻ�������

double a_time[MAX];                         //arrival time arrays
double l_time[MAX];                         //leave time arrays
double w_time[MAX];                         //wait time	arrays
double d_time[MAX];                          //delay time	arrays
double total_wait[MAX];                     //the total wait time before i
double total_delay[MAX];                     //the total delay time before i

double max_wait[MAX];						//the max wait time���Ϸ��ӳ�ʱ��

double b_demand[MAX];                      // the total demand before customer i including i in the route
double a_demand[MAX];                      // the total demand after customer i including i	in the route

										   //��i��֮ǰ֮���·��������ʲô��
double b_distance[MAX];                    //the total distance before customer i including i
double a_distance[MAX];                    //the total distance after customer i including i


struct Route {							//�ṹ�壺·��
	vector<int> customers;				//·���к��еĿͻ�����
	double travel_dist;					//����·������ʻ����
	double travel_time;					//����·������ʻʱ��
	double wait_time;					//����·���ĵȴ�ʱ��
	double delay_time;					//����·�����ӳ�ʱ��
	double capacity;					//����
};

int cust_num;							//�ͻ�����Ŀ
double max_capacity = 200;				//�������
double max_delay_time = 1800;				//����ӳ�ʱ��1800


double peer_time[MAX][MAX];				//�����ͻ���֮�����ʻʱ��
double peer_distance[MAX][MAX];			//�����ͻ���֮�����ʻ����

int archive_size = 500;					//�浵��С.
										/*double epsilon[FUNC_NUM];  */            //epsilon ���漫ֵ
double epsilon = 0.05;					//����<-ռ�Ź�ϵ�Ĵ浵������


const int N = 5;							//Ŀ����Ŀ



struct Chromosome {						//��
	vector<Route> routes;				//·������
	double f[5];						//5��Ŀ��ֵ,f[0]:������Ŀ,f[1]:����ʻ����;f[2]:�·����ʻʱ��;f[3]:�ܵȴ�ʱ��;f[4]:���ӳ�ʱ��
	int box_f[5];						//��ռ�Ž�Ĺ�������������<-��ռ��ϵ�λ��
										//int s_f[FUNC_NUM];					//�ɹ���ʧ�ܵĴ���
	int local[FUNC_NUM];					//�����ľֲ�����
	int count;
	double cub_len;
	int ranking;

	bool operator < (const Chromosome &chrome) {    //��Ҫ����ǰ����Ŀ����ȷ����С��
		return f[0]<chrome.f[0] || (f[0] == chrome.f[0] && f[1]<chrome.f[1]) || (f[0] == chrome.f[0] && f[1] == chrome.f[1] && f[2]<chrome.f[2]);
	}

	bool operator == (const Chromosome &chrome) {
		bool used[MAX][MAX];
		for (int i = 0; i <= cust_num; ++i)
			for (int j = 0; j <= cust_num; ++j)
				used[i][j] = false;

		for (vector<Route>::iterator r_iter = routes.begin(); r_iter != routes.end(); ++r_iter)
			for (vector<int>::iterator c_iter = r_iter->customers.begin(); c_iter != r_iter->customers.end() - 1; ++c_iter)
				used[*c_iter][*(c_iter + 1)] = !used[*c_iter][*(c_iter + 1)];					//������·���������ӵĿͻ����־Ϊtrue

		for (vector<Route>::const_iterator r_iter = chrome.routes.begin(); r_iter != chrome.routes.end(); ++r_iter)
			for (vector<int>::const_iterator c_iter = r_iter->customers.begin(); c_iter != r_iter->customers.end() - 1; ++c_iter)
				used[*c_iter][*(c_iter + 1)] = !used[*c_iter][*(c_iter + 1)];

		for (int i = 0; i <= cust_num; ++i)
			for (int j = 0; j <= cust_num; ++j)
				if (used[i][j]) return false;
		return true;
	}
	double similarity;									//����; ������; �����; �����ԣ�������;
	double crowding;									//ӵ������; ���߼���;
};


struct SUCCESS {							//�ṹ�壺·��
	vector<Chromosome> customers;				//·���к��еĿͻ�����
												//double *sf[1000][FUNC_NUM];					//�ɹ��Ĵ�������
	int sf[500][FUNC_NUM];
};

vector<Chromosome> total_best;							//��õĽ⼯��
Chromosome extreme[FUNC_NUM];							//�߽�⼯�ϣ�����5��Ŀ�������ŵĽ⣻�磺extreme[0]:f[0]�����ţ�extreme[1]:f[1]�����ţ�extreme[2]:f[2]�����ţ�extreme[3]:f[3]�����ţ�extreme[4]:f[4]������
SUCCESS SIA;										//�ɹ���Ϣ�浵

vector<Chromosome>chromosome;						   //�⼯
vector<Chromosome> EP;								  //
													  //vector<Chromosome> BETWEEN;								//�м��
vector<double> IS;
double FP[maxpop][FUNC_NUM];
double MMV[FUNC_NUM][2];									//SFC[FUNC_NUM][0]:Ŀ�����Сֵ ;SFC[FUNC_NUM][1]:Ŀ������ֵ
double LSArray[FUNC_NUM][NEIGH_NUM];
double FIR[FUNC_NUM][NEIGH_NUM], q[FUNC_NUM][NEIGH_NUM];
int lsn[NEIGH_NUM];

bool LSTable[NEIGH_NUM];
vector<Chromosome> parent;								//��һ����
vector<Chromosome> children;								//�Ӵ���
vector< vector<int> > F;								//��intֵ���ϵļ���
vector< vector<int>> S;									//��intֵ���ϵļ���

vector<string> files;

double times[45];
int ITER = 3000;											//����Ŀ��3000
int ind2 = 0;											//��¼ѡ�������
int CP[3] = { 50,150,250 };
bool EP_flag, is_EP = false;											//EP��־

float fpara1[2 * maxpop][2];

#endif