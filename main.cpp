#include <iostream>
#include <vector>
#include <algorithm>
#include <limits>

bool finished(std::vector<std::vector<bool>> &striked);
void print_table(std::vector<std::vector<int>> &cost_table, std::vector<std::vector<bool>> &striked);

int main()
{
	int rows, columns;
	std::cin >> rows;
	std::cin >> columns;

	std::vector<std::vector<int>> cost_table(rows, std::vector<int>(columns));
	std::vector<std::vector<bool>> striked(rows, std::vector<bool>(columns));
	int lowest;
	std::vector<std::pair<int, int>> possibles;
	int highest_alloc, row, column, curr;
	std::vector<std::pair<int, int>> answer;

	// Isi tabel dengan cost
	for (auto &row : cost_table)
		for (auto &column : row)
			std::cin >> column;

	std::vector<int> supply(rows);
	std::vector<int> demand(columns);

	// Isi supply dan demand
	for (auto &x : supply)
		std::cin >> x;
	for (auto &x : demand)
		std::cin >> x;

	// print_table(cost_table, striked);
	while (!finished(striked))
	{
		// Cari cost terendah
		lowest = std::numeric_limits<int>::max();
		for (size_t i = 0; i < cost_table.size(); i++)
		{
			for (size_t j = 0; j < cost_table.at(i).size(); j++)
			{
				if (!striked.at(i).at(j) && cost_table.at(i).at(j) <= lowest)
				{
					lowest = cost_table.at(i).at(j);
				}
			}
		}

		// Cari index cost terendah
		possibles.clear();
		for (size_t i = 0; i < cost_table.size(); i++)
		{
			for (size_t j = 0; j < cost_table.at(i).size(); j++)
			{
				if (!striked.at(i).at(j) && cost_table.at(i).at(j) == lowest)
				{
					possibles.push_back(std::make_pair(i, j));
				}
			}
		}

		// Test masing-masing untuk alokasi tertinggi
		highest_alloc = std::numeric_limits<int>::max();
		for (const auto &x : possibles)
		{
			curr = std::min(supply[x.first], demand[x.second]);
			// std::cout << "(" << x.first << ", " << x.second << ") dengan alokasi " << curr << '\n';
			if (curr * cost_table.at(x.first).at(x.second) < highest_alloc)
			{
				highest_alloc = curr;
				row = x.first;
				column = x.second;
			}
		}
		// std::cout << "yang dipilih (" << row << ", " << column << ") dengan alokasi " << highest_alloc << '\n';

		// Kurangi supply dan demand
		supply[row] -= highest_alloc;
		demand[column] -= highest_alloc;

		// Coret jika sudah 0
		if (supply[row] == 0)
			for (auto it = striked.at(row).begin(); it != striked.at(row).end(); it++)
				*it = true;

		if (demand[column] == 0)
			for (auto it = striked.begin(); it != striked.end(); it++)
				it->at(column) = true;

		// Masukkan ke penyelesaian
		answer.push_back(std::make_pair(cost_table.at(row).at(column), highest_alloc));
		// print_table(cost_table, striked);
	}

	int total = 0;
	for (const std::pair<int, int> &i : answer)
	{
		total += (i.first * i.second);
	}
	std::cout << total << '\n';
	
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

void print_table(std::vector<std::vector<int>> &cost_table, std::vector<std::vector<bool>> &striked)
{
	for (auto i = 0; i < cost_table.size(); i++)
	{
		for (auto j = 0; j < cost_table.at(i).size(); j++)
		{
			if (striked.at(i).at(j))
				std::cout << "x\t";
			else
				std::cout << cost_table.at(i).at(j) << '\t';
		}
		std::cout << '\n';
	}
	std::cout << '\n';
}