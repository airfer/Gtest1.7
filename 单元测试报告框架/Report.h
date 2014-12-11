/*************************************************************************
	> File Name: Report.cpp
	> Author: airfer
	> Mail: airfer@126.com 
	> Created Time: Tue 09 Dec 2014 07:51:49 PM PST
************************************************************************/
#include<iostream>
#include<python2.6/Python.h>
#include<string>
using namespace std;

class Report {
    //下面主要是向EXCEL写入数据的方法
public:
    Report():pModule(NULL),pFunc(NULL),pArgs(NULL),errorSign(NULL){
        Py_Initialize();
        PyRun_SimpleString("import sys");
        PyRun_SimpleString("sys.path.append('./')");
    }//构造函数结束
     
    //导入文件
     int readFile(const char* fileName){
         pModule=PyImport_ImportModule("reportTest");
         if(pModule==NULL){
             cout<<"[ERROR] pModule为空指针，请检测"<<endl;
             //返回值不为0则失败
             return 1;
         }
     }
    //模板初始化
    int writeInit(const char* funcName){
        //模板初始化
        pFunc=PyObject_GetAttrString(pModule,funcName);
        if(pFunc==NULL){
            cout<<"[ERROR] pFunc为空指针，请检测"<<endl;
        }
        errorSign=PyEval_CallObject(pFunc,NULL);
        if(errorSign==NULL){
            cout<<"[ERROR] 函数调用出错，请检查"<<endl;
        }
    }
    //向单元格中写入数据
    int writeData(const char* funcName,int index_x, int index_y){

        pFunc=PyObject_GetAttrString(pModule,funcName);
        if(pFunc==NULL){
            cout<<"[ERROR] pFunc为空指针，请检测"<<endl;
        }
        //构建元组用于传递数据
        pArgs=PyTuple_New(2);
        PyTuple_SetItem(pArgs,0,Py_BuildValue("i",index_x));
        PyTuple_SetItem(pArgs,1,Py_BuildValue("i",index_y));
        //调用函数
        PyObject_CallObject(pFunc,pArgs);
    }
    //析够函数
    ~Report(){
        Py_DECREF(pModule);
        Py_DECREF(pFunc);
        Py_DECREF(errorSign);
        //Py_DECREF(pArgs);
        Py_Finalize();
    }

//定义私有变量
private:
    PyObject *pModule;
    PyObject *pFunc;
    PyObject *errorSign;
    PyObject *pArgs;
};//Report类定义结束









    
