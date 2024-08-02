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

plot filename using 1:14 with lines lw 5 title 'shuffle' , \
"" using 1:34 with lines lw 5 title 'naive shuffle\_2' , \
"" using 1:16 with lines lw 5 title 'shuffle\_2' , \
"" using 1:18 with lines lw 5 title 'shuffle\_6' 

set out filename . "pcg64.pdf"

plot filename using 1:20 with lines lw 5 title 'shuffle' , \
"" using 1:36 with lines lw 5 title 'naive shuffle\_2' ,\
"" using 1:22 with lines lw 5 title 'shuffle\_2' , \
"" using 1:24 with lines lw 5 title 'shuffle\_6'

set out filename . "chacha.pdf"

plot filename using 1:26 with lines lw 5 title 'shuffle' , \
"" using 1:38 with lines lw 5 title 'naive shuffle\_2' , \
"" using 1:28 with lines lw 5 title 'shuffle\_2' , \
"" using 1:30 with lines lw 5 title 'shuffle\_6' 
