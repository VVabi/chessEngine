sed -i 's/\([^ !><=|+-]\)=/\1 =/g' $(find ../../Cpp -type f -regex ".*\.\(c\|h\|hpp\|cpp\)")
sed -i 's/\=\([^ =<>]\)/= \1/g' $(find ../../Cpp -type f -regex ".*\.\(c\|h\|hpp\|cpp\)")
