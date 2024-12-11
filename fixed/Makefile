1 = ./built_from_Make/main.exe
ifdef OS
	RM = del /Q
	FixPath = $(subst /,\,$1)
else
	ifeq ($(shell uname), Linux)
		RM = rm -f
		FixPath = $1
	endif
endif

all:
	g++ -std=c++20 -g *.cpp -o $(1)

clean:
	$(RM) $(FixPath)