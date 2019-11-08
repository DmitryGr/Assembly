g++ gen.cpp -std=c++17 -o gen
arm-linux-gnueabi-gcc -lstdc++ -marm tester.c task1.cpp -o main

for ((i = 0; i < 100000; i++))
do
    ./gen $i > input.txt
    python3 checker.py < input.txt > out1.txt
    qemu-arm -L $LINARO_SYSROOT main < input.txt > out2.txt
    diff out1.txt out2.txt || break
    echo $i
done
