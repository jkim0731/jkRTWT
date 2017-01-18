fna = '20170117_213809.txt';
fid = fopen(fna);
ard_angle = [];
ard_angle_time  = [];
ard_str = {};
ard_str_time = [];
i = 0;
while 1
    c = fgetl(fid);
    if c == -1 | isempty(c)
        break
    end
    if length(c) > 2        
        temps = regexp(c,',','split');
        if isempty(str2num(temps{2}))
            i = i + 1;
            ard_str_time = [ard_str_time; str2num(temps{1})];
            ard_str{i} = temps{2};
        else
            ard_angle_time = [ard_angle_time; str2num(temps{1})];
            ard_angle = [ard_angle; str2num(temps{2})];
        end
    end
end
fclose(fid)

ard_angle_time = ard_angle_time - ard_angle_time(1);
ard_angle_time = ard_angle_time/1000000;
ard_str_time = ard_str_time - ard_angle_time(1);

fnb = 'Metadata _20170117_213818_1.txt';
M = dlmread(fnb,',',1,0);
bt = M(:,1);
bb = M(:,9);

figure, plot(bt, bb,'r.'), hold on, plot(ard_angle_time, ard_angle,'b.')
% 
% sizediff = length(a) - length(b);
% c = zeros(sizediff,1);
% for i = 1 : sizediff
%     c(i) = corr(a(i:i+length(b)-1),b);
% end
% 
% figure, plot(c)
% 
% [d,maxid] = max(c);
% maxid = maxid+1;
% 
% 
% figure, plot(a(maxid:end),'b.'), hold on, plot(b,'r.')
% 
% 
% for i = 1 : length(a)
%     if isnan(a(i))
%         i
%         a(i) = (a(i-1) + a(i+1))/2;
%     end
% end

