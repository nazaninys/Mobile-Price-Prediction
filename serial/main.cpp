#include <iostream>
#include <vector>
#include <string>
#include <fstream> 
#include <bits/stdc++.h>

using namespace std;

void read_data(vector <vector<float>>& data, vector<float>& maxs, vector<float>& mins, string adr){
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
        
        data.push_back(vector<float>());
        int j = 0;
        while (getline(s, word, ',')) {
        	float item = stof(word);
        	if(i==1){
        		mins.push_back(item);
        		maxs.push_back(item);
        	}
        	else{
        		if(item < mins[j])
        			mins[j] = item;
        		if (item > maxs[j])
        			maxs[j] = item;
        	}

           data[i - 1].push_back(item);
           j++;
        } 

        i++;    
    }
    fin.close();
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

void normalize(vector<vector<float>>& data, vector<float> maxs, vector<float> mins){
	int size1 = data.size();
	int size2 = data[0].size() - 1;

	for(int i=0; i< size1; i++){
		for (int j=0; j<size2; j++){
			data[i][j] = (data[i][j] - mins[j]) / (maxs[j] - mins[j]);
		}
	}
}

float calculate(vector<vector<float>> data, vector<vector<float>> weight){
	int size1 = data.size();
	int size2 = data[0].size() - 1;
	int match = 0;
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
		if(group == (int) data[i][size2])
			match ++;
	}
	float accuracy = (float)match / size1;
	return accuracy;


}

int main(int argc, char** argv){
	vector <vector<float>> data;
	vector <vector<float>> weight;
	vector<int> result;
	vector <float> mins;
	vector <float> maxs;
	read_data(data, maxs, mins, string(argv[1]) + "train.csv");
	read_weight(weight, string(argv[1]) + "weights.csv");
	normalize(data, maxs, mins);
	float accuracy = calculate(data, weight);
	printf("Accuracy: %.2f%c\n", accuracy * 100, '%');
	
}