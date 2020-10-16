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
	std::vector<std::vector<bool>> &striked,
	std::vector<int> &supply,
	std::vector<int> &demand,
	std::ofstream &out,
	int &counter
);

int main()
{
	// PRNG
	std::random_device rd;
	std::mt19937_64 mt(rd());
	
	const std::string soal = ".\\soal";
	struct dirent *entry;
	DIR *dp = opendir(soal.c_str());
	while ((entry = readdir(dp)))
	{
		int counter = 0;
		auto begin = std::chrono::steady_clock::now();
		std::string nama(entry->d_name);
		if (nama == "." || nama == "..")
			continue;
		std::ifstream inp(soal + "\\" + nama);
		std::ofstream out(".\\jawab\\" + nama);

		// Baris = supply, kolom = demand
		int rows, columns;
		inp >> rows;
		inp >> columns;
		
		std::vector<std::vector<int>> cost_table(rows, std::vector<int>(columns));
		std::vector<std::vector<bool>> striked(rows, std::vector<bool>(columns));
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

		print_table(cost_table, striked, supply, demand, out, counter);
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
			std::pair<int, int> chosen;
			if (possibles.size() > 1)
			{
				std::uniform_int_distribution<> distrib(0, possibles.size() - 1);
				chosen = possibles.at(distrib(mt));			
			}
			else
			{
				chosen = possibles.at(0);
			}
			int allocation = std::min(supply.at(chosen.first), demand.at(chosen.second));
			int row = chosen.first;
			int column = chosen.second;

			// Kurangi supply dan demand
			supply.at(row) -= allocation;
			demand.at(column) -= allocation;

			// Coret jika sudah 0
			if (supply.at(row) == 0)
				for (auto it = striked.at(row).begin(); it != striked.at(row).end(); it++)
					*it = true;

			if (demand.at(column) == 0)
				for (auto it = striked.begin(); it != striked.end(); it++)
					it->at(column) = true;

			// Masukkan ke penyelesaian
			answer.push_back(std::make_pair(cost_table.at(row).at(column), allocation));
			print_table(cost_table, striked, supply, demand, out, ++counter);
		}

		int total = 0;
		for (const std::pair<int, int> &i : answer)
			total += (i.first * i.second);
		out << "Solusi: " << total << '\n';
		answer.clear();

		auto end = std::chrono::steady_clock::now();
		auto elapsed = end - begin;
		out << "Runtime: " << std::chrono::duration<float, std::milli>(elapsed).count() << "ms\n";
		std::cout << "Runtime: " << std::chrono::duration<float, std::milli>(elapsed).count() << "ms\n";
	}
	closedir(dp);

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
	std::vector<std::vector<bool>> &striked,
	std::vector<int> &supply,
	std::vector<int> &demand,
	std::ofstream &out,
	int &counter
){
	if (counter > 0)
		out << "Iterasi ke-" << counter << '\n';
	for (auto i = 0; i < cost_table.size(); i++)
	{
		for (auto j = 0; j < cost_table.at(i).size(); j++)
		{
			if (striked.at(i).at(j))
				out << "x\t";
			else
				out << cost_table.at(i).at(j) << '\t';
		}
		out << supply.at(i) << '\n';
	}

	for (auto it = demand.begin(); it != demand.end(); it++)
		out << *it << '\t';
	out << "\n\n";
}