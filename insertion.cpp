#include<iostream>
#include<stdlib.h>
#include "file_manager.h"
#include "errors.h"
#include "constants.h"
#include<cstring>
#include<bits/stdc++.h>
#define FRAMES PAGE_SIZE/sizeof(int)-1
using namespace std;
FileManager fm;
int linear_search(char*, int);
void search(int, int , FileHandler, int, int);
void insertTop(int page, int insert_num, int offset, int last_pg, FileHandler fh);
void insert(int page, int insert_num, int offset, int last_pg, FileHandler fh);
int findSize(int pg, FileHandler fh);
void printfile(FileHandler);


int main(int argc, char** argv) {
	
	//fm.ClearBuffer();
	FileHandler fh;
	PageHandler p_first, p_last, p_mid;
	char *data;
	char *file;
	int num, mid, F, L;
	fstream f;
	
	int first, last;
	char str[255];
	
	char* file_insert = argv[2];
	char* input_file = argv[1];
	f.open(file_insert);
	if(!f) cout << "File not opened" << endl;
	while(!f.eof())
	{

		fm.ClearBuffer();
		fh = fm.OpenFile (input_file);
		f.getline(str,255);
		
		if(str!= " "){
			num = atoi(str);
			//cout<<"int num: "<<num<<endl;
			p_first = fh.FirstPage();
			p_last = fh.LastPage();
			first = p_first.GetPageNum();
			last = p_last.GetPageNum();
			data = p_first.GetData();

			search(first, last, fh, num, last);
			cout << "Value inserted" << endl;
		}
		//fm.CloseFile (fh);
	}
	//printfile(fh);
	f.close();
	fm.ClearBuffer();

}

void printfile(FileHandler fh){
	int n=0,i,num,last;
	int count;
	PageHandler ph, ph2;
	char * data;
	
	ph2 = fh.LastPage();
	last = ph2.GetPageNum();
	
	//cout << ph2.GetPageNum() << endl;
	
	n=0;
	while (n < last+1)	
	{
		ph = fh.PageAt (n);
		cout << "Printing Page " << n << endl;
		data = ph.GetData();

		count=1;
		for ( i=0;;i+=4)
		{
			memcpy (&num, &data[i], sizeof(int));
			if(num == INT_MIN)
			{	fh.UnpinPage(ph.GetPageNum());
				break;
			}
		cout << "number: " << num <<" "<<count << endl;
		
		count++;
	}
	//fh.FlushPages ();
		n++;
			
	}
}


int linear_search (char* data, int insert_value){
	int i=0,num,first,last, size = sizeof(int), num1;
	memcpy (&num, &data[0], size);
	if(num > insert_value)
		return -2;
	while(num <= insert_value){
		i+=size;
		num1 = num;
		memcpy (&num, &data[i], size);
		//cout<< "i :"<< i << " " << "num :"<< num << endl;;
		if(num == INT_MIN && num1 == insert_value)
			return i;
		if(num == INT_MIN)
			break;
	}
	if(num == INT_MIN)
		return -3;
	if(num > insert_value)	
		return i;
}

void search(int first_page, int last_page, FileHandler fh, int num, int const_last){

	int mid = (first_page + last_page)/2;
	PageHandler mid_page = fh.PageAt(mid);
	int isPresent = linear_search(mid_page.GetData(), num);
	//cout << isPresent <<" "<< endl;
	if(isPresent > 0){
		insert(mid, num, isPresent, const_last, fh);
		return;		
	}
	else if(isPresent == -2){
		if(mid -1 >= first_page){
			return search(first_page,mid-1,fh,num, const_last);
		}
		else
		{
			return insertTop(first_page, num, 0, const_last, fh);
		}
		
	}
	else if(isPresent == -3 ){
		if(mid + 1 <= last_page)
			return search(mid+1,last_page,fh,num, const_last);
		else{
			int s = findSize(last_page, fh);
			return insert(last_page, num, s*sizeof(int), const_last, fh);
		}	
	}
}

int findSize(int pg, FileHandler fh){
		PageHandler ph = fh.PageAt(pg);
		char * data = ph.GetData();
		int i , count = 0, num1, size = sizeof(int);
		for ( i=0;;i+=size){
		memcpy (&num1, &data[i], sizeof(int));
		if(num1 == INT_MIN)
			break;
		count++;
	}
	return count;
}


void insert(int page, int insert_num, int offset, int last_pg, FileHandler fh){
	int size = sizeof(int);
	//cout << "I am in Insert" << endl;
	//cout << "Current Page" << page << "   " << "Last Page" << last_pg << endl;
	if(page > last_pg){
		//cout << " I am at last page" << endl;
		PageHandler ph = fh.NewPage();
		//cout << "Insert value" << insert_num << endl;
		char* data = ph.GetData();
		memcpy (&data[0], &insert_num, size);
		insert_num = INT_MIN;
		memcpy (&data[size], &insert_num, size);
		fh.MarkDirty(ph.GetPageNum());
		fh.FlushPage(ph.GetPageNum());
		return;
	}
	int s  = findSize(page, fh);
	int temp1;
	//cout << "size" << s << endl;
	int index = offset/size + 1, temp = insert_num;
	//cout << "Index" << index << "  " << "Offset" << offset << endl;
	PageHandler ph = fh.PageAt(page);
	char * data = ph.GetData();
	if(offset < PAGE_CONTENT_SIZE - size){
	
		memcpy (&temp, &data[offset], size);
		
		memcpy (&data[offset], &insert_num, size);
	
		int x;
		memcpy (&x, &data[offset], size);
		while(index < s){
			offset+= size;
			memcpy (&temp1, &data[offset], size);
			memcpy (&data[offset], &temp, size);
			index++;
			temp = temp1;
		}
	}
	
	if(s != FRAMES - 1){
		offset+= size;
		if(temp != INT_MIN){
			memcpy (&data[offset], &temp, size);
			offset+= size;
			temp = INT_MIN;
			memcpy (&data[offset], &temp, size);
		}
		else{
			memcpy (&data[offset], &temp, size);
		}
		// mark dirty and flush
		fh.MarkDirty(ph.GetPageNum());
		fh.FlushPage(ph.GetPageNum());
		return;
	}
	else{
		fh.MarkDirty(ph.GetPageNum());
		fh.FlushPage(ph.GetPageNum());
		return insert(page + 1, temp, 0, last_pg, fh);
		
	}

}

void insertTop(int page, int insert_num, int offset, int last_pg, FileHandler fh){
	int size = sizeof(int);
	//cout << "I am in insertTop" << endl;
	if(page > last_pg){
		PageHandler ph = fh.NewPage();
		char* data = ph.GetData();
		memcpy (&data[0], &insert_num, size);
		insert_num = INT_MIN;
		memcpy (&data[size], &insert_num, size);
		fh.MarkDirty(ph.GetPageNum());
		fh.FlushPage(ph.GetPageNum());
		//cout<< "New page " << endl;
		return;
	}
	int s  = findSize(page, fh);
	int temp1;
	//cout << "s is " << s << endl;
	int index = offset/size + 1, temp = insert_num;
	//cout << "index is " << index << endl;
	PageHandler ph = fh.PageAt(page);
	char * data = ph.GetData();
	memcpy (&temp, &data[offset], size);
	//cout << "temp" << temp << " " << "offset" << offset <<  endl;
	memcpy (&data[offset], &insert_num, size);
	//cout << "data[offset]" << data[offset]<<endl;
	while(index < s){
		offset+= size;
		memcpy (&temp1, &data[offset], size);
		memcpy (&data[offset], &temp, size);
		index++;
		temp = temp1;
	}
	
	//cout<<"temp " << temp << endl;	
	if(s!= FRAMES - 1){
		offset+= size;
		if(temp != INT_MIN){
			memcpy (&data[offset], &temp, size);
			offset+= size;
			temp = INT_MIN;
			memcpy (&data[offset], &temp, size);
		}
		else{
			memcpy (&data[offset], &temp, size);
		}
		// mark dirty and flush
		fh.MarkDirty(ph.GetPageNum());
		fh.FlushPage(ph.GetPageNum());
		return;
	}
	else{
		fh.MarkDirty(ph.GetPageNum());
		fh.FlushPage(ph.GetPageNum());
		return insertTop(page + 1, temp, 0, last_pg, fh);
	}
}