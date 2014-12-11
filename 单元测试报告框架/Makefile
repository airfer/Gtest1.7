# 用于生成目标文件
#
GXX= g++
PYTHON_DIR = /usr/include/python2.6
USER_DIR = .
LIBRARY= -L/usr/lib/python2.6 -lpython2.6
DEBUG_FLAG= -g
CPPFLAGS_OBJECT= -o

HEADERS = -I/usr/include/python2.6 -I./*h

SOURCE_DIR = ./*.cpp 

airferMain : $(SOURCE_DIR)
	$(GXX) $(HEADERS) $(LIBRARY) $(DEBUG_FLAG) $(SOURCE_DIR) $(CPPFLAGS_OBJECT) $@
clean : 
	rm -f airferMain.o airferMain
