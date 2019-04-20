// FileWatcher.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#include <string>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <experimental/filesystem>
using namespace std;
using namespace std::experimental::filesystem;

int main()
{
	wstring rootpath(L"D:\\00\\");
	//wstringstream ss;
	vector<path> orgpathlist;
	vector<path> compathlist;
	int pdfnum = 0;
	int docnum = 0;
	for (auto itr : recursive_directory_iterator(rootpath))
	{
		if (!is_directory(itr.path()))
		{
			orgpathlist.push_back(itr.path());
			if (itr.path().extension() == ".pdf")
				pdfnum++;
			//cout << itr.path().stem() << " " << itr.path().extension()
			 //        <<" "<<itr.path().extension().compare(L".pdf")<<itr.path().stem()<<endl;
		}

	}
	cout << "Found " << pdfnum << " PDF documents." << endl;
	compathlist = orgpathlist;
	for (auto itr : orgpathlist)
	{
		for (auto it : compathlist)
		{
			if (itr == it)
			{
				itr.clear();
			}

		}
	}

}


