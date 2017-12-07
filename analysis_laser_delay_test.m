%% read data from avi and txt from RTWT
for delay_set = 2
    threshold = -25;
    direction = 'protraction';
    fn = ['delay_', num2str(delay_set), 'ms'];
    v = VideoReader([fn,'.avi']);
    num_frames = v.NumberOfFrames;
    lasertest = zeros(num_frames,1);

    numchunk = ceil(num_frames/10000);
    for i = 1 : numchunk - 1
        video = read(v,[(i-1) * 10000 + 1, i * 10000]);
        video = squeeze(video(:,:,1,:));
        lasertest((i-1)*10000+1 : i*10000) = squeeze(mean(mean(video(:,1:100,:),1),2));
    end
    video = read(v, [i*10000 + 1, Inf]);
    video = squeeze(video(:,:,1,:));
    lasertest(i*10000+1:end) = squeeze(mean(mean(video(:,1:100,:),1),2));

    % for the sake of memory,
    clear video

    laserON = lasertest> (mean(lasertest) + sqrt(std(lasertest)));

    fnb = [fn,'.txt'];
    % fnblist = dir('Metadata _*.txt');
    % fnb = fnblist(str2double(fn(end-1:end))).name;
    M = dlmread(fnb,',',1,0);
    b = M(:,9);
    time_rtwt = M(:,1) - M(1,1); % in sec % 20170208


    % check consistency of indices
    if abs(length(b) - length(laserON)) > 2
        error('Num mismatch between txt and video');
    elseif length(b) > length(laserON)
        b = b(1:length(laserON));
        time_rtwt = time_rtwt(1:length(laserON));
    elseif length(b) < length(laserON)
        laserON = laserON(1:length(b));
    end

    laser_b = laserON.*b;
    laser_b(laser_b == 0) = NaN;

    %% point out threshold points (not considering stimulation ready periods
    threshold_point = zeros(num_frames,1);
    i = 0;
    while(i <= num_frames - 5)
        i = i + 1;
        temp_theta = b(i:i+4);
        temp_delta = temp_theta(2:end) - temp_theta(1:end-1);
        if strcmp(direction,'protraction')
            if isempty(find(temp_delta > 0,1)) && (temp_theta(4) > threshold) && (temp_theta(5) <= threshold)
                threshold_point(i+4) = 1;
                i = i + 10; % 4 for calculating direction, 2 for laser duration, 4 for next direction calculation
            end
        elseif strcmp(direction,'retraction')
            if isempty(find(temp_delta < 0,1)) && (temp_theta(4) < threshold) && (temp_theta(5) >= threshold)
                threshold_point(i+4) = 1;
                i = i + 10;
            end
        else
            error('Wrong direction')
        end        
    end

    threshold_b = threshold_point .* b;
    threshold_b(threshold_point == 0) = NaN;

    %%
    figure, plot(time_rtwt,-b,'r.'), hold on, plot(time_rtwt,-laser_b,'b.'), plot(time_rtwt,-threshold_b,'g.'), xlabel('Time (ms)'), ylabel('Angle (degrees)'), ylim([-40 70])
    legend({'Theta','Laser','Passing points'})

    %%
    threshold_ind = find(~isnan(threshold_b));
    delay = zeros(length(threshold_ind),1);
    for i = 1 : length(threshold_ind)
        temp_laser_ind = threshold_ind(i);
        while(isnan(laser_b(temp_laser_ind)))
            temp_laser_ind = temp_laser_ind + 1;
            if temp_laser_ind > length(laser_b)
                temp_laser_ind = temp_laser_ind - 1;
                break
            end
        end
        delay(i) = M(temp_laser_ind,1) - M(threshold_ind(i),1);
    end

    %% 
    failure_ind = find(delay > (0.006 + delay_set * 0.002) | delay < (delay_set-1) * 0.001);
    success_ind = setdiff([1:length(threshold_ind)], failure_ind);
    failure_rate = (1 - length(success_ind)/length(delay))*100;
    delay_mean = mean(delay(success_ind));
    delay_std = std(delay(success_ind));
    %% save
    save([fn,'.mat'], 'b','laser_b','threshold_b','lasertest','failure_*', 'delay_*', 'success_ind')
end

%%
for ind = 1 : 7
    delay_set = 1;
    fnb = ['delay_', num2str(delay_set),'ms.txt'];
    M = dlmread(fnb,',',1,0);       
    fn = ['delay_', num2str(delay_set), 'ms.mat'];
    load(fn)
    threshold_ind = find(~isnan(threshold_b));
    delay = zeros(length(threshold_ind),1);
    for i = 1 : length(threshold_ind)
        temp_laser_ind = threshold_ind(i);
        while(isnan(laser_b(temp_laser_ind)))
            temp_laser_ind = temp_laser_ind + 1;
            if temp_laser_ind > length(laser_b)
                temp_laser_ind = temp_laser_ind - 1;
                break
            end
        end
        delay(i) = M(temp_laser_ind,1) - M(threshold_ind(i),1);
    end

    %% 
    failure_ind = find(delay > (0.006 + delay_set * 0.002) | delay < (delay_set-2) * 0.001);
    success_ind = setdiff([1:length(threshold_ind)], failure_ind);
    failure_rate = (1 - length(success_ind)/length(delay))*100;
    delay_mean = mean(delay(success_ind));
    delay_std = std(delay(success_ind));
    %% save
    save(fn, 'b','laser_b','threshold_b','lasertest','failure_rate', 'delay_*')
end

%%
delay_mean_total = zeros(7,1);
delay_std_total = zeros(7,1);
failure_rate_total = zeros(7,1);
for i = 1 : 7
    fn = ['delay_', num2str(i-1), 'ms.mat'];
    load(fn)
    delay_mean_total(i) = delay_mean - (i - 1)*0.001;
    delay_std_total(i) = delay_std;
    failure_rate_total(i) = failure_rate;
end
%% Plot
figure, errorbar(-1:6,[NaN; delay_mean_total*1000], [NaN; delay_std_total*1000],'k.-', 'LineWidth', 3, 'CapSize', 10,'MarkerSize', 10),
ax = gca; ax.LineWidth = 3; ax.FontWeight = 'bold'; ax.FontSize = 15; box off
xlabel('Time delay (ms)'), ylabel('Time to laser on (ms)'), xlim([-1 6])
ax.XTickLabel = {'', '0', '1', '2', '3', '4', '5', '6'};
