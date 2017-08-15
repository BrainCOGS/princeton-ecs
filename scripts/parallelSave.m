%% save() functionality for parallel computing. N.B. All arguments must be Matlab variables, e.g. an array element x(i) will break this code!
function parallelSave(filename, varargin)

  %% Make a backup if the target exists
  if iscell(filename)
    backupFile        = filename{2};
    filename          = filename{1};
    
    if exist(filename, 'file')
      if exist(backupFile, 'file')
        delete(backupFile);
      end
      [status,msg]    = movefile(filename, backupFile);
      if ~status
        error('parallelSave:backupFile', msg);
      end
    end
  end
  
  %% Save all specified variables with their names as passed to this function
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
