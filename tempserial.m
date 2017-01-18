
if exist('s','var')
    fclose(s)
    delete(s)
    clear s
end
fnt = clock;
fn = '';
for i = 1 : length(fnt)
    temp = num2str(floor(fnt(i)));
    if length(temp) < 2
        temp = ['0',temp];
    end
    fn = [fn,temp];
end
fn = [fn(1:8), '_', fn(9:end),'.txt'];
fid = fopen(fn,'wt');

s = serial('COM8');
% theta = {};
% time = [];
set(s,'BaudRate',115200);
fopen(s);

while(1)
    temp = fgetl(s);    
%     if (i > 1000)
%         timediff = toc - time;
%         if (timediff > 5)
%             break
%         end
%     end
%     time = toc;
%     theta{i} = temp;    
%     time = [time;toc];
    fprintf(fid, '%s\n', temp);   
end


%%
fclose(s)
delete(s)
clear s
fclose(fid);