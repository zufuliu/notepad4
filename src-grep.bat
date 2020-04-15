@rem see https://github.com/BurntSushi/ripgrep
@rem or https://github.com/ggreer/the_silver_searcher
@rem for similar tools with colored output.
@grep --exclude-dir=.git --exclude-dir=build --exclude-dir=doc --exclude-dir=res -nr %1
