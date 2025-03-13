CXX = g++
CXXFLAGS	:= -std=c++17 -w -g
OUTPUT	:= output

# define source directory 运行时修改此处路径
SRC		:= src/$(dir) #// 传递 var 变量定义执行文件目录
CLEAN_SRC		:= src/$(dir)/*.o #// 删除所有.o文件

INCLUDE	:= include
LIB		:= lib

LIBRARIES	:= -lglad -lglfw3dll -llibassimp
MAIN	:= main.exe
SOURCEDIRS	:= $(SRC)
INCLUDEDIRS	:= $(INCLUDE)
LIBDIRS		:= $(LIB)
FIXPATH = $(subst /,/,$1)
RM			:= del /q a/f
MD	:= mkdir

INCLUDES	:= $(patsubst %,-I%, $(INCLUDEDIRS:%/=%))
LIBS		:= $(patsubst %,-L%, $(LIBDIRS:%/=%))
SOURCES		:= $(wildcard $(patsubst %,%/*.cpp, $(SOURCEDIRS)))
SOURCES	+= include/imgui/imgui_impl_glfw.cpp include/imgui/imgui_impl_opengl3.cpp
SOURCES	+= include/imgui/imgui.cpp include/imgui/imgui_draw.cpp include/imgui/imgui_widgets.cpp
OBJECTS		:= $(SOURCES:.cpp=.o)

OUTPUTMAIN	:= $(call FIXPATH,$(OUTPUT)/$(MAIN))

all: $(OUTPUT) $(MAIN)
	@echo Executing 'all' complete!

$(OUTPUT):
	$(MD) $(OUTPUT)
$(MAIN): $(OBJECTS) 
	$(CXX) $(CXXFLAGS) $(INCLUDES) -o $(OUTPUTMAIN) $(OBJECTS) $(LFLAGS) $(LIBS) $(LIBRARIES)

.cpp.o:
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $<  -o $@

.PHONY: clean
clean:
	$(RM) $(OUTPUTMAIN)
	$(RM) $(call FIXPATH,$(CLEAN_SRC))
	@echo Cleanup complete!
# 此处./src/$(dir) 传递main函数 argv 的参数
run: all
	./$(OUTPUTMAIN) src/$(dir)/
	@echo Executing 'run: all' complete!


