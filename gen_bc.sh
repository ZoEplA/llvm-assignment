cp ./assign2-tests/$1.c ./test.c
cp ./assign2-tests/$1.c ./debug_info_dot
clang -emit-llvm -c -g test.c -o test.bc
clang -emit-llvm -c -g $1.c -o ./debug_info_dot/$1.bc
./llvmassignment test.bc
