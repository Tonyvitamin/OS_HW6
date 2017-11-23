#include <iostream>
#include <fstream>
#include <algorithm>
#include <vector>
#include <list>
#include <map>
#include <string>
#include <sys/time.h>
using namespace std;
struct Node
{
	Node *next;
	Node *prev;
	string key;
	int value;
	Node(string tmp):key(tmp){};
	Node():value(0){};	
};
Node* head = new Node();
Node* tail = new Node();

void insert_node(Node* node , int flag)
{
	if(flag==0)
	{
		tail = node ;
		head = node ;
		return;
	}
	else 
	{
		tail->next = node;
		node->prev = tail;
		tail = tail->next;
	}
}
void move_node(Node * node)
{
	if(node==tail)
	{
		return;
	}
	else if(node==head)
	{
		node->next->prev = node->next;
		head = node->next;
		tail->next = node;
		node->prev = tail;
		tail = tail->next;
	}
	else
	{
		node->next->prev = node->prev;
		node->prev->next = node->next;
		tail->next = node;
		node->prev = tail;
		tail = tail->next;
	}
}
void delete_node()
{
	head->next->prev = head;
	head = head->next;
}
//struct timeval	thread8 , thread8_end , thread7 , thread7_end;
int main()
{
	fstream fin;
	fin.open("trace.txt", fstream::in);
	//FIFO 
	cout<<"FIFO---\n";
	cout<<"size	miss 	hit 		page fault ratio\n";
	//gettimeofday(&thread8 , 0);
	for(int i=64;i<=512;i*=2)
	{
		list<string> fifo_queue;
		map<string , int> fifo_hash_table;
		int start = 0;
		string input;
		
		unsigned long long int hit_count=0;
		unsigned long long int miss_count=0;
		double pagefault_ratio;
		
		while(getline(fin,input))
		{
			string sub = input.substr(3 , 5);
			if(start==0)
			{
				start++;
				fifo_hash_table.insert(pair<string , int>(sub , 0));
				fifo_queue.push_back(sub);
				miss_count++;
			}
			else if(fifo_hash_table.end()!=fifo_hash_table.find(sub))
			{
				hit_count++;
			}
			else 
			{
				if(miss_count < i)
				{
					fifo_hash_table.insert(pair<string , int>(sub , 0));
					fifo_queue.push_back(sub);
					miss_count++;					
				}
				else 
				{
					string out = fifo_queue.front();
					fifo_queue.pop_front();
					fifo_queue.push_back(sub);
					fifo_hash_table.erase(out);
					fifo_hash_table.insert(pair<string , int>(sub , 0));
					miss_count++;
				}
			}
		}
		fin.clear();
		fin.seekg(0);
		pagefault_ratio = (double)miss_count / (double)(hit_count+miss_count);
		fifo_hash_table.clear();
		fifo_queue.clear();
		cout<<i<<"	 "<<miss_count<<" 	"<<hit_count<<" 	"<<pagefault_ratio<<endl;
	}
	//gettimeofday(&thread8_end , 0);
	//double sec = thread8_end.tv_sec - thread8.tv_sec;
    //double msec = thread8_end.tv_usec - thread8.tv_usec;
    //cout<<"elased time of threadpool size 8 is  "<<sec+msec/1000000<<" ms\n";
	//LRU
	cout<<"LRU---\n";
	cout<<"size	miss 	hit 		page fault ratio\n";
	//gettimeofday(&thread7 , 0);
	for(int i=64;i<=512;i*=2)
	{
		int start = 0;
		string input;
		map<string , Node*> LRU_hash_table;
		unsigned long long int hit_count=0;
		unsigned long long int miss_count=0;
		double pagefault_ratio;
		int size = i;
		tail->next = tail;
		head->prev = head;

		while(getline(fin , input))
		{
			string sub = input.substr(3 , 5);
			if(start==0)
			{
				Node* node = new Node(sub);
				LRU_hash_table[sub] = node;
				insert_node(node , 0); 
				miss_count++;
				start++;
			}
			else
			{
				if(LRU_hash_table.find(sub)!=LRU_hash_table.end())
				{
					move_node(LRU_hash_table[sub]);
					hit_count++;
				}
				else if(miss_count < size)
				{
					Node* node = new Node(sub);
					LRU_hash_table[sub] = node;
					insert_node(node , 1);
					miss_count++;
				}
				else
				{
					LRU_hash_table.erase(head->key);
					delete_node();
					Node* node = new Node(sub);
					LRU_hash_table[sub] = node;
					insert_node(node , 1);
					miss_count++;
				}
			}
			input.clear();
		}
		fin.clear();
		fin.seekg(0);
		LRU_hash_table.clear();
		Node* tmp1 = head;
		//for(int j=0;j<size;j++)
		//{
		//	delete tmp1;
		//	tmp1 = tmp1->next;
		//}
		head = new Node();
		tail = new Node();
		pagefault_ratio = (double)miss_count / (double)(hit_count+miss_count);
		cout<<i<<"	 "<<miss_count<<" 	"<<hit_count<<" 	"<<pagefault_ratio<<endl;		
	}	
	//gettimeofday(&thread7_end , 0);
	//double sec1 = thread7_end.tv_sec - thread7.tv_sec;
    //double msec1 = thread7_end.tv_usec - thread7.tv_usec;
    //cout<<"elased time of threadpool size 7 is  "<<sec1+msec1/1000000<<" ms\n";
}