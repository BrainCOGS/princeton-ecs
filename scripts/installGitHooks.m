function installGitHooks(codeDir)
  
  %%
  origPath      = cd(codeDir);

  %%
  [status,info] = system('git rev-parse --git-dir');
  gitDir        = java.io.File(info);
  if ~gitDir.isAbsolute()
    gitDir      = java.io.File(fullfile(codeDir, info));
  end
  gitDir        = strtrim(char(gitDir.getAbsolutePath()));
  hooksDir      = fullfile(gitDir, 'hooks');
  sourceDir     = fullfile(gitDir, '..', 'git-hooks');
  
  if ~exist(hooksDir, 'dir')
    mkdir(hooksDir);
  end
  
  %%
  hooks         = rdir(sourceDir);
  for iHook = 1:numel(hooks)
    [~,name]    = parsePath(hooks(iHook).name);
    target      = fullfile(hooksDir, name);
    if ~exist(target, 'file')
      copyfile(hooks(iHook).name, target);
    end
  end
  
  %%
  versioning    = fullfile(gitDir, '..', 'version.txt');
  if ~exist(versioning, 'file')
    system(sprintf('git -C "%s" log -1 --pretty=oneline HEAD > %s', gitDir, versioning));
  end
  
  %%
  cd(origPath);
  
end

