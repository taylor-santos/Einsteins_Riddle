#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <regex>
#include <algorithm>
#include <math.h>
#include <assert.h>

using namespace std;

#define VERT_BAR (char)179
#define HORIZ_BAR (char)196
#define UP_RIGHT_BAR (char)192
#define UP_LEFT_BAR (char)217
#define DOWN_RIGHT_BAR (char)218
#define DOWN_LEFT_BAR (char)191
#define UP_HORIZ_CROSS (char)193
#define DOWN_HORIZ_CROSS (char)194
#define VERT_RIGHT_CROSS (char)195
#define VERT_LEFT_CROSS (char)180
#define CROSS (char)197

int num_digits(int n) {
	if (n == 0) {
		return 1;
	}else if (n < 0) {
		n = (-n) * 10;
	}
	return int(log10(n))+1;
}

int get_category(int index, int item_count) {
	return (int)(index / item_count);
}

int get_item(int index, int item_count){
	return (int)(index % item_count);
}

int evaluate_correlation(vector<vector<int>> correlations, vector<vector<int>> contradictions, int item_count, int index1, int index2, vector<int> chain) {
	if (get_category(index1, item_count) == get_category(index2, item_count)) {
		return index1 == index2 ? 1 : -1;
	}
	for (int i = 0; i < contradictions[index1].size(); ++i) {
		if (index2 == contradictions[index1][i])
			return -1;
	}
	for (int i = 0; i < contradictions[index2].size(); ++i) {
		if (index1 == contradictions[index2][i])
			return -1;
	}
	for (int i = 0; i < correlations[index1].size(); ++i) {
		int cor1 = correlations[index1][i];
		if (find(chain.begin(), chain.end(), cor1) == chain.end()) {
			chain.push_back(cor1);
			int val = evaluate_correlation(correlations, contradictions, item_count, index2, cor1,chain);
			if (val != 0)
				return val;
		}
	}
	for (int i = 0; i < correlations[index2].size(); ++i) {
		int cor1 = correlations[index2][i];
		if (find(chain.begin(), chain.end(), cor1) == chain.end()) {
			chain.push_back(cor1);
			int val = evaluate_correlation(correlations, contradictions, item_count, cor1, index1, chain);
			if (val != 0)
				return val;
		}
	}
	return 0;
}

void print_grid(vector<vector<string>> items, vector<vector<int>> correlations, vector<vector<int>> contradictions) {
	int cat_count = items.size();
	int item_count = items[0].size();
	int max_vertical_element_name_length = 0;
	int max_horizontal_element_name_length = 0;
	for (int i = 0; i < cat_count; ++i) {
		/*Find the longest label+id for the vertical and horizontal labels*/
		for (int j = 0; j < item_count; ++j) { 
			if (i<cat_count-1)	//Vertical categories do not include the last category
				max_vertical_element_name_length = max(max_vertical_element_name_length, (int)items[i][j].size() + num_digits(cat_count*i+j+1) + 1); //Length of item label + length of item id + space
			if (i!=0)	//Horizontal categories do not include the first category
				max_horizontal_element_name_length = max(max_horizontal_element_name_length, (int)items[i][j].size() + num_digits(cat_count*i + j + 1) + 1); //Length of item label + length of item id + space
		}
	}
	vector<vector<char>> labels = vector<vector<char>>(cat_count*(cat_count - 1));
	for (int i = 0; i < cat_count - 1; ++i) {
		for (int j = 0; j < item_count; ++j) {
			labels[cat_count*i + j] = vector<char>(max_vertical_element_name_length);
			for (int k = 0; k < items[i][j].length(); ++k) {
				labels[cat_count*i + j][max_vertical_element_name_length - items[i][j].length() + k] = items[i][j][k];
			}
			int id = item_count*i + j + 1;
			int digit_diff = num_digits(cat_count*item_count) - num_digits(id);
			for (int k = 0; k < num_digits(id); ++k) {
				labels[cat_count*i + j][digit_diff+k] = to_string(id)[k];
			}
			labels[cat_count*i + j][num_digits(cat_count*item_count)] = HORIZ_BAR;
		}
	}
	for (int i = 0; i < max_vertical_element_name_length; ++i) {
		cout << string(max_horizontal_element_name_length, ' ');
		cout << VERT_BAR << " ";
		for (int j = 0; j < cat_count - 1; ++j) {
			for (int k = 0; k < item_count; ++k) {
				if (labels[cat_count*j + k][i] != 0)
					cout << labels[cat_count*j + k][i];
				else
					cout << " ";
				cout << " ";
			}
			cout << VERT_BAR << " ";
		}
		cout << endl;
	}
	cout << string(max_horizontal_element_name_length, HORIZ_BAR) << CROSS;
	for (int j = 0; j < cat_count-1; ++j) {
		cout << string(2 * item_count + 1, HORIZ_BAR) << (j<cat_count-2 ? CROSS : VERT_LEFT_CROSS);
	}
	cout << endl;
	for (int i = 0; i < cat_count - 1; ++i) {
		for (int j = 0; j < item_count; ++j) {
			string label = items[items.size() - i - 1][j];
			int id = item_count*(items.size() - i - 1) + j + 1;
			cout << string(num_digits(cat_count*item_count)-num_digits(id), ' ') << id << VERT_BAR << string(max_horizontal_element_name_length - label.size() - num_digits(cat_count*item_count) - 1, ' ') << label << VERT_BAR << " ";
			for (int k = 0; k < cat_count - i - 1; ++k) {
				for (int l = 0; l < item_count; ++l) {
					int id2 = item_count*k + l + 1;
					int val = evaluate_correlation(correlations, contradictions, item_count, id2-1, id-1, vector<int>());
					if (val == 2)
						cout << CROSS;
					else if (val == 1)
						cout << "+";
					else if (val == -1)
						cout << "-";
					else
						cout << " ";
					cout << " ";
				}
				cout << VERT_BAR << " ";
			}
			cout << endl;
		}
		cout << string(max_horizontal_element_name_length, HORIZ_BAR) << (i < cat_count-2 ? CROSS : UP_HORIZ_CROSS);
		for (int j = 0; j < cat_count-i-1; ++j) {
			cout << string(2 * item_count + 1, HORIZ_BAR) << (j<cat_count-i-2 ? CROSS : UP_LEFT_BAR);
		}
		cout << endl;
	}
}

int main() {
	cout << "Number of categories: ";
	int cat_count = 0;
	while (!(cin >> cat_count) || cat_count < 2) {
		cin.clear();
		cin.ignore(numeric_limits<streamsize>::max(), '\n');
		cout << "Invalid input (Count must be at least 2). Try again: ";
	}
	cout << "Number of items per category: ";
	int item_count = 0;
	while (!(cin >> item_count) || item_count < 2) {
		cin.clear();
		cin.ignore(numeric_limits<streamsize>::max(), '\n');
		cout << "Invalid input (Count must be at least 2).  Try again: ";
	}
	cin.ignore();
	vector<string> categories = vector<string>(cat_count);
	vector<vector<string>> items = vector<vector<string>>(cat_count);
	cout << "Category names:" << endl;
	for (int i = 0; i < cat_count; ++i) {
		cout << i+1 << ") ";
		string input;
		getline(cin, input);
		categories[i] = input;
	}
	cout << "Category elements:" << endl;
	for (int i = 0; i < cat_count; ++i) {
		cout << "Enter " << item_count << " comma-separated names for the " << categories[i] << " items:";
		vector<string> elements;
		while (elements.size() != item_count) {
			cout << endl << "> ";
			elements.clear();
			string input;
			getline(cin, input);
			int index = input.find(',');
			while (index != string::npos){
				string substr = input.substr(0, index);
				substr = regex_replace(substr, regex("^ +| +$|( ) +"), "$1");	//Remove leading, trailing, and extra whitespace (http://stackoverflow.com/a/21815483)
				elements.push_back(substr);
				input = input.substr(index+1, input.size());
				index = input.find(',');
			} 
			input = regex_replace(input, regex("^ +| +$|( ) +"), "$1");
			elements.push_back(input);
		}
		items[i] = elements;
	}
	vector<vector<int>> correlations = vector<vector<int>>(cat_count*item_count);
	vector<vector<int>> contradictions = vector<vector<int>>(cat_count*item_count);

	while (1) {
		find_correlations:
		for (int i = 0; i < cat_count - 1; ++i) {
			for (int k = 0; k < cat_count - i - 1; ++k) {
				vector<vector<int>> rows = vector<vector<int>>(item_count);
				vector<vector<int>> cols = vector<vector<int>>(item_count);
				for (int j = 0; j < item_count; ++j) {
					rows[j] = vector<int>(item_count);
					cols[j] = vector<int>(item_count);
				}
				for (int j = 0; j < item_count; ++j) {
					int id = item_count*(items.size() - i - 1) + j + 1;
					for (int l = 0; l < item_count; ++l) {
						int id2 = item_count*k + l + 1;
						int val = evaluate_correlation(correlations, contradictions, item_count, id2 - 1, id - 1, vector<int>());
						rows[j][l] = val;
						cols[l][j] = val;
					}
				}
				for (int x = 0; x < item_count; ++x) {
					int rows_cor_count = 0;
					int rows_cont_count = 0;
					int rows_first_empty = -1;
					int cols_cor_count = 0;
					int cols_cont_count = 0;
					int cols_first_empty = -1;
					for (int y = 0; y < item_count; ++y) {
						if (rows[x][y] == 1)
							rows_cor_count++;
						else if (rows[x][y] == -1)
							rows_cont_count++;
						else if (rows_first_empty == -1) {
							rows_first_empty = y;
						}
						if (cols[x][y] == 1)
							cols_cor_count++;
						else if (cols[x][y] == -1)
							cols_cont_count++;
						else if (cols_first_empty == -1) {
							cols_first_empty = y;
						}
					}
					if (rows_cor_count == 0 && rows_cont_count == item_count - 1) {
						int index1 = item_count*(items.size() - i - 1) + x;
						int index2 = item_count*k + rows_first_empty;
						correlations[index1].push_back(index2);
						correlations[index2].push_back(index1);
						cout << "FOUND AT (" << index1+1 << "," << index2+1 << ")" << endl;
						goto find_correlations;
					}
					if (cols_cor_count == 0 && cols_cont_count == item_count - 1) {
						int index1 = item_count*(items.size() - i - 1) + cols_first_empty;
						int index2 = item_count*k + x;
						correlations[index1].push_back(index2);
						correlations[index2].push_back(index1);
						cout << "FOUND AT (" << index1 + 1 << "," << index2 + 1 << ")" << endl;
						goto find_correlations;
					}
				}
			}
		}
		print_grid(items, correlations, contradictions);
		cout << "Enter two comma-separated indices between 1 and " << cat_count*item_count << ", followed by either + or -:" << endl;
		vector<int> indices;
		int cat1, cat2, item1, item2, sign;
		while (indices.size() != 2 || cat1 == cat2) {
			cout << "> ";
			indices.clear();
			string input;
			getline(cin, input);
			input.erase(remove_if(input.begin(), input.end(), isspace), input.end());
			int index = input.find(',');
			while (index != string::npos) {
				string substr = input.substr(0, index);
				int val = atoi(substr.c_str());
				if (val > 0 && val <= cat_count*item_count) {
					indices.push_back(val-1);
				}
				input = input.substr(index + 1, input.size());
				index = input.find(',');
			}
			if (indices.size() != 2) {
				cout << "Invalid input. Try again:" << endl;
			}
			else {
				int s = -1;
				if (input == "+") {
					s = 1;
				}
				else if (input == "-") {
					s = 0;
				}
				else {
					cout << "Invalid input. Must end with a + or -. Try again:" << endl;
				}
				if (s != -1) {
					sign = s;
					cat1 = get_category(indices[0], item_count);
					cat2 = get_category(indices[1], item_count);
					item1 = get_item(indices[0], item_count);
					item2 = get_item(indices[1], item_count);
					cout << indices[0] << " " << indices[1] << " (" << cat1 << "," << item1 << ") (" << cat2 << "," << item2 << ")" << endl;
					if (cat1 == cat2) {
						cout << "Invalid input. " << items[cat1][item1] << " is in the same category as " << items[cat2][item2] << ". Try again:" << endl;
					}
				}
			}
		}
		if (sign == 1) {
			correlations[cat1*item_count + item1].push_back(cat2*item_count + item2);
			correlations[cat2*item_count + item2].push_back(cat1*item_count + item1);
		}
		else {
			contradictions[cat1*item_count + item1].push_back(cat2*item_count + item2);
			contradictions[cat2*item_count + item2].push_back(cat1*item_count + item1);
		}
	}
}