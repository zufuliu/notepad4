# 7.2 https://github.com/PowerShell/PowerShell
# https://docs.microsoft.com/en-us/powershell/
# Windows PowerShell Language Specification 3.0
# https://docs.microsoft.com/en-us/powershell/scripting/lang-spec/chapter-01
# https://docs.microsoft.com/en-us/powershell/module/microsoft.powershell.core/about/about

#! Keywords				=======================================================
# https://docs.microsoft.com/en-us/powershell/module/microsoft.powershell.core/about/about_language_keywords
# https://docs.microsoft.com/en-us/powershell/scripting/lang-spec/chapter-02#231-keywords
begin break
catch class continue
data define do dynamicparam dynamicParam
else elseif end enum exit
filter finally for foreach from function
hidden
if in
param process
return
static switch
throw trap try
until using
var
while

# workflow
inlinescript inlineScript
parallel
sequence
workflow

# https://docs.microsoft.com/en-us/powershell/module/microsoft.powershell.core/about/about_reserved_words
assembly
base
command configuration
interface
module
namespace
private public
type

#! type					=======================================================
# https://docs.microsoft.com/en-us/powershell/scripting/lang-spec/chapter-04
[math]
[ordered]
# https://docs.microsoft.com/en-us/powershell/module/microsoft.powershell.core/about/about_type_accelerators
[adsi]
[array]
[bigint]
[bool]
[byte]
[char]
[cimclass]
[datetime]
[decimal]
[double]
[float]
[guid]
[hashtable]
[int]
[int16]
[int32]
[int64]
[ipaddress]
[long]
[mailaddress]
[psobject]
[psvariable]
[ref]
[regex]
[runspace]
[sbyte]
[scriptblock]
[short]
[single]
[string]
[switch]
[timespan]
[type]
[uint]
[uint16]
[uint32]
[uint64]
[ulong]
[uri]
[ushort]
[version]
[void]
[wmi]
[wmiclass]
[xml]

#! Variables			=======================================================
$Function
$Alias
$Variable
# Get-Variable
# Automatic Variables
# https://docs.microsoft.com/en-us/powershell/module/microsoft.powershell.core/about/about_automatic_variables
#$$ $? $^ $_
$args
$ConsoleFileName
$Error
$ErrorView
$Event
$EventArgs
$EventSubscriber
$ExecutionContext
$false
$foreach
$HOME
$Host
$input
$IsCoreCLR
$IsLinux
$IsMacOS
$IsWindows
$LastExitCode
$Matches
$MyInvocation
$NestedPromptLevel
$null
$PID
$PROFILE
$PSBoundParameters
$PSCmdlet
$PSCommandPath
$PSCulture
$PSDebugContext
$PSHOME
$PSItem
$PSNativeCommandArgumentPassing
$PSScriptRoot
$PSSenderInfo
$PSStyle
$PSUICulture
$PSVersionTable
$PWD
$Sender
$ShellId
$StackTrace
$switch
$this
$true

# https://docs.microsoft.com/en-us/powershell/module/microsoft.powershell.core/about/about_language_modes
$PSScriptRoot
$PSEdition
$EnabledExperimentalFeatures

# Environment Variables
# https://docs.microsoft.com/en-us/powershell/module/microsoft.powershell.core/about/about_environment_variables
$Env

# Preference Variables
# https://docs.microsoft.com/en-us/powershell/module/microsoft.powershell.core/about/about_preference_variables
$ConfirmPreference
$DebugPreference
$ErrorActionPreference
$ErrorView
$FormatEnumerationLimit
$InformationPreference
$LogCommandHealthEvent
$LogCommandLifecycleEvent
$LogEngineHealthEvent
$LogEngineLifecycleEvent
$LogProviderLifecycleEvent
$LogProviderHealthEvent
$MaximumHistoryCount
$OFS
$OutputEncoding
$ProgressPreference
$PSDefaultParameterValues
$PSEmailServer
$PSModuleAutoLoadingPreference
$PSSessionApplicationName
$PSSessionConfigurationName
$PSSessionOption
$Transcript
$VerbosePreference
$WarningPreference
$WhatIfPreference

#! cmdlet 				=======================================================
# Get-Command
{ # CimCmdlets
Get-CimAssociatedInstance [[-Association] <String>] [-ResultClassName <String>] [-InputObject] <CimInstance> [-Namespace <String>] [-OperationTimeoutSec <UInt32>] [-ResourceUri <Uri>] [-ComputerName <String[]>] [-KeyOnly] [<CommonParameters>]
Get-CimAssociatedInstance [[-Association] <String>] [-ResultClassName <String>] [-InputObject] <CimInstance> [-Namespace <String>] [-OperationTimeoutSec <UInt32>] [-ResourceUri <Uri>] -CimSession <CimSession[]> [-KeyOnly] [<CommonParameters>]
Get-CimClass [[-ClassName] <String>] [[-Namespace] <String>] [-OperationTimeoutSec <UInt32>] [-ComputerName <String[]>] [-MethodName <String>] [-PropertyName <String>] [-QualifierName <String>] [<CommonParameters>]
Get-CimClass [[-ClassName] <String>] [[-Namespace] <String>] [-OperationTimeoutSec <UInt32>] -CimSession <CimSession[]> [-MethodName <String>] [-PropertyName <String>] [-QualifierName <String>] [<CommonParameters>]
Get-CimInstance [-ClassName] <String> [-ComputerName <String[]>] [-KeyOnly] [-Namespace <String>] [-OperationTimeoutSec <UInt32>] [-QueryDialect <String>] [-Shallow] [-Filter <String>] [-Property <String[]>] [<CommonParameters>]
Get-CimInstance -CimSession <CimSession[]> -ResourceUri <Uri> [-KeyOnly] [-Namespace <String>] [-OperationTimeoutSec <UInt32>] [-Shallow] [-Filter <String>] [-Property <String[]>] [<CommonParameters>]
Get-CimInstance -CimSession <CimSession[]> [-ResourceUri <Uri>] [-Namespace <String>] [-OperationTimeoutSec <UInt32>] -Query <String> [-QueryDialect <String>] [-Shallow] [<CommonParameters>]
Get-CimInstance -CimSession <CimSession[]> [-ClassName] <String> [-KeyOnly] [-Namespace <String>] [-OperationTimeoutSec <UInt32>] [-QueryDialect <String>] [-Shallow] [-Filter <String>] [-Property <String[]>] [<CommonParameters>]
Get-CimInstance -CimSession <CimSession[]> [-ResourceUri <Uri>] [-OperationTimeoutSec <UInt32>] [-InputObject] <CimInstance> [<CommonParameters>]
Get-CimInstance [-ResourceUri <Uri>] [-ComputerName <String[]>] [-OperationTimeoutSec <UInt32>] [-InputObject] <CimInstance> [<CommonParameters>]
Get-CimInstance -ResourceUri <Uri> [-ComputerName <String[]>] [-KeyOnly] [-Namespace <String>] [-OperationTimeoutSec <UInt32>] [-Shallow] [-Filter <String>] [-Property <String[]>] [<CommonParameters>]
Get-CimInstance [-ResourceUri <Uri>] [-ComputerName <String[]>] [-Namespace <String>] [-OperationTimeoutSec <UInt32>] -Query <String> [-QueryDialect <String>] [-Shallow] [<CommonParameters>]
Get-CimSession [[-ComputerName] <String[]>] [<CommonParameters>]
Get-CimSession [-Id] <UInt32[]> [<CommonParameters>]
Get-CimSession -InstanceId <Guid[]> [<CommonParameters>]
Get-CimSession -Name <String[]> [<CommonParameters>]
Import-BinaryMiLog [-Path] <String> [<CommonParameters>]
Invoke-CimMethod [-ClassName] <String> [-ComputerName <String[]>] [[-Arguments] <IDictionary>] [-MethodName] <String> [-Namespace <String>] [-OperationTimeoutSec <UInt32>] [-WhatIf] [-Confirm] [<CommonParameters>]
Invoke-CimMethod [-ClassName] <String> -CimSession <CimSession[]> [[-Arguments] <IDictionary>] [-MethodName] <String> [-Namespace <String>] [-OperationTimeoutSec <UInt32>] [-WhatIf] [-Confirm] [<CommonParameters>]
Invoke-CimMethod -ResourceUri <Uri> [-ComputerName <String[]>] [[-Arguments] <IDictionary>] [-MethodName] <String> [-Namespace <String>] [-OperationTimeoutSec <UInt32>] [-WhatIf] [-Confirm] [<CommonParameters>]
Invoke-CimMethod [-ResourceUri <Uri>] [-InputObject] <CimInstance> -CimSession <CimSession[]> [[-Arguments] <IDictionary>] [-MethodName] <String> [-OperationTimeoutSec <UInt32>] [-WhatIf] [-Confirm] [<CommonParameters>]
Invoke-CimMethod [-ResourceUri <Uri>] [-InputObject] <CimInstance> [-ComputerName <String[]>] [[-Arguments] <IDictionary>] [-MethodName] <String> [-OperationTimeoutSec <UInt32>] [-WhatIf] [-Confirm] [<CommonParameters>]
Invoke-CimMethod -ResourceUri <Uri> -CimSession <CimSession[]> [[-Arguments] <IDictionary>] [-MethodName] <String> [-Namespace <String>] [-OperationTimeoutSec <UInt32>] [-WhatIf] [-Confirm] [<CommonParameters>]
Invoke-CimMethod [-CimClass] <CimClass> [-ComputerName <String[]>] [[-Arguments] <IDictionary>] [-MethodName] <String> [-OperationTimeoutSec <UInt32>] [-WhatIf] [-Confirm] [<CommonParameters>]
Invoke-CimMethod [-CimClass] <CimClass> -CimSession <CimSession[]> [[-Arguments] <IDictionary>] [-MethodName] <String> [-OperationTimeoutSec <UInt32>] [-WhatIf] [-Confirm] [<CommonParameters>]
Invoke-CimMethod -Query <String> [-QueryDialect <String>] [-ComputerName <String[]>] [[-Arguments] <IDictionary>] [-MethodName] <String> [-Namespace <String>] [-OperationTimeoutSec <UInt32>] [-WhatIf] [-Confirm] [<CommonParameters>]
Invoke-CimMethod -Query <String> [-QueryDialect <String>] -CimSession <CimSession[]> [[-Arguments] <IDictionary>] [-MethodName] <String> [-Namespace <String>] [-OperationTimeoutSec <UInt32>] [-WhatIf] [-Confirm] [<CommonParameters>]
New-CimInstance [-ClassName] <String> [-Key <String[]>] [[-Property] <IDictionary>] [-Namespace <String>] [-OperationTimeoutSec <UInt32>] [-ComputerName <String[]>] [-ClientOnly] [-WhatIf] [-Confirm] [<CommonParameters>]
New-CimInstance [-ClassName] <String> [-Key <String[]>] [[-Property] <IDictionary>] [-Namespace <String>] [-OperationTimeoutSec <UInt32>] -CimSession <CimSession[]> [-ClientOnly] [-WhatIf] [-Confirm] [<CommonParameters>]
New-CimInstance -ResourceUri <Uri> [-Key <String[]>] [[-Property] <IDictionary>] [-Namespace <String>] [-OperationTimeoutSec <UInt32>] -CimSession <CimSession[]> [-WhatIf] [-Confirm] [<CommonParameters>]
New-CimInstance -ResourceUri <Uri> [-Key <String[]>] [[-Property] <IDictionary>] [-Namespace <String>] [-OperationTimeoutSec <UInt32>] [-ComputerName <String[]>] [-WhatIf] [-Confirm] [<CommonParameters>]
New-CimInstance [-CimClass] <CimClass> [[-Property] <IDictionary>] [-OperationTimeoutSec <UInt32>] -CimSession <CimSession[]> [-ClientOnly] [-WhatIf] [-Confirm] [<CommonParameters>]
New-CimInstance [-CimClass] <CimClass> [[-Property] <IDictionary>] [-OperationTimeoutSec <UInt32>] [-ComputerName <String[]>] [-ClientOnly] [-WhatIf] [-Confirm] [<CommonParameters>]
New-CimSession [-Authentication <PasswordAuthenticationMechanism>] [[-Credential] <PSCredential>] [[-ComputerName] <String[]>] [-Name <String>] [-OperationTimeoutSec <UInt32>] [-SkipTestConnection] [-Port <UInt32>] [-SessionOption <CimSessionOptions>] [<CommonParameters>]
New-CimSession [-CertificateThumbprint <String>] [[-ComputerName] <String[]>] [-Name <String>] [-OperationTimeoutSec <UInt32>] [-SkipTestConnection] [-Port <UInt32>] [-SessionOption <CimSessionOptions>] [<CommonParameters>]
New-CimSessionOption [-Protocol] <ProtocolType> [-UICulture <CultureInfo>] [-Culture <CultureInfo>] [<CommonParameters>]
New-CimSessionOption [-NoEncryption] [-SkipCACheck] [-SkipCNCheck] [-SkipRevocationCheck] [-EncodePortInServicePrincipalName] [-Encoding <PacketEncoding>] [-HttpPrefix <Uri>] [-MaxEnvelopeSizeKB <UInt32>] [-ProxyAuthentication <PasswordAuthenticationMechanism>] [-ProxyCertificateThumbprint <String>] [-ProxyCredential <PSCredential>] [-ProxyType <ProxyType>] [-UseSsl] [-UICulture <CultureInfo>] [-Culture <CultureInfo>] [<CommonParameters>]
New-CimSessionOption [-Impersonation <ImpersonationType>] [-PacketIntegrity] [-PacketPrivacy] [-UICulture <CultureInfo>] [-Culture <CultureInfo>] [<CommonParameters>]
Register-CimIndicationEvent [-Namespace <String>] [-ClassName] <String> [-OperationTimeoutSec <UInt32>] [-ComputerName <String>] [[-SourceIdentifier] <String>] [[-Action] <ScriptBlock>] [-MessageData <PSObject>] [-SupportEvent] [-Forward] [-MaxTriggerCount <Int32>] [<CommonParameters>]
Register-CimIndicationEvent [-Namespace <String>] [-ClassName] <String> [-OperationTimeoutSec <UInt32>] -CimSession <CimSession> [[-SourceIdentifier] <String>] [[-Action] <ScriptBlock>] [-MessageData <PSObject>] [-SupportEvent] [-Forward] [-MaxTriggerCount <Int32>] [<CommonParameters>]
Register-CimIndicationEvent [-Namespace <String>] [-Query] <String> [-QueryDialect <String>] [-OperationTimeoutSec <UInt32>] -CimSession <CimSession> [[-SourceIdentifier] <String>] [[-Action] <ScriptBlock>] [-MessageData <PSObject>] [-SupportEvent] [-Forward] [-MaxTriggerCount <Int32>] [<CommonParameters>]
Register-CimIndicationEvent [-Namespace <String>] [-Query] <String> [-QueryDialect <String>] [-OperationTimeoutSec <UInt32>] [-ComputerName <String>] [[-SourceIdentifier] <String>] [[-Action] <ScriptBlock>] [-MessageData <PSObject>] [-SupportEvent] [-Forward] [-MaxTriggerCount <Int32>] [<CommonParameters>]
Remove-CimInstance [-ResourceUri <Uri>] [-ComputerName <String[]>] [-OperationTimeoutSec <UInt32>] [-InputObject] <CimInstance> [-WhatIf] [-Confirm] [<CommonParameters>]
Remove-CimInstance -CimSession <CimSession[]> [-ResourceUri <Uri>] [-OperationTimeoutSec <UInt32>] [-InputObject] <CimInstance> [-WhatIf] [-Confirm] [<CommonParameters>]
Remove-CimInstance -CimSession <CimSession[]> [[-Namespace] <String>] [-OperationTimeoutSec <UInt32>] [-Query] <String> [-QueryDialect <String>] [-WhatIf] [-Confirm] [<CommonParameters>]
Remove-CimInstance [-ComputerName <String[]>] [[-Namespace] <String>] [-OperationTimeoutSec <UInt32>] [-Query] <String> [-QueryDialect <String>] [-WhatIf] [-Confirm] [<CommonParameters>]
Remove-CimSession [-CimSession] <CimSession[]> [-WhatIf] [-Confirm] [<CommonParameters>]
Remove-CimSession [-ComputerName] <String[]> [-WhatIf] [-Confirm] [<CommonParameters>]
Remove-CimSession [-Id] <UInt32[]> [-WhatIf] [-Confirm] [<CommonParameters>]
Remove-CimSession -InstanceId <Guid[]> [-WhatIf] [-Confirm] [<CommonParameters>]
Remove-CimSession -Name <String[]> [-WhatIf] [-Confirm] [<CommonParameters>]
Set-CimInstance [-ComputerName <String[]>] [-ResourceUri <Uri>] [-OperationTimeoutSec <UInt32>] [-InputObject] <CimInstance> [-Property <IDictionary>] [-PassThru] [-WhatIf] [-Confirm] [<CommonParameters>]
Set-CimInstance -CimSession <CimSession[]> [-ResourceUri <Uri>] [-OperationTimeoutSec <UInt32>] [-InputObject] <CimInstance> [-Property <IDictionary>] [-PassThru] [-WhatIf] [-Confirm] [<CommonParameters>]
Set-CimInstance -CimSession <CimSession[]> [-Namespace <String>] [-OperationTimeoutSec <UInt32>] [-Query] <String> [-QueryDialect <String>] -Property <IDictionary> [-PassThru] [-WhatIf] [-Confirm] [<CommonParameters>]
Set-CimInstance [-ComputerName <String[]>] [-Namespace <String>] [-OperationTimeoutSec <UInt32>] [-Query] <String> [-QueryDialect <String>] -Property <IDictionary> [-PassThru] [-WhatIf] [-Confirm] [<CommonParameters>]
}

{ # Microsoft.PowerShell.Archive
Compress-Archive [-Path] <String[]> [-DestinationPath] <String> [-CompressionLevel <String>] [-PassThru] [-WhatIf] [-Confirm] [<CommonParameters>]
Compress-Archive [-Path] <String[]> [-DestinationPath] <String> [-CompressionLevel <String>] -Update [-PassThru] [-WhatIf] [-Confirm] [<CommonParameters>]
Compress-Archive [-Path] <String[]> [-DestinationPath] <String> [-CompressionLevel <String>] -Force [-PassThru] [-WhatIf] [-Confirm] [<CommonParameters>]
Compress-Archive -LiteralPath <String[]> [-DestinationPath] <String> [-CompressionLevel <String>] -Update [-PassThru] [-WhatIf] [-Confirm] [<CommonParameters>]
Compress-Archive -LiteralPath <String[]> [-DestinationPath] <String> [-CompressionLevel <String>] -Force [-PassThru] [-WhatIf] [-Confirm] [<CommonParameters>]
Compress-Archive -LiteralPath <String[]> [-DestinationPath] <String> [-CompressionLevel <String>] [-PassThru] [-WhatIf] [-Confirm] [<CommonParameters>]
Expand-Archive [-Path] <String> [[-DestinationPath] <String>] [-Force] [-PassThru] [-WhatIf] [-Confirm] [<CommonParameters>]
Expand-Archive -LiteralPath <String> [[-DestinationPath] <String>] [-Force] [-PassThru] [-WhatIf] [-Confirm] [<CommonParameters>]
}

{ # Microsoft.PowerShell.Core
Add-History [[-InputObject] <PSObject[]>] [-Passthru] [<CommonParameters>]
Clear-History [[-Id] <int[]>] [[-Count] <int>] [-Newest] [-WhatIf] [-Confirm] [<CommonParameters>]
Clear-History [[-Count] <int>] [-CommandLine <string[]>] [-Newest] [-WhatIf] [-Confirm] [<CommonParameters>]
Clear-Host []
Connect-PSSession -Name <String[]> [-ThrottleLimit <Int32>] [-WhatIf] [-Confirm] [<CommonParameters>]
Connect-PSSession [-Session] <PSSession[]> [-ThrottleLimit <Int32>] [-WhatIf] [-Confirm] [<CommonParameters>]
Connect-PSSession -ComputerName <String[]> [-ApplicationName <String>] [-ConfigurationName <String>] -InstanceId <Guid[]> [-Credential <PSCredential>] [-Authentication <AuthenticationMechanism>] [-CertificateThumbprint <String>] [-Port <Int32>] [-UseSSL] [-SessionOption <PSSessionOption>] [-ThrottleLimit <Int32>] [-WhatIf] [-Confirm] [<CommonParameters>]
Connect-PSSession -ComputerName <String[]> [-ApplicationName <String>] [-ConfigurationName <String>] [-Name <String[]>] [-Credential <PSCredential>] [-Authentication <AuthenticationMechanism>] [-CertificateThumbprint <String>] [-Port <Int32>] [-UseSSL] [-SessionOption <PSSessionOption>] [-ThrottleLimit <Int32>] [-WhatIf] [-Confirm] [<CommonParameters>]
Connect-PSSession [-ConfigurationName <String>] [-ConnectionUri] <Uri[]> [-AllowRedirection] -InstanceId <Guid[]> [-Credential <PSCredential>] [-Authentication <AuthenticationMechanism>] [-CertificateThumbprint <String>] [-SessionOption <PSSessionOption>] [-ThrottleLimit <Int32>] [-WhatIf] [-Confirm] [<CommonParameters>]
Connect-PSSession -InstanceId <Guid[]> [-ThrottleLimit <Int32>] [-WhatIf] [-Confirm] [<CommonParameters>]
Connect-PSSession [-ThrottleLimit <Int32>] [-Id] <Int32[]> [-WhatIf] [-Confirm] [<CommonParameters>]
Debug-Job [-Job] <Job> [-BreakAll] [-WhatIf] [-Confirm] [<CommonParameters>]
Debug-Job [-Name] <String> [-BreakAll] [-WhatIf] [-Confirm] [<CommonParameters>]
Debug-Job [-Id] <Int32> [-BreakAll] [-WhatIf] [-Confirm] [<CommonParameters>]
Debug-Job [-InstanceId] <Guid> [-BreakAll] [-WhatIf] [-Confirm] [<CommonParameters>]
Disable-ExperimentalFeature [-Name] <String[]> [-Scope <ConfigScope>] [-WhatIf] [-Confirm] [<CommonParameters>]
Disable-PSRemoting [-Force] [-WhatIf] [-Confirm] [<CommonParameters>]
Disable-PSSessionConfiguration [[-Name] <String[]>] [-Force] [-NoServiceRestart] [-WhatIf] [-Confirm] [<CommonParameters>]
Disconnect-PSSession [-Session] <PSSession[]> [-IdleTimeoutSec <Int32>] [-OutputBufferingMode <OutputBufferingMode>] [-ThrottleLimit <Int32>] [-WhatIf] [-Confirm] [<CommonParameters>]
Disconnect-PSSession [-IdleTimeoutSec <Int32>] [-OutputBufferingMode <OutputBufferingMode>] [-ThrottleLimit <Int32>] -Name <String[]> [-WhatIf] [-Confirm] [<CommonParameters>]
Disconnect-PSSession [-IdleTimeoutSec <Int32>] [-OutputBufferingMode <OutputBufferingMode>] [-ThrottleLimit <Int32>] -InstanceId <Guid[]> [-WhatIf] [-Confirm] [<CommonParameters>]
Disconnect-PSSession [-IdleTimeoutSec <Int32>] [-OutputBufferingMode <OutputBufferingMode>] [-ThrottleLimit <Int32>] [-Id] <Int32[]> [-WhatIf] [-Confirm] [<CommonParameters>]
Enable-ExperimentalFeature [-Name] <String[]> [-Scope <ConfigScope>] [-WhatIf] [-Confirm] [<CommonParameters>]
Enable-PSRemoting [-Force] [-SkipNetworkProfileCheck] [-WhatIf] [-Confirm] [<CommonParameters>]
Enable-PSSessionConfiguration [[-Name] <String[]>] [-Force] [-SecurityDescriptorSddl <String>] [-SkipNetworkProfileCheck] [-NoServiceRestart] [-WhatIf] [-Confirm] [<CommonParameters>]
Enter-PSHostProcess [-Id] <Int32> [[-AppDomainName] <String>] [<CommonParameters>]
Enter-PSHostProcess [-Process] <Process> [[-AppDomainName] <String>] [<CommonParameters>]
Enter-PSHostProcess [-Name] <String> [[-AppDomainName] <String>] [<CommonParameters>]
Enter-PSHostProcess [-HostProcessInfo] <PSHostProcessInfo> [[-AppDomainName] <String>] [<CommonParameters>]
Enter-PSHostProcess -CustomPipeName <String> [<CommonParameters>]
Enter-PSSession [-ComputerName] <String> [-EnableNetworkAccess] [[-Credential] <PSCredential>] [-ConfigurationName <String>] [-Port <Int32>] [-UseSSL] [-ApplicationName <String>] [-SessionOption <PSSessionOption>] [-Authentication <AuthenticationMechanism>] [-CertificateThumbprint <String>] [<CommonParameters>]
Enter-PSSession [-HostName] <String> [-Port <Int32>] [-UserName <String>] [-KeyFilePath <String>] [-SSHTransport] [-ConnectingTimeout <int>] [<CommonParameters>]
Enter-PSSession [[-Session] <PSSession>] [<CommonParameters>]
Enter-PSSession [[-ConnectionUri] <Uri>] [-EnableNetworkAccess] [[-Credential] <PSCredential>] [-ConfigurationName <String>] [-AllowRedirection] [-SessionOption <PSSessionOption>] [-Authentication <AuthenticationMechanism>] [-CertificateThumbprint <String>] [<CommonParameters>]
Enter-PSSession [-InstanceId <Guid>] [<CommonParameters>]
Enter-PSSession [[-Id] <Int32>] [<CommonParameters>]
Enter-PSSession [-Name <String>] [<CommonParameters>]
Enter-PSSession [-VMId] <Guid> [-Credential] <PSCredential> [-ConfigurationName <String>] [<CommonParameters>]
Enter-PSSession [-VMName] <String> [-Credential] <PSCredential> [-ConfigurationName <String>] [<CommonParameters>]
Enter-PSSession [-ContainerId] <String> [-ConfigurationName <String>] [-RunAsAdministrator] [<CommonParameters>] Exit-PSHostProcess []
Exit-PSSession []
Exit-PSHostProcess []
Export-ModuleMember [[-Function] <String[]>] [-Cmdlet <String[]>] [-Variable <String[]>] [-Alias <String[]>] [<CommonParameters>]
ForEach-Object [-InputObject <PSObject>] [-Begin <ScriptBlock>] [-Process] <ScriptBlock[]> [-End <ScriptBlock>] [-RemainingScripts <ScriptBlock[]>] [-WhatIf] [-Confirm] [<CommonParameters>]
ForEach-Object [-InputObject <PSObject>] [-MemberName] <String> [-ArgumentList <Object[]>] [-WhatIf] [-Confirm] [<CommonParameters>]
ForEach-Object [-InputObject <PSObject>] -Parallel <ScriptBlock> [-ThrottleLimit <Int32>] [-TimeoutSeconds <Int32>] [-AsJob] [-WhatIf] [-Confirm] [<CommonParameters>]
Get-Command [-Verb <String[]>] [-Noun <String[]>] [-Module <String[]>] [-FullyQualifiedModule <ModuleSpecification[]>] [-TotalCount <Int32>] [-Syntax] [-ShowCommandInfo] [[-ArgumentList] <Object[]>] [-All] [-ListImported] [-ParameterName <String[]>] [-ParameterType <PSTypeName[]>] [<CommonParameters>]
Get-Command [[-Name] <String[]>] [-Module <String[]>] [-FullyQualifiedModule <ModuleSpecification[]>] [-CommandType <CommandTypes>] [-TotalCount <Int32>] [-Syntax] [-ShowCommandInfo] [[-ArgumentList] <Object[]>] [-All] [-ListImported] [-ParameterName <String[]>] [-ParameterType <PSTypeName[]>] [-UseFuzzyMatching] [-UseAbbreviationExpansion] [<CommonParameters>]
Get-ExperimentalFeature [[-Name] <String[]>] [<CommonParameters>]
Get-Help [[-Name] <String>] [-Path <String>] [-Category <String[]>] [-Full] [-Component <String[]>] [-Functionality <String[]>] [-Role <String[]>] [<CommonParameters>]
Get-Help [[-Name] <String>] [-Path <String>] [-Category <String[]>] -Detailed [-Component <String[]>] [-Functionality <String[]>] [-Role <String[]>] [<CommonParameters>]
Get-Help [[-Name] <String>] [-Path <String>] [-Category <String[]>] -Examples [-Component <String[]>] [-Functionality <String[]>] [-Role <String[]>] [<CommonParameters>]
Get-Help [[-Name] <String>] [-Path <String>] [-Category <String[]>] -Parameter <String[]> [-Component <String[]>] [-Functionality <String[]>] [-Role <String[]>] [<CommonParameters>]
Get-Help [[-Name] <String>] [-Path <String>] [-Category <String[]>] [-Component <String[]>] [-Functionality <String[]>] [-Role <String[]>] -Online [<CommonParameters>]
Get-Help [[-Name] <String>] [-Path <String>] [-Category <String[]>] [-Component <String[]>] [-Functionality <String[]>] [-Role <String[]>] -ShowWindow [<CommonParameters>]
Get-History [[-Id] <Int64[]>] [[-Count] <Int32>] [<CommonParameters>]
Get-Job [-IncludeChildJob] [-ChildJobState <JobState>] [-HasMoreData <Boolean>] [-Before <DateTime>] [-After <DateTime>] [-Newest <Int32>] [[-Id] <Int32[]>] [<CommonParameters>]
Get-Job [-IncludeChildJob] [-ChildJobState <JobState>] [-HasMoreData <Boolean>] [-Before <DateTime>] [-After <DateTime>] [-Newest <Int32>] [-InstanceId] <Guid[]> [<CommonParameters>]
Get-Job [-IncludeChildJob] [-ChildJobState <JobState>] [-HasMoreData <Boolean>] [-Before <DateTime>] [-After <DateTime>] [-Newest <Int32>] [-Name] <String[]> [<CommonParameters>]
Get-Job [-IncludeChildJob] [-ChildJobState <JobState>] [-HasMoreData <Boolean>] [-Before <DateTime>] [-After <DateTime>] [-Newest <Int32>] [-State] <JobState> [<CommonParameters>]
Get-Job [-IncludeChildJob] [-ChildJobState <JobState>] [-HasMoreData <Boolean>] [-Before <DateTime>] [-After <DateTime>] [-Newest <Int32>] [-Command <String[]>] [<CommonParameters>]
Get-Job [-Filter] <Hashtable> [<CommonParameters>]
Get-Module [[-Name] <String[]>] [-FullyQualifiedName <ModuleSpecification[]>] [-All] [<CommonParameters>]
Get-Module [[-Name] <String[]>] [-FullyQualifiedName <ModuleSpecification[]>] [-All] [-ListAvailable] [-PSEdition <String>] [-SkipEditionCheck] [-Refresh] [<CommonParameters>]
Get-Module [[-Name] <String[]>] [-FullyQualifiedName <ModuleSpecification[]>] [-ListAvailable] [-PSEdition <String>] [-SkipEditionCheck] [-Refresh] -PSSession <PSSession> [<CommonParameters>]
Get-Module [[-Name] <String[]>] [-FullyQualifiedName <ModuleSpecification[]>] [-ListAvailable] [-SkipEditionCheck] [-Refresh] -CimSession <CimSession> [-CimResourceUri <Uri>] [-CimNamespace <String>] [<CommonParameters>]
Get-PSHostProcessInfo [[-Name] <String[]>] [<CommonParameters>]
Get-PSHostProcessInfo [-Process] <Process[]> [<CommonParameters>]
Get-PSHostProcessInfo [-Id] <Int32[]> [<CommonParameters>]
Get-PSSession [-Name <String[]>] [<CommonParameters>]
Get-PSSession [-ComputerName] <String[]> [-ApplicationName <String>] [-ConfigurationName <String>] [-Name <String[]>] [-Credential <PSCredential>] [-Authentication <AuthenticationMechanism>] [-CertificateThumbprint <String>] [-Port <Int32>] [-UseSSL] [-ThrottleLimit <Int32>] [-State <SessionFilterState>] [-SessionOption <PSSessionOption>] [<CommonParameters>]
Get-PSSession [-ComputerName] <String[]> [-ApplicationName <String>] [-ConfigurationName <String>] -InstanceId <Guid[]> [-Credential <PSCredential>] [-Authentication <AuthenticationMechanism>] [-CertificateThumbprint <String>] [-Port <Int32>] [-UseSSL] [-ThrottleLimit <Int32>] [-State <SessionFilterState>] [-SessionOption <PSSessionOption>] [<CommonParameters>]
Get-PSSession [-ConnectionUri] <Uri[]> [-ConfigurationName <String>] [-AllowRedirection] [-Name <String[]>] [-Credential <PSCredential>] [-Authentication <AuthenticationMechanism>] [-CertificateThumbprint <String>] [-ThrottleLimit <Int32>] [-State <SessionFilterState>] [-SessionOption <PSSessionOption>] [<CommonParameters>]
Get-PSSession [-ConnectionUri] <Uri[]> [-ConfigurationName <String>] [-AllowRedirection] -InstanceId <Guid[]> [-Credential <PSCredential>] [-Authentication <AuthenticationMechanism>] [-CertificateThumbprint <String>] [-ThrottleLimit <Int32>] [-State <SessionFilterState>] [-SessionOption <PSSessionOption>] [<CommonParameters>]
Get-PSSession [-ConfigurationName <String>] -InstanceId <Guid[]> [-State <SessionFilterState>] -VMName <String[]> [<CommonParameters>]
Get-PSSession [-ConfigurationName <String>] [-Name <String[]>] [-State <SessionFilterState>] -ContainerId <String[]> [<CommonParameters>]
Get-PSSession [-ConfigurationName <String>] -InstanceId <Guid[]> [-State <SessionFilterState>] -ContainerId <String[]> [<CommonParameters>]
Get-PSSession [-ConfigurationName <String>] [-Name <String[]>] [-State <SessionFilterState>] -VMId <Guid[]> [<CommonParameters>]
Get-PSSession [-ConfigurationName <String>] -InstanceId <Guid[]> [-State <SessionFilterState>] -VMId <Guid[]> [<CommonParameters>]
Get-PSSession [-ConfigurationName <String>] [-Name <String[]>] [-State <SessionFilterState>] -VMName <String[]> [<CommonParameters>]
Get-PSSession [-InstanceId <Guid[]>] [<CommonParameters>]
Get-PSSession [-Id] <Int32[]> [<CommonParameters>]
Get-PSSessionCapability [-ConfigurationName] <String> [-Username] <String> [-Full] [<CommonParameters>]
Get-PSSessionConfiguration [[-Name] <String[]>] [-Force] [<CommonParameters>]
Get-PSSubsystem []
Get-PSSubsystem -Kind <SubsystemKind> [<CommonParameters>]
Get-PSSubsystem -SubsystemType <Type> [<CommonParameters>]
Import-Module [-Global] [-Prefix <String>] [-Name] <String[]> [-Function <String[]>] [-Cmdlet <String[]>] [-Variable <String[]>] [-Alias <String[]>] [-Force] [-SkipEditionCheck] [-PassThru] [-AsCustomObject] [-MinimumVersion <Version>] [-MaximumVersion <String>] [-RequiredVersion <Version>] [-ArgumentList <Object[]>] [-DisableNameChecking] [-NoClobber] [-Scope <String>] [<CommonParameters>]
Import-Module [-Global] [-Prefix <String>] [-Name] <String[]> [-Function <String[]>] [-Cmdlet <String[]>] [-Variable <String[]>] [-Alias <String[]>] [-Force] [-SkipEditionCheck] [-PassThru] [-AsCustomObject] [-MinimumVersion <Version>] [-MaximumVersion <String>] [-RequiredVersion <Version>] [-ArgumentList <Object[]>] [-DisableNameChecking] [-NoClobber] [-Scope <String>] -PSSession <PSSession> [<CommonParameters>]
Import-Module [-Global] [-Prefix <String>] [-Name] <String[]> [-Function <String[]>] [-Cmdlet <String[]>] [-Variable <String[]>] [-Alias <String[]>] [-Force] [-SkipEditionCheck] [-PassThru] [-AsCustomObject] [-MinimumVersion <Version>] [-MaximumVersion <String>] [-RequiredVersion <Version>] [-ArgumentList <Object[]>] [-DisableNameChecking] [-NoClobber] [-Scope <String>] -CimSession <CimSession> [-CimResourceUri <Uri>] [-CimNamespace <String>] [<CommonParameters>]
Import-Module [-Name] <string[]> -UseWindowsPowerShell [-Global] [-Prefix <string>] [-Function <string[]>] [-Cmdlet <string[]>] [-Variable <string[]>] [-Alias <string[]>] [-Force] [-PassThru] [-AsCustomObject] [-MinimumVersion <version>] [-MaximumVersion <string>] [-RequiredVersion <version>] [-ArgumentList <Object[]>] [-DisableNameChecking] [-NoClobber] [-Scope <string>] [<CommonParameters>]
Import-Module [-Global] [-Prefix <String>] [-FullyQualifiedName] <ModuleSpecification[]> [-Function <String[]>] [-Cmdlet <String[]>] [-Variable <String[]>] [-Alias <String[]>] [-Force] [-SkipEditionCheck] [-PassThru] [-AsCustomObject] [-ArgumentList <Object[]>] [-DisableNameChecking] [-NoClobber] [-Scope <String>] [<CommonParameters>]
Import-Module [-Global] [-Prefix <String>] [-FullyQualifiedName] <ModuleSpecification[]> [-Function <String[]>] [-Cmdlet <String[]>] [-Variable <String[]>] [-Alias <String[]>] [-Force] [-SkipEditionCheck] [-PassThru] [-AsCustomObject] [-ArgumentList <Object[]>] [-DisableNameChecking] [-NoClobber] [-Scope <String>] -PSSession <PSSession> [<CommonParameters>]
Import-Module [-FullyQualifiedName] <ModuleSpecification[]> -UseWindowsPowerShell [-Global] [-Prefix <string>] [-Function <string[]>] [-Cmdlet <string[]>] [-Variable <string[]>] [-Alias <string[]>] [-Force] [-PassThru] [-AsCustomObject] [-ArgumentList <Object[]>] [-DisableNameChecking] [-NoClobber] [-Scope <string>] [<CommonParameters>]
Import-Module [-Global] [-Prefix <String>] [-Assembly] <Assembly[]> [-Function <String[]>] [-Cmdlet <String[]>] [-Variable <String[]>] [-Alias <String[]>] [-Force] [-SkipEditionCheck] [-PassThru] [-AsCustomObject] [-ArgumentList <Object[]>] [-DisableNameChecking] [-NoClobber] [-Scope <String>] [<CommonParameters>]
Import-Module [-Global] [-Prefix <String>] [-Function <String[]>] [-Cmdlet <String[]>] [-Variable <String[]>] [-Alias <String[]>] [-Force] [-SkipEditionCheck] [-PassThru] [-AsCustomObject] [-ModuleInfo] <PSModuleInfo[]> [-ArgumentList <Object[]>] [-DisableNameChecking] [-NoClobber] [-Scope <String>] [<CommonParameters>]
Invoke-Command [-ScriptBlock] <ScriptBlock> [-NoNewScope] [-InputObject <PSObject>] [-ArgumentList <Object[]>] [<CommonParameters>]
Invoke-Command [[-Session] <PSSession[]>] [-ThrottleLimit <Int32>] [-AsJob] [-HideComputerName] [-JobName <String>] [-FilePath] <String> [-RemoteDebug] [-InputObject <PSObject>] [-ArgumentList <Object[]>] [<CommonParameters>]
Invoke-Command [[-Session] <PSSession[]>] [-ThrottleLimit <Int32>] [-AsJob] [-HideComputerName] [-JobName <String>] [-ScriptBlock] <ScriptBlock> [-RemoteDebug] [-InputObject <PSObject>] [-ArgumentList <Object[]>] [<CommonParameters>]
Invoke-Command [[-ComputerName] <String[]>] [-Credential <PSCredential>] [-Port <Int32>] [-UseSSL] [-ConfigurationName <String>] [-ApplicationName <String>] [-ThrottleLimit <Int32>] [-AsJob] [-InDisconnectedSession] [-SessionName <String[]>] [-HideComputerName] [-JobName <String>] [-FilePath] <String> [-SessionOption <PSSessionOption>] [-Authentication <AuthenticationMechanism>] [-EnableNetworkAccess] [-RemoteDebug] [-InputObject <PSObject>] [-ArgumentList <Object[]>] [<CommonParameters>]
Invoke-Command [[-ComputerName] <String[]>] [-Credential <PSCredential>] [-Port <Int32>] [-UseSSL] [-ConfigurationName <String>] [-ApplicationName <String>] [-ThrottleLimit <Int32>] [-AsJob] [-InDisconnectedSession] [-SessionName <String[]>] [-HideComputerName] [-JobName <String>] [-ScriptBlock] <ScriptBlock> [-SessionOption <PSSessionOption>] [-Authentication <AuthenticationMechanism>] [-EnableNetworkAccess] [-RemoteDebug] [-InputObject <PSObject>] [-ArgumentList <Object[]>] [-CertificateThumbprint <String>] [<CommonParameters>]
Invoke-Command [-Credential <PSCredential>] [-ConfigurationName <String>] [-ThrottleLimit <Int32>] [[-ConnectionUri] <Uri[]>] [-AsJob] [-InDisconnectedSession] [-HideComputerName] [-JobName <String>] [-ScriptBlock] <ScriptBlock> [-AllowRedirection] [-SessionOption <PSSessionOption>] [-Authentication <AuthenticationMechanism>] [-EnableNetworkAccess] [-RemoteDebug] [-InputObject <PSObject>] [-ArgumentList <Object[]>] [-CertificateThumbprint <String>] [<CommonParameters>]
Invoke-Command [-Credential <PSCredential>] [-ConfigurationName <String>] [-ThrottleLimit <Int32>] [[-ConnectionUri] <Uri[]>] [-AsJob] [-InDisconnectedSession] [-HideComputerName] [-JobName <String>] [-FilePath] <String> [-AllowRedirection] [-SessionOption <PSSessionOption>] [-Authentication <AuthenticationMechanism>] [-EnableNetworkAccess] [-RemoteDebug] [-InputObject <PSObject>] [-ArgumentList <Object[]>] [<CommonParameters>]
Invoke-Command -Credential <PSCredential> [-ConfigurationName <String>] [-ThrottleLimit <Int32>] [-AsJob] [-HideComputerName] [-ScriptBlock] <ScriptBlock> [-RemoteDebug] [-InputObject <PSObject>] [-ArgumentList <Object[]>] [-VMId] <Guid[]> [<CommonParameters>]
Invoke-Command -Credential <PSCredential> [-ConfigurationName <String>] [-ThrottleLimit <Int32>] [-AsJob] [-HideComputerName] [-ScriptBlock] <ScriptBlock> [-RemoteDebug] [-InputObject <PSObject>] [-ArgumentList <Object[]>] -VMName <String[]> [<CommonParameters>]
Invoke-Command -Credential <PSCredential> [-ConfigurationName <String>] [-ThrottleLimit <Int32>] [-AsJob] [-HideComputerName] [-FilePath] <String> [-RemoteDebug] [-InputObject <PSObject>] [-ArgumentList <Object[]>] [-VMId] <Guid[]> [<CommonParameters>]
Invoke-Command -Credential <PSCredential> [-ConfigurationName <String>] [-ThrottleLimit <Int32>] [-AsJob] [-HideComputerName] [-FilePath] <String> [-RemoteDebug] [-InputObject <PSObject>] [-ArgumentList <Object[]>] -VMName <String[]> [<CommonParameters>]
Invoke-Command [-Port <Int32>] [-AsJob] [-HideComputerName] [-JobName <String>] [-ScriptBlock] <ScriptBlock> -HostName <String[]> [-UserName <String>] [-KeyFilePath <String>] [-SSHTransport] [-RemoteDebug] [-InputObject <PSObject>] [-ArgumentList <Object[]>] [-Subsystem <String>] [-ConnectingTimeout <int>] [<CommonParameters>]
Invoke-Command [-ConfigurationName <String>] [-ThrottleLimit <Int32>] [-AsJob] [-HideComputerName] [-JobName <String>] [-ScriptBlock] <ScriptBlock> [-RunAsAdministrator] [-RemoteDebug] [-InputObject <PSObject>] [-ArgumentList <Object[]>] -ContainerId <String[]> [<CommonParameters>]
Invoke-Command [-ConfigurationName <String>] [-ThrottleLimit <Int32>] [-AsJob] [-HideComputerName] [-JobName <String>] [-FilePath] <String> [-RunAsAdministrator] [-RemoteDebug] [-InputObject <PSObject>] [-ArgumentList <Object[]>] -ContainerId <String[]> [<CommonParameters>]
Invoke-Command [-AsJob] [-HideComputerName] [-JobName <String>] [-ScriptBlock] <ScriptBlock> -SSHConnection <Hashtable[]> [-RemoteDebug] [-InputObject <PSObject>] [-ArgumentList <Object[]>] [<CommonParameters>]
Invoke-Command [-AsJob] [-HideComputerName] -FilePath <String> -HostName <String[]> [-UserName <String>] [-KeyFilePath <String>] [-SSHTransport] [-RemoteDebug] [-InputObject <PSObject>] [-ArgumentList <Object[]>] [<CommonParameters>]
Invoke-Command [-AsJob] [-HideComputerName] -FilePath <String> -SSHConnection <Hashtable[]> [-RemoteDebug] [-InputObject <PSObject>] [-ArgumentList <Object[]>] [<CommonParameters>]
Invoke-History [[-Id] <String>] [-WhatIf] [-Confirm] [<CommonParameters>]
New-Module [-ScriptBlock] <ScriptBlock> [-Function <String[]>] [-Cmdlet <String[]>] [-ReturnResult] [-AsCustomObject] [-ArgumentList <Object[]>] [<CommonParameters>]
New-Module [-Name] <String> [-ScriptBlock] <ScriptBlock> [-Function <String[]>] [-Cmdlet <String[]>] [-ReturnResult] [-AsCustomObject] [-ArgumentList <Object[]>] [<CommonParameters>]
New-ModuleManifest [-Path] <String> [-NestedModules <Object[]>] [-Guid <Guid>] [-Author <String>] [-CompanyName <String>] [-Copyright <String>] [-RootModule <String>] [-ModuleVersion <Version>] [-Description <String>] [-ProcessorArchitecture <ProcessorArchitecture>] [-PowerShellVersion <Version>] [-CLRVersion <Version>] [-DotNetFrameworkVersion <Version>] [-PowerShellHostName <String>] [-PowerShellHostVersion <Version>] [-RequiredModules <Object[]>] [-TypesToProcess <String[]>] [-FormatsToProcess <String[]>] [-ScriptsToProcess <String[]>] [-RequiredAssemblies <String[]>] [-FileList <String[]>] [-ModuleList <Object[]>] [-FunctionsToExport <String[]>] [-AliasesToExport <String[]>] [-VariablesToExport <String[]>] [-CmdletsToExport <String[]>] [-DscResourcesToExport <String[]>] [-CompatiblePSEditions <String[]>] [-PrivateData <Object>] [-Tags <String[]>] [-ProjectUri <Uri>] [-LicenseUri <Uri>] [-IconUri <Uri>] [-ReleaseNotes <String>] [-Prerelease <String>] [-RequireLicenseAcceptance] [-ExternalModuleDependencies <String[]>] [-HelpInfoUri <String>] [-PassThru] [-DefaultCommandPrefix <String>] [-WhatIf] [-Confirm] [<CommonParameters>]
New-PSRoleCapabilityFile [-Path] <String> [-Guid <Guid>] [-Author <String>] [-Description <String>] [-CompanyName <String>] [-Copyright <String>] [-ModulesToImport <Object[]>] [-VisibleAliases <String[]>] [-VisibleCmdlets <Object[]>] [-VisibleFunctions <Object[]>] [-VisibleExternalCommands <String[]>] [-VisibleProviders <String[]>] [-ScriptsToProcess <String[]>] [-AliasDefinitions <IDictionary[]>] [-FunctionDefinitions <IDictionary[]>] [-VariableDefinitions <Object>] [-EnvironmentVariables <IDictionary>] [-TypesToProcess <String[]>] [-FormatsToProcess <String[]>] [-AssembliesToLoad <String[]>] [<CommonParameters>]
New-PSSession [[-ComputerName] <String[]>] [-Credential <PSCredential>] [-Name <String[]>] [-EnableNetworkAccess] [-ConfigurationName <String>] [-Port <Int32>] [-UseSSL] [-ApplicationName <String>] [-ThrottleLimit <Int32>] [-SessionOption <PSSessionOption>] [-Authentication <AuthenticationMechanism>] [-CertificateThumbprint <String>] [<CommonParameters>]
New-PSSession [-Credential <PSCredential>] [-Name <String[]>] [-EnableNetworkAccess] [-ConfigurationName <String>] [-ThrottleLimit <Int32>] [-ConnectionUri] <Uri[]> [-AllowRedirection] [-SessionOption <PSSessionOption>] [-Authentication <AuthenticationMechanism>] [-CertificateThumbprint <String>] [<CommonParameters>]
New-PSSession -Credential <PSCredential> [-Name <String[]>] [-ConfigurationName <String>] [-VMId] <Guid[]> [-ThrottleLimit <Int32>] [<CommonParameters>]
New-PSSession -Credential <PSCredential> [-Name <String[]>] [-ConfigurationName <String>] -VMName <String[]> [-ThrottleLimit <Int32>] [<CommonParameters>]
New-PSSession [[-Session] <PSSession[]>] [-Name <String[]>] [-EnableNetworkAccess] [-ThrottleLimit <Int32>] [<CommonParameters>]
New-PSSession [-Name <String[]>] [-ConfigurationName <String>] -ContainerId <String[]> [-RunAsAdministrator] [-ThrottleLimit <Int32>] [<CommonParameters>]
New-PSSession -UseWindowsPowerShell [-Name <string[]>] [<CommonParameters>]
New-PSSession [-Name <String[]>] [-Port <Int32>] [-HostName] <String[]> [-UserName <String>] [-KeyFilePath <String>] [-SSHTransport] [-Subsystem <String>] [-ConnectingTimeout <int>] [<CommonParameters>]
New-PSSession [-Name <String[]>] -SSHConnection <Hashtable[]> [<CommonParameters>]
New-PSSessionConfigurationFile [-Path] <String> [-SchemaVersion <Version>] [-Guid <Guid>] [-Author <String>] [-Description <String>] [-CompanyName <String>] [-Copyright <String>] [-SessionType <SessionType>] [-TranscriptDirectory <String>] [-RunAsVirtualAccount] [-RunAsVirtualAccountGroups <String[]>] [-MountUserDrive] [-UserDriveMaximumSize <Int64>] [-GroupManagedServiceAccount <String>] [-ScriptsToProcess <String[]>] [-RoleDefinitions <IDictionary>] [-RequiredGroups <IDictionary>] [-LanguageMode <PSLanguageMode>] [-ExecutionPolicy <ExecutionPolicy>] [-PowerShellVersion <Version>] [-ModulesToImport <Object[]>] [-VisibleAliases <String[]>] [-VisibleCmdlets <Object[]>] [-VisibleFunctions <Object[]>] [-VisibleExternalCommands <String[]>] [-VisibleProviders <String[]>] [-AliasDefinitions <IDictionary[]>] [-FunctionDefinitions <IDictionary[]>] [-VariableDefinitions <Object>] [-EnvironmentVariables <IDictionary>] [-TypesToProcess <String[]>] [-FormatsToProcess <String[]>] [-AssembliesToLoad <String[]>] [-Full] [<CommonParameters>]
New-PSSessionOption [-MaximumRedirection <Int32>] [-NoCompression] [-NoMachineProfile] [-Culture <CultureInfo>] [-UICulture <CultureInfo>] [-MaximumReceivedDataSizePerCommand <Int32>] [-MaximumReceivedObjectSize <Int32>] [-OutputBufferingMode <OutputBufferingMode>] [-MaxConnectionRetryCount <Int32>] [-ApplicationArguments <PSPrimitiveDictionary>] [-OpenTimeout <Int32>] [-CancelTimeout <Int32>] [-IdleTimeout <Int32>] [-ProxyAccessType <ProxyAccessType>] [-ProxyAuthentication <AuthenticationMechanism>] [-ProxyCredential <PSCredential>] [-SkipCACheck] [-SkipCNCheck] [-SkipRevocationCheck] [-OperationTimeout <Int32>] [-NoEncryption] [-UseUTF16] [-IncludePortInSPN] [<CommonParameters>]
New-PSTransportOption [-MaxIdleTimeoutSec <Int32>] [-ProcessIdleTimeoutSec <Int32>] [-MaxSessions <Int32>] [-MaxConcurrentCommandsPerSession <Int32>] [-MaxSessionsPerUser <Int32>] [-MaxMemoryPerSessionMB <Int32>] [-MaxProcessesPerSession <Int32>] [-MaxConcurrentUsers <Int32>] [-IdleTimeoutSec <Int32>] [-OutputBufferingMode <OutputBufferingMode>] [<CommonParameters>]
Out-Default [-Transcript] [-InputObject <PSObject>] [<CommonParameters>]
Out-Host [-Paging] [-InputObject <PSObject>] [<CommonParameters>]
Out-Null [-InputObject <PSObject>] [<CommonParameters>]
Receive-Job [-Job] <Job[]> [[-Location] <String[]>] [-Keep] [-NoRecurse] [-Force] [-Wait] [-AutoRemoveJob] [-WriteEvents] [-WriteJobInResults] [<CommonParameters>]
Receive-Job [-Job] <Job[]> [[-ComputerName] <String[]>] [-Keep] [-NoRecurse] [-Force] [-Wait] [-AutoRemoveJob] [-WriteEvents] [-WriteJobInResults] [<CommonParameters>]
Receive-Job [-Job] <Job[]> [[-Session] <PSSession[]>] [-Keep] [-NoRecurse] [-Force] [-Wait] [-AutoRemoveJob] [-WriteEvents] [-WriteJobInResults] [<CommonParameters>]
Receive-Job [-Keep] [-NoRecurse] [-Force] [-Wait] [-AutoRemoveJob] [-WriteEvents] [-WriteJobInResults] [-Name] <String[]> [<CommonParameters>]
Receive-Job [-Keep] [-NoRecurse] [-Force] [-Wait] [-AutoRemoveJob] [-WriteEvents] [-WriteJobInResults] [-InstanceId] <Guid[]> [<CommonParameters>]
Receive-Job [-Keep] [-NoRecurse] [-Force] [-Wait] [-AutoRemoveJob] [-WriteEvents] [-WriteJobInResults] [-Id] <Int32[]> [<CommonParameters>]
Receive-PSSession [-Session] <PSSession> [-OutTarget <OutTarget>] [-JobName <String>] [-WhatIf] [-Confirm] [<CommonParameters>]
Receive-PSSession [-Id] <Int32> [-OutTarget <OutTarget>] [-JobName <String>] [-WhatIf] [-Confirm] [<CommonParameters>]
Receive-PSSession [-ComputerName] <String> [-ApplicationName <String>] [-ConfigurationName <String>] -Name <String> [-OutTarget <OutTarget>] [-JobName <String>] [-Credential <PSCredential>] [-Authentication <AuthenticationMechanism>] [-CertificateThumbprint <String>] [-Port <Int32>] [-UseSSL] [-SessionOption <PSSessionOption>] [-WhatIf] [-Confirm] [<CommonParameters>]
Receive-PSSession [-ComputerName] <String> [-ApplicationName <String>] [-ConfigurationName <String>] -InstanceId <Guid> [-OutTarget <OutTarget>] [-JobName <String>] [-Credential <PSCredential>] [-Authentication <AuthenticationMechanism>] [-CertificateThumbprint <String>] [-Port <Int32>] [-UseSSL] [-SessionOption <PSSessionOption>] [-WhatIf] [-Confirm] [<CommonParameters>]
Receive-PSSession [-ConfigurationName <String>] [-ConnectionUri] <Uri> [-AllowRedirection] -Name <String> [-OutTarget <OutTarget>] [-JobName <String>] [-Credential <PSCredential>] [-Authentication <AuthenticationMechanism>] [-CertificateThumbprint <String>] [-SessionOption <PSSessionOption>] [-WhatIf] [-Confirm] [<CommonParameters>]
Receive-PSSession [-ConfigurationName <String>] [-ConnectionUri] <Uri> [-AllowRedirection] -InstanceId <Guid> [-OutTarget <OutTarget>] [-JobName <String>] [-Credential <PSCredential>] [-Authentication <AuthenticationMechanism>] [-CertificateThumbprint <String>] [-SessionOption <PSSessionOption>] [-WhatIf] [-Confirm] [<CommonParameters>]
Receive-PSSession -InstanceId <Guid> [-OutTarget <OutTarget>] [-JobName <String>] [-WhatIf] [-Confirm] [<CommonParameters>]
Receive-PSSession -Name <String> [-OutTarget <OutTarget>] [-JobName <String>] [-WhatIf] [-Confirm] [<CommonParameters>]
Register-ArgumentCompleter -CommandName <String[]> -ScriptBlock <ScriptBlock> [-Native] [<CommonParameters>]
Register-ArgumentCompleter [-CommandName <String[]>] -ParameterName <String> -ScriptBlock <ScriptBlock> [<CommonParameters>]
Register-PSSessionConfiguration [-ProcessorArchitecture <String>] [-Name] <String> [-ApplicationBase <String>] [-RunAsCredential <PSCredential>] [-ThreadApartmentState <ApartmentState>] [-ThreadOptions <PSThreadOptions>] [-AccessMode <PSSessionConfigurationAccessMode>] [-UseSharedProcess] [-StartupScript <String>] [-MaximumReceivedDataSizePerCommandMB <Double>] [-MaximumReceivedObjectSizeMB <Double>] [-SecurityDescriptorSddl <String>] [-ShowSecurityDescriptorUI] [-Force] [-NoServiceRestart] [-PSVersion <Version>] [-SessionTypeOption <PSSessionTypeOption>] [-TransportOption <PSTransportOption>] [-ModulesToImport <Object[]>] [-WhatIf] [-Confirm] [<CommonParameters>]
Register-PSSessionConfiguration [-ProcessorArchitecture <String>] [-Name] <String> [-AssemblyName] <String> [-ApplicationBase <String>] [-ConfigurationTypeName] <String> [-RunAsCredential <PSCredential>] [-ThreadApartmentState <ApartmentState>] [-ThreadOptions <PSThreadOptions>] [-AccessMode <PSSessionConfigurationAccessMode>] [-UseSharedProcess] [-StartupScript <String>] [-MaximumReceivedDataSizePerCommandMB <Double>] [-MaximumReceivedObjectSizeMB <Double>] [-SecurityDescriptorSddl <String>] [-ShowSecurityDescriptorUI] [-Force] [-NoServiceRestart] [-PSVersion <Version>] [-SessionTypeOption <PSSessionTypeOption>] [-TransportOption <PSTransportOption>] [-ModulesToImport <Object[]>] [-WhatIf] [-Confirm] [<CommonParameters>]
Register-PSSessionConfiguration [-ProcessorArchitecture <String>] [-Name] <String> [-RunAsCredential <PSCredential>] [-ThreadApartmentState <ApartmentState>] [-ThreadOptions <PSThreadOptions>] [-AccessMode <PSSessionConfigurationAccessMode>] [-UseSharedProcess] [-StartupScript <String>] [-MaximumReceivedDataSizePerCommandMB <Double>] [-MaximumReceivedObjectSizeMB <Double>] [-SecurityDescriptorSddl <String>] [-ShowSecurityDescriptorUI] [-Force] [-NoServiceRestart] [-TransportOption <PSTransportOption>] -Path <String> [-WhatIf] [-Confirm] [<CommonParameters>]
Remove-Job [-Force] [-Id] <Int32[]> [-WhatIf] [-Confirm] [<CommonParameters>]
Remove-Job [-Job] <Job[]> [-Force] [-WhatIf] [-Confirm] [<CommonParameters>]
Remove-Job [-Force] [-Name] <String[]> [-WhatIf] [-Confirm] [<CommonParameters>]
Remove-Job [-Force] [-InstanceId] <Guid[]> [-WhatIf] [-Confirm] [<CommonParameters>]
Remove-Job [-Force] [-Filter] <Hashtable> [-WhatIf] [-Confirm] [<CommonParameters>]
Remove-Job [-State] <JobState> [-WhatIf] [-Confirm] [<CommonParameters>]
Remove-Job [-Command <String[]>] [-WhatIf] [-Confirm] [<CommonParameters>]
Remove-Module [-Name] <String[]> [-Force] [-WhatIf] [-Confirm] [<CommonParameters>]
Remove-Module [-FullyQualifiedName] <ModuleSpecification[]> [-Force] [-WhatIf] [-Confirm] [<CommonParameters>]
Remove-Module [-ModuleInfo] <PSModuleInfo[]> [-Force] [-WhatIf] [-Confirm] [<CommonParameters>]
Remove-PSSession [-Id] <Int32[]> [-WhatIf] [-Confirm] [<CommonParameters>]
Remove-PSSession [-Session] <PSSession[]> [-WhatIf] [-Confirm] [<CommonParameters>]
Remove-PSSession -ContainerId <String[]> [-WhatIf] [-Confirm] [<CommonParameters>]
Remove-PSSession -VMId <Guid[]> [-WhatIf] [-Confirm] [<CommonParameters>]
Remove-PSSession -VMName <String[]> [-WhatIf] [-Confirm] [<CommonParameters>]
Remove-PSSession -InstanceId <Guid[]> [-WhatIf] [-Confirm] [<CommonParameters>]
Remove-PSSession -Name <String[]> [-WhatIf] [-Confirm] [<CommonParameters>]
Remove-PSSession [-ComputerName] <String[]> [-WhatIf] [-Confirm] [<CommonParameters>]
Save-Help [-DestinationPath] <String[]> [[-Module] <PSModuleInfo[]>] [-FullyQualifiedModule <ModuleSpecification[]>] [[-UICulture] <CultureInfo[]>] [-Credential <PSCredential>] [-UseDefaultCredentials] [-Force] [-Scope <UpdateHelpScope>] [<CommonParameters>]
Save-Help -LiteralPath <String[]> [[-Module] <PSModuleInfo[]>] [-FullyQualifiedModule <ModuleSpecification[]>] [[-UICulture] <CultureInfo[]>] [-Credential <PSCredential>] [-UseDefaultCredentials] [-Force] [-Scope <UpdateHelpScope>] [<CommonParameters>]
Set-PSDebug [-Trace <Int32>] [-Step] [-Strict] [<CommonParameters>]
Set-PSDebug [-Off] [<CommonParameters>]
Set-PSSessionConfiguration [-Name] <String> [-ApplicationBase <String>] [-RunAsCredential <PSCredential>] [-ThreadApartmentState <ApartmentState>] [-ThreadOptions <PSThreadOptions>] [-AccessMode <PSSessionConfigurationAccessMode>] [-UseSharedProcess] [-StartupScript <String>] [-MaximumReceivedDataSizePerCommandMB <Double>] [-MaximumReceivedObjectSizeMB <Double>] [-SecurityDescriptorSddl <String>] [-ShowSecurityDescriptorUI] [-Force] [-NoServiceRestart] [-PSVersion <Version>] [-SessionTypeOption <PSSessionTypeOption>] [-TransportOption <PSTransportOption>] [-ModulesToImport <Object[]>] [-WhatIf] [-Confirm] [<CommonParameters>]
Set-PSSessionConfiguration [-Name] <String> [-AssemblyName] <String> [-ApplicationBase <String>] [-ConfigurationTypeName] <String> [-RunAsCredential <PSCredential>] [-ThreadApartmentState <ApartmentState>] [-ThreadOptions <PSThreadOptions>] [-AccessMode <PSSessionConfigurationAccessMode>] [-UseSharedProcess] [-StartupScript <String>] [-MaximumReceivedDataSizePerCommandMB <Double>] [-MaximumReceivedObjectSizeMB <Double>] [-SecurityDescriptorSddl <String>] [-ShowSecurityDescriptorUI] [-Force] [-NoServiceRestart] [-PSVersion <Version>] [-SessionTypeOption <PSSessionTypeOption>] [-TransportOption <PSTransportOption>] [-ModulesToImport <Object[]>] [-WhatIf] [-Confirm] [<CommonParameters>]
Set-PSSessionConfiguration [-Name] <String> [-RunAsCredential <PSCredential>] [-ThreadApartmentState <ApartmentState>] [-ThreadOptions <PSThreadOptions>] [-AccessMode <PSSessionConfigurationAccessMode>] [-UseSharedProcess] [-StartupScript <String>] [-MaximumReceivedDataSizePerCommandMB <Double>] [-MaximumReceivedObjectSizeMB <Double>] [-SecurityDescriptorSddl <String>] [-ShowSecurityDescriptorUI] [-Force] [-NoServiceRestart] [-TransportOption <PSTransportOption>] -Path <String> [-WhatIf] [-Confirm] [<CommonParameters>]
Set-StrictMode -Version <Version> [<CommonParameters>]
Set-StrictMode [-Off] [<CommonParameters>]
Start-Job [-Name <String>] [-ScriptBlock] <ScriptBlock> [-Credential <PSCredential>] [-Authentication <AuthenticationMechanism>] [[-InitializationScript] <ScriptBlock>] [-WorkingDirectory <String>] [-RunAs32] [-PSVersion <Version>] [-InputObject <PSObject>] [-ArgumentList <Object[]>] [<CommonParameters>]
Start-Job [-DefinitionName] <String> [[-DefinitionPath] <String>] [[-Type] <String>] [-WorkingDirectory <String>] [<CommonParameters>]
Start-Job [-Name <String>] [-Credential <PSCredential>] [-FilePath] <String> [-Authentication <AuthenticationMechanism>] [[-InitializationScript] <ScriptBlock>] [-WorkingDirectory <String>] [-RunAs32] [-PSVersion <Version>] [-InputObject <PSObject>] [-ArgumentList <Object[]>] [<CommonParameters>]
Start-Job [-Name <String>] [-Credential <PSCredential>] -LiteralPath <String> [-Authentication <AuthenticationMechanism>] [[-InitializationScript] <ScriptBlock>] [-WorkingDirectory <String>] [-RunAs32] [-PSVersion <Version>] [-InputObject <PSObject>] [-ArgumentList <Object[]>] [<CommonParameters>]
Stop-Job [-PassThru] [-Id] <Int32[]> [-WhatIf] [-Confirm] [<CommonParameters>]
Stop-Job [-Job] <Job[]> [-PassThru] [-WhatIf] [-Confirm] [<CommonParameters>]
Stop-Job [-PassThru] [-Name] <String[]> [-WhatIf] [-Confirm] [<CommonParameters>]
Stop-Job [-PassThru] [-InstanceId] <Guid[]> [-WhatIf] [-Confirm] [<CommonParameters>]
Stop-Job [-PassThru] [-State] <JobState> [-WhatIf] [-Confirm] [<CommonParameters>]
Stop-Job [-PassThru] [-Filter] <Hashtable> [-WhatIf] [-Confirm] [<CommonParameters>]
Test-ModuleManifest [-Path] <String> [<CommonParameters>]
Test-PSSessionConfigurationFile [-Path] <String> [<CommonParameters>]
Unregister-PSSessionConfiguration [-Name] <String> [-Force] [-NoServiceRestart] [-WhatIf] [-Confirm] [<CommonParameters>]
Update-Help [[-Module] <String[]>] [-FullyQualifiedModule <ModuleSpecification[]>] [[-SourcePath] <String[]>] [-Recurse] [[-UICulture] <CultureInfo[]>] [-Credential <PSCredential>] [-UseDefaultCredentials] [-Force] [-Scope <UpdateHelpScope>] [-WhatIf] [-Confirm] [<CommonParameters>]
Update-Help [[-Module] <String[]>] [-FullyQualifiedModule <ModuleSpecification[]>] [-LiteralPath <String[]>] [-Recurse] [[-UICulture] <CultureInfo[]>] [-Credential <PSCredential>] [-UseDefaultCredentials] [-Force] [-Scope <UpdateHelpScope>] [-WhatIf] [-Confirm] [<CommonParameters>]
Wait-Job [-Any] [-Timeout <Int32>] [-Force] [-Id] <Int32[]> [<CommonParameters>]
Wait-Job [-Job] <Job[]> [-Any] [-Timeout <Int32>] [-Force] [<CommonParameters>]
Wait-Job [-Any] [-Timeout <Int32>] [-Force] [-Name] <String[]> [<CommonParameters>]
Wait-Job [-Any] [-Timeout <Int32>] [-Force] [-InstanceId] <Guid[]> [<CommonParameters>]
Wait-Job [-Any] [-Timeout <Int32>] [-Force] [-State] <JobState> [<CommonParameters>]
Wait-Job [-Any] [-Timeout <Int32>] [-Force] [-Filter] <Hashtable> [<CommonParameters>]
Where-Object [-InputObject <PSObject>] [-Property] <String> [[-Value] <Object>] [-EQ] [<CommonParameters>]
Where-Object [-InputObject <PSObject>] [-FilterScript] <ScriptBlock> [<CommonParameters>]
Where-Object [-InputObject <PSObject>] [-Property] <String> [[-Value] <Object>] -Match [<CommonParameters>]
Where-Object [-InputObject <PSObject>] [-Property] <String> [[-Value] <Object>] -CEQ [<CommonParameters>]
Where-Object [-InputObject <PSObject>] [-Property] <String> [[-Value] <Object>] -NE [<CommonParameters>]
Where-Object [-InputObject <PSObject>] [-Property] <String> [[-Value] <Object>] -CNE [<CommonParameters>]
Where-Object [-InputObject <PSObject>] [-Property] <String> [[-Value] <Object>] -GT [<CommonParameters>]
Where-Object [-InputObject <PSObject>] [-Property] <String> [[-Value] <Object>] -CGT [<CommonParameters>]
Where-Object [-InputObject <PSObject>] [-Property] <String> [[-Value] <Object>] -LT [<CommonParameters>]
Where-Object [-InputObject <PSObject>] [-Property] <String> [[-Value] <Object>] -CLT [<CommonParameters>]
Where-Object [-InputObject <PSObject>] [-Property] <String> [[-Value] <Object>] -GE [<CommonParameters>]
Where-Object [-InputObject <PSObject>] [-Property] <String> [[-Value] <Object>] -CGE [<CommonParameters>]
Where-Object [-InputObject <PSObject>] [-Property] <String> [[-Value] <Object>] -LE [<CommonParameters>]
Where-Object [-InputObject <PSObject>] [-Property] <String> [[-Value] <Object>] -CLE [<CommonParameters>]
Where-Object [-InputObject <PSObject>] [-Property] <String> [[-Value] <Object>] -Like [<CommonParameters>]
Where-Object [-InputObject <PSObject>] [-Property] <String> [[-Value] <Object>] -CLike [<CommonParameters>]
Where-Object [-InputObject <PSObject>] [-Property] <String> [[-Value] <Object>] -NotLike [<CommonParameters>]
Where-Object [-InputObject <PSObject>] [-Property] <String> [[-Value] <Object>] -CNotLike [<CommonParameters>]
Where-Object [-InputObject <PSObject>] [-Property] <String> [[-Value] <Object>] -CMatch [<CommonParameters>]
Where-Object [-InputObject <PSObject>] [-Property] <String> [[-Value] <Object>] -NotMatch [<CommonParameters>]
Where-Object [-InputObject <PSObject>] [-Property] <String> [[-Value] <Object>] -CNotMatch [<CommonParameters>]
Where-Object [-InputObject <PSObject>] [-Property] <String> [[-Value] <Object>] -Contains [<CommonParameters>]
Where-Object [-InputObject <PSObject>] [-Property] <String> [[-Value] <Object>] -CContains [<CommonParameters>]
Where-Object [-InputObject <PSObject>] [-Property] <String> [[-Value] <Object>] -NotContains [<CommonParameters>]
Where-Object [-InputObject <PSObject>] [-Property] <String> [[-Value] <Object>] -CNotContains [<CommonParameters>]
Where-Object [-InputObject <PSObject>] [-Property] <String> [[-Value] <Object>] -In [<CommonParameters>]
Where-Object [-InputObject <PSObject>] [-Property] <String> [[-Value] <Object>] -CIn [<CommonParameters>]
Where-Object [-InputObject <PSObject>] [-Property] <String> [[-Value] <Object>] -NotIn [<CommonParameters>]
Where-Object [-InputObject <PSObject>] [-Property] <String> [[-Value] <Object>] -CNotIn [<CommonParameters>]
Where-Object [-InputObject <PSObject>] [-Property] <String> [[-Value] <Object>] -Is [<CommonParameters>]
Where-Object [-InputObject <PSObject>] [-Property] <String> [[-Value] <Object>] -IsNot [<CommonParameters>]
Where-Object [-InputObject <PSObject>] [-Property] <String> -Not [<CommonParameters>]
}

{ # Microsoft.PowerShell.Diagnostics
Get-Counter [[-Counter] <String[]>] [-SampleInterval <Int32>] [-MaxSamples <Int64>] [-Continuous] [-ComputerName <String[]>] [<CommonParameters>]
Get-Counter [-ListSet] <String[]> [-ComputerName <String[]>] [<CommonParameters>]
Get-WinEvent [[-LogName] <String[]>] [-MaxEvents <Int64>] [-ComputerName <String>] [-Credential <PSCredential>] [-FilterXPath <String>] [-Force] [-Oldest] [<CommonParameters>]
Get-WinEvent [-ListLog] <String[]> [-ComputerName <String>] [-Credential <PSCredential>] [-Force] [<CommonParameters>]
Get-WinEvent [-ListProvider] <String[]> [-ComputerName <String>] [-Credential <PSCredential>] [<CommonParameters>]
Get-WinEvent [-ProviderName] <String[]> [-MaxEvents <Int64>] [-ComputerName <String>] [-Credential <PSCredential>] [-FilterXPath <String>] [-Force] [-Oldest] [<CommonParameters>]
Get-WinEvent [-Path] <String[]> [-MaxEvents <Int64>] [-Credential <PSCredential>] [-FilterXPath <String>] [-Oldest] [<CommonParameters>]
Get-WinEvent [-MaxEvents <Int64>] [-ComputerName <String>] [-Credential <PSCredential>] [-FilterHashtable] <Hashtable[]> [-Force] [-Oldest] [<CommonParameters>]
Get-WinEvent [-MaxEvents <Int64>] [-ComputerName <String>] [-Credential <PSCredential>] [-FilterXml] <XmlDocument> [-Oldest] [<CommonParameters>]
New-WinEvent [-ProviderName] <String> [-Id] <Int32> [-Version <Byte>] [[-Payload] <Object[]>] [<CommonParameters>]
}

{ # Microsoft.PowerShell.Host
Start-Transcript [[-Path] <String>] [-Append] [-Force] [-NoClobber] [-IncludeInvocationHeader] [-UseMinimalHeader] [-WhatIf] [-Confirm] [<CommonParameters>]
Start-Transcript [[-LiteralPath] <String>] [-Append] [-Force] [-NoClobber] [-IncludeInvocationHeader] [-UseMinimalHeader] [-WhatIf] [-Confirm] [<CommonParameters>]
Start-Transcript [[-OutputDirectory] <String>] [-Append] [-Force] [-NoClobber] [-IncludeInvocationHeader] [-UseMinimalHeader] [-WhatIf] [-Confirm] [<CommonParameters>]
Stop-Transcript []
}

{ # Microsoft.PowerShell.Management
Add-Content [-Path] <string[]> [-Value] <Object[]> [-PassThru] [-Filter <string>] [-Include <string[]>] [-Exclude <string[]>] [-Force] [-Credential <pscredential>] [-WhatIf] [-Confirm] [-NoNewline] [-Encoding <Encoding>] [-AsByteStream] [-Stream <string>] [<CommonParameters>]
Add-Content [-Value] <Object[]> -LiteralPath <string[]> [-PassThru] [-Filter <string>] [-Include <string[]>] [-Exclude <string[]>] [-Force] [-Credential <pscredential>] [-WhatIf] [-Confirm] [-NoNewline] [-Encoding <Encoding>] [-AsByteStream] [-Stream <string>] [<CommonParameters>]
Clear-Content [-Path] <String[]> [-Filter <String>] [-Include <String[]>] [-Exclude <String[]>] [-Force] [-Credential <PSCredential>] [-WhatIf] [-Confirm] [-Stream <String>] [<CommonParameters>]
Clear-Content -LiteralPath <String[]> [-Filter <String>] [-Include <String[]>] [-Exclude <String[]>] [-Force] [-Credential <PSCredential>] [-WhatIf] [-Confirm] [-Stream <String>] [<CommonParameters>]
Clear-Item [-Path] <String[]> [-Force] [-Filter <String>] [-Include <String[]>] [-Exclude <String[]>] [-Credential <PSCredential>] [-WhatIf] [-Confirm] [<CommonParameters>]
Clear-Item -LiteralPath <String[]> [-Force] [-Filter <String>] [-Include <String[]>] [-Exclude <String[]>] [-Credential <PSCredential>] [-WhatIf] [-Confirm] [<CommonParameters>]
Clear-ItemProperty [-Path] <String[]> [-Name] <String> [-PassThru] [-Force] [-Filter <String>] [-Include <String[]>] [-Exclude <String[]>] [-Credential <PSCredential>] [-WhatIf] [-Confirm] [<CommonParameters>]
Clear-ItemProperty -LiteralPath <String[]> [-Name] <String> [-PassThru] [-Force] [-Filter <String>] [-Include <String[]>] [-Exclude <String[]>] [-Credential <PSCredential>] [-WhatIf] [-Confirm] [<CommonParameters>]
Clear-RecycleBin [[-DriveLetter] <String[]>] [-Force] [-WhatIf] [-Confirm] [<CommonParameters>]
Convert-Path [-Path] <String[]> [<CommonParameters>]
Convert-Path -LiteralPath <String[]> [<CommonParameters>]
Copy-Item [-Path] <String[]> [[-Destination] <String>] [-Container] [-Force] [-Filter <String>] [-Include <String[]>] [-Exclude <String[]>] [-Recurse] [-PassThru] [-Credential <PSCredential>] [-WhatIf] [-Confirm] [-FromSession <PSSession>] [-ToSession <PSSession>] [<CommonParameters>]
Copy-Item -LiteralPath <String[]> [[-Destination] <String>] [-Container] [-Force] [-Filter <String>] [-Include <String[]>] [-Exclude <String[]>] [-Recurse] [-PassThru] [-Credential <PSCredential>] [-WhatIf] [-Confirm] [-FromSession <PSSession>] [-ToSession <PSSession>] [<CommonParameters>]
Copy-ItemProperty [-Path] <String[]> [-Name] <String> [-Destination] <String> [-PassThru] [-Force] [-Filter <String>] [-Include <String[]>] [-Exclude <String[]>] [-Credential <PSCredential>] [-WhatIf] [-Confirm] [<CommonParameters>]
Copy-ItemProperty -LiteralPath <String[]> [-Name] <String> [-Destination] <String> [-PassThru] [-Force] [-Filter <String>] [-Include <String[]>] [-Exclude <String[]>] [-Credential <PSCredential>] [-WhatIf] [-Confirm] [<CommonParameters>]
Debug-Process [-Name] <String[]> [-WhatIf] [-Confirm] [<CommonParameters>]
Debug-Process [-Id] <Int32[]> [-WhatIf] [-Confirm] [<CommonParameters>]
Debug-Process -InputObject <Process[]> [-WhatIf] [-Confirm] [<CommonParameters>]
Get-ChildItem [[-Path] <string[]>] [[-Filter] <string>] [-Include <string[]>] [-Exclude <string[]>] [-Recurse] [-Depth <uint32>] [-Force] [-Name] [-Attributes <FlagsExpression[FileAttributes]>] [-FollowSymlink] [-Directory] [-File] [-Hidden] [-ReadOnly] [-System] [<CommonParameters>]
Get-ChildItem [[-Filter] <string>] -LiteralPath <string[]> [-Include <string[]>] [-Exclude <string[]>] [-Recurse] [-Depth <uint32>] [-Force] [-Name] [-Attributes <FlagsExpression[FileAttributes]>] [-FollowSymlink] [-Directory] [-File] [-Hidden] [-ReadOnly] [-System] [<CommonParameters>]
Get-Clipboard [-Raw] [<CommonParameters>]
Get-ComputerInfo [[-Property] <String[]>] [<CommonParameters>]
Get-Content [-ReadCount <Int64>] [-TotalCount <Int64>] [-Tail <Int32>] [-Path] <String[]> [-Filter <String>] [-Include <String[]>] [-Exclude <String[]>] [-Force] [-Credential <PSCredential>] [-Delimiter <String>] [-Wait] [-Raw] [-Encoding <Encoding>] [-AsByteStream] [-Stream <String>] [<CommonParameters>]
Get-Content [-ReadCount <Int64>] [-TotalCount <Int64>] [-Tail <Int32>] -LiteralPath <String[]> [-Filter <String>] [-Include <String[]>] [-Exclude <String[]>] [-Force] [-Credential <PSCredential>] [-Delimiter <String>] [-Wait] [-Raw] [-Encoding <Encoding>] [-AsByteStream] [-Stream <String>] [<CommonParameters>]
Get-HotFix [[-Id] <String[]>] [-ComputerName <String[]>] [-Credential <PSCredential>] [<CommonParameters>]
Get-HotFix [-Description <String[]>] [-ComputerName <String[]>] [-Credential <PSCredential>] [<CommonParameters>]
Get-Item [-Path] <String[]> [-Filter <String>] [-Include <String[]>] [-Exclude <String[]>] [-Force] [-Credential <PSCredential>] [-Stream <String[]>] [<CommonParameters>]
Get-Item -LiteralPath <String[]> [-Filter <String>] [-Include <String[]>] [-Exclude <String[]>] [-Force] [-Credential <PSCredential>] [-Stream <String[]>] [<CommonParameters>]
Get-ItemProperty [-Path] <String[]> [[-Name] <String[]>] [-Filter <String>] [-Include <String[]>] [-Exclude <String[]>] [-Credential <PSCredential>] [<CommonParameters>]
Get-ItemProperty -LiteralPath <String[]> [[-Name] <String[]>] [-Filter <String>] [-Include <String[]>] [-Exclude <String[]>] [-Credential <PSCredential>] [<CommonParameters>]
Get-ItemPropertyValue [[-Path] <String[]>] [-Name] <String[]> [-Filter <String>] [-Include <String[]>] [-Exclude <String[]>] [-Credential <PSCredential>] [<CommonParameters>]
Get-ItemPropertyValue -LiteralPath <String[]> [-Name] <String[]> [-Filter <String>] [-Include <String[]>] [-Exclude <String[]>] [-Credential <PSCredential>] [<CommonParameters>]
Get-Location [-PSProvider <String[]>] [-PSDrive <String[]>] [<CommonParameters>]
Get-Location [-Stack] [-StackName <String[]>] [<CommonParameters>]
Get-Process [[-Name] <String[]>] [-Module] [-FileVersionInfo] [<CommonParameters>]
Get-Process [[-Name] <String[]>] -IncludeUserName [<CommonParameters>]
Get-Process -Id <Int32[]> [-Module] [-FileVersionInfo] [<CommonParameters>]
Get-Process -Id <Int32[]> -IncludeUserName [<CommonParameters>]
Get-Process -InputObject <Process[]> [-Module] [-FileVersionInfo] [<CommonParameters>]
Get-Process -InputObject <Process[]> -IncludeUserName [<CommonParameters>]
Get-PSDrive [[-Name] <String[]>] [-Scope <String>] [-PSProvider <String[]>] [<CommonParameters>]
Get-PSDrive [-LiteralName] <String[]> [-Scope <String>] [-PSProvider <String[]>] [<CommonParameters>]
Get-PSProvider [[-PSProvider] <String[]>] [<CommonParameters>]
Get-Service [[-Name] <String[]>] [-DependentServices] [-RequiredServices] [-Include <String[]>] [-Exclude <String[]>] [<CommonParameters>]
Get-Service [-DependentServices] [-RequiredServices] -DisplayName <String[]> [-Include <String[]>] [-Exclude <String[]>] [<CommonParameters>]
Get-Service [-DependentServices] [-RequiredServices] [-Include <String[]>] [-Exclude <String[]>] [-InputObject <ServiceController[]>] [<CommonParameters>]
Get-TimeZone [[-Name] <String[]>] [<CommonParameters>]
Get-TimeZone -Id <String[]> [<CommonParameters>]
Get-TimeZone [-ListAvailable] [<CommonParameters>]
Invoke-Item [-Path] <String[]> [-Filter <String>] [-Include <String[]>] [-Exclude <String[]>] [-Credential <PSCredential>] [-WhatIf] [-Confirm] [<CommonParameters>]
Invoke-Item -LiteralPath <String[]> [-Filter <String>] [-Include <String[]>] [-Exclude <String[]>] [-Credential <PSCredential>] [-WhatIf] [-Confirm] [<CommonParameters>]
Join-Path [-Path] <String[]> [-ChildPath] <String> [[-AdditionalChildPath] <String[]>] [-Resolve] [-Credential <PSCredential>] [<CommonParameters>]
Move-Item [-Path] <String[]> [[-Destination] <String>] [-Force] [-Filter <String>] [-Include <String[]>] [-Exclude <String[]>] [-PassThru] [-Credential <PSCredential>] [-WhatIf] [-Confirm] [<CommonParameters>]
Move-Item -LiteralPath <String[]> [[-Destination] <String>] [-Force] [-Filter <String>] [-Include <String[]>] [-Exclude <String[]>] [-PassThru] [-Credential <PSCredential>] [-WhatIf] [-Confirm] [<CommonParameters>]
Move-ItemProperty [-Path] <String[]> [-Name] <String[]> [-Destination] <String> [-PassThru] [-Force] [-Filter <String>] [-Include <String[]>] [-Exclude <String[]>] [-Credential <PSCredential>] [-WhatIf] [-Confirm] [<CommonParameters>]
Move-ItemProperty -LiteralPath <String[]> [-Name] <String[]> [-Destination] <String> [-PassThru] [-Force] [-Filter <String>] [-Include <String[]>] [-Exclude <String[]>] [-Credential <PSCredential>] [-WhatIf] [-Confirm] [<CommonParameters>]
New-Item [-Path] <String[]> [-ItemType <String>] [-Value <Object>] [-Force] [-Credential <PSCredential>] [-WhatIf] [-Confirm] [<CommonParameters>]
New-Item [[-Path] <String[]>] -Name <String> [-ItemType <String>] [-Value <Object>] [-Force] [-Credential <PSCredential>] [-WhatIf] [-Confirm] [<CommonParameters>]
New-ItemProperty [-Path] <String[]> [-Name] <String> [-PropertyType <String>] [-Value <Object>] [-Force] [-Filter <String>] [-Include <String[]>] [-Exclude <String[]>] [-Credential <PSCredential>] [-WhatIf] [-Confirm] [<CommonParameters>]
New-ItemProperty -LiteralPath <String[]> [-Name] <String> [-PropertyType <String>] [-Value <Object>] [-Force] [-Filter <String>] [-Include <String[]>] [-Exclude <String[]>] [-Credential <PSCredential>] [-WhatIf] [-Confirm] [<CommonParameters>]
New-PSDrive [-Name] <String> [-PSProvider] <String> [-Root] <String> [-Description <String>] [-Scope <String>] [-Persist] [-Credential <PSCredential>] [-WhatIf] [-Confirm] [<CommonParameters>]
New-Service [-Name] <String> [-BinaryPathName] <String> [-DisplayName <String>] [-Description <String>] [-SecurityDescriptorSddl <String>] [-StartupType <ServiceStartupType>] [-Credential <PSCredential>] [-DependsOn <String[]>] [-WhatIf] [-Confirm] [<CommonParameters>]
Pop-Location [-PassThru] [-StackName <String>] [<CommonParameters>]
Push-Location [[-Path] <String>] [-PassThru] [-StackName <String>] [<CommonParameters>]
Push-Location [-LiteralPath <String>] [-PassThru] [-StackName <String>] [<CommonParameters>]
Remove-Item [-Path] <String[]> [-Filter <String>] [-Include <String[]>] [-Exclude <String[]>] [-Recurse] [-Force] [-Credential <PSCredential>] [-WhatIf] [-Confirm] [-Stream <String[]>] [<CommonParameters>]
Remove-Item -LiteralPath <String[]> [-Filter <String>] [-Include <String[]>] [-Exclude <String[]>] [-Recurse] [-Force] [-Credential <PSCredential>] [-WhatIf] [-Confirm] [-Stream <String[]>] [<CommonParameters>]
Remove-ItemProperty [-Path] <String[]> [-Name] <String[]> [-Force] [-Filter <String>] [-Include <String[]>] [-Exclude <String[]>] [-Credential <PSCredential>] [-InformationAction <ActionPreference>] [-InformationVariable <String>] [-WhatIf] [-Confirm] [<CommonParameters>]
Remove-ItemProperty -LiteralPath <String[]> [-Name] <String[]> [-Force] [-Filter <String>] [-Include <String[]>] [-Exclude <String[]>] [-Credential <PSCredential>] [-WhatIf] [-Confirm] [<CommonParameters>]
Remove-PSDrive [-Name] <String[]> [-PSProvider <String[]>] [-Scope <String>] [-Force] [-WhatIf] [-Confirm] [<CommonParameters>]
Remove-PSDrive [-LiteralName] <String[]> [-PSProvider <String[]>] [-Scope <String>] [-Force] [-WhatIf] [-Confirm] [<CommonParameters>]
Remove-Service [-Name] <String> [-WhatIf] [-Confirm] [<CommonParameters>]
Remove-Service [-InputObject <ServiceController>] [-WhatIf] [-Confirm] [<CommonParameters>]
Rename-Computer [-ComputerName <String>] [-PassThru] [-DomainCredential <PSCredential>] [-LocalCredential <PSCredential>] [-NewName] <String> [-Force] [-Restart] [-WsmanAuthentication <String>] [-WhatIf] [-Confirm] [<CommonParameters>]
Rename-Item [-Path] <String> [-NewName] <String> [-Force] [-PassThru] [-Credential <PSCredential>] [-WhatIf] [-Confirm] [<CommonParameters>]
Rename-Item -LiteralPath <String> [-NewName] <String> [-Force] [-PassThru] [-Credential <PSCredential>] [-WhatIf] [-Confirm] [<CommonParameters>]
Rename-ItemProperty [-Path] <String> [-Name] <String> [-NewName] <String> [-PassThru] [-Force] [-Filter <String>] [-Include <String[]>] [-Exclude <String[]>] [-Credential <PSCredential>] [-WhatIf] [-Confirm] [<CommonParameters>]
Rename-ItemProperty -LiteralPath <String> [-Name] <String> [-NewName] <String> [-PassThru] [-Force] [-Filter <String>] [-Include <String[]>] [-Exclude <String[]>] [-Credential <PSCredential>] [-WhatIf] [-Confirm] [<CommonParameters>]
Resolve-Path [-Path] <String[]> [-Relative] [-Credential <PSCredential>] [<CommonParameters>]
Resolve-Path -LiteralPath <String[]> [-Relative] [-Credential <PSCredential>] [<CommonParameters>]
Restart-Computer [-WsmanAuthentication <String>] [[-ComputerName] <String[]>] [[-Credential]<PSCredential>] [-Force] [-Wait] [-Timeout <Int32>] [-For <WaitForServiceTypes>] [-Delay <Int16>] [-WhatIf] [-Confirm] [<CommonParameters>]
Restart-Service [-Force] [-InputObject] <ServiceController[]> [-PassThru] [-Include <String[]>] [-Exclude <String[]>] [-WhatIf] [-Confirm] [<CommonParameters>]
Restart-Service [-Force] [-Name] <String[]> [-PassThru] [-Include <String[]>] [-Exclude <String[]>] [-WhatIf] [-Confirm] [<CommonParameters>]
Restart-Service [-Force] [-PassThru] -DisplayName <String[]> [-Include <String[]>] [-Exclude <String[]>] [-WhatIf] [-Confirm] [<CommonParameters>]
Resume-Service [-InputObject] <ServiceController[]> [-PassThru] [-Include <String[]>] [-Exclude <String[]>] [-WhatIf] [-Confirm] [<CommonParameters>]
Resume-Service [-Name] <String[]> [-PassThru] [-Include <String[]>] [-Exclude <String[]>] [-WhatIf] [-Confirm] [<CommonParameters>]
Resume-Service [-PassThru] -DisplayName <String[]> [-Include <String[]>] [-Exclude <String[]>] [-WhatIf] [-Confirm] [<CommonParameters>]
Set-Clipboard -Value <String[]> [-Append] [-WhatIf] [-Confirm] [-PassThru] [<CommonParameters>]
Set-Content [-Path] <string[]> [-Value] <Object[]> [-PassThru] [-Filter <string>] [-Include <string[]>] [-Exclude <string[]>] [-Force] [-Credential <pscredential>] [-WhatIf] [-Confirm] [-NoNewline] [-Encoding <Encoding>] [-AsByteStream] [-Stream <string>] [<CommonParameters>]
Set-Content [-Value] <Object[]> -LiteralPath <string[]> [-PassThru] [-Filter <string>] [-Include <string[]>] [-Exclude <string[]>] [-Force] [-Credential <pscredential>] [-WhatIf] [-Confirm] [-NoNewline] [-Encoding <Encoding>] [-AsByteStream] [-Stream <string>] [<CommonParameters>]
Set-Item [-Path] <String[]> [[-Value] <Object>] [-Force] [-PassThru] [-Filter <String>] [-Include <String[]>] [-Exclude <String[]>] [-Credential <PSCredential>] [-WhatIf] [-Confirm] [<CommonParameters>]
Set-Item -LiteralPath <String[]> [[-Value] <Object>] [-Force] [-PassThru] [-Filter <String>] [-Include <String[]>] [-Exclude <String[]>] [-Credential <PSCredential>] [-WhatIf] [-Confirm] [<CommonParameters>]
Set-ItemProperty [-Path] <String[]> [-Name] <String> [-Value] <Object> [-PassThru] [-Force] [-Filter <String>] [-Include <String[]>] [-Exclude <String[]>] [-Credential <PSCredential>] [-WhatIf] [-Confirm] [-Type <RegistryValueKind>] [<CommonParameters>]
Set-ItemProperty [-Path] <String[]> -InputObject <PSObject> [-PassThru] [-Force] [-Filter <String>] [-Include <String[]>] [-Exclude <String[]>] [-Credential <PSCredential>] [-WhatIf] [-Confirm] [-Type <RegistryValueKind>] [<CommonParameters>]
Set-ItemProperty -LiteralPath <String[]> [-Name] <String> [-Value] <Object> [-PassThru] [-Force] [-Filter <String>] [-Include <String[]>] [-Exclude <String[]>] [-Credential <PSCredential>] [-WhatIf] [-Confirm] [-Type <RegistryValueKind>] [<CommonParameters>]
Set-ItemProperty -LiteralPath <String[]> -InputObject <PSObject> [-PassThru] [-Force] [-Filter <String>] [-Include <String[]>] [-Exclude <String[]>] [-Credential <PSCredential>] [-WhatIf] [-Confirm] [-Type <RegistryValueKind>] [<CommonParameters>]
Set-Location [[-Path] <String>] [-PassThru] [<CommonParameters>]
Set-Location -LiteralPath <String> [-PassThru] [<CommonParameters>]
Set-Location [-PassThru] [-StackName <String>] [<CommonParameters>]
Set-Service [-Name] <String> [-DisplayName <String>] [-Credential <PSCredential>] [-Description <String>] [-StartupType <ServiceStartupType>] [-Status <String>] [-SecurityDescriptorSddl <String>] [-Force] [-PassThru] [-WhatIf] [-Confirm] [<CommonParameters>]
Set-Service [-InputObject] <ServiceController> [-DisplayName <String>] [-Credential <PSCredential>] [-Description <String>] [-StartupType <ServiceStartupType>] [-SecurityDescriptorSddl <String>] [-Status <String>] [-Force] [-PassThru] [-WhatIf] [-Confirm] [<CommonParameters>]
Set-TimeZone [-Name] <String> [-PassThru] [-WhatIf] [-Confirm] [<CommonParameters>]
Set-TimeZone -Id <String> [-PassThru] [-WhatIf] [-Confirm] [<CommonParameters>]
Set-TimeZone [-InputObject] <TimeZoneInfo> [-PassThru] [-WhatIf] [-Confirm] [<CommonParameters>]
Split-Path [-Path] <String[]> [-Parent] [-Resolve] [-Credential <PSCredential>] [<CommonParameters>]
Split-Path [-Path] <String[]> -Leaf [-Resolve] [-Credential <PSCredential>] [<CommonParameters>]
Split-Path [-Path] <String[]> -LeafBase [-Resolve] [-Credential <PSCredential>] [<CommonParameters>]
Start-Process [-FilePath] <String> [[-ArgumentList] <String[]>] [-Credential <PSCredential>] [-WorkingDirectory <String>] [-LoadUserProfile] [-NoNewWindow] [-PassThru] [-RedirectStandardError <String>] [-RedirectStandardInput <String>] [-RedirectStandardOutput <String>] [-WindowStyle <ProcessWindowStyle>] [-Wait] [-UseNewEnvironment] [-WhatIf] [-Confirm] [<CommonParameters>]
Start-Process [-FilePath] <String> [[-ArgumentList] <String[]>] [-WorkingDirectory <String>] [-PassThru] [-Verb <String>] [-WindowStyle <ProcessWindowStyle>] [-Wait] [-WhatIf] [-Confirm] [<CommonParameters>]
Start-Service [-InputObject] <ServiceController[]> [-PassThru] [-Include <String[]>] [-Exclude <String[]>] [-WhatIf] [-Confirm] [<CommonParameters>]
Start-Service [-Name] <String[]> [-PassThru] [-Include <String[]>] [-Exclude <String[]>] [-WhatIf] [-Confirm] [<CommonParameters>]
Start-Service [-PassThru] -DisplayName <String[]> [-Include <String[]>] [-Exclude <String[]>] [-WhatIf] [-Confirm] [<CommonParameters>]
Stop-Computer [-WsmanAuthentication <String>] [[-ComputerName] <String[]>] [[-Credential] <PSCredential>] [-Force] [-WhatIf] [-Confirm] [<CommonParameters>]
Stop-Process [-Id] <Int32[]> [-PassThru] [-Force] [-WhatIf] [-Confirm] [<CommonParameters>]
Stop-Process -Name <String[]> [-PassThru] [-Force] [-WhatIf] [-Confirm] [<CommonParameters>]
Stop-Process [-InputObject] <Process[]> [-PassThru] [-Force] [-WhatIf] [-Confirm] [<CommonParameters>]
Stop-Service [-Force] [-NoWait] [-InputObject] <ServiceController[]> [-PassThru] [-Include <String[]>] [-Exclude <String[]>] [-WhatIf] [-Confirm] [<CommonParameters>]
Stop-Service [-Force] [-NoWait] [-Name] <String[]> [-PassThru] [-Include <String[]>] [-Exclude <String[]>] [-WhatIf] [-Confirm] [<CommonParameters>]
Stop-Service [-Force] [-NoWait] [-PassThru] -DisplayName <String[]> [-Include <String[]>] [-Exclude <String[]>] [-WhatIf] [-Confirm] [<CommonParameters>]
Suspend-Service [-InputObject] <ServiceController[]> [-PassThru] [-Include <String[]>] [-Exclude <String[]>] [-WhatIf] [-Confirm] [<CommonParameters>]
Suspend-Service [-Name] <String[]> [-PassThru] [-Include <String[]>] [-Exclude <String[]>] [-WhatIf] [-Confirm] [<CommonParameters>]
Suspend-Service [-PassThru] -DisplayName <String[]> [-Include <String[]>] [-Exclude <String[]>] [-WhatIf] [-Confirm] [<CommonParameters>]
Test-Connection [-TargetName] <string[]> [-Ping] [-IPv4] [-IPv6] [-ResolveDestination] [-Source <string>] [-MaxHops <int>] [-Count <int>] [-Delay <int>] [-BufferSize <int>] [-DontFragment] [-TimeoutSeconds <int>] [-Quiet] [<CommonParameters>]
Test-Connection [-TargetName] <string[]> -Repeat [-Ping] [-IPv4] [-IPv6] [-ResolveDestination] [-Source <string>] [-MaxHops <int>] [-Delay <int>] [-BufferSize <int>] [-DontFragment] [-TimeoutSeconds <int>] [-Quiet] [<CommonParameters>]
Test-Connection [-TargetName] <string[]> -MtuSize [-IPv4] [-IPv6] [-ResolveDestination] [-TimeoutSeconds <int>] [-Quiet] [<CommonParameters>]
Test-Connection [-TargetName] <string[]> -Traceroute [-IPv4] [-IPv6] [-ResolveDestination] [-Source <string>] [-MaxHops <int>] [-TimeoutSeconds <int>] [-Quiet] [<CommonParameters>]
Test-Connection [-TargetName] <string[]> -TcpPort <int> [-IPv4] [-IPv6] [-ResolveDestination] [-Source <string>] [-TimeoutSeconds <int>] [-Quiet] [<CommonParameters>]
Test-Path [-Path] <String[]> [-Filter <String>] [-Include <String[]>] [-Exclude <String[]>] [-PathType <TestPathType>] [-IsValid] [-Credential <PSCredential>] [-OlderThan <DateTime>] [-NewerThan <DateTime>] [<CommonParameters>]
Test-Path -LiteralPath <String[]> [-Filter <String>] [-Include <String[]>] [-Exclude <String[]>] [-PathType <TestPathType>] [-IsValid] [-Credential <PSCredential>] [-OlderThan <DateTime>] [-NewerThan <DateTime>] [<CommonParameters>]
Wait-Process [-Name] <String[]> [[-Timeout] <Int32>] [<CommonParameters>]
Wait-Process [-Id] <Int32[]> [[-Timeout] <Int32>] [<CommonParameters>]
Wait-Process [[-Timeout] <Int32>] -InputObject <Process[]> [<CommonParameters>]
}

{ # Microsoft.PowerShell.Security
ConvertFrom-SecureString [-SecureString] <SecureString> [[-SecureKey] <SecureString>] [<CommonParameters>]
ConvertFrom-SecureString [-SecureString] <SecureString> [-AsPlainText] [<CommonParameters>]
ConvertFrom-SecureString [-SecureString] <SecureString> [-Key <Byte[]>] [<CommonParameters>]
ConvertTo-SecureString [-String] <String> [[-SecureKey] <SecureString>] [<CommonParameters>]
ConvertTo-SecureString [-String] <String> [-AsPlainText] [-Force] [<CommonParameters>]
ConvertTo-SecureString [-String] <String> [-Key <Byte[]>] [<CommonParameters>]
Get-Acl [[-Path] <String[]>] [-Audit] [-Filter <String>] [-Include <String[]>] [-Exclude <String[]>] [<CommonParameters>]
Get-Acl -InputObject <PSObject> [-Audit] [-Filter <String>] [-Include <String[]>] [-Exclude <String[]>] [<CommonParameters>]
Get-Acl [-LiteralPath <String[]>] [-Audit] [-Filter <String>] [-Include <String[]>] [-Exclude <String[]>] [<CommonParameters>]
Get-AuthenticodeSignature [-FilePath] <String[]> [<CommonParameters>]
Get-AuthenticodeSignature -LiteralPath <String[]> [<CommonParameters>]
Get-AuthenticodeSignature -SourcePathOrExtension <String[]> -Content <Byte[]> [<CommonParameters>]
Get-CmsMessage [-Content] <String> [<CommonParameters>]
Get-CmsMessage [-Path] <String> [<CommonParameters>]
Get-CmsMessage [-LiteralPath] <String> [<CommonParameters>]
Get-Credential [[-Credential] <PSCredential>] [<CommonParameters>]
Get-Credential [-Message <String>] [[-UserName] <String>] [-Title <String>] [<CommonParameters>]
Get-ExecutionPolicy [[-Scope] <ExecutionPolicyScope>] [-List] [<CommonParameters>]
Get-PfxCertificate [-FilePath] <String[]> [-Password <SecureString>] [-NoPromptForPassword] [<CommonParameters>]
Get-PfxCertificate -LiteralPath <String[]> [-Password <SecureString>] [-NoPromptForPassword] [<CommonParameters>]
New-FileCatalog [-CatalogVersion <Int32>] [-CatalogFilePath] <String> [[-Path] <String[]>] [-WhatIf] [-Confirm] [<CommonParameters>]
Protect-CmsMessage [-To] <CmsMessageRecipient[]> [-Content] <PSObject> [[-OutFile] <String>] [<CommonParameters>]
Protect-CmsMessage [-To] <CmsMessageRecipient[]> [-Path] <String> [[-OutFile] <String>] [<CommonParameters>]
Protect-CmsMessage [-To] <CmsMessageRecipient[]> [-LiteralPath] <String> [[-OutFile] <String>] [<CommonParameters>]
Set-Acl [-Path] <String[]> [-AclObject] <Object> [-ClearCentralAccessPolicy] [-Passthru] [-Filter <String>] [-Include <String[]>] [-Exclude <String[]>] [-WhatIf] [-Confirm] [<CommonParameters>]
Set-Acl [-InputObject] <PSObject> [-AclObject] <Object> [-Passthru] [-Filter <String>] [-Include <String[]>] [-Exclude <String[]>] [-WhatIf] [-Confirm] [<CommonParameters>]
Set-Acl -LiteralPath <String[]> [-AclObject] <Object> [-ClearCentralAccessPolicy] [-Passthru] [-Filter <String>] [-Include <String[]>] [-Exclude <String[]>] [-WhatIf] [-Confirm] [<CommonParameters>]
Set-AuthenticodeSignature [-Certificate] <X509Certificate2> [-IncludeChain <String>] [-TimestampServer <String>] [-HashAlgorithm <String>] [-Force] [-FilePath] <String[]> [-WhatIf] [-Confirm] [<CommonParameters>]
Set-AuthenticodeSignature [-Certificate] <X509Certificate2> [-IncludeChain <String>] [-TimestampServer <String>] [-HashAlgorithm <String>] [-Force] -LiteralPath <String[]> [-WhatIf] [-Confirm] [<CommonParameters>]
Set-AuthenticodeSignature [-Certificate] <X509Certificate2> [-IncludeChain <String>] [-TimestampServer <String>] [-HashAlgorithm <String>] [-Force] -SourcePathOrExtension <String[]> -Content <Byte[]> [-WhatIf] [-Confirm] [<CommonParameters>]
Set-ExecutionPolicy [-ExecutionPolicy] <ExecutionPolicy> [[-Scope] <ExecutionPolicyScope>] [-Force] [-WhatIf] [-Confirm] [<CommonParameters>]
Test-FileCatalog [-Detailed] [-FilesToSkip <String[]>] [-CatalogFilePath] <String> [[-Path] <String[]>] [-WhatIf] [-Confirm] [<CommonParameters>]
Unprotect-CmsMessage [-EventLogRecord] <PSObject> [-IncludeContext] [[-To] <CmsMessageRecipient[]>] [<CommonParameters>]
Unprotect-CmsMessage [-Content] <String> [-IncludeContext] [[-To] <CmsMessageRecipient[]>] [<CommonParameters>]
Unprotect-CmsMessage [-Path] <String> [-IncludeContext] [[-To] <CmsMessageRecipient[]>] [<CommonParameters>]
Unprotect-CmsMessage [-LiteralPath] <String> [-IncludeContext] [[-To] <CmsMessageRecipient[]>] [<CommonParameters>]
}

{ # Microsoft.PowerShell.Utility
Add-Member -InputObject <PSObject> -TypeName <String> [-PassThru] [<CommonParameters>]
Add-Member -InputObject <PSObject> [-TypeName <String>] [-Force] [-PassThru] [-NotePropertyMembers] <IDictionary> [<CommonParameters>]
Add-Member -InputObject <PSObject> [-TypeName <String>] [-Force] [-PassThru] [-NotePropertyName] <String> [-NotePropertyValue] <Object> [<CommonParameters>]
Add-Member -InputObject <PSObject> [-MemberType] <PSMemberTypes> [-Name] <String> [[-Value] <Object>] [[-SecondValue] <Object>] [-TypeName <String>] [-Force] [-PassThru] [<CommonParameters>]
Add-Type [-TypeDefinition] <String> [-Language <Language>] [-ReferencedAssemblies <String[]>] [-OutputAssembly <String>] [-OutputType <OutputAssemblyType>] [-PassThru] [-IgnoreWarnings] [-CompilerOptions <String[]>] [<CommonParameters>]
Add-Type [-Name] <String> [-MemberDefinition] <String[]> [-Namespace <String>] [-UsingNamespace <String[]>] [-Language <Language>] [-ReferencedAssemblies <String[]>] [-OutputAssembly <String>] [-OutputType <OutputAssemblyType>] [-PassThru] [-IgnoreWarnings] [-CompilerOptions <String[]>] [<CommonParameters>]
Add-Type [-Path] <String[]> [-ReferencedAssemblies <String[]>] [-OutputAssembly <String>] [-OutputType <OutputAssemblyType>] [-PassThru] [-IgnoreWarnings] [-CompilerOptions <String[]>] [<CommonParameters>]
Add-Type -LiteralPath <String[]> [-ReferencedAssemblies <String[]>] [-OutputAssembly <String>] [-OutputType <OutputAssemblyType>] [-PassThru] [-IgnoreWarnings] [-CompilerOptions <String[]>] [<CommonParameters>]
Add-Type -AssemblyName <String[]> [-PassThru] [<CommonParameters>]
Clear-Variable [-Name] <String[]> [-Include <String[]>] [-Exclude <String[]>] [-Force] [-PassThru] [-Scope <String>] [-WhatIf] [-Confirm] [<CommonParameters>]
Compare-Object [-ReferenceObject] <PSObject[]> [-DifferenceObject] <PSObject[]> [-SyncWindow <Int32>] [-Property <Object[]>] [-ExcludeDifferent] [-IncludeEqual] [-PassThru] [-Culture <String>] [-CaseSensitive] [<CommonParameters>]
ConvertFrom-Csv [[-Delimiter] <Char>] [-InputObject] <PSObject[]> [-Header <String[]>] [<CommonParameters>]
ConvertFrom-Csv -UseCulture [-InputObject] <PSObject[]> [-Header <String[]>] [<CommonParameters>]
ConvertFrom-Json [-InputObject] <String> [-AsHashtable] [-Depth <Int32>] [-NoEnumerate] [<CommonParameters>]
ConvertFrom-Markdown [-Path] <String[]> [-AsVT100EncodedString] [<CommonParameters>]
ConvertFrom-Markdown -LiteralPath <String[]> [-AsVT100EncodedString] [<CommonParameters>]
ConvertFrom-Markdown -InputObject <PSObject> [-AsVT100EncodedString] [<CommonParameters>]
ConvertFrom-SddlString [-Sddl] <String> [-Type <AccessRightTypeNames>] [<CommonParameters>]
ConvertFrom-StringData [-StringData] <String> [[-Delimiter] <Char>] [<CommonParameters>]
ConvertTo-Csv [-InputObject] <PSObject> [[-Delimiter] <Char>] [-IncludeTypeInformation] [-NoTypeInformation] [-QuoteFields <String[]>] [-UseQuotes <QuoteKind>] [<CommonParameters>]
ConvertTo-Csv [-InputObject] <PSObject> [-UseCulture] [-IncludeTypeInformation] [-NoTypeInformation] [-QuoteFields <String[]>] [-UseQuotes <QuoteKind>] [<CommonParameters>]
ConvertTo-Html [-InputObject <PSObject>] [[-Property] <Object[]>] [[-Body] <String[]>] [[-Head] <String[]>] [[-Title] <String>] [-As <String>] [-CssUri <Uri>] [-PostContent <String[]>] [-PreContent <String[]>] [-Meta <Hashtable>] [-Charset <String>] [-Transitional] [<CommonParameters>]
ConvertTo-Html [-InputObject <PSObject>] [[-Property] <Object[]>] [-As <String>] [-Fragment] [-PostContent <String[]>] [-PreContent <String[]>] [<CommonParameters>]
ConvertTo-Json [-InputObject] <Object> [-Depth <Int32>] [-Compress] [-EnumsAsStrings] [-AsArray] [-EscapeHandling <StringEscapeHandling>] [<CommonParameters>]
ConvertTo-Xml [-Depth <Int32>] [-InputObject] <PSObject> [-NoTypeInformation] [-As <String>] [<CommonParameters>]
Debug-Runspace [-Runspace] <Runspace> [-BreakAll] [-WhatIf] [-Confirm] [<CommonParameters>]
Debug-Runspace [-Name] <String> [-BreakAll] [-WhatIf] [-Confirm] [<CommonParameters>]
Debug-Runspace [-Id] <Int32> [-BreakAll] [-WhatIf] [-Confirm] [<CommonParameters>]
Debug-Runspace [-InstanceId] <Guid> [-BreakAll] [-WhatIf] [-Confirm] [<CommonParameters>]
Disable-PSBreakpoint [-PassThru] [-Breakpoint] <Breakpoint[]> [-WhatIf] [-Confirm] [<CommonParameters>]
Disable-PSBreakpoint [-PassThru] [-Id] <Int32[]> [-Runspace <Runspace>] [-WhatIf] [-Confirm] [<CommonParameters>]
Disable-RunspaceDebug [[-RunspaceName] <String[]>] [<CommonParameters>]
Disable-RunspaceDebug [-Runspace] <Runspace[]> [<CommonParameters>]
Disable-RunspaceDebug [-RunspaceId] <Int32[]> [<CommonParameters>]
Disable-RunspaceDebug [-RunspaceInstanceId] <Guid[]> [<CommonParameters>]
Disable-RunspaceDebug [[-ProcessName] <String>] [[-AppDomainName] <String[]>] [<CommonParameters>]
Enable-PSBreakpoint [-PassThru] [-Breakpoint] <Breakpoint[]> [-WhatIf] [-Confirm] [<CommonParameters>]
Enable-PSBreakpoint [-PassThru] [-Id] <Int32[]> [-Runspace <Runspace>] [-WhatIf] [-Confirm] [<CommonParameters>]
Enable-RunspaceDebug [-BreakAll] [[-RunspaceName] <String[]>] [<CommonParameters>]
Enable-RunspaceDebug [-BreakAll] [-Runspace] <Runspace[]> [<CommonParameters>]
Enable-RunspaceDebug [-BreakAll] [-RunspaceId] <Int32[]> [<CommonParameters>]
Enable-RunspaceDebug [-RunspaceInstanceId] <Guid[]> [<CommonParameters>]
Enable-RunspaceDebug [-RunspaceInstanceId] <Guid[]> [<CommonParameters>]
Enable-RunspaceDebug [[-ProcessName] <String>] [[-AppDomainName] <String[]>] [<CommonParameters>]
Export-Alias [-Path] <String> [[-Name] <String[]>] [-PassThru] [-As <ExportAliasFormat>] [-Append] [-Force] [-NoClobber] [-Description <String>] [-Scope <String>] [-WhatIf] [-Confirm] [<CommonParameters>]
Export-Alias -LiteralPath <String> [[-Name] <String[]>] [-PassThru] [-As <ExportAliasFormat>] [-Append] [-Force] [-NoClobber] [-Description <String>] [-Scope <String>] [-WhatIf] [-Confirm] [<CommonParameters>]
Export-Clixml [-Depth <Int32>] [-Path] <String> -InputObject <PSObject> [-Force] [-NoClobber] [-Encoding <Encoding>] [-WhatIf] [-Confirm] [<CommonParameters>]
Export-Clixml [-Depth <Int32>] -LiteralPath <String> -InputObject <PSObject> [-Force] [-NoClobber] [-Encoding <Encoding>] [-WhatIf] [-Confirm] [<CommonParameters>]
Export-Csv -InputObject <PSObject> [[-Path] <String>] [-LiteralPath <String>] [-Force] [-NoClobber] [-Encoding <Encoding>] [-Append] [[-Delimiter] <Char>] [-IncludeTypeInformation] [-NoTypeInformation] [-QuoteFields <String[]>] [-UseQuotes <QuoteKind>] [-WhatIf] [-Confirm] [<CommonParameters>]
Export-Csv -InputObject <PSObject> [[-Path] <String>] [-LiteralPath <String>] [-Force] [-NoClobber] [-Encoding <Encoding>] [-Append] [-UseCulture] [-IncludeTypeInformation] [-NoTypeInformation] [-QuoteFields <String[]>] [-UseQuotes <QuoteKind>] [-WhatIf] [-Confirm] [<CommonParameters>]
Export-FormatData -InputObject <ExtendedTypeDefinition[]> -Path <String> [-Force] [-NoClobber] [-IncludeScriptBlock] [<CommonParameters>]
Export-FormatData -InputObject <ExtendedTypeDefinition[]> -LiteralPath <String> [-Force] [-NoClobber] [-IncludeScriptBlock] [<CommonParameters>]
Export-PSSession [-OutputModule] <String> [-Force] [-Encoding <Encoding>] [[-CommandName] <String[]>] [-AllowClobber] [-ArgumentList <Object[]>] [-CommandType <CommandTypes>] [-Module <String[]>] [-FullyQualifiedModule <ModuleSpecification[]>] [[-FormatTypeName] <String[]>] [-Certificate <X509Certificate2>] [-Session] <PSSession> [<CommonParameters>]
Format-Custom [[-Property] <Object[]>] [-Depth <Int32>] [-GroupBy <Object>] [-View <String>] [-ShowError] [-DisplayError] [-Force] [-Expand <String>] [-InputObject <PSObject>] [<CommonParameters>]
Format-Hex [-Path] <String[]> [-Count <Int64>] [-Offset <Int64>] [<CommonParameters>]
Format-Hex -LiteralPath <String[]> [-Count <Int64>] [-Offset <Int64>] [<CommonParameters>]
Format-Hex -InputObject <PSObject> [-Encoding <Encoding>] [-Count <Int64>] [-Offset <Int64>] [-Raw] [<CommonParameters>]
Format-List [[-Property] <Object[]>] [-GroupBy <Object>] [-View <string>] [-ShowError] [-DisplayError] [-Force] [-Expand <string>] [-InputObject <psobject>] [<CommonParameters>]
Format-Table [-AutoSize] [-RepeatHeader] [-HideTableHeaders] [-Wrap] [[-Property] <Object[]>] [-GroupBy <Object>] [-View <String>] [-ShowError] [-DisplayError] [-Force] [-Expand <String>] [-InputObject <PSObject>] [<CommonParameters>]
Format-Wide [[-Property] <Object>] [-AutoSize] [-Column <int>] [-GroupBy <Object>] [-View <string>] [-ShowError] [-DisplayError] [-Force] [-Expand <string>] [-InputObject <psobject>] [<CommonParameters>]
Get-Alias [[-Name] <String[]>] [-Exclude <String[]>] [-Scope <String>] [<CommonParameters>]
Get-Alias [-Exclude <String[]>] [-Scope <String>] [-Definition <String[]>] [<CommonParameters>]
Get-Culture [-NoUserOverrides] [<CommonParameters>]
Get-Culture [-Name <String[]>] [-NoUserOverrides] [<CommonParameters>]
Get-Culture [-ListAvailable] [<CommonParameters>]
Get-Date [[-Date] <DateTime>] [-Year <Int32>] [-Month <Int32>] [-Day <Int32>] [-Hour <Int32>] [-Minute <Int32>] [-Second <Int32>] [-Millisecond <Int32>] [-DisplayHint <DisplayHintType>] [-Format <String>] [-AsUTC] [<CommonParameters>]
Get-Date [[-Date] <DateTime>] [-Year <Int32>] [-Month <Int32>] [-Day <Int32>] [-Hour <Int32>] [-Minute <Int32>] [-Second <Int32>] [-Millisecond <Int32>] [-DisplayHint <DisplayHintType>] -UFormat <String> [<CommonParameters>]
Get-Date -UnixTimeSeconds <Int64> [-Year <Int32>] [-Month <Int32>] [-Day <Int32>] [-Hour <Int32>] [-Minute <Int32>] [-Second <Int32>] [-Millisecond <Int32>] [-DisplayHint <DisplayHintType>] [-Format <String>] [-AsUTC] [<CommonParameters>]
Get-Date -UnixTimeSeconds <Int64> [-Year <Int32>] [-Month <Int32>] [-Day <Int32>] [-Hour <Int32>] [-Minute <Int32>] [-Second <Int32>] [-Millisecond <Int32>] [-DisplayHint <DisplayHintType>] -UFormat <String> [<CommonParameters>]
Get-Error [[-Newest] <Int32>] [<CommonParameters>]
Get-Error [-InputObject <PSObject>] [<CommonParameters>]
Get-Event [[-SourceIdentifier] <String>] [<CommonParameters>]
Get-Event [-EventIdentifier] <Int32> [<CommonParameters>]
Get-EventSubscriber [[-SourceIdentifier] <String>] [-Force] [<CommonParameters>]
Get-EventSubscriber [-SubscriptionId] <Int32> [-Force] [<CommonParameters>]
Get-FileHash [-Path] <String[]> [[-Algorithm] <String>] [<CommonParameters>]
Get-FileHash [-LiteralPath] <String[]> [[-Algorithm] <String>] [<CommonParameters>]
Get-FileHash [-InputStream] <Stream> [[-Algorithm] <String>] [<CommonParameters>]
Get-FormatData [[-TypeName] <String[]>] [-PowerShellVersion <Version>] [<CommonParameters>]
Get-Host []
Get-MarkdownOption []
Get-Member [-InputObject <PSObject>] [[-Name] <String[]>] [-MemberType <PSMemberTypes>] [-View <PSMemberViewTypes>] [-Static] [-Force] [<CommonParameters>]
Get-PSBreakpoint [[-Script] <String[]>] [-Runspace <Runspace>] [<CommonParameters>]
Get-PSBreakpoint [[-Script] <String[]>] -Command <String[]> [-Runspace <Runspace>] [<CommonParameters>]
Get-PSBreakpoint [[-Script] <String[]>] -Variable <String[]> [-Runspace <Runspace>] [<CommonParameters>]
Get-PSBreakpoint [[-Script] <String[]>] [-Type] <BreakpointType[]> [-Runspace <Runspace>] [<CommonParameters>]
Get-PSBreakpoint [-Id] <Int32[]> [-Runspace <Runspace>] [<CommonParameters>]
Get-PSCallStack []
Get-Random [-SetSeed <Int32>] [[-Maximum] <Object>] [-Minimum <Object>] [-Count <Int32>] [<CommonParameters>]
Get-Random [-SetSeed <Int32>] [-InputObject] <Object[]> [-Count <Int32>] [<CommonParameters>]
Get-Random [-SetSeed <Int32>] [-InputObject] <Object[]> [-Shuffle] [<CommonParameters>]
Get-Runspace [[-Name] <String[]>] [<CommonParameters>]
Get-Runspace [-Id] <Int32[]> [<CommonParameters>]
Get-Runspace [-InstanceId] <Guid[]> [<CommonParameters>]
Get-RunspaceDebug [[-RunspaceName] <String[]>] [<CommonParameters>]
Get-RunspaceDebug [-Runspace] <Runspace[]> [<CommonParameters>]
Get-RunspaceDebug [-RunspaceId] <Int32[]> [<CommonParameters>]
Get-RunspaceDebug [-RunspaceInstanceId] <Guid[]> [<CommonParameters>]
Get-RunspaceDebug [[-ProcessName] <String>] [[-AppDomainName] <String[]>] [<CommonParameters>]
Get-TraceSource [[-Name] <String[]>] [<CommonParameters>]
Get-TypeData [[-TypeName] <String[]>] [<CommonParameters>]
Get-UICulture []
Get-Unique [-InputObject <PSObject>] [-AsString] [<CommonParameters>]
Get-Unique [-InputObject <PSObject>] [-OnType] [<CommonParameters>]
Get-Uptime []
Get-Uptime [-Since] [<CommonParameters>]
Get-Variable [[-Name] <String[]>] [-ValueOnly] [-Include <String[]>] [-Exclude <String[]>] [-Scope <String>] [<CommonParameters>]
Get-Verb [[-Verb] <String[]>] [[-Group] <String[]>] [<CommonParameters>]
Group-Object [-NoElement] [-AsHashTable] [-AsString] [-InputObject <PSObject>] [[-Property] <Object[]>] [-Culture <String>] [-CaseSensitive] [<CommonParameters>]
Import-Alias [-Path] <String> [-Scope <String>] [-PassThru] [-Force] [-WhatIf] [-Confirm] [<CommonParameters>]
Import-Alias -LiteralPath <String> [-Scope <String>] [-PassThru] [-Force] [-WhatIf] [-Confirm] [<CommonParameters>]
Import-Clixml [-Path] <String[]> [-IncludeTotalCount] [-Skip <UInt64>] [-First <UInt64>] [<CommonParameters>]
Import-Clixml -LiteralPath <String[]> [-IncludeTotalCount] [-Skip <UInt64>] [-First <UInt64>] [<CommonParameters>]
Import-Csv [[-Delimiter] <Char>] [-Path] <String[]> [-Header <String[]>] [-Encoding <Encoding>] [<CommonParameters>]
Import-Csv [[-Delimiter] <Char>] -LiteralPath <String[]> [-Header <String[]>] [-Encoding <Encoding>] [<CommonParameters>]
Import-Csv [-Path] <String[]> -UseCulture [-Header <String[]>] [-Encoding <Encoding>] [<CommonParameters>]
Import-Csv -LiteralPath <String[]> -UseCulture [-Header <String[]>] [-Encoding <Encoding>] [<CommonParameters>]
Import-LocalizedData [[-BindingVariable] <String>] [[-UICulture] <String>] [-BaseDirectory <String>] [-FileName <String>] [-SupportedCommand <String[]>] [<CommonParameters>]
Import-PowerShellDataFile [-Path] <String[]> [-SkipLimitCheck] [<CommonParameters>]
Import-PowerShellDataFile [-LiteralPath] <String[]> [-SkipLimitCheck] [<CommonParameters>]
Import-PSSession [-Prefix <String>] [-DisableNameChecking] [[-CommandName] <String[]>] [-AllowClobber] [-ArgumentList <Object[]>] [-CommandType <CommandTypes>] [-Module <String[]>] [-FullyQualifiedModule <ModuleSpecification[]>] [[-FormatTypeName] <String[]>] [-Certificate <X509Certificate2>] [-Session] <PSSession> [<CommonParameters>]
Invoke-Expression [-Command] <String> [<CommonParameters>]
Invoke-RestMethod [-Method <WebRequestMethod>] [-FollowRelLink] [-MaximumFollowRelLink <Int32>] [-ResponseHeadersVariable <String>] [-StatusCodeVariable <String>] [-UseBasicParsing] [-Uri] <Uri> [-WebSession <WebRequestSession>] [-SessionVariable <String>] [-AllowUnencryptedAuthentication] [-Authentication <WebAuthenticationType>] [-Credential <PSCredential>] [-UseDefaultCredentials] [-CertificateThumbprint <String>] [-Certificate <X509Certificate>] [-SkipCertificateCheck] [-SslProtocol <WebSslProtocol>] [-Token <SecureString>] [-UserAgent <String>] [-DisableKeepAlive] [-TimeoutSec <Int32>] [-Headers <IDictionary>] [-MaximumRedirection <Int32>] [-MaximumRetryCount <Int32>] [-RetryIntervalSec <Int32>] [-Proxy <Uri>] [-ProxyCredential <PSCredential>] [-ProxyUseDefaultCredentials] [-Body <Object>] [-Form <IDictionary>] [-ContentType <String>] [-TransferEncoding <String>] [-InFile <String>] [-OutFile <String>] [-PassThru] [-Resume] [-SkipHttpErrorCheck] [-PreserveAuthorizationOnRedirect] [-SkipHeaderValidation] [<CommonParameters>]
Invoke-RestMethod [-Method <WebRequestMethod>] [-FollowRelLink] [-MaximumFollowRelLink <Int32>] [-ResponseHeadersVariable <String>] [-StatusCodeVariable <String>] [-UseBasicParsing] [-Uri] <Uri> [-WebSession <WebRequestSession>] [-SessionVariable <String>] [-AllowUnencryptedAuthentication] [-Authentication <WebAuthenticationType>] [-Credential <PSCredential>] [-UseDefaultCredentials] [-CertificateThumbprint <String>] [-Certificate <X509Certificate>] [-SkipCertificateCheck] [-SslProtocol <WebSslProtocol>] [-Token <SecureString>] [-UserAgent <String>] [-DisableKeepAlive] [-TimeoutSec <Int32>] [-Headers <IDictionary>] [-MaximumRedirection <Int32>] [-MaximumRetryCount <Int32>] [-RetryIntervalSec <Int32>] -NoProxy [-Body <Object>] [-Form <IDictionary>] [-ContentType <String>] [-TransferEncoding <String>] [-InFile <String>] [-OutFile <String>] [-PassThru] [-Resume] [-SkipHttpErrorCheck] [-PreserveAuthorizationOnRedirect] [-SkipHeaderValidation] [<CommonParameters>]
Invoke-RestMethod -CustomMethod <String> [-FollowRelLink] [-MaximumFollowRelLink <Int32>] [-ResponseHeadersVariable <String>] [-StatusCodeVariable <String>] [-UseBasicParsing] [-Uri] <Uri> [-WebSession <WebRequestSession>] [-SessionVariable <String>] [-AllowUnencryptedAuthentication] [-Authentication <WebAuthenticationType>] [-Credential <PSCredential>] [-UseDefaultCredentials] [-CertificateThumbprint <String>] [-Certificate <X509Certificate>] [-SkipCertificateCheck] [-SslProtocol <WebSslProtocol>] [-Token <SecureString>] [-UserAgent <String>] [-DisableKeepAlive] [-TimeoutSec <Int32>] [-Headers <IDictionary>] [-MaximumRedirection <Int32>] [-MaximumRetryCount <Int32>] [-RetryIntervalSec <Int32>] -NoProxy [-Body <Object>] [-Form <IDictionary>] [-ContentType <String>] [-TransferEncoding <String>] [-InFile <String>] [-OutFile <String>] [-PassThru] [-Resume] [-SkipHttpErrorCheck] [-PreserveAuthorizationOnRedirect] [-SkipHeaderValidation] [<CommonParameters>]
Invoke-RestMethod -CustomMethod <String> [-FollowRelLink] [-MaximumFollowRelLink <Int32>] [-ResponseHeadersVariable <String>] [-StatusCodeVariable <String>] [-UseBasicParsing] [-Uri] <Uri> [-WebSession <WebRequestSession>] [-SessionVariable <String>] [-AllowUnencryptedAuthentication] [-Authentication <WebAuthenticationType>] [-Credential <PSCredential>] [-UseDefaultCredentials] [-CertificateThumbprint <String>] [-Certificate <X509Certificate>] [-SkipCertificateCheck] [-SslProtocol <WebSslProtocol>] [-Token <SecureString>] [-UserAgent <String>] [-DisableKeepAlive] [-TimeoutSec <Int32>] [-Headers <IDictionary>] [-MaximumRedirection <Int32>] [-MaximumRetryCount <Int32>] [-RetryIntervalSec <Int32>] [-Proxy <Uri>] [-ProxyCredential <PSCredential>] [-ProxyUseDefaultCredentials] [-Body <Object>] [-Form <IDictionary>] [-ContentType <String>] [-TransferEncoding <String>] [-InFile <String>] [-OutFile <String>] [-PassThru] [-Resume] [-SkipHttpErrorCheck] [-PreserveAuthorizationOnRedirect] [-SkipHeaderValidation] [<CommonParameters>]
Invoke-WebRequest [-UseBasicParsing] [-Uri] <Uri> [-WebSession <WebRequestSession>] [-SessionVariable <String>] [-AllowUnencryptedAuthentication] [-Authentication <WebAuthenticationType>] [-Credential <PSCredential>] [-UseDefaultCredentials] [-CertificateThumbprint <String>] [-Certificate <X509Certificate>] [-SkipCertificateCheck] [-SslProtocol <WebSslProtocol>] [-Token <SecureString>] [-UserAgent <String>] [-DisableKeepAlive] [-TimeoutSec <Int32>] [-Headers <IDictionary>] [-MaximumRedirection <Int32>] [-MaximumRetryCount <Int32>] [-RetryIntervalSec <Int32>] [-Method <WebRequestMethod>] [-Proxy <Uri>] [-ProxyCredential <PSCredential>] [-ProxyUseDefaultCredentials] [-Body <Object>] [-Form <IDictionary>] [-ContentType <String>] [-TransferEncoding <String>] [-InFile <String>] [-OutFile <String>] [-PassThru] [-Resume] [-SkipHttpErrorCheck] [-PreserveAuthorizationOnRedirect] [-SkipHeaderValidation] [<CommonParameters>]
Invoke-WebRequest [-UseBasicParsing] [-Uri] <Uri> [-WebSession <WebRequestSession>] [-SessionVariable <String>] [-AllowUnencryptedAuthentication] [-Authentication <WebAuthenticationType>] [-Credential <PSCredential>] [-UseDefaultCredentials] [-CertificateThumbprint <String>] [-Certificate <X509Certificate>] [-SkipCertificateCheck] [-SslProtocol <WebSslProtocol>] [-Token <SecureString>] [-UserAgent <String>] [-DisableKeepAlive] [-TimeoutSec <Int32>] [-Headers <IDictionary>] [-MaximumRedirection <Int32>] [-MaximumRetryCount <Int32>] [-RetryIntervalSec <Int32>] [-Method <WebRequestMethod>] -NoProxy [-Body <Object>] [-Form <IDictionary>] [-ContentType <String>] [-TransferEncoding <String>] [-InFile <String>] [-OutFile <String>] [-PassThru] [-Resume] [-SkipHttpErrorCheck] [-PreserveAuthorizationOnRedirect] [-SkipHeaderValidation] [<CommonParameters>]
Invoke-WebRequest [-UseBasicParsing] [-Uri] <Uri> [-WebSession <WebRequestSession>] [-SessionVariable <String>] [-AllowUnencryptedAuthentication] [-Authentication <WebAuthenticationType>] [-Credential <PSCredential>] [-UseDefaultCredentials] [-CertificateThumbprint <String>] [-Certificate <X509Certificate>] [-SkipCertificateCheck] [-SslProtocol <WebSslProtocol>] [-Token <SecureString>] [-UserAgent <String>] [-DisableKeepAlive] [-TimeoutSec <Int32>] [-Headers <IDictionary>] [-MaximumRedirection <Int32>] [-MaximumRetryCount <Int32>] [-RetryIntervalSec <Int32>] -CustomMethod <String> [-Proxy <Uri>] [-ProxyCredential <PSCredential>] [-ProxyUseDefaultCredentials] [-Body <Object>] [-Form <IDictionary>] [-ContentType <String>] [-TransferEncoding <String>] [-InFile <String>] [-OutFile <String>] [-PassThru] [-Resume] [-SkipHttpErrorCheck] [-PreserveAuthorizationOnRedirect] [-SkipHeaderValidation] [<CommonParameters>]
Invoke-WebRequest [-UseBasicParsing] [-Uri] <Uri> [-WebSession <WebRequestSession>] [-SessionVariable <String>] [-AllowUnencryptedAuthentication] [-Authentication <WebAuthenticationType>] [-Credential <PSCredential>] [-UseDefaultCredentials] [-CertificateThumbprint <String>] [-Certificate <X509Certificate>] [-SkipCertificateCheck] [-SslProtocol <WebSslProtocol>] [-Token <SecureString>] [-UserAgent <String>] [-DisableKeepAlive] [-TimeoutSec <Int32>] [-Headers <IDictionary>] [-MaximumRedirection <Int32>] [-MaximumRetryCount <Int32>] [-RetryIntervalSec <Int32>] -CustomMethod <String> -NoProxy [-Body <Object>] [-Form <IDictionary>] [-ContentType <String>] [-TransferEncoding <String>] [-InFile <String>] [-OutFile <String>] [-PassThru] [-Resume] [-SkipHttpErrorCheck] [-PreserveAuthorizationOnRedirect] [-SkipHeaderValidation] [<CommonParameters>]
Join-String [[-Property] <PSPropertyExpression>] [[-Separator] <String>] [-OutputPrefix <String>] [-OutputSuffix <String>] [-UseCulture] [-InputObject <PSObject[]>] [<CommonParameters>]
Join-String [[-Property] <PSPropertyExpression>] [[-Separator] <String>] [-OutputPrefix <String>] [-OutputSuffix <String>] [-SingleQuote] [-UseCulture] [-InputObject <PSObject[]>] [<CommonParameters>]
Join-String [[-Property] <PSPropertyExpression>] [[-Separator] <String>] [-OutputPrefix <String>] [-OutputSuffix <String>] [-DoubleQuote] [-UseCulture] [-InputObject <PSObject[]>] [<CommonParameters>]
Join-String [[-Property] <PSPropertyExpression>] [[-Separator] <String>] [-OutputPrefix <String>] [-OutputSuffix <String>] [-FormatString <String>] [-UseCulture] [-InputObject <PSObject[]>] [<CommonParameters>]
Measure-Command [-InputObject <PSObject>] [-Expression] <ScriptBlock> [<CommonParameters>]
Measure-Object [[-Property] <PSPropertyExpression[]>] [-InputObject <PSObject>] [-StandardDeviation] [-Sum] [-AllStats] [-Average] [-Maximum] [-Minimum] [<CommonParameters>]
Measure-Object [[-Property] <PSPropertyExpression[]>] [-InputObject <PSObject>] [-Line] [-Word] [-Character] [-IgnoreWhiteSpace] [<CommonParameters>]
New-Alias [-Name] <String> [-Value] <String> [-Description <String>] [-Option <ScopedItemOptions>] [-PassThru] [-Scope <String>] [-Force] [-WhatIf] [-Confirm] [<CommonParameters>]
New-Event [-SourceIdentifier] <String> [[-Sender] <PSObject>] [[-EventArguments] <PSObject[]>] [[-MessageData] <PSObject>] [<CommonParameters>]
New-Guid []
New-Object [-TypeName] <String> [[-ArgumentList] <Object[]>] [-Property <IDictionary>] [<CommonParameters>]
New-Object [-ComObject] <String> [-Strict] [-Property <IDictionary>] [<CommonParameters>]
New-TemporaryFile [-WhatIf] [-Confirm] [<CommonParameters>]
New-TimeSpan [[-Start] <DateTime>] [[-End] <DateTime>] [<CommonParameters>]
New-TimeSpan [-Days <Int32>] [-Hours <Int32>] [-Minutes <Int32>] [-Seconds <Int32>] [<CommonParameters>]
New-Variable [-Name] <String> [[-Value] <Object>] [-Description <String>] [-Option <ScopedItemOptions>] [-Visibility <SessionStateEntryVisibility>] [-Force] [-PassThru] [-Scope <String>] [-WhatIf] [-Confirm] [<CommonParameters>]
Out-File [-FilePath] <string> [[-Encoding] <Encoding>] [-Append] [-Force] [-NoClobber] [-Width <int>] [-NoNewline] [-InputObject <psobject>] [-WhatIf] [-Confirm] [<CommonParameters>]
Out-File [[-Encoding] <Encoding>] -LiteralPath <string> [-Append] [-Force] [-NoClobber] [-Width <int>] [-NoNewline] [-InputObject <psobject>] [-WhatIf] [-Confirm] [<CommonParameters>]
Out-GridView [-InputObject <PSObject>] [-Title <String>] [-PassThru] [<CommonParameters>]
Out-GridView [-InputObject <PSObject>] [-Title <String>] [-Wait] [<CommonParameters>]
Out-GridView [-InputObject <PSObject>] [-Title <String>] [-OutputMode <OutputModeOption>] [<CommonParameters>]
Out-Printer [[-Name] <String>] [-InputObject <PSObject>] [<CommonParameters>]
Out-String [-Width <Int32>] [-NoNewline] [-InputObject <PSObject>] [<CommonParameters>]
Out-String [-Stream] [-Width <Int32>] [-InputObject <PSObject>] [<CommonParameters>]
Read-Host [[-Prompt] <Object>] [-MaskInput] [<CommonParameters>]
Read-Host [[-Prompt] <Object>] [-AsSecureString] [<CommonParameters>]
Register-EngineEvent [-SourceIdentifier] <String> [[-Action] <ScriptBlock>] [-MessageData <PSObject>] [-SupportEvent] [-Forward] [-MaxTriggerCount <Int32>] [<CommonParameters>]
Register-ObjectEvent [-InputObject] <PSObject> [-EventName] <String> [[-SourceIdentifier] <String>] [[-Action] <ScriptBlock>] [-MessageData <PSObject>] [-SupportEvent] [-Forward] [-MaxTriggerCount <Int32>] [<CommonParameters>]
Remove-Alias [-Name] <String[]> [-Scope <String>] [-Force] [<CommonParameters>]
Remove-Event [-SourceIdentifier] <String> [-WhatIf] [-Confirm] [<CommonParameters>]
Remove-Event [-EventIdentifier] <Int32> [-WhatIf] [-Confirm] [<CommonParameters>]
Remove-PSBreakpoint [-Breakpoint] <Breakpoint[]> [-WhatIf] [-Confirm] [<CommonParameters>]
Remove-PSBreakpoint [-Id] <Int32[]> [-Runspace <Runspace>] [-WhatIf] [-Confirm] [<CommonParameters>]
Remove-TypeData -TypeData <TypeData> [-WhatIf] [-Confirm] [<CommonParameters>]
Remove-TypeData [-TypeName] <String> [-WhatIf] [-Confirm] [<CommonParameters>]
Remove-TypeData -Path <String[]> [-WhatIf] [-Confirm] [<CommonParameters>]
Remove-Variable [-Name] <String[]> [-Include <String[]>] [-Exclude <String[]>] [-Force] [-Scope <String>] [-WhatIf] [-Confirm] [<CommonParameters>]
Select-Object [-InputObject <PSObject>] [[-Property] <Object[]>] [-ExcludeProperty <String[]>] [-ExpandProperty <String>] [-Unique] [-Last <Int32>] [-First <Int32>] [-Skip <Int32>] [-Wait] [<CommonParameters>]
Select-Object [-InputObject <PSObject>] [[-Property] <Object[]>] [-ExcludeProperty <String[]>] [-ExpandProperty <String>] [-Unique] [-SkipLast <Int32>] [<CommonParameters>]
Select-Object [-InputObject <PSObject>] [-Unique] [-Wait] [-Index <Int32[]>] [<CommonParameters>]
Select-Object [-InputObject <PSObject>] [-Unique] [-SkipIndex <Int32[]>] [<CommonParameters>]
Select-String [-Culture <String>] [-Pattern] <String[]> [-Path] <String[]> [-SimpleMatch] [-CaseSensitive] [-Quiet] [-List] [-NoEmphasis] [-Include <String[]>] [-Exclude <String[]>] [-NotMatch] [-AllMatches] [-Encoding <Encoding>] [-Context <Int32[]>] [<CommonParameters>]
Select-String [-Culture <String>] -InputObject <PSObject> [-Pattern] <String[]> -Raw [-SimpleMatch] [-CaseSensitive] [-List] [-NoEmphasis] [-Include <String[]>] [-Exclude <String[]>] [-NotMatch] [-AllMatches] [-Encoding <Encoding>] [-Context <Int32[]>] [<CommonParameters>]
Select-String [-Culture <String>] -InputObject <PSObject> [-Pattern] <String[]> [-SimpleMatch] [-CaseSensitive] [-Quiet] [-List] [-NoEmphasis] [-Include <String[]>] [-Exclude <String[]>] [-NotMatch] [-AllMatches] [-Encoding <Encoding>] [-Context <Int32[]>] [<CommonParameters>]
Select-String [-Culture <String>] [-Pattern] <String[]> [-Path] <String[]> -Raw [-SimpleMatch] [-CaseSensitive] [-List] [-NoEmphasis] [-Include <String[]>] [-Exclude <String[]>] [-NotMatch] [-AllMatches] [-Encoding <Encoding>] [-Context <Int32[]>] [<CommonParameters>]
Select-String [-Culture <String>] [-Pattern] <String[]> -LiteralPath <String[]> -Raw [-SimpleMatch] [-CaseSensitive] [-List] [-NoEmphasis] [-Include <String[]>] [-Exclude <String[]>] [-NotMatch] [-AllMatches] [-Encoding <Encoding>] [-Context <Int32[]>] [<CommonParameters>]
Select-String [-Culture <String>] [-Pattern] <String[]> -LiteralPath <String[]> [-SimpleMatch] [-CaseSensitive] [-Quiet] [-List] [-NoEmphasis] [-Include <String[]>] [-Exclude <String[]>] [-NotMatch] [-AllMatches] [-Encoding <Encoding>] [-Context <Int32[]>] [<CommonParameters>]
Select-Xml [-XPath] <string> [-Xml] <XmlNode[]> [-Namespace <hashtable>] [<CommonParameters>]
Select-Xml [-XPath] <string> [-Path] <string[]> [-Namespace <hashtable>] [<CommonParameters>]
Select-Xml [-XPath] <string> -LiteralPath <string[]> [-Namespace <hashtable>] [<CommonParameters>]
Select-Xml [-XPath] <string> -Content <string[]> [-Namespace <hashtable>] [<CommonParameters>]
Send-MailMessage [-Attachments <String[]>] [-Bcc <String[]>] [[-Body] <String>] [-BodyAsHtml] [-Encoding <Encoding>] [-Cc <String[]>] [-DeliveryNotificationOption <DeliveryNotificationOptions>] -From <String> [[-SmtpServer] <String>] [-Priority <MailPriority>] [-ReplyTo <String[]>] [[-Subject] <String>] [-To] <String[]> [-Credential <PSCredential>] [-UseSsl] [-Port <Int32>] [<CommonParameters>]
Set-Alias [-Name] <string> [-Value] <string> [-Description <string>] [-Option <ScopedItemOptions>] [-PassThru] [-Scope <string>] [-Force] [-WhatIf] [-Confirm] [<CommonParameters>]
Set-Date [-Date] <DateTime> [-DisplayHint <DisplayHintType>] [-WhatIf] [-Confirm] [<CommonParameters>]
Set-Date [-Adjust] <TimeSpan> [-DisplayHint <DisplayHintType>] [-WhatIf] [-Confirm] [<CommonParameters>]
Set-MarkdownOption [-Header1Color <String>] [-Header2Color <String>] [-Header3Color <String>] [-Header4Color <String>] [-Header5Color <String>] [-Header6Color <String>] [-Code <String>] [-ImageAltTextForegroundColor <String>] [-LinkForegroundColor <String>] [-ItalicsForegroundColor <String>] [-BoldForegroundColor <String>] [-PassThru] [<CommonParameters>]
Set-MarkdownOption [-PassThru] -Theme <String> [<CommonParameters>]
Set-MarkdownOption [-PassThru] [-InputObject] <PSObject> [<CommonParameters>]
Set-PSBreakpoint [-Action <ScriptBlock>] [[-Column] <Int32>] [-Line] <Int32[]> [-Script] <String[]> [-Runspace <Runspace>] [<CommonParameters>]
Set-PSBreakpoint [-Action <ScriptBlock>] -Command <String[]> [[-Script] <String[]>] [-Runspace <Runspace>] [<CommonParameters>]
Set-PSBreakpoint [-Action <ScriptBlock>] [[-Script] <String[]>] -Variable <String[]> [-Mode <VariableAccessMode>] [-Runspace <Runspace>] [<CommonParameters>]
Set-TraceSource [-Name] <String[]> [[-Option] <PSTraceSourceOptions>] [-ListenerOption <TraceOptions>] [-FilePath <String>] [-Force] [-Debugger] [-PSHost] [-PassThru] [<CommonParameters>]
Set-TraceSource [-Name] <String[]> [-RemoveListener <String[]>] [<CommonParameters>]
Set-TraceSource [-Name] <String[]> [-RemoveFileListener <String[]>] [<CommonParameters>]
Set-Variable [-Name] <String[]> [[-Value] <Object>] [-Include <String[]>] [-Exclude <String[]>] [-Description <String>] [-Option <ScopedItemOptions>] [-Force] [-Visibility <SessionStateEntryVisibility>] [-PassThru] [-Scope <String>] [-WhatIf] [-Confirm] [<CommonParameters>]
Show-Command [[-Name] <String>] [-Height <Double>] [-Width <Double>] [-NoCommonParameter] [-ErrorPopup] [-PassThru] [<CommonParameters>]
Show-Markdown [-Path] <String[]> [-UseBrowser] [<CommonParameters>]
Show-Markdown -InputObject <PSObject> [-UseBrowser] [<CommonParameters>]
Show-Markdown -LiteralPath <String[]> [-UseBrowser] [<CommonParameters>]
Sort-Object [-Stable] [-Descending] [-Unique] [-InputObject <PSObject>] [[-Property] <Object[]>] [-Culture <String>] [-CaseSensitive] [<CommonParameters>]
Sort-Object [-Descending] [-Unique] -Top <Int32> [-InputObject <PSObject>] [[-Property] <Object[]>] [-Culture <String>] [-CaseSensitive] [<CommonParameters>]
Sort-Object [-Descending] [-Unique] -Bottom <Int32> [-InputObject <PSObject>] [[-Property] <Object[]>] [-Culture <String>] [-CaseSensitive] [<CommonParameters>]
Start-Sleep [-Seconds] <Double> [<CommonParameters>]
Start-Sleep -Milliseconds <Int32> [<CommonParameters>]
Tee-Object [-InputObject <PSObject>] [-FilePath] <String> [-Append] [[-Encoding] <Encoding>] [<CommonParameters>]
Tee-Object [-InputObject <PSObject>] -LiteralPath <String> [[-Encoding] <Encoding>] [<CommonParameters>]
Tee-Object [-InputObject <PSObject>] -Variable <String> [<CommonParameters>]
Test-Json [-Json] <String> [<CommonParameters>]
Test-Json [-Json] <String> [[-Schema] <String>] [<CommonParameters>]
Test-Json [-Json] <String> [-SchemaFile <String>] [<CommonParameters>]
Trace-Command [-InputObject <PSObject>] [-Name] <String[]> [[-Option] <PSTraceSourceOptions>] [-Expression] <ScriptBlock> [-ListenerOption <TraceOptions>] [-FilePath <String>] [-Force] [-Debugger] [-PSHost] [<CommonParameters>]
Trace-Command [-InputObject <PSObject>] [-Name] <String[]> [[-Option] <PSTraceSourceOptions>] [-Command] <String> [-ArgumentList <Object[]>] [-ListenerOption <TraceOptions>] [-FilePath <String>] [-Force] [-Debugger] [-PSHost] [<CommonParameters>]
Unblock-File [-Path] <String[]> [-WhatIf] [-Confirm] [<CommonParameters>]
Unblock-File -LiteralPath <String[]> [-WhatIf] [-Confirm] [<CommonParameters>]
Unregister-Event [-SourceIdentifier] <String> [-Force] [-WhatIf] [-Confirm] [<CommonParameters>]
Unregister-Event [-SubscriptionId] <Int32> [-Force] [-WhatIf] [-Confirm] [<CommonParameters>]
Update-FormatData [[-AppendPath] <String[]>] [-PrependPath <String[]>] [-WhatIf] [-Confirm] [<CommonParameters>]
Update-List [-Add <Object[]>] [-Remove <Object[]>] [-InputObject <PSObject>] [[-Property] <String>] [<CommonParameters>]
Update-List -Replace <Object[]> [-InputObject <PSObject>] [[-Property] <String>] [<CommonParameters>]
Update-TypeData [[-AppendPath] <String[]>] [-PrependPath <String[]>] [-WhatIf] [-Confirm] [<CommonParameters>]
Update-TypeData [-MemberType <PSMemberTypes>] [-MemberName <String>] [-Value <Object>] [-SecondValue <Object>] [-TypeConverter <Type>] [-TypeAdapter <Type>] [-SerializationMethod <String>] [-TargetTypeForDeserialization <Type>] [-SerializationDepth <Int32>] [-DefaultDisplayProperty <String>] [-InheritPropertySerializationSet <Nullable`1>] [-StringSerializationSource <String>] [-DefaultDisplayPropertySet <String[]>] [-DefaultKeyPropertySet <String[]>] [-PropertySerializationSet <String[]>] -TypeName <String> [-Force] [-WhatIf] [-Confirm] [<CommonParameters>]
Update-TypeData [-Force] [-TypeData] <TypeData[]> [-WhatIf] [-Confirm] [<CommonParameters>]
Wait-Debugger []
Wait-Event [[-SourceIdentifier] <String>] [-Timeout <Int32>] [<CommonParameters>]
Write-Debug [-Message] <String> [<CommonParameters>]
Write-Error [-Message] <String> [-Category <ErrorCategory>] [-ErrorId <String>] [-TargetObject <Object>] [-RecommendedAction <String>] [-CategoryActivity <String>] [-CategoryReason <String>] [-CategoryTargetName <String>] [-CategoryTargetType <String>] [<CommonParameters>]
Write-Error [-Exception] <Exception> [[-Message] <String>] [-Category <ErrorCategory>] [-ErrorId <String>] [-TargetObject <Object>] [-RecommendedAction <String>] [-CategoryActivity <String>] [-CategoryReason <String>] [-CategoryTargetName <String>] [-CategoryTargetType <String>] [<CommonParameters>]
Write-Error [-ErrorRecord] <ErrorRecord> [-RecommendedAction <String>] [-CategoryActivity <String>] [-CategoryReason <String>] [-CategoryTargetName <String>] [-CategoryTargetType <String>] [<CommonParameters>]
Write-Host [[-Object] <Object>] [-NoNewline] [-Separator <Object>] [-ForegroundColor <ConsoleColor>] [-BackgroundColor <ConsoleColor>] [<CommonParameters>]
Write-Information [-MessageData] <Object> [[-Tags] <String[]>] [<CommonParameters>]
Write-Output [-InputObject] <PSObject[]> [-NoEnumerate] [<CommonParameters>]
Write-Progress [-Activity] <String> [[-Status] <String>] [[-Id] <Int32>] [-PercentComplete <Int32>] [-SecondsRemaining <Int32>] [-CurrentOperation <String>] [-ParentId <Int32>] [-Completed] [-SourceId <Int32>] [<CommonParameters>]
Write-Verbose [-Message] <String> [<CommonParameters>]
Write-Warning [-Message] <String> [<CommonParameters>]
}

{ # Microsoft.WSMan.Management
Connect-WSMan [-ApplicationName <String>] [[-ComputerName] <String>] [-OptionSet <Hashtable>] [-Port <Int32>] [-SessionOption <SessionOption>] [-UseSSL] [-Credential <PSCredential>] [-Authentication <AuthenticationMechanism>] [-CertificateThumbprint <String>] [<CommonParameters>]
Connect-WSMan [-ConnectionURI <Uri>] [-OptionSet <Hashtable>] [-Port <Int32>] [-SessionOption <SessionOption>] [-Credential <PSCredential>] [-Authentication <AuthenticationMechanism>] [-CertificateThumbprint <String>] [<CommonParameters>]
Disable-WSManCredSSP [-Role] <String> [<CommonParameters>]
Disconnect-WSMan [[-ComputerName] <String>] [<CommonParameters>]
Enable-WSManCredSSP [[-DelegateComputer] <String[]>] [-Force] [-Role] <String> [<CommonParameters>]
Get-WSManCredSSP []
Get-WSManInstance [-ApplicationName <String>] [-ComputerName <String>] [-ConnectionURI <Uri>] [-Dialect <Uri>] [-Fragment <String>] [-OptionSet <Hashtable>] [-Port <Int32>] [-ResourceURI] <Uri> [-SelectorSet <Hashtable>] [-SessionOption <SessionOption>] [-UseSSL] [-Credential <PSCredential>] [-Authentication <AuthenticationMechanism>] [-CertificateThumbprint <String>] [<CommonParameters>]
Get-WSManInstance [-ApplicationName <String>] [-BasePropertiesOnly] [-ComputerName <String>] [-ConnectionURI <Uri>] [-Dialect <Uri>] [-Enumerate] [-Filter <String>] [-OptionSet <Hashtable>] [-Port <Int32>] [-Associations] [-ResourceURI] <Uri> [-ReturnType <String>] [-SessionOption <SessionOption>] [-Shallow] [-UseSSL] [-Credential <PSCredential>] [-Authentication <AuthenticationMechanism>] [-CertificateThumbprint <String>] [<CommonParameters>]
Invoke-WSManAction [-Action] <String> [-ConnectionURI <Uri>] [-FilePath <String>] [-OptionSet <Hashtable>] [[-SelectorSet] <Hashtable>] [-SessionOption <SessionOption>] [-ValueSet <Hashtable>] [-ResourceURI] <Uri> [-Credential <PSCredential>] [-Authentication <AuthenticationMechanism>] [-CertificateThumbprint <String>] [<CommonParameters>]
Invoke-WSManAction [-Action] <String> [-ApplicationName <String>] [-ComputerName <String>] [-FilePath <String>] [-OptionSet <Hashtable>] [-Port <Int32>] [[-SelectorSet] <Hashtable>] [-SessionOption <SessionOption>] [-UseSSL] [-ValueSet <Hashtable>] [-ResourceURI] <Uri> [-Credential <PSCredential>] [-Authentication <AuthenticationMechanism>] [-CertificateThumbprint <String>] [<CommonParameters>]
New-WSManInstance [-ApplicationName <String>] [-ComputerName <String>] [-FilePath <String>] [-OptionSet <Hashtable>] [-Port <Int32>] [-ResourceURI] <Uri> [-SelectorSet] <Hashtable> [-SessionOption <SessionOption>] [-UseSSL] [-ValueSet <Hashtable>] [-Credential <PSCredential>] [-Authentication <AuthenticationMechanism>] [-CertificateThumbprint <String>] [<CommonParameters>]
New-WSManInstance [-ConnectionURI <Uri>] [-FilePath <String>] [-OptionSet <Hashtable>] [-ResourceURI] <Uri> [-SelectorSet] <Hashtable> [-SessionOption <SessionOption>] [-ValueSet <Hashtable>] [-Credential <PSCredential>] [-Authentication <AuthenticationMechanism>] [-CertificateThumbprint <String>] [<CommonParameters>]
New-WSManSessionOption [-ProxyAccessType <ProxyAccessType>] [-ProxyAuthentication <ProxyAuthentication>] [-ProxyCredential <PSCredential>] [-SkipCACheck] [-SkipCNCheck] [-SkipRevocationCheck] [-SPNPort <Int32>] [-OperationTimeout <Int32>] [-NoEncryption] [-UseUTF16] [<CommonParameters>]
Remove-WSManInstance [-ApplicationName <String>] [-ComputerName <String>] [-OptionSet <Hashtable>] [-Port <Int32>] [-ResourceURI] <Uri> [-SelectorSet] <Hashtable> [-SessionOption <SessionOption>] [-UseSSL] [-Credential <PSCredential>] [-Authentication <AuthenticationMechanism>] [-CertificateThumbprint <String>] [<CommonParameters>]
Remove-WSManInstance [-ConnectionURI <Uri>] [-OptionSet <Hashtable>] [-ResourceURI] <Uri> [-SelectorSet] <Hashtable> [-SessionOption <SessionOption>] [-Credential <PSCredential>] [-Authentication <AuthenticationMechanism>] [-CertificateThumbprint <String>] [<CommonParameters>]
Set-WSManInstance [-ApplicationName <String>] [-ComputerName <String>] [-Dialect <Uri>] [-FilePath <String>] [-Fragment <String>] [-OptionSet <Hashtable>] [-Port <Int32>] [-ResourceURI] <Uri> [[-SelectorSet] <Hashtable>] [-SessionOption <SessionOption>] [-UseSSL] [-ValueSet <Hashtable>] [-Credential <PSCredential>] [-Authentication <AuthenticationMechanism>] [-CertificateThumbprint <String>] [<CommonParameters>]
Set-WSManInstance [-ConnectionURI <Uri>] [-Dialect <Uri>] [-FilePath <String>] [-Fragment <String>] [-OptionSet <Hashtable>] [-ResourceURI] <Uri> [[-SelectorSet] <Hashtable>] [-SessionOption <SessionOption>] [-ValueSet <Hashtable>] [-Credential <PSCredential>] [-Authentication <AuthenticationMechanism>] [-CertificateThumbprint <String>] [<CommonParameters>]
Set-WSManQuickConfig [-UseSSL] [-Force] [-SkipNetworkProfileCheck] [<CommonParameters>]
Test-WSMan [[-ComputerName] <String>] [-Authentication <AuthenticationMechanism>] [-Port <Int32>] [-UseSSL] [-ApplicationName <String>] [-Credential <PSCredential>] [-CertificateThumbprint <String>] [<CommonParameters>]
}

{ # PackageManagement
Find-Package [-IncludeDependencies] [-AllVersions] [-Source <String[]>] [-Credential <PSCredential>] [-Proxy <Uri>] [-ProxyCredential <PSCredential>] [[-Name] <String[]>] [-RequiredVersion <String>] [-MinimumVersion <String>] [-MaximumVersion <String>] [-Force] [-ForceBootstrap] [-ProviderName <String[]>] [-ConfigFile <String>] [-SkipValidate] [-Headers <String[]>] [-FilterOnTag <String[]>] [-Contains <String>] [-AllowPrereleaseVersions] [<CommonParameters>]
Find-Package [-IncludeDependencies] [-AllVersions] [-Source <String[]>] [-Credential <PSCredential>] [-Proxy <Uri>] [-ProxyCredential <PSCredential>] [[-Name] <String[]>] [-RequiredVersion <String>] [-MinimumVersion <String>] [-MaximumVersion <String>] [-Force] [-ForceBootstrap] [-ProviderName <String[]>] [-AllowPrereleaseVersions] [-PackageManagementProvider <String>] [-PublishLocation <String>] [-ScriptSourceLocation <String>] [-ScriptPublishLocation <String>] [-Type <String>] [-Filter <String>] [-Tag <String[]>] [-Includes <String[]>] [-DscResource <String[]>] [-RoleCapability <String[]>] [-Command <String[]>] [-AcceptLicense] [<CommonParameters>]
Find-PackageProvider [[-Name] <String[]>] [-AllVersions] [-Source <String[]>] [-IncludeDependencies] [-Credential <PSCredential>] [-Proxy <Uri>] [-ProxyCredential <PSCredential>] [-RequiredVersion <String>] [-MinimumVersion <String>] [-MaximumVersion <String>] [-Force] [-ForceBootstrap] [<CommonParameters>]
Get-Package [[-Name] <String[]>] [-RequiredVersion <String>] [-MinimumVersion <String>] [-MaximumVersion <String>] [-AllVersions] [-Force] [-ForceBootstrap] [-ProviderName <String[]>] [-Destination <String>] [-ExcludeVersion] [-Scope <String>] [-SkipDependencies] [<CommonParameters>]
Get-Package [[-Name] <String[]>] [-RequiredVersion <String>] [-MinimumVersion <String>] [-MaximumVersion <String>] [-AllVersions] [-Force] [-ForceBootstrap] [-ProviderName <String[]>] [-Scope <String>] [-PackageManagementProvider <String>] [-Type <String>] [-AllowClobber] [-SkipPublisherCheck] [-InstallUpdate] [-NoPathUpdate] [-AllowPrereleaseVersions] [<CommonParameters>]
Get-PackageProvider [[-Name] <String[]>] [-ListAvailable] [-Force] [-ForceBootstrap] [<CommonParameters>]
Get-PackageSource [[-Name] <String>] [-Location <String>] [-Force] [-ForceBootstrap] [-ProviderName <String[]>] [-ConfigFile <String>] [-SkipValidate] [<CommonParameters>]
Get-PackageSource [[-Name] <String>] [-Location <String>] [-Force] [-ForceBootstrap] [-ProviderName <String[]>] [-PackageManagementProvider <String>] [-PublishLocation <String>] [-ScriptSourceLocation <String>] [-ScriptPublishLocation <String>] [<CommonParameters>]
Import-PackageProvider [-Name] <String[]> [-RequiredVersion <String>] [-MinimumVersion <String>] [-MaximumVersion <String>] [-Force] [-ForceBootstrap] [<CommonParameters>]
Install-Package [-Name] <String[]> [-RequiredVersion <String>] [-MinimumVersion <String>] [-MaximumVersion <String>] [-Source <String[]>] [-Credential <PSCredential>] [-Proxy <Uri>] [-ProxyCredential <PSCredential>] [-AllVersions] [-Force] [-ForceBootstrap] [-WhatIf] [-Confirm] [-ProviderName <String[]>] [<CommonParameters>]
Install-Package [-InputObject] <SoftwareIdentity[]> [-Credential <PSCredential>] [-Proxy <Uri>] [-ProxyCredential <PSCredential>] [-AllVersions] [-Force] [-ForceBootstrap] [-WhatIf] [-Confirm] [<CommonParameters>]
Install-Package [-Credential <PSCredential>] [-Proxy <Uri>] [-ProxyCredential <PSCredential>] [-AllVersions] [-Force] [-ForceBootstrap] [-WhatIf] [-Confirm] [-ConfigFile <String>] [-SkipValidate] [-Headers <String[]>] [-FilterOnTag <String[]>] [-Contains <String>] [-AllowPrereleaseVersions] [-Destination <String>] [-ExcludeVersion] [-Scope <String>] [-SkipDependencies] [<CommonParameters>]
Install-Package [-Credential <PSCredential>] [-Proxy <Uri>] [-ProxyCredential <PSCredential>] [-AllVersions] [-Force] [-ForceBootstrap] [-WhatIf] [-Confirm] [-ConfigFile <String>] [-SkipValidate] [-Headers <String[]>] [-FilterOnTag <String[]>] [-Contains <String>] [-AllowPrereleaseVersions] [-Destination <String>] [-ExcludeVersion] [-Scope <String>] [-SkipDependencies] [<CommonParameters>]
Install-Package [-Credential <PSCredential>] [-Proxy <Uri>] [-ProxyCredential <PSCredential>] [-AllVersions] [-Force] [-ForceBootstrap] [-WhatIf] [-Confirm] [-AllowPrereleaseVersions] [-Scope <String>] [-PackageManagementProvider <String>] [-PublishLocation <String>] [-ScriptSourceLocation <String>] [-ScriptPublishLocation <String>] [-Type <String>] [-Filter <String>] [-Tag <String[]>] [-Includes <String[]>] [-DscResource <String[]>] [-RoleCapability <String[]>] [-Command <String[]>] [-AcceptLicense] [-AllowClobber] [-SkipPublisherCheck] [-InstallUpdate] [-NoPathUpdate] [<CommonParameters>]
Install-Package [-Credential <PSCredential>] [-Proxy <Uri>] [-ProxyCredential <PSCredential>] [-AllVersions] [-Force] [-ForceBootstrap] [-WhatIf] [-Confirm] [-AllowPrereleaseVersions] [-Scope <String>] [-PackageManagementProvider <String>] [-PublishLocation <String>] [-ScriptSourceLocation <String>] [-ScriptPublishLocation <String>] [-Type <String>] [-Filter <String>] [-Tag <String[]>] [-Includes <String[]>] [-DscResource <String[]>] [-RoleCapability <String[]>] [-Command <String[]>] [-AcceptLicense] [-AllowClobber] [-SkipPublisherCheck] [-InstallUpdate] [-NoPathUpdate] [<CommonParameters>]
Install-PackageProvider [-Name] <String[]> [-RequiredVersion <String>] [-MinimumVersion <String>] [-MaximumVersion <String>] [-Credential <PSCredential>] [-Scope <String>] [-Source <String[]>] [-Proxy <Uri>] [-ProxyCredential <PSCredential>] [-AllVersions] [-Force] [-ForceBootstrap] [-WhatIf] [-Confirm] [<CommonParameters>]
Install-PackageProvider [-Scope <String>] [-InputObject] <SoftwareIdentity[]> [-Proxy <Uri>] [-ProxyCredential <PSCredential>] [-AllVersions] [-Force] [-ForceBootstrap] [-WhatIf] [-Confirm] [<CommonParameters>]
Register-PackageSource [-Proxy <Uri>] [-ProxyCredential <PSCredential>] [[-Name] <String>] [[-Location] <String>] [-Credential <PSCredential>] [-Trusted] [-Force] [-ForceBootstrap] [-WhatIf] [-Confirm] [-ProviderName <String>] [<CommonParameters>]
Register-PackageSource [-Proxy <Uri>] [-ProxyCredential <PSCredential>] [[-Name] <String>] [[-Location] <String>] [-Credential <PSCredential>] [-Trusted] [-Force] [-ForceBootstrap] [-WhatIf] [-Confirm] [-ConfigFile <String>] [-SkipValidate] [<CommonParameters>]
Register-PackageSource [-Proxy <Uri>] [-ProxyCredential <PSCredential>] [[-Name] <String>] [[-Location] <String>] [-Credential <PSCredential>] [-Trusted] [-Force] [-ForceBootstrap] [-WhatIf] [-Confirm] [-PackageManagementProvider <String>] [-PublishLocation <String>] [-ScriptSourceLocation <String>] [-ScriptPublishLocation <String>] [<CommonParameters>]
Save-Package [-Name] <String[]> [-RequiredVersion <String>] [-MinimumVersion <String>] [-MaximumVersion <String>] [-Source <String[]>] [-Path <String>] [-LiteralPath <String>] [-Credential <PSCredential>] [-Proxy <Uri>] [-ProxyCredential <PSCredential>] [-AllVersions] [-Force] [-ForceBootstrap] [-WhatIf] [-Confirm] [-ProviderName <String[]>] [<CommonParameters>]
Save-Package [-Path <String>] [-LiteralPath <String>] -InputObject <SoftwareIdentity> [-Credential <PSCredential>] [-Proxy <Uri>] [-ProxyCredential <PSCredential>] [-AllVersions] [-Force] [-ForceBootstrap] [-WhatIf] [-Confirm] [<CommonParameters>]
Save-Package [-Path <String>] [-LiteralPath <String>] [-Credential <PSCredential>] [-Proxy <Uri>] [-ProxyCredential <PSCredential>] [-AllVersions] [-Force] [-ForceBootstrap] [-WhatIf] [-Confirm] [-ConfigFile <String>] [-SkipValidate] [-Headers <String[]>] [-FilterOnTag <String[]>] [-Contains <String>] [-AllowPrereleaseVersions] [<CommonParameters>]
Save-Package [-Path <String>] [-LiteralPath <String>] [-Credential <PSCredential>] [-Proxy <Uri>] [-ProxyCredential <PSCredential>] [-AllVersions] [-Force] [-ForceBootstrap] [-WhatIf] [-Confirm] [-ConfigFile <String>] [-SkipValidate] [-Headers <String[]>] [-FilterOnTag <String[]>] [-Contains <String>] [-AllowPrereleaseVersions] [<CommonParameters>]
Save-Package [-Path <String>] [-LiteralPath <String>] [-Credential <PSCredential>] [-Proxy <Uri>] [-ProxyCredential <PSCredential>] [-AllVersions] [-Force] [-ForceBootstrap] [-WhatIf] [-Confirm] [-AllowPrereleaseVersions] [-PackageManagementProvider <String>] [-PublishLocation <String>] [-ScriptSourceLocation <String>] [-ScriptPublishLocation <String>] [-Type <String>] [-Filter <String>] [-Tag <String[]>] [-Includes <String[]>] [-DscResource <String[]>] [-RoleCapability <String[]>] [-Command <String[]>] [-AcceptLicense] [<CommonParameters>]
Save-Package [-Path <String>] [-LiteralPath <String>] [-Credential <PSCredential>] [-Proxy <Uri>] [-ProxyCredential <PSCredential>] [-AllVersions] [-Force] [-ForceBootstrap] [-WhatIf] [-Confirm] [-AllowPrereleaseVersions] [-PackageManagementProvider <String>] [-PublishLocation <String>] [-ScriptSourceLocation <String>] [-ScriptPublishLocation <String>] [-Type <String>] [-Filter <String>] [-Tag <String[]>] [-Includes <String[]>] [-DscResource <String[]>] [-RoleCapability <String[]>] [-Command <String[]>] [-AcceptLicense] [<CommonParameters>]
Set-PackageSource [-Proxy <Uri>] [-ProxyCredential <PSCredential>] [-Credential <PSCredential>] [[-Name] <String>] [-Location <String>] [-NewLocation <String>] [-NewName <String>] [-Trusted] [-Force] [-ForceBootstrap] [-WhatIf] [-Confirm] [-ProviderName <String>] [<CommonParameters>]
Set-PackageSource [-Proxy <Uri>] [-ProxyCredential <PSCredential>] [-Credential <PSCredential>] [-NewLocation <String>] [-NewName <String>] [-Trusted] -InputObject <PackageSource> [-Force] [-ForceBootstrap] [-WhatIf] [-Confirm] [<CommonParameters>]
Set-PackageSource [-Proxy <Uri>] [-ProxyCredential <PSCredential>] [-Credential <PSCredential>] [-NewLocation <String>] [-NewName <String>] [-Trusted] [-Force] [-ForceBootstrap] [-WhatIf] [-Confirm] [-ConfigFile <String>] [-SkipValidate] [<CommonParameters>]
Set-PackageSource [-Proxy <Uri>] [-ProxyCredential <PSCredential>] [-Credential <PSCredential>] [-NewLocation <String>] [-NewName <String>] [-Trusted] [-Force] [-ForceBootstrap] [-WhatIf] [-Confirm] [-ConfigFile <String>] [-SkipValidate] [<CommonParameters>]
Set-PackageSource [-Proxy <Uri>] [-ProxyCredential <PSCredential>] [-Credential <PSCredential>] [-NewLocation <String>] [-NewName <String>] [-Trusted] [-Force] [-ForceBootstrap] [-WhatIf] [-Confirm] [-PackageManagementProvider <String>] [-PublishLocation <String>] [-ScriptSourceLocation <String>] [-ScriptPublishLocation <String>] [<CommonParameters>]
Set-PackageSource [-Proxy <Uri>] [-ProxyCredential <PSCredential>] [-Credential <PSCredential>] [-NewLocation <String>] [-NewName <String>] [-Trusted] [-Force] [-ForceBootstrap] [-WhatIf] [-Confirm] [-PackageManagementProvider <String>] [-PublishLocation <String>] [-ScriptSourceLocation <String>] [-ScriptPublishLocation <String>] [<CommonParameters>]
Uninstall-Package [-InputObject] <SoftwareIdentity[]> [-AllVersions] [-Force] [-ForceBootstrap] [-WhatIf] [-Confirm] [<CommonParameters>]
Uninstall-Package [-Name] <String[]> [-RequiredVersion <String>] [-MinimumVersion <String>] [-MaximumVersion <String>] [-AllVersions] [-Force] [-ForceBootstrap] [-WhatIf] [-Confirm] [-ProviderName <String[]>] [<CommonParameters>]
Uninstall-Package [-AllVersions] [-Force] [-ForceBootstrap] [-WhatIf] [-Confirm] [-Destination <String>] [-ExcludeVersion] [-Scope <String>] [-SkipDependencies] [<CommonParameters>]
Uninstall-Package [-AllVersions] [-Force] [-ForceBootstrap] [-WhatIf] [-Confirm] [-Destination <String>] [-ExcludeVersion] [-Scope <String>] [-SkipDependencies] [<CommonParameters>]
Uninstall-Package [-AllVersions] [-Force] [-ForceBootstrap] [-WhatIf] [-Confirm] [-Scope <String>] [-PackageManagementProvider <String>] [-Type <String>] [-AllowClobber] [-SkipPublisherCheck] [-InstallUpdate] [-NoPathUpdate] [-AllowPrereleaseVersions] [<CommonParameters>]
Uninstall-Package [-AllVersions] [-Force] [-ForceBootstrap] [-WhatIf] [-Confirm] [-Scope <String>] [-PackageManagementProvider <String>] [-Type <String>] [-AllowClobber] [-SkipPublisherCheck] [-InstallUpdate] [-NoPathUpdate] [-AllowPrereleaseVersions] [<CommonParameters>]
Unregister-PackageSource [[-Source] <String>] [-Location <String>] [-Credential <PSCredential>] [-Force] [-ForceBootstrap] [-WhatIf] [-Confirm] [-ProviderName <String>] [<CommonParameters>]
Unregister-PackageSource -InputObject <PackageSource[]> [-Credential <PSCredential>] [-Force] [-ForceBootstrap] [-WhatIf] [-Confirm] [<CommonParameters>]
Unregister-PackageSource [-Credential <PSCredential>] [-Force] [-ForceBootstrap] [-WhatIf] [-Confirm] [-ConfigFile <String>] [-SkipValidate] [<CommonParameters>]
Unregister-PackageSource [-Credential <PSCredential>] [-Force] [-ForceBootstrap] [-WhatIf] [-Confirm] [-ConfigFile <String>] [-SkipValidate] [<CommonParameters>]
Unregister-PackageSource [-Credential <PSCredential>] [-Force] [-ForceBootstrap] [-WhatIf] [-Confirm] [-PackageManagementProvider <String>] [-PublishLocation <String>] [-ScriptSourceLocation <String>] [-ScriptPublishLocation <String>] [<CommonParameters>]
Unregister-PackageSource [-Credential <PSCredential>] [-Force] [-ForceBootstrap] [-WhatIf] [-Confirm] [-PackageManagementProvider <String>] [-PublishLocation <String>] [-ScriptSourceLocation <String>] [-ScriptPublishLocation <String>] [<CommonParameters>]
}

{ # PowerShellGet
Find-Command [[-Name] <String[]>] [-ModuleName <String>] [-MinimumVersion <String>] [-MaximumVersion <String>] [-RequiredVersion <String>] [-AllVersions] [-AllowPrerelease] [-Tag <String[]>] [-Filter <String>] [-Proxy <Uri>] [-ProxyCredential <PSCredential>] [-Repository <String[]>] [<CommonParameters>]
Find-DscResource [[-Name] <String[]>] [-ModuleName <String>] [-MinimumVersion <String>] [-MaximumVersion <String>] [-RequiredVersion <String>] [-AllVersions] [-AllowPrerelease] [-Tag <String[]>] [-Filter <String>] [-Proxy <Uri>] [-ProxyCredential <PSCredential>] [-Repository <String[]>] [<CommonParameters>]
Find-Module [[-Name] <string[]>] [-MinimumVersion <string>] [-MaximumVersion <string>] [-RequiredVersion <string>] [-AllVersions] [-IncludeDependencies] [-Filter <string>] [-Tag <string[]>] [-Includes <string[]>] [-DscResource <string[]>] [-RoleCapability <string[]>] [-Command <string[]>] [-Proxy <uri>] [-ProxyCredential <pscredential>] [-Repository <string[]>] [-Credential <pscredential>] [-AllowPrerelease] [<CommonParameters>]
Find-RoleCapability [[-Name] <String[]>] [-ModuleName <String>] [-MinimumVersion <String>] [-MaximumVersion <String>] [-RequiredVersion <String>] [-AllVersions] [-AllowPrerelease] [-Tag <String[]>] [-Filter <String>] [-Proxy <Uri>] [-ProxyCredential <PSCredential>] [-Repository <String[]>] [<CommonParameters>]
Find-Script [[-Name] <String[]>] [-MinimumVersion <String>] [-MaximumVersion <String>] [-RequiredVersion <String>] [-AllVersions] [-IncludeDependencies] [-Filter <String>] [-Tag <String[]>] [-Includes <String[]>] [-Command <String[]>] [-Proxy <Uri>] [-ProxyCredential <PSCredential>] [-Repository <String[]>] [-Credential <PSCredential>] [-AllowPrerelease] [<CommonParameters>]
Get-InstalledModule [[-Name] <String[]>] [-MinimumVersion <String>] [-RequiredVersion <String>] [-MaximumVersion <String>] [-AllVersions] [-AllowPrerelease] [<CommonParameters>]
Get-InstalledScript [[-Name] <String[]>] [-MinimumVersion <String>] [-RequiredVersion <String>] [-MaximumVersion <String>] [-AllowPrerelease] [<CommonParameters>]
Get-PSRepository [[-Name] <String[]>] [<CommonParameters>]
Install-Module [-Name] <String[]> [-MinimumVersion <String>] [-MaximumVersion <String>] [-RequiredVersion <String>] [-Repository <String[]>] [-Credential <PSCredential>] [-Scope <String>] [-Proxy <Uri>] [-ProxyCredential <PSCredential>] [-AllowClobber] [-SkipPublisherCheck] [-Force] [-AllowPrerelease] [-AcceptLicense] [-PassThru] [-WhatIf] [-Confirm] [<CommonParameters>]
Install-Module [-InputObject] <PSObject[]> [-Credential <PSCredential>] [-Scope <String>] [-Proxy <Uri>] [-ProxyCredential <PSCredential>] [-AllowClobber] [-SkipPublisherCheck] [-Force] [-AcceptLicense] [-PassThru] [-WhatIf] [-Confirm] [<CommonParameters>]
Install-Script [-Name] <String[]> [-MinimumVersion <String>] [-MaximumVersion <String>] [-RequiredVersion <String>] [-Repository <String[]>] [-Scope <String>] [-NoPathUpdate] [-Proxy <Uri>] [-ProxyCredential <PSCredential>] [-Credential <PSCredential>] [-Force] [-AllowPrerelease] [-AcceptLicense] [-PassThru] [-WhatIf] [-Confirm] [<CommonParameters>]
Install-Script [-InputObject] <PSObject[]> [-Scope <String>] [-NoPathUpdate] [-Proxy <Uri>] [-ProxyCredential <PSCredential>] [-Credential <PSCredential>] [-Force] [-AcceptLicense] [-PassThru] [-WhatIf] [-Confirm] [<CommonParameters>]
New-ScriptFileInfo [[-Path] <String>] [-Version <String>] [-Author <String>] -Description <String> [-Guid <Guid>] [-CompanyName <String>] [-Copyright <String>] [-RequiredModules <Object[]>] [-ExternalModuleDependencies <String[]>] [-RequiredScripts <String[]>] [-ExternalScriptDependencies <String[]>] [-Tags <String[]>] [-ProjectUri <Uri>] [-LicenseUri <Uri>] [-IconUri <Uri>] [-ReleaseNotes <String[]>] [-PrivateData <String>] [-PassThru] [-Force] [-WhatIf] [-Confirm] [<CommonParameters>]
Publish-Module -Name <String> [-RequiredVersion <String>] [-NuGetApiKey <String>] [-Repository <String>] [-Credential <PSCredential>] [-FormatVersion <Version>] [-ReleaseNotes <String[]>] [-Tags <String[]>] [-LicenseUri <Uri>] [-IconUri <Uri>] [-ProjectUri <Uri>] [-Exclude <String[]>] [-Force] [-AllowPrerelease] [-SkipAutomaticTags] [-WhatIf] [-Confirm] [<CommonParameters>]
Publish-Module -Path <String> [-NuGetApiKey <String>] [-Repository <String>] [-Credential <PSCredential>] [-FormatVersion <Version>] [-ReleaseNotes <String[]>] [-Tags <String[]>] [-LicenseUri <Uri>] [-IconUri <Uri>] [-ProjectUri <Uri>] [-Force] [-SkipAutomaticTags] [-WhatIf] [-Confirm] [<CommonParameters>]
Publish-Script -Path <String> [-NuGetApiKey <String>] [-Repository <String>] [-Credential <PSCredential>] [-Force] [-WhatIf] [-Confirm] [<CommonParameters>]
Publish-Script -LiteralPath <String> [-NuGetApiKey <String>] [-Repository <String>] [-Credential <PSCredential>] [-Force] [-WhatIf] [-Confirm] [<CommonParameters>]
Register-PSRepository [-Name] <String> [-SourceLocation] <Uri> [-PublishLocation <Uri>] [-ScriptSourceLocation <Uri>] [-ScriptPublishLocation <Uri>] [-Credential <PSCredential>] [-InstallationPolicy <String>] [-Proxy <Uri>] [-ProxyCredential <PSCredential>] [-PackageManagementProvider <String>] [<CommonParameters>]
Register-PSRepository [-Default] [-InstallationPolicy <String>] [-Proxy <Uri>] [-ProxyCredential <PSCredential>] [<CommonParameters>]
Save-Module [-Name] <String[]> [-MinimumVersion <String>] [-MaximumVersion <String>] [-RequiredVersion <String>] [-Repository <String[]>] [-Path] <String> [-Proxy <Uri>] [-ProxyCredential <PSCredential>] [-Credential <PSCredential>] [-Force] [-AllowPrerelease] [-AcceptLicense] [-WhatIf] [-Confirm] [<CommonParameters>]
Save-Module [-Name] <String[]> [-MinimumVersion <String>] [-MaximumVersion <String>] [-RequiredVersion <String>] [-Repository <String[]>] -LiteralPath <String> [-Proxy <Uri>] [-ProxyCredential <PSCredential>] [-Credential <PSCredential>] [-Force] [-AllowPrerelease] [-AcceptLicense] [-WhatIf] [-Confirm] [<CommonParameters>]
Save-Module [-InputObject] <PSObject[]> -LiteralPath <String> [-Proxy <Uri>] [-ProxyCredential <PSCredential>] [-Credential <PSCredential>] [-Force] [-AcceptLicense] [-WhatIf] [-Confirm] [<CommonParameters>]
Save-Module [-InputObject] <PSObject[]> [-Path] <String> [-Proxy <Uri>] [-ProxyCredential <PSCredential>] [-Credential <PSCredential>] [-Force] [-AcceptLicense] [-WhatIf] [-Confirm] [<CommonParameters>]
Save-Script [-Name] <String[]> [-MinimumVersion <String>] [-MaximumVersion <String>] [-RequiredVersion <String>] [-Repository <String[]>] [-Path] <String> [-Proxy <Uri>] [-ProxyCredential <PSCredential>] [-Credential <PSCredential>] [-Force] [-AllowPrerelease] [-AcceptLicense] [-WhatIf] [-Confirm] [<CommonParameters>]
Save-Script [-Name] <String[]> [-MinimumVersion <String>] [-MaximumVersion <String>] [-RequiredVersion <String>] [-Repository <String[]>] -LiteralPath <String> [-Proxy <Uri>] [-ProxyCredential <PSCredential>] [-Credential <PSCredential>] [-Force] [-AllowPrerelease] [-AcceptLicense] [-WhatIf] [-Confirm] [<CommonParameters>]
Save-Script [-InputObject] <PSObject[]> -LiteralPath <String> [-Proxy <Uri>] [-ProxyCredential <PSCredential>] [-Credential <PSCredential>] [-Force] [-AcceptLicense] [-WhatIf] [-Confirm] [<CommonParameters>]
Save-Script [-InputObject] <PSObject[]> [-Path] <String> [-Proxy <Uri>] [-ProxyCredential <PSCredential>] [-Credential <PSCredential>] [-Force] [-AcceptLicense] [-WhatIf] [-Confirm] [<CommonParameters>]
Set-PSRepository [-Name] <String> [[-SourceLocation] <Uri>] [-PublishLocation <Uri>] [-ScriptSourceLocation <Uri>] [-ScriptPublishLocation <Uri>] [-Credential <PSCredential>] [-InstallationPolicy <String>] [-Proxy <Uri>] [-ProxyCredential <PSCredential>] [-PackageManagementProvider <String>] [<CommonParameters>]
Test-ScriptFileInfo [-Path] <String> [<CommonParameters>]
Test-ScriptFileInfo -LiteralPath <String> [<CommonParameters>]
Uninstall-Module [-Name] <String[]> [-MinimumVersion <String>] [-RequiredVersion <String>] [-MaximumVersion <String>] [-AllVersions] [-Force] [-AllowPrerelease] [-WhatIf] [-Confirm] [<CommonParameters>]
Uninstall-Module [-InputObject] <PSObject[]> [-Force] [-WhatIf] [-Confirm] [<CommonParameters>]
Uninstall-Script [-Name] <String[]> [-MinimumVersion <String>] [-RequiredVersion <String>] [-MaximumVersion <String>] [-Force] [-AllowPrerelease] [-WhatIf] [-Confirm] [<CommonParameters>]
Uninstall-Script [-InputObject] <PSObject[]> [-Force] [-WhatIf] [-Confirm] [<CommonParameters>]
Unregister-PSRepository [-Name] <String[]> [<CommonParameters>]
Update-Module [[-Name] <String[]>] [-RequiredVersion <String>] [-MaximumVersion <String>] [-Credential <PSCredential>] [-Scope <String>] [-Proxy <Uri>] [-ProxyCredential <PSCredential>] [-Force] [-AllowPrerelease] [-AcceptLicense] [-PassThru] [-WhatIf] [-Confirm] [<CommonParameters>]
Update-ModuleManifest [-Path] <String> [-NestedModules <Object[]>] [-Guid <Guid>] [-Author <String>] [-CompanyName <String>] [-Copyright <String>] [-RootModule <String>] [-ModuleVersion <Version>] [-Description <String>] [-ProcessorArchitecture <ProcessorArchitecture>] [-CompatiblePSEditions <String[]>] [-PowerShellVersion <Version>] [-ClrVersion <Version>] [-DotNetFrameworkVersion <Version>] [-PowerShellHostName <String>] [-PowerShellHostVersion <Version>] [-RequiredModules <Object[]>] [-TypesToProcess <String[]>] [-FormatsToProcess <String[]>] [-ScriptsToProcess <String[]>] [-RequiredAssemblies <String[]>] [-FileList <String[]>] [-ModuleList <Object[]>] [-FunctionsToExport <String[]>] [-AliasesToExport <String[]>] [-VariablesToExport <String[]>] [-CmdletsToExport <String[]>] [-DscResourcesToExport <String[]>] [-PrivateData <Hashtable>] [-Tags <String[]>] [-ProjectUri <Uri>] [-LicenseUri <Uri>] [-IconUri <Uri>] [-ReleaseNotes <String[]>] [-Prerelease <String>] [-HelpInfoUri <Uri>] [-PassThru] [-DefaultCommandPrefix <String>] [-ExternalModuleDependencies <String[]>] [-PackageManagementProviders <String[]>] [-RequireLicenseAcceptance] [-WhatIf] [-Confirm] [<CommonParameters>]
Update-Script [[-Name] <String[]>] [-RequiredVersion <String>] [-MaximumVersion <String>] [-Proxy <Uri>] [-ProxyCredential <PSCredential>] [-Credential <PSCredential>] [-Force] [-AllowPrerelease] [-AcceptLicense] [-WhatIf] [-Confirm] [<CommonParameters>]
Update-ScriptFileInfo [-Path] <String> [-Version <String>] [-Author <String>] [-Guid <Guid>] [-Description <String>] [-CompanyName <String>] [-Copyright <String>] [-RequiredModules <Object[]>] [-ExternalModuleDependencies <String[]>] [-RequiredScripts <String[]>] [-ExternalScriptDependencies <String[]>] [-Tags <String[]>] [-ProjectUri <Uri>] [-LicenseUri <Uri>] [-IconUri <Uri>] [-ReleaseNotes <String[]>] [-PrivateData <String>] [-PassThru] [-Force] [-WhatIf] [-Confirm] [<CommonParameters>]
Update-ScriptFileInfo [-LiteralPath] <String> [-Version <String>] [-Author <String>] [-Guid <Guid>] [-Description <String>] [-CompanyName <String>] [-Copyright <String>] [-RequiredModules <Object[]>] [-ExternalModuleDependencies <String[]>] [-RequiredScripts <String[]>] [-ExternalScriptDependencies <String[]>] [-Tags <String[]>] [-ProjectUri <Uri>] [-LicenseUri <Uri>] [-IconUri <Uri>] [-ReleaseNotes <String[]>] [-PrivateData <String>] [-PassThru] [-Force] [-WhatIf] [-Confirm] [<CommonParameters>]
}

{ # PSDiagnostics
Disable-PSTrace [-AnalyticOnly] [<CommonParameters>]
Disable-PSWSManCombinedTrace []
Disable-WSManTrace []
Enable-PSTrace [-Force] [-AnalyticOnly] [<CommonParameters>]
Enable-PSWSManCombinedTrace [-DoNotOverwriteExistingTrace] [<CommonParameters>]
Enable-WSManTrace []
Get-LogProperties [-Name] <Object> [<CommonParameters>]
Set-LogProperties [-LogDetails] <LogDetails> [-Force] [<CommonParameters>]
Start-Trace [-SessionName] <String> [[-OutputFilePath] <String>] [[-ProviderFilePath] <String>] [-ETS] [-Format <String>] [-MinBuffers <Int32>] [-MaxBuffers <Int32>] [-BufferSizeInKB <Int32>] [-MaxLogFileSizeInMB <Int32>] [<CommonParameters>]
Stop-Trace [-SessionName] <Object> [-ETS] [<CommonParameters>]
}

{ # PSReadLine
Get-PSReadLineKeyHandler [-Bound] [-Unbound] [<CommonParameters>]
Get-PSReadLineKeyHandler [-Chord] <String[]> [<CommonParameters>]
Get-PSReadLineOption []
PSConsoleHostReadLine
Remove-PSReadLineKeyHandler [-Chord] <String[]> [-ViMode <ViMode>] [<CommonParameters>]
Set-PSReadLineKeyHandler [-ScriptBlock] <ScriptBlock> [-BriefDescription <String>] [-Description <String>] [-Chord] <String[]> [-ViMode <ViMode>] [<CommonParameters>]
Set-PSReadLineKeyHandler [-Chord] <String[]> [-ViMode <ViMode>] [-Function] <String> [<CommonParameters>]
Set-PSReadLineOption [-EditMode <EditMode>] [-ContinuationPrompt <String>] [-HistoryNoDuplicates] [-AddToHistoryHandler <System.Func`2[System.String,System.Object]>] [-CommandValidationHandler <System.Action`1[System.Management.Automation.Language.CommandAst]>] [-HistorySearchCursorMovesToEnd] [-MaximumHistoryCount <Int32>] [-MaximumKillRingCount <Int32>] [-ShowToolTips] [-ExtraPromptLineCount <Int32>] [-DingTone <Int32>] [-DingDuration <Int32>] [-BellStyle <BellStyle>] [-CompletionQueryItems <Int32>] [-WordDelimiters <String>] [-HistorySearchCaseSensitive] [-HistorySaveStyle <HistorySaveStyle>] [-HistorySavePath <String>] [-AnsiEscapeTimeout <Int32>] [-PromptText <String[]>] [-ViModeIndicator <ViModeStyle>] [-ViModeChangeHandler <ScriptBlock>] [-PredictionSource <PredictionSource>] [-PredictionViewStyle <PredictionViewStyle>] [-Colors <Hashtable>] [<CommonParameters>]
}

{ # ThreadJob
Start-ThreadJob [-ScriptBlock] <ScriptBlock> [-Name <String>] [-InitializationScript <ScriptBlock>] [-InputObject <PSObject>] [-ArgumentList <Object[]>] [-ThrottleLimit <Int32>] [-StreamingHost <PSHost>] [<CommonParameters>]
Start-ThreadJob [-FilePath] <String> [-Name <String>] [-InitializationScript <ScriptBlock>] [-InputObject <PSObject>] [-ArgumentList <Object[]>] [-ThrottleLimit <Int32>] [-StreamingHost <PSHost>] [<CommonParameters>]
}

# PowerShell 5.1		=======================================================
{ # CimCmdlets
Export-BinaryMiLog [-InputObject <CimInstance>] [-Path] <String> [<CommonParameters>]
}

{ # Microsoft.PowerShell.Core
Add-PSSnapin [-Name] <String[]> [-PassThru] [<CommonParameters>]
Export-Console [[-Path] <String>] [-Force] [-NoClobber] [-WhatIf] [-Confirm] [<CommonParameters>]
Get-PSSnapin [[-Name] <String[]>] [-Registered] [<CommonParameters>]
Remove-PSSnapin [-Name] <String[]> [-PassThru] [-WhatIf] [-Confirm] [<CommonParameters>]
Resume-Job [-Wait] [-Id] <Int32[]> [-WhatIf] [-Confirm] [<CommonParameters>]
Resume-Job [-Job] <Job[]> [-Wait] [-WhatIf] [-Confirm] [<CommonParameters>]
Resume-Job [-Wait] [-Name] <String[]> [-WhatIf] [-Confirm] [<CommonParameters>]
Resume-Job [-Wait] [-InstanceId] <Guid[]> [-WhatIf] [-Confirm] [<CommonParameters>]
Resume-Job [-Wait] [-State] <JobState> [-WhatIf] [-Confirm] [<CommonParameters>]
Resume-Job [-Wait] [-Filter] <Hashtable> [-WhatIf] [-Confirm] [<CommonParameters>]
Suspend-Job [-Force] [-Wait] [-Id] <Int32[]> [-WhatIf] [-Confirm] [<CommonParameters>]
Suspend-Job [-Job] <Job[]> [-Force] [-Wait] [-WhatIf] [-Confirm] [<CommonParameters>]
Suspend-Job [-Force] [-Wait] [-Name] <String[]> [-WhatIf] [-Confirm] [<CommonParameters>]
Suspend-Job [-Force] [-Wait] [-InstanceId] <Guid[]> [-WhatIf] [-Confirm] [<CommonParameters>]
Suspend-Job [-Force] [-Wait] [-Filter] <Hashtable> [-WhatIf] [-Confirm] [<CommonParameters>]
Suspend-Job [-Force] [-Wait] [-State] <JobState> [-WhatIf] [-Confirm] [<CommonParameters>]
}

{ # Microsoft.PowerShell.Diagnostics
Export-Counter [-Path] <String> [-FileFormat <String>] [-MaxSize <UInt32>] -InputObject <PerformanceCounterSampleSet[]> [-Force] [-Circular] [<CommonParameters>]
Import-Counter [-Path] <String[]> [-StartTime <DateTime>] [-EndTime <DateTime>] [-Counter <String[]>] [-MaxSamples <Int64>] [<CommonParameters>]
Import-Counter [-Path] <String[]> -ListSet <String[]> [<CommonParameters>]
Import-Counter [-Path] <String[]> [-Summary] [<CommonParameters>]
}

{ # Microsoft.PowerShell.LocalAccounts
Add-LocalGroupMember [-Group] <LocalGroup> [-Member] <LocalPrincipal[]> [-WhatIf] [-Confirm] [<CommonParameters>]
Add-LocalGroupMember [-Member] <LocalPrincipal[]> [-Name] <String> [-WhatIf] [-Confirm] [<CommonParameters>]
Add-LocalGroupMember [-Member] <LocalPrincipal[]> [-SID] <SecurityIdentifier> [-WhatIf] [-Confirm] [<CommonParameters>]
Disable-LocalUser [-InputObject] <LocalUser[]> [-WhatIf] [-Confirm] [<CommonParameters>]
Disable-LocalUser [-Name] <String[]> [-WhatIf] [-Confirm] [<CommonParameters>]
Disable-LocalUser [-SID] <SecurityIdentifier[]> [-WhatIf] [-Confirm] [<CommonParameters>]
Enable-LocalUser [-InputObject] <LocalUser[]> [-WhatIf] [-Confirm] [<CommonParameters>]
Enable-LocalUser [-Name] <String[]> [-WhatIf] [-Confirm] [<CommonParameters>]
Enable-LocalUser [-SID] <SecurityIdentifier[]> [-WhatIf] [-Confirm] [<CommonParameters>]
Get-LocalGroup [[-Name] <String[]>] [<CommonParameters>]
Get-LocalGroup [[-SID] <SecurityIdentifier[]>] [<CommonParameters>]
Get-LocalGroupMember [[-Member] <String>] [-Name] <String> [<CommonParameters>]
Get-LocalGroupMember [-Group] <LocalGroup> [[-Member] <String>] [<CommonParameters>]
Get-LocalGroupMember [[-Member] <String>] [-SID] <SecurityIdentifier> [<CommonParameters>]
Get-LocalUser [[-Name] <String[]>] [<CommonParameters>]
Get-LocalUser [[-SID] <SecurityIdentifier[]>] [<CommonParameters>]
New-LocalGroup [-Description <String>] [-Name] <String> [-WhatIf] [-Confirm] [<CommonParameters>]
New-LocalUser [-AccountExpires <DateTime>] [-AccountNeverExpires] [-Description <String>] [-Disabled] [-FullName <String>] [-Name] <String> -Password <SecureString> [-PasswordNeverExpires] [-UserMayNotChangePassword] [-WhatIf] [-Confirm] [<CommonParameters>]
New-LocalUser [-AccountExpires <DateTime>] [-AccountNeverExpires] [-Description <String>] [-Disabled] [-FullName <String>] [-Name] <String> [-NoPassword] [-UserMayNotChangePassword] [-WhatIf] [-Confirm] [<CommonParameters>]
Remove-LocalGroup [-InputObject] <LocalGroup[]> [-WhatIf] [-Confirm] [<CommonParameters>]
Remove-LocalGroup [-Name] <String[]> [-WhatIf] [-Confirm] [<CommonParameters>]
Remove-LocalGroup [-SID] <SecurityIdentifier[]> [-WhatIf] [-Confirm] [<CommonParameters>]
Remove-LocalGroupMember [-Group] <LocalGroup> [-Member] <LocalPrincipal[]> [-WhatIf] [-Confirm] [<CommonParameters>]
Remove-LocalGroupMember [-Member] <LocalPrincipal[]> [-Name] <String> [-WhatIf] [-Confirm] [<CommonParameters>]
Remove-LocalGroupMember [-Member] <LocalPrincipal[]> [-SID] <SecurityIdentifier> [-WhatIf] [-Confirm] [<CommonParameters>]
Remove-LocalUser [-InputObject] <LocalUser[]> [-WhatIf] [-Confirm] [<CommonParameters>]
Remove-LocalUser [-Name] <String[]> [-WhatIf] [-Confirm] [<CommonParameters>]
Remove-LocalUser [-SID] <SecurityIdentifier[]> [-WhatIf] [-Confirm] [<CommonParameters>]
Rename-LocalGroup [-InputObject] <LocalGroup> [-NewName] <String> [-WhatIf] [-Confirm] [<CommonParameters>]
Rename-LocalGroup [-Name] <String> [-NewName] <String> [-WhatIf] [-Confirm] [<CommonParameters>]
Rename-LocalGroup [-NewName] <String> [-SID] <SecurityIdentifier> [-WhatIf] [-Confirm] [<CommonParameters>]
Rename-LocalUser [-InputObject] <LocalUser> [-NewName] <String> [-WhatIf] [-Confirm] [<CommonParameters>]
Rename-LocalUser [-Name] <String> [-NewName] <String> [-WhatIf] [-Confirm] [<CommonParameters>]
Rename-LocalUser [-NewName] <String> [-SID] <SecurityIdentifier> [-WhatIf] [-Confirm] [<CommonParameters>]
Set-LocalGroup -Description <String> [-InputObject] <LocalGroup> [-WhatIf] [-Confirm] [<CommonParameters>]
Set-LocalGroup -Description <String> [-Name] <String> [-WhatIf] [-Confirm] [<CommonParameters>]
Set-LocalGroup -Description <String> [-SID] <SecurityIdentifier> [-WhatIf] [-Confirm] [<CommonParameters>]
Set-LocalUser [-AccountExpires <DateTime>] [-AccountNeverExpires] [-Description <String>] [-FullName <String>] [-Name] <String> [-Password <SecureString>] [-PasswordNeverExpires <Boolean>] [-UserMayChangePassword <Boolean>] [-WhatIf] [-Confirm] [<CommonParameters>]
Set-LocalUser [-AccountExpires <DateTime>] [-AccountNeverExpires] [-Description <String>] [-FullName <String>] [-InputObject] <LocalUser> [-Password <SecureString>] [-PasswordNeverExpires <Boolean>] [-UserMayChangePassword <Boolean>] [-WhatIf] [-Confirm] [<CommonParameters>]
Set-LocalUser [-AccountExpires <DateTime>] [-AccountNeverExpires] [-Description <String>] [-FullName <String>] [-Password <SecureString>] [-PasswordNeverExpires <Boolean>] [-SID] <SecurityIdentifier> [-UserMayChangePassword <Boolean>] [-WhatIf] [-Confirm] [<CommonParameters>]
}

{ # Microsoft.PowerShell.Management
Add-Computer [-ComputerName <String[]>] [-LocalCredential <PSCredential>] [-UnjoinDomainCredential <PSCredential>] -Credential <PSCredential> [-DomainName] <String> [-OUPath <String>] [-Server <String>] [-Unsecure] [-Options <JoinOptions>] [-Restart] [-PassThru] [-NewName <String>] [-Force] [-WhatIf] [-Confirm] [<CommonParameters>]
Add-Computer [-ComputerName <String[]>] [-LocalCredential <PSCredential>] [-Credential <PSCredential>] [-WorkgroupName] <String> [-Restart] [-PassThru] [-NewName <String>] [-Force] [-WhatIf] [-Confirm] [<CommonParameters>]
Checkpoint-Computer [-Description] <String> [[-RestorePointType] <String>] [<CommonParameters>]
Clear-EventLog [-LogName] <String[]> [[-ComputerName] <String[]>] [-WhatIf] [-Confirm] [<CommonParameters>]
Complete-Transaction [-WhatIf] [-Confirm] [<CommonParameters>]
Disable-ComputerRestore [-Drive] <String[]> [-WhatIf] [-Confirm] [<CommonParameters>]
Enable-ComputerRestore [-Drive] <String[]> [-WhatIf] [-Confirm] [<CommonParameters>]
Get-ComputerRestorePoint [[-RestorePoint] <Int32[]>] [<CommonParameters>]
Get-ComputerRestorePoint -LastStatus [<CommonParameters>]
Get-ControlPanelItem [[-Name] <String[]>] [-Category <String[]>] [<CommonParameters>]
Get-ControlPanelItem -CanonicalName <String[]> [-Category <String[]>] [<CommonParameters>]
Get-EventLog [-LogName] <String> [-ComputerName <String[]>] [-Newest <Int32>] [-After <DateTime>] [-Before <DateTime>] [-UserName <String[]>] [[-InstanceId] <Int64[]>] [-Index <Int32[]>] [-EntryType <String[]>] [-Source <String[]>] [-Message <String>] [-AsBaseObject] [<CommonParameters>]
Get-EventLog [-ComputerName <String[]>] [-List] [-AsString] [<CommonParameters>]
Get-Transaction []
Get-WmiObject [-Class] <String> [[-Property] <String[]>] [-Filter <String>] [-Amended] [-DirectRead] [-AsJob] [-Impersonation <ImpersonationLevel>] [-Authentication <AuthenticationLevel>] [-Locale <String>] [-EnableAllPrivileges] [-Authority <String>] [-Credential <PSCredential>] [-ThrottleLimit <Int32>] [-ComputerName <String[]>] [-Namespace <String>] [<CommonParameters>]
Get-WmiObject [[-Class] <String>] [-Recurse] [-Amended] [-List] [-AsJob] [-Impersonation <ImpersonationLevel>] [-Authentication <AuthenticationLevel>] [-Locale <String>] [-EnableAllPrivileges] [-Authority <String>] [-Credential <PSCredential>] [-ThrottleLimit <Int32>] [-ComputerName <String[]>] [-Namespace <String>] [<CommonParameters>]
Get-WmiObject [-Amended] [-DirectRead] -Query <String> [-AsJob] [-Impersonation <ImpersonationLevel>] [-Authentication <AuthenticationLevel>] [-Locale <String>] [-EnableAllPrivileges] [-Authority <String>] [-Credential <PSCredential>] [-ThrottleLimit <Int32>] [-ComputerName <String[]>] [-Namespace <String>] [<CommonParameters>]
Get-WmiObject [-Amended] [-AsJob] [-Impersonation <ImpersonationLevel>] [-Authentication <AuthenticationLevel>] [-Locale <String>] [-EnableAllPrivileges] [-Authority <String>] [-Credential <PSCredential>] [-ThrottleLimit <Int32>] [-ComputerName <String[]>] [-Namespace <String>] [<CommonParameters>]
Get-WmiObject [-Amended] [-AsJob] [-Impersonation <ImpersonationLevel>] [-Authentication <AuthenticationLevel>] [-Locale <String>] [-EnableAllPrivileges] [-Authority <String>] [-Credential <PSCredential>] [-ThrottleLimit <Int32>] [-ComputerName <String[]>] [-Namespace <String>] [<CommonParameters>]
Invoke-WmiMethod [-Class] <String> [-Name] <String> [-ArgumentList <Object[]>] [-AsJob] [-Impersonation <ImpersonationLevel>] [-Authentication <AuthenticationLevel>] [-Locale <String>] [-EnableAllPrivileges] [-Authority <String>] [-Credential <PSCredential>] [-ThrottleLimit <Int32>] [-ComputerName <String[]>] [-Namespace <String>] [-WhatIf] [-Confirm] [<CommonParameters>]
Invoke-WmiMethod -InputObject <ManagementObject> [-Name] <String> [-ArgumentList <Object[]>] [-AsJob] [-ThrottleLimit <Int32>] [-WhatIf] [-Confirm] [<CommonParameters>]
Invoke-WmiMethod -Path <String> [-Name] <String> [-ArgumentList <Object[]>] [-AsJob] [-Impersonation <ImpersonationLevel>] [-Authentication <AuthenticationLevel>] [-Locale <String>] [-EnableAllPrivileges] [-Authority <String>] [-Credential <PSCredential>] [-ThrottleLimit <Int32>] [-ComputerName <String[]>] [-Namespace <String>] [-WhatIf] [-Confirm] [<CommonParameters>]
Invoke-WmiMethod [-Name] <String> [-AsJob] [-Impersonation <ImpersonationLevel>] [-Authentication <AuthenticationLevel>] [-Locale <String>] [-EnableAllPrivileges] [-Authority <String>] [-Credential <PSCredential>] [-ThrottleLimit <Int32>] [-ComputerName <String[]>] [-Namespace <String>] [-WhatIf] [-Confirm] [<CommonParameters>]
Invoke-WmiMethod [-Name] <String> [-AsJob] [-Impersonation <ImpersonationLevel>] [-Authentication <AuthenticationLevel>] [-Locale <String>] [-EnableAllPrivileges] [-Authority <String>] [-Credential <PSCredential>] [-ThrottleLimit <Int32>] [-ComputerName <String[]>] [-Namespace <String>] [-WhatIf] [-Confirm] [<CommonParameters>]
Invoke-WmiMethod [-Name] <String> [-AsJob] [-Impersonation <ImpersonationLevel>] [-Authentication <AuthenticationLevel>] [-Locale <String>] [-EnableAllPrivileges] [-Authority <String>] [-Credential <PSCredential>] [-ThrottleLimit <Int32>] [-ComputerName <String[]>] [-Namespace <String>] [-WhatIf] [-Confirm] [<CommonParameters>]
Limit-EventLog [-LogName] <String[]> [-ComputerName <String[]>] [-RetentionDays <Int32>] [-OverflowAction <OverflowAction>] [-MaximumSize <Int64>] [-WhatIf] [-Confirm] [<CommonParameters>]
New-EventLog [-LogName] <string> [-Source] <string[]> [[-ComputerName] <string[]>] [-CategoryResourceFile <string>] [-MessageResourceFile <string>] [-ParameterResourceFile <string>] [<CommonParameters>]
New-WebServiceProxy [-Uri] <Uri> [[-Class] <String>] [[-Namespace] <String>] [<CommonParameters>]
New-WebServiceProxy [-Uri] <Uri> [[-Class] <String>] [[-Namespace] <String>] [-Credential <PSCredential>] [<CommonParameters>]
New-WebServiceProxy [-Uri] <Uri> [[-Class] <String>] [[-Namespace] <String>] [-UseDefaultCredential] [<CommonParameters>]
Register-WmiEvent [-Namespace <String>] [-Credential <PSCredential>] [-ComputerName <String>] [-Class] <String> [-Timeout <Int64>] [[-SourceIdentifier] <String>] [[-Action] <ScriptBlock>] [-MessageData <PSObject>] [-SupportEvent] [-Forward] [-MaxTriggerCount <Int32>] [<CommonParameters>]
Register-WmiEvent [-Namespace <String>] [-Credential <PSCredential>] [-ComputerName <String>] [-Query] <String> [-Timeout <Int64>] [[-SourceIdentifier] <String>] [[-Action] <ScriptBlock>] [-MessageData <PSObject>] [-SupportEvent] [-Forward] [-MaxTriggerCount <Int32>] [<CommonParameters>]
Remove-Computer [[-UnjoinDomainCredential] <PSCredential>] [-Restart] [-Force] [-PassThru] [-WorkgroupName <String>] [-WhatIf] [-Confirm] [<CommonParameters>]
Remove-Computer -UnjoinDomainCredential <PSCredential> [-LocalCredential <PSCredential>] [-Restart] [-ComputerName <String[]>] [-Force] [-PassThru] [-WorkgroupName <String>] [-WhatIf] [-Confirm] [<CommonParameters>]
Remove-EventLog [[-ComputerName] <String[]>] [-LogName] <String[]> [-WhatIf] [-Confirm] [<CommonParameters>]
Remove-EventLog [[-ComputerName] <String[]>] [-Source <String[]>] [-WhatIf] [-Confirm] [<CommonParameters>]
Remove-WmiObject [-Class] <String> [-AsJob] [-Impersonation <ImpersonationLevel>] [-Authentication <AuthenticationLevel>] [-Locale <String>] [-EnableAllPrivileges] [-Authority <String>] [-Credential <PSCredential>] [-ThrottleLimit <Int32>] [-ComputerName <String[]>] [-Namespace <String>] [-WhatIf] [-Confirm] [<CommonParameters>]
Remove-WmiObject -InputObject <ManagementObject> [-AsJob] [-ThrottleLimit <Int32>] [-WhatIf] [-Confirm] [<CommonParameters>]
Remove-WmiObject -Path <String> [-AsJob] [-Impersonation <ImpersonationLevel>] [-Authentication <AuthenticationLevel>] [-Locale <String>] [-EnableAllPrivileges] [-Authority <String>] [-Credential <PSCredential>] [-ThrottleLimit <Int32>] [-ComputerName <String[]>] [-Namespace <String>] [-WhatIf] [-Confirm] [<CommonParameters>]
Remove-WmiObject [-AsJob] [-Impersonation <ImpersonationLevel>] [-Authentication <AuthenticationLevel>] [-Locale <String>] [-EnableAllPrivileges] [-Authority <String>] [-Credential <PSCredential>] [-ThrottleLimit <Int32>] [-ComputerName <String[]>] [-Namespace <String>] [-WhatIf] [-Confirm] [<CommonParameters>]
Remove-WmiObject [-AsJob] [-Impersonation <ImpersonationLevel>] [-Authentication <AuthenticationLevel>] [-Locale <String>] [-EnableAllPrivileges] [-Authority <String>] [-Credential <PSCredential>] [-ThrottleLimit <Int32>] [-ComputerName <String[]>] [-Namespace <String>] [-WhatIf] [-Confirm] [<CommonParameters>]
Remove-WmiObject [-AsJob] [-Impersonation <ImpersonationLevel>] [-Authentication <AuthenticationLevel>] [-Locale <String>] [-EnableAllPrivileges] [-Authority <String>] [-Credential <PSCredential>] [-ThrottleLimit <Int32>] [-ComputerName <String[]>] [-Namespace <String>] [-WhatIf] [-Confirm] [<CommonParameters>]
Reset-ComputerMachinePassword [-Server <String>] [-Credential <PSCredential>] [-WhatIf] [-Confirm] [<CommonParameters>]
Restore-Computer [-RestorePoint] <Int32> [-WhatIf] [-Confirm] [<CommonParameters>]
Set-WmiInstance [-Class] <String> [-Arguments <Hashtable>] [-PutType <PutType>] [-AsJob] [-Impersonation <ImpersonationLevel>] [-Authentication <AuthenticationLevel>] [-Locale <String>] [-EnableAllPrivileges] [-Authority <String>] [-Credential <PSCredential>] [-ThrottleLimit <Int32>] [-ComputerName <String[]>] [-Namespace <String>] [-WhatIf] [-Confirm] [<CommonParameters>]
Set-WmiInstance -InputObject <ManagementObject> [-Arguments <Hashtable>] [-PutType <PutType>] [-AsJob] [-ThrottleLimit <Int32>] [-WhatIf] [-Confirm] [<CommonParameters>]
Set-WmiInstance -Path <String> [-Arguments <Hashtable>] [-PutType <PutType>] [-AsJob] [-Impersonation <ImpersonationLevel>] [-Authentication <AuthenticationLevel>] [-Locale <String>] [-EnableAllPrivileges] [-Authority <String>] [-Credential <PSCredential>] [-ThrottleLimit <Int32>] [-ComputerName <String[]>] [-Namespace <String>] [-WhatIf] [-Confirm] [<CommonParameters>]
Set-WmiInstance [-PutType <PutType>] [-AsJob] [-Impersonation <ImpersonationLevel>] [-Authentication <AuthenticationLevel>] [-Locale <String>] [-EnableAllPrivileges] [-Authority <String>] [-Credential <PSCredential>] [-ThrottleLimit <Int32>] [-ComputerName <String[]>] [-Namespace <String>] [-WhatIf] [-Confirm] [<CommonParameters>]
Set-WmiInstance [-PutType <PutType>] [-AsJob] [-Impersonation <ImpersonationLevel>] [-Authentication <AuthenticationLevel>] [-Locale <String>] [-EnableAllPrivileges] [-Authority <String>] [-Credential <PSCredential>] [-ThrottleLimit <Int32>] [-ComputerName <String[]>] [-Namespace <String>] [-WhatIf] [-Confirm] [<CommonParameters>]
Set-WmiInstance [-PutType <PutType>] [-AsJob] [-Impersonation <ImpersonationLevel>] [-Authentication <AuthenticationLevel>] [-Locale <String>] [-EnableAllPrivileges] [-Authority <String>] [-Credential <PSCredential>] [-ThrottleLimit <Int32>] [-ComputerName <String[]>] [-Namespace <String>] [-WhatIf] [-Confirm] [<CommonParameters>]
Show-ControlPanelItem [-Name] <String[]> [<CommonParameters>]
Show-ControlPanelItem -CanonicalName <String[]> [<CommonParameters>]
Show-ControlPanelItem [[-InputObject] <ControlPanelItem[]>] [<CommonParameters>]
Show-EventLog [[-ComputerName] <String>] [<CommonParameters>]
Start-Transaction [-Timeout <Int32>] [-Independent] [-RollbackPreference <RollbackSeverity>] [-WhatIf] [-Confirm] [<CommonParameters>]
Test-ComputerSecureChannel [-Repair] [-Server <String>] [-Credential <PSCredential>] [-WhatIf] [-Confirm] [<CommonParameters>]
Undo-Transaction [-WhatIf] [-Confirm] [<CommonParameters>]
Use-Transaction [-TransactedScript] <ScriptBlock> [-UseTransaction] [<CommonParameters>]
Write-EventLog [-LogName] <String> [-Source] <String> [[-EntryType] <EventLogEntryType>] [-Category <Int16>] [-EventId] <Int32> [-Message] <String> [-RawData <Byte[]>] [-ComputerName <String>] [<CommonParameters>]
}

{ # Microsoft.PowerShell.ODataUtils
Export-ODataEndpointProxy [-Uri] <String> [-OutputModule] <String> [[-MetadataUri] <String>] [[-Credential] <PSCredential>] [[-CreateRequestMethod] <String>] [[-UpdateRequestMethod] <String>] [[-CmdletAdapter] <String>] [[-ResourceNameMapping] <Hashtable>] [-Force] [[-CustomData] <Hashtable>] [-AllowClobber] [-AllowUnsecureConnection] [[-Headers] <Hashtable>] [-WhatIf] [-Confirm] [<CommonParameters>]
}

{ # Microsoft.PowerShell.Operation.Validation
Get-OperationValidation [[-ModuleName] <String[]>] [-TestType <String[]>] [<CommonParameters>]
Invoke-OperationValidation [-TestInfo <PSObject[]>] [-IncludePesterOutput] [-WhatIf] [-Confirm] [<CommonParameters>]
}

{ # Microsoft.PowerShell.Utility
ConvertFrom-String [-Delimiter <String>] [-PropertyNames <String[]>] [-InputObject] <String> [<CommonParameters>]
ConvertFrom-String [-TemplateFile <String[]>] [-TemplateContent <String[]>] [-IncludeExtent] [-UpdateTemplate] [-InputObject] <String> [<CommonParameters>]
Convert-String [-Example <System.Collections.Generic.List`1[System.Management.Automation.PSObject]>] -InputObject <String> [<CommonParameters>]
}

{ # PSScheduledJob
Add-JobTrigger [-Trigger] <ScheduledJobTrigger[]> [-InputObject] <ScheduledJobDefinition[]> [<CommonParameters>]
Add-JobTrigger [-Trigger] <ScheduledJobTrigger[]> [-Id] <Int32[]> [<CommonParameters>]
Add-JobTrigger [-Trigger] <ScheduledJobTrigger[]> [-Name] <String[]> [<CommonParameters>]
Disable-JobTrigger [-InputObject] <ScheduledJobTrigger[]> [-PassThru] [-WhatIf] [-Confirm] [<CommonParameters>]
Disable-ScheduledJob [-InputObject] <ScheduledJobDefinition> [-PassThru] [-WhatIf] [-Confirm] [<CommonParameters>]
Disable-ScheduledJob [-Id] <Int32> [-PassThru] [-WhatIf] [-Confirm] [<CommonParameters>]
Disable-ScheduledJob [-Name] <String> [-PassThru] [-WhatIf] [-Confirm] [<CommonParameters>]
Enable-JobTrigger [-InputObject] <ScheduledJobTrigger[]> [-PassThru] [-WhatIf] [-Confirm] [<CommonParameters>]
Enable-ScheduledJob [-InputObject] <ScheduledJobDefinition> [-PassThru] [-WhatIf] [-Confirm] [<CommonParameters>]
Enable-ScheduledJob [-Id] <Int32> [-PassThru] [-WhatIf] [-Confirm] [<CommonParameters>]
Enable-ScheduledJob [-Name] <String> [-PassThru] [-WhatIf] [-Confirm] [<CommonParameters>]
Get-JobTrigger [[-TriggerId] <Int32[]>] [-InputObject] <ScheduledJobDefinition> [<CommonParameters>]
Get-JobTrigger [[-TriggerId] <Int32[]>] [-Id] <Int32> [<CommonParameters>]
Get-JobTrigger [[-TriggerId] <Int32[]>] [-Name] <String> [<CommonParameters>]
Get-ScheduledJob [[-Id] <Int32[]>] [<CommonParameters>]
Get-ScheduledJob [-Name] <String[]> [<CommonParameters>]
Get-ScheduledJobOption [-InputObject] <ScheduledJobDefinition> [<CommonParameters>]
Get-ScheduledJobOption [-Id] <Int32> [<CommonParameters>]
Get-ScheduledJobOption [-Name] <String> [<CommonParameters>]
New-JobTrigger [-RandomDelay <TimeSpan>] -At <DateTime> [-Once] [-RepetitionInterval <TimeSpan>] [-RepetitionDuration <TimeSpan>] [-RepeatIndefinitely] [<CommonParameters>]
New-JobTrigger [-DaysInterval <Int32>] [-RandomDelay <TimeSpan>] -At <DateTime> [-Daily] [<CommonParameters>]
New-JobTrigger [-WeeksInterval <Int32>] [-RandomDelay <TimeSpan>] -At <DateTime> -DaysOfWeek<DayOfWeek[]> [-Weekly] [<CommonParameters>]
New-JobTrigger [-RandomDelay <TimeSpan>] [-AtStartup] [<CommonParameters>]
New-JobTrigger [-RandomDelay <TimeSpan>] [-User <String>] [-AtLogOn] [<CommonParameters>]
New-ScheduledJobOption [-RunElevated] [-HideInTaskScheduler] [-RestartOnIdleResume] [-MultipleInstancePolicy <TaskMultipleInstancePolicy>] [-DoNotAllowDemandStart] [-RequireNetwork] [-StopIfGoingOffIdle] [-WakeToRun] [-ContinueIfGoingOnBattery] [-StartIfOnBattery] [-IdleTimeout <TimeSpan>] [-IdleDuration <TimeSpan>] [-StartIfIdle] [<CommonParameters>]
Register-ScheduledJob [-ScriptBlock] <ScriptBlock> [-Name] <String> [-Trigger <ScheduledJobTrigger[]>] [-InitializationScript <ScriptBlock>] [-RunAs32] [-Credential <PSCredential>] [-Authentication <AuthenticationMechanism>] [-ScheduledJobOption <ScheduledJobOptions>] [-ArgumentList <Object[]>] [-MaxResultCount <Int32>] [-RunNow] [-RunEvery <TimeSpan>] [-WhatIf] [-Confirm] [<CommonParameters>]
Register-ScheduledJob [-FilePath] <String> [-Name] <String> [-Trigger <ScheduledJobTrigger[]>] [-InitializationScript <ScriptBlock>] [-RunAs32] [-Credential <PSCredential>] [-Authentication <AuthenticationMechanism>] [-ScheduledJobOption <ScheduledJobOptions>] [-ArgumentList <Object[]>] [-MaxResultCount <Int32>] [-RunNow] [-RunEvery <TimeSpan>] [-WhatIf] [-Confirm] [<CommonParameters>]
Remove-JobTrigger [-TriggerId <Int32[]>] [-InputObject] <ScheduledJobDefinition[]> [<CommonParameters>]
Remove-JobTrigger [-TriggerId <Int32[]>] [-Id] <Int32[]> [<CommonParameters>]
Remove-JobTrigger [-TriggerId <Int32[]>] [-Name] <String[]> [<CommonParameters>]
Set-JobTrigger [-InputObject] <ScheduledJobTrigger[]> [-DaysInterval <Int32>] [-WeeksInterval <Int32>] [-RandomDelay <TimeSpan>] [-At <DateTime>] [-User <String>] [-DaysOfWeek <DayOfWeek[]>] [-AtStartup] [-AtLogOn] [-Once] [-RepetitionInterval <TimeSpan>] [-RepetitionDuration <TimeSpan>] [-RepeatIndefinitely] [-Daily] [-Weekly] [-PassThru] [<CommonParameters>]
Set-ScheduledJob [-Name <String>] [-ScriptBlock <ScriptBlock>] [-Trigger <ScheduledJobTrigger[]>] [-InitializationScript <ScriptBlock>] [-RunAs32] [-Credential <PSCredential>] [-Authentication <AuthenticationMechanism>] [-ScheduledJobOption <ScheduledJobOptions>] [-InputObject] <ScheduledJobDefinition> [-MaxResultCount <Int32>] [-PassThru] [-ArgumentList <Object[]>] [-RunNow] [-RunEvery <TimeSpan>] [<CommonParameters>]
Set-ScheduledJob [-Name <String>] [-FilePath <String>] [-Trigger <ScheduledJobTrigger[]>] [-InitializationScript <ScriptBlock>] [-RunAs32] [-Credential <PSCredential>] [-Authentication <AuthenticationMechanism>] [-ScheduledJobOption <ScheduledJobOptions>] [-InputObject] <ScheduledJobDefinition> [-MaxResultCount <Int32>] [-PassThru] [-ArgumentList <Object[]>] [-RunNow] [-RunEvery <TimeSpan>] [<CommonParameters>]
Set-ScheduledJob [-InputObject] <ScheduledJobDefinition> [-ClearExecutionHistory] [-PassThru] [<CommonParameters>]
Set-ScheduledJobOption [-InputObject] <ScheduledJobOptions> [-PassThru] [-RunElevated] [-HideInTaskScheduler] [-RestartOnIdleResume] [-MultipleInstancePolicy <TaskMultipleInstancePolicy>] [-DoNotAllowDemandStart] [-RequireNetwork] [-StopIfGoingOffIdle] [-WakeToRun] [-ContinueIfGoingOnBattery] [-StartIfOnBattery] [-IdleTimeout <TimeSpan>] [-IdleDuration <TimeSpan>] [-StartIfIdle] [<CommonParameters>]
Unregister-ScheduledJob [-InputObject] <ScheduledJobDefinition[]> [-Force] [-WhatIf] [-Confirm] [<CommonParameters>]
Unregister-ScheduledJob [-Id] <Int32[]> [-Force] [-WhatIf] [-Confirm] [<CommonParameters>]
Unregister-ScheduledJob [-Name] <String[]> [-Force] [-WhatIf] [-Confirm] [<CommonParameters>]
}

{ # PSWorkflow
New-PSWorkflowExecutionOption [-PersistencePath <String>] [-MaxPersistenceStoreSizeGB <Int64>] [-PersistWithEncryption] [-MaxRunningWorkflows <Int32>] [-AllowedActivity <String[]>] [-OutOfProcessActivity <String[]>] [-EnableValidation] [-MaxDisconnectedSessions <Int32>] [-MaxConnectedSessions <Int32>] [-MaxSessionsPerWorkflow <Int32>] [-MaxSessionsPerRemoteNode <Int32>] [-MaxActivityProcesses <Int32>] [-ActivityProcessIdleTimeoutSec <Int32>] [-RemoteNodeSessionIdleTimeoutSec <Int32>] [-SessionThrottleLimit <Int32>] [-WorkflowShutdownTimeoutMSec <Int32>] [<CommonParameters>]
New-PSWorkflowSession [[-ComputerName] <String[]>] [-Credential <Object>] [-Name <String[]>] [-Port <Int32>] [-UseSSL] [-ApplicationName <String>] [-ThrottleLimit <Int32>] [-SessionOption <PSSessionOption>] [-Authentication <AuthenticationMechanism>] [-CertificateThumbprint <String>] [-EnableNetworkAccess] [<CommonParameters>]
}

{ # PSWorkflowUtility
Invoke-AsWorkflow [-CommandName <String>] [-Parameter <Hashtable>] [-InputObject <Object>] [<CommonParameters>]
Invoke-AsWorkflow [-Expression <String>] [-InputObject <Object>] [<CommonParameters>]
}

#! alias				=======================================================
# https://docs.microsoft.com/en-us/powershell/module/microsoft.powershell.core/about/about_built-in_functions
help
mkdir
oss
pause
prompt
# Get-Alias
#? # Where-Object
#% # ForEach-Object
ac # Add-Content
cat # Get-Content
cd # Set-Location
chdir # Set-Location
clc # Clear-Content
clear # Clear-Host
clhy # Clear-History
cli # Clear-Item
clp # Clear-ItemProperty
cls # Clear-Host
clv # Clear-Variable
cnsn # Connect-PSSession
compare # Compare-Object
copy # Copy-Item
cp # Copy-Item
cpi # Copy-Item
cpp # Copy-ItemProperty
cvpa # Convert-Path
dbp # Disable-PSBreakpoint
del # Remove-Item
diff # Compare-Object
dir # Get-ChildItem
dnsn # Disconnect-PSSession
ebp # Enable-PSBreakpoint
echo # Write-Output
epal # Export-Alias
epcsv # Export-Csv
erase # Remove-Item
etsn # Enter-PSSession
exsn # Exit-PSSession
fc # Format-Custom
fhx # Format-Hex
fl # Format-List
foreach # ForEach-Object
ft # Format-Table
fw # Format-Wide
gal # Get-Alias
gbp # Get-PSBreakpoint
gc # Get-Content
gci # Get-ChildItem
gcm # Get-Command
gcs # Get-PSCallStack
gdr # Get-PSDrive
gerr # Get-Error
ghy # Get-History
gi # Get-Item
gjb # Get-Job
gl # Get-Location
gm # Get-Member
gmo # Get-Module
gp # Get-ItemProperty
gps # Get-Process
gpv # Get-ItemPropertyValue
group # Group-Object
gsn # Get-PSSession
gsv # Get-Service
gu # Get-Unique
gv # Get-Variable
h # Get-History
history # Get-History
icm # Invoke-Command
iex # Invoke-Expression
ihy # Invoke-History
ii # Invoke-Item
ipal # Import-Alias
ipcsv # Import-Csv
ipmo # Import-Module
irm # Invoke-RestMethod
iwr # Invoke-WebRequest
kill # Stop-Process
ls # Get-ChildItem
man # help
md # mkdir
measure # Measure-Object
mi # Move-Item
mount # New-PSDrive
move # Move-Item
mp # Move-ItemProperty
mv # Move-Item
nal # New-Alias
ndr # New-PSDrive
ni # New-Item
nmo # New-Module
nsn # New-PSSession
nv # New-Variable
ogv # Out-GridView
oh # Out-Host
popd # Pop-Location
ps # Get-Process
pushd # Push-Location
pwd # Get-Location
r # Invoke-History
rbp # Remove-PSBreakpoint
rcjb # Receive-Job
rcsn # Receive-PSSession
rd # Remove-Item
rdr # Remove-PSDrive
ren # Rename-Item
ri # Remove-Item
rjb # Remove-Job
rm # Remove-Item
rmdir # Remove-Item
rmo # Remove-Module
rni # Rename-Item
rnp # Rename-ItemProperty
rp # Remove-ItemProperty
rsn # Remove-PSSession
rv # Remove-Variable
rvpa # Resolve-Path
sajb # Start-Job
sal # Set-Alias
saps # Start-Process
sasv # Start-Service
sbp # Set-PSBreakpoint
select # Select-Object
set # Set-Variable
shcm # Show-Command
si # Set-Item
sl # Set-Location
sleep # Start-Sleep
sls # Select-String
sort # Sort-Object
sp # Set-ItemProperty
spjb # Stop-Job
spps # Stop-Process
spsv # Stop-Service
start # Start-Process
sv # Set-Variable
tee # Tee-Object
type # Get-Content
where # Where-Object
wjb # Wait-Job
write # Write-Output

# 5.1 Get-Alias
asnp # Add-PSSnapin
cfs # ConvertFrom-String
curl # Invoke-WebRequest
epsn # Export-PSSession
gcb # Get-Clipboard
gin # Get-ComputerInfo
gsnp # Get-PSSnapin
gtz # Get-TimeZone
gwmi # Get-WmiObject
ipsn # Import-PSSession
ise # powershell_ise.exe
iwmi # Invoke-WmiMethod
lp # Out-Printer
npssc # New-PSSessionConfigurationFile
rsnp # Remove-PSSnapin
rujb # Resume-Job
rwmi # Remove-WmiObject
sc # Set-Content
scb # Set-Clipboard
stz # Set-TimeZone
sujb # Suspend-Job
swmi # Set-WmiInstance
trcm # Trace-Command
wget # Invoke-WebRequest

#! misc					=======================================================
# variable scope
global
local
private
script
using
workflow

# switch statement
default

# https://docs.microsoft.com/en-us/powershell/scripting/developer/module/how-to-write-a-powershell-script-module
# https://docs.microsoft.com/en-us/powershell/module/microsoft.powershell.core/about/about_tab_expansion
# https://docs.microsoft.com/en-us/powershell/module/microsoft.powershell.core/about/about_comment_based_help
<#
.Component
.Description
.Example
.ExternalHelp
.ForwardHelpCategory
.ForwardHelpTargetName
.Functionality
.Inputs
.Link
.Notes
.Outputs
.Parameter
.RemoteHelpRunspace
.Role
.Synopsis
#>

# https://docs.microsoft.com/en-us/powershell/module/microsoft.powershell.core/about/about_requires
Requires
requires

region
endregion
Region
EndEegion
