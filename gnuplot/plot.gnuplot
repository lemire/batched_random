# gnuplot -e "filename='foo.data'" plot.gnuplot


set term pdf
set out filename . ".pdf"
plot filename using 1:2:3 with filledcurves title 'std::shuffle' fillstyle solid 0.5, \
"" using 1:4:5 with filledcurves title 'shuffle\_2' fillstyle solid 0.5, \
"" using 1:6:7 with filledcurves title 'shuffle\_2\_4' fillstyle solid 0.5, \
"" using 1:8:9 with filledcurves title 'shuffle\_2\_4\_6' fillstyle solid 0.5, \
"" using 1:10:11 with filledcurves title 'precomp' fillstyle solid 0.5