#!/bin/python
#coding=utf-8
import sys
from xlwt import Workbook

class Report(object): 
    
    #类初始化
    def __init__(self):
        print "***************Starting Unit Test***************"
        self.index_x=0
        self.index_y=0 

    #模板初始化
    def Init(self):
        self.report =Workbook(encoding='utf-8')
        self.sheet  =self.report.add_sheet('测试结束报告',cell_overwrite_ok=True)
        self.sheet.write(self.index_x,self.index_y,'测试结束报告')
        self.sheet.write(2,0,'测试用例名词')
        self.sheet.write(2,1,'测试结果')

    #向单元格中写入数据
    def writeData(self,*args):
        for x,y in args:
            index_x=x
            index_y=y
            self.sheet.write(index_x,index_y,'ceshi')
        self.report.save("测试结束报告.xls")











