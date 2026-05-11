#!/bin/bash


script_dir="$( cd "$( dirname "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )"


total_tests=0
passed_tests=0


run_test() {
    local test_file=$1
    local expected_exit_code=$2
    local test_type=$3

    ./../bin/tpcas < "$test_file"
    exit_code=$?

    if [ $exit_code -eq $expected_exit_code ]; then
        echo "Test passed: $test_file"
        ((passed_tests++))
    else
        echo "Test failed: $test_file"
    fi

    ((total_tests++))
    echo "Test type: $test_type"
}


good_folder="${script_dir}/good"
echo "Testing 'good' folder..."
if [ -d "$good_folder" ]; then
    for test_file in "$good_folder"/*; do
        [ -e "$test_file" ] || continue
        run_test "$test_file" 0 "good"
    done
else
    echo "Directory not found: $good_folder"
fi


syn_err_folder="${script_dir}/syn-err"
echo "Testing 'syn-err' folder..."
if [ -d "$syn_err_folder" ]; then
    for test_file in "$syn_err_folder"/*; do
        [ -e "$test_file" ] || continue
        run_test "$test_file" 1 "syn-err"
    done
else
    echo "Directory not found: $syn_err_folder"
fi


final_score=$(( (passed_tests * 100) / total_tests ))
echo "Tests passed: $passed_tests out of $total_tests"
echo "Final score: $final_score%"


exit $((total_tests - passed_tests))

