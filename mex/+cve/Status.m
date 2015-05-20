classdef Status
  properties (Constant)
    OK = 0
    ERR_NEED_MORE_IMGS = 1
    ERR_HOMOGRAPHY_EST_FAIL = 2
    ERR_CAMERA_PARAMS_ADJUST_FAIL = 3
  end
end
