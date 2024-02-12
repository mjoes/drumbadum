make -B
./drumbadum 40 10 2
graph -T png < output.txt > output.png
echo "Plotted graph to output.png"