make -B
./drumbadum 440 10 2
graph -T png < output.txt > output.png
echo "Plotted graph to output.png"
sox -r 48000 -e signed -b 16 -c 1 -t raw output.raw output.wav