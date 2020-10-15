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
	int row, column;
	int lowest = std::numeric_limits<int>::max();
	for (size_t i = 0; i < cost_table.size(); i++)
	{
		for (size_t j = 0; j < cost_table.at(i).size(); j++)
		{
			if (!striked.at(i).at(j) && cost_table.at(i).at(j) < lowest)
			{
				lowest = cost_table.at(i).at(j);
				row = i;
				column = j;
			}
		}
	}
	std::cout << lowest << " di (" << row << ", " << column << ")\n";
	
	return 0;
}