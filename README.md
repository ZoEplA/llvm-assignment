# llvm-assignment

以test00为例子，读取assign2-tests下的c文件
```
make
./gen_bc.sh test00
./gen_dot.sh test00 clever # clever为test00的主函数
# 生成的dot文件在debug_info_dot下
```
