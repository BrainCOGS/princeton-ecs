classdef InterpolationFlags
  properties (Constant)
    INTER_NEAREST = 0
    INTER_LINEAR = 1
    INTER_CUBIC = 2
    INTER_AREA = 3
    INTER_LANCZOS4 = 4
    INTER_MAX = 7
    WARP_FILL_OUTLIERS = 8
    WARP_INVERSE_MAP = 16
  end
end
