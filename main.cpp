#include <iostream>
#include <vector>
#include <algorithm>
#include <limits>

int main()
{
	int rows, columns;
	std::cin >> rows;
	std::cin >> columns;

	std::vector<std::vector<int>> cost_table(rows, std::vector<int>(columns));
	std::vector<std::vector<bool>> striked(rows, std::vector<bool>(columns));

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

	// Cari cost terendah
	int lowest = std::numeric_limits<int>::max();
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
	std::vector<std::pair<int, int>> possibles;
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
	int highest_alloc = 0;
	int row, column, curr;
	for (const auto &x : possibles)
	{
		curr = std::min(supply[x.first], demand[x.second]);
		std::cout << "(" << x.first << ", " << x.second << ") dengan alokasi " << curr << '\n';
		if (curr > highest_alloc)
		{
			highest_alloc = curr;
			row = x.first;
			column = x.second;
		}
	}
	std::cout << "yang dipilih (" << row << ", " << column << ") dengan alokasi " << highest_alloc << '\n';

	// Kurangi supply dan demand
	int minimum = std::min(supply[row], demand[column]);
	supply[row] -= minimum;
	demand[column] -= minimum;

	// Coret jika sudah 0
	if (supply[row] == 0)
		for (auto it = striked.at(row).begin(); it != striked.at(row).end(); it++)
			*it = true;

	if (demand[column] == 0)
		for (auto it = striked.begin(); it != striked.end(); it++)
			it->at(column) = true;
	
	return 0;
}