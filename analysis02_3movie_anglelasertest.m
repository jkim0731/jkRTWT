%%
clear

load('data_4ms.mat', 'b', 'threshold_point')
b = -b;
range = 14000:15000;
v = VideoReader('Rawdata _20170110_154303_1.avi');
video = read(v,[min(range) max(range)]);
% video = squeeze(video(:,:,:,:));
timepoints = range - min(range);

%%
vwrite = VideoWriter('eg_4ms.avi');
vwrite.FrameRate = 5;
open(vwrite)
for i = 1 : length(range)
    tempim = video(:,:,:,i);
    postim = AddTextToImage(tempim, [num2str(i),' ms'], [5 20], [0.7 0.7 0.7], 'Arial', 20);
    if threshold_point(range(i)) == 1
        postim = AddTextToImage(postim, [num2str(b(range(i))), char(176)], [50 20], [1 0 0], 'Arial', 20);
    else
        postim = AddTextToImage(postim, [num2str(b(range(i))), char(176)], [50 20], [1 1 0], 'Arial', 20);
    end      
    writeVideo(vwrite,im2frame(postim));
end
close(vwrite)
%%
