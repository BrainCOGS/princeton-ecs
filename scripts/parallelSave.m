%% save() functionality for parallel computing. N.B. All arguments must be Matlab variables, e.g. an array element x(i) will break this code!
function parsave(filename, varargin)

  options             = {};
  what                = struct();
  for iArg = 1:numel(varargin)
    if ischar(varargin{iArg}) && ~isempty(varargin{iArg}) && varargin{iArg}(1) == '-'
      options{end+1}  = varargin{iArg};
    else
      what.(inputname(iArg + 1))  = varargin{iArg};
    end
  end
  
  save(filename, options{:}, '-struct', 'what');
  
end
