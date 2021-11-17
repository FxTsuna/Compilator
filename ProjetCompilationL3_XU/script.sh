#!/bin/bash

make
touch test.txt

total=0
passed=0

tests() {
    echo "Fichier: $1" | tee -a test.txt
    let total++
    error=`./bin/tpcc < $1 2>&1`
    if [ $? = 0 ]; then
        echo "Syntaxe valide" | tee -a test.txt
        if [ $2 = 0 ]; then
            let passed++
        fi
    else
        printf "$error\n" | tee -a test.txt
        echo "test invalide" | tee -a test.txt
        if [ $2 = 1 ]; then
            let passed++
        fi
    fi
}

valid_tests() {
    tests $1 0
}

wrong_tests() {
    tests $1 1
}

echo "### Tests valides ###" | tee -a test.txt
for file in ./test/good/*.tpc; do
    valid_tests $file
done
echo "" | tee -a test.txt

echo "### Tests Syntaxe invalides ###" | tee -a test.txt
for file in ./test/syn-err/*.tpc; do
    wrong_tests $file
done
echo "" | tee -a test.txt

echo "### Tests Semantique invalides ###" | tee -a test.txt
for file in ./test/sem-err/*.tpc; do
    wrong_tests $file
done
echo "" | tee -a test.txt

echo "### Tests Warning invalides ###" | tee -a test.txt
for file in ./test/warn/*.tpc; do
    wrong_tests $file
done
echo "" | tee -a test.txt

echo -e "$passed out of $total test(s) passed!" | tee -a test.txt

make clean