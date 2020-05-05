#include<stdio.h>
#include<time.h>
#include<stdlib.h>
#include<math.h>
#include<io.h>
#include<direct.h>    //ͷ�ļ�  

#include "vrp_5_2.h"
#include<vector>
#include<algorithm>
#include<iostream>
#include<fstream>
#include<string>
using namespace std;

/*
�����б�˵����
vector<double> a		:	��һ����һ�����������飬��Ϊ1
����ֵ��
���������±�����
*/
int RouletteWheelSelection(vector<double> a)
{
	double fSlice = rand() % 1000 / (double)1001;

	double cfTotal = 0.0; //����ͳ��

	int SelectedGenomePos = 0; //��ѡ�еĸ����±�
	int i;
	for (i = 0; i < a.size(); ++i)
	{

		cfTotal += a[i];
		if (cfTotal > fSlice)
		{
			SelectedGenomePos = i;
			break;
		}
	}

	return SelectedGenomePos;
}

/*
����:��ʼ���ͻ��㡢�ͻ���֮�����ʻʱ�����ʻ����
distance[]:������ļ�·���ַ�����
time[]:ʱ����ļ�·���ַ�����
spec[]:
�㷨���裺
1.ͨ���ļ���·�������������ļ���ʱ���ļ�����Ⱥ�ļ��������ļ�������
2.��r_spec�ļ����ж�ȡ���ݣ�Ϊ250���ͻ��㸳ֵ
3.�ر�r_spec�ļ���
4.��r_time��r_distance�ļ����ж�ȡ���ݣ�Ϊ�ͻ���i��ͻ���j����ʻʱ�䡢��ʻ���븳ֵ
5.Ϊ�ͻ���251���������������ͻ���[0,250]����ʻʱ�䡢��ʻ���븳ֵ����Ϊ�ͻ���0��ͻ���n+1���ǳ���
6.�ر�ʱ�����;�����
*/
void getData(char distance[], char time[], char spec[]) {

	//���ļ���ȡ�ļ����ݵ��ļ������� in��
	//1.�������ļ������󣬺��ļ�����
	fstream r_distance(distance, ios::in), r_time(time, ios::in), r_spec(spec, ios::in);
	//cust_num=50;

	char line[300];
	for (int i = 0; i < 4; ++i)
	{
		r_spec.getline(line, 300);			//���ļ����е�300���ֽڵ����ݷ���line�С�
		cout << line << endl;
	}

	for (int i = 0; i<2; ++i)
	{
		r_spec >> max_capacity;				//���ļ����е�����int���͸�max_capacity
		cout << max_capacity << endl;
	}

	for (int i = 0; i < 4; ++i)
	{
		r_spec.getline(line, 300);
		cout << line << endl;
	}
	//2.��r_spec�ļ����ж�ȡ����Ϊ250���ͻ��㸳ֵ
	for (int i = 0; i <= cust_num; ++i) {			//Ϊ�ͻ���0-249����ֵ
		r_spec >> customer[i].id;				//�ͻ���i�����
		r_spec >> customer[i].x;				//�ͻ���i��xֵ
		r_spec >> customer[i].y;				//�ͻ���i��yֵ
		r_spec >> customer[i].demand;			//�ͻ���i��������
		r_spec >> customer[i].b_time;			//�ͻ���i���������ʱ��
		r_spec >> customer[i].e_time;			//�ͻ���i����ٷ���ʱ��
		r_spec >> customer[i].s_time;			//�ͻ���i�ķ���ʱ��
		cout << " id:" << customer[i].id << " x:" << customer[i].x << " y:" << customer[i].y << " demand:" << customer[i].demand << " b_time:" << customer[i].b_time << "  e_time:" << customer[i].e_time << "  s_time:" << customer[i].s_time << endl;
	}
	//3.�ر�r_spec�ļ���
	r_spec.close();

	//4.��r_time��r_distance�ļ����ж�ȡ���ݣ�Ϊ�ͻ���i��ͻ���j����ʻʱ�䡢��ʻ���븳ֵ
	for (int i = 0; i <= cust_num; ++i)
		for (int j = 0; j <= cust_num; ++j) {
			r_time >> peer_time[i][j];			//�����ӿͻ���i���ͻ���j����ʻʱ��
			r_distance >> peer_distance[i][j];	//�����ӿͻ���i ���ͻ���j����ʻ����
		}

	//5.Ϊ�ͻ���0-250��ͻ���251����ʻʱ������ʻ���븳ֵ����Ϊ�ͻ���0��ͻ���251����ʾ����
	for (int i = 0; i <= cust_num; ++i) {				//Ϊ�ͻ���251���������� 0����ӿͻ���i���ͻ���cust_num + 1 = 251����ʻʱ�䡢��ʻ����
		peer_time[i][cust_num + 1] = peer_time[i][0];
		peer_time[cust_num + 1][i] = peer_time[0][i];
		peer_distance[i][cust_num + 1] = peer_distance[i][0];
		peer_distance[cust_num + 1][i] = peer_distance[0][i];
	}
	customer[cust_num + 1] = customer[0];			//����Ϊcust_num + 1�� 0�Ķ���ʾ����
													//6.�ر�ʱ�����;�����
	r_time.close();								//ʱ�����ر�
	r_distance.close();							//�������ر�
}


//���·�����������ӳ�ʱ�䡢��ʻ���롢��ʻʱ�䡢�ȴ�ʱ�䣻���·�������еĿͻ���
void clear_route(Route &route) {
	route.capacity = 0;
	route.delay_time = 0;
	route.travel_dist = 0;
	route.travel_time = 0;
	route.wait_time = 0;
	route.customers.clear();
}

/*��ս⣺
1.��ս��е�·����
2.��һ�����е�5��Ŀ���ֵ��Ϊ��0��
*/
void clean_chromosome(Chromosome &chrome) {

	chrome.routes.clear();						//��ս��е�·��
	for (int i = 0; i<FUNC_NUM; ++i)
		chrome.f[i] = 0;							//��һ�����е�5��Ŀ���ֵ��Ϊ��0

}

//������Ŀ�꺯����f[0]:������Ŀ��f[1]:����ʻ���룻f[2]:�·����ʻʱ�䣻f[3]:�ܵȴ�ʱ�䣻f[4]:���ӳ�ʱ��
void compute_f(Chromosome &chrome) {

	for (int i = 0; i<FUNC_NUM; ++i)
		chrome.f[i] = 0;
	chrome.f[0] = chrome.routes.size();
	for (int i = 0; i<chrome.routes.size(); ++i)
		chrome.f[1] += chrome.routes[i].travel_dist;
	int index = 0; double travel_time = 0;
	for (int i = 0; i<chrome.routes.size(); ++i)
		if (chrome.routes[i].travel_time>travel_time)
		{
			travel_time = chrome.routes[i].travel_time;
			index = i;
		}
	chrome.f[2] = chrome.routes[index].travel_time;
	chrome.cub_len = 0;

	//  chrome.f[1]=0;
	//chrome.f[2]=0;
	for (int i = 0; i<chrome.routes.size(); ++i) chrome.f[3] += chrome.routes[i].wait_time;
	for (int i = 0; i<chrome.routes.size(); ++i) chrome.f[4] += chrome.routes[i].delay_time;

}

//����·���ĳ��ȣ�b_distance[*iter]��a_distance[*iter]�ļ��㣬ΪLS6�ṩ����
void compute_route_distance(Route &route) {                                               //compute the traval distance for a route

	route.travel_dist = 0;
	vector<int>::iterator iter;
	for (iter = route.customers.begin(); iter != route.customers.end() - 1; ++iter) {
		route.travel_dist += peer_distance[*iter][*(iter + 1)];
		a_distance[*iter] = 0;
		b_distance[*iter] = 0;
	}
	iter = route.customers.begin();
	b_distance[*iter] = 0;
	a_distance[*iter] = 0;

	//i֮ǰ�ļ���
	for (iter = route.customers.begin() + 1; iter != route.customers.end() - 1; ++iter)
		b_distance[*iter] += (b_distance[*(iter - 1)] + peer_distance[*(iter - 1)][*iter]);
	//i֮��ļ���
	for (iter = route.customers.end() - 2; iter >= route.customers.begin() + 1; --iter)
		a_distance[*iter] += (a_distance[*(iter + 1)] + peer_distance[*iter][*(iter + 1)]);

}

//����·���и����ͻ������ʻʱ�䡢�ȴ�ʱ�䡢�ӳ�ʱ������Ϸ��ӳ�ʱ�䣨�������ͻ��㣩
void compute_route_time(Route &route) {                                                   //compute the traval time, wait time and delay time for a route
	route.travel_time = 0;
	route.delay_time = 0;
	route.wait_time = 0;
	vector<int>::iterator iter;
	total_wait[0] = 0;
	total_delay[0] = 0;
	for (iter = route.customers.begin(); iter != route.customers.end() - 1; ++iter) {
		double wait = 0, delay = 0;
		route.travel_time += peer_time[*iter][*(iter + 1)];
		a_time[*(iter + 1)] = route.travel_time;
		//���ӳ�ʱ��
		if (route.travel_time>customer[*(iter + 1)].e_time)	delay = route.travel_time - customer[*(iter + 1)].e_time;			//��ͻ��㣨1--- n - 1���ӳ�ʱ��
		route.delay_time += delay;
		total_delay[*(iter + 1)] = total_delay[*iter] + delay;								//�ܵ���һ������ӳ�ʱ�� = ���ͻ�����ӳ�ʱ�� + ��һ������ӳ�ʱ��
																							//��ȴ�ʱ��
		if (route.travel_time<customer[*(iter + 1)].b_time)	wait = customer[*(iter + 1)].b_time - route.travel_time;
		w_time[*(iter + 1)] = wait;
		total_wait[*(iter + 1)] = total_wait[*iter] + wait;
		route.wait_time += wait;
		route.travel_time += (wait + customer[*(iter + 1)].s_time);							//·��������ʻʱ�� = [k + 1]����ʻʱ�� + [k + 1]�ĵȴ�ʱ�� + [k + 1]�ķ���ʱ�䣻
		l_time[*(iter + 1)] = route.travel_time;											//�� k + 1���뿪ʱ�� = ��k + 1����ʻʱ��
	}
	iter = route.customers.begin();
	l_time[*iter] = 0;
	//������ͻ�������Ϸ��ӳ�ʱ��
	max_wait[*iter] = customer[*iter].e_time - a_time[*iter];								//�������Ϸ��ӳ�ʱ�� = �����ر�ʱ�� - ���ﳵ����ʱ��

	for (iter = route.customers.end() - 2; iter != route.customers.begin(); --iter) {			//����k......1�Ŀͻ�������Ϸ��ӳ�ʱ��
																								//�ٵ�																		�絽
		max_wait[*iter] = min(customer[*iter].e_time + max_delay_time - a_time[*iter], w_time[*iter] + max_wait[*(iter + 1)]);
	}
	iter = route.customers.begin();
	total_wait[*iter] = 0;
	total_delay[*iter] = 0;

}

//����·��route������
void compute_route_capacity(Route &route) {                                                      //compute the route capacity

	route.capacity = 0;
	vector<int>::iterator iter;
	for (iter = route.customers.begin() + 1; iter != route.customers.end() - 1; ++iter) {
		route.capacity += customer[*iter].demand;
		b_demand[*iter] = 0;
		a_demand[*iter] = 0;
	}
	iter = route.customers.begin();
	a_demand[*iter] = 0;
	b_demand[*iter] = 0;
	//����·���и����ͻ���i��·��֮ǰ������b_demand,��֮�������a_demand(����i������)
	for (iter = route.customers.begin() + 1; iter != route.customers.end() - 1; ++iter)
		b_demand[*iter] += (b_demand[*(iter - 1)] + customer[*iter].demand);
	for (iter = route.customers.end() - 2; iter >= route.customers.begin() + 1; --iter)
		a_demand[*iter] += (a_demand[*(iter + 1)] + customer[*iter].demand);

}

/*
��ʼ��·��,�����½⣺
�ֱ����������е�Ԫ�ز��뵽·���У������꣬�����½⣻�˴�û����:delay_time
���ڸտ�ʼ�Ŀͻ����ѡ��ͨ��234�У�int r_ind=rand()%cust.size();���������������ɵĽ��������
*/
void init_route(Chromosome &chrome) {
	clean_chromosome(chrome);
	vector<int> cust, temp_cust;						//����:�������ڵļ���
														//���ɷ������󼯺�
	for (int i = 1; i <= cust_num; ++i)					//[0,250]
		cust.push_back(i);							//�ú�����һ���µ�Ԫ�ؼӵ�vector�������

	Route new_route;								//����һ��·��
	while (true) {
		clear_route(new_route);						//����µ�·�� 
		new_route.customers.push_back(0);			//�ѳ������0��ӵ���ǰ·������
		double w_time = 0;							//��ʱ�����ȴ�ʱ���ʼ��Ϊ0					
		double t_time = 0;							//��ʱ������ʻʱ���ʼ��Ϊ0	
		double capacity = 0;							//��ʱ����������ʼ��Ϊ0	
		int last = 0;									//��¼��һ���ͻ�����							
		while (true) {                                                                                                         //create a route
			vector<int>::iterator iter = cust.begin();
			int r_ind = rand() % cust.size();			//���ѡ��һ���ͻ��������,���������ѡ��ͻ��㣬���в����Ľ��������
			t_time += peer_time[last][cust[r_ind]];	//·��ʱ�����·������ʻ��ʱ��
			w_time = t_time > customer[cust[r_ind]].b_time ? 0 : customer[cust[r_ind]].b_time - t_time;								//���ݹ�ʽ��ȴ�ʱ��
																																	//��������Լ����ʱ�䴰Լ�����жϣ��Ƿ���Լ��뵱ǰ·�������:
																																	//�����Լ��뵱ǰ·��
			if (capacity + customer[cust[r_ind]].demand > max_capacity ||		//����֮����������·�����������
				t_time > customer[cust[r_ind]].e_time + max_delay_time ||	//�ٵ�����ʻʱ����ڿͻ���r_ind�����ʱ�� + ����ӳ�ʱ��
																			//���ͻ�����뵽��i����ʱ������0��ʱ��,����󵽴ﳵ����ʱ����ڳ����������ʱ��
				t_time + w_time + customer[cust[r_ind]].s_time + peer_time[cust[r_ind]][0] > customer[0].e_time) {

				t_time -= peer_time[last][cust[r_ind]];//����ʻʱ�������һ����ʻʱ��
				temp_cust.push_back(cust[r_ind]);	//������ͻ������������ʱ�ͻ��㼯
				cust.erase(iter + r_ind);				//�����õĿͻ����cust�ͻ��㼯��ɾ��
				if (cust.size() == 0)
					break;			//�ͻ�����Ϊ0ʱ,����ѭ��
				continue;							//����,�����ж�
			}
			else {		//����ʱ�䴰Լ���복������Լ�������:
				t_time += w_time + customer[cust[r_ind]].s_time;		//��ʻʱ�� = ��һ����ʻʱ�� + ��r_ind �ͻ���ĵȴ�ʱ�� + ��r_ind�ͻ���ķ���ʱ��
				capacity += customer[cust[r_ind]].demand;				//��r_ind�ͻ�������� = ǰһ���ͻ�������� + ��r_ind�ͻ��������
				new_route.customers.push_back(cust[r_ind]);			//���ͻ�����뵽·����
																	//�˴��������ӳ�ʱ��
				last = cust[r_ind];
				cust.erase(iter + r_ind);								//�Ӽ���cust��ɾ����r_ind���ͻ���
				for (iter = cust.begin(); iter != cust.end(); ++iter)		//��ӳɹ��󣬽�ʣ��Ŀͻ�����뵽temp_cust�ͻ��㼯��
					temp_cust.push_back(*iter);						//��cust�����е�Ԫ��copy��temp_cust������
				cust = temp_cust;										//��ʣ��ķ����������¼��뵽cust������					
				temp_cust.clear();									//��ʱ�������

			}

			if (cust.size() == 0)									//�ͻ�����Ϊ0ʱ,����ѭ��;��cust�����е�Ԫ��Ϊ0����Ϊcust�еĿͻ��㶼���ʺ���ӵ���·��,���Կ�ʼ��·���Ĵ���
				break;
		}

		cust = temp_cust;
		temp_cust.clear();
		new_route.customers.push_back(0);							//·�������һ����0�Ĳ���
		compute_route_distance(new_route);							//������·������ʻ����
		compute_route_time(new_route);
		compute_route_capacity(new_route);
		chrome.routes.push_back(new_route);
		if (cust.size() == 0)
			break;
	}
}

/*���浵�еĽ����½���бȽ�
chrome_1:
chrome_2:
chrome_2�Ĺ��������ֵ������chrome_1��better = true
ֻҪchrome_2�Ĺ���������һ�������chrome_1С��2�ã�1���ã�����false
*/
bool is_better(Chromosome &chrome_1, Chromosome &chrome_2) {

	bool better = false;
	for (int i = 0; i<FUNC_NUM; ++i) {
		if (chrome_2.f[i] - chrome_1.f[i]>0.000001)
			better = true;
		if (chrome_1.f[i] - chrome_2.f[i]>0.000001)
			return false;	//ֻҪchrome_1�Ĺ���������һ������Ⱦɽ��chrome_1�ǻ��ģ�chrome_2�п�ȡ֮��������false
	}//chrome_2�Ĺ��������ֵ������chrome_1��better = true��chrome_1�Ǻõģ�chrome_2�ǻ���
	return better;
}

/*
���ܣ��Ƚ�chrome_1��chrome_2������
���з�����chrome_1��chrome_2ʱ��chrome_1���Ǻõ�
ֻҪchrome_1��һ�������ϱ�chrome_2��ͷ���false,����chrome_1������chrome_2
*/
bool is_better_box(Chromosome &chrome_1, Chromosome &chrome_2) {

	bool better = false;
	for (int i = 0; i<FUNC_NUM; ++i) {
		if (chrome_2.box_f[i] - chrome_1.box_f[i]>0.000001)				//���з�����chrome_1��chrome_2ʱ��chrome_1���Ǻõ�				
			better = true;
		if (chrome_1.box_f[i] - chrome_2.box_f[i]>0.000001)				//ֻҪchrome_1��һ�������ϱ�chrome_2��ͷ���false,����chrome_1������chrome_2
			return false;
	}
	return better;

}


/*
���ܣ���LS6,������ŵĲ���λ��,û�н�����·�����еĽ�����
����true:�ҵ����ŵ�λ����
����false:û�ҵ�����λ��
check_best_pos(chrome.routes[r_ind],c_ind,chrome.routes[i],c_pos,delta,r_lamda,chrome.routes.size(),dist,now_time,now_wait,now_delay)
check_best_pos(Route &org_route, int cust_pos, Route &route, int &c_pos, double &min_delta, double r_lamda[], int r_num, double now_dist,double now_time,double now_wait, double now_delay)
org_route:	chrome.routes[r_ind]����chrome�ĵ�r_ind��·��
cust_pos��	c_ind�������[0,r_ind.size - 2]�в���������
route��		chrome.routes[i]����chrome�ĵ�i��·��,���������ѭ��
c_pos��		c_pos			�ں����У�ֻ��ֵ�����ڼ�¼���Ų���λ��
min_delta��	delta = 0		��������
r_lamda[]��	r_lamda
r_num��		chrome.routes.size()���˽���·������Ŀ
now_dist��	dist����org_route(r_ind)��route(i) ·����·������֮��
now_time:	now_time 	//��¼һ��·������ʻʱ��,���Ż�f[2]��f[4]��f[5]��now_time = 0;���Ż�f[3]��now_time = ����ʻʱ�����·������ʻʱ��
now_wait��	now_wait,��org_route(r_ind)��route(i) ·���ĵȴ�ʱ��֮��
now_delay��	now_delay,��org_route(r_ind)��route(i) ·�����ӳ�ʱ��֮��

�����
��min_dalta��¼LS6����������ӦĿ���ֵ�����ж���f[2]��min_delta��¼����r_ind��i·�����·����ʻʱ��
�����ҵ����Ų���λ�ú󣬶���f[0]�����ٳ�����Ŀ��
����f[1]������ʻ���룻
����f[2]��r_ind·����i·�������ʻʱ��
����f[3]���ܵȴ�ʱ��
����f[4]�����ӳ�ʱ��
��c_pos��¼���ŵĲ���λ�õ�����
feasible��¼�Ƿ��ҵ�����λ��
*/
bool check_best_pos(Route &org_route, int cust_pos, Route &route, int &c_pos, double &min_delta, double r_lamda[], int r_num, double now_dist, double now_time, double now_wait, double now_delay) {    //2-opt*

	bool feasible = false;
	min_delta = 9999999;												//���ó�һ����ֵ
																		//LS6������ģ�
																		//i·���ľ�������,
	double	delta_dist = 0;
	//r_ind·����ʱ������,
	double	delta_time_org = 0;
	//i·����ʱ��������
	double	delta_time = 0;
	//����
	double	delta = 0;
	//r_ind·�����룬	
	double	dist_org;
	//i·�����룬	
	double	dist_mod;
	//r_ind·��ʱ�䣬
	double	time_org;
	//i·��ʱ��
	double	time_mod;

	Route temp_route = route;																					//chrome.routes[i]����chrome�ĵ�i��·��,���������ѭ��
	temp_route.customers[0] = cust_num + 1;																	//251,�ͻ���ı��
	temp_route.customers[temp_route.customers.size() - 1] = cust_num + 1;										//251
	compute_route_capacity(temp_route);
	compute_route_time(temp_route);
	compute_route_distance(temp_route);
	/*
	����Լ����飺
	r_ind·���е�cust_pos�ͻ���						 i·���Ŀͻ���ͻ��������
	*/
	vector<int>::iterator	org_c_iter = org_route.customers.begin() + cust_pos, c_iter;
	//��r_ind·��cust_pos�ͻ�����������i·���ʵ�c_iter�ͻ��������н��н���
	for (c_iter = temp_route.customers.begin() + 1; c_iter != temp_route.customers.end(); ++c_iter) {
		//����������У�ָ������һλ���ƶ�
		//����r_ind·����if cust_ind�ͻ���֮ǰ���� + c_iter֮������ > max_capacity��++c_iter
		if (b_demand[*org_c_iter] + a_demand[*c_iter] > max_capacity)
			continue;
		//����r_ind·�������������ͨ�������i·��
		//����i·����·��iֻʣc_iter - 1֮ǰ���֣�r_ind·��ֻʣcust_ind + 1֮�󲿷֣����Ӽ������Լ��
		if (b_demand[*(c_iter - 1)] + a_demand[*(org_c_iter + 1)] > max_capacity)
			continue;
		/*����i·�����Ӻ��������ͨ��������ʱ����
		��1��ʱ���飺���ͻ����ǲ���0
		��ʱ�������������c_iter��cust_ind +����	û�е�0
		��0
		*/
		if (*c_iter != 0) {
			if (l_time[*org_c_iter] + peer_time[*org_c_iter][*c_iter]>customer[*c_iter].e_time + max_delay_time)
				continue;
		}	//*c_iter == 0
		else {
			if (l_time[*org_c_iter] + peer_time[*org_c_iter][*c_iter]>customer[*c_iter].e_time) continue;
		}

		if (*(org_c_iter + 1) != 0) {  //·��r_ind�ͻ���c_ind����һ�㲻��0
			if (l_time[*(c_iter - 1)] + peer_time[*(c_iter - 1)][*(org_c_iter + 1)]>customer[*(org_c_iter + 1)].e_time + max_delay_time)continue;
		}
		else {
			if (l_time[*(c_iter - 1)] + peer_time[*(c_iter - 1)][*(org_c_iter + 1)]>customer[*(org_c_iter + 1)].e_time)continue;
		}
		/*
		�ڶ���ʱ���飬��i·����c_iter�����в���r_ind·��cust_ind��   ��   ��r_ind·��cust_ind + 1�����в���i·��c_iter - 1ǰ
		*/
		//��c_iter�����в���r_ind·������c_iterʱ�ģ�ʱ������
		delta_time_org = l_time[*org_c_iter] + peer_time[*org_c_iter][*c_iter] - a_time[*c_iter];
		if (delta_time_org > max_wait[*c_iter])	//���ܲ���
			continue;
		//c_iter�����в���r_ind·�����Բ���
		//��cust_ind + 1�����в���i·������org_c_iter+1ʱ�ģ�ʱ������
		delta_time = l_time[*(c_iter - 1)] + peer_time[*(c_iter - 1)][*(org_c_iter + 1)] - a_time[*(org_c_iter + 1)];
		if (delta_time>max_wait[*(org_c_iter + 1)])
			continue;
		//����ʱ�����
		//�󽻻���r_ind·������֮��dist_org
		dist_org = b_distance[*org_c_iter] + peer_distance[*org_c_iter][*c_iter] + a_distance[*c_iter];
		//�󽻻���i·������֮��dist_mod
		dist_mod = b_distance[*(c_iter - 1)] + peer_distance[*(c_iter - 1)][*(org_c_iter + 1)] + a_distance[*(org_c_iter + 1)];

		if (dist_org == 0) {
			--r_num;					//���·������1
		}
		if (dist_mod == 0) {
			--r_num;
		}

		double time_1 = 0;
		double time_2 = 0;
		double wait_1 = 0;
		double wait_2 = 0;
		double delay_1 = 0;
		double delay_2 = 0;
		//��0 ...... cust_pos - c_iter......0����ʻʱ��time_1,�ȴ�ʱ��wait_1,�ӳ�ʱ��delay_1
		time_1 = l_time[org_route.customers[cust_pos]];
		wait_1 = total_wait[org_route.customers[cust_pos]];
		delay_1 = total_delay[org_route.customers[cust_pos]];

		time_1 += peer_time[org_route.customers[cust_pos]][*c_iter];

		if (time_1 < customer[*c_iter].b_time) {					//С������ʱ��
			wait_1 += customer[*c_iter].b_time - time_1;
			time_1 = customer[*c_iter].b_time;
		}
		else if (time_1 > customer[*c_iter].e_time)				//�������ʱ��
			delay_1 += time_1 - customer[*c_iter].e_time;
		//��ʱ�䴰֮��
		time_1 += customer[*c_iter].s_time;
		for (vector<int>::iterator c_ind = c_iter + 1; c_ind != temp_route.customers.end(); ++c_ind) {
			time_1 += peer_time[*(c_ind - 1)][*c_ind];
			if (time_1 < customer[*c_ind].b_time) {
				wait_1 += customer[*c_ind].b_time - time_1;
				time_1 = customer[*c_ind].b_time;
			}
			else if (time_1>customer[*c_ind].e_time)
				delay_1 += time_1 - customer[*c_ind].e_time;
			time_1 += customer[*c_ind].s_time;
		}
		//��0 ...... c_iter - 1 -> cust_pos + 1......0����ʻʱ��time_2,�ȴ�ʱ��wait_3,�ӳ�ʱ��delay_2
		time_2 = l_time[*(c_iter - 1)];
		wait_2 = total_wait[*(c_iter - 1)];
		delay_2 = total_delay[*(c_iter - 1)];

		time_2 += peer_time[*(c_iter - 1)][org_route.customers[cust_pos + 1]];
		if (time_2<customer[org_route.customers[cust_pos + 1]].b_time) {
			wait_2 += customer[org_route.customers[cust_pos + 1]].b_time - time_2;
			time_2 = customer[org_route.customers[cust_pos + 1]].b_time;
		}
		else if (time_2>customer[org_route.customers[cust_pos + 1]].e_time) delay_2 += time_2 - customer[org_route.customers[cust_pos + 1]].e_time;
		time_2 += customer[org_route.customers[cust_pos + 1]].s_time;
		for (int i = cust_pos + 2; i<org_route.customers.size(); ++i) {
			time_2 += peer_time[org_route.customers[i - 1]][org_route.customers[i]];
			if (time_2<customer[org_route.customers[i]].b_time) {
				wait_2 = customer[org_route.customers[i]].b_time - time_2;
				time_2 = customer[org_route.customers[i]].b_time;
			}
			else if (time_2>customer[org_route.customers[i]].e_time) delay_2 += customer[org_route.customers[i]].e_time - time_2;
			time_2 += customer[org_route.customers[i]].s_time;
		}

		if (r_lamda[0] == 1) {													//������Ż�f[0],��delta ��¼ ·����	
			delta = r_num;
		}
		else if (r_lamda[1] == 1) {												//�����f[1](����ʻ����)�Ż���
			delta = now_dist + dist_org + dist_mod;
		}
		else if (r_lamda[2] == 1) {                                            //�����f[2]�·����ʻʱ���Ż���the longer time between both routes��
			if (time_2 > time_1)
				delta = time_2;
			else
				delta = time_1;

		}
		else if (r_lamda[3] == 1) {													//��f[3]�ܵȴ�ʱ������Ż�
			delta = now_wait + wait_1 + wait_2;
		}
		else {																	//��f[4]�ӳ�ʱ���Ż�
			delta = now_delay + delay_1 + delay_2;
		}

		if (min_delta > delta) {						//��min_dalta��¼LS6����������ӦĿ���ֵ�����ж���f[2]��min_delta��¼����r_ind��i·�����·����ʻʱ��
			min_delta = delta;
			c_pos = c_iter - temp_route.customers.begin();						//c_pos�����ŵĲ���λ��
			feasible = true;													//����Ϊtrue���ҵ����ŵ�λ����
		}

	}
	return feasible;

}

/*���ܣ���r_ind·�����������·������LS6������
�������£�
1.���ҵ����ŵ�r_pos��min_c_pos
2.�ж� �Ƿ��ҵ����Ų���λ�û��f[2]���Ż��Ƿ�������
false	: 	��chrome.routes[r_ind]����r_ind��ֳ�����,�磺
chrome.routes[temp_route_1] : 0,......,c_ind,0��
chrome.routes[temp_route_2] : 0,c_ind + 1,......,0
true	:	ʵ�ֽ���chrome.routes[r_ind] ��chrome.routes[r_pos]
����ǰ��
chrome.routes[r_ind] : 0,......,c_ind,c_ind + 1,......,0
chrome.routes[r_pos] : 0,......,min_c_pos,min_c_pos + 1,......,0
������
chrome.routes[r_ind] : 0,......,c_ind,c_pos,min_c_pos,......,0
chrome.routes[r_pos] : 0,......,min_c_pos - 1,c_ind + 1,......,0
��¼����С�Ķ�ӦĿ��ֵ���磺����f[1],��delta��¼��̵�����ʻ����,��min_c_pos��¼�����ŵĲ���λ��
r_pos����¼�½�����r_ind·���ɹ�����LS6������·������
r_ind:���ݲ�ͬ��Ŀ��ѡ�ò�ͬ�����ȵ�·��
*/
void two_opt(Chromosome &chrome, double r_lamda[], int r_ind, double now_time) {

	//1.���ҵ����ŵ�r_pos��min_c_pos
	int c_ind = rand() % (chrome.routes[r_ind].customers.size() - 1);
	compute_route_time(chrome.routes[r_ind]);
	compute_route_distance(chrome.routes[r_ind]);
	compute_route_capacity(chrome.routes[r_ind]);

	/*
	��r_ind·�����������·������LS6��������¼����С�Ķ�Ӧ����ֵ���ҵ�feasible = true;����Ϊfalse
	�磺����f[1],��delta��¼��̵�����ʻ����,��min_c_pos��¼�����ŵĲ���λ��

	*/
	double dist = 0, now_wait, now_delay;
	int c_pos = 0;
	//��¼ʹ��LS6���ŵĲ���λ������
	int min_c_pos = 0;
	double min_delta = 99999999, delta = 0;
	bool feasible = false;
	int r_pos;									//��¼�½�����r_ind·���ɹ�����LS6������·������
	for (int i = 0; i < chrome.routes.size(); ++i) {
		//���r_ind��i ·����·������֮�͡��ȴ�ʱ��֮�͡��ӳ�ʱ��֮�͡�
		if (i != r_ind) {
			//���r_ind��i ·����·������֮�͡��ȴ�ʱ��֮�͡��ӳ�ʱ��֮�͡�
			dist = 0; now_time = 0; now_wait = 0; now_delay = 0;
			for (int j = 0; j < chrome.routes.size(); ++j)
				if (j != r_ind && j != i) {
					dist += chrome.routes[j].travel_dist;				//��r_ind��i ·����·������֮��
					now_wait += chrome.routes[j].wait_time;				//��r_ind��i ·���ĵȴ�ʱ��֮��
					now_delay += chrome.routes[j].delay_time;			//��r_ind��i ·�����ӳ�ʱ��֮��
				}
			//��LS6,�������ŵĲ���λ�ã��ҵ�����λ�ú���delta��¼��ӦĿ���ֵ�����д���f[2]�ı�ʾ��r_ind·����i·�������ʻʱ��
			if (check_best_pos(chrome.routes[r_ind], c_ind, chrome.routes[i], c_pos, delta, r_lamda, chrome.routes.size(), dist, now_time, now_wait, now_delay)) {    //�ҵ�����λ��
				feasible = true;
				if (min_delta > delta) {
					min_delta = delta;
					min_c_pos = c_pos;
					r_pos = i;
				}
			}
		}
	}
	/*
	2.�ж� �Ƿ��ҵ����Ų���λ�û��f[2]���Ż��Ƿ�������
	false	: 	��chrome.routes[r_ind]����r_ind��ֳ�����,�磺
	chrome.routes[temp_route_1] : 0,......,c_ind,0��
	chrome.routes[temp_route_2] : 0,c_ind + 1,......,0
	true	:	ʵ�ֽ���chrome.routes[r_ind] ��chrome.routes[r_pos]

	*/

	if (!feasible || (r_lamda[2] == 1 && min_delta > now_time)) {			//û�ҵ���Ѳ��� || (���·����ʻʱ��f[2]�����Ż� || LS6�����f[2] > ԭ����f[2] )
																			/*
																			��chrome.routes[r_ind]����r_ind��ֳ�����,�磺
																			chrome.routes[temp_route_1] : 0,......,c_ind,0��
																			chrome.routes[temp_route_2] : 0,c_ind + 1,......,0
																			*/
		Route temp_route_1, temp_route_2;
		for (int i = 0; i <= c_ind; ++i)
			temp_route_1.customers.push_back(chrome.routes[r_ind].customers[i]);
		temp_route_1.customers.push_back(0);

		temp_route_2.customers.push_back(0);
		for (int i = c_ind + 1; i<chrome.routes[r_ind].customers.size(); ++i)
			temp_route_2.customers.push_back(chrome.routes[r_ind].customers[i]);
		compute_route_time(temp_route_1);
		compute_route_distance(temp_route_1);
		compute_route_time(temp_route_2);
		compute_route_distance(temp_route_2);

		//��·��chrome.routes[r_ind]�ӽ���ɾ��
		chrome.routes.erase(chrome.routes.begin() + r_ind);
		//��·��chrome.routes[temp_route_1]��chrome.routes[temp_route_2]��û�������·��Ҳ�ӽ���ɾ��
		if (temp_route_1.customers.size() != 2)  chrome.routes.push_back(temp_route_1);
		if (temp_route_2.customers.size() != 2)  chrome.routes.push_back(temp_route_2);
	}//�ҵ��� ||  ���Ƕ�f[2]�����Ż� || (LS6�����f[2] < ԭ����f[2] )
	else {
		vector<int> r1, r2;
		//��0 ......c_ind ��ӵ�r1��
		for (int i = 0; i <= c_ind; ++i)
			r1.push_back(chrome.routes[r_ind].customers[i]);
		//��(���Ų���λ��)min_c_pos ...... chrome.routes[r_pos].customers.size() -1 ��ӵ�r1��
		for (int i = min_c_pos; i<chrome.routes[r_pos].customers.size(); ++i)
			r1.push_back(chrome.routes[r_pos].customers[i]);
		//��0 ......min_c_pos-1 ��ӵ�r2��
		for (int i = 0; i<min_c_pos; ++i)
			r2.push_back(chrome.routes[r_pos].customers[i]);
		//��c_ind+1 ......chrome.routes[r_ind].customers.size() ��ӵ�r2��
		for (int i = c_ind + 1; i<chrome.routes[r_ind].customers.size(); ++i)
			r2.push_back(chrome.routes[r_ind].customers[i]);
		//�ֱ𽫿ͻ�������������Ӧ·���Ŀͻ�����������
		chrome.routes[r_ind].customers = r1;
		chrome.routes[r_pos].customers = r2;
		//�ֱ��������Ϊr_ind��r_pos·������ʻʱ�䡢��ʻ����ȣ�
		compute_route_time(chrome.routes[r_ind]);
		compute_route_distance(chrome.routes[r_ind]);
		compute_route_time(chrome.routes[r_pos]);
		compute_route_distance(chrome.routes[r_pos]);
		//���·����Ϊ��Ч·�����ӽ���ɾ����·��
		if (r1.size() == 2) chrome.routes.erase(chrome.routes.begin() + r_ind);
		if (r2.size() == 2) chrome.routes.erase(chrome.routes.begin() + r_pos);
	}
}

/*���ܣ�		��⽫cust����r_iter·�����Ƿ�������
���⣺��706��717����delay_time += customer[*c_iter].e_time - travel_time;��ԭ���������ģ�������������ӳ�ʱ����
check_best_pos(cust,*r_iter,c_pos,delta,r_lamda,now_dist,now_time,now_wait,now_delay)
cust_index	:	cust			�ͻ������ܿͻ��������е�����
route		:	r_iter			����Ҫ��������Ϊcust�ͻ����·��
c_pos		:	c_pos			��¼cust_indx���뵽route����ѵ�λ������
min_delta	:	delta			����󣬶�ӦĿ��ĺ���ֵ��f[2]���⣻
now_dist	:	now_dist		û�в���custermer[cust]�ͻ���ʱ���������ʻ����
now_time	:	now_time		û�в���custermer[cust]�ͻ���ʱ���������ʻʱ��
now_wait	:	now_wait		��r_iter��������·���ĵȴ�ʱ��֮��
now_delay	:	now_delay		��r_iter��������·�����ӳ�ʱ��֮��
*/
bool check_best_pos(int cust_index, Route& route, int &c_pos, double &min_delta, double r_lamda[], double now_dist, double now_time, double now_wait, double now_delay) {

	bool feasible = false;
	min_delta = 99999999; double delta_dist = 0, delta_time = 0;
	double capacity = 0, delta = 0;
	//����Լ�����
	vector<int>::iterator c_iter;
	for (c_iter = route.customers.begin() + 1; c_iter != route.customers.end() - 1; ++c_iter) {
		capacity += customer[*c_iter].demand;
	}
	capacity += customer[cust_index].demand;
	if (capacity>max_capacity)					//����Լ��ûͨ��
		return false;
	//����Լ�����ͨ�������ʱ��Լ��
	compute_route_time(route);
	for (c_iter = route.customers.begin() + 1; c_iter != route.customers.end(); ++c_iter) {
		double travel_time = 0;
		double wait_time = total_wait[*(c_iter - 1)];
		double delay_time = total_delay[*(c_iter - 1)];
		//���뵽c_iter֮ǰ���󵽴�cust_index��ʱ��
		a_time[cust_index] = l_time[*(c_iter - 1)] + peer_time[*(c_iter - 1)][cust_index];
		//�������cust_index��ʱ��δͨ��ʱ��Լ��������forѭ��
		if (a_time[cust_index] > customer[cust_index].e_time + max_delay_time)
			continue;
		//��ǰ��cust_index�ͻ���
		if (a_time[cust_index] < customer[cust_index].b_time) {
			wait_time += customer[cust_index].b_time - a_time[cust_index];
			a_time[cust_index] = customer[cust_index].b_time;
		}
		//�ٵ�cust_index�ͻ��㣬���ٵ�ʱ�䲻����max_delay_time
		if (a_time[cust_index] > customer[cust_index].e_time)
			delay_time += a_time[cust_index] - customer[cust_index].e_time;

		l_time[cust_index] = a_time[cust_index] + customer[cust_index].s_time;
		//�����cust_index��c_iterr ����ʱ��,��������cust_index�ͻ���󣬳�������ͻ���i��ʱ���ȥ����ǰ���������ͻ���i��ʱ��
		delta_time = l_time[cust_index] + peer_time[cust_index][*c_iter] - a_time[*c_iter];
		//����cust_index��c_iterr ����ʱ�� > c_iter�����Ϸ��ӳ�ʱ��
		if (delta_time>max_wait[*c_iter])
			continue;
		//��c_iter�ľ�������
		delta_dist = -peer_distance[*(c_iter - 1)][*c_iter] + peer_distance[*(c_iter - 1)][cust_index] + peer_distance[cust_index][*c_iter];
		//��c_iter����ʻʱ��
		travel_time = l_time[cust_index] + peer_time[cust_index][*c_iter];
		//�絽c_iter
		if (travel_time < customer[*c_iter].b_time) {
			wait_time += customer[*c_iter].b_time - travel_time;
			travel_time = customer[*c_iter].b_time;
		}
		//�ٵ�c_iter
		if (travel_time > customer[*c_iter].e_time)
			//delay_time += customer[*c_iter].e_time - travel_time;						//ԭ���������ģ�������������ӳ�ʱ����
			delay_time += travel_time - customer[*c_iter].e_time;

		travel_time += customer[*c_iter].s_time;
		for (vector<int>::iterator c_ind = c_iter; c_ind != route.customers.end() - 1; ++c_ind) {
			travel_time += peer_time[*c_iter][*(c_iter + 1)];
			if (travel_time < customer[*(c_iter + 1)].b_time) {
				wait_time += customer[*(c_iter + 1)].b_time - travel_time;
				travel_time = customer[*(c_iter + 1)].b_time;
			}
			//travel_time��ʾ����c_iter + 1����ʻʱ�䣬
			if (travel_time > customer[*(c_iter + 1)].e_time)
				//delay_time+=customer[*(c_iter+1)].e_time-travel_time;  //��ʦ���Ĵ�����������
				delay_time += travel_time - customer[*(c_iter + 1)].e_time; //����Ϊ��������

			travel_time += customer[*(c_iter + 1)].s_time;
		}

		if (r_lamda[1] == 1) {
			delta = now_dist + delta_dist;
		}
		else if (r_lamda[2] == 1) {
			delta = travel_time;
		}
		else if (r_lamda[3] == 1) {
			delta = now_wait + wait_time;
		}
		else
			delta = now_delay + delay_time;

		if (min_delta>delta) {
			min_delta = delta;
			c_pos = c_iter - route.customers.begin();
			feasible = true;
		}

	}
	return feasible;

}

/*����:�ж�cust�ͻ����Ƿ���Բ��뵽route·����,����ʵ�ֲ���
1.route			:	ĳ·��
2.cust			:	·���еĿͻ���
*/
bool check_feasible_place(Route &route, int cust) {
	double time = 0, capacity = 0;
	vector<int>::iterator c_iter;
	compute_route_time(route);
	//�����ӿͻ���cust�������Լ��
	for (c_iter = route.customers.begin() + 1; c_iter != route.customers.end() - 1; ++c_iter) {
		capacity += customer[*c_iter].demand;
	}
	capacity += customer[cust].demand;
	if (capacity > max_capacity)							//ûͨ������Լ��
		return false;
	//ʱ��Լ����飺���route����û�к��ʲ����λ��
	for (c_iter = route.customers.begin() + 1; c_iter != route.customers.end() - 1; ++c_iter) {					//��cust���뵽c_iter���ڵ�λ��
		double tmp_time = time + peer_time[*(c_iter - 1)][cust];
		if (tmp_time > customer[cust].e_time + max_delay_time) {													//����,c_iter֮ǰ�Ŀͻ��������⣬���ܲ���,��ȡһ�㴦��,����������
			time += peer_time[*(c_iter - 1)][*c_iter];
			if (time < customer[*c_iter].b_time) time = customer[*c_iter].b_time;
			time += customer[*c_iter].s_time;
			continue;
		}
		//���Բ���,c_iter֮ǰ�Ŀͻ���û����
		if (tmp_time<customer[cust].b_time) tmp_time = customer[cust].b_time;										//��tmp_time����Ϊcust�ͻ����b_time
																													//�ж�c_iter֮��Ŀͻ�������
		tmp_time += customer[cust].s_time + peer_time[cust][*c_iter];
		if (tmp_time <= a_time[*c_iter] || tmp_time <= customer[*c_iter].b_time) {								//�ж�c_iter����Ŀͻ���Ҳ���Բ���
																												//���������ǵ�һ��������λ��
			route.customers.insert(c_iter, cust);																//���ͻ���cust���뵽c_iter���ڵ�λ��,c_iter�ͻ������.
			compute_route_capacity(route);
			compute_route_distance(route);
			compute_route_time(route);
			return true;																						//���뵽���ʵ�λ��,�˳�����
		}//���c_iter����Ŀͻ��㲻���Բ���,������������
		time += peer_time[*(c_iter - 1)][*c_iter];
		if (time<customer[*c_iter].b_time) time = customer[*c_iter].b_time;
		time += customer[*c_iter].s_time;
	}//route·����[0,route.customers.end() - 2]û�к��ʵ�λ��,���뵽0֮ǰ��λ��
	time += peer_time[*(c_iter - 1)][cust];
	if (time > customer[cust].e_time + max_delay_time)																//���ܲ���
		return false;
	if (time<customer[cust].b_time)
		time = customer[cust].b_time;
	time += customer[cust].s_time + peer_time[cust][*c_iter];
	if (time <= customer[*c_iter].e_time) {																			//cust���Բ��뵽0֮ǰ��λ��
		route.customers.insert(c_iter, cust);
		compute_route_capacity(route);
		compute_route_distance(route);
		compute_route_time(route);
		return true;
	}//cust�����Բ��뵽0֮ǰ��λ��
	return false;

}

void check(Chromosome &chrome);

/*ɾ��·��route_ind
��route_ind·����ÿ���Ŀͻ��㳢�Բ�������·�������ֻʣ��0��0�ͻ���ʱ��ɾ��·��
ɾ���ɹ�������true
��������false
*/
bool route_number(Chromosome &chrome, int route_ind) {
	vector<int>::iterator c_iter;
	vector<Route>::iterator r_iter;

	//��route_ind·���еĿͻ�����б���
	for (c_iter = chrome.routes[route_ind].customers.begin() + 1; c_iter != chrome.routes[route_ind].customers.end() - 1;) {
		//����·���и����ͻ������ʻʱ�䡢�ȴ�ʱ�䡢�ӳ�ʱ������Ϸ��ӳ�ʱ�䣨�������ͻ��㣩
		compute_route_time(chrome.routes[route_ind]);
		/*
		·��route_ind�п���ɾ���Ŀͻ���Ҫ���������:
		ɾ����:
		1.��ɾ�������һ�ͻ�����0(����):ɾ����,���ﳵ����ʱ��ҪС�ڳ�������ٹر�ʱ��
		2.��ɾ������һ���ͻ��㲻�ǳ���:ɾ����,������һ���ͻ����ʱ��С���¸��ͻ�������ʱ��+�������ٵ�ʱ��
		*/
		if ((*(c_iter + 1) == 0 && l_time[*(c_iter - 1)] + peer_time[*(c_iter - 1)][*(c_iter + 1)] < customer[0].e_time) ||								//ɾ������һ���ͻ����ǳ���
																																						//2.c_iter != 0	
			(*(c_iter + 1) != 0 && l_time[*(c_iter - 1)] + peer_time[*(c_iter - 1)][*(c_iter + 1)] < customer[*(c_iter + 1)].e_time + max_delay_time)) {		//ɾ������һ���ͻ��㲻�ǳ���
			for (r_iter = chrome.routes.begin(); r_iter != chrome.routes.end(); ++r_iter) {													//·��0 - n-2,������n - 1(���һ��·��)													
				if (r_iter - chrome.routes.begin() != route_ind && check_feasible_place(*r_iter, *c_iter)) {								//*c_iter�ͻ�����Բ��뵽*r_iter·��
					c_iter = chrome.routes[route_ind].customers.erase(c_iter);//��Ϊc_iter�ͻ��㱻�Ƴ���c_iter = chrome.routes[route_ind].customers.begin()+1�������������ͻ���
					break;
				}
			}
			if (r_iter == chrome.routes.end())
				return false;
		}
		else
			return false;

		if (r_iter == chrome.routes.end())																		//c_iter��û���룬�����һ��·�� ���������ˣ�������һ��
			++c_iter;																												//ѭ������
	}
	if (chrome.routes[route_ind].customers.size() == 2) {
		chrome.routes.erase(chrome.routes.begin() + route_ind);																		//ɾ��route_ind·��
		return true;
	}
	return false;
}

/*	�˴�������
���chrome�в��뵥���ͻ���cust
insert_customer(chrome,r_lamda, customers[iter],now_time);

*/
void insert_customer(Chromosome &chrome, double r_lamda[], int cust, double now_time = 9999999) {
	//û�в���custermer[cust]�ͻ���ʱ���������ʻ����
	double now_dist = 0;
	//û�в���custermer[cust]�ͻ���ʱ������ܵȴ�ʱ��
	double now_wait = 0;
	////û�в���custermer[cust]�ͻ���ʱ��������ӳ�ʱ��
	double now_delay = 0;

	Route temp_route;
	vector<Route>::iterator r_iter, r_pos;
	//��Ŀǰ����ʻ����
	for (r_iter = chrome.routes.begin(); r_iter != chrome.routes.end(); ++r_iter) {
		now_dist += r_iter->travel_dist;
	}

	int c_pos = 0;
	int min_c_pos = 0;
	double min_delta = 99999999;/*double min_time=0;*/
	double delta = 0;
	bool feasible = false;

	for (r_iter = chrome.routes.begin(); r_iter != chrome.routes.end(); ++r_iter) {
		now_wait = 0;
		now_delay = 0;
		for (vector<Route>::iterator r_index = chrome.routes.begin(); r_index != chrome.routes.end(); ++r_index)
			if (r_iter != r_index) {
				//��r_iter��������·���ĵȴ�ʱ��
				now_wait += r_index->wait_time;
				//��r_iter��������·�����ӳ�ʱ��
				now_delay += r_index->delay_time;
			}
		//�˴�������
		if (check_best_pos(cust, *r_iter, c_pos, delta, r_lamda, now_dist, now_time, now_wait, now_delay)) {  //��⽫cust����r_iter·�����Ƿ�������
			feasible = true;
			if (min_delta>delta) {
				min_delta = delta;
				min_c_pos = c_pos;
				r_pos = r_iter;
			}
		}
	}

	//û���ҵ���Ѳ���λ��
	if (!feasible /*|| delta< min_delta*/ || (r_lamda[2] == 1 && now_time<min_delta)) {
		//�½�һ��·����0,cust,0
		clear_route(temp_route);
		temp_route.customers.push_back(0);
		temp_route.customers.push_back(cust);
		temp_route.customers.push_back(0);

		compute_route_time(temp_route);
		compute_route_distance(temp_route);
		compute_route_capacity(temp_route);
		chrome.routes.push_back(temp_route);
	}
	else { //�ҵ���Ѳ���λ�ã���cust���뵽���·��r_pos�����λ��min_c_pos��		 
		r_pos->customers.insert(r_pos->customers.begin() + min_c_pos, cust);
		compute_route_time(*r_pos);
		compute_route_distance(*r_pos);
	}
}

bool update_EP(Chromosome &new_chrome);
bool is_equal(Chromosome &chrome_1, Chromosome &chrome_2);

/*ԭ������û��ע��
void get_RP(int RP[],int size){

for(int i=0;i<size;++i)
RP[i]=i;
for(int i=0;i<size;++i){
int r0=i+rand()%(size-i);
std::swap(RP[i],RP[r0]);
}
}
*/
int RP[MAX];

/*
֮ǰ��������route�����ɾ��count���ͻ��� 950
�����д�·���Ƿ���Ч��
��Ч���أ�true
��Ч���أ�false
//ֻ������·���ϵ�ʱ��Լ����û�м������Լ��

���⣺ֻ���ʱ��Լ�������������Լ�����᲻��ɾ������ȡ���㣬�����Ļ��ﲻ���٣��᲻����ɺ���ĳ��Ļ��ﳬ��
*/
bool check_feasible_route(Route &route) {
	double time = 0;
	//ֻ�����ʱ��Լ����û�м������Լ��
	for (vector<int>::iterator c_iter = route.customers.begin(); c_iter != route.customers.end() - 1; ++c_iter) {
		time += peer_time[*c_iter][*(c_iter + 1)];
		//������Ч·�����������
		if (*(c_iter + 1) == 0 && time>customer[*(c_iter + 1)].e_time)			//�����һ��·����0������ʻʱ�� > ��һ���ͻ��㣨�����������ʱ�䣬����false.������������Ч·����
			return false;
		//�����һ��·������0������ʻʱ�� > ��һ���ͻ���Ĺر�ʱ�� + ����ӳ�ʱ�䣬����false.������������Ч·����
		if (*(c_iter + 1) != 0 && time>customer[*(c_iter + 1)].e_time + max_delay_time)
			return false;
		//��ĿǰΪֹ������Ч·��������ʻʱ�� < ��һ���ͻ�������翪��ʱ��
		if (time < customer[*(c_iter + 1)].b_time)
			time = customer[*(c_iter + 1)].b_time;
		time += customer[*(c_iter + 1)].s_time;
	}
	return true;
}

/*
���ܣ���ֽ�chrome�ĵ�r_ind��·��.�����µ�·����ӵ����·��������
������:���յ�temp_route����һ������һ��0
*/
void split_route(Chromosome &chrome, vector<int> &tempCusts, int r_ind) {
	Route temp_route;
	//���Ҫ��ֵ�·��
	vector<Route>::iterator r_iter = chrome.routes.begin() + r_ind;
	//
	vector<int>::iterator c_iter;
	double time = 0;
	temp_route.customers.push_back(0);
	//�����������ǿͻ���
	int previous_cust = 0, now_cust, tmp = 0, tmp2 = 0;

	for (c_iter = r_iter->customers.begin(); c_iter != r_iter->customers.end() - 1; ++c_iter) {
		now_cust = *(c_iter + 1);
		time += peer_time[previous_cust][now_cust];
		double return_time = time;

		if (return_time<customer[now_cust].b_time)
			return_time = customer[now_cust].b_time;
		return_time += customer[now_cust].s_time + peer_time[now_cust][0];

		if (time > customer[now_cust].e_time + max_delay_time || return_time>customer[0].e_time) {
			temp_route.customers.push_back(0);
			if (temp_route.customers.size() != 2)
			{
				compute_route_time(temp_route);
				compute_route_distance(temp_route);
				compute_route_capacity(temp_route);
				chrome.routes.push_back(temp_route);
			}
			temp_route.customers.clear();

			temp_route.customers.push_back(0);
			previous_cust = 0;
			time = 0;
			tmp = *c_iter;
			//�����1���ͻ���Ͳ���������,��ô��,�� 0,6
			if (tmp <= 0 && now_cust > 0)
			{
				tempCusts.push_back(now_cust);
			}
		}
		else {
			temp_route.customers.push_back(now_cust);
			if (time<customer[now_cust].b_time) time = customer[now_cust].b_time;
			time += customer[now_cust].s_time;
			previous_cust = now_cust;
		}
	}
	if (temp_route.customers.size() != 2)
	{
		compute_route_time(temp_route);
		compute_route_distance(temp_route);
		compute_route_capacity(temp_route);
		chrome.routes.push_back(temp_route);
	}
	chrome.routes.erase(chrome.routes.begin() + r_ind);
}

int adaptive_select_neighbor(double r_lamda[])
{
	int ind = 0;							//��¼Ҫ�Ż���Ŀ��
	vector <double> ls;
	for (int i = 0; i < FUNC_NUM; i++)
		if (r_lamda[i] == 1)
		{
			ind = i;
			break;
		}
	//����������̶ĵķ�����ѡ��Ŀ������
	double sum = 0;
	for (int i = 0; i < NEIGH_NUM; i++)
	{
		ls.push_back(LSArray[ind][i]);
		sum += LSArray[ind][i];
	}
	for (int i = 0; i < NEIGH_NUM; i++)
		ls[i] /= sum;
	ind2 = RouletteWheelSelection(ls);		//ѡ���ind2���ֲ�����
	LSTable[ind2] = true;
	lsn[ind2]++;
	return ind2;
}

//����Ŀ��-�ֲ���������
void updateobjls()
{
	double s[FUNC_NUM][NEIGH_NUM];
	//ÿ��Ŀ�������ȵ��ܺ�
	double objsum[FUNC_NUM];
	//����ÿ���ֲ�������ÿ��Ŀ���ƽ��������
	for (int i = 0; i < FUNC_NUM; i++)						//��i��Ŀ���ڸ����ֲ������е�����
	{
		objsum[i] = 0;
		for (int j = 0; j < NEIGH_NUM; j++)					//��j����������
		{
			s[i][j] = FIR[i][j] / lsn[j];
			objsum[i] += s[i][j];
			//�������ȹ�Ϊ0
			FIR[i][j] = 0;
		}
	}//��һ������

	for (int i = 0; i < FUNC_NUM; i++)						//��i��Ŀ���ڸ����ֲ������е�����
		for (int j = 0; j < NEIGH_NUM; j++)					//��i��Ŀ���ڵ�j���ֲ������ϵ�������
			if (objsum[i] != 0)
				s[i][j] /= objsum[i];						//�൱��CVi����������ƽ��ֵ
			else
				s[i][j] = 0;

	//��������
	for (int i = 0; i < FUNC_NUM; i++)
		for (int j = 0; j < NEIGH_NUM; j++)
			q[i][j] = 0.5 * q[i][j] + 0.5 * s[i][j];
	//�����Ը���
	for (int i = 0; i < FUNC_NUM; i++)			//Ŀ��ı���
	{
		double pmin = LSArray[i][0], qsum = 0;
		for (int j = 0; j < NEIGH_NUM; j++)    //�ҵ���Ŀ���Ӧ�ֲ������У���С�ġ�
		{
			if (pmin > LSArray[i][j])
				pmin = LSArray[i][j];
			qsum += q[i][j];
		}

		objsum[i] = 0;
		for (int j = 0; j < NEIGH_NUM; j++)
		{
			if (qsum != 0)
				LSArray[i][j] = pmin + (1 - NEIGH_NUM * pmin) * (q[i][j] / qsum);
			else
				LSArray[i][j] = pmin;
			objsum[i] += LSArray[i][j];
		}

		//Ŀ���Ӧ�ֲ�������һ��
		for (int j = 0; j < NEIGH_NUM; j++)
			LSArray[i][j] /= objsum[i];
	}
	//���
	for (int i = 0; i < NEIGH_NUM; i++)
	{
		LSTable[i] = false;
		lsn[i] = 0;
	}
	is_EP = false;
}

/*
���ܣ�remove k customers and re-inserted them to the best place one by one
�������ֲ�ͬ�ľֲ���������{"ɾ�������ͻ���","ɾ��n���ͻ���","��LS6�����������"}���Խ�����Ż������½����ִ��update_EP(chrome);����
������:���յ�temp_route����һ������һ��0 ,			split_route(chrome,r_ind);
�����⣨821����ֻ���ʱ��Լ�������������Լ�����᲻��ɾ������ȡ���㣬�����Ļ��ﲻ���٣��᲻����ɺ���ĳ��Ļ��ﳬ��		check_feasible_route(chrome.routes[r_ind])
*/
void localsearch_k(Chromosome &chrome, int &count, double r_lamda[], Chromosome &best, int index) {

	//���LS��ѡ��һ������н���뵽EP�У������table
	if (LSTable[0] && LSTable[1] && LSTable[2] && is_EP)
	{
		//ÿ���ֲ������ڸ���Ŀ���������ȵ�ƽ��ֵ
		updateobjls();
	}

	Chromosome previous = chrome;
	//�洢���ǿͻ���
	vector<int> customers;					//ɾ���ͻ���ļ���,�洢���ǿͻ�����ȫ���ͻ��������е�����
	Route temp_route;
	double now_time = 0;						//��¼һ��·������ʻʱ��,���Ż�f[1]��f[3]��f[4]��now_time = 0;���Ż�f[2]��now_time = ����ʻʱ�����·������ʻʱ��
	customers.clear();
	/*���ܣ���ͬĿ���ϵ�ѡ��·������
	�ӽ���ѡ��·���������·����ʻʱ��f[3]����ѡ��·���Ĳ��ԣ�ѡ������ʻʱ�����·����
	f[2]��f[4]��f[5]ѡ��·���Ĳ��ԣ������̶�ѡ����ԣ�ѡ��ϳ�·��
	*/
	int r_ind = 0;

	if (r_lamda[2] != 1)															///f[2]��f[4]��f[5]ѡ��·���Ĳ���
		r_ind = rand() % chrome.routes.size();												//���ѡ��һ��·��,now_time = 0;
	else {																		//f[3]���·����ʻʱ�䣩��ѡ��·���Ĳ��ԣ�ѡ������ʻʱ�����·����
		for (int i = 0; i<chrome.routes.size(); ++i)											//ѡ������ʻʱ�����·�� 
			if (now_time<chrome.routes[i].travel_time) {
				now_time = chrome.routes[i].travel_time;
				r_ind = i;
			}
	}
	/*
	{"ɾ�������ͻ���","ɾ��n���ͻ���","��LS6�����������"}���Խ���и��졣ѡ�еĸ��ʾ�Ϊ1/3.
	*/
	//�˴������ѡ�񣬸ĳ�����Ӧ��ѡ��
	//double p=rand()%1000/1000.0;				//����[0,1]��������������������ɾ�ȷ�ȵ�ǧ��λ�ϵ�[0,1]С��
	int k = adaptive_select_neighbor(r_lamda);
	//if(p < 1.0/3 ){								//ɾ��1������Ŀͻ���
	if (k == 0) {
		//		int r_ind=r_ind_1;
		int cust_ind = rand() % (chrome.routes[r_ind].customers.size() - 2) + 1;		//�������ͻ��������
		int cust = chrome.routes[r_ind].customers[cust_ind];						//�õ�r_ind·���ϵ�����Ϊcust_ind�Ŀͻ���
		customers.push_back(cust);												//���ͻ������ɾ������
		chrome.routes[r_ind].customers.erase(chrome.routes[r_ind].customers.begin() + cust_ind);		//��·����ɾ���ͻ���
		if (chrome.routes[r_ind].customers.size() == 2)							//���·����û�����ÿͻ��㣬��ӽ���ɾ����·��
			chrome.routes.erase(chrome.routes.begin() + r_ind);
		else {//���������·��
			  //ɾ��chrome.routes[r_ind]·����cust_ind�󣬼��ɾ�����·���Ƿ���Ч	
			if (check_feasible_route(chrome.routes[r_ind])) {//�����⣺ֻ���ʱ��Լ�������������Լ�����᲻��ɾ������ȡ���㣬�����Ļ��ﲻ���٣��᲻����ɺ���ĳ��Ļ��ﳬ��
				compute_route_time(chrome.routes[r_ind]);
				compute_route_distance(chrome.routes[r_ind]);
			}
			else {																//·����Ч,�Ͳ�ִ�·��
																				//�������·��:���յ�temp_route����һ������һ��0
				split_route(chrome, customers, r_ind);										//���·��,�����µ�·�����뵽����
			}
		}
	}
	//else if(p < 2.0/3 && p >= 1.0/3 ) {		//ɾ�������count������Ŀͻ���	
	else if (k == 1) {
		int count = rand() % (chrome.routes[r_ind].customers.size() - 2) + 1;			//���Ҫɾ���Ŀͻ�������[1,chrome.routes[r_ind].customers.size()-2]֮��
		for (int i = 0; i < count; ++i) {
			int cust_ind = rand() % (chrome.routes[r_ind].customers.size() - 2) + 1;	//��������ͻ�������
			customers.push_back(chrome.routes[r_ind].customers[cust_ind]);		//���ÿͻ�����뵽ɾ������
			chrome.routes[r_ind].customers.erase(chrome.routes[r_ind].customers.begin() + cust_ind);	//�ӽ��е�·����ɾ���˿ͻ���
		}
		if (chrome.routes[r_ind].customers.size() == 2)							//������е�����·����Ч,�ӽ���ɾ��·��
			chrome.routes.erase(chrome.routes.begin() + r_ind);
		else {		//·����  > 2		���⣺ֻ���ʱ��Լ�������������Լ�����᲻��ɾ������ȡ���㣬�����Ļ��ﲻ���٣��᲻����ɺ���ĳ��Ļ��ﳬ��
			if (check_feasible_route(chrome.routes[r_ind])) {
				compute_route_time(chrome.routes[r_ind]);
				compute_route_distance(chrome.routes[r_ind]);
			}
			else {
				split_route(chrome, customers, r_ind);
			}
		}
	}
	else	//ʹ��LS6������������½⣬��ɾ���ͻ��㣬���ڴ����r_lamda[]���鲻ͬ���Ż��ķ���Ҳ��ͬ
		two_opt(chrome, r_lamda, r_ind, now_time);


	//��ɾ�������еĿͻ�����뵽���в����½�
	for (int iter = 0; iter < customers.size(); ++iter)
		insert_customer(chrome, r_lamda, customers[iter], now_time);		//���chrome�в��뵥���ͻ���customers[iter]
	compute_f(chrome);								//chrome�ھֲ������б仯
	compute_f(best);								//best��¼��ԭ����chrome��ֵ
	int i;
	for (i = 0; i<FUNC_NUM; ++i)
		if (r_lamda[i] == 1)
			break;
	if (i != FUNC_NUM) index = i;
	if (best.f[index] > chrome.f[index]) {
		best = chrome;
		//		count=0;
	}
	else {
		if (chrome.f[index] > best.f[index]) chrome = best;
		//	++count;
	}
	/*}*/
	++count;
	if (update_EP(chrome))
	{
		double div = 0;
		for (int i = 0; i < FUNC_NUM; i++)       //Ŀ��
		{
			//df[i] = 0;
			div = previous.f[i] - chrome.f[i];
			if (div > 0)
				FIR[i][ind2] += (1.0 * div) / previous.f[i];					//�ֲ�����ind2��Ŀ��i�������ۼ�
																				//sum += df[i];
		}
		is_EP = true;
	}
	//if(!update_EP(chrome))
	//BETWEEN.push_back(chrome);
}

/*���� ���Խ���оֲ������������½⣬���´浵���Բ�ͬ��Ŀ�꣬�ֲ�������ͬ
�ֱ����
ɾ������·��   ���Ż�f[0];
��
���ѡȡ{"ɾ�������ͻ���","ɾ��n���ͻ���","��LS6�����������"}���Ż�f[1]��f[2]��f[3]��f[4]
���ն�Ҫ���´浵EP
*/
void local_search(Chromosome &chrome, double r_lamda[], int index) {
	if (r_lamda[0] == 1) {			//ͨ��ɾ������·�������ֲ��������Ż���һ��Ŀ��
									/*
									ѡ����пͻ��������ٵ�·����ɾ��������ͻ���ֱ����������·��
									*/
		while (true) {
			//����һ������������
			vector<int>::iterator c_iter;
			vector<Route>::iterator r_iter;
			int route_ind = 0;																			//��¼�ڼ���·��
																										//�ҵ������ҿͻ������ٵ�·���������򵥵����̶�ѡ����ԣ�����¼���±꣬����ɾ��·����
			int min_num = 999999;
			for (r_iter = chrome.routes.begin(); r_iter != chrome.routes.end(); ++r_iter) {
				if (min_num > r_iter->customers.size()) {
					min_num = r_iter->customers.size();
					route_ind = r_iter - chrome.routes.begin();
				}
			}
			if (!route_number(chrome, route_ind))														//ɾ��route_ind·��ʧ�ܣ�����whileѭ��
				break;
			//���ɾ��·���ɹ�����������ɾ��
		}
		//���½������·����������ʻ·������������ʻʱ��
		for (vector<Route>::iterator iter = chrome.routes.begin(); iter != chrome.routes.end(); ++iter) {
			compute_route_distance(*iter);
			compute_route_capacity(*iter);
			compute_route_time(*iter);
		}
		//�����½����Ŀ�꺯��ֵ
		compute_f(chrome);																		//�����½��5��Ŀ�꺯��ֵ
																								//���½����浵�ͱ߽�⣬���Ż��浵	
		update_EP(chrome);
		//if(!update_EP(chrome))
		//BETWEEN.push_back(chrome);
	}
	else {																						//�ֱ��f[1]��f[2]��f[3]��f[4]�����Ż�																					//
		int count = 0; Chromosome best = chrome;
		for (count = 0; count<1;) {
			//�������ֲ�ͬ�ľֲ���������{"ɾ�������ͻ���","ɾ��n���ͻ���","��LS6�����������"}���Խ�����Ż������½����ִ��cout++;update_EP(chrome);����
			localsearch_k(chrome, count, r_lamda, best, -1);
		}
		for (vector<Route>::iterator iter = chrome.routes.begin(); iter != chrome.routes.end(); ++iter) {
			compute_route_distance(*iter);
			compute_route_capacity(*iter);
			compute_route_time(*iter);
		}
		compute_f(chrome);
		//	update_EP(chrome,-1);
	}

}

/*�жϴ浵�еĽ����½��Ƿ����
chrome_1:�浵�еĽ⣻
chrome_2:�½�
ֻҪ�������ĳһ�����ֵ����0.00001��������Ͳ���ȡ�����false
��������true
*/
bool is_equal(Chromosome &chrome_1, Chromosome &chrome_2) {

	for (int i = 0; i<FUNC_NUM; ++i) {
		if (fabs(chrome_1.f[i] - chrome_2.f[i])>0.000001) return false;			//ֻҪ�������ĳһ�����ֵ����0.00001��������Ͳ���ȡ�����false
	}
	return true;
}

/*
���chrome_1��chrome2�Ĺ���������ȣ�����true
���򣬷���false
*/
bool is_equal_box(Chromosome &chrome_1, Chromosome &chrome_2) {

	for (int i = 0; i<FUNC_NUM; ++i) {
		if (abs(chrome_1.box_f[i] - chrome_2.box_f[i])>0.000001) return false;
	}
	return true;
}
//����浵A��ÿ����ռ�Ž��Ӧ��һ����������B = {B0,B1,B2,B3,B4}
void get_box(Chromosome &chrome) {
	for (int i = 0; i<FUNC_NUM; ++i) {
		if (chrome.f[i] == 0) chrome.box_f[i] = 0;
		else chrome.box_f[i] = log(chrome.f[i]) / log(1 + epsilon);					//�˴������⣬�������ǣ�chrome.box_f[i]=log(chrome.f[i] + 1)/log(1+epsilon);
	}
}

//��ĳ�������Ϻã��ͷ���true
bool is_extreme(Chromosome &chrome) {
	for (int index = 0; index<FUNC_NUM; ++index) {
		if (extreme[index].f[index] - chrome.f[index]>0.00001) return true;//����������Ϻã��ͷ���true
	}
	return false;
}

//�洢�߽�⣺��5��Ŀ�꿼���½⣬���½�洢������Ŀ���ϣ�ֵԽСԽ��
void preserve_extreme(Chromosome &chrome, int index) {

	if (extreme[index].f[index] - chrome.f[index]>0.00001) {				//����������Ϻã��õ����棬����
		extreme[index] = chrome;
	}

}


//���±߽��
void update_extreme(Chromosome &new_chromosome) {

	for (int i = 0; i<FUNC_NUM; ++i) {					//��5��Ŀ����п���
		preserve_extreme(new_chromosome, i);
	}
}

/*
���ܣ����½����浵�ͱ߽�⣨���ܲ��룬Ҳ���ܲ����룩�����Ż��浵
�����½�new_chromosome
1.����浵Ϊ��
���±߽��
��浵������½�
2.�浵��Ϊ��
if new chromosome is worse than the one in EP, return�������´浵

���򣬸��´浵
�ж��½��Ƿ��Ǽ�ֵ��
�Ǳ߽��
���½���浵�еĽ�Ƚϣ�����浵�е�ĳ����û�������½⣬��ɾ���浵�е��Ǹ���
����½����浵
���Ǳ߽��
���½���浵�еĽ�Ƚ�
����½����ڴ浵�⣬���½����浵
����浵�е�ĳ����û�������½⣬��ɾ���浵�е��Ǹ���
�½�����д浵�еĽⶼ��Ҳ��Ǽ�ֵ��
���½�Ĺ���������浵��Ĺ���������бȽ�
�浵�е�һ����Ĺ����������½����ȣ��˳�ѭ�������ٱȽ�
�浵�еĽ�Ĺ����������½�Ķ�����ȣ��Ҵ浵�Ľ�Ҳ�������½⡣		���½����浵

*/
bool update_EP(Chromosome &new_chromosome) {

	bool flag = false, f = false;
	//����浵A��ÿ����Ĺ�������
	get_box(new_chromosome);

	if (EP.size() == 0) {								//����⼯Ϊ��
														//���±߽��
		update_extreme(new_chromosome);
		//��浵������½�
		EP.push_back(new_chromosome);				//���½����浵��
		f = true;
	}
	else {											//�⼯��Ϊ��
		vector<Chromosome>::iterator iter = EP.begin();
		for (; iter != EP.end(); ++iter) {                            // if new chromosome is worse than the one in EP, return
			if (is_better(*iter, new_chromosome) || is_equal(*iter, new_chromosome)) return false;
		}
		iter = EP.begin();
		//�ж��½��Ƿ��Ǽ�ֵ��
		if (is_extreme(new_chromosome)) {					//	����½��Ǽ�ֵ��
			update_extreme(new_chromosome);
			iter = EP.begin();
			while (iter != EP.end()) {
				if (is_better(new_chromosome, *iter)) //���½���浵�еĽ�Ƚϣ�����浵�е�ĳ����û�������½⣬��ɾ���浵�е��Ǹ���
					iter = EP.erase(iter);
				else ++iter;
			}
			EP.push_back(new_chromosome);				//����½����浵
			f = true;

		}
		else {											//�½ⲻ�Ǽ�ֵ��
			iter = EP.begin();
			while (iter != EP.end()) {
				if (is_better(new_chromosome, *iter)) { //���½���浵�еĽ�Ƚϣ�����浵�е�ĳ����û�������½⣬��ɾ���浵�е��Ǹ���
					iter = EP.erase(iter);
					flag = true;
				}
				else ++iter;
			}
			if (flag) {									 //����½����ڴ浵�⣬���½����浵
				EP.push_back(new_chromosome);
				f = true;
			}
			else {										//�½�����д浵�еĽⶼ��Ҳ��Ǽ�ֵ��
				iter = EP.begin();
				for (; iter != EP.end(); ++iter) {			//���½�Ĺ���������浵��Ĺ���������бȽ�
					if (is_equal_box(new_chromosome, *iter) || is_better_box(*iter, new_chromosome)) break;
				}										//�浵�еĽ�Ĺ����������½�Ķ�����ȣ��Ҵ浵�Ľ�Ҳ�������½�
				if (iter == EP.end()) {
					EP.push_back(new_chromosome);		//���½����浵
					f = true;
				}
			}
		}
	}
	return f;
}


/*
���ܣ��ж�new_chromosome�Ƿ���ռ�Ž⣬ɾ���������new_chromosome��ռ�ŵģ���ռ�ŵ�new_chromosome�����ռ�Ž⼯total_best��
��new_chromosome����ռ�Ž⼯total_best�ɹ�������true
���򣬼���δ�ɹ�������false
*/

bool update_best(Chromosome &new_chromosome) {

	int size = EP.size();
	if (total_best.size() == 0)
		total_best.push_back(new_chromosome);
	else {
		//���浵EP�еĽ���new_chromosome���бȽ�
		vector<Chromosome>::iterator iter = total_best.begin();
		for (; iter != total_best.end(); ++iter) {
			//�������е�����iter��new_chromosome�Ƚϣ�iter�Ƿ�����new_chromosome    ||     iter��chomosome�Ƿ����
			if (is_better(*iter, new_chromosome) || is_equal(*iter, new_chromosome))
				return false;
		}//new_chromosome����EP�����еĽ�

		 //��EP��ȥ������new_chromosome�Ľ�
		iter = total_best.begin();
		while (iter != total_best.end()) {
			if (is_better(new_chromosome, *iter)) iter = total_best.erase(iter);
			else ++iter;
		}
		//��new_chromosome����ռ�Ž⼯
		total_best.push_back(new_chromosome);
	}
	return true;

}

bool is_convergence(int index, vector<Chromosome> &chrome) {
	double sum[FUNC_NUM];
	for (int i = 0; i<FUNC_NUM; ++i)sum[i] = 0;
	int size = chrome.size();
	if (index == -1) {
		for (int i = 0; i<size - 1; ++i) {
			if (chrome[i] == chrome[i + 1]) {
			}
			else return false;
		}
	}
	else {
		for (int j = 0; j<size - 1; ++j) sum[index] += fabs(chrome[j + 1].f[index] - chrome[j].f[index]);
	}
	for (int i = 0; i<FUNC_NUM; ++i)
		if (sum[i]>0.1) return false;
	return true;
}

/*
��ʼ����5���⣬����ÿ���������ӦĿ����Ż����磺
�Ȳ�����1���⣬
Ȼ�󣬶Ե�1�������f[0]�����ϵ��Ż���
�����´浵EP�ͱ߽�⼯
���գ�EP.size()���Ȳ�ȷ����extreme.size() == 5
��5���߽�ֵ�����Ŀ������Ϊ������磺��1���߽�ֵ�Ķ�ӦĿ����f[0]

*/
void init_pop() {

	//��5���߽�ֵ�����Ŀ������Ϊ������磺��1���߽�ֵ�Ķ�ӦĿ����f[0] = INF
	for (int i = 0; i<FUNC_NUM; ++i)
		extreme[i].f[i] = INF;
	Chromosome chrome;				//��ʼ��
	double r_lamda[FUNC_NUM];		//��־���ĸ�Ŀ������Ż�
	for (int i = 0; i < N; ++i) {			//����5���⣬����ÿ���������ӦĿ����Ż�
		clean_chromosome(chrome);
		init_route(chrome);			//��������½⣬����5����
		compute_f(chrome);			//������5��Ŀ��
		for (int j = 0; j < FUNC_NUM; ++j)
			r_lamda[j] = 0;			//��r_lamda������г�ʼ��
		r_lamda[i] = 1;
		//��1���⣬��f[0]���������������½⣻��2���⣬��f[1]�����Ż��������½�
		local_search(chrome, r_lamda, -1);				//�ֱ�����Ŀ������Ż�
														//		update_EP(chrome,-1);
	}
}

void check();

vector<Chromosome> temp_EP;

void get_tempEP() {
	temp_EP.clear();
	temp_EP = EP;
	for (int i = 0; i<FUNC_NUM; ++i) {
		vector<Chromosome>::iterator iter;
		for (iter = temp_EP.begin(); iter != temp_EP.end(); ++iter) {
			if (is_better(*iter, extreme[i]) || is_equal(*iter, extreme[i])) break;
		}
		if (iter == temp_EP.end()) {
			iter = temp_EP.begin();
			while (iter != temp_EP.end()) {
				if (is_better(extreme[i], *iter)) iter = temp_EP.erase(iter);
				else ++iter;
			}
			temp_EP.push_back(extreme[i]);
		}
	}

}





/*
�򵥵�ð�����򣬶�ĳǰ�����ϵĸ������ĳ��Ŀ�꺯��ֵ������������
m1:ÿ��ǰ�����ϸ������Ŀ��
fpara1[][2]: fpara1[][0],��������Ⱥ�е�����
fpara1[][1]��������ĳ��Ŀ���ϵĺ���ֵ��
*/
void sort1(int m1)
{
	float temp, temp1;
	int i1, j1, k1;
	for (k1 = 0; k1 < m1 - 1; k1++)
	{
		for (i1 = k1 + 1; i1 < m1; i1++)
		{
			if (fpara1[k1][1] > fpara1[i1][1])
			{
				temp = fpara1[k1][1];
				temp1 = fpara1[k1][0];
				fpara1[k1][1] = fpara1[i1][1];
				fpara1[k1][0] = fpara1[i1][0];
				fpara1[i1][1] = temp;
				fpara1[i1][0] = temp1;
			}
		}


	}
	return;
}

/*========================================================================*/
/*
rank	: 1,...,globalpop.maxrank
��ӵ������,globalpop.cub_len[a]
���ǰ����ӵ������
1.��ȡÿ��ǰ�����ϵĸ�����Ŀ:m1
2.ѭ������ÿ��Ŀ��
ǰ���ϸ��������ӦĿ��,����ð������(����)
�������Ľ����ֵ:
i = 0 || m1 - 1
length[i][0] = fpara1[i][0];
length[i][1] = 100*max;
����
length[i][0] = fpara1[i][0];
length[i][1] = fabs(fpara1[i+1][1]- fpara1[i-1][1])
3. ���ÿ�����,ӵ������:globalpop.cub_len[a] += length[i][1];

*/
void gshare()
{

	float length[2 * maxpop][2], max;
	int i, j, m1, a;
	float min, Diff;  // Added 18.08.2003
					  //ÿ��ǰ�����ϵĸ�����Ŀ
	m1 = EP.size();

	for (j = 0; j < FUNC_NUM; j++)
	{
		for (i = 0; i < m1; i++)
		{
			fpara1[i][0] = 0;
			fpara1[i][1] = 0;
		}

		for (i = 0; i < m1; i++)
		{//��Ⱥ�и��������
			fpara1[i][0] = i;
			//��Ⱥ�е�a������ĵ�j��Ŀ��
			//fpara1[i][1] = globalpop.fitness[a][j];
			fpara1[i][1] = EP[i].f[j];
		}
		sort1(m1); /*Sort the arrays in ascending order of the fitness*/

		max = fpara1[m1 - 1][1];
		min = fpara1[0][1];  // Added 18.08.2003
		Diff = max - min;      // Added 18.08.2003 and 5 subsequent lines
		if (Diff < 0.0)
		{
			printf("Something wrong in keepaliven.h\n");
			exit(1);
		}
		else if (Diff == 0)
			Diff = 1;
		for (i = 0; i < m1; i++)
		{
			//���ڱ߽��,��������ֵ
			if (i == 0 || i == (m1 - 1))
			{
				length[i][0] = fpara1[i][0];
				length[i][1] = 100 * max;
			}
			else	//�����м��,�������ڽ�ĺ���ֵ�����ľ��Թ�һ��ֵ
			{
				length[i][0] = fpara1[i][0];
				length[i][1] = fabs(fpara1[i + 1][1] - fpara1[i - 1][1]) / Diff; // crowding distances are normalized 18.08.2003
			}
		}
		for (i = 0; i < m1; i++)
		{
			a = (int)length[i][0];
			//��ӵ������
			EP[a].cub_len += length[i][1];
		}
	}
	for (i = 0; i < m1; i++)
	{//��Ⱥ�и��������
		fpara1[i][0] = i;
		//��Ⱥ�е�a������ĵ�j��Ŀ��
		//fpara1[i][1] = globalpop.fitness[a][j];
		fpara1[i][1] = EP[i].cub_len;
	}
	sort1(m1); /*Sort the arrays in ascending order of the fitness*/
	vector<double> sp;
	for (i = 0; i < m1; i++)
	{
		EP[(int)fpara1[i][0]].ranking = m1 - i;

	}
	return;
}

void getMinMax()
{

	for (int i = 0; i < FUNC_NUM; i++)
	{
		double min = 999999, max = 0;
		for (int j = 0; j < EP.size(); j++)
		{
			if (EP[j].f[i] < min)
				min = EP[j].f[i];
			if (EP[j].f[i] > max)
				max = EP[j].f[i];
		}
		MMV[i][0] = min;
		if (min == max)
			MMV[i][1] = min + 1;
		else
			MMV[i][1] = max;

	}
}


void updateObjPossible()
{
	getMinMax();
	for (int i = 0; i < FUNC_NUM; i++)
	{
		if (MMV[i][1] - MMV[i][0] == 0)
			MMV[i][1] = 1 + MMV[i][0];
		//��ĳ��Ŀ����Ż��������������Ŀ����Ż��������˽�Ķ����ԣ��Ӷ��Ż�������Ŀ��
		for (int j = 0; j < EP.size(); j++)
		{
			FP[j][i] = 1 - ((EP[j].f[i] - MMV[i][0]) * 1.0) / (MMV[i][1] - MMV[i][0]);
		}
	}
}

/*
D:\3\running\movrptw_change\input\test50-0-0-0-0.d1.tw0DistanceMatrix.dat D:\3\running\movrptw_change\input\test50-0-0-0-0.d1.tw0TimeMatrix.dat D:\3\running\movrptw_change\input\test50-0-0-0-0.d1.tw0Specs.dat D:\3\running\movrptw_change\out\test_WheelSelection_i 3420 50

֮ǰ����ʲô��
1.��ʼ�����пͻ��㡢�ͻ���֮�����ʻʱ�����ʻ����
2.ƴ��������ļ���·���������˽������ļ���ָ��
*/

void process(double stopTime) {
	clock_t start, finish;
	start = clock();
	//��ʼ����Ⱥ
	init_pop();

	Chromosome temp_chrome, chrome;
	double r_lamda[FUNC_NUM];
	int ind, ncount = 1;

	//��¼ÿ���ֲ������Ĵ���
	//�趨һ�����󣬼�¼�ڸ�LS������LS���ʵ�Ӱ��
	for (int i = 0; i < FUNC_NUM; i++)
	{
		for (int j = 0; j < NEIGH_NUM; j++)				//�������
		{
			FIR[i][j] = 0;
			q[i][j] = 0;
		}
		LSArray[i][2] = 1.0 / 2.0;
		LSArray[i][1] = 1.0 / 3.0;
		LSArray[i][0] = 1.0 / 6.0;
	}

	for (int j = 0; j < NEIGH_NUM; j++)
	{
		LSTable[j] = false;
		lsn[j] = 0;
	}
	//���¸����Ŀ��ѡ�����
	updateObjPossible();

	while (true) {

		clean_chromosome(temp_chrome);
		clean_chromosome(chrome);
		ind = rand() % EP.size();

		temp_chrome = EP[ind];
		chrome = temp_chrome;

		vector <double> sf;
		double sum = 0;
		for (int j = 0; j<FUNC_NUM; ++j)
		{
			r_lamda[j] = 0;
			//FP[ind][j]  += 0.000001;
			sum += FP[ind][j];
			sf.push_back(FP[ind][j]);
		}
		for (int j = 0; j < FUNC_NUM; j++)
			sf[j] /= sum;
		ind = RouletteWheelSelection(sf);
		r_lamda[ind] = 1;

		//����LSArray���Ա�ֲ�����ѡ���������	


		local_search(temp_chrome, r_lamda, -1);

		int count = 0;
		double div;

		if (!is_equal(temp_chrome, chrome) && is_equal(temp_chrome, EP.back()))
			//���³ɹ��ˣ�һ����Ӱ�����еĽ�
		{	//�������ֵ����Сֵ	
			//��¼ÿ��Ŀ��ĸı���
			updateObjPossible();
		}


		finish = clock();
		if ((double)((finish - start) / CLOCKS_PER_SEC) > stopTime)
			break;
	}
}






void check(Chromosome &chrome) {
	bool used[MAX];
	vector<Route>::iterator r_iter;
	for (int j = 0; j <= cust_num; ++j) used[j] = false;
	for (r_iter = chrome.routes.begin(); r_iter != chrome.routes.end(); ++r_iter) {
		double t_time = 0;
		double w_time = 0;
		double capacity = 0;
		vector<int>::iterator c_iter;
		for (c_iter = r_iter->customers.begin(); c_iter != r_iter->customers.end() - 1; ++c_iter) {
			t_time += peer_time[*c_iter][*(c_iter + 1)];
			if (*(c_iter + 1) != 0) {
				if (t_time>customer[*(c_iter + 1)].e_time + max_delay_time) {
					printf("time false!!\n");
				}
			}
			else {
				if (t_time>customer[*(c_iter + 1)].e_time) {
					printf("end time false!!\n");
				}
			}
			w_time = t_time> customer[*(c_iter + 1)].b_time ? 0 : customer[*(c_iter + 1)].b_time - t_time;
			t_time += w_time + customer[*(c_iter + 1)].s_time;
			capacity += customer[*c_iter].demand;
			if (capacity>max_capacity) printf(" capacity false!\n");
			if (!used[*c_iter]) used[*c_iter] = true;
		}
	}
	int count = 0;
	for (int j = 1; j <= cust_num; ++j) if (used[j]) ++count;
	if (count != cust_num) {
		printf("less customer! \n");
	}
}

/*
�Դ浵�еĽ�������򣬲��������ÿ�����5��Ŀ��ֵ��
��ʽ��
f[0] f[1] f[2] f[3] f[4]
......
*/
void output() {
	//���浵EP�еĽ⣬����
	sort(EP.begin(), EP.end());

	//
	vector<Chromosome>::iterator iter;
	//for(iter=EP.begin(); iter!=EP.end();++iter){
	//	vector<Route>::iterator r_iter;
	//	for(r_iter=iter->routes.begin(); r_iter!=iter->routes.end();++r_iter){
	//		vector<int>::iterator c_iter;
	//		for(c_iter=r_iter->customers.begin();c_iter!=r_iter->customers.end();++c_iter){
	//			printf("%d ",*c_iter);
	//		}
	//		printf("\n");
	//	}
	//	printf("the route number is : %lf\n",iter->f[0]);
	//	printf("total distance is:%lf\n", iter->f[1]);
	//	printf("makespan is:%lf\n", iter->f[2]);
	//	printf("total waiting time is:%lf\n", iter->f[3]);
	//	printf("total delay time is:%lf\n\n",iter->f[4]);
	//}
	//printf("total solutions: %d\n", EP.size());
	cout << "�����EP�н��" << EP.size() << "��Ŀ��ֵ��" << endl;
	for (iter = EP.begin(); iter != EP.end(); ++iter) {
		for (int i = 0; i<FUNC_NUM; ++i) {
			printf("%lf ", iter->f[i]);
		}
		printf("\n");
	}

}

void output_best() {

	sort(total_best.begin(), total_best.end());
	vector<Chromosome>::iterator iter;
	for (iter = total_best.begin(); iter != total_best.end(); ++iter) {
		printf("%lf %lf %lf %lf %lf\n", iter->f[0], iter->f[1], iter->f[2], iter->f[3], iter->f[4]);
	}
	//	printf("total solutions: %d\n", total_best.size());

}

/*
��EP�еĽ���м�飬����ǲ�����ȷ�⣬�Ƿ���ռ�Ž�
*/
void check() {

	bool used[MAX];
	//��ÿ�����·�����м�飬�����ͻ������Ƿ����cust_num 
	for (int i = 0; i < EP.size(); ++i) {													//����EPÿ����
		vector<Route>::iterator r_iter;
		for (int j = 0; j <= cust_num; ++j)												//���ͻ���ı�־�����ʼ��Ϊfalse
			used[j] = false;
		//�Խ���·������ʱ��Լ��������Լ�����
		for (r_iter = EP[i].routes.begin(); r_iter != EP[i].routes.end(); ++r_iter) {		//����ÿ�����е�·��
			double t_time = 0;
			double w_time = 0;
			double capacity = 0;
			vector<int>::iterator c_iter;
			for (c_iter = r_iter->customers.begin(); c_iter != r_iter->customers.end() - 1; ++c_iter) { //����ÿ��·���еĿͻ���
				t_time += peer_time[*c_iter][*(c_iter + 1)];
				//·��r_iter�ϵ�ʱ��Լ�����
				if (*(c_iter + 1) != 0) {
					if (t_time > customer[*(c_iter + 1)].e_time + max_delay_time)
						printf("time false!!\n");
				}
				else {
					if (t_time > customer[*(c_iter + 1)].e_time)
						printf("end time false!!\n");
				}//ʱ��Լ�����ͨ��
				w_time = t_time> customer[*(c_iter + 1)].b_time ? 0 : customer[*(c_iter + 1)].b_time - t_time;
				t_time += w_time + customer[*(c_iter + 1)].s_time;
				capacity += customer[*c_iter].demand;
				//����Լ�����
				if (capacity > max_capacity)
					printf(" capacity false!\n");
				if (!used[*c_iter])					//�������Ŀͻ�����б�ǣ�ÿ������ֻ����һ��
					used[*c_iter] = true;
			}
		}
		int count = 0;
		for (int j = 1; j <= cust_num; ++j)
			if (used[j]) ++count;
		if (count != cust_num)						//����������Ŀͻ������������ܵĿͻ�����
			printf("less customer! \n");
		//�����ܵĿͻ�����

		//printf("After check\n");
		//double dist=0;
		//for(r_iter=total_best[i].routes.begin(); r_iter!= total_best[i].routes.end(); ++r_iter){
		//	vector<int>::iterator c_iter;
		//	for(c_iter=r_iter->customers.begin(); c_iter!=r_iter->customers.end()-1; ++c_iter) dist+= peer_distance[*c_iter][*(c_iter+1)];
		//}
		//printf("distance is %lf\n", dist);
	}
	//��EP�еĽ������Ƿ���ռ�Ž�
	for (int i = 0; i<EP.size(); ++i)
		for (int j = 0; j<EP.size(); ++j)
			if (is_better(EP[i], EP[j])) {
				printf("%d is better than %d \n", i, j);
			}

}

//int main(){
//		char r_distance[100], r_time[100], r_spec[100];
//	//strcpy(r_distance,argv[1]);
//	//strcpy(r_time,argv[2]);
//	//strcpy(r_spec,argv[3]);
//	strcpy(r_distance,"distance.txt");
//	strcpy(r_time,"time.txt");
//	strcpy(r_spec,"spec.txt");
//	getData(r_distance,r_time,r_spec);
//	Route route;
//	route.customers.push_back(0);
//	route.customers.push_back(185);
//	route.customers.push_back(178);
//	route.customers.push_back(182);
//	route.customers.push_back(70);
////	route.customers.push_back(93);
//	route.customers.push_back(216);
//	route.customers.push_back(199);
//	route.customers.push_back(0);
//	compute_route_time(route);
//	 
//}

void initReadFile(char* c)
{


	vector<string> type;
	type.push_back("DistanceMatrix.dat");
	type.push_back("TimeMatrix.dat");
	type.push_back("Specs.dat");


	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 3; j++)
		{

			for (int k = 0; k < 5; k++)
			{

				for (int l = 0; l < 3; l++)
				{
					char path[100];
					char num[4];

					strcpy(path, c);
					strcat(path, "\\test");
					itoa(CP[i], num, 10);
					strcat(path, num);			//test50

					strcat(path, "-0-0-0-0.d");
					itoa(j, num, 10);
					strcat(path, num);			//test50-0-0-0-0.d0

					strcat(path, ".tw");
					itoa(k, num, 10);
					strcat(path, num);			//test50-0-0-0-0.d0.tw0

					strcat(path, type[l].c_str());
					files.push_back(path);
				}

			}
		}
	}
	return;
}

void readtime(char *c)
{
	fstream r_time(c, ios::in);

	for (int i = 0; i < 45; i++)
		r_time >> times[i];
}

/*
argc:�������������
����̨�����г������ƺ���뺬������ָ��, ��ô���Ĳ��������ݸ�argv[1]
�������ݣ�
argv:��ά�ַ����飻
argv[1]:�����ļ���·���ַ�����
argv[2]:ʱ���ļ���·���ַ�����
argv[3]:��Ⱥ�ļ���·���ַ�����
argv[4]������ļ��ĸ�·����ַ
argv[5]:stopTime�ַ���

D:\3\running\movrptw_change\input\test50-0-0-0-0.d1.tw0DistanceMatrix.dat D:\3\running\movrptw_change\input\test50-0-0-0-0.d1.tw0TimeMatrix.dat D:\3\running\movrptw_change\input\test50-0-0-0-0.d1.tw0Specs.dat D:\3\running\movrptw_change\out\5010_321\ 3420 50
D:\running\code\expercienceresult\mdls\input\test50-0-0-0-0.d1.tw0DistanceMatrix.dat D:\running\code\expercienceresult\mdls\input\test50-0-0-0-0.d1.tw0TimeMatrix.dat D:\running\code\expercienceresult\mdls\input\test50-0-0-0-0.d1.tw0Specs.dat D:\running\code\expercienceresult\mdls\out\1_321 2700 50
D:\running\code\expercienceresult\instance\Real-mdvrptw D:\running\code\expercienceresult\instance\times.txt D:\running\code\expercienceresult\instance\result\mdls_change2
*/

int main(int argc, char **argv) {				//һ��int���ͣ�һ������ָ�롣
	srand(time(NULL));							//�õ�����������ʱ�������������ʱ�� 
												//	 freopen("result.txt","w",stdout);
												//argc  >= 6
	clock_t start, finish;			//typedef long clock_t; long����
	char r_distance[100], r_time[100], r_spec[100];		//����char���͵�����һά����		�ֱ��ʾ��	���롢ʱ�䡢�ռ�
														//strcpy(r_distance,argv[0]);
	initReadFile(argv[1]);			//D:\3\experimentalresult\instance\Real-mdvrptw,�����Ǹ���.txt�ļ�

	readtime(argv[2]);										//����ʱ��
	for (int n = 0; n < 45; n++)
	{
		strcpy(r_distance, files[n].c_str());								//argv[1]:�����ļ���·��
		strcpy(r_time, files[n + 1].c_str());									//argv[2]:ʱ���ļ���·��
		strcpy(r_spec, files[n + 2].c_str());									//argv[3]:��Ⱥ�ļ���·��
																				//atof()��ɨ�����nptr�ַ���������ǰ��Ŀո��ַ���ֱ���������ֻ��������Ųſ�ʼ��ת����
																				//�������������ֻ��ַ�������ʱ��'\0'���Ž���ת��������������أ�str�ַ����ɰ��������š�С�����E(e)��ʾָ������
		double stopTime = times[n];						   //���ַ���ת���ɸ�����
														   //double stopTime=11400;
														   //����:1.��ʼ�����пͻ��㡢�ͻ���֮�����ʻʱ�����ʻ����

		char out_file[100];
		strcpy(out_file, argv[3]);				//����ļ�Ŀ¼
		char num[4];
		itoa(n, num, 10);
		strcat(out_file, "\\");
		strcat(out_file, num);
		if (_access(out_file, 0) == -1)
		{
			_mkdir(out_file);
		}

		cout << "���в��ԣ����r_distance��r_time��r_spec" << endl;
		cout << r_distance << endl;
		cout << r_time << endl;
		cout << r_spec << endl;

		cust_num = CP[n / 15];						//�ͻ�����Ŀ 

		getData(r_distance, r_time, r_spec);
		int i = 0;
		start = clock();								//���ش���������ĳ�����̻��������ѵ�ʱ�䡣
														//int i;
		for (i = 0; i < 30; ++i) {							//30��ʾ:��ʾÿ���������д���Ϊ30��

			char num[3];
			//���ܣ���һ����ת��Ϊ�ַ���
			/*
			itoa(i,num,10);
			i ----��Ҫת�����ַ���������
			num---- ת���󱣴��ַ����ı���
			10---- ת�����ֵĻ����������ƣ���10����˵��10����ת�����֡���������2��8��16�ȵ���ϲ���Ľ�������
			*/
			char out_file1[100];
			itoa(i, num, 10);
			strcpy(out_file1, out_file);
			strcat(out_file1, "\\");
			strcat(out_file1, num);			//outfile + "result" + i
			strcat(out_file1, ".txt");		//outfile + "result" + i + ".txt"
											/*
											freopen("in.txt","r",stdin);     //��in.txt �ж�������
											freopen("out.txt","w",stdout);  // ���������д��out.txt�� 		*/
			freopen(out_file1, "w", stdout);		//��ֻдw��ʽ���ļ�out_file
													//��ָ����ʱ�������г���	

			process(stopTime);
			//�Դ浵�еĽ�������򣬲��������ÿ�����5��Ŀ��ֵ��	
			output();
			for (int j = 0; j < EP.size(); ++j)
				//���ܣ��ж�new_chromosome�Ƿ���ռ�Ž⣬ɾ���������new_chromosome��ռ�ŵģ���ռ�ŵ�new_chromosome�����ռ�Ž⼯total_best��
				update_best(EP[j]);
			//��EP�еĽ���м�飬����ǲ�����ȷ�⣬�Ƿ���ռ�Ž�
			check();
			//EP�⼯���
			EP.clear();
		}

		//��ÿ����������30�ε��ܵ���ռ�Ž⼯total_best�����"final.txt"�ļ���
		finish = clock();
		char out_file1[100];
		strcpy(out_file1, out_file);
		strcat(out_file1, "\\");
		strcat(out_file1, "final.txt");
		freopen(out_file1, "w", stdout);
		output_best();
		//����б�Ҫ��
		check();
	}//n�������ڼ�������

	 //	printf("time: %lf\n",(double)((finish-start)/CLOCKS_PER_SEC)/i);
	 //cout << "������!" << endl;


	return 0;


}

