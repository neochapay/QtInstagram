#! /bin/bash
for filename in $(find src/api2 src/cripto | egrep '\.cpp')
do 
  gcov -n -o cbuild/src $filename > /dev/null
done
