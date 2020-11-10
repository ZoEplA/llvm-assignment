cp ./assign2-tests/$1.c ./test.c
cp ./assign2-tests/$1.c ./debug_info_dot
clang -emit-llvm -S -g test.c -o test.ll
clang -emit-llvm -c -g test.c -o test.bc
clang -emit-llvm -c -g test.c -o ./debug_info_dot/$1.bc
./llvmassignment test.bc
