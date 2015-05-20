classdef Mode
  properties (Constant)
    READ = 0
    WRITE = 1
    APPEND = 2
    MEMORY = 4
    FORMAT_MASK = 4
    FORMAT_AUTO = 0
    FORMAT_XML = 6
    FORMAT_YAML = 7
  end
end
