#!/usr/bin/env bats

# File: student_tests.sh
# 
# Create your unit tests suit in this file

@test "Example: check ls runs without errors" {
    run ./dsh <<EOF                
ls
EOF

    # Assertions
    [ "$status" -eq 0 ]
}

@test "Exit" {
    run "./dsh" <<EOF                
exit
EOF

    # Strip all whitespace (spaces, tabs, newlines) from the output
    stripped_output=$(echo "$output" | tr -d '[:space:]')

    # Expected output with all whitespace removed for easier matching
    expected_output="dsh2>cmdloopreturned0"

    # These echo commands will help with debugging and will only print
    #if the test fails
    echo "Captured stdout:" 
    echo "Output: $output"
    echo "Exit Status: $status"
    echo "${stripped_output} -> ${expected_output}"

    # Check exact match
    [ "$stripped_output" = "$expected_output" ]
}

@test "Will not run long exe" {
    echo "echo "I actually did run..."" > this_is_some_file_that_is_an_executable_but_too_longgggggg.bash
    chmod +x this_is_some_file_that_is_an_executable_but_too_longgggggg.bash
    run "./dsh" <<EOF                
./this_is_some_file_that_is_an_executable_but_too_longgggggg.bash
EOF
    rm -rf this_is_some_file_that_is_an_executable_but_too_longgggggg.bash

    # Strip all whitespace (spaces, tabs, newlines) from the output
    stripped_output=$(echo "$output" | tr -d '[:space:]')

    # Expected output with all whitespace removed for easier matching
    expected_output="dsh2>dsh2>cmdloopreturned0"

    # These echo commands will help with debugging and will only print
    #if the test fails
    echo "Captured stdout:" 
    echo "Output: $output"
    echo "Exit Status: $status"
    echo "${stripped_output} -> ${expected_output}"

    # Check exact match
    [ "$stripped_output" = "$expected_output" ]
}

@test "Change to a quoted directory" {
    current=$(pwd)

    mkdir -p "test    for    fun    "
    run "${current}/dsh" <<EOF                
cd "test    for    fun    "  
pwd
EOF
    rmdir "test    for    fun    "

#     # Strip all whitespace (spaces, tabs, newlines) from the output
    stripped_output=$(echo "$output" | tr -d '\t\n\r\f\v')

#     # Expected output with all whitespace removed for easier matching
    expected_output="${current}/test    for    fun    dsh2> dsh2> dsh2> cmd loop returned 0"

#     # These echo commands will help with debugging and will only print
#     #if the test fails
    echo "Captured stdout:" 
    echo "Output: $output"
    echo "Exit Status: $status"
    echo "${stripped_output} -> ${expected_output}"

#     # Check exact match
    [ "$stripped_output" = "$expected_output" ]
}

@test "cd does nothing with 3 commands" {
    run "./dsh" <<EOF                
cd bats fun
EOF

    # Strip all whitespace (spaces, tabs, newlines) from the output
    stripped_output=$(echo "$output" | tr -d '[:space:]')

    # Expected output with all whitespace removed for easier matching
    expected_output="dsh2>dsh2>cmdloopreturned0"

    # These echo commands will help with debugging and will only print
    #if the test fails
    echo "Captured stdout:" 
    echo "Output: $output"
    echo "Exit Status: $status"
    echo "${stripped_output} -> ${expected_output}"

    # Check exact match
    [ "$stripped_output" = "$expected_output" ]
}

@test "Can run other programs" {
    echo "echo hello!" > test.bash
    chmod +x test.bash
    run "./dsh" <<EOF                
./test.bash
EOF
    rm -rf test.bash

    # Strip all whitespace (spaces, tabs, newlines) from the output
    stripped_output=$(echo "$output" | tr -d '[:space:]')

    # Expected output with all whitespace removed for easier matching
    expected_output="hello!dsh2>dsh2>cmdloopreturned0"

    # These echo commands will help with debugging and will only print
    #if the test fails
    echo "Captured stdout:" 
    echo "Output: $output"
    echo "Exit Status: $status"
    echo "${stripped_output} -> ${expected_output}"

    # Check exact match
    [ "$stripped_output" = "$expected_output" ]
}

@test "Print errno and rc" {
    touch test
    run "./dsh" <<EOF                
./test
rc
EOF
    rm -rf test

    # Strip all whitespace (spaces, tabs, newlines) from the output
    stripped_output=$(echo "$output" | tr -d '[:space:]')

    # Expected output with all whitespace removed for easier matching
    expected_output="dsh2>dsh2>Permissiondenieddsh2>13dsh2>cmdloopreturned0"

    # These echo commands will help with debugging and will only print
    #if the test fails
    echo "Captured stdout:" 
    echo "Output: $output"
    echo "Exit Status: $status"
    echo "${stripped_output} -> ${expected_output}"

    # Check exact match
    [ "$stripped_output" = "$expected_output" ]
}