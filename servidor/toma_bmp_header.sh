#!/usr/bin/env bash
# @author Adrian González Pardo
if [ $# -ne  1 ];then
  echo -e "Usage:\n\tbash toma_bmp_header.sh <output>.bmp\n\t./toma_bmp_header.sh <output>.bmp"
  exit 1
fi
output=$1
echo "Tomando una captura de 480*640"
raspistill -e bmp -h 480 -w 640 -o ${output}.bmp
convert ${output}.bmp ${output}.bmp
