classdef MotionModel
  properties (Constant)
    MM_TRANSLATION = 0
    MM_TRANSLATION_AND_SCALE = 1
    MM_ROTATION = 2
    MM_RIGID = 3
    MM_SIMILARITY = 4
    MM_AFFINE = 5
    MM_HOMOGRAPHY = 6
    MM_UNKNOWN = 7
  end
end
