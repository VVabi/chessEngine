sed -i 's/,\([^ ]\)/, \1/g' $(find ../../Cpp -type f -name "*.cpp") $(find ../../Cpp -type f -name "*.hpp")  $(find ../../Cpp -type f -name "*.c") $(find ../../Cpp -type f -name "*.h") 
