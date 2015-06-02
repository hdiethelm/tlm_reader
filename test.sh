#!/bin/sh

if [ $# -lt 1 ] ; then
        echo "usage: test.sh file ..."
        exit
fi

FILES=$@

for FILE in $FILES ; do
        NAME=$(basename "$FILE" ".TLM")
	echo -n "Press enter to run tlm_reader $FILE > out.txt: "
        read INPUT
	./tlm_reader $FILE > out.txt
done
