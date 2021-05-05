set terminal wxt #background rgb "black75"

#load '~/gnuplot-colorbrewer/sequential/PuRd.plt'

set view equal xyz
#set view ,,2
set object circle at first 0,0,0 radius char 0.5 \
    fillstyle empty border lc rgb '#aa1100' lw 2
splot 'pointcloud.csv' using 1:2:3:4 pt 2 ps 0.3 linecolor palette 
#7

pause mouse close
