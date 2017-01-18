% clear
 
matfn = 'data_4ms.mat';
load(matfn)


laser_b = laserON.*b;
laser_b(laser_b == 0) = NaN;

figure, plot(-b,'r.'), hold on, plot(-laser_b,'b.'), plot(-threshold_b,'g.'), xlabel('Time (ms)'), ylabel('Angle (degrees)'), xlim([0 40000]), ylim([-40 70])
legend({'Theta','Laser','Passing points'})

figure, bar([0:14],laser_onset_pct), xlabel('Time (ms)'), ylabel('Proportion (%)'), title('Laser onset time'), xlim([0 15])
figure, bar(laser_dur_pct), xlabel('Time (ms)'), ylabel('Proportion (%)'), title('Laser duration'), xlim([0 5])