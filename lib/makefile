main: main.cpp Instruction.hpp Instruction.cpp Executor.hpp Executor.cpp config.hpp
	clang++ main.cpp -o main Instruction.cpp Executor.cpp  \
	-pipe -std=c++20 -ggdb -Og -march=native               \
	-Wall -Wextra -Wfloat-equal -Wshadow -Wconversion -Wcast-align -Wlogical-op -Wpadded -Wredundant-decls -Winline -Weffc++ \
	-fsanitize=address -fsanitize=undefined -fsanitize-address-use-after-scope -fstack-protector-strong \
	-D_GLIBCXX_DEBUG;
