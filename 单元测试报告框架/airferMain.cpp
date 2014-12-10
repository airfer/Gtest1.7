/*************************************************************************
	> File Name: airferMain.cpp
	> Author: ma6174
	> Mail: ma6174@163.com 
	> Created Time: Mon 08 Dec 2014 06:04:01 PM PST
 ************************************************************************/

#include<iostream>
#include<string>
#include<python2.6/Python.h>
#include"Report.h"
using namespace std;

void writefile();

int main(){

    cout<<"Starting test...."<<endl;
    writefile();
    cout<<"Ending test...."<<endl;
}

void writefile(){
    /*Py_Initialize();

    PyObject *pModule = NULL;
    PyObject *pFunc = NULL;
    PyObject *pName = NULL;
    
    PyRun_SimpleString("import sys");
    PyRun_SimpleString("sys.path.append('./')");
   // pName=PyString_FromString("reportTest");
   // pModule=PyImport_Import(pName);
    pModule = PyImport_ImportModule("reportTest");
    if(pModule==NULL){
        cout<<"read file error!"<<endl;
    }

    pFunc=PyObject_GetAttrString(pModule,"Init");
    PyEval_CallObject(pFunc,NULL);

    Py_Finalize();*/

    Report report;
    string fileName="reportTest";
    string funcName_init="Init";
    string funcName_write="writeData";
    report.readFile(fileName.c_str());
    report.writeInit(funcName_init.c_str());
    report.writeData(funcName_write.c_str(),2,2);
    

}

