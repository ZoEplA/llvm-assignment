clang ./debug_info_dot/$1.c -emit-llvm -S -g -o test.ll
clang ./debug_info_dot/$1.c -emit-llvm -S -g -o ./debug_info_dot/$1.ll
opt -dot-cfg ./debug_info_dot/$1.ll > /dev/null
opt -dot-callgraph ./debug_info_dot/$1.ll > /dev/null
dot -Tpng -o ./debug_info_dot/$1.png .$2.dot
dot -Tpng -o ./debug_info_dot/$1.callgraph.png callgraph.dot
rm .*.dot
rm callgraph.dot

