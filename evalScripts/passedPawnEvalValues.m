firstValue = 7;
lastValue  = 91;
range = 7;
exponent   = 2;

% f(x) = ax^exponent+b

b = firstValue;

%a*(range-1)^exponent+b = 91


a = (91-b)/((range-1)^exponent);

inds = [0:range-1];
y    = a*inds.^exponent+b;

plot(inds,y, 'r-');



