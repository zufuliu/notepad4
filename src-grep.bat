@rem see https://github.com/BurntSushi/ripgrep for anothor tool
@grep --exclude-dir=.git --exclude-dir=build --exclude-dir=doc --exclude-dir=res -nr %1
