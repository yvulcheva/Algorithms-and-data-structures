//Yolina Valcheva
//Structures and algorithms course assignment FMI
//External sorting

#include <iostream>
#include <vector>
#include <fstream>
#include <algorithm>
#include <string>
#include <queue>

using namespace std;

void createFiles(size_t bufferSize, string fileToBeSortedName, int &numberOfFilesCreated)
{
	vector<int> vecWithNumbers;
	ifstream fileToBeSorted(fileToBeSortedName.c_str());

	if (fileToBeSorted.is_open())
	{
		//output file
		ofstream file;

		bool endOfBigFile = false;

		while (!endOfBigFile)
		{

			while (vecWithNumbers.size() < bufferSize)
			{
				int number;
				if (fileToBeSorted >> number)
				{
					vecWithNumbers.push_back(number);
				}
				else
				{
					endOfBigFile = true;
					break;
				}
			}

			//to avoid creating empty file
			if (vecWithNumbers.empty())
				break;

			//create file for output which name is F and the number of file that is being created
			file.open("F" + to_string(numberOfFilesCreated + 1) + ".txt");
			if (file.is_open())
			{
				//sort vector
				std::sort(vecWithNumbers.begin(), vecWithNumbers.end());

				//increase number of created files
				++numberOfFilesCreated;
				int size = vecWithNumbers.size();
				for (size_t i = 0; i < size; i++)
					file << vecWithNumbers[i] << ' ';

				file.close();

				vecWithNumbers.clear();
			}
			else
				perror("Error: not able to open file");
		}

		fileToBeSorted.close();
	}
	else
		perror("Error: not able to open file");
}

string _merge_K(int bufferSize, int indexOfName, vector<string> fileNames)
{
	ofstream mergedFile;
	string mergedName = "Merged" + to_string(indexOfName + 1) + ".txt";
	mergedFile.open(mergedName);

	if (mergedFile.is_open())
	{
		//storing every first element from input files and index of files
		priority_queue<pair<int, int>, vector<pair<int, int>>, greater<pair<int, int>>> buffer;

		//vector with all temp files
		std::vector<ifstream> file(bufferSize);

		for (int i = 0; i < bufferSize; i++)
		{
			file[i].open(fileNames[i]);
		}

		int num;
		for (int i = 0; i < bufferSize; i++)
		{
			file[i] >> num;
			//push the element and index of the location file
			buffer.push(make_pair(num, i));
		}

		while (!buffer.empty())
		{
			pair<int, int> temp = buffer.top();
			buffer.pop();
			mergedFile << temp.first << ' ';

			if (file[temp.second] >> num)
			{
				temp.first = num;
				buffer.push(temp);
			}
			else
			{
				file[temp.second].close();
				remove(fileNames[temp.second].c_str());
			}
		}

		mergedFile.close();
	}

	else
		perror("Error: not able to open file");

	return mergedName;
}

void merge(int filesCreated, int bufferSize, string nameOfOutputFile)
{
	//names of temp files
	vector<string> fileNames;
	for (int i = 0; i < filesCreated; i++)
		fileNames.push_back("F" + to_string(i + 1) + ".txt");

	//end index for each group of temp files
	int endIndex;

	/* if there is room in memory to store one block
	for each run, then all runs can be merged in a single pass*/

	endIndex = (filesCreated < bufferSize) ? filesCreated : bufferSize - 1;

	int nameIndex = 0;
	while (fileNames.size() > 1)
	{
		vector<string> filesToMerge(endIndex);
		for (int i = 0; i < endIndex; i++)
			filesToMerge[i] = fileNames[i];

		fileNames.erase(fileNames.begin(), fileNames.begin() + endIndex);

		fileNames.push_back(_merge_K(endIndex, nameIndex, filesToMerge));

		if (fileNames.size() <= endIndex)
			endIndex = fileNames.size();

		filesToMerge.clear();

		++nameIndex;
	}

	//renaming the last Merged file which is the final output file
	string oldName = "Merged" + to_string(nameIndex) + ".txt";
	if (filesCreated == 1)
		oldName = "F1.txt";

	string newName = nameOfOutputFile;

	if (rename(oldName.c_str(), newName.c_str()) == 0)
	{
		auto output = "Sorted file named: " + nameOfOutputFile;
		puts(output.c_str());
	}
	else
	{
		perror("Error");
		cout << "Sorted file named: " + oldName << "\n";
	}
}

void ExternalSort(int Buffer, string fileToBeSorted, string nameOfOutputFile)
{
	//counter of files we create depending on the buffer size
	int numberOfFilesCreated = 0;
	//buffer is equal to the amount of numbers we can store in one run
	Buffer = (Buffer * 1024 * 1024) / 4;
	createFiles(Buffer, fileToBeSorted, numberOfFilesCreated);

	merge(numberOfFilesCreated, Buffer, nameOfOutputFile);
}

int main()
{
	ios_base::sync_with_stdio(false);
	cin.tie(NULL);

	//in MB
	int Buffer = 3;

	string fileToBeSortedName = "ExternalSort1.txt";

	string nameOfOutputFile = "SortedFile1.txt";
	ExternalSort(Buffer, fileToBeSortedName, nameOfOutputFile);

	return 0;
}
