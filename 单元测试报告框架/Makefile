# 用于生成目标文件
#

PYTHON_DIR = /usr/include/python2.6/
USER_DIR = .
CPPFLAGS_COMPLIEE= -c
CPPFLAGS_OBJECT= -o

HEADERS = /usr/include/python2.6 ./Report.h

SOURCE_DIR = ./*.cpp $(PYTHON_DIR)/Python.h

airferMain : $(SOURCE_DIR)
	g++ -I$(HEADERS) -L/usr/lib/python2.6 -lpython2.6 -g  ./airferMain.cpp -o airferMain
clean : 
	rm -f airferMain.o airferMain
