function [val] = randomiser (min, max)
    val = rand(1) * (max - min) + min;