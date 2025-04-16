#include<stdio.h>
#include<vector>
#include<string>
#include<map>
#include <iostream>
#include <mpi.h>
using namespace std;

string Generation(const string& base, const int count);
vector<string> LZW(const string& str, const string& alf);
string De_LZW(const vector<string>& str, const string& alf);

int main(int argc, char** argv) {
	

	int process_Rank;
	int size_Of_Cluster;

	string alf = "qazwsxedcrfvtgbyhnujmikolp123456789";
	int COUNT_SYMBOLS = 10000;
	int thread_id;
	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &thread_id);
	MPI_Comm_rank(MPI_COMM_WORLD, &process_Rank);

	
	
	
	
	vector<string> pr_str(thread_id, string());
	int str_size = COUNT_SYMBOLS / thread_id;
	vector<string> De_LZW_vec(thread_id, string());
	vector<vector<string>> LZW_vec(thread_id, vector<string>());
	string in_string;
	if (process_Rank == 0) {
		cout << "THREADS COUNT: " << thread_id << '\n';
		pr_str[process_Rank] = Generation(alf, str_size);
		cout << "string was generation on rank " << process_Rank << '\n';
	}
	else 
	{
		int size1 = str_size / 3;
		int size2 = str_size / 3;
		int size3 = str_size - size2 - size1;

		string tmp1 = Generation(alf, size1);
		string tmp2 = Generation(alf, size2);
		string tmp3 = Generation(alf, size3);


		MPI_Send(&size1, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
		MPI_Send(&size2, 1, MPI_INT, 0, 1, MPI_COMM_WORLD);
		MPI_Send(&size3, 1, MPI_INT, 0, 2, MPI_COMM_WORLD);
		MPI_Send(tmp1.c_str(), size1, MPI_CHAR, 0, 3, MPI_COMM_WORLD);
		MPI_Send(tmp2.c_str(), size2, MPI_CHAR, 0, 4, MPI_COMM_WORLD);
		MPI_Send(tmp3.c_str(), size3, MPI_CHAR, 0, 5, MPI_COMM_WORLD);
		cout << "string was generation on rank " << process_Rank << '\n';
		
	}

	
	MPI_Barrier(MPI_COMM_WORLD);
	

	if (process_Rank == 0) {
		for (int i = 1; i < thread_id; i++)
		{
			int size1;
			int size2;
			int size3;
			MPI_Recv(&size1, 1, MPI_INT, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			MPI_Recv(&size2, 1, MPI_INT, i, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			MPI_Recv(&size3, 1, MPI_INT, i, 2, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

			vector<char> message_It1(size1);
			vector<char> message_It2(size2);
			vector<char> message_It3(size3);

			MPI_Recv(message_It1.data(), size1, MPI_CHAR, i, 3, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			MPI_Recv(message_It2.data(), size2, MPI_CHAR, i, 4, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			MPI_Recv(message_It3.data(), size3, MPI_CHAR, i, 5, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

			string str1(message_It1.begin(), message_It1.end());
			string str2(message_It2.begin(), message_It2.end());
			string str3(message_It3.begin(), message_It3.end());
			pr_str[i] = str1 + str2 + str3;
			
		}
		
	}
	else
	{
		int i;
	}



	MPI_Barrier(MPI_COMM_WORLD);
	
	if (process_Rank == 0) {
		for (int i = 1; i < thread_id; i++) 
		{
			int size = pr_str[i].size();

			
			MPI_Send(&size, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
			MPI_Send(pr_str[i].c_str(), size, MPI_CHAR, i, 1, MPI_COMM_WORLD);
			
		}
	
		
		LZW_vec[process_Rank] = (LZW(pr_str[0], "qazwsxedcrfvtgbyhnujmikolp123456789"));
		cout << "Part " << process_Rank << " was encoded" << '\n';
	}
	else 
	{
		int size;

		MPI_Recv(&size, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		
		vector<char> message_It(size);
		
		MPI_Recv(message_It.data(), size, MPI_CHAR, 0, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

		

		string str(message_It.begin(), message_It.end());
		
		vector<string> tmp;
		tmp = (LZW(str, "qazwsxedcrfvtgbyhnujmikolp123456789"));
		
	
		string str_to_send;
		for (int i = 0; i < tmp.size(); i++)
		{
			str_to_send += tmp[i] + "/";
		}
		
		

		int str_to_send_size = str_to_send.size();
		int size1 = str_to_send_size / 3;
		int size2 = str_to_send_size / 3;
		int size3 = str_to_send_size - size2 - size1;

		string str1(str_to_send.begin(), str_to_send.begin() + size1);
		string str2(str_to_send.begin() + size1, str_to_send.begin() + size1 + size2);
		string str3(str_to_send.begin() + size1 + size2, str_to_send.end());

		MPI_Send(&size1, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
		MPI_Send(&size2, 1, MPI_INT, 0, 1, MPI_COMM_WORLD);
		MPI_Send(&size3, 1, MPI_INT, 0, 2, MPI_COMM_WORLD);
		
		MPI_Send(str1.c_str(), size1, MPI_CHAR, 0, 3, MPI_COMM_WORLD);
		MPI_Send(str2.c_str(), size2, MPI_CHAR, 0, 4, MPI_COMM_WORLD);
		MPI_Send(str3.c_str(), size3, MPI_CHAR, 0, 5, MPI_COMM_WORLD);


		cout << "Part " << process_Rank << " was encoded" << '\n';
	}
	
	
	MPI_Barrier(MPI_COMM_WORLD);
	

	if (process_Rank == 0) {
		cout << "Encoding was successful" << '\n';
		
		for (int i = 1; i < thread_id; i++)
		{
			int size1;
			int size2;
			int size3;

			MPI_Recv(&size1, 1, MPI_INT, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			MPI_Recv(&size2, 1, MPI_INT, i, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			MPI_Recv(&size3, 1, MPI_INT, i, 2, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

			vector<char> message_It1(size1);
			vector<char> message_It2(size2);
			vector<char> message_It3(size3);

			MPI_Recv(message_It1.data(), size1, MPI_CHAR, i, 3, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			MPI_Recv(message_It2.data(), size2, MPI_CHAR, i, 4, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			MPI_Recv(message_It3.data(), size3, MPI_CHAR, i, 5, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

			string str1(message_It1.begin(), message_It1.end());
			string str2(message_It2.begin(), message_It2.end());
			string str3(message_It3.begin(), message_It3.end());

			string tmp = str1 + str2 + str3;
			string vec_tmp;

			for (int j = 0; j < tmp.size(); j++)
			{
				if (tmp[j] == '/')
				{
					LZW_vec[i].push_back(vec_tmp);
					vec_tmp.clear();
				}
				else
				{
					vec_tmp += tmp[j];
				}
			}
			
		}
		
	}
	else
	{
		int i;
	}
	
	MPI_Barrier(MPI_COMM_WORLD);
	

	


	if (process_Rank == 0) {

		for (int i = 1; i < thread_id; i++) {
			string message_Item1;

			for (int j = 0; j < LZW_vec[i].size(); j++)
			{
				message_Item1 += LZW_vec[i][j] + "/";

			}
		

			int size1 = message_Item1.size();


			MPI_Send(&size1, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
			MPI_Send(message_Item1.c_str(), size1, MPI_CHAR, i, 1, MPI_COMM_WORLD);
			
		}
		De_LZW_vec[process_Rank] = (De_LZW(LZW_vec[process_Rank], "qazwsxedcrfvtgbyhnujmikolp123456789"));
		cout << "Part " << process_Rank << " was decoded" << '\n';
	}
	else 
	{
		
		int size;

		MPI_Recv(&size, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		vector<char> message_It(size);
		MPI_Recv(message_It.data(), size, MPI_CHAR, 0, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

		vector<string> tmp;
		string tmp_str(message_It.begin(), message_It.end());
		
		string vec_tmp;

		for (int i = 0; i < tmp_str.size(); i++)
		{
			if (tmp_str[i] == '/')
			{
				tmp.push_back(vec_tmp);
				vec_tmp.clear();
			}
			else
			{
				vec_tmp += tmp_str[i];
			}
		}

		
		string str = (De_LZW(tmp, "qazwsxedcrfvtgbyhnujmikolp123456789"));
		
		
		int str_size = str.size();
		


		int size1 = str_size / 3;
		int size2 = str_size / 3;
		int size3 = str_size - size2 - size1;

		string str1(str.begin(), str.begin() + size1);
		string str2(str.begin() + size1, str.begin() + size1 + size2);
		string str3(str.begin() + size1 + size2, str.end());



		MPI_Send(&size1, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
		MPI_Send(&size2, 1, MPI_INT, 0, 1, MPI_COMM_WORLD);
		MPI_Send(&size3, 1, MPI_INT, 0, 2, MPI_COMM_WORLD);

		MPI_Send(str1.c_str(), size1, MPI_CHAR, 0, 3, MPI_COMM_WORLD);
		MPI_Send(str2.c_str(), size2, MPI_CHAR, 0, 4, MPI_COMM_WORLD);
		MPI_Send(str3.c_str(), size3, MPI_CHAR, 0, 5, MPI_COMM_WORLD);

		cout << "Part " << process_Rank << " was decoded" << '\n';
	}
	


	
	MPI_Barrier(MPI_COMM_WORLD);
	
	if (process_Rank == 0) {
		
		cout << "Decoding was successful" << '\n';
		for (int i = 1; i < thread_id; i++) {
			int size1;
			int size2;
			int size3;

			MPI_Recv(&size1, 1, MPI_INT, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			MPI_Recv(&size2, 1, MPI_INT, i, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			MPI_Recv(&size3, 1, MPI_INT, i, 2, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

			vector<char> message_It1(size1);
			vector<char> message_It2(size2);
			vector<char> message_It3(size3);

			MPI_Recv(message_It1.data(), size1, MPI_CHAR, i, 3, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			MPI_Recv(message_It2.data(), size2, MPI_CHAR, i, 4, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			MPI_Recv(message_It3.data(), size3, MPI_CHAR, i, 5, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

			string str1(message_It1.begin(), message_It1.end());
			string str2(message_It2.begin(), message_It2.end());
			string str3(message_It3.begin(), message_It3.end());

			De_LZW_vec[i] = str1 + str2 + str3;
		}
	}
	else
	{
		int i;
	}

	MPI_Barrier(MPI_COMM_WORLD);

	if (process_Rank == 0) {
		string res;
		for (string x : De_LZW_vec)
		{
			res += x;
		}

		for (string x : pr_str)
		{
			in_string += x;
		}
		cout << (in_string == res) << '\n';
	}


	MPI_Finalize();
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
	string gen_str = "";

	for (int i = 0; i < count; i++)
	{
		gen_str += base[rand() % base.length()];
	}

	return gen_str;
}