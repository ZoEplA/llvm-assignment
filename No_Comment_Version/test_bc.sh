for((i=0;i<=9;i++));
do
./llvmassignment ./assign2-tests/test0$i.bc
echo "====="
done
for((i=10;i<=19;i++));                                                                                
do
./llvmassignment ./assign2-tests/test$i.bc
echo "====="
done
