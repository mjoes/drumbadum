make -B
./drumbadum
graph -T png < output.txt > output.png
echo "Plotted graph to output.png"
sox -r 44100 -e signed -b 16 -c 2 -t raw output.raw output.wav