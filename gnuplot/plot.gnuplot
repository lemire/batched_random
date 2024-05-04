# gnuplot -e "filename='foo.data'" plot.gnuplot


set term pdf
set out filename . "cpppcg64.pdf"
set ylabel "time per item (ns)"
set xlabel "number of entries"
stats filename using 1 nooutput name 'X_'
set style fill transparent solid 0.1

# Access the min and max X-values
xmin = X_min
xmax = X_max
set xrange [xmin:xmax]
set yrange [0:]
set key bottom center

plot filename using 1:2:3 with filledcurves title 'std::shuffle' fillstyle solid 0.9, \
"" using 1:4:5 with filledcurves title 'shuffle\_2' fillstyle solid 0.9, \
"" using 1:8:9 with filledcurves title 'shuffle\_2\_4\_6' fillstyle solid 0.9

#"" using 1:6:7 with filledcurves title 'shuffle\_2\_4' fillstyle solid 0.9, \


set out filename . "cppmersenne.pdf"

plot filename using 1:10:11 with filledcurves title 'std::shuffle' fillstyle solid 0.9, \
"" using 1:12:13 with filledcurves title 'shuffle\_2' fillstyle solid 0.9, \
"" using 1:16:17 with filledcurves title 'shuffle\_2\_4\_6' fillstyle solid 0.9

#"" using 1:14:15 with filledcurves title 'shuffle\_2\_4' fillstyle solid 0.9, \


set out filename . "lehmer.pdf"

plot filename using 1:20:21 with filledcurves title 'shuffle' fillstyle solid 0.9, \
"" using 1:22:23 with filledcurves title 'shuffle\_2' fillstyle solid 0.9, \
"" using 1:26:27 with filledcurves title 'shuffle\_2\_4\_6' fillstyle solid 0.9

#"" using 1:24:25 with filledcurves title 'shuffle\_2\_4' fillstyle solid 0.9, \


set out filename . "pcg64.pdf"

plot filename using 1:28:29 with filledcurves title 'shuffle' fillstyle solid 0.9, \
"" using 1:30:31 with filledcurves title 'shuffle\_2' fillstyle solid 0.9, \
"" using 1:34:35 with filledcurves title 'shuffle\_2\_4\_6' fillstyle solid 0.9

#"" using 1:32:33 with filledcurves title 'shuffle\_2\_4' fillstyle solid 0.9, \
