#include <iostream>
#include <vector>
#include <string>
#include <fstream> 
#include <bits/stdc++.h>
#include <pthread.h>
#define TNUM 4 
using namespace std;


vector <vector<float>> weight;
vector<float> maxis;
vector<float> minis;
int total_match = 0;
pthread_t threads[TNUM];
pthread_mutex_t mutex_match;

class ThreadInfo{
public:
	vector<vector<float>> data;
	vector<float> mins;
	vector<float> maxs;
	string adr;
	ThreadInfo(string address){
		adr = address;
	}
};

void compare(vector<int>result, vector<vector <float>>data){

	int size = result.size();
	int match = 0;
	int col = data[0].size() - 1;
	for (int i=0; i< size; i++)
		if (result[i] == (int)data[i][col])
			match ++;
	pthread_mutex_lock(&mutex_match);
	total_match += match;
	pthread_mutex_unlock(&mutex_match);
	
}


void calculate(vector<vector<float>> data){
	vector<int> result;
	int size1 = data.size();
	int size2 = data[0].size() - 1;
	for(int i=0; i< size1;i++){
		float score;
		int group;
		for (int k=0; k<weight.size(); k++){
			float cur = 0;
			int j;
			for(j=0; j<size2; j++){
				cur += data[i][j] * weight[k][j];
			}
			cur += weight[k][j];
			if(k == 0){
				score = cur;
				group = k; 

			}
			else{
				if(cur > score){
					score = cur;
					group = k;
				}
			}
		}
		result.push_back(group);
	}
	compare(result, data);


}



void* normalize(void* arg){
	ThreadInfo *td = (ThreadInfo*)arg;
    
	int size1 = td->data.size();
	int size2 = td->data[0].size() - 1;

	for(int i=0; i< size1; i++){
		for (int j=0; j<size2; j++){
			td->data[i][j] = (td->data[i][j] - minis[j]) / (maxis[j] - minis[j]);
		}
	}
	calculate(td->data);
	
}

void* read_data(void* address){
	
	ThreadInfo *td = (ThreadInfo*)address;
	string adr = td->adr;
	fstream fin;
    fin.open(adr, ios::in); 
    string line;
    int i =0;
    float mini , maxi;
    while(getline(fin, line)){
    	if (i==0){
    		i ++;
    		continue;
    	}
        stringstream s(line);
        string word;
        
        td->data.push_back(vector<float>());
        int j = 0;
        while (getline(s, word, ',')) {
        	float item = stof(word);
        	if(i==1){
        		td->mins.push_back(item);
        		td->maxs.push_back(item);
        	}
        	else{
        		if(item < td->mins[j])
        			td->mins[j] = item;
        		if (item > td->maxs[j])
        			td->maxs[j] = item;
        	}

           td->data[i - 1].push_back(item);
           j++;
        } 

        i++;    
    }
    
    fin.close();
    pthread_exit(NULL);
}

void read_weight(vector<vector<float>>& weight, string adr){
	fstream fin; 
    fin.open(adr, ios::in); 
    string line;
    int i =0;
    while(getline(fin, line)){
    	if (i == 0){
    		i++;
    		continue;
    	}
        stringstream s(line);
        string word;
        
        weight.push_back(vector<float>());
        while (getline(s, word, ',')) {
           weight[i - 1].push_back(stof(word));
        } 

        i++;    
    }
    fin.close();

    
}

void find_minmax(vector<ThreadInfo*> thread_inf){
	int size = thread_inf[0]->data[0].size() - 1;
	for(int i=0; i<TNUM; i++){
		for(int j=0; j<size; j++){
			if(i==0){
				maxis.push_back(thread_inf[i]->maxs[j]);
				minis.push_back(thread_inf[i]->mins[j]);
			}
			else{
				if(maxis[j] < thread_inf[i]->maxs[j])
					maxis[j] = thread_inf[i]->maxs[j];
				if(minis[j] > thread_inf[i]->mins[j])
					minis[j] = thread_inf[i]->mins[j];
			}
		}
	}

}

int find_size(vector<ThreadInfo*> thread_inf){
	int size = 0;
	for(int i=0; i<TNUM; i++)
		size += thread_inf[i] -> data.size();
	return size;


}









int main(int argc, char** argv){
	void* status;
	read_weight(weight, string(argv[1]) + "weights.csv");
	pthread_mutex_init(&mutex_match, NULL);
	vector<ThreadInfo> temp;
	for(int i = 0; i < TNUM; i++){
		string adr = string(argv[1]) + "train_" + to_string(i) + ".csv";
		temp.push_back(ThreadInfo(adr));
	}
	vector<ThreadInfo*> thread_inf;

	for(int i = 0; i < TNUM; i++)
		thread_inf.push_back(&temp[i]);
	
	

	for(int i = 0; i < TNUM; i++)
		pthread_create(&threads[i], NULL, read_data, thread_inf[i]);
		
	for(int i = 0; i <TNUM; i++)
		pthread_join(threads[i], NULL);
	
	find_minmax(thread_inf);
	int total_size = find_size(thread_inf);

	for(int i = 0; i < TNUM; i++)
	
		pthread_create(&threads[i], NULL, normalize, thread_inf[i]);
		
	

	
	
	for(int i = 0; i <TNUM; i++)
		pthread_join(threads[i], NULL);


	float accuracy = (float) total_match / total_size;
	printf("Accuracy: %.2f%c\n", accuracy * 100, '%');
	pthread_mutex_destroy(&mutex_match);
	pthread_exit(NULL);
	
	


}