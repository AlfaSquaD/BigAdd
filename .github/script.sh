ba_files=`ls ../testFiles/*.ba`
for file in $ba_files
do
  filename="${file%.*}"
  printf "%s\n\n" $filename
  ./BigAdd $filename &
  PID=$!
  wait $PID
  printf "\n\n"
done
