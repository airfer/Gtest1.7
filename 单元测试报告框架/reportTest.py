#!/usr/bin/python
#coding=utf-8
import sys
from reportClass import Report

report=Report()

def Init():
    report.Init()

def writeData(*args):
    report.writeData(args)


Init()
writeData(2,2)

