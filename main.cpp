#include <iostream>
#include <vector>
#include <algorithm>
#include <limits>
#include <random>
#include <string>
#include <fstream>
#include <chrono>
#include <dirent.h>

bool finished(std::vector<std::vector<bool>> &striked);
void print_table(
	std::vector<std::vector<int>> &cost_table,
	std::vector<int> &supply,
	std::vector<int> &demand,
	std::vector<int> &shallow_supply,
	std::vector<int> &shallow_demand,
	std::ofstream &out,
	int &counter,
	std::vector<std::pair<int, int>> &chosen,
	std::vector<int> &allocation
);

int main()
{
	// PRNG
	std::random_device rd;
	std::mt19937_64 mt(rd());
	
	const std::string soal = ".\\soal";
	struct dirent *entry;
	DIR *dp = opendir(soal.c_str());
	// std::cout << "Membaca folder soal. . .\n";
	while ((entry = readdir(dp)))
	{
		int counter = 0;
		auto begin = std::chrono::steady_clock::now();
		std::string nama(entry->d_name);
		if (nama == "." || nama == "..")
			continue;
		std::cout << "Membaca soal " << nama << '\n';
		std::ifstream inp(soal + "\\" + nama);
		std::ofstream out(".\\jawab\\" + nama);

		// Baris = supply, kolom = demand
		int rows, columns;
		inp >> rows;
		inp >> columns;
		
		std::vector<std::vector<int>> cost_table(rows, std::vector<int>(columns, 0));
		std::vector<std::vector<bool>> striked(rows, std::vector<bool>(columns, 0));
		std::vector<std::pair<int, int>> answer(0);

		// Isi tabel dengan cost
		for (auto &row : cost_table)
			for (auto &column : row)
				inp >> column;

		std::vector<int> supply(rows);
		std::vector<int> demand(columns);

		// Isi supply dan demand
		for (auto &x : supply)
			inp >> x;
		for (auto &x : demand)
			inp >> x;
		std::vector<int> shallow_supply(supply);
		std::vector<int> shallow_demand(demand);

		out << "Tabel awal\n";
		for (int i = 0; i < cost_table.size(); i++)
		{
			for (int j = 0; j < cost_table.at(i).size(); j++)
			{
				out << cost_table.at(i).at(j) << '\t';
			}
			out << supply.at(i) << '\n';
		}

		for (auto it = demand.begin(); it != demand.end(); it++)
			out << *it << '\t';
		out << "\n\n";

		std::vector<std::pair<int, int>> chosen(0);
		std::vector<int> allocation(0);
		while (!finished(striked))
		{
			// Cari cost terendah
			int lowest = std::numeric_limits<int>::max();
			for (size_t i = 0; i < cost_table.size(); i++)
				for (size_t j = 0; j < cost_table.at(i).size(); j++)
					if (!striked.at(i).at(j) && cost_table.at(i).at(j) <= lowest)
						lowest = cost_table.at(i).at(j);

			// Cari index cost terendah
			std::vector<std::pair<int, int>> possibles(0);
			for (size_t i = 0; i < cost_table.size(); i++)
				for (size_t j = 0; j < cost_table.at(i).size(); j++)
					if (!striked.at(i).at(j) && cost_table.at(i).at(j) == lowest)
						possibles.push_back(std::make_pair(i, j));

			// Jika cost minimum tidak unik, pilih secara random mana yang dipakai
			if (possibles.size() > 1)
			{
				std::uniform_int_distribution<> distrib(0, possibles.size() - 1);
				chosen.push_back(possibles.at(distrib(mt)));
			}
			else
			{
				chosen.push_back(possibles.at(0));
			}
			allocation.push_back(std::min(supply.at(chosen.at(counter).first), demand.at(chosen.at(counter).second)));
			int row = chosen.at(counter).first;
			int column = chosen.at(counter).second;

			// Kurangi supply dan demand
			supply.at(row) -= allocation.at(counter);
			demand.at(column) -= allocation.at(counter);

			// Coret jika sudah 0
			if (supply.at(row) == 0)
				for (auto it = striked.at(row).begin(); it != striked.at(row).end(); it++)
					*it = true;

			if (demand.at(column) == 0)
				for (auto it = striked.begin(); it != striked.end(); it++)
					it->at(column) = true;

			// Masukkan ke penyelesaian
			answer.push_back(std::make_pair(cost_table.at(row).at(column), allocation.at(counter)));
			print_table(cost_table, supply, demand, shallow_supply, shallow_demand, out, ++counter, chosen, allocation);
		}

		int total = 0;
		for (const std::pair<int, int> &i : answer)
			total += (i.first * i.second);
		out << "Solusi: " << total << '\n';
		// std::cout << total << '\n';

		auto end = std::chrono::steady_clock::now();
		auto elapsed = end - begin;
		out << "Runtime: " << std::chrono::duration<float, std::milli>(elapsed).count() << "ms\n";
	}
	closedir(dp);
	std::cout << "Jawaban per soal ada di folder 'jawab'";

	return 0;
}

bool finished(std::vector<std::vector<bool>> &striked)
{
	// Jika sudah distrike semua, berarti sudah selesai
	for (auto i = striked.begin(); i != striked.end(); i++)
		for (auto j = i->begin(); j != i->end(); j++)
			if (*j == false)
				return false;
	return true;
}

void print_table(
	std::vector<std::vector<int>> &cost_table,
	std::vector<int> &supply,
	std::vector<int> &demand,
	std::vector<int> &shallow_supply,
	std::vector<int> &shallow_demand,
	std::ofstream &out,
	int &counter,
	std::vector<std::pair<int, int>> &chosen,
	std::vector<int> &allocation
){
	out << "Iterasi ke-" << counter << '\n';
	for (int i = 0; i < cost_table.size(); i++)
	{
		for (int j = 0; j < cost_table.at(i).size(); j++)
		{
			auto it = std::find(chosen.begin(), chosen.end(), std::pair<int, int>(i, j));
			if (it != chosen.end())
				out << '(' << cost_table.at(i).at(j) << "," << allocation.at(it - chosen.begin()) << ")\t";
			else
				out << cost_table.at(i).at(j) << '\t';
		}
		out << shallow_supply.at(i) << ' ';
		bool in_chosen = false;
		for (auto it = chosen.begin(); it != chosen.end(); it++)
		{
			if (it->first == i)
			{
				in_chosen = true;
				break;
			}
		}
		if (in_chosen)
		{
			out << "(" << supply.at(i) << ")";
		}
		out << '\n';
		
	}

	for (int i = 0; i < shallow_demand.size(); i++)
	{
		out << shallow_demand.at(i) << ' ';
		bool in_chosen = false;
		for (auto it = chosen.begin(); it != chosen.end(); it++)
		{
			if (it->second == i)
			{
				in_chosen = true;
				break;
			}
		}
		if (in_chosen)
		{
			out << "(" << demand.at(i) << ")\t";
		}
		else
		{
			out << '\t';
		}
		
	}
	out << "\n\n";
}