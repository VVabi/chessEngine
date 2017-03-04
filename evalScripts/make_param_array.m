function out = make_param_array(exponent_raw, spread, mean_value, size, start_offset)

exponent = exponent_raw/100;

if(exponent < 0)
    exponent = 1/(-exponent);
end

total_size = size+start_offset;

in = [0:1/(total_size-1):1];

out = in.^exponent;

out = out(end-size+1:end);

in = in(end-size+1:end);

spr = out(end)-out(1);

out = spread/spr*out;

current_mean = mean(out);

offset = mean_value-current_mean;

out = out+offset;


out = round(out(end-size+1:end));
plot(in(end-size+1:end), out, 'r-');



end

