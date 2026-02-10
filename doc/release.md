> [!Note]
> [v25.07r5738](https://github.com/zufuliu/notepad4/releases/tag/v25.07r5738) and later no longer supports Windows XP, Windows Server 2003 (see issue #974) and Windows 8/8.1 on ARM32 (Windows RT) (see issue #1023), use [v25.06r5696](https://github.com/zufuliu/notepad4/releases/tag/v25.06r5696) instead.

## Changes Since
* Scintilla updated to 5, see https://www.scintilla.org/ScintillaHistory.html for the changes.
* Lexer and API updates: .
* Other bug fixes and improvements.

## Behavior Changes

## Breaking Changes

## File List
| Localization | Language | Architecture | Minimum System |
|--|--|--|--|
| de | Deutsch | ARM64 | 🟢Windows 10 on ARM |
| en | English | AVX512🧪 | 🟢64-bit Windows 10, Server 2019 |
| fr | French | AVX2 | 🟢64-bit Windows 7, Server 2008 R2 |
| it | Italiano | x64 | 🟡legacy 64-bit Windows Vista, Server 2008 |
| ja | 日本語 | Win32 | 🟠legacy Windows Vista, Server 2008 |
| ko | 한국어 |
| pl | Polski | HD | HiDPI, High Definition |
| ru | Русский |
| zh-Hans | 中文 (简体) | winget | `winget install zufuliu.notepad4.AVX2` |
| zh-Hant | 中文 (繁體) | winget | `winget install zufuliu.notepad4` |
| i18n | all above languages | winget | `winget install -e "Notepad4"` |
| 한국어 | https://teus.me/Notepad4 | Chocolatey | `choco install notepad4` |

Latest development builds (artifacts in Release configuration for each compiler and platform) are available at https://github.com/zufuliu/notepad4/actions and https://ci.appveyor.com/project/zufuliu/notepad4.

[FindInFiles](https://github.com/zufuliu/FindInFiles) (see issue #259) is a simple GUI for [ripgrep](https://github.com/BurntSushi/ripgrep/releases), latest builds are available at https://github.com/zufuliu/FindInFiles/actions.
| File | System Requirement |
| - | - |
| FindInFiles-net4.8 | [.NET Framework 4.8](https://dotnet.microsoft.com/en-us/download/dotnet-framework) prior Windows 10 |
| FindInFiles-net9.0 | [.NET Desktop Runtime 9.0](https://dotnet.microsoft.com/en-us/download/dotnet/9.0) |
| FindInFiles-net8.0 | [.NET Desktop Runtime 8.0](https://dotnet.microsoft.com/en-us/download/dotnet/8.0) |
