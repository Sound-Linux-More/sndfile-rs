#!/bin/bash
#Depends: sox

echo ""
echo "Sound eXchange (SoX), the Swiss Army knife of audio manipulation"
tfile="$1"
if [ -f "$tfile" ]
then
    echo "Spectrogram for $tfile..."
    sox -S $tfile -n spectrogram -x 512 -y 256 -t "$tfile" -o "$tfile-spectrogram.png"
else
    echo "Usage: bash $0 {wav,flac,mp3,ogg}-file"
fi
