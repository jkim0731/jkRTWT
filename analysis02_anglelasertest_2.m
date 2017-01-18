v2 = VideoReader('Rawdata _20170115_171217_2.avi');
lasertest2 = zeros(v2.NumberOfFrames,1);

numchunk = ceil(v2.NumberOfFrames/10000);
for i = 1 : numchunk - 1
    video2 = read(v2,[(i-1) * 10000 + 1, i * 10000]);
    video2 = squeeze(video2(:,:,1,:));
    lasertest2((i-1)*10000+1 : i*10000) = squeeze(mean(mean(video2(:,250:end,:),1),2));
end
video2 = read(v2, [i*10000 + 1, Inf]);
video2 = squeeze(video2(:,:,1,:));
lasertest2(i*10000+1:end) = squeeze(mean(mean(video2(:,250:end,:),1),2));

laserON2 = lasertest2> (mean(lasertest2) + 5 * std(lasertest2));

fnb2 = 'Metadata _20170115_171217_2.txt';
M2 = dlmread(fnb2,',',1,0);
b2 = M2(:,9);
laser_b2 = laserON2.*b2;
laser_b2(laser_b2 == 0) = NaN;

time_rtwt2 = M2(:,1) - M(1,1);
%%
threshold_point2 = zeros(v2.NumberOfFrames,1);

%%
for i = 1 : v2.NumberOfFrames - 4
    temp_theta = b2(i:i+4);
    temp_delta = temp_theta(2:end) - temp_theta(1:end-1);
    if isempty(find(temp_delta > 0)) && (temp_theta(4) > -17) && (temp_theta(5) <= -17)
        threshold_point2(i+4) = 1;
    end
end

threshold_b2 = threshold_point2 .* b2;
threshold_b2(threshold_b2 == 0) = NaN;

%%
threshold_b = [threshold_b;threshold_b2];
laserON = [laserON; laserON2];
b = [b;b2];
laser_b = [laser_b; laser_b2];
time_rtwt = [time_rtwt; time_rtwt2];