#!/bin/bash

sfile="$1"
sdest="$2"
if [ -f "$sfile" ]
then
    if [ -f "$sdest" ]
    then
	tfile="temp-$$-$sdest"
	echo ffmpeg -i \""$sdest"\" -codec copy \\ > "$tfile.meta.sh"
	ffmpeg -i "$sfile" 2>&1 | sed -e '1,/^  Metadata\:/d;/^  Duration\:/,$d;s/^[ \t]*//;s/[ \t]*$//;s/[ ]*\:[ ]*/:/;s/\:/\"\=\"/;s/^/\"/;s/$/\" \\/;s/^/-metadata /' >> "$tfile.meta.sh"
	echo \""$tfile"\" >> "$tfile.meta.sh"
	echo "mv -fv \"$tfile\" \"$sdest\"" >> "$tfile.meta.sh"
	bash "$tfile.meta.sh"
	rm -v "$tfile.meta.sh"

	echo ""
    fi
fi
