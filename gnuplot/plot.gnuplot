# gnuplot -e "filename='foo.data'" plot.gnuplot

set term pdf
set ylabel "time per item (ns)"
set xlabel "number of entries"
stats filename using 1 nooutput name 'X_'
set style fill border

# Access the min and max X-values
xmin = X_min
xmax = X_max
set xrange [xmin:xmax]
set yrange [0:]
set key bottom center
set logscale x 2
set format x "2^{%L}"


set out filename . "lehmer.pdf"

plot filename using 1:2 with lines lw 5 title 'shuffle' , \
"" using 1:3 with lines lw 5 title 'naive shuffle\_2' , \
"" using 1:4 with lines lw 5 title 'shuffle\_2' , \
"" using 1:5 with lines lw 5 title 'shuffle\_6' 

set out filename . "pcg64.pdf"

plot filename using 1:6 with lines lw 5 title 'shuffle' , \
"" using 1:7 with lines lw 5 title 'naive shuffle\_2' ,\
"" using 1:8 with lines lw 5 title 'shuffle\_2' , \
"" using 1:9 with lines lw 5 title 'shuffle\_6'

set out filename . "chacha.pdf"

plot filename using 1:10 with lines lw 5 title 'shuffle' , \
"" using 1:11 with lines lw 5 title 'naive shuffle\_2' , \
"" using 1:12 with lines lw 5 title 'shuffle\_2' , \
"" using 1:13 with lines lw 5 title 'shuffle\_6' 


set ylabel "speed ratio (shuffle\\\_6/shuffle)"
set yrange [1:5]
set out filename . "ratio.pdf"

plot filename using 1:($10/$13) with lines lw 5 title 'ChaCha', \
"" using 1:($6/$9) with lines lw 5 title 'PCG64', \
"" using 1:($2/$5) with lines lw 5 title 'Lehmer'