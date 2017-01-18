%%
clear
v = VideoReader('Rawdata _20170115_171217_1.avi');
lasertest = zeros(v.NumberOfFrames,1);

numchunk = ceil(v.NumberOfFrames/10000);
for i = 1 : numchunk - 1
    video = read(v,[(i-1) * 10000 + 1, i * 10000]);
    video = squeeze(video(:,:,1,:));
    lasertest((i-1)*10000+1 : i*10000) = squeeze(mean(mean(video(:,250:end,:),1),2));
end
video = read(v, [i*10000 + 1, Inf]);
video = squeeze(video(:,:,1,:));
lasertest(i*10000+1:end) = squeeze(mean(mean(video(:,250:end,:),1),2));

laserON = lasertest> (mean(lasertest) + 5 * std(lasertest));

fnb = 'Metadata _20170115_171217_1.txt';
M = dlmread(fnb,',',1,0);
b = M(:,9);
laser_b = laserON.*b;
laser_b(laser_b == 0) = NaN;

time_rtwt = M(:,1) - M(1,1);
time_rtwt = time_rtwt * 1000; % to make it ms
%%
threshold_point = zeros(v.NumberOfFrames,1);

for i = 1 : v.NumberOfFrames - 4
    temp_theta = b(i:i+4);
    temp_delta = temp_theta(2:end) - temp_theta(1:end-1);
    if isempty(find(temp_delta > 0)) && (temp_theta(4) > -17) && (temp_theta(5) <= -17)
        threshold_point(i+4) = 1;
    end
end

threshold_b = threshold_point .* b;
threshold_b(threshold_b == 0) = NaN;
%%
fnb_test = '20170115171204.txt';
fid = fopen(fnb_test);

time_ard = [];
angle_ard = [];
time_event = [];
event_ard = {};

i = 0;
while 1     
    C = fgetl(fid);
    if isempty(C) | (C == -1), break, end
    if length(C) > 1
        s = regexp(C,'\t', 'split');
        if isempty(str2num(s{2}))
            i = i + 1;
            time_event = [time_event; str2num(s{1})];
            event_ard{i} = s{2};
        else
            time_ard = [time_ard; str2num(s{1})];
            angle_ard = [angle_ard; str2num(s{2})];
        end
    end
end
fclose(fid);
time_ard = time_ard - time_ard(1);
time_event = time_event - time_ard(1);
time_ard = time_ard * 1000; % to make it ms
time_event = time_event * 1000;


%%
laserONtime_ard = [];
laserOFFtime_ard = [];
poleUPtime_ard = [];
poleDOWNtime_ard = [];
stimreadytime_ard = [];
for i = 1 : length(event_ard)
    if ~isempty(strfind(event_ard{i},'laser ON'))
        laserONtime_ard = [laserONtime_ard; time_event(i)];
    end
    if ~isempty(strfind(event_ard{i},'laser OFF'))
        laserOFFtime_ard = [laserOFFtime_ard; time_event(i)];
    end
    if ~isempty(strfind(event_ard{i},'Pole Up'))
        poleUPtime_ard = [poleUPtime_ard; time_event(i)];
    end
    if ~isempty(strfind(event_ard{i},'Pole Down'))
        poleDOWNtime_ard = [poleDOWNtime_ard; time_event(i)];
    end    
    if ~isempty(strfind(event_ard{i},'Stim Ready'))
        stimreadytime_ard = [stimreadytime_ard; time_event(i)];
    end    
end

%%
laser_ind_ard = [];
for i = 1 : length(laserONtime_ard)
    laser_ind_ard = [laser_ind_ard; find(time_ard >= laserONtime_ard(i) & time_ard < laserOFFtime_ard(i))];
end

figure, plot(time_ard, -angle_ard, 'r.'), hold on, plot(time_ard(laser_ind_ard), -angle_ard(laser_ind_ard), 'b.')


%%
figure, plot(time_rtwt,-b,'r.'), hold on, plot(time_ard, -angle_ard, 'k.')
%%
figure, plot(time_rtwt,-b,'r.'), hold on, plot(time_rtwt,-laser_b,'b.'), plot(time_rtwt,-threshold_b,'g.'), xlabel('Time (ms)'), ylabel('Angle (degrees)'), ylim([-40 70])
legend({'Theta','Laser','Passing points'})

%%
checkpoints = find(threshold_point == 1);
laser_onset = zeros(15,1);
laser_dur = zeros(15,1);
for i = 1 : length(checkpoints)
    temp_range = checkpoints(i):checkpoints(i)+15;
    temp_laser_dur = length(find(laserON(temp_range) == 1));
    temp_laser_on = find(laserON(temp_range) == 1,1);
    
    if ~isempty(temp_laser_on)
        laser_onset(temp_laser_on) = laser_onset(temp_laser_on) + 1;
        laser_dur(temp_laser_dur) = laser_dur(temp_laser_dur) + 1;
    end
end
laser_onset_pct = laser_onset/sum(laser_onset)*100;
laser_dur_pct = laser_dur/sum(laser_dur)*100;
% figure, bar([0:9],laser_onset), xlabel('Time (ms)'), ylabel('# of incidents'), title('Laser onset time'), xlim([0 10])
figure, bar([0:14],laser_onset_pct), xlabel('Time (ms)'), ylabel('Proportion (%)'), title('Laser onset time'), xlim([0 15])
figure, bar(laser_dur_pct), xlabel('Time (ms)'), ylabel('Proportion (%)'), title('Laser duration'), xlim([0 5])


%%
save('data_4ms.mat', 'b', 'laser_dur_pct', 'laser_onset_pct', 'laser_dur', 'laser_onset', 'lasertest', 'laserON', 'threshold_point', 'threshold_b')