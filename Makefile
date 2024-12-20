1 = ./built_from_Make/main.exe
2 = ./built_from_Make/plots.exe
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
	g++ -std=c++20 hadamard.cpp encode.cpp bpsk.cpp main.cpp -o $(1)

noise:
	g++ -std=c++20 hadamard.cpp encode.cpp bpsk.cpp noise_plots.cpp -o $(2)

clean:
	$(RM) $(FixPath)