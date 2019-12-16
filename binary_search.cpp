//Sample file for students to get their code running

#include<iostream>
#include<stdlib.h>
#include "file_manager.h"
#include "errors.h"
#include<cstring>
#include<bits/stdc++.h>
#define PAGE_SIZE 4096
using namespace std;

int linear_search(char*, int);
void search(int, int , FileHandler, int);


int main(int argc, char** argv) {
	FileManager fm;
	FileHandler fh;
	PageHandler p_first, p_last, p_mid;
	char *data;
	char *file;
	int num1;
	int num, mid, F, L;
	int page_len = PAGE_SIZE/4;
	fm.ClearBuffer();
 	
	fh = fm.OpenFile (argv[1]);
	num = atoi(argv[2]);
	p_first = fh.FirstPage();
	p_last = fh.LastPage();
	data = p_first.GetData();
		
	search(p_first.GetPageNum(), p_last.GetPageNum(), fh, num);
		
	fm.CloseFile (fh);
	fm.ClearBuffer();
	
}


int linear_search (char* data, int search_value){
	int i,num,first,last, size = sizeof(int);	
	for (i=0;;i+=size)
	{
	memcpy (&num, &data[i], size);
	if(num == INT_MIN)
	break;
	if(num == search_value){
		return (i/size + 1);	
		}	
	}
	memcpy(&first,&data[0],size);
	if(search_value < first)
		return -2;
	memcpy(&last,&data[i-size],size);
	if(search_value > last)	
		return -3;
	else
		return -1;
}

void search(int first_page, int last_page, FileHandler fh, int num){

	int mid = (first_page + last_page)/2;
	PageHandler mid_page = fh.PageAt(mid);
	int isPresent = linear_search(mid_page.GetData(), num);
	if(isPresent == -1){
		cout<< "-1,-1" << endl;
		return;		
	}
	else if(isPresent == -2){
		if(mid -1 >= first_page){
			fh.FlushPage(mid_page.GetPageNum());
			return search(first_page,mid-1,fh,num);
		}
	else{
		cout<< "-1,-1" << endl;
		return;
	}	
	}
	else if(isPresent == -3 ){
		if(mid + 1 <= last_page){
			fh.FlushPage(mid_page.GetPageNum());
			return search(mid+1,last_page,fh,num);
		}
		else{
		cout<< "-1,-1" << endl;
		return;
		}	
	}
	else
	cout << mid+1 << "," << isPresent << endl;
}


