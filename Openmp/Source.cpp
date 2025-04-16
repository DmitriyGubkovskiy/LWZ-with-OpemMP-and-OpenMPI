#include<stdio.h>
#include<omp.h>
#include<vector>
#include<string>
#include<map>
#include <iostream>
using namespace std;


string Generation(const string& base, const int count);
vector<string> LZW(const string& str, const string& alf);
string De_LZW(const vector<string>& str, const string& alf);

int main(int argc, char** argv) {

	string alf = "qazwsxedcrfvtgbyhnujmikolp123456789";
	int COUNT_SYMBOLS = 10000;


	int thread_id;
	
	
	
	omp_set_num_threads(4);
	#pragma omp parallel num_threads(4)
	{
		
		thread_id = omp_get_num_threads();
		cout << thread_id << '\n';
	}
	
	

	int size_str = COUNT_SYMBOLS / thread_id;
	
	vector<string> vec_str(thread_id, string());

	#pragma omp parallel shared(vec_str, size_str)
	{
		int id_par = omp_get_thread_num();
		vec_str[id_par] = Generation("qazwsxedcrfvtgbyhnujmikolp123456789", size_str);
	}

	string in_string;
	for (auto x: vec_str)
	{
		in_string += x;
	}
	
	

	vector<vector<string>> LZW_vec(thread_id, vector<string>());

	#pragma omp parallel shared(vec_str, LZW_vec)
	{
		int id_par = omp_get_thread_num();
		LZW_vec[id_par] = (LZW(vec_str[id_par], "qazwsxedcrfvtgbyhnujmikolp123456789"));
		cout << "Part " << id_par << " was encoded" << '\n';
	}



	vector<string> De_LZW_vec(thread_id, string());

	#pragma omp parallel shared(LZW_vec, De_LZW_vec)
	{
		int id_par = omp_get_thread_num();
		De_LZW_vec[id_par] = (De_LZW(LZW_vec[id_par], "qazwsxedcrfvtgbyhnujmikolp123456789"));
		cout << "Part " << id_par << " was decoded" << '\n';
	}
    


	string end_str;
	for (string x : De_LZW_vec)
	{
		end_str += x;
	}
	cout << (end_str  == in_string)  << '\n';

	return 0;
}

vector<string> LZW(const string& str, const string& alf)
{
	map<string, int> dict;
	int dictSize = alf.length();
	for (int i = 0; i < dictSize; i++)
	{
		dict[string(1, alf[i])] = i;
	}
	vector<string> res;
	string s;
	for (char z : str)
	{
		if (dict.count(s + z))
		{
			s += z;
		}
		else
		{
			res.push_back(to_string((dict[s])));
			dict[s + z] = dictSize++;
			s = z;
		}
	}
	if (s.size())
	{
		res.push_back(to_string((dict[s])));
	}

	return res;
}

string De_LZW(const vector<string>& str, const string& alf)
{
	string tmp;
	map<int, string> inv_dict;
	int dictSize = alf.size();
	for (int i = 0; i < dictSize; i++)
	{
		inv_dict[i] = string(1, alf[i]);
	}
	string s, entry, res;
	s = res = inv_dict[stoi(str[0])];
	for (int i = 1; i < str.size(); i++)
	{
		int k = stoi(str[i]);
		if (inv_dict.count(k))
		{
			entry = inv_dict[k];
		}
		else if (k == dictSize)
		{
			entry = s + s[0];
		}
		res += entry;
		inv_dict[dictSize++] = s + entry[0];
		s = entry;
	}
	return res;
}

string Generation(const string& base, const int count)
{
	srand(time(0));
	string gen_str = "";

	for (int i = 0; i < count; i++)
	{
		gen_str += base[rand() % base.length()];
	}

	return gen_str;
}