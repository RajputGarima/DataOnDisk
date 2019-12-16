#include<iostream>
#include<stdlib.h>
#include "file_manager.h"
#include "errors.h"
#include<cstring>
#include<bits/stdc++.h>
#include "constants.h"
using namespace std;
# define size_int sizeof(int)
FileManager fm;
int findSize(PageHandler);
void write_back(PageHandler, int*, int, FileHandler);
void read_page(int*, PageHandler,int);
void merge_runs(int, int , string);


class HeapNode{
	int element;
	int page_no;
	public:
	HeapNode(int val, int no){
		element = val;
		page_no =  no;
	}
	int getElement() const{
		return element;
	}
	int getPageNo() const{
		return page_no;
	}
};

class Comparator{
	public: 
    int operator() (const HeapNode& p1, const HeapNode& p2) 
    { 
        return p1.getElement() > p2.getElement(); 
    } 
};

int main(int argc, char** argv) {
	fm.ClearBuffer();
	string s = "R.txt";
	PageHandler p_first, p_last, ph;
    int i = 0, j = 0, *arr, num, n, runs = 1;
	
	int first_page, last_page, min;
	char * data;
	char* input_file = argv[1];
	char* sorted_file = argv[2];
	FileHandler fh = fm.OpenFile(input_file);
	cout << "File created " << endl;
	//PageHandler phr = fhr.NewPage();
	PageHandler phr;
	int value = 1;


	p_first = fh.FirstPage();
	p_last = fh.LastPage();

	cout << "First page" << p_first.GetPageNum();
	cout << "Last Page" << p_last.GetPageNum();
	first_page = p_first.GetPageNum();
	last_page = p_last.GetPageNum();

	cout << "First Page" << p_first.GetPageNum() << endl;
	cout << "Last Page" << p_last.GetPageNum() << endl;
	fh.UnpinPage(p_first.GetPageNum());
	fh.UnpinPage(p_last.GetPageNum());
	while ( first_page <= last_page)
	{
		string s2 = std::to_string(runs) + s;
		cout<< s2 << endl;
		FileHandler fhr = fm.CreateFile(s2.c_str());
		phr = fhr.NewPage();
		cout << "In While Loop" << endl;
		int min = first_page + BUFFER_SIZE - 2 < last_page ? first_page + BUFFER_SIZE - 2 : last_page;
		cout << "Value of min" << min << endl;
		PageHandler * ph = new PageHandler[min + 1 - (runs-1)*(BUFFER_SIZE - 1)];
		int * count = new int[min + 1 - (runs-1)*(BUFFER_SIZE - 1)];
		char ** data = new char*[min + 1 - (runs-1)*(BUFFER_SIZE - 1)];
		int i = first_page, j= 0;
		cout << "i start = " << i << endl;
		while(i<= min){
			ph[j] = fh.PageAt(i);
			count[j] = findSize(ph[j]);
			data[j] = ph[j].GetData();
			cout << "Count in each page " << count[j] << endl; 
			i++;
			j++;
		}
		//cout<<"here";
		i = 0;
		int limit = min - (runs - 1)*(BUFFER_SIZE - 1);
		while(i <= limit){
			int * arr = new int[count[i]];
			read_page(arr, ph[i],count[i]);
			sort(arr, arr + count[i]);
			write_back(ph[i], arr, count[i], fh);
			delete arr;
			i++;
		}
		
	
		// priority queue 
		priority_queue <HeapNode, vector<HeapNode>, Comparator > pq;
		// first element from all pages
		
		i = 0;
		int y;
		while(i <= limit){
			memcpy(&y, &data[i][0], size_int);
			pq.push(HeapNode(y, i));
			i++;
		}
		// cout<<"--------------------------------------------------------"<<endl;
		// while (pq.empty() == false) 
    // 	{ 
		// 	HeapNode p = pq.top(); 
		// 	cout << "(" << p.getElement() << ", " << p.getPageNo() << ")"; 
		// 	cout << endl;
		// 	pq.pop();
		// }
		// cout <<"---------------------------------------------------------"<< endl;
		int * size = new int[min + 1 - (runs-1)*(BUFFER_SIZE - 1)];
		for(i = 0; i<=limit; i++)
			size[i] = 0;
		
		char * data_ptr = phr.GetData();
		cout<<"******************************* "<<phr.GetPageNum()<<endl;
		int run_offset = 0;
		bool flag = true, checkLast=false;
		while (pq.empty() == false) 
    	{ 
				HeapNode p = pq.top(); 
				//cout << "(" << p.getElement() << ", " << p.getPageNo() << ")"; 
				//cout << endl; 
				int element = p.getElement();
				int pg = p.getPageNo();
			
				pq.pop(); 
				size[pg]+= 4;
				if( size[pg]/4 < count[pg])
				{
					int z;
					memcpy(&z, &data[pg][size[pg]], size_int);
					pq.push(HeapNode(z, pg));
				}	
					memcpy(&data_ptr[run_offset], &element, size_int);
					run_offset+= 4;
					if(run_offset == PAGE_CONTENT_SIZE - size_int && pq.empty()==false){
						int p = INT_MIN;
						cout<<"heap "<<" new page"<< phr.GetPageNum()<<endl;
						memcpy(&data_ptr[run_offset], &p, size_int);
					fhr.UnpinPage(phr.GetPageNum());
					phr = fhr.NewPage();
					data_ptr = phr.GetData();
					run_offset = 0;
					flag = false;
					checkLast = true;
				}
    	} 
			if(flag || checkLast){
				int p = INT_MIN;
				memcpy(&data_ptr[run_offset], &p, size_int);
				fhr.UnpinPage(phr.GetPageNum());
				}
			
		i = 0;
		while(i<=limit){
			//cout<<"here"<<endl;
			fh.UnpinPage(ph[i].GetPageNum());
			i++;
		}
		fhr.FlushPages();
		cout<<"close "<<fm.CloseFile(fhr)<<endl;
		cout<<"last"<<endl;
		first_page += BUFFER_SIZE - 1;
		delete ph;
		delete size;
		delete count;
		runs++;
		value = 0;
		
	}
	
		runs--;
		cout<< "Total runs "<<runs<<endl;
		fm.ClearBuffer();
		


		// merging runs

		if(runs< BUFFER_SIZE){
			//string s = "sorted.txt";
			merge_runs(1, runs, sorted_file);
		}
		else{
			int new_file_count = runs, maintain_run = 0;
    		int i, one_go, maintain_i = 0;
			bool merge_flag = false;
			while(runs > BUFFER_SIZE - 1){
				i = maintain_i;
        		one_go = 0;
				while(true){
					cout<<"reched while true"<<endl;
					cout<<"runs: "<< maintain_run + runs;
					int min = (i + BUFFER_SIZE  - 2) < (maintain_run + runs) ? (i + BUFFER_SIZE - 2) : (maintain_run + runs);
					one_go++;
					new_file_count++;
					string s = "R.txt";
					string s2 = std::to_string(new_file_count) + s;
					cout<<"i "<<i<<" min "<<min<<endl;
					if(merge_flag)
						merge_runs(i, min, s2);
					else{
						merge_runs(1, min, s2);
						merge_flag = true;
					}
					fm.ClearBuffer();
					//cout<<"new count"<<new_file_count<<endl;
					//cout<<"i "<<i<<" min "<<min<<endl;
					i = min + 1;
					if(min == maintain_run + runs){
						cout<<"min break: "<<min<<endl;
						break;
					}
				}
			runs = one_go;
			maintain_i = new_file_count - one_go + 1;
			maintain_run = new_file_count - one_go;
			}
			if(runs <= BUFFER_SIZE - 1){
				merge_runs(new_file_count - runs + 1, new_file_count, sorted_file);
			}

		}

		
		fm.CloseFile(fh);
		cout<<"After close sorted"<<endl;

		//print sorted file

		cout<< "reached at the end of sorting"<< endl;
		fh = fm.OpenFile(sorted_file);
		p_last = fh.LastPage();
		cout<<"Last page of sorted file: "<<p_last.GetPageNum()<<endl;
		fh.UnpinPage(p_last.GetPageNum());	
		n=0;
		while (n <= p_last.GetPageNum())	{
			ph = fh.PageAt(n);
			cout << "Printing Page " << n << endl;
			data = ph.GetData ();
			for ( i=0;;i+=4){
				memcpy (&num, &data[i], sizeof(int));
				if(num == INT_MIN)
					{	
						fh.UnpinPage(ph.GetPageNum());
						break;
					}
				cout << num << endl;	
			}
			n++;
		}
		fh.FlushPages();
		cout<<"closed sorted file "<<fm.CloseFile(fh)<<endl;

}


void merge_runs(int first_run, int last_run, string s2){
	int i = 0;
	int runs = last_run - first_run + 1;
	FileHandler fh = fm.CreateFile(s2.c_str());
	PageHandler ph = fh.NewPage();
	char* data_sorted = ph.GetData();
	PageHandler* ph_run = new PageHandler[runs];
	FileHandler* fh_run = new FileHandler[runs];
	char ** data = new char*[runs];
	int* count = new int[runs];
	int* total_pages = new int[runs];
	int* current_pages = new int[runs];
	bool flag = true, checkLast=false;
	string s = "R.txt";
	// loading 1st page of every run files
	while(i < runs){
		string s3 = std::to_string(i + first_run) + s;
		cout<<s3<<endl;
		fh_run[i] = fm.OpenFile(s3.c_str());
		total_pages[i] = fh_run[i].LastPage().GetPageNum() + 1;
		fh_run[i].UnpinPage(fh_run[i].LastPage().GetPageNum());
		ph_run[i] = fh_run[i].PageAt(0);
		data[i]= ph_run[i].GetData();
		count[i] = findSize(ph_run[i]);
		i++;
	}
	// priority queue
		priority_queue <HeapNode, vector<HeapNode>, Comparator > pq;
	// first element from all pages
		i = 0;
		int y;
		while(i < runs){
			memcpy(&y, &data[i][0], size_int);
			pq.push(HeapNode(y, i));
			i++;
		}
		int offset = 0;
		int * size = new int[runs];
		for(i = 0; i<runs; i++){
			size[i] = 0;
			current_pages[i] = 0;
		}
		while(pq.empty() == false){
			HeapNode p = pq.top(); 
			//cout << "(" << p.getElement() << ", " << p.getPageNo() << ")"; 
			//cout << endl; 
			int element = p.getElement();
			int pg = p.getPageNo();
			pq.pop();
			size[pg]+=4;
			if(size[pg]/4 < count[pg]){
				int z;
				memcpy(&z, &data[pg][size[pg]], size_int);
				pq.push(HeapNode(z, pg));
			}
			else{
				current_pages[pg]++;
				if(current_pages[pg] < total_pages[pg]){
					cout<<"Run "<<pg<<" new page loaded"<<endl;
					fh_run[pg].UnpinPage(current_pages[pg] -1 );
					ph_run[pg]= fh_run[pg].PageAt(current_pages[pg]);
					data[pg] = ph_run[pg].GetData();
					int z;
					size[pg] = 0;
					count[pg] = findSize(ph_run[pg]);
					memcpy(&z, &data[pg][size[pg]], size_int);
					pq.push(HeapNode(z, pg));
				}
				else{
					fh_run[pg].UnpinPage(current_pages[pg] -1 );
				}
			}
			memcpy(&data_sorted[offset], &element, size_int);
			offset+= 4;
			if(offset == PAGE_CONTENT_SIZE - size_int && pq.empty()==false){
				int p = INT_MIN;
				cout<<"heap final "<<" new page "<< ph.GetPageNum()<<endl;
				memcpy(&data_sorted[offset], &p, size_int);
				fh.UnpinPage(ph.GetPageNum());
				fh.FlushPage(ph.GetPageNum());	
				ph = fh.NewPage();
				data_sorted = ph.GetData();
				offset = 0;
				flag = false;
				checkLast = true;
			}
		}
		cout<<"Heap empty"<<endl;
		if(flag || checkLast){
		int p = INT_MIN;
			memcpy(&data_sorted[offset], &p, size_int);
			fh.UnpinPage(ph.GetPageNum());
			fh.FlushPage(ph.GetPageNum());
			fm.CloseFile(fh);
		}
	for(int i =0; i<runs; i++)
		fm.CloseFile(fh_run[i]);
	//cout<<"Sorted file page unpinned"<<endl;

}


void read_page(int *arr, PageHandler ph, int s)
{
	int i=0;
	char* data = ph.GetData();
	int offset = 0;
	int size = sizeof(int);
	while (i<s)
	{
		memcpy(&arr[i],&data[offset],size);
		offset+=size;
		i++;
	}
}
int findSize(PageHandler ph){
	//PageHandler ph = fh.PageAt(pg);
	char * data = ph.GetData();
	int i , count = 0, num1, size = sizeof(int);
	for ( i=0;;i+=size)
	{
		memcpy (&num1, &data[i], sizeof(int));
		if(num1 == INT_MIN)
			break;
	//cout << "number: " << num1 <<" "<<count << endl;
		count++;
	}
	return count;
}

void write_back(PageHandler ph, int* arr, int n, FileHandler fh){
	char* data = ph.GetData();
	int offset = 0, size = sizeof(int);
	for(int i =0; i<n; i++){
		memcpy(&data[offset], &arr[i], size);
		offset+= size;
	}
	offset+= size;
	int x = INT_MIN;
	memcpy(&data[offset], &x, size);
	fh.MarkDirty(ph.GetPageNum());
}

	